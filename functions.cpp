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
            QString error = "left(string, length) expects 2 arguments(string, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else if (args.length() != expected_num_of_args) {
            QString error = "left(string, length) expects 2 arguments(string, number) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else {
            //qDebug() << "arg1: " << args[0].get_token().to_string() <<" value: "<< args[0].get_token().string_value;
                //qDebug() << "arg2: " << args[1].get_token().to_string() <<" value: "<< args[1].get_token().number_value;

            std::any arg1 = args[0].get_token().string_value;
            std::any arg2 = args[1].get_token().number_value;
            QString str = std::any_cast<QString>(arg1);
            double length = std::any_cast<double>(arg2);

            if (length < 0) {
                QString error = "left(string, length) invalid length on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                Token t;
                t.string_value = str.left(length);
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
            QString str = std::any_cast<QString>(args[0].get_token().string_value);
            double length = std::any_cast<double>(args[1].get_token().number_value);

            if (length < 0) {
                QString error = "right(string, length) invalid length on line ";
                error += QString::number(args.at(0).get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                Token t;
                t.string_value = str.right(length);
                t.token_type = TokenType::STRING;
                result = Term(t);
            }
            
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
            QString error = "String passed to number(string) on line ";
            error += QString::number(args.at(0).get_token().line_number);
            error += " is not a number!";
            throw std::logic_error(error.toStdString());
        }
        
        return result;
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

            if (aggregate_expression_reg[aggregate_expression_reg_key].contains(aggregate_name)) {
                std::shared_ptr<AggregateCounter>& count_counter = aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name];

                bool function_called_already = false;

                if (check_if_aggregate_done[aggregate_expression_reg_key].contains(aggregate_name)) {
                    function_called_already = check_if_aggregate_done[aggregate_expression_reg_key][aggregate_name];
                }


                if (function_called_already) { // called already in another column
                    Token t = args.at(1).get_token();
                    t.token_type = TokenType::NUMBER;
                    t.number_value = count_counter->get_value();
                    t.string_value = QString::number(count_counter->get_value());

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

                    result = Term(t);

                    check_if_aggregate_done[aggregate_expression_reg_key][aggregate_name] = true; // skip aggregation if same aggregate function and parameter is called

                    //qDebug() << "current count: " << count_counter->get_value();
                }

            }
            else {// first time this aggregation is being called with this parameter
                if (func_type == AggregFuncType::count) {
                    aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<CountCounter>();
                }
                else if (func_type == AggregFuncType::avg) {
                    aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<AvgCounter>();
                }
                else if (func_type == AggregFuncType::sum) {
                    aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<SumCounter>();
                }
                else if (func_type == AggregFuncType::min) {
                    aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<MinCounter>();
                }
                else if (func_type == AggregFuncType::max) {
                    aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name] = std::make_shared<MaxCounter>();
                }
                else {
                    QString error = "Internal error executing aggregate function on line ";
                    error += QString::number(args.at(0).get_token().line_number);
                    throw std::logic_error(error.toStdString());
                }
                

                std::shared_ptr<AggregateCounter>& count_counter = aggregate_expression_reg[aggregate_expression_reg_key][aggregate_name];

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

                result = Term(t);

                check_if_aggregate_done[aggregate_expression_reg_key][aggregate_name] = true; // skip aggregation if same aggregate function and parameter is called
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
    
}