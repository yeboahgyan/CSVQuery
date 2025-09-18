#include "functions.h"
#include <QDateTime>
#include <stdexcept>
#include <any>


Term trim(QList<Term> args)
{
    Term result;

    QList<TokenType> arg_types;
    foreach(auto t, args){
        arg_types.append(t.get_token().token_type);
    }

    QList<TokenType> expected_arg_types = {TokenType::STRING};
    int expected_num_of_args = 1;

    if(arg_types != expected_arg_types){
        QString error = "trim(string) expects 1 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else if(args.length() != expected_num_of_args){
        QString error = "trim(string) expects 1 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else{
        try{
            QString str = std::any_cast<QString>(args.front().get_token().string_value);
            Token t;
            t.string_value = str.trimmed();
            t.token_type = TokenType::STRING;
            result = Term(t);
        }catch(const std::bad_any_cast& e){
            QString error = "trim(string) expects a string argument!";
            throw std::logic_error(error.toStdString()+ e.what());
        }
    }

    return result;
}

Term length(QList<Term> args)
{
    Term result;

    QList<TokenType> arg_types;
    foreach(auto t, args){
        arg_types.append(t.get_token().token_type);
    }

    QList<TokenType> expected_arg_types = {TokenType::STRING};
    int expected_num_of_args = 1;

    if(arg_types != expected_arg_types){
        QString error = "length(string) expects 1 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else if(args.length() != expected_num_of_args){
        QString error = "length(string) expects 1 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else{
        try{
            QString str = std::any_cast<QString>(args.front().get_token().string_value);
            Token t;
            t.number_value = str.length();
            t.token_type = TokenType::NUMBER;
            result = Term(t);

        }catch(const std::bad_any_cast& e){
            QString error = "length(string) expects a string argument! ";
            throw std::logic_error(error.toStdString()+ e.what());
        }
    }
    return result;
}

Term substring(QList<Term> args)
{
    Term result;

    QList<TokenType> arg_types;
    foreach(auto t, args){
        arg_types.append(t.get_token().token_type);
    }

    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::NUMBER, TokenType::NUMBER};
    int expected_num_of_args = 3;

    if(arg_types != expected_arg_types){
        QString error = "substring(string, index, length) expects 3 arguments(string, number, number)!";
        throw std::logic_error(error.toStdString());
    }
    else if(args.length() != expected_num_of_args){
        QString error = "substring(string, index, length) expects 3 arguments(string, number, number)!";
        throw std::logic_error(error.toStdString());
    }
    else{
        try{
            QString str = std::any_cast<QString>(args[0].get_token().string_value);
            double index = std::any_cast<int>(args[1].get_token().number_value);
            double length = std::any_cast<int>(args[2].get_token().number_value);

            if(index > str.length()){
                QString error = "substring(string, index, length) index greater than string length!";
                throw std::logic_error(error.toStdString());
            }
            else if(index < 0){
                QString error = "substring(string, index, length) invalid index!";
                throw std::logic_error(error.toStdString());
            }
            else{
                Token t;
                t.string_value = str.mid(index, length);
                t.token_type = TokenType::STRING;
                result = Term(t);
            }

        }catch(const std::bad_any_cast& e){
            QString error = "substring(string, index, length) expects 3 arguments(string, number, number)!";
            throw std::logic_error(error.toStdString()+ e.what());
        }
    }
    return result;
}

Term left(QList<Term> args)
{
    Term result;

    QList<TokenType> arg_types;
    foreach(auto t, args){
        arg_types.append(t.get_token().token_type);
    }


    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::NUMBER};
    int expected_num_of_args = 2;

    if(arg_types != expected_arg_types){
        QString error = "left(string, length) expects 2 arguments(string, number)!";
        throw std::logic_error(error.toStdString());
    }
    else if(args.length() != expected_num_of_args){
        QString error = "left(string, length) expects 2 arguments(string, number)!";
        throw std::logic_error(error.toStdString());
    }
    else{
        try{
            QString str = std::any_cast<QString>(args[0].get_token().string_value);
            double length = std::any_cast<int>(args[1].get_token().number_value);

            if(length < 0){
                QString error = "left(string, length) invalid length!";
                throw std::logic_error(error.toStdString());
            }
            else{
                Token t;
                t.string_value = str.left(length);
                t.token_type = TokenType::STRING;
                result = Term(t);
            }

        }catch(const std::bad_any_cast& e){
            QString error = "left(string, length) expects 2 arguments(string, number)! ";
            throw std::logic_error(error.toStdString()+ e.what());
        }
    }

    return result;
}

Term right(QList<Term> args)
{
    Term result;

    QList<TokenType> arg_types;
    foreach(auto t, args){
        arg_types.append(t.get_token().token_type);
    }

    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::NUMBER};
    int expected_num_of_args = 2;

    if(arg_types != expected_arg_types){
        QString error = "right(string, length) expects 2 arguments(string, number)!";
        throw std::logic_error(error.toStdString());
    }
    else if(args.length() != expected_num_of_args){
        QString error = "right(string, length) expects 2 arguments(string, number)!";
        throw std::logic_error(error.toStdString());
    }
    else{
        try{
            QString str = std::any_cast<QString>(args[0].get_token().string_value);
            double length = std::any_cast<int>(args[1].get_token().number_value);

            if(length < 0){
                QString error = "right(string, length) invalid length!";
                throw std::logic_error(error.toStdString());
            }
            else{
                Token t;
                t.string_value = str.right(length);
                t.token_type = TokenType::STRING;
                result = Term(t);
            }

        }catch(const std::bad_any_cast& e){
            QString error = "right(string, length) expects 2 arguments(string, number)!";
            throw std::logic_error(error.toStdString()+ e.what());
        }
    }
    return result;
}

Term date_gt(QList<Term> args)
{
    Term result;

    QList<TokenType> arg_types;
    foreach(auto t, args){
        arg_types.append(t.get_token().token_type);
    }

    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING}; //(date_str, format, date_str, format)
    int expected_num_of_args = 4;

    if(arg_types != expected_arg_types){
        QString error = "date_gt(string, date_format, string2, date_format) expects 4 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else if(args.length() != expected_num_of_args){
        QString error = "date_gt(string, date_format, string2, date_format) expects 4 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else{
        try{
            QString date_str1 = std::any_cast<QString>(args[0].get_token().string_value);
            QString date_str2 = std::any_cast<QString>(args[1].get_token().string_value);

            QString format1 = std::any_cast<QString>(args[2].get_token().string_value);
            QString format2 = std::any_cast<QString>(args[3].get_token().string_value);

            QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
            QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

            QString failure_str;
            if(!datetime1.isValid()){
                failure_str = date_str1;
            }
            else if( !datetime2.isValid()){
                failure_str = date_str2;
            }

            if(!failure_str.isEmpty()){
                QString error = "Failed to convert "+failure_str+" to a DateTime!";
                throw std::logic_error(error.toStdString());
            }else{
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value =  (datetime1 > datetime2);
                result = Term(t);
            }

        }catch(const std::bad_any_cast& e){
            QString error = "date_gt(string, date_format, string2, date_format) expects 4 string argument!";
            throw std::logic_error(error.toStdString() + e.what());
        }
    }

    return result;
}

Term date_lt(QList<Term> args)
{
    Term result;

    QList<TokenType> arg_types;
    foreach(auto t, args){
        arg_types.append(t.get_token().token_type);
    }

    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING}; //(date_str, format, date_str, format)
    int expected_num_of_args = 4;

    if(arg_types != expected_arg_types){
        QString error = "date_lt(string, date_format, string2, date_format) expects 4 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else if(args.length() != expected_num_of_args){
        QString error = "date_lt(string, date_format, string2, date_format) expects 4 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else{
        try{
            QString date_str1 = std::any_cast<QString>(args[0].get_token().string_value);
            QString date_str2 = std::any_cast<QString>(args[1].get_token().string_value);

            QString format1 = std::any_cast<QString>(args[2].get_token().string_value);
            QString format2 = std::any_cast<QString>(args[3].get_token().string_value);

            QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
            QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

            QString failure_str;
            if(!datetime1.isValid()){
                failure_str = date_str1;
            }
            else if( !datetime2.isValid()){
                failure_str = date_str2;
            }

            if(!failure_str.isEmpty()){
                QString error = "Failed to convert "+failure_str+" to a DateTime!";
                throw std::logic_error(error.toStdString());
            }else{
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value =  (datetime1 < datetime2);

                result = Term(t);
            }

        }catch(const std::bad_any_cast& e){
            QString error = "date_lt(string, date_format, string2, date_format) expects 4 string argument!";
            throw std::logic_error(error.toStdString() + e.what());
        }
    }
    return result;
}

Term date_ge(QList<Term> args) //greater than or equal
{
    Term result;

    QList<TokenType> arg_types;
    foreach(auto t, args){
        arg_types.append(t.get_token().token_type);
    }

    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING}; //(date_str, format, date_str, format)
    int expected_num_of_args = 4;

    if(arg_types != expected_arg_types){
        QString error = "date_ge(string, date_format, string2, date_format) expects 4 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else if(args.length() != expected_num_of_args){
        QString error = "date_ge(string, date_format, string2, date_format) expects 4 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else{
        try{
            QString date_str1 = std::any_cast<QString>(args[0].get_token().string_value);
            QString date_str2 = std::any_cast<QString>(args[1].get_token().string_value);

            QString format1 = std::any_cast<QString>(args[2].get_token().string_value);
            QString format2 = std::any_cast<QString>(args[3].get_token().string_value);

            QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
            QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

            QString failure_str;
            if(!datetime1.isValid()){
                failure_str = date_str1;
            }
            else if( !datetime2.isValid()){
                failure_str = date_str2;
            }

            if(!failure_str.isEmpty()){
                QString error = "Failed to convert "+failure_str+" to a DateTime!";
                throw std::logic_error(error.toStdString());
            }else{
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value =  (datetime1 >= datetime2);

                result = Term(t);
            }

        }catch(const std::bad_any_cast& e){
            QString error = "date_ge(string, date_format, string2, date_format) expects 4 string argument!";
            throw std::logic_error(error.toStdString() + e.what());
        }
    }
    return result;
}

Term date_le(QList<Term> args) //less than or equal
{
    Term result;

    QList<TokenType> arg_types;
    foreach(auto t, args){
        arg_types.append(t.get_token().token_type);
    }

    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING}; //(date_str, format, date_str, format)
    int expected_num_of_args = 4;

    if(arg_types != expected_arg_types){
        QString error = "date_le(string, date_format, string2, date_format) expects 4 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else if(args.length() != expected_num_of_args){
        QString error = "date_le(string, date_format, string2, date_format) expects 4 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else{
        try{
            QString date_str1 = std::any_cast<QString>(args[0].get_token().string_value);
            QString date_str2 = std::any_cast<QString>(args[1].get_token().string_value);

            QString format1 = std::any_cast<QString>(args[2].get_token().string_value);
            QString format2 = std::any_cast<QString>(args[3].get_token().string_value);

            QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
            QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

            QString failure_str;
            if(!datetime1.isValid()){
                failure_str = date_str1;
            }
            else if( !datetime2.isValid()){
                failure_str = date_str2;
            }

            if(!failure_str.isEmpty()){
                QString error = "Failed to convert "+failure_str+" to a DateTime!";
                throw std::logic_error(error.toStdString());
            }else{
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value =  (datetime1 <= datetime2);
                result = Term(t);
            }

        }catch(const std::bad_any_cast& e){
            QString error = "date_le(string, date_format, string2, date_format) expects 4 string argument!";
            throw std::logic_error(error.toStdString() + e.what());
        }
    }
    return result;
}

Term date_eq(QList<Term> args)
{
    Term result;

    QList<TokenType> arg_types;
    foreach(auto t, args){
        arg_types.append(t.get_token().token_type);
    }

    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING}; //(date_str, format, date_str, format)
    int expected_num_of_args = 4;

    if(arg_types != expected_arg_types){
        QString error = "date_eq(string, date_format, string2, date_format) expects 4 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else if(args.length() != expected_num_of_args){
        QString error = "date_eq(string, date_format, string2, date_format) expects 4 string argument!";
        throw std::logic_error(error.toStdString());
    }
    else{
        try{
            QString date_str1 = std::any_cast<QString>(args[0].get_token().string_value);
            QString date_str2 = std::any_cast<QString>(args[1].get_token().string_value);

            QString format1 = std::any_cast<QString>(args[2].get_token().string_value);
            QString format2 = std::any_cast<QString>(args[3].get_token().string_value);

            QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
            QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

            QString failure_str;
            if(!datetime1.isValid()){
                failure_str = date_str1;
            }
            else if( !datetime2.isValid()){
                failure_str = date_str2;
            }

            if(!failure_str.isEmpty()){
                QString error = "Failed to convert "+failure_str+" to a DateTime!";
                throw std::logic_error(error.toStdString());
            }else{
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value =  (datetime1 == datetime2);
                result = Term(t);
            }

        }catch(const std::bad_any_cast& e){
            QString error = "date_eq(string, date_format, string2, date_format) expects 4 string argument!";
            throw std::logic_error(error.toStdString()+ e.what());
        }
    }
    return result;
}
