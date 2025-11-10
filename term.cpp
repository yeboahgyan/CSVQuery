
#include "term.h"
#include <QMap>
#include <stdexcept>
#include <QDebug>

namespace csvquery {

    Token Term::eval(const QStringList& row) {
        //ColumnResult result;
        Token result;
        double index = 0;

        //literal as is
        if (token.token_type == TokenType::MULT) {
            result.token_type = TokenType::STRING;
            result.string_value = row.join(',');
            return result;
        }

        if (token.token_type == TokenType::STRING) {
            result.token_type = TokenType::STRING;
            result.string_value = token.string_value;
            return result;
        }
        if (token.token_type == TokenType::NUMBER) {
            result.token_type = TokenType::NUMBER;
            result.number_value = token.number_value;
            result.string_value = QString::number(token.number_value);
            return result;
        }

        //get column index
        if (token.token_type == TokenType::COLUMNNUMBER) {

            index = token.number_value;

        }
        else if (token.token_type == TokenType::COLUMNNAME) {
            if (!columns_table.contains(token.string_value.toLower())) {
                result.token_type = TokenType::ERROR;
                result.error_msg = "Unknown column name " + token.string_value + " on line " + QString::number(token.line_number);
                throw std::logic_error(result.error_msg.toStdString());
                //return result;
            }
            index = columns_table[token.string_value];
        }

        if (index < 0 || index > row.length()) {
            result.token_type = TokenType::ERROR;
            result.error_msg = "Invalid column index " + QString::number(token.number_value) + " on line " + QString::number(token.line_number);
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

        if (num_of_rows == 1) {
            row = data_rows["$"];
        }
        else {
            //check if name is of the format file.number
            QString file_name;
            QString column_name;
            QStringList name_parts = token.string_value.split('.'); //columnname in format file.column

            if (name_parts.size() != 2) {
                result.token_type = TokenType::ERROR;
                result.error_msg = "Unknown column name " + token.string_value + " on line " + QString::number(token.line_number);
                throw std::logic_error(result.error_msg.toStdString());
            }

            file_name = name_parts[0];
            column_name = name_parts[1];

            //qDebug() << "symbol table: " << symbol_table.contains(file_name);

            if (!symbol_table.contains(file_name)) {
                result.token_type = TokenType::ERROR;
                result.error_msg = "Unknown column name " + token.string_value + " on line " + QString::number(token.line_number);
                throw std::logic_error(result.error_msg.toStdString());
            }

            TokenType tk = symbol_table[file_name];
            if (tk != TokenType::STRING) {
                result.token_type = TokenType::ERROR;
                result.error_msg = "Unknown column name " + token.string_value + " on line " + QString::number(token.line_number);
                throw std::logic_error(result.error_msg.toStdString());
            }

            if (!strings_table.contains(file_name)) {
                result.token_type = TokenType::ERROR;
                result.error_msg = "Unknown column name " + token.string_value + " on line " + QString::number(token.line_number);
                throw std::logic_error(result.error_msg.toStdString());
            }

            QString file_path = strings_table[file_name];

            row = data_rows[file_path];
            //qDebug()<<"row size: "<<row.size();


        }

        //literal as is
        if (token.token_type == TokenType::MULT) {

            if (num_of_rows == 2) {
                std::string error = "Ambigious column " + token.string_value.toStdString() + " on line ";
                error += token.line_number;
                throw std::logic_error(error);
            }

            result.token_type = TokenType::STRING;
            result.string_value = row.join(',');
            return result;
        }

        if (token.token_type == TokenType::STRING) {
            result.token_type = TokenType::STRING;
            result.string_value = token.string_value;
            return result;
        }
        if (token.token_type == TokenType::NUMBER) {
            result.token_type = TokenType::NUMBER;
            result.number_value = token.number_value;
            result.string_value = QString::number(token.number_value);
            return result;
        }

        //get column index
        if (token.token_type == TokenType::COLUMNNUMBER) {
            //qDebug()<<"I am here! "<<token.string_value;
            if (num_of_rows == 2) {
                std::string error = "Ambigious column " + token.string_value.toStdString() + " on line ";
                error += token.line_number;
                throw std::logic_error(error);
            }

            index = token.number_value;

        }
        else if (token.token_type == TokenType::COLUMNNAME) {
            if (!columns_table.contains(token.string_value.toLower())) { //not in columns table

                //check if name is of the format file.number
                QString file_name;
                QString column_name;
                QStringList name_parts = token.string_value.split('.'); //columnname in format file.column

                if (name_parts.size() != 2) {
                    result.token_type = TokenType::ERROR;
                    result.error_msg = "Unknown column name " + token.string_value + " on line " + QString::number(token.line_number);
                    throw std::logic_error(result.error_msg.toStdString());
                }

                file_name = name_parts[0];
                column_name = name_parts[1];

                bool is_number;
                int value = column_name.toInt(&is_number);

                /*
                if(num_of_rows == 2){
                    row = data_rows[file_name];
                }*/

                if (column_name == '*') { // if column name is of the form, file_name.*
                    result.token_type = TokenType::STRING;
                    result.string_value = row.join(',');
                    return result;
                }
                else if (is_number) {
                    //qDebug()<<"column index: "<<value;
                    index = value;
                }
            }
            else {
                index = columns_table[token.string_value];
            }

            /*
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
            }*/
        }
        else if (token.token_type == TokenType::NAME) {
            Token tk = token;
            Term t;
            //qDebug()<<"evaluating a NAME Token \n";
            if (token.string_value.contains('.')) { // check if name is of the format, file.* or file.number
                //qDebug()<<"name is "<<token.string_value;

                QStringList name_parts = token.string_value.split('.'); //columnname in format file.column
                if (name_parts.size() == 2) {
                    QString file_name = name_parts[0];
                    QString column_name = name_parts[1];

                    bool is_number;
                    double num = column_name.toInt(&is_number);

                    if (data_rows.size() == 1) {
                        row = data_rows["$"];
                    }
                    else if (data_rows.size() > 1) {
                        //qDebug()<<"data_rows: "<<data_rows;
                        if (!strings_table.contains(file_name)) {
                            std::string error = "Unknown colunm name '" + token.string_value.toStdString() + "' on line ";
                            error += token.line_number;
                            throw std::logic_error(error);
                        }
                        QString file_path = strings_table[file_name];
                        if (data_rows.contains(file_path)) {
                            row = data_rows[file_path];
                        }
                        else {
                            std::string error = "Unknown colunm name '" + token.string_value.toStdString() + "' on line ";
                            error += token.line_number;
                            throw std::logic_error(error);
                        }
                    }

                    if (column_name == '*') {
                        tk.token_type = TokenType::MULT;
                        t = Term(tk);
                        //QStringList row;

                        return t.eval(row);
                    }
                    else if (is_number) {
                        tk.token_type = TokenType::COLUMNNUMBER;
                        tk.number_value = num;
                        t = Term(tk);
                        //QStringList row = (data_rows.size() == 2)? data_rows[file_name] : data_rows["$"];

                        // column index check
                        if ( num < 0 || num >= row.length() ) {
                            QString error = "Invalid index '";
                            error += token.string_value;
                            error += " (";
                            error += QString::number(num);
                            error += ")' on line ";
                            error += QString::number(token.line_number);
                            throw std::logic_error(error.toStdString());
                        }

                        return t.eval(row);
                    }
                    else if (symbol_table.contains(token.string_value.toLower())) {
                        TokenType type = symbol_table[token.string_value.toLower()];
                        //Token tk = token;
                        tk.token_type = type;

                        if (type == TokenType::NUMBER) {
                            double num = numbers_table[token.string_value.toLower()];
                            tk.number_value = num;
                            tk.string_value = QString::number(num);
                        }
                        else if (type == TokenType::STRING) {
                            QString str = strings_table[token.string_value.toLower()];
                            tk.string_value = str;
                        }
                        else if (type == TokenType::COLUMNNAME) {
                            tk.token_type = TokenType::COLUMNNUMBER;
                            tk.number_value = columns_table[token.string_value.toLower()];

                            // column index check
                            if (tk.number_value < 0 || tk.number_value >= row.length()) {
                                QString error = "Invalid index '";
                                error += token.string_value;
                                error += " (";
                                error += QString::number(tk.number_value);
                                error += ")' on line ";
                                error += QString::number(token.line_number);
                                throw std::logic_error(error.toStdString());
                            }
                        }

                        t = Term(tk);
                        return t.eval(row);
                    }
                    else {
                        std::string error = "Unknown name '" + token.string_value.toStdString() + "' on line ";
                        error += token.line_number;
                        throw std::logic_error(error);
                    }
                }
                else {
                    std::string error = "Invalid syntax for column name (" + token.string_value.toStdString() + ") on line ";
                    error += token.line_number;
                    throw std::logic_error(error);
                }
            }

            if (!symbol_table.contains(token.string_value.toLower())) {
                //foreach(auto s, symbol_table.keys()) {
                //    qDebug() <<"Symbol: "<< s;
                //}
                std::string error = "Unknown name '" + token.string_value.toStdString() + "' on line ";
                error += token.line_number;
                throw std::logic_error(error);
            }

            TokenType type = symbol_table[token.string_value.toLower()];
            //Token tk = token;
            tk.token_type = type;

            if (type == TokenType::NUMBER) {
                double num = numbers_table[token.string_value.toLower()];
                tk.number_value = num;
                tk.string_value = QString::number(num);
            }
            else if (type == TokenType::STRING) {
                QString str = strings_table[token.string_value.toLower()];
                tk.string_value = str;
            }
            else if (type == TokenType::COLUMNNAME) {
                if (data_rows.size() != 1) {
                    std::string error = "Ambigious column '" + token.string_value.toStdString() + "' on line ";
                    error += token.line_number;
                    throw std::logic_error(error);
                }

                double number_value = columns_table[token.string_value.toLower()];

                // column index check
                if (number_value < 0 || number_value >= row.length()) {
                    QString error = "Invalid index '";
                    error += token.string_value;
                    error += " (";
                    error += QString::number(number_value);
                    error += ")' on line ";
                    error += QString::number(token.line_number);
                    throw std::logic_error(error.toStdString());
                }

            }

            row = data_rows["$"];

            t = Term(tk);
            return t.eval(row);
        }

        //qDebug()<<"column index2: "<<index <<" row length: "<<row.length();
        if (index < 0 || index > (row.length() -1)) {
            result.token_type = TokenType::ERROR;
            result.error_msg = "Invalid column index '" + QString::number(token.number_value) + "' on line " + QString::number(token.line_number);
            throw std::logic_error(result.error_msg.toStdString());
            //return result;
        }

        //return value at row index
        result.token_type = TokenType::STRING;
        result.string_value = row.at(index);
        return result;
    }

    std::function <Token(const QStringList& row)> Term::compile_eval(const QStringList row) 
    {
        Token result;
        double index = 0;

        //literal as is
        if (token.token_type == TokenType::MULT) {
            //result.token_type = TokenType::STRING;
            //result.string_value = row.join(',');
            //return result;
            auto compiled_func = [token = token](const QStringList& row) {

                Token result = token;
                result.token_type = TokenType::STRING;
                result.string_value = row.join(',');

                return result;
                };
            return compiled_func;
        }

        if (token.token_type == TokenType::STRING) {
            //result.token_type = TokenType::STRING;
            //result.string_value = token.string_value;
            auto compiled_func = [token = token](const QStringList& row) {

                Token result = token;
                result.token_type = TokenType::STRING;
                result.string_value = token.string_value;

                return result;
                };
            return compiled_func;
        }
        if (token.token_type == TokenType::NUMBER) {
            //result.token_type = TokenType::NUMBER;
            //result.number_value = token.number_value;
            //result.string_value = QString::number(token.number_value);
            auto compiled_func = [token = token](const QStringList& row) {

                Token result = token;
                result.token_type = TokenType::NUMBER;
                result.number_value = token.number_value;
                result.string_value = QString::number(token.number_value);

                return result;
                };
            return compiled_func;
        }

        //get column index
        if (token.token_type == TokenType::COLUMNNUMBER) {

            index = token.number_value;

        }
        else if (token.token_type == TokenType::COLUMNNAME) {
            if (!columns_table.contains(token.string_value.toLower())) {
                result.token_type = TokenType::ERROR;
                result.error_msg = "1 Unknown column name " + token.string_value + " on line " + QString::number(token.line_number);
                throw std::logic_error(result.error_msg.toStdString());
                //return result;
            }
            index = columns_table[token.string_value];
        }

        if (index < 0 || index > row.length()) {
            result.token_type = TokenType::ERROR;
            result.error_msg = "Invalid column index " + QString::number(token.number_value) + " on line " + QString::number(token.line_number);
            throw std::logic_error(result.error_msg.toStdString());
            //return result;
        }

        //return value at row index
        result.token_type = TokenType::STRING;
        result.string_value = row.at(index);
        auto compiled_func = [token = token, index](const QStringList& row) {

            Token result = token;
            result.token_type = TokenType::STRING;
            result.string_value = row.at(index);

            return result;
            };
        return compiled_func;
    }
    
    std::function<Token(const QMap<QString, QStringList>& data_rows)> Term::compile(const QMap<QString, QStringList> data_rows)
    {
        // ColumnResult result;
        Token result = token;
        double index = 0;

        //qDebug() << "compiling term: " << token.string_value <<" type:"<< token.to_string();
		//qDebug() << "data_rows: " << data_rows;
        QStringList row;

        int num_of_rows = data_rows.size();
        QString data_row_key = "$";

        //check name
        if (token.token_type == TokenType::NAME) {
            if (token.string_value.contains(".")) { // in the format file.number
                QString file_name;
                QString column_name;
                QStringList name_parts = token.string_value.split('.'); //columnname in format file.column

                file_name = name_parts[0];
                column_name = name_parts[1];

                if (!symbol_table.contains(file_name)) {
                    result.token_type = TokenType::ERROR;
                    result.error_msg = "3 Unknown column name '" + token.string_value + "' on line " + QString::number(token.line_number);
                    throw std::logic_error(result.error_msg.toStdString());
                }

                bool is_num;
                double column_index = column_name.toInt(&is_num);
                if (!is_num) {
                    //qDebug() << "does columns_table contain '"<< column_name<<"' :"<< columns_table.contains(column_name);
                    if (!columns_table.contains(column_name) && column_name != "*") {
                        result.token_type = TokenType::ERROR;
                        result.error_msg = "3 Unknown column name '" + token.string_value + "' on line " + QString::number(token.line_number);
                        //qDebug() << "about to throw!";
                        throw std::logic_error(result.error_msg.toStdString());
                    }
                    //qDebug() << "here!";

                }


            }
            else if (!symbol_table.contains(token.string_value.toLower())) {  // name not in symbol table
                result.token_type = TokenType::ERROR;
                result.error_msg = "3 Unknown symbol '" + token.string_value + "' on line " + QString::number(token.line_number);
                throw std::logic_error(result.error_msg.toStdString());
            }

        }

        if (num_of_rows == 1) {
            row = data_rows["$"];
        }
		else { // set row based on file.column format

            //check if name is of the format file.number

            
            /*
            QString file_name;
            QString column_name;
            QStringList name_parts = token.string_value.split('.'); //columnname in format file.column

            if (name_parts.size() != 2) {
                result.token_type = TokenType::ERROR;
                result.error_msg = "2 Unknown column name " + token.string_value + " on line " + QString::number(token.line_number);
                throw std::logic_error(result.error_msg.toStdString());
            }

            file_name = name_parts[0];
            column_name = name_parts[1];

            if (!symbol_table.contains(file_name)) {
                result.token_type = TokenType::ERROR;
                result.error_msg = "3 Unknown column name " + token.string_value + " on line " + QString::number(token.line_number);
                throw std::logic_error(result.error_msg.toStdString());
            }

            TokenType tk = symbol_table[file_name];
            if (tk != TokenType::STRING) {
                result.token_type = TokenType::ERROR;
                result.error_msg = "4 Unknown column name " + token.string_value + " on line " + QString::number(token.line_number);
                throw std::logic_error(result.error_msg.toStdString());
            }

            if (!strings_table.contains(file_name)) {
                result.token_type = TokenType::ERROR;
                result.error_msg = "5 Unknown column name " + token.string_value + " on line " + QString::number(token.line_number);
                throw std::logic_error(result.error_msg.toStdString());
            }

            QString file_path = strings_table[file_name];

            row = data_rows[file_path];
            data_row_key = file_path;
            //qDebug()<<"row size: "<<row.size();
            */

        }

        //literal as is
        if (token.token_type == TokenType::MULT) {

            if (num_of_rows == 2) {
                std::string error = "Ambigious column " + token.string_value.toStdString() + " on line ";
                error += token.line_number;
                throw std::logic_error(error);
            }

            result.token_type = TokenType::STRING;
            result.string_value = row.join(',');

            auto compiled_func = [token = token, data_row_key](const QMap<QString, QStringList>& data_rows) {
                QStringList row; 
                row = data_rows[data_row_key];

                Token result = token;
                result.token_type = TokenType::STRING;
                result.string_value = row.join(',');

                return result;
                };
            return compiled_func;
        }

        if (token.token_type == TokenType::STRING) {

            auto compiled_func = [token = token](const QMap<QString, QStringList>& data_rows) {

                Token result = token;
                result.token_type = TokenType::STRING;
                result.string_value = token.string_value;

                return result;
                };
            return compiled_func;
        }
        if (token.token_type == TokenType::NUMBER) {

            auto compiled_func = [token = token](const QMap<QString, QStringList>& data_rows) {

                Token result = token;
                result.token_type = TokenType::NUMBER;
                result.number_value = token.number_value;

               if (std::floor(token.number_value) == token.number_value) {
                   result.string_value = QString::number(static_cast<qint64>(token.number_value));
               }
               else {
                   result.string_value = QString::number(token.number_value, 'f', 2);
               }


                //result.string_value = QString::number(token.number_value);

                return result;
                };
            return compiled_func;
        }

        //get column index
        if (token.token_type == TokenType::COLUMNNUMBER) {
            //qDebug()<<"I am here! "<<token.string_value;
            if (num_of_rows == 2) {
                std::string error = "Ambigious column '" + token.string_value.toStdString() + "' on line ";
                error += token.line_number;
                throw std::logic_error(error);
            }

            index = token.number_value;

        }
        else if (token.token_type == TokenType::COLUMNNAME) {
            //qDebug() << "here";
            if (!columns_table.contains(token.string_value.toLower())) { //not in columns table

                //check if name is of the format file.number
                QString file_name;
                QString column_name;
                QStringList name_parts = token.string_value.split('.'); //columnname in format file.column

                if (name_parts.size() != 2) {
                    result.token_type = TokenType::ERROR;
                    result.error_msg = "6 Unknown column name " + token.string_value + " on line " + QString::number(token.line_number);
                    throw std::logic_error(result.error_msg.toStdString());
                }

                file_name = name_parts[0];
                column_name = name_parts[1];

                bool is_number;
                int value = column_name.toInt(&is_number);

                data_row_key = file_name;

				qDebug() << "data_row_key: " << data_row_key <<" data_rows:"<<data_rows;

                /*
                if(num_of_rows == 2){
                    row = data_rows[file_name];
                }*/

                if (column_name == '*') { // if column name is of the form, file_name.*
                    result.token_type = TokenType::STRING;
                    result.string_value = row.join(',');

                    auto compiled_func = [token = token, data_row_key](const QMap<QString, QStringList>& data_rows) {
                        QStringList row;
                        row = data_rows[data_row_key];

                        Token result = token;
                        result.token_type = TokenType::STRING;
                        result.string_value = row.join(',');

                        return result;
                        };
                    return compiled_func;
                }
                else if (is_number) {
                    //qDebug()<<"column index: "<<value;
                    index = value;
                }
            }
            else {
				QStringList name_parts = token.string_value.split('.'); //columnname in format file.column

                if (!symbol_table.contains(name_parts[0].toLower())) {
                    result.token_type = TokenType::ERROR;
                    result.error_msg = "Unknown column name '" + token.string_value + "' on line " + QString::number(token.line_number);
                    throw std::logic_error(result.error_msg.toStdString());
                }

                if (!strings_table.contains(name_parts[0].toLower())) {
                    result.token_type = TokenType::ERROR;
                    result.error_msg = "Unable to resolve column name '" + token.string_value + "' on line " + QString::number(token.line_number);
                    throw std::logic_error(result.error_msg.toStdString());
                }
                //qDebug() << "setting index and data-row-key...";
				QString file_path = strings_table[name_parts[0].toLower()];
				data_row_key = file_path;
    
                index = columns_table[token.string_value];
				row = data_rows[data_row_key];
				//qDebug() << "index set to " << index <<" and data-row-key:"<<data_row_key;
            }

            /*
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
            }*/
        }
        else if (token.token_type == TokenType::NAME) {
            Token tk = token;
            Term t;
            //qDebug()<<"evaluating a NAME Token \n";
            if (token.string_value.contains('.')) { // check if name is of the format, file.* or file.number
                //qDebug()<<"name is "<<token.string_value;

                QStringList name_parts = token.string_value.split('.'); //columnname in format file.column
                if (name_parts.size() == 2) {
                    QString file_name = name_parts[0];
                    QString column_name = name_parts[1];

                    bool is_number;
                    double num = column_name.toInt(&is_number);

                    if (data_rows.size() == 1) {
                        row = data_rows["$"];
                        data_row_key = "$";
                    }
                    else if (data_rows.size() > 1) {
                        //qDebug()<<"data_rows: "<<data_rows;
                        if (!strings_table.contains(file_name)) {
                            std::string error = "7 Unknown colunm name '" + token.string_value.toStdString() + "' on line ";
                            error += token.line_number;
                            throw std::logic_error(error);
                        }
                        QString file_path = strings_table[file_name];
                        if (data_rows.contains(file_path)) {
                            row = data_rows[file_path];
                            data_row_key = file_path;
                        }
                        else {
                            std::string error = "8 Unknown colunm name '" + token.string_value.toStdString() + "' on line ";
                            error += token.line_number;
                            throw std::logic_error(error);
                        }
                    }

                    if (column_name == '*') {
                        tk.token_type = TokenType::MULT;
                        t = Term(tk);
                        //QStringList row;

                        //return t.eval(row);
                        auto comp_term_f = t.compile_eval(row);


                        auto compiled_func = [token = token, data_row_key, comp_term_f](const QMap<QString, QStringList>& data_rows) {
                            QStringList _row = data_rows[data_row_key];

                            return comp_term_f(_row);
                            };
                        return compiled_func;
                        //return t.compile_eval(row);

                    }
                    else if (is_number) {
                        tk.token_type = TokenType::COLUMNNUMBER;
                        tk.number_value = num;
                        t = Term(tk);
                        //QStringList row = (data_rows.size() == 2)? data_rows[file_name] : data_rows["$"];

                        // column index check
                        if (num < 0 || num >= row.length()) {
                            QString error = "Invalid index '";
                            error += token.string_value;
                            error += " (";
                            error += QString::number(num);
                            error += ")' on line ";
                            error += QString::number(token.line_number);
                            throw std::logic_error(error.toStdString());
                        }

                        //return t.eval(row);
                        auto comp_term_f = t.compile_eval(row);
                        auto compiled_func = [token = token, data_row_key, comp_term_f](const QMap<QString, QStringList>& data_rows) {
                            QStringList _row = data_rows[data_row_key];

                            return comp_term_f(_row);
                            };
                        return compiled_func;
                    }
                    else if (symbol_table.contains(token.string_value.toLower())) {
                        TokenType type = symbol_table[token.string_value.toLower()];
                        //Token tk = token;
                        tk.token_type = type;

                        if (type == TokenType::NUMBER) {
                            double num = numbers_table[token.string_value.toLower()];
                            tk.number_value = num;
                            tk.string_value = QString::number(num);
                        }
                        else if (type == TokenType::STRING) {
                            QString str = strings_table[token.string_value.toLower()];
                            tk.string_value = str;
                        }
                        else if (type == TokenType::COLUMNNAME) {
                            tk.token_type = TokenType::COLUMNNUMBER;
                            tk.number_value = columns_table[token.string_value.toLower()];

                            // column index check
                            if (tk.number_value < 0 || tk.number_value >= row.length()) {
                                QString error = "Invalid index '";
                                error += token.string_value;
                                error += " (";
                                error += QString::number(tk.number_value);
                                error += ")' on line ";
                                error += QString::number(token.line_number);
                                throw std::logic_error(error.toStdString());
                            }
                        }

                        t = Term(tk);
                        //return t.eval(row);
                        auto comp_term_f = t.compile_eval(row);
                        auto compiled_func = [token = token, data_row_key, comp_term_f](const QMap<QString, QStringList>& data_rows) {
                            QStringList _row = data_rows[data_row_key];

                            return comp_term_f(_row);
                            };
                        return compiled_func;
                    }
                    else {
                        std::string error = "9 Unknown name '" + token.string_value.toStdString() + "' on line ";
                        error += token.line_number;
                        throw std::logic_error(error);
                    }
                }
                else {
                    std::string error = "Invalid syntax for column name (" + token.string_value.toStdString() + ") on line ";
                    error += token.line_number;
                    throw std::logic_error(error);
                }
            }

            if (!symbol_table.contains(token.string_value.toLower())) {
                //foreach(auto s, symbol_table.keys()) {
                //    qDebug() <<"Symbol: "<< s;
                //}
                std::string error = "10 Unknown name '" + token.string_value.toStdString() + "' on line ";
                error += token.line_number;
                throw std::logic_error(error);
            }

            TokenType type = symbol_table[token.string_value.toLower()];
            //Token tk = token;
            tk.token_type = type;

            if (type == TokenType::NUMBER) {
                double num = numbers_table[token.string_value.toLower()];
                tk.number_value = num;
                tk.string_value = QString::number(num);
            }
            else if (type == TokenType::STRING) {
                QString str = strings_table[token.string_value.toLower()];
                tk.string_value = str;
            }
            else if (type == TokenType::COLUMNNAME) {
                if (data_rows.size() != 1) {
                    std::string error = "Ambigious column '" + token.string_value.toStdString() + "' on line ";
                    error += token.line_number;
                    throw std::logic_error(error);
                }

                double number_value = columns_table[token.string_value.toLower()];

                // column index check
                if (number_value < 0 || number_value >= row.length()) {
                    QString error = "Invalid index '";
                    error += token.string_value;
                    error += " (";
                    error += QString::number(number_value);
                    error += ")' on line ";
                    error += QString::number(token.line_number);
                    throw std::logic_error(error.toStdString());
                }

            }

            row = data_rows["$"];
            data_row_key = "$";

            t = Term(tk);
            //return t.eval(row);
            auto comp_term_f = t.compile_eval(row);
            auto compiled_func = [token = token, data_row_key, comp_term_f](const QMap<QString, QStringList>& data_rows) {
                QStringList _row = data_rows[data_row_key];

                return comp_term_f(_row);
                };
            return compiled_func;
        }

        //qDebug()<<"column index2: "<<index <<" row length: "<<row.length() <<", data_row_key: "<<data_row_key;
		//qDebug() << "row: " << row;
        if (index < 0 || index >(row.length() - 1)) {
            result.token_type = TokenType::ERROR;
            result.error_msg = "Invalid column index '" + QString::number(token.number_value) + "' on line " + QString::number(token.line_number);
            throw std::logic_error(result.error_msg.toStdString());
            //return result;
        }

        //return value at row index
        result.token_type = TokenType::STRING;
        result.string_value = row.at(index);
        //return result;
        auto compiled_func = [token = token, data_row_key, index](const QMap<QString, QStringList>& data_rows) {
            Token result = token;
            result.token_type = TokenType::STRING;
            QStringList row = data_rows[data_row_key];
            result.string_value = row.at(index);

            return result;
            };
        return compiled_func;

    }

}