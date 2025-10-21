#include "assignstatement.h"
#include <stdexcept>
#include <QMap>
#include <QFileInfo>
#include <regex>

namespace csvquery {

    AssignStatement::AssignStatement(const QList<Token>& tks)
        :
        tokens(tks)
    {
        last_token_pos = tokens.begin();
    }

    void AssignStatement::throw_exception_if_unexpected_end()
    {
        if (last_token_pos == tokens.cend()) {
            --last_token_pos; //get last but one token
            double line_numer = (*last_token_pos).line_number;
            QString str_num = QString::number(line_numer);

            throw std::logic_error("Unexpected end to Import statement on line " + str_num.toStdString());
        }
    }

    Expression AssignStatement::read_expression()
    {
        QList<Term> ts;

        while (last_token_pos != tokens.cend()) {

            if ((last_token_pos->token_type == TokenType::SEMICOLON) || (last_token_pos->token_type == TokenType::END)) {
                break;
            }

            ts.append(Term(*last_token_pos));
            ++last_token_pos;
        }

        return Expression(ts);
    }

    void AssignStatement::process_expression(Expression& rhs)
    {
        QMap<QString, QStringList> dummy;
        dummy["$"] = { "" };

        //qDebug()<<"processing right hand side of assignment...";
        Term t = rhs.eval(dummy);
        //qDebug()<<"done.";
        TokenType variable_type = t.get_token().token_type;

        //qDebug() << "variable name: "<< variable_name;
        //qDebug() << "keywords:" << keywords;

        //check if name is a reserved word
        if ( keywords.contains(variable_name.toLower()) || funcs_table.keys().contains(variable_name.toLower())) {
            QString error = "Invalid assignment to a keyword '";
            error += variable_name;
            error += "' on line ";
            error += QString::number(t.get_line_number());
            throw std::logic_error(error.toStdString());
        }

        //valid variable name must begin with a letter or an underscore and should not contain operators / special characters
        std::regex re("^(?!_+$)[A-Za-z_][A-Za-z0-9_]*$");
        if (!std::regex_match(variable_name.toLower().toStdString(), re)) {
            QString error = "Variable name '";
            error += variable_name;
            error += "' on line ";
            error += QString::number(t.get_line_number());
            error += "' is invalid!";
            throw std::logic_error(error.toStdString());
        }

        // variable is already defined
        if (symbol_table.contains(variable_name.toLower())) {
            TokenType t_type = symbol_table[variable_name.toLower()];

            if (t_type == TokenType::STRING ) {
                strings_table.remove(variable_name.toLower());
            }
            else if(t_type == TokenType::NUMBER) {
                numbers_table.remove(variable_name.toLower());
            }
        }

        symbol_table[variable_name] = variable_type;

        if (variable_type == TokenType::STRING) {
            strings_table[variable_name] = t.get_token().string_value;
        }
        else if (variable_type == TokenType::NUMBER) {
            numbers_table[variable_name] = t.get_token().number_value;
        }

        assignment_value = t;
    }

    void AssignStatement::execute()
    {
        if (last_token_pos->token_type != TokenType::NAME) {
            std::string error = "Invalid syntax on line ";
            error += last_token_pos->line_number;
            error += " token: ";
            error += last_token_pos->to_string().toStdString();

            throw std::logic_error(error);
        }

        variable_name = last_token_pos->string_value.toLower();
        //qDebug()<<"Read variable";

        /*
        qDebug() << "0 variable name: " << variable_name;
        qDebug() << "0 keywords:" << keywords;
        qDebug() << "symbols:" << symbol_table.keys();

        //check if name is a reserved word
        if (keywords.contains(variable_name.toLower()) || funcs_table.keys().contains(variable_name.toLower())) {
            QString error = "0 Invalid assignment to a keyword '";
            error += variable_name;
            error += "' on line ";
            error += QString::number(last_token_pos->line_number);
            throw std::logic_error(error.toStdString());
        }*/

        //next token
        ++last_token_pos;
        throw_exception_if_unexpected_end();

        if (last_token_pos->token_type == TokenType::ASSIGN) {
            //qDebug()<<"Read assigment operator";
            //next token
            ++last_token_pos;
            throw_exception_if_unexpected_end();

            Expression e = read_expression();
            //qDebug()<<"Read expression";
            process_expression(e);
            //qDebug()<<"processed expression";

            symbol_table[variable_name] = assignment_value.get_token().token_type; //TokenType::STRING;
            //strings_table[variable_name] = assignment_value.get_token().string_value;
            //qDebug()<<"saved variable to symbol table 1";

            //foreach(auto s, symbol_table.keys()) {
            //    qDebug() << "Symbol: " << s;
            //}
        }
        else if (last_token_pos->token_type == TokenType::COLON) { // save column alias using imported names
            //next token
            ++last_token_pos;
            throw_exception_if_unexpected_end();
            //qDebug()<<"Read colon";

            if (last_token_pos->token_type != TokenType::IMPORT) {
                std::string error = "Invalid syntax on line ";
                error += last_token_pos->line_number;
                error += " token: ";
                error += last_token_pos->to_string().toStdString();

                throw std::logic_error(error);
            }
            //qDebug()<<"Read annotation";
            QString def_alias = last_token_pos->string_value.toLower();
            if (!import_defs.contains(def_alias)) {
                std::string error = "Unknown import definition alias ";
                error += def_alias.toStdString();
                error += " on line ";
                error += last_token_pos->line_number;

                throw std::logic_error(error);
            }

            QList<QString> column_list = import_defs[def_alias];
            //qDebug()<<"Retrieved annotation column list";

            int index = 0;
            foreach(auto col_name, column_list) {
                QString name = variable_name + '.' + col_name;
                name = name.toLower();
                symbol_table[name] = TokenType::COLUMNNAME;
                columns_table[name] = index;

                commands.push_back(name.toUpper().toStdString()); //add new name for tab completion
                ++index;
            }

            //qDebug()<<"Added annotated column list to symbol table";

            //next token
            ++last_token_pos;
            throw_exception_if_unexpected_end();

            //qDebug()<<"Read assignment";

            if (last_token_pos->token_type != TokenType::ASSIGN) {
                std::string error = "Invalid syntax on line ";
                error += last_token_pos->line_number;
                error += " token: ";
                error += last_token_pos->to_string().toStdString();

                throw std::logic_error(error);
            }

            //next token
            ++last_token_pos;
            throw_exception_if_unexpected_end();

            Expression e = read_expression();
            //qDebug()<<"Read expression 2";
            process_expression(e);
            //qDebug()<<"processed expression 2";

            if (assignment_value.get_token().token_type != TokenType::STRING) {
                std::string error = "Expected a string or for assignment on line ";
                error += last_token_pos->line_number;

                throw std::logic_error(error);
            }

            //qDebug()<<"Read expression 2 value";

            //valid file or path?

            QFileInfo file_info(assignment_value.get_token().string_value);
            if (!file_info.exists() || !file_info.isFile()) {
                std::string error = "Invalid file provided in assignment on line ";
                error += last_token_pos->line_number;

                throw std::logic_error(error);
            }

            //qDebug()<<"Read valid file";


            symbol_table[variable_name] = assignment_value.get_token().token_type;;
            //strings_table[variable_name] = assignment_value.get_token().string_value;
            //qDebug()<<"saved variable to symbol table";
        }
    }

}
