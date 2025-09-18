
#include "term.h"
#include <QMap>
#include <stdexcept>

Token Term::eval(const QStringList& row){
    //ColumnResult result;
    Token result;
    double index = 0;

    //literal as is
    if(token.token_type == TokenType::MULT){
        result.token_type = TokenType::STRING;
        result.string_value = row.join(',');
        return result;
    }

    if(token.token_type == TokenType::STRING){
        result.token_type = TokenType::STRING;
        result.string_value = token.string_value;
        return result;
    }
    if(token.token_type == TokenType::NUMBER){
        result.token_type = TokenType::NUMBER;
        result.number_value = token.number_value;
        return result;
    }

    //get column index
    if(token.token_type == TokenType::COLUMNNUMBER){

        index = token.number_value;

    }
    else if(token.token_type == TokenType::COLUMNNAME){
        if(!columns_table.contains(token.string_value.toLower())){
            result.token_type = TokenType::ERROR;
            result.error_msg = "Unknown column name "+token.string_value+" on line "+ QString::number(token.line_number);
            throw std::logic_error(result.error_msg.toStdString());
            //return result;
        }
        index = columns_table[token.string_value];
    }

    if(index < 0 || index > row.length()){
        result.token_type = TokenType::ERROR;
        result.error_msg = "Invalid column index "+QString::number(token.number_value)+" on line "+ QString::number(token.line_number);
        throw std::logic_error(result.error_msg.toStdString());
        //return result;
    }

    //return value at row index
    result.token_type = TokenType::STRING;
    result.string_value = row.at(index);
    return result;
}

Term Term::eval_and_return_term(const QStringList& row)
{
    Term result(eval(row));
    return result;
}

Token Term::eval(const QMap<QString, QStringList>& data_rows)
{
    //ColumnResult result;
    Token result = token;
    double index = 0;

    QStringList row;

    int num_of_rows = data_rows.size();

    if(num_of_rows == 1){
        row = data_rows["$"];
    }

    //literal as is
    if(token.token_type == TokenType::MULT){

        if(num_of_rows == 2){
            std::string error = "Ambigious column "+ token.string_value.toStdString() + " on line ";
            error+=  token.line_number;
            throw std::logic_error(error);
        }

        result.token_type = TokenType::STRING;
        result.string_value = row.join(',');
        return result;
    }

    if(token.token_type == TokenType::STRING){
        result.token_type = TokenType::STRING;
        result.string_value = token.string_value;
        return result;
    }
    if(token.token_type == TokenType::NUMBER){
        result.token_type = TokenType::NUMBER;
        result.number_value = token.number_value;
        return result;
    }

    //get column index
    if(token.token_type == TokenType::COLUMNNUMBER){

        if(num_of_rows == 2){
            std::string error = "Ambigious column "+ token.string_value.toStdString() + " on line ";
            error+=  token.line_number;
            throw std::logic_error(error);
        }

        index = token.number_value;

    }
    else if(token.token_type == TokenType::COLUMNNAME){
        if(!columns_table.contains(token.string_value.toLower())){
            result.token_type = TokenType::ERROR;
            result.error_msg = "Unknown column name "+token.string_value+" on line "+ QString::number(token.line_number);
            throw std::logic_error(result.error_msg.toStdString());
            //return result;
        }

        QString file_name;
        QString column_name;
        QStringList name_parts = token.string_value.split('.'); //columnname in format file.column

        if(name_parts.size() == 2){
            file_name = name_parts[0];
            column_name = name_parts[1];
        }

        if(num_of_rows == 2){
            //QStringList name_parts = token.string_value.split('.'); //columnname in format file.column

            if(name_parts.size() != 2){
                std::string error = "Ambigious column "+ token.string_value.toStdString() + " on line ";
                error+=  token.line_number;
                throw std::logic_error(error);
            }

            //file_name = name_parts[0];
            //column_name = name_parts[1];

            row = data_rows[file_name];

            bool is_number;
            int value = column_name.toInt(&is_number);


            if(column_name == '*'){ // if column name is of the form, file_name.*
                result.token_type = TokenType::STRING;
                result.string_value = row.join(',');
                return result;
            }
            else if(is_number){
                index = value;
            }
            else{
                index = columns_table[token.string_value];
            }
        }
        else if(num_of_rows == 1){
            bool is_number;
            int value = column_name.toInt(&is_number);


            if(column_name == '*'){ // if column name is of the form, file_name.*
                result.token_type = TokenType::STRING;
                result.string_value = row.join(',');
                return result;
            }
            else if(is_number){
                index = value;
            }
            else{
                index = columns_table[token.string_value];
            }
        }
    }
    else if(token.token_type == TokenType::NAME){
        Token tk = token;
        Term t;

        if(token.string_value.contains('.')){ // check if name is of the format, file.* or file.number

            QStringList name_parts = token.string_value.split('.'); //columnname in format file.column
            if(name_parts.size() == 2){
                QString file_name = name_parts[0];
                QString column_name = name_parts[1];

                bool is_number;
                double num = column_name.toInt(&is_number);

                if(data_rows.size() == 1){
                    row = data_rows["$"];
                }
                else if(data_rows.size() > 1){
                    if(data_rows.contains(file_name)){
                        row = data_rows[file_name];
                    }
                    else{
                        std::string error = "Unknown colunm name "+ token.string_value.toStdString() + " on line ";
                        error+=  token.line_number;
                        throw std::logic_error(error);
                    }
                }

                if(column_name == '*'){
                    tk.token_type = TokenType::MULT;
                    t = Term(tk);
                    //QStringList row;

                    return t.eval(row);
                }
                else if(is_number){
                    tk.token_type = TokenType::COLUMNNUMBER;
                    tk.number_value = num;
                    t = Term(tk);
                    //QStringList row = (data_rows.size() == 2)? data_rows[file_name] : data_rows["$"];
                    return t.eval(row);
                }
                else if(symbol_table.contains(token.token_name.toLower())){
                    TokenType type = symbol_table[token.token_name.toLower()];
                    //Token tk = token;
                    tk.token_type = type;

                    if(type == TokenType::NUMBER){
                        double num = numbers_table[token.token_name.toLower()];
                        tk.number_value = num;
                        tk.string_value = QString::number(num);
                    }
                    else if(type == TokenType::STRING){
                        QString str = strings_table[token.token_name.toLower()];
                        tk.string_value = str;
                    }
                    else if(type == TokenType::COLUMNNAME){
                        tk.token_type = TokenType::COLUMNNUMBER;
                        tk.number_value = columns_table[token.token_name.toLower()];
                    }

                    t = Term(tk);
                    return t.eval(row);
                }
                else{
                    std::string error = "Unknown name "+ token.string_value.toStdString() + " on line ";
                    error+=  token.line_number;
                    throw std::logic_error(error);
                }
            }
            else{
                std::string error = "Invalid syntax for column name ("+ token.string_value.toStdString() + ") on line ";
                error+=  token.line_number;
                throw std::logic_error(error);
            }
        }

        if(!symbol_table.contains(token.token_name.toLower())){
            std::string error = "Unknown name "+ token.string_value.toStdString() + " on line ";
            error+=  token.line_number;
            throw std::logic_error(error);
        }

        TokenType type = symbol_table[token.token_name.toLower()];
        //Token tk = token;
        tk.token_type = type;

        if(type == TokenType::NUMBER){
            double num = numbers_table[token.token_name.toLower()];
            tk.number_value = num;
            tk.string_value = QString::number(num);
        }
        else if(type == TokenType::STRING){
            QString str = strings_table[token.token_name.toLower()];
            tk.string_value = str;
        }
        else if(type == TokenType::COLUMNNAME){
            if(data_rows.size() != 1){
                std::string error = "Ambigious column "+ token.string_value.toStdString() + " on line ";
                error+=  token.line_number;
                throw std::logic_error(error);
            }

        }

        row = data_rows["$"];

        t = Term(tk);
        return t.eval(row);
    }

    if(index < 0 || index > row.length()){
        result.token_type = TokenType::ERROR;
        result.error_msg = "Invalid column index "+QString::number(token.number_value)+" on line "+ QString::number(token.line_number);
        throw std::logic_error(result.error_msg.toStdString());
        //return result;
    }

    //return value at row index
    result.token_type = TokenType::STRING;
    result.string_value = row.at(index);
    return result;
}
