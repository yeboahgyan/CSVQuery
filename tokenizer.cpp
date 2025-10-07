#include "tokenizer.h"
#include <stdexcept>
#include <QDebug>


namespace csvquery {

    Tokenizer::Tokenizer(std::shared_ptr<QTextStream> str)
        : stream{ str },
        token{ TokenType::END }
    {
        char_table['+'] = TokenType::PLUS;
        char_table['-'] = TokenType::MINUS;
        char_table['*'] = TokenType::MULT;
        char_table['/'] = TokenType::DIV;
        char_table['='] = TokenType::ASSIGN;
        char_table['('] = TokenType::LBRACKET;
        char_table[')'] = TokenType::RBRACKET;
        //char_table['['] = TokenType::LSQBRACKET;
        //char_table[']'] = TokenType::RSQBRACKET;
        char_table[','] = TokenType::COMMA;
        char_table[';'] = TokenType::SEMICOLON;
        char_table[':'] = TokenType::COLON;
        //char_table['"'] = TokenType::DOUBLEQUOTE;
        //char_table['\''] = TokenType::SINGLEQOUTE;
        char_table['>'] = TokenType::GREATERTHAN;
        char_table['<'] = TokenType::LESSTHAN;
        char_table['!'] = TokenType::NOT;
        char_table['$'] = TokenType::DOLLAR;
    }

    Token Tokenizer::read(QChar quote)
    {
        QString str;
        QChar ch;

        if (quote == '*') { // read till whitespace or operator character
            //std::cout<<"reading name...\n";
            QChar previous_ch;
            while (true) {
                previous_ch = ch;
                (*stream) >> ch;

                if (ch.isSpace()) {
                    if (ch == '\n') {
                        line_number++;
                    }
                    break;
                }

                if (char_table.contains(ch)) {

                    if (ch == '*') { // allow names in format tablename.*
                        if (previous_ch == '.') {
                            //continue reading
                        }
                    }
                    else
                    {
                        stream->seek(stream->pos() - 1); //put char back in stream
                        break;
                    }
                }

                str.append(ch);
                if (stream->atEnd()) {
                    break;
                }
            }
            //std::cout <<"name = "<<str.toStdString()<<"\n";
            token = { .token_type = TokenType::NAME, .string_value = str, .line_number = line_number, .token_name = "TokenType::NAME" };
            //std::cout<<"Tokenizer::get() str="<<str.toStdString() <<" \n";

        }
        else {
            while (true) {
                (*stream) >> ch;
                if (ch == quote) {
                    break;
                }

                if (ch == '\n') {
                    line_number++;
                }

                if (char_table.contains(ch) && quote == ']') {
                    stream->seek(stream->pos() - 1); //put char back in stream
                    //token = {.token_type = TokenType::ERROR, .line_number = line_number, .error_msg = "Unvalid operator in column name", .token_name="Unvalid operator in column name"};
                    QString error_msg = "Unvalid operator in column name ";
                    error_msg += QString::number(line_number);
                    throw std::logic_error(error_msg.toStdString());
                    //return token;;
                }

                str.append(ch);
                if (stream->atEnd()) {//error
                    QString error = (quote == ']') ? "Unexpected end to column name" : "Unexpected end to string";
                    throw std::logic_error(error.toStdString());
                    //token = {.token_type = TokenType::ERROR, .line_number = line_number, .error_msg = error, .token_name=error};
                    //return token;
                }
            }


            TokenType type = TokenType::STRING;

            if (quote == ']') {
                bool ok;
                double number = str.toDouble(&ok);

                if (ok) {
                    type = TokenType::COLUMNNUMBER;
                    token = { .token_type = type, .string_value = QString::number(number), .number_value = number,  .line_number = line_number, .token_name = "TokenType::COLUMNNUMBER" };
                }
                else {
                    type = TokenType::COLUMNNAME;
                    token = { .token_type = type, .string_value = str, .line_number = line_number, .token_name = "TokenType::COLUMNNAME" };
                }
            }
            else {
                token = { .token_type = type, .string_value = str, .line_number = line_number, .token_name = "TokenType::STRING" };
            }
        }

        //token = {.token_type = TokenType::NAME, .string_value = str, .line_number = line_number};
        return token;
    }

    Token Tokenizer::get()
    {
        QChar ch;

        if (stream->atEnd()) {
            token = { .token_type = TokenType::END, .line_number = line_number };
            token.token_name = token.to_string();
            return token;
        }

        (*stream) >> ch;

        while (ch.isSpace()) { //skip whitespace
            if (ch == '\n') {
                line_number++; //track current line number
            }
            if (stream->atEnd()) {
                token = { .token_type = TokenType::END, .line_number = line_number };
                token.token_name = token.to_string();
                return token;
            }
            (*stream) >> ch;
        }

        if (ch == '#') { //Skip comment line
            QString str = stream->readLine();
            //std::cout<<"\n{comment: "<<str.toStdString()<<"}\n";
            line_number++;

            return get();
        }


        /* OPERATORS */
        if (char_table.contains(ch)) {
            if (ch == '>') {
                (*stream) >> ch;
                if (ch == '=') {
                    token = { .token_type = TokenType::GREATERTHANOREQUAL, .string_value = ">=", .line_number = line_number };
                    token.token_name = token.to_string();
                }
                else {
                    stream->seek(stream->pos() - 1); //put char back in stream
                    token = { .token_type = TokenType::GREATERTHAN, .string_value = ">", .line_number = line_number };
                    token.token_name = token.to_string();
                }
            }
            else if (ch == '<') {
                (*stream) >> ch;
                if (ch == '=') {
                    token = { .token_type = TokenType::LESSTHANOREQUAL, .string_value = "<=", .line_number = line_number };
                    token.token_name = token.to_string();
                }
                else {
                    stream->seek(stream->pos() - 1); //put char back in stream
                    token = { .token_type = TokenType::LESSTHAN, .string_value = "<", .line_number = line_number };
                    token.token_name = token.to_string();
                }
            }
            else if (ch == '!') {
                (*stream) >> ch;
                if (ch == '=') {
                    token = { .token_type = TokenType::NOTEQUALTO, .string_value = "!=", .line_number = line_number };
                    token.token_name = token.to_string();
                }
                else {
                    QString error = "Unexpected character ";
                    error.append(ch);
                    error += " after ! on line " + QString::number(line_number);
                    throw std::logic_error(error.toStdString());
                }
            }
            else {
                token = { .token_type = char_table[ch], .string_value = ch, .line_number = line_number };
                token.token_name = token.to_string();
                //return token;
            }
        }
        /* NUMBERS */
        else if (ch.isDigit()) {
            stream->seek(stream->pos() - 1); //put char back in stream

            double number;

            (*stream) >> number;
            token = { .token_type = TokenType::NUMBER, .string_value = QString::number(number), .number_value = number, .line_number = line_number };
            token.token_name = token.to_string();
        }
        /* NAMES */
        else if (ch.isLetter()) {
            stream->seek(stream->pos() - 1); //put char back in stream

            token = read();
            //std::cout<<"debug: " <<token <<" "<< token.string_value.toStdString()<<"\n";

            // is a keyword
            QStringList expected_joins = { "inner", "outer", "cross"}; //joins
            if (keywords.contains(token.string_value.toLower()) || expected_joins.contains(token.string_value.toLower()) || token.string_value.toLower() == "not") {

                if (expected_joins.contains(token.string_value.toLower()) || token.string_value.toLower() == "not") { // is join
                    //std::cout<<"new token read!\n";
                    Token inner_token = token;
                    Token join_token = read(); //read join

                    if (join_token.string_value.toLower() != "join" && join_token.string_value.toLower() != "like") {
                        QString error_msg = "Unexpected name after " + inner_token.string_value +" (" + join_token.string_value + ") on line " + QString::number(line_number);
                        throw std::logic_error(error_msg.toStdString());
                    }
                    //qDebug()<<"join: "<<token.string_value.toLower()+" "+ join_token.string_value.toLower();
                    token.token_type = keywords[inner_token.string_value.toLower() + " " + join_token.string_value.toLower()];

                    if (token.token_type == TokenType::INNERJOIN) {
                        token.token_name = "inner join";
                        token.string_value = "inner join";
                        token.token_name = token.to_string();

                    }
                    else if (token.token_type == TokenType::OUTERJOIN) {
                        token.token_name = "outer join";
                        token.string_value = "outer join";
                        token.token_name = token.to_string();
                    }
                    else if (token.token_type == TokenType::CROSSJOIN) {
                        token.token_name = "cross join";
                        token.string_value = "outer join";
                        token.token_name = token.to_string();
                    }
                    else if (token.token_type == TokenType::NOTLIKE) {
                        token.token_name = "not like";
                        token.string_value = "not like";
                        token.token_name = token.to_string();
                        //qDebug() << "not like!";
                    }
                }
                else { // is other keyword
                    token.token_type = keywords[token.string_value.toLower()];
                    token.string_value = keywords.key(token.token_type);
                    token.token_name = token.to_string();
                
                }
            }
            else if (symbol_table.contains(token.string_value.toLower())) { // is built in variable or function
                //std::cout<<"found a built-in symbol "<<token.string_value.toStdString()<<"\n";

                TokenType name_type = symbol_table[token.string_value.toLower()];
                token.token_type = name_type;

                //std::cout << "symbol: " << token.string_value.toStdString() << " type: " << token.to_string().toStdString() <<"\n";

                if (name_type == TokenType::STRING) {
                    QString string_value = strings_table[token.string_value.toLower()];
                    token.string_value = string_value;
                    token.token_type = TokenType::STRING;
                    token.token_name = "TokenType::STRING";
                }
                else if (name_type == TokenType::NUMBER) {
                    double number_value = numbers_table[token.string_value.toLower()];
                    token.number_value = number_value;
                    token.string_value = QString::number(number_value);
                    token.token_type = TokenType::NUMBER;
                    token.token_name = "TokenType::NUMBER";
                }
                else if (name_type == TokenType::COLUMNNAME) {
                    double index = columns_table[token.string_value.toLower()]; //get column index
                    token.number_value = index;
                    //token.string_value = QString::number(index);
                    token.token_type = TokenType::COLUMNNAME;
                    token.token_name = "TokenType::COLUMNNAME";
                }
                else if (name_type == TokenType::FUNCTION) {
                    //std::cout<<"symbol '"<<token.string_value.toStdString()<<"' is a function.\n";

                    token.token_type = TokenType::FUNCTION;
                    token.token_name = "TokenType::FUNCTION";

                    token.func = funcs_table[token.string_value.toLower()];

                    Token func_token = token;

                    //read function args etc.
                    QList<TokenType> arg_types = func_args_type_list[token.string_value.toLower()];
                    Token next_token = get();

                    if (next_token.token_type != TokenType::LBRACKET) {
                        std::string num;
                        num += line_number;
                        throw std::logic_error("Unexpected character after function " + token.string_value.toStdString() + " on line " + num);
                    }

                    //int count = arg_types.length();
                    //std::cout<<"number of args for "<<func_token.string_value.toLower().toStdString()<<" is "<<count<<"\n";

                    next_token = get();
                    while (next_token.token_type != TokenType::RBRACKET) {


                        /*
                        if(next_token.token_type == TokenType::RBRACKET){
                            std::string error = "Unexpected end to function arguments on line ";
                            error += QString::number(line_number).toStdString();
                            error += " " + token.string_value.toStdString();
                            error += "requires ";
                            error += arg_types.length();
                            error += "arguments";
                            throw std::logic_error(error);
                        }

                        if(next_token.token_type == TokenType::COMMA){
                            continue;
                        }*/

                        // save function arguments
                        func_token.func_args.append(next_token);
                        next_token = get();


                        //if(next_token.token_type != arg_type){
                        //    std::string error ="Argument number ";
                        //    error += count;
                        //    error += " should be of type ";
                        //    error += token_to_string[arg_type].toStdString();
                        //    throw std::logic_error(error);
                        //}
                        //count--;
                    }

                    /*
                    if(count != 0){
                        std::string error = "Fewer arguments provided to function on line ";
                        error += QString::number(line_number).toStdString();
                        error += " " + token.string_value.toStdString();
                        error += "requires ";
                        error += arg_types.length();
                        error += "arguments";
                        throw std::logic_error(error);
                    }*/

                    //eat ) bracket
                    //next_token = get();
                    //std::cout<<"\n token after function args: "<<next_token.to_string().toStdString()<<"\n";
                    if (next_token.token_type != TokenType::RBRACKET) {
                        std::string error = "invalid function syntax on line ";
                        error += QString::number(line_number).toStdString();
                        throw std::logic_error(error);
                    }
                    token = func_token;
                }
            }
            else { // it is an unknown name
                token.token_name = token.to_string();
                //qDebug()<<"unknown name: "<<token.string_value;
            }

        }
        /* STRINGS */
        else if (ch == '\'' || ch == '"' || ch == '[') {
            QChar delimiter = (ch == '[') ? ']' : ch;

            token = read(delimiter);
        }

        return token;
    }

    QList<Token> Tokenizer::read_statement()
    {
        QList<Token> tokens;

        Token t = get();
        while (t.token_type != TokenType::SEMICOLON) {
            if (t.token_type == TokenType::END) {
                tokens.append(t);
                break;
            }
            tokens.append(t);
        }

        return tokens;
    }

    std::ostream& operator<<(std::ostream& out, const Token& t)
    {

        return out << t.token_name.toStdString();
    }

}