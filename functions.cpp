// CSVQuery - An SQL-like query language for CSV files
// Copyright (c) 2025-2026 Kwame Yeboah-Gyan
// Distributed under the MIT License.

#include "functions.h"
#include <QDateTime>
#include <stdexcept>
#include <any>

namespace csvquery {

    Term trim(QList<Term> args)
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING };
        int expected_num_of_args = 1;

        if (arg_types != expected_arg_types) {
            QString error = "trim(string) expects 1 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "trim(string) expects 1 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            QString str = args.front().get_token().string_value;
            Token t;
            t.string_value = str.trimmed();
            t.line_number = args.front().get_token().line_number;
            t.token_type = TokenType::STRING;
            result = Term(t);
        }

        return result;
    }

    Term length(QList<Term> args)
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING };
        int expected_num_of_args = 1;

        if (arg_types != expected_arg_types) {
            QString error = "length(string) expects 1 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "length(string) expects 1 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            QString str = args.front().get_token().string_value;
            Token t;
            t.number_value = str.length();
            t.line_number = args.front().get_token().line_number;
            t.token_type = TokenType::NUMBER;
            result = Term(t);

        }
        return result;
    }

    Term substring(QList<Term> args)
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::NUMBER, TokenType::NUMBER };
        int expected_num_of_args = 3;

        if (arg_types != expected_arg_types) {
            QString error = "substring(string, index, length) expects 3 arguments(string, number, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "substring(string, index, length) expects 3 arguments(string, number, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            QString str = args[0].get_token().string_value;
            double index = args[1].get_token().number_value;
            double length = args[2].get_token().number_value;

            if (index > str.length()) {
                QString error = "substring(string, index, length) index greater than string length on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else if (index < 0) {
                QString error = "substring(string, index, length) invalid index on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                Token t;
                t.string_value = str.mid(index, length);
                t.line_number = args.front().get_token().line_number;
                t.token_type = TokenType::STRING;
                result = Term(t);
            }
        }
        return result;
    }

    Term left(QList<Term> args)
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }


        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::NUMBER };
        int expected_num_of_args = 2;

        if (arg_types != expected_arg_types) {
            QString error = "left(string, length) expects 2 arguments (string, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "left(string, length) expects 2 arguments (string, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            //qDebug() << "arg1: " << args[0].get_token().to_string() <<" value: "<< args[0].get_token().string_value;
                //qDebug() << "arg2: " << args[1].get_token().to_string() <<" value: "<< args[1].get_token().number_value;

            //std::any arg1 = args[0].get_token().string_value;
            //std::any arg2 = args[1].get_token().number_value;
            QString str = args[0].get_token().string_value;
            double length = args[1].get_token().number_value;

            if (length < 0) {
                QString error = "left(string, length) invalid length on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                Token t;
                t.string_value = str.left(length);
                t.line_number = args.front().get_token().line_number;
                t.token_type = TokenType::STRING;
                result = Term(t);
            }
            
        }

        return result;
    }

    Term right(QList<Term> args)
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::NUMBER };
        int expected_num_of_args = 2;

        if (arg_types != expected_arg_types) {
            QString error = "right(string, length) expects 2 arguments(string, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "right(string, length) expects 2 arguments(string, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            QString str = args[0].get_token().string_value;
            double length = args[1].get_token().number_value;

            if (length < 0) {
                QString error = "right(string, length) invalid length on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                Token t;
                t.string_value = str.right(length);
                t.line_number = args.front().get_token().line_number;
                t.token_type = TokenType::STRING;
                result = Term(t);
            }
            
        }
        return result;
    }

    Term strip_quotes(QList<Term> args) //remove doubles from column value surrounded by double quotes
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING };
        int expected_num_of_args = 1;

        if (arg_types != expected_arg_types) {
            QString error = "strip_quotes(string) expects 1 argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "strip_quotes(string) expects 1 argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            QString str = args.front().get_token().string_value;

            if (str.front() == "\"") {
                str.remove(0, 1); //remove first char
            }

            if (str.back() == "\"") {
                str.chop(1); //remove last char
            }
            Token t;
            t.string_value = str;
            t.line_number = args.front().get_token().line_number;
            t.token_type = TokenType::STRING;
            result = Term(t);
        }

        return result;
    }

    Term date_gt(QList<Term> args)
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str, format)
        int expected_num_of_args = 4;

        QList<TokenType> alt_expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str)
        int alt_expected_num_of_args = 3;

        if (arg_types != expected_arg_types && arg_types != alt_expected_arg_types) {
            QString error = "date_gt(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args && args.length() != alt_expected_num_of_args) {
            QString error = "date_gt(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            QString date_str1 = args[0].get_token().string_value;
            QString format1 = args[1].get_token().string_value;

            QString date_str2 = args[2].get_token().string_value;

            QString format2 = (args.length() == 4) ? args[3].get_token().string_value : format1;

            QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
            QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

            QString failure_str;
            if (!datetime1.isValid()) {
                failure_str = date_str1;
            }
            else if (!datetime2.isValid()) {
                failure_str = date_str2;
            }

            if (!failure_str.isEmpty()) {
                QString error = "Failed to convert " + failure_str + " to a DateTime on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.line_number = args.front().get_token().line_number;
                t.boolean_value = (datetime1 > datetime2);
                result = Term(t);
            }
        }

        return result;
    }

    Term date_lt(QList<Term> args)
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str, format)
        int expected_num_of_args = 4;

        QList<TokenType> alt_expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str)
        int alt_expected_num_of_args = 3;

        if (arg_types != expected_arg_types && arg_types != alt_expected_arg_types) {
            QString error = "date_lt(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args && args.length() != alt_expected_num_of_args) {
            QString error = "date_lt(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {

            QString date_str1 = args[0].get_token().string_value;
            QString format1 = args[1].get_token().string_value;

            QString date_str2 = args[2].get_token().string_value;


            QString format2 = (args.length() == 4) ? args[3].get_token().string_value : format1;

            QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
            QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

            QString failure_str;
            if (!datetime1.isValid()) {
                failure_str = date_str1;
            }
            else if (!datetime2.isValid()) {
                failure_str = date_str2;
            }

            if (!failure_str.isEmpty()) {
                QString error = "Failed to convert " + failure_str + " to a DateTime on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = (datetime1 < datetime2);
                t.line_number = args.front().get_token().line_number;

                result = Term(t);
            }
        }
        return result;
    }

    Term date_ge(QList<Term> args) //greater than or equal
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str, format)
        int expected_num_of_args = 4;

        QList<TokenType> alt_expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str)
        int alt_expected_num_of_args = 3;

        if (arg_types != expected_arg_types && arg_types != alt_expected_arg_types) {
            QString error = "date_ge(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args && args.length() != alt_expected_num_of_args) {
            QString error = "date_ge(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            QString date_str1 = args[0].get_token().string_value;
            QString format1 = args[1].get_token().string_value;

            QString date_str2 = args[2].get_token().string_value;
            QString format2 = (args.length() == 4) ? args[3].get_token().string_value : format1;

            QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
            QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

            QString failure_str;
            if (!datetime1.isValid()) {
                failure_str = date_str1;
            }
            else if (!datetime2.isValid()) {
                failure_str = date_str2;
            }

            if (!failure_str.isEmpty()) {
                QString error = "Failed to convert " + failure_str + " to a DateTime on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = (datetime1 >= datetime2);
                t.line_number = args.front().get_token().line_number;

                result = Term(t);
            }
        }
        return result;
    }

    Term date_le(QList<Term> args) //less than or equal
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str, format)
        int expected_num_of_args = 4;

        QList<TokenType> alt_expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str)
        int alt_expected_num_of_args = 3;

        if (arg_types != expected_arg_types && arg_types != alt_expected_arg_types) {
            QString error = "date_le(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args && args.length() != alt_expected_num_of_args) {
            QString error = "date_le(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {

            QString date_str1 = args[0].get_token().string_value;
            QString format1 = args[1].get_token().string_value;

            QString date_str2 = args[2].get_token().string_value;


            QString format2 = (args.length() == 4) ? args[3].get_token().string_value : format1;

            QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
            QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

            QString failure_str;
            if (!datetime1.isValid()) {
                failure_str = date_str1;
            }
            else if (!datetime2.isValid()) {
                failure_str = date_str2;
            }

            if (!failure_str.isEmpty()) {
                QString error = "Failed to convert " + failure_str + " to a DateTime on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = (datetime1 <= datetime2);
                t.line_number = args.front().get_token().line_number;

                result = Term(t);
            }
           
        }
        return result;
    }

    Term date_eq(QList<Term> args)
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str, format)
        int expected_num_of_args = 4;

        QList<TokenType> alt_expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str)
        int alt_expected_num_of_args = 3;

        if (arg_types != expected_arg_types && arg_types != alt_expected_arg_types) {
            QString error = "Incorrect use of date_eq(string, date_format, string2, date_format) or date_eq(string, date_format, string2) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        
        if (args.length() != expected_num_of_args && args.length() != alt_expected_num_of_args) {
            QString error = "Incorrect use of date_eq(string, date_format, string2, date_format) or date_eq(string, date_format, string2) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        
        QString date_str1 = args[0].get_token().string_value;
        QString format1 = args[1].get_token().string_value;

        QString date_str2 = args[2].get_token().string_value;


        QString format2 = (args.length() == 4) ? args[3].get_token().string_value : format1;

        QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
        QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

        QString failure_str;
        if (!datetime1.isValid()) {
            failure_str = date_str1;
        }
        else if (!datetime2.isValid()) {
            failure_str = date_str2;
        }

        if (!failure_str.isEmpty()) {
            QString error = "Failed to convert " + failure_str + " to a DateTime!";
            throw std::logic_error(error.toStdString());
        }
        else {
            Token t;
            t.token_type = TokenType::BOOLEAN;
            t.boolean_value = (datetime1 == datetime2);
            t.line_number = args.front().get_token().line_number;

            result = Term(t);
        }

        return result;
    }

    Term number(QList<Term> args) // convert string to number or throw exception
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING};
        int expected_num_of_args = 1;

        if (arg_types != expected_arg_types) {
            QString error = "number(string) expects a string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        
        if (args.length() != expected_num_of_args) {
            QString error = "number(string) expects a string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }

        QString num_str = args[0].get_token().string_value;
        //qDebug() << "number string: " << num_str;
        bool is_number = false;
        double number = num_str.toDouble(&is_number);

        if (is_number) {
            Token t;
            t.token_type = TokenType::NUMBER;
            t.number_value = number;
            t.string_value = num_str;
            t.line_number = args.at(0).get_token().line_number;
            result = Term(t);
        }
        else {

            Token t;
            t.token_type = TokenType::NUMBER;
            t.number_value = 0;
            t.string_value = num_str;
            t.line_number = args.at(0).get_token().line_number;
            result = Term(t);

            /*
            QString error = "String '";
            error += num_str;
            error += "' passed to number(string) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            error += " is not a number!";
            throw std::logic_error(error.toStdString());
            */
        }
        
        return result;
    }

    Term modulo(QList<Term> args) // modulo of 2 numbers
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types1 = { TokenType::STRING, TokenType::NUMBER };
        QList<TokenType> expected_arg_types2= { TokenType::STRING, TokenType::STRING };
        QList<TokenType> expected_arg_types3 = { TokenType::NUMBER, TokenType::NUMBER };
        int expected_num_of_args = 2;

        if (args.length() != expected_num_of_args) {
            QString error = "modulo(string, number) or modulo(string, string) expects 2 arguments (string, number) or (string, string) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }


        if (arg_types == expected_arg_types1) { //string, number

            QString num_str = args[0].get_token().string_value;
            bool is_number = false;
            int numerator = num_str.toInt(&is_number);

            int denominator = args[1].get_token().number_value;

            if (!is_number) {
                QString error = "Unable to convert an argument passed to modulo(string, number) or modulo(string, string) to an integer on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }

            if (denominator == 0) {
                QString error = "Division by zero in modulo(string, number) or modulo(string, string) on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            
            int mod_result = (numerator % denominator + denominator) % denominator;//std::fmod(numerator, denominator);
            Token t;
            t.token_type = TokenType::NUMBER;
            t.number_value = mod_result;
            t.string_value = QString::number(mod_result);
            t.line_number = args.at(0).get_token().line_number;
            result = Term(t);
            
        }
		else if (arg_types == expected_arg_types2) { // both arguments are Numbers


            int numerator = args[0].get_token().number_value;

            int denominator = args[1].get_token().number_value;


            if (denominator == 0) {
                QString error = "Division by zero in modulo(string, number) or modulo(string, string) on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            
            double mod_result = (numerator % denominator + denominator) % denominator;
            Token t;
            t.token_type = TokenType::NUMBER;
            t.number_value = mod_result;
            t.string_value = QString::number(mod_result);
            t.line_number = args.at(0).get_token().line_number;
            result = Term(t);
            
        }
        else if (arg_types == expected_arg_types3) { //number, number

            QString num_str = args[0].get_token().string_value;
            bool is_number = false;
            int numerator = num_str.toInt(&is_number);

            int denominator = args[1].get_token().number_value;

            if (!is_number) {
                QString error = "Unable to convert an argument passed to modulo(string, number) or modulo(string, string) to an integer on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }

            if (denominator == 0) {
                QString error = "Division by zero in modulo(string, number) or modulo(string, string) on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }

            int mod_result = (numerator % denominator + denominator) % denominator;//std::fmod(numerator, denominator);
            Token t;
            t.token_type = TokenType::NUMBER;
            t.number_value = mod_result;
            t.string_value = QString::number(mod_result);
            t.line_number = args.at(0).get_token().line_number;
            result = Term(t);

        }
        else {
            QString error = "modulo(string, number) or modulo(string, string) expects 2 arguments (string, number) or (string, string) on line ";
            error += QString::number(args.at(0).get_token().line_number);
			throw std::logic_error(error.toStdString());
        }


        return result;
    }

    /*
    Term case_function(QList<Term> args)
    {
        //case function is handled as a special form in the interpreter and should not be called here
		//syntax: CASE variable WHEN value1 THEN result1 ... ELSE default_result END

        const Term& key = args.at(0);
        TokenType key_type = key.get_token_type();

        auto it = args.cbegin();
        
        int i = 1;
        while (it != args.cend()) {

            if (it == args.cbegin()) {
				++it; // move to first value to compare against key
            }


            if (i % 2 != 0) {
                const Term& value = *it;
                TokenType value_type = value.get_token_type();

				++it; // move to result corresponding to value
                ++i; // used to check for even/odd to determine if current term is value to compare against key or result corresponding to value

                if (it == args.cend()) { // Handle Else branch
					return value; // no more values, return current value term as default result
                }

                if (key_type != value_type) {
                    QString error = "Type mismatch in CASE function on line ";
                    error += QString::number(key.get_token().line_number);
                    throw std::logic_error(error.toStdString());
                }

                if (key.get_token_type() == TokenType::NUMBER) {
                    if (key.get_token().number_value == value.get_token().number_value) {
                        return *it;
                    }
                }
                else if (key.get_token_type() == TokenType::STRING) {
                    if (key.get_token().string_value == value.get_token().string_value) {
                        return *it;
                    }
                }
                

            }

			++i; // used to check for even/odd to determine if current term is value to compare against key or result corresponding to value
            ++it;
        }

		// no match found and no else branch, return null
        Term result;
		Token t;
		t.token_type = TokenType::STRING;
        t.string_value = "";
        t.line_number = key.get_token().line_number;
        result = Term(t);

		return result;
    }
    */

    Term case_function(QList<Term> args)
    {
        if (args.size() < 3) {
            throw std::logic_error("Invalid CASE expression");
        }

        const Term& key = args.at(0);

        int i = 1;
        while (i + 1 < args.size()) {
            const Term& value = args.at(i);
            const Term& result = args.at(i + 1);

            if (key.get_token_type() != value.get_token_type()) {
                QString error = "Type mismatch in CASE function on line ";
                error += QString::number(key.get_token().line_number);
                throw std::logic_error(error.toStdString());
            }

            if (key.get_token_type() == TokenType::NUMBER &&
                key.get_token().number_value == value.get_token().number_value) {
                return result;
            }

            if (key.get_token_type() == TokenType::STRING &&
                key.get_token().string_value.toLower() == value.get_token().string_value.toLower()) {
                return result;
            }

            i += 2;
        }

        // ELSE branch exists if there is one leftover argument
        if (i < args.size()) {
            return args.at(i);
        }

        // no match and no ELSE
        Token t;
        t.token_type = TokenType::STRING;
        t.string_value = "";
        t.line_number = key.get_token().line_number;
        return Term(t);
    }

    Term dynamic_case_function(QList<Term> args)
    {
        if (args.size() < 3) {
            throw std::logic_error("Invalid CASE expression");
        }

        const Term& key = args.at(0);

        int i = 1;
        while (i + 1 < args.size()) {
            const Term& value = args.at(i);
            const Term& result = args.at(i + 1);

            if (key.get_token_type() != value.get_token_type()) {
                QString error = "Type mismatch in CASE function on line ";
                error += QString::number(key.get_token().line_number);
                throw std::logic_error(error.toStdString());
            }

            if (key.get_token_type() == TokenType::NUMBER &&
                key.get_token().number_value == value.get_token().number_value) {
                return result;
            }

            if (key.get_token_type() == TokenType::STRING &&
                key.get_token().string_value == value.get_token().string_value) {
                return result;
            }

            i += 2;
        }

        // ELSE branch exists if there is one leftover argument
        if (i < args.size()) {
            return args.at(i);
        }

        // no match and no ELSE
        Token t;
        t.token_type = TokenType::STRING;
        t.string_value = "";
        t.line_number = key.get_token().line_number;
        return Term(t);
    }

    
    //Aggregate functions have an extra hidden argument (the function token itself)
    // that is added before the function is called when evaluating the expression

    Term execute_aggregation(const QList<Term>& args, AggregFuncType func_type)
    {
        Term result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
            //qDebug() << "count args " << t.get_token().to_string();
        }

        QList<TokenType> expected_arg_types1 = { TokenType::STRING, TokenType::FUNCTION };
        QList<TokenType> expected_arg_types2 = { TokenType::NUMBER, TokenType::FUNCTION };
        int expected_num_of_args = 2;


        if (arg_types != expected_arg_types1 && arg_types != expected_arg_types2) {
            QString error = "count() expects a string or number argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "count() expects 1 argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            // retrieve counter
            // check if already updated for this count and parameter
            // if so return counter without updating
            // else update counter and return new value

            QString aggregate_name = args.at(1).get_token().string_value;

            //QMap<QString, std::shared_ptr<AggregateCounter>> aggregate_expression_reg = {};
            //QMap<QString, bool> check_if_aggregate_done = {};

            //qDebug() << "aggregate key is " << aggregate_expression_reg_key;

            //if (aggregate_expression_reg[aggregate_expression_reg_key].contains(aggregate_name)) {
            if (aggregate_expression_reg.contains(aggregate_expression_reg_key+"|" + aggregate_name)) {
                //std::shared_ptr<AggregateCounter>& count_counter = aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name];
                std::shared_ptr<AggregateCounter>& count_counter = aggregate_expression_reg[aggregate_expression_reg_key+"|" + aggregate_name];

                bool function_called_already = false;

                //if (check_if_aggregate_done[aggregate_expression_reg_key].contains(aggregate_name)) {
                if (check_if_aggregate_done.contains(aggregate_expression_reg_key+"|" + aggregate_name)) {
                    //function_called_already = check_if_aggregate_done[aggregate_expression_reg_key][aggregate_name];
                    function_called_already = check_if_aggregate_done.contains(aggregate_expression_reg_key+"|" + aggregate_name);
                }


                if (function_called_already) { // called already in another column
                    Token t = args.at(1).get_token();
                    t.token_type = TokenType::NUMBER;
                    t.number_value = count_counter->get_value();
                    t.string_value = QString::number(count_counter->get_value());
                    t.line_number = args.front().get_token().line_number;

                    result = Term(t);
                    //qDebug() << "[called already] current count: " << count_counter->get_value();

                }
                else {
                    if (args.at(0).get_token().token_type == TokenType::STRING) {
                        count_counter->process_data(args.at(0).get_token().string_value);
                    }
                    else if (args.at(0).get_token().token_type == TokenType::NUMBER) {
                        count_counter->process_data(QString::number(args.at(0).get_token().number_value));
                    }

                    Token t = args.at(1).get_token();
                    t.token_type = TokenType::NUMBER;
                    t.number_value = count_counter->get_value();
                    t.string_value = QString::number(count_counter->get_value());
                    t.line_number = args.front().get_token().line_number;

                    result = Term(t);

                    //check_if_aggregate_done[aggregate_expression_reg_key][aggregate_name] = true; // skip aggregation if same aggregate function and parameter is called
                    check_if_aggregate_done[aggregate_expression_reg_key+"|" + aggregate_name] = true; // skip aggregation if same aggregate function and parameter is called

                    //qDebug() << "current count: " << count_counter->get_value();
                }

            }
            else {// first time this aggregation is being called with this parameter
                if (func_type == AggregFuncType::count) {
                    //aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<CountCounter>();
                    aggregate_expression_reg[aggregate_expression_reg_key+"|" + aggregate_name] = std::make_shared<CountCounter>();
                }
                else if (func_type == AggregFuncType::avg) {
                    //aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<AvgCounter>();
                    aggregate_expression_reg[aggregate_expression_reg_key+"|" + aggregate_name] = std::make_shared<AvgCounter>();
                }
                else if (func_type == AggregFuncType::sum) {
                    //aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<SumCounter>();
                    aggregate_expression_reg[aggregate_expression_reg_key+"|"+aggregate_name] = std::make_shared<SumCounter>();
                }
                else if (func_type == AggregFuncType::min) {
                    //aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<MinCounter>();
                    aggregate_expression_reg[aggregate_expression_reg_key+"|"+aggregate_name] = std::make_shared<MinCounter>();
                }
                else if (func_type == AggregFuncType::max) {
                    //aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<MaxCounter>();
                    aggregate_expression_reg[aggregate_expression_reg_key+"|" + aggregate_name] = std::make_shared<MaxCounter>();
                }
                else {
                    QString error = "Internal error executing aggregate function on line ";
                    error += QString::number(args.at(0).get_token().line_number);
                    throw std::logic_error(error.toStdString());
                }
                

                //std::shared_ptr<AggregateCounter>& count_counter = aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name];
                std::shared_ptr<AggregateCounter>& count_counter = aggregate_expression_reg[aggregate_expression_reg_key+"|" + aggregate_name];

                if (args.at(0).get_token().token_type == TokenType::STRING) {
                    count_counter->process_data(args.at(0).get_token().string_value);
                }
                else if (args.at(0).get_token().token_type == TokenType::NUMBER) {
                    count_counter->process_data(QString::number(args.at(0).get_token().number_value));
                }
                else {
                    //qDebug() << "unknown type";
                }

                Token t = args.at(1).get_token();
                t.token_type = TokenType::NUMBER;
                t.number_value = count_counter->get_value();
                t.string_value = QString::number(count_counter->get_value());
                t.line_number = args.front().get_token().line_number;

                result = Term(t);

                //check_if_aggregate_done[aggregate_expression_reg_key][aggregate_name] = true; // skip aggregation if same aggregate function and parameter is called
                check_if_aggregate_done[aggregate_expression_reg_key+"|" + aggregate_name] = true; // skip aggregation if same aggregate function and parameter is called
                //qDebug() << "current count: " << count_counter->get_value();
            }
        }

        return result;
    }


    Term count(QList<Term> args)
    {
        return execute_aggregation(args, AggregFuncType::count);
    }

    Term sum(QList<Term> args)
    {
        return execute_aggregation(args, AggregFuncType::sum);
    }

    Term min(QList<Term> args)
    {
        return execute_aggregation(args, AggregFuncType::min);
    }

    Term max(QList<Term> args)
    {
        return execute_aggregation(args, AggregFuncType::max);
    }

    Term avg(QList<Term> args)
    {
        return execute_aggregation(args, AggregFuncType::avg);
    }
    

    //compiled functions
    //compile equivalents functions
    std::function<Term(const QList<Term>&)> comp_trim(const QList<Term>& args)
    {
        std::function<Term(const QList<Term>&)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING };
        int expected_num_of_args = 1;

        if (arg_types != expected_arg_types) {
            QString error = "trim(string) expects 1 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "trim(string) expects 1 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            func_result = [](const QList<Term>& args) {
                Term result;
                QString str = args.front().get_token().string_value;
                Token t;
                t.string_value = str.trimmed();
                t.line_number = args.front().get_token().line_number;
                t.token_type = TokenType::STRING;
                result = Term(t);
                return result;
                };
            
        }

        return func_result;
    }

    std::function<Term(const QList<Term>&)> comp_length(const QList<Term>& args)
    {
        std::function<Term(const QList<Term>&)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING };
        int expected_num_of_args = 1;

        if (arg_types != expected_arg_types) {
            QString error = "length(string) expects 1 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "length(string) expects 1 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            func_result = [](const QList<Term>& args) {
                Term result;

                QString str = args.front().get_token().string_value;
                Token t;
                t.number_value = str.length();
                t.line_number = args.front().get_token().line_number;
                t.token_type = TokenType::NUMBER;
                result = Term(t);
                return result;

                };
            

        }
        return func_result;
    }

    std::function<Term(const QList<Term>&)> comp_substring(const QList<Term>& args)
    {
        std::function<Term(const QList<Term>&)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::NUMBER, TokenType::NUMBER };
        int expected_num_of_args = 3;

        if (arg_types != expected_arg_types) {
            QString error = "substring(string, index, length) expects 3 arguments(string, number, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "substring(string, index, length) expects 3 arguments(string, number, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            QString str = args[0].get_token().string_value;
            double index = args[1].get_token().number_value;
            double length = args[2].get_token().number_value;

            if (index > str.length()) {
                QString error = "substring(string, index, length) index greater than string length on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else if (index < 0) {
                QString error = "substring(string, index, length) invalid index on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                func_result = [index, length](const QList<Term>& args) {
                    Term result;
                    Token t;
                    QString str = args[0].get_token().string_value;
                    t.string_value = str.mid(index, length);
                    t.line_number = args.front().get_token().line_number;
                    t.token_type = TokenType::STRING;
                    result = Term(t);
                    return result;
                    };
                
            }
        }
        return func_result;

    }

    std::function<Term(const QList<Term>&)> comp_left(const QList<Term>& args)
    {
        std::function<Term(const QList<Term>&)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }


        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::NUMBER };
        int expected_num_of_args = 2;

        if (arg_types != expected_arg_types) {
            QString error = "left(string, length) expects 2 arguments (string, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "left(string, length) expects 2 arguments (string, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            //qDebug() << "arg1: " << args[0].get_token().to_string() <<" value: "<< args[0].get_token().string_value;
                //qDebug() << "arg2: " << args[1].get_token().to_string() <<" value: "<< args[1].get_token().number_value;

            //std::any arg1 = args[0].get_token().string_value;
            //std::any arg2 = args[1].get_token().number_value;
            QString str = args[0].get_token().string_value;
            double length = args[1].get_token().number_value;

            if (length < 0) {
                QString error = "left(string, length) invalid length on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                func_result = [length](const QList<Term>& args) {
                    Term result;
                    Token t;
                    QString str = args[0].get_token().string_value;
                    t.string_value = str.left(length);
                    t.line_number = args.front().get_token().line_number;
                    t.token_type = TokenType::STRING;
                    result = Term(t);
                    return result;
                    };
                
            }

        }

        return func_result;
    }

    std::function<Term(const QList<Term>&)> comp_right(const QList<Term>& args)
    {
        std::function<Term(const QList<Term>&)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::NUMBER };
        int expected_num_of_args = 2;

        if (arg_types != expected_arg_types) {
            QString error = "right(string, length) expects 2 arguments(string, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "right(string, length) expects 2 arguments(string, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            QString str = args[0].get_token().string_value;
            double length = args[1].get_token().number_value;

            if (length < 0) {
                QString error = "right(string, length) invalid length on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                func_result = [length](const QList<Term>& args) {
                    Term result;
                    Token t;
                    QString str = args[0].get_token().string_value;
                    t.string_value = str.right(length);
                    t.line_number = args.front().get_token().line_number;
                    t.token_type = TokenType::STRING;
                    result = Term(t);
                    return result;
                    };
                
            }

        }
        return func_result;
    }

    std::function<Term(const QList<Term>&)> comp_strip_quotes(const QList<Term>& args) //remove doubles from column value surrounded by double quotes
    {
        std::function<Term(const QList<Term>&)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING };
        int expected_num_of_args = 1;

        if (arg_types != expected_arg_types) {
            QString error = "strip_quotes(string) expects 1 argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "strip_quotes(string) expects 1 argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            func_result = [](const QList<Term>& args) {
                Term result;
                QString str = args.front().get_token().string_value;

                if (str.front() == "\"") {
                    str.remove(0, 1); //remove first char
                }

                if (str.back() == "\"") {
                    str.chop(1); //remove last char
                }
                Token t;
                t.string_value = str;
                t.line_number = args.front().get_token().line_number;
                t.token_type = TokenType::STRING;
                result = Term(t);

                return result;
                };
            
        }

        return func_result;
    }

    std::function<Term(const QList<Term>&)> comp_date_gt(const QList<Term>& args) // date greater then
    {
        std::function<Term(const QList<Term>&)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str, format)
        int expected_num_of_args = 4;

        QList<TokenType> alt_expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str)
        int alt_expected_num_of_args = 3;

        if (arg_types != expected_arg_types && arg_types != alt_expected_arg_types) {
            QString error = "date_gt(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args && args.length() != alt_expected_num_of_args) {
            QString error = "date_gt(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            QString format1 = args[1].get_token().string_value;
            QString format2 = (args.length() == 4) ? args[3].get_token().string_value : format1;

            func_result = [format1, format2](const QList<Term>& args) {
                Term result;

                QString date_str1 = args[0].get_token().string_value;
                

                QString date_str2 = args[2].get_token().string_value;

                

                QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
                QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

                QString failure_str;
                if (!datetime1.isValid()) {
                    failure_str = date_str1;
                }
                else if (!datetime2.isValid()) {
                    failure_str = date_str2;
                }

                if (!failure_str.isEmpty()) {
                    QString error = "Failed to convert " + failure_str + " to a DateTime on line ";
                    error += QString::number(args.at(0).get_token().line_number);
                    throw std::logic_error(error.toStdString());
                }
                else {
                    Token t;
                    t.token_type = TokenType::BOOLEAN;
                    t.line_number = args.front().get_token().line_number;
                    t.boolean_value = (datetime1 > datetime2);
                    result = Term(t);
                }

                return result;
                };
            
        }

        return func_result;
    }

    std::function<Term(const QList<Term>&)> comp_date_lt(const QList<Term>& args) // date less than
    {
        std::function<Term(const QList<Term>&)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str, format)
        int expected_num_of_args = 4;

        QList<TokenType> alt_expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str)
        int alt_expected_num_of_args = 3;

        if (arg_types != expected_arg_types && arg_types != alt_expected_arg_types) {
            QString error = "date_lt(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args && args.length() != alt_expected_num_of_args) {
            QString error = "date_lt(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            QString format1 = args[1].get_token().string_value;
            QString format2 = (args.length() == 4) ? args[3].get_token().string_value : format1;

            func_result = [format1, format2](const QList<Term>& args) {
                Term result;
                QString date_str1 = args[0].get_token().string_value;
                

                QString date_str2 = args[2].get_token().string_value;


                

                QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
                QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

                QString failure_str;
                if (!datetime1.isValid()) {
                    failure_str = date_str1;
                }
                else if (!datetime2.isValid()) {
                    failure_str = date_str2;
                }

                if (!failure_str.isEmpty()) {
                    QString error = "Failed to convert " + failure_str + " to a DateTime on line ";
                    error += QString::number(args.at(0).get_token().line_number);
                    throw std::logic_error(error.toStdString());
                }
                else {
                    Token t;
                    t.token_type = TokenType::BOOLEAN;
                    t.boolean_value = (datetime1 < datetime2);
                    t.line_number = args.front().get_token().line_number;

                    result = Term(t);
                }

                return result;
                };
            
        }

        return func_result;
    }

    std::function<Term(const QList<Term>&)> comp_date_ge(const QList<Term>& args) // date greater than or equal
    {
        std::function<Term(const QList<Term>&)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str, format)
        int expected_num_of_args = 4;

        QList<TokenType> alt_expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str)
        int alt_expected_num_of_args = 3;

        if (arg_types != expected_arg_types && arg_types != alt_expected_arg_types) {
            QString error = "date_ge(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args && args.length() != alt_expected_num_of_args) {
            QString error = "date_ge(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            QString format1 = args[1].get_token().string_value;
            QString format2 = (args.length() == 4) ? args[3].get_token().string_value : format1;

            func_result = [format1, format2](const QList<Term>& args) {
                Term result;
                QString date_str1 = args[0].get_token().string_value;
                

                QString date_str2 = args[2].get_token().string_value;
                

                QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
                QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

                QString failure_str;
                if (!datetime1.isValid()) {
                    failure_str = date_str1;
                }
                else if (!datetime2.isValid()) {
                    failure_str = date_str2;
                }

                if (!failure_str.isEmpty()) {
                    QString error = "Failed to convert " + failure_str + " to a DateTime on line ";
                    error += QString::number(args.at(0).get_token().line_number);
                    throw std::logic_error(error.toStdString());
                }
                else {
                    Token t;
                    t.token_type = TokenType::BOOLEAN;
                    t.boolean_value = (datetime1 >= datetime2);
                    t.line_number = args.front().get_token().line_number;

                    result = Term(t);
                }
                return result;
                };

            
        }

        return func_result;
    }

    std::function<Term(const QList<Term>&)> comp_date_le(const QList<Term>& args) // date less than or equal
    {
        std::function<Term(const QList<Term>&)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str, format)
        int expected_num_of_args = 4;

        QList<TokenType> alt_expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str)
        int alt_expected_num_of_args = 3;

        if (arg_types != expected_arg_types && arg_types != alt_expected_arg_types) {
            QString error = "date_le(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args && args.length() != alt_expected_num_of_args) {
            QString error = "date_le(string, date_format, string2, date_format) expects 4 string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            QString format1 = args[1].get_token().string_value;
            QString format2 = (args.length() == 4) ? args[3].get_token().string_value : format1;

            func_result = [format1, format2](const QList<Term>& args) {
                Term result;

                QString date_str1 = args[0].get_token().string_value;


                QString date_str2 = args[2].get_token().string_value;




                QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
                QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

                QString failure_str;
                if (!datetime1.isValid()) {
                    failure_str = date_str1;
                }
                else if (!datetime2.isValid()) {
                    failure_str = date_str2;
                }

                if (!failure_str.isEmpty()) {
                    QString error = "Failed to convert " + failure_str + " to a DateTime on line ";
                    error += QString::number(args.at(0).get_token().line_number);
                    throw std::logic_error(error.toStdString());
                }
                else {
                    Token t;
                    t.token_type = TokenType::BOOLEAN;
                    t.boolean_value = (datetime1 <= datetime2);
                    t.line_number = args.front().get_token().line_number;

                    result = Term(t);
                }

                return result;
                };


        }

        return func_result;
    }

    std::function<Term(const QList<Term>&)> comp_date_eq(const QList<Term>& args) // date equal
    {
        std::function<Term(const QList<Term>&)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str, format)
        int expected_num_of_args = 4;

        QList<TokenType> alt_expected_arg_types = { TokenType::STRING, TokenType::STRING, TokenType::STRING }; //(date_str, format, date_str)
        int alt_expected_num_of_args = 3;

        if (arg_types != expected_arg_types && arg_types != alt_expected_arg_types) {
            QString error = "Incorrect use of date_eq(string, date_format, string2, date_format) or date_eq(string, date_format, string2) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }

        if (args.length() != expected_num_of_args && args.length() != alt_expected_num_of_args) {
            QString error = "Incorrect use of date_eq(string, date_format, string2, date_format) or date_eq(string, date_format, string2) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }

        QString format1 = args[1].get_token().string_value;
        QString format2 = (args.length() == 4) ? args[3].get_token().string_value : format1;

        func_result = [format1, format2](const QList<Term>& args) {
            Term result;

            QString date_str1 = args[0].get_token().string_value;


            QString date_str2 = args[2].get_token().string_value;

            QDateTime datetime1 = QDateTime::fromString(date_str1, format1);
            QDateTime datetime2 = QDateTime::fromString(date_str2, format2);

            QString failure_str;
            if (!datetime1.isValid()) {
                failure_str = date_str1;
            }
            else if (!datetime2.isValid()) {
                failure_str = date_str2;
            }

            if (!failure_str.isEmpty()) {
                QString error = "Failed to convert " + failure_str + " to a DateTime!";
                throw std::logic_error(error.toStdString());
            }
            else {
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = (datetime1 == datetime2);
                t.line_number = args.front().get_token().line_number;

                result = Term(t);
            }

            return result;
            };
        

        return func_result;
    }

    std::function<Term(const QList<Term>&)> comp_number(const QList<Term>& args) // convert string to number or throw exception
    {
        std::function<Term(const QList<Term>&)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }

        QList<TokenType> expected_arg_types = { TokenType::STRING };
        int expected_num_of_args = 1;

        if (arg_types != expected_arg_types) {
            QString error = "number(string) expects a string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }

        if (args.length() != expected_num_of_args) {
            QString error = "number(string) expects a string argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }

        func_result = [](const QList<Term>& args) {
            Term result;

            QString num_str = args[0].get_token().string_value;
            //qDebug() << "number string: " << num_str;
            bool is_number = false;
            double number = num_str.toDouble(&is_number);

            if (is_number) {
                Token t;
                t.token_type = TokenType::NUMBER;
                t.number_value = number;
                t.string_value = num_str;
                t.line_number = args.at(0).get_token().line_number;
                result = Term(t);
            }
            else {

                Token t;
                t.token_type = TokenType::NUMBER;
                t.number_value = 0;
                t.string_value = num_str;
                t.line_number = args.at(0).get_token().line_number;
                result = Term(t);

                /*
                QString error = "String '";
                error += num_str;
                error += "' passed to number(string) on line ";
                error += QString::number(args.at(0).get_token().line_number);
                error += " is not a number!";
                throw std::logic_error(error.toStdString());
                */
            }

            return result;
            };

        

        return func_result;
    }

    //enum class AggregFuncType { count, sum, min, max, avg };
    std::function<Term(const QList<Term>& args)> comp_execute_aggregation(const QList<Term>& args, const AggregFuncType& func_type)
    {
        std::function<Term(const QList<Term>& args)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
            //qDebug() << "count args " << t.get_token().to_string();
        }

        QList<TokenType> expected_arg_types1 = { TokenType::STRING, TokenType::FUNCTION };
        QList<TokenType> expected_arg_types2 = { TokenType::NUMBER, TokenType::FUNCTION };
        int expected_num_of_args = 2;


        if (arg_types != expected_arg_types1 && arg_types != expected_arg_types2) {
            QString error = "count() expects a string or number argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "count() expects 1 argument on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            // retrieve counter
            // check if already updated for this count and parameter
            // if so return counter without updating
            // else update counter and return new value
            
            /*
            QString data_to_count;
            if (args.at(0).get_token().token_type == TokenType::STRING) {
                data_to_count = args.at(0).get_token().string_value;
            }
            else if (args.at(0).get_token().token_type == TokenType::NUMBER) {
                data_to_count = QString::number(args.at(0).get_token().number_value);
            }
            */

            func_result = [/* data_to_count, */ func_type](const QList<Term> args) {
                Term result;

                QString data_to_count;
                if (args.at(0).get_token().token_type == TokenType::STRING) {
                    data_to_count = args.at(0).get_token().string_value;
                }
                else if (args.at(0).get_token().token_type == TokenType::NUMBER) {
                    data_to_count = QString::number(args.at(0).get_token().number_value);
                }

                QString aggregate_name = args.at(1).get_token().string_value;

                //QMap<QString, std::shared_ptr<AggregateCounter>> aggregate_expression_reg = {};
                //QMap<QString, bool> check_if_aggregate_done = {};

                //qDebug() << "aggregate key is " << aggregate_expression_reg_key;

                //if (aggregate_expression_reg[aggregate_expression_reg_key].contains(aggregate_name)) {
                if (aggregate_expression_reg.contains(aggregate_expression_reg_key+"|" + aggregate_name)) {
                    //std::shared_ptr<AggregateCounter>& count_counter = aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name];
                    std::shared_ptr<AggregateCounter>& count_counter = aggregate_expression_reg[aggregate_expression_reg_key+"|" + aggregate_name];

                    bool function_called_already = false;

                    //if (check_if_aggregate_done[aggregate_expression_reg_key].contains(aggregate_name)) {
                    if (check_if_aggregate_done.contains(aggregate_expression_reg_key+"|" + aggregate_name)) {
                        //function_called_already = check_if_aggregate_done[aggregate_expression_reg_key][aggregate_name];
                        function_called_already = check_if_aggregate_done[aggregate_expression_reg_key+"|" + aggregate_name];
                    }


                    if (function_called_already) { // called already in another column
                        Token t = args.at(1).get_token();
                        t.token_type = TokenType::NUMBER;
                        t.number_value = count_counter->get_value();
                        t.string_value = QString::number(count_counter->get_value());
                        t.line_number = args.front().get_token().line_number;

                        result = Term(t);
                        //qDebug() << "[called already] current count: " << count_counter->get_value();

                    }
                    else {
                        /*
                        if (args.at(0).get_token().token_type == TokenType::STRING) {
                            count_counter->process_data(args.at(0).get_token().string_value);
                        }
                        else if (args.at(0).get_token().token_type == TokenType::NUMBER) {
                            count_counter->process_data(QString::number(args.at(0).get_token().number_value));
                        }*/

                        count_counter->process_data(data_to_count);

                        Token t = args.at(1).get_token();
                        t.token_type = TokenType::NUMBER;
                        t.number_value = count_counter->get_value();
                        t.string_value = QString::number(count_counter->get_value());
                        t.line_number = args.front().get_token().line_number;

                        result = Term(t);

                        //check_if_aggregate_done[aggregate_expression_reg_key][aggregate_name] = true; // skip aggregation if same aggregate function and parameter is called
                        check_if_aggregate_done[aggregate_expression_reg_key+"|" + aggregate_name] = true; // skip aggregation if same aggregate function and parameter is called

                        //qDebug() << "current count: " << count_counter->get_value();
                    }

                }
                else {// first time this aggregation is being called with this parameter
                    if (func_type == AggregFuncType::count) {
                        //aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<CountCounter>();
                        aggregate_expression_reg[aggregate_expression_reg_key+"|" + aggregate_name] = std::make_shared<CountCounter>();
                    }
                    else if (func_type == AggregFuncType::avg) {
                        //aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<AvgCounter>();
                        aggregate_expression_reg[aggregate_expression_reg_key+"|" + aggregate_name] = std::make_shared<AvgCounter>();
                    }
                    else if (func_type == AggregFuncType::sum) {
                        //aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<SumCounter>();
                        aggregate_expression_reg[aggregate_expression_reg_key+"|" + aggregate_name] = std::make_shared<SumCounter>();
                    }
                    else if (func_type == AggregFuncType::min) {
                        //aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<MinCounter>();
                        aggregate_expression_reg[aggregate_expression_reg_key+"|" + aggregate_name] = std::make_shared<MinCounter>();
                    }
                    else if (func_type == AggregFuncType::max) {
                        //aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<MaxCounter>();
                        aggregate_expression_reg[aggregate_expression_reg_key+"|" + aggregate_name] = std::make_shared<MaxCounter>();
                    }
                    else {
                        QString error = "Internal error executing aggregate function on line ";
                        error += QString::number(args.at(0).get_token().line_number);
                        throw std::logic_error(error.toStdString());
                    }


                    //std::shared_ptr<AggregateCounter>& count_counter = aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name];
                    std::shared_ptr<AggregateCounter>& count_counter = aggregate_expression_reg[aggregate_expression_reg_key+"|" + aggregate_name];

                    /*
                    if (args.at(0).get_token().token_type == TokenType::STRING) {
                        count_counter->process_data(args.at(0).get_token().string_value);
                    }
                    else if (args.at(0).get_token().token_type == TokenType::NUMBER) {
                        count_counter->process_data(QString::number(args.at(0).get_token().number_value));
                    }
                    else {
                        //qDebug() << "unknown type";
                    }*/

                    count_counter->process_data(data_to_count);

                    Token t = args.at(1).get_token();
                    t.token_type = TokenType::NUMBER;
                    t.number_value = count_counter->get_value();
                    t.string_value = QString::number(count_counter->get_value());
                    t.line_number = args.front().get_token().line_number;

                    result = Term(t);

                    //check_if_aggregate_done[aggregate_expression_reg_key][aggregate_name] = true; // skip aggregation if same aggregate function and parameter is called
                    check_if_aggregate_done[aggregate_expression_reg_key+"|" + aggregate_name] = true; // skip aggregation if same aggregate function and parameter is called
                    //qDebug() << "current count: " << count_counter->get_value();
                }

                return result;
                };

            
        }
        return func_result;
    }

    std::function<Term(const QList<Term>&)> comp_count(const QList<Term>& args) //aggregate function
    {
        //qDebug() << "compiling count";
        auto f =  comp_execute_aggregation(args, AggregFuncType::count);
        //qDebug() << "Done compiling count";

        return f;
    }

    std::function<Term(const QList<Term>&)> comp_sum(const QList<Term>& args) //aggregate function
    {
        return comp_execute_aggregation(args, AggregFuncType::sum);
    }

    std::function<Term(const QList<Term>&)> comp_min(const QList<Term>& args) //aggregate function
    {
        return comp_execute_aggregation(args, AggregFuncType::min);
    }

    std::function<Term(const QList<Term>&)> comp_max(const QList<Term>& args) //aggregate function
    {
        return comp_execute_aggregation(args, AggregFuncType::max);
    }

    std::function<Term(const QList<Term>&)> comp_avg(const QList<Term>& args) //aggregate function
    {
        return comp_execute_aggregation(args, AggregFuncType::avg);
    }

    std::function<Term(const QList<Term>&)> comp_modulo(const QList<Term>& args)
    {
        std::function<Term(const QList<Term>&)> func_result;

        QList<TokenType> arg_types;
        foreach(auto t, args) {
            arg_types.append(t.get_token().token_type);
        }


        QList<TokenType> expected_arg_types1 = { TokenType::STRING, TokenType::NUMBER };
        QList<TokenType> expected_arg_types2 = { TokenType::STRING, TokenType::STRING };
        QList<TokenType> expected_arg_types3 = { TokenType::NUMBER, TokenType::NUMBER };
        int expected_num_of_args = 2;

        if (args.length() != expected_num_of_args) {
            QString error = "modulo(string, number) or modulo(string, string) expects 2 arguments (string, number) or (string, string) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }

        

        if (arg_types == expected_arg_types1) { //String, Number

            int denominator = args[1].get_token().number_value;

            if (denominator == 0) {
                QString error = "Division by zero in modulo(string, number) on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }

            func_result = [denominator](const QList<Term>& args) {

                Term result;

                QString num_str = args[0].get_token().string_value;
                bool is_number = false;
                int numerator = num_str.toInt(&is_number);

                if (!is_number) {
                    QString error = "Unable to convert numerator argument passed to modulo(string, number) to an integer on line ";
                    error += QString::number(args.at(0).get_token().line_number);
                    throw std::logic_error(error.toStdString());
                }
                
                int mod_result = (numerator % denominator + denominator) % denominator;//std::fmod(numerator, denominator);
                Token t;
                t.token_type = TokenType::NUMBER;
                t.number_value = mod_result;
                t.string_value = QString::number(mod_result);
                t.line_number = args.at(0).get_token().line_number;
                    
                result = Term(t);
				return result;

             };

            
        }
        else if (arg_types == expected_arg_types2) { // both arguments are strings, convert both to numbers
            QString denom_str = args[1].get_token().string_value;
            bool is_denominator_number = false;
            int denominator = denom_str.toInt(&is_denominator_number);

            if (!is_denominator_number) {
                QString error = "Unable to convert an denominator passed to modulo(string, string) to an integer on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }

            if (denominator == 0) {
                QString error = "Division by zero in modulo(string, string) on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }

            func_result = [denominator](const QList<Term>& args) {
				Term result;
                QString num_str = args[0].get_token().string_value;
                bool is_number = false;
                int numerator = num_str.toInt(&is_number);

                if (!is_number) {
                    QString error = "Unable to convert numerator argument passed to modulo(string, string) to an integer on line ";
                    error += QString::number(args.at(0).get_token().line_number);
                    throw std::logic_error(error.toStdString());
                }

                double mod_result = (numerator % denominator + denominator) % denominator;
                Token t;
                t.token_type = TokenType::NUMBER;
                t.number_value = mod_result;
                t.string_value = QString::number(mod_result);
                t.line_number = args.at(0).get_token().line_number;
                result = Term(t);

				return result;

                };

            
        }
        else if (arg_types == expected_arg_types3) { //both are Numbers


            int denominator = args[1].get_token().number_value;

            if (denominator == 0) {
                QString error = "Division by zero in modulo(string, string) on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }

            func_result = [denominator](const QList<Term>& args) {
                Term result;
                int numerator = args[0].get_token().number_value;

                double mod_result = (numerator % denominator + denominator) % denominator;
                Token t;
                t.token_type = TokenType::NUMBER;
                t.number_value = mod_result;
                t.string_value = QString::number(mod_result);
                t.line_number = args.at(0).get_token().line_number;
                result = Term(t);

                return result;

                };
        }
        else {
            QString error = "modulo(string, number) or modulo(string, string) expects 2 arguments (string, number) or (string, string) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }

        return func_result;
    }

    /*
    std::function<Term(const QList<Term>&)> comp_case_function(const QList<Term>& args)
    {
        if (args.size() < 3) {
            throw std::logic_error("Invalid CASE expression");
        }

        const TokenType case_type = args.at(1).get_token_type();

        bool has_default = false;
        Term default_result;

        int i = 1;

        if (case_type == TokenType::NUMBER) {
            QHash<double, Term> lookup;

            while (i + 1 < args.size()) {
                const Term& value = args.at(i);
                const Term& result = args.at(i + 1);

                if (value.get_token_type() != TokenType::NUMBER) {
                    throw std::logic_error("Mixed WHEN types in CASE function");
                }

                double key = value.get_token().number_value;

                if (!lookup.contains(key)) {
                    lookup.insert(key, result);
                }

                i += 2;
            }

            if (i < args.size()) {
                has_default = true;
                default_result = args.at(i);
            }

            return [lookup, has_default, default_result](const QList<Term>& runtime_args) -> Term {
                if (runtime_args.isEmpty()) {
                    throw std::logic_error("CASE function missing runtime key");
                }

                const Term& key = runtime_args.at(0);

                if (key.get_token_type() != TokenType::NUMBER) {
                    throw std::logic_error("Type mismatch in CASE function");
                }

                auto it = lookup.constFind(key.get_token().number_value);
                if (it != lookup.constEnd()) {
                    return it.value();
                }

                if (has_default) {
                    return default_result;
                }

                Token t;
                t.token_type = TokenType::STRING;
                t.string_value = "";
                t.line_number = key.get_token().line_number;
                return Term(t);
                };
        }

        if (case_type == TokenType::STRING) {
            QHash<QString, Term> lookup;

            while (i + 1 < args.size()) {
                const Term& value = args.at(i);
                const Term& result = args.at(i + 1);

                if (value.get_token_type() != TokenType::STRING) {
                    throw std::logic_error("Mixed WHEN types in CASE function");
                }

                QString key = value.get_token().string_value;

                if (!lookup.contains(key)) {
                    lookup.insert(key, result);
                }

                i += 2;
            }

            if (i < args.size()) {
                has_default = true;
                default_result = args.at(i);
            }

            return [lookup, has_default, default_result](const QList<Term>& runtime_args) -> Term {
                if (runtime_args.isEmpty()) {
                    throw std::logic_error("CASE function missing runtime key");
                }

                const Term& key = runtime_args.at(0);

                if (key.get_token_type() != TokenType::STRING) {
                    throw std::logic_error("Type mismatch in CASE function");
                }

                auto it = lookup.constFind(key.get_token().string_value);
                if (it != lookup.constEnd()) {
                    return it.value();
                }

                if (has_default) {
                    return default_result;
                }

                Token t;
                t.token_type = TokenType::STRING;
                t.string_value = "";
                t.line_number = key.get_token().line_number;
                return Term(t);
                };
        }

        throw std::logic_error("Unsupported CASE key type");
    }
    */

    std::function<Term(const QList<Term>&)> comp_case_function(const QList<Term>& args)
    {
        if (args.size() < 3) {
            throw std::logic_error("Invalid CASE expression");
        }
    
        const Term& first_when = args.at(1);
        const TokenType case_type = first_when.get_token_type();
    
        QHash<double, Term> number_lookup;
        QHash<QString, Term> string_lookup;
    
        bool has_default = false;
        Term default_result;
    
        int i = 1;
    
        while (i + 1 < args.size()) {
            const Term& value = args.at(i);
            const Term& result = args.at(i + 1);
    
            if (value.get_token_type() != case_type) {
                QString error = "Mixed WHEN types in CASE function on line ";
                error += QString::number(value.get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
    
            if (case_type == TokenType::NUMBER) {
                double key = value.get_token().number_value;
    
                // Preserve SQL CASE behavior: first matching WHEN wins.
                if (!number_lookup.contains(key)) {
                    number_lookup.insert(key, result);
                }
            }
            else if (case_type == TokenType::STRING) {
                QString key = value.get_token().string_value.toLower();
    
                if (!string_lookup.contains(key)) {
                    string_lookup.insert(key, result);
                }
            }
            else {
                throw std::logic_error("Unsupported CASE key type");
            }
    
            i += 2;
        }
    
        if (i < args.size()) {
            has_default = true;
            default_result = args.at(i);
        }
    
        return [case_type, number_lookup, string_lookup, has_default, default_result]
        (const QList<Term>& runtime_args) -> Term
            {
                if (runtime_args.isEmpty()) {
                    throw std::logic_error("CASE function missing runtime key");
                }
    
                const Term& key = runtime_args.at(0);
    
                if (key.get_token_type() != case_type) {
                    QString error = "Type mismatch in CASE function on line ";
                    error += QString::number(key.get_token().line_number);
                    throw std::logic_error(error.toStdString());
                }
    
                if (case_type == TokenType::NUMBER) {
                    auto it = number_lookup.constFind(key.get_token().number_value);
                    if (it != number_lookup.constEnd()) {
                        return it.value();
                    }
                }
                else if (case_type == TokenType::STRING) {
                    auto it = string_lookup.constFind(key.get_token().string_value.toLower());
                    if (it != string_lookup.constEnd()) {
                        return it.value();
                    }
                }
    
                if (has_default) {
                    return default_result;
                }
    
                Token t;
                t.token_type = TokenType::STRING;
                t.string_value = "";
                t.line_number = key.get_token().line_number;
                return Term(t);
            };
    }

}