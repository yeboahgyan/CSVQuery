#include "functions.h"
#include <QDateTime>


ColumnResult trim(QList<TokenType> args, QList<std::any> vals)
{
    ColumnResult result;
    QList<TokenType> expected_arg_types = {TokenType::STRING};
    int expected_num_of_args = 1;

    if(args != expected_arg_types){
        result.error = "trim(string) expects 1 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else if(vals.length() != expected_num_of_args){
        result.error = "trim(string) expects 1 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else{
        try{
            QString str = std::any_cast<QString>(vals.front());
            result.string_value = str.trimmed();
            result.token_type = TokenType::STRING;
        }catch(const std::bad_any_cast& e){
            result.error = "trim(string) expects a string argument!";
            result.token_type = TokenType::ERROR;
        }
    }

    return result;
}

ColumnResult length(QList<TokenType> args, QList<std::any> vals)
{
    ColumnResult result;
    QList<TokenType> expected_arg_types = {TokenType::STRING};
    int expected_num_of_args = 1;

    if(args != expected_arg_types){
        result.error = "length(string) expects 1 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else if(vals.length() != expected_num_of_args){
        result.error = "length(string) expects 1 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else{
        try{
            QString str = std::any_cast<QString>(vals.front());
            result.number_value = str.length();
            result.token_type = TokenType::NUMBER;
        }catch(const std::bad_any_cast& e){
            result.error = "length(string) expects a string argument!";
            result.token_type = TokenType::ERROR;
        }
    }
    return result;
}

ColumnResult substring(QList<TokenType> args, QList<std::any> vals)
{
    ColumnResult result;
    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::NUMBER, TokenType::NUMBER};
    int expected_num_of_args = 3;

    if(args != expected_arg_types){
        result.error = "substring(string, index, length) expects 3 arguments(string, number, number)!";
        result.token_type = TokenType::ERROR;
    }
    else if(vals.length() != expected_num_of_args){
        result.error = "substring(string, index, length) expects 3 arguments(string, number, number)!";
        result.token_type = TokenType::ERROR;
    }
    else{
        try{
            QString str = std::any_cast<QString>(vals[0]);
            double index = std::any_cast<int>(vals[1]);
            double length = std::any_cast<int>(vals[2]);

            if(index > str.length()){
                result.error = "substring(string, index, length) index greater than string length!";
                result.token_type = TokenType::ERROR;
            }
            else if(index < 0){
                result.error = "substring(string, index, length) invalid index!";
                result.token_type = TokenType::ERROR;
            }
            else{
                result.string_value = str.mid(index, length);
                result.token_type = TokenType::STRING;
            }

        }catch(const std::bad_any_cast& e){
            result.error = "substring(string, index, length) expects 3 arguments(string, number, number)!";
            result.token_type = TokenType::ERROR;
        }
    }
    return result;
}

ColumnResult left(QList<TokenType> args, QList<std::any> vals)
{
    ColumnResult result;
    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::NUMBER};
    int expected_num_of_args = 2;

    if(args != expected_arg_types){
        result.error = "left(string, length) expects 2 arguments(string, number)!";
        result.token_type = TokenType::ERROR;
    }
    else if(vals.length() != expected_num_of_args){
        result.error = "left(string, length) expects 2 arguments(string, number)!";
        result.token_type = TokenType::ERROR;
    }
    else{
        try{
            QString str = std::any_cast<QString>(vals[0]);
            double length = std::any_cast<int>(vals[1]);

            if(length < 0){
                result.error = "left(string, length) invalid length!";
                result.token_type = TokenType::ERROR;
            }
            else{
                result.string_value = str.left(length);
                result.token_type = TokenType::STRING;
            }

        }catch(const std::bad_any_cast& e){
            result.error = "left(string, length) expects 2 arguments(string, number)!";
            result.token_type = TokenType::ERROR;
        }
    }

    return result;
}

ColumnResult right(QList<TokenType> args, QList<std::any> vals)
{
    ColumnResult result;
    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::NUMBER};
    int expected_num_of_args = 2;

    if(args != expected_arg_types){
        result.error = "right(string, length) expects 2 arguments(string, number)!";
        result.token_type = TokenType::ERROR;
    }
    else if(vals.length() != expected_num_of_args){
        result.error = "right(string, length) expects 2 arguments(string, number)!";
        result.token_type = TokenType::ERROR;
    }
    else{
        try{
            QString str = std::any_cast<QString>(vals[0]);
            double length = std::any_cast<int>(vals[1]);

            if(length < 0){
                result.error = "right(string, length) invalid length!";
                result.token_type = TokenType::ERROR;
            }
            else{
                result.string_value = str.right(length);
                result.token_type = TokenType::STRING;
            }

        }catch(const std::bad_any_cast& e){
            result.error = "right(string, length) expects 2 arguments(string, number)!";
            result.token_type = TokenType::ERROR;
        }
    }
    return result;
}

ColumnResult date_gt(QList<TokenType> args, QList<std::any> vals)
{
    ColumnResult result;
    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING}; //(date_str, format, date_str, format)
    int expected_num_of_args = 4;

    if(args != expected_arg_types){
        result.error = "date_gt(string, date_format, string2, date_format) expects 4 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else if(vals.length() != expected_num_of_args){
        result.error = "date_gt(string, date_format, string2, date_format) expects 4 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else{
        try{
            QString date_str1 = std::any_cast<QString>(vals[0]);
            QString date_str2 = std::any_cast<QString>(vals[1]);

            QString format1 = std::any_cast<QString>(vals[2]);
            QString format2 = std::any_cast<QString>(vals[3]);

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
                result.error = "Failed to convert "+failure_str+" to a DateTime!";
                result.token_type = TokenType::ERROR;
            }else{
                result.token_type = TokenType::BOOLEAN;
                result.boolean_value =  (datetime1 > datetime2);
            }

        }catch(const std::bad_any_cast& e){
            result.error = "date_gt(string, date_format, string2, date_format) expects 4 string argument!";
            result.token_type = TokenType::ERROR;
        }
    }

    return result;
}

ColumnResult date_lt(QList<TokenType> args, QList<std::any> vals)
{
    ColumnResult result;
    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING}; //(date_str, format, date_str, format)
    int expected_num_of_args = 4;

    if(args != expected_arg_types){
        result.error = "date_lt(string, date_format, string2, date_format) expects 4 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else if(vals.length() != expected_num_of_args){
        result.error = "date_lt(string, date_format, string2, date_format) expects 4 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else{
        try{
            QString date_str1 = std::any_cast<QString>(vals[0]);
            QString date_str2 = std::any_cast<QString>(vals[1]);

            QString format1 = std::any_cast<QString>(vals[2]);
            QString format2 = std::any_cast<QString>(vals[3]);

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
                result.error = "Failed to convert "+failure_str+" to a DateTime!";
                result.token_type = TokenType::ERROR;
            }else{
                result.token_type = TokenType::BOOLEAN;
                result.boolean_value =  (datetime1 < datetime2);
            }

        }catch(const std::bad_any_cast& e){
            result.error = "date_lt(string, date_format, string2, date_format) expects 4 string argument!";
            result.token_type = TokenType::ERROR;
        }
    }
    return result;
}

ColumnResult date_ge(QList<TokenType> args, QList<std::any> vals) //greater than or equal
{
    ColumnResult result;
    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING}; //(date_str, format, date_str, format)
    int expected_num_of_args = 4;

    if(args != expected_arg_types){
        result.error = "date_ge(string, date_format, string2, date_format) expects 4 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else if(vals.length() != expected_num_of_args){
        result.error = "date_ge(string, date_format, string2, date_format) expects 4 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else{
        try{
            QString date_str1 = std::any_cast<QString>(vals[0]);
            QString date_str2 = std::any_cast<QString>(vals[1]);

            QString format1 = std::any_cast<QString>(vals[2]);
            QString format2 = std::any_cast<QString>(vals[3]);

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
                result.error = "Failed to convert "+failure_str+" to a DateTime!";
                result.token_type = TokenType::ERROR;
            }else{
                result.token_type = TokenType::BOOLEAN;
                result.boolean_value =  (datetime1 >= datetime2);
            }

        }catch(const std::bad_any_cast& e){
            result.error = "date_ge(string, date_format, string2, date_format) expects 4 string argument!";
            result.token_type = TokenType::ERROR;
        }
    }
    return result;
}

ColumnResult date_le(QList<TokenType> args, QList<std::any> vals) //less than or equal
{
    ColumnResult result;
    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING}; //(date_str, format, date_str, format)
    int expected_num_of_args = 4;

    if(args != expected_arg_types){
        result.error = "date_le(string, date_format, string2, date_format) expects 4 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else if(vals.length() != expected_num_of_args){
        result.error = "date_le(string, date_format, string2, date_format) expects 4 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else{
        try{
            QString date_str1 = std::any_cast<QString>(vals[0]);
            QString date_str2 = std::any_cast<QString>(vals[1]);

            QString format1 = std::any_cast<QString>(vals[2]);
            QString format2 = std::any_cast<QString>(vals[3]);

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
                result.error = "Failed to convert "+failure_str+" to a DateTime!";
                result.token_type = TokenType::ERROR;
            }else{
                result.token_type = TokenType::BOOLEAN;
                result.boolean_value =  (datetime1 <= datetime2);
            }

        }catch(const std::bad_any_cast& e){
            result.error = "date_le(string, date_format, string2, date_format) expects 4 string argument!";
            result.token_type = TokenType::ERROR;
        }
    }
    return result;
}

ColumnResult date_eq(QList<TokenType> args, QList<std::any> vals)
{
    ColumnResult result;
    QList<TokenType> expected_arg_types = {TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING}; //(date_str, format, date_str, format)
    int expected_num_of_args = 4;

    if(args != expected_arg_types){
        result.error = "date_eq(string, date_format, string2, date_format) expects 4 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else if(vals.length() != expected_num_of_args){
        result.error = "date_eq(string, date_format, string2, date_format) expects 4 string argument!";
        result.token_type = TokenType::ERROR;
    }
    else{
        try{
            QString date_str1 = std::any_cast<QString>(vals[0]);
            QString date_str2 = std::any_cast<QString>(vals[1]);

            QString format1 = std::any_cast<QString>(vals[2]);
            QString format2 = std::any_cast<QString>(vals[3]);

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
                result.error = "Failed to convert "+failure_str+" to a DateTime!";
                result.token_type = TokenType::ERROR;
            }else{
                result.token_type = TokenType::BOOLEAN;
                result.boolean_value =  (datetime1 == datetime2);
            }

        }catch(const std::bad_any_cast& e){
            result.error = "date_eq(string, date_format, string2, date_format) expects 4 string argument!";
            result.token_type = TokenType::ERROR;
        }
    }
    return result;
}
