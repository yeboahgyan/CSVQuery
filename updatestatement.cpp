#include "updatestatement.h"
#include <stdexcept>
#include <QFileInfo>

namespace csvquery {

    UpdateStatement::UpdateStatement(const QList<Token>& tks)
        :
        tokens(tks)
    {
        last_token_pos = tokens.begin();
        parse();
    }

    void UpdateStatement::throw_exception_if_unexpected_end()
    {
        if (last_token_pos == tokens.cend()) {
            --last_token_pos; //get last but one token
            double line_numer = (*last_token_pos).line_number;
            QString str_num = QString::number(line_numer);

            throw std::logic_error("Unexpected end to Update statement on line " + str_num.toStdString());
        }
    }

    std::shared_ptr<CSVFile> UpdateStatement::read_file(QIODeviceBase::OpenMode m)
    {
        throw_exception_if_unexpected_end();

        QString f;
        if ((last_token_pos->token_type == TokenType::NAME) || (last_token_pos->token_type == TokenType::STRING)) {
            if (last_token_pos->token_type == TokenType::NAME) { // name
                if (!symbol_table.contains(last_token_pos->string_value.toLower())) {
                    double line_numer = (*last_token_pos).line_number;
                    QString str_num = QString::number(line_numer);

                    throw std::logic_error("Unknown name " + last_token_pos->string_value.toStdString() + " on line " + str_num.toStdString());
                }

                TokenType token_type = symbol_table[last_token_pos->string_value.toLower()];
                if (token_type != TokenType::STRING) {
                    double line_numer = (*last_token_pos).line_number;
                    QString str_num = QString::number(line_numer);

                    throw std::logic_error("Invalid name on line " + str_num.toStdString());
                }



                f = strings_table[last_token_pos->string_value.toLower()];
            }
            else { //String
                f = last_token_pos->string_value.toLower();
            }

            // check if provided string is a valid file
            if (m != QIODevice::WriteOnly) {
                QFileInfo fileInfo(f);
                if (!fileInfo.exists() || !fileInfo.isFile()) {
                    double line_numer = (*last_token_pos).line_number;
                    QString str_num = QString::number(line_numer);

                    throw std::logic_error("Invalid file provided on line " + str_num.toStdString());
                }
            }


            //left_file = std::make_shared<CSVFile>(f);
        }

        CSVFile csv(f, m);
        csv.set_token(*last_token_pos);

        return std::make_shared<CSVFile>(csv);
    }

    Expression UpdateStatement::read_expression()
    {
        QList<Term> ts;

        while (last_token_pos != tokens.cend()) {

            if ((last_token_pos->token_type == TokenType::COMMA) || (last_token_pos->token_type == TokenType::INTO)
                || (last_token_pos->token_type == TokenType::WHERE)
                ) {

                break;
            }

            ts.append(Term(*last_token_pos));
            ++last_token_pos;
        }

        return Expression(ts);
    }

    Token UpdateStatement::read_column()
    {
        throw_exception_if_unexpected_end();

        Token lhs_token;

        QList<TokenType> allowed_lhs = { TokenType::COLUMNNAME, TokenType::COLUMNNUMBER, TokenType::NAME };

        if (allowed_lhs.contains(last_token_pos->token_type)) {

            lhs_token = (*last_token_pos);

            if (last_token_pos->token_type == TokenType::NAME || last_token_pos->token_type == TokenType::COLUMNNAME) {
                QStringList name_parts = last_token_pos->string_value.split('.');

                if (name_parts.size() == 2) {
                    QString num_str = name_parts[1];
                    bool is_number = false;

                    int number = num_str.toInt(&is_number);
                    if (is_number) {
                        lhs_token.number_value = number;
                        lhs_token.string_value = num_str;
                        lhs_token.token_type = TokenType::COLUMNNUMBER;
                    }
                    else if (symbol_table.contains(last_token_pos->string_value)) {
                        lhs_token.number_value = columns_table[last_token_pos->string_value];
                        lhs_token.string_value = QString::number(lhs_token.number_value);
                        lhs_token.token_type = TokenType::COLUMNNUMBER;
                    }
                    else {
                        double line_numer = (*last_token_pos).line_number;
                        QString str_num = QString::number(line_numer);

                        throw std::logic_error("Unknown column name on line " + str_num.toStdString());
                    }
                }
                else { //column names are of the format alias.name or alias.number
                    double line_numer = (*last_token_pos).line_number;
                    QString str_num = QString::number(line_numer);

                    throw std::logic_error("Unknown column name on line " + str_num.toStdString());
                }

            }

        }
        return lhs_token;
    }

    void UpdateStatement::read_column_update_list()
    {
        //read column to update
        //read assign operator
        //read expression to assign
        while (last_token_pos != tokens.cend()) {

            Token lhs_token = read_column();

            ++last_token_pos; //next token

            throw_exception_if_unexpected_end();

            if (last_token_pos->token_type != TokenType::ASSIGN) {
                double line_numer = (--last_token_pos)->line_number;
                QString str_num = QString::number(line_numer);

                throw std::logic_error("Expected the '=' operator. Invalid syntax in Update statement on line " + str_num.toStdString());
            }

            ++last_token_pos; //next token

            Expression rhs_expr = read_expression();
            column_update_list.append(std::make_pair(lhs_token, rhs_expr));

            if ((last_token_pos->token_type == TokenType::INTO)
                || (last_token_pos->token_type == TokenType::WHERE)
                ) {

                return;
            }

            ++last_token_pos; //next token
        }
    }

    std::shared_ptr<ConditionalExpression> UpdateStatement::read_where_clause()
    {
        QList<Term> cond_terms;

        for (; last_token_pos != tokens.cend(); ++last_token_pos) {
            if (last_token_pos->token_type == TokenType::INTO) {
                break;
            }

            Term t(*last_token_pos);
            //qDebug()<<"where term: "<<last_token_pos->to_string();
            cond_terms.append(t);
        }

        return std::make_shared<ConditionalExpression>(cond_terms);
    }

    void UpdateStatement::parse()
    {
        ++last_token_pos; //move to next token; assuming current token is Update

        left_file = read_file(); // file to update

        ++last_token_pos; //move to next token

        throw_exception_if_unexpected_end();

        //next token should be Set
        if (last_token_pos->token_type != TokenType::SET) {
            double line_numer = (--last_token_pos)->line_number;
            QString str_num = QString::number(line_numer);

            throw std::logic_error("Expected the SET keyword. Invalid syntax in Update statement on line " + str_num.toStdString());
        }

        ++last_token_pos; //move to next token

        throw_exception_if_unexpected_end();

        //read column update list
        read_column_update_list();


        throw_exception_if_unexpected_end();

        //read where clause if any
        if (last_token_pos->token_type == TokenType::WHERE)
        {
            ++last_token_pos; //move to next token
            throw_exception_if_unexpected_end();

            has_where_clause = true;
            where_expr = read_where_clause();
        }

        if (last_token_pos->token_type != TokenType::INTO) {
            double line_numer = (--last_token_pos)->line_number;
            QString str_num = QString::number(line_numer);

            throw std::logic_error("Expected an INTO clasuse. Invalid syntax in Update statement on line " + str_num.toStdString());
        }

        ++last_token_pos; //move to next token
        throw_exception_if_unexpected_end();

        out_file = read_file(QIODevice::WriteOnly);
    }


    void UpdateStatement::execute()
    {
        while (!left_file->end_of_file()) {
            QStringList row = left_file->readRow();

            QMap<QString, QStringList> data_rows;
            data_rows["$"] = row;


            if (has_where_clause) {
                Term where_t = where_expr->eval(data_rows);
                //qDebug()<<"Conditional evaluation done.";
                qDebug() << row << " where clause result:" << where_t.get_token().boolean_value;

                if (where_t.get_token().boolean_value == true) {

                    //columns = compute_columns(data_rows);
                    foreach(auto pair, column_update_list) {
                        Token t = pair.first;
                        Term val_t = pair.second.eval(data_rows);

                        if (t.number_value >= row.length() || t.number_value < 0) {
                            double line_numer = (*last_token_pos).line_number;
                            QString str_num = QString::number(line_numer);

                            throw std::logic_error("Invalid column index in Update statement on line " + str_num.toStdString());
                        }
                        row[t.number_value] = val_t.get_token().string_value;
                    }

                    //write updated data to file
                    out_file->writeLine(row.join(','));
                    ++NUMBER_OF_AFFECTED_ROWS;

                }
                else {
                    //write data unchanged to file
                    out_file->writeLine(row.join(','));
                }
            }
            else { //no where clause
                foreach(auto pair, column_update_list) {
                    Token t = pair.first;
                    Term val_t = pair.second.eval(data_rows);

                    if (t.number_value >= row.length() || t.number_value < 0) {
                        double line_numer = (*last_token_pos).line_number;
                        QString str_num = QString::number(line_numer);

                        throw std::logic_error("Invalid column index in Update statement on line " + str_num.toStdString());
                    }
                    row[t.number_value] = val_t.get_token().string_value;
                }

                out_file->writeLine(row.join(','));
                ++NUMBER_OF_AFFECTED_ROWS;

            }
        }

    }

}