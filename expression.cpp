#include "expression.h"
#include <stdexcept>
#include <iostream>
#include <QMap>

Expression::Expression(const QList<Term>& ts)
    : terms{ts}
{
    current_term = terms.begin();
    item_left = (terms.length() == 0)? 0 : terms.length() - 1;

    end_of_experssion["TokenType::AND"] = terms.end();
    end_of_experssion["TokenType::OR"] = terms.end();
    end_of_experssion["TokenType::LESSTHAN"] = terms.end();
    end_of_experssion["TokenType::GREATERTHAN"] = terms.end();
    end_of_experssion["TokenType::LESSTHANOREQUAL"] = terms.end();
    end_of_experssion["TokenType::GREATERTHANOREQUAL"] = terms.end();
    end_of_experssion["TokenType::NOTEQUALTO"] = terms.end();
}

 QList<Term>::iterator Expression::end() const
{
     if(current_term == terms.end()){
        return current_term;
     }

     if(end_of_experssion.contains(current_term->get_token().to_string())){
         return end_of_experssion[current_term->get_token().to_string()];
     }

     return current_term;
}

void Expression::move_to_next_term()
{
    if(current_term == terms.end()){
        return;
    }

    //auto n = current_term;

    if(item_left == 0){
        current_term = terms.end();
        return;
    }

    ++current_term;
    --item_left;
    //return n;
}

QList<Term>::iterator Expression::peak_next_term() {
    QList<Term>::iterator n = current_term + 1;
    return n;
}

QList<Term>::iterator Expression::get_current_term() const{
    return current_term;
}

bool Expression::iscolumn_or_literal(Token t) const{
    if (t.token_type == TokenType::COLUMNNAME){
        return true;
    }

    if(t.token_type == TokenType::COLUMNNUMBER){
        return true;
    }

    if(t.token_type == TokenType::STRING){
        return true;
    }

    if(t.token_type == TokenType::NUMBER){
        return true;
    }

    return false;
}

double Expression::add(double num1, double num2){
    return num1 + num2;
}

QString Expression::add(double num1, QString str){
    return QString::number(num1) + str;
}

QString Expression::add(QString str, double num2){
    return str + QString::number(num2);
}

QString Expression::add(QString str, QString str2){
    return str + str2;
}

Term Expression::mult(Term left, Term right){
    Term result;

    if(left.get_token_type() ==TokenType::NUMBER && right.get_token_type() ==TokenType::NUMBER){
        Token t {.token_type=TokenType::NUMBER, .token_name="TokenType::NUMBER"};
        t.number_value = left.get_token().number_value * right.get_token().number_value;
        t.line_number = left.get_token().line_number;
        result = Term(t);
    }
    else{
        QString error = "cannot multiply types on line " + QString::number(left.get_token().line_number);
        throw std::logic_error(error.toStdString());
    }
    return result;
}

Term Expression::div(Term left, Term right){
    Term result;

    if(left.get_token_type() ==TokenType::NUMBER && right.get_token_type() ==TokenType::NUMBER){
        Token t {.token_type=TokenType::NUMBER, .token_name="TokenType::NUMBER"};

        if(right.get_token().number_value == 0){
            //result.token_type = TokenType::ERROR;
            QString error = "divide by zero error on line "+ QString::number(left.get_token().line_number);
            throw std::logic_error(error.toStdString());
        }else{
            t.number_value = left.get_token().number_value / right.get_token().number_value;
            t.line_number = left.get_token().line_number;
            result = Term(t);
        }
    }
    else{
        //result.token_type = TokenType::ERROR;
        QString error = "cannot divide types on line "+ QString::number(left.get_token().line_number);
        throw std::logic_error(error.toStdString());
    }
    return result;
}


Term Expression::minus(Term left, Term right){
    Term result;

    if(left.get_token_type() ==TokenType::NUMBER && right.get_token_type() ==TokenType::NUMBER){
        Token t {.token_type=TokenType::NUMBER, .token_name="TokenType::NUMBER"};

        t.number_value = left.get_token().number_value - right.get_token().number_value;
        t.line_number = left.get_token().line_number;
        result = Term(t);
    }
    else{
        //result.token_type = TokenType::ERROR;
        QString error = "cannot subtract values on line "+ QString::number(left.get_token().line_number);
        throw std::logic_error(error.toStdString());
    }
    return result;
}


Term Expression::add(Term left, Term right){

    Term result;

    if(left.get_token_type() ==TokenType::STRING){
        if(right.get_token_type() ==TokenType::STRING){
            Token t {.token_type=TokenType::STRING, .token_name="TokenType::STRING"};

            t.string_value = add(left.get_token().string_value, right.get_token().string_value);
            t.line_number = left.get_token().line_number;
            result = Term(t);
            //return result;
        }
        else{
            Token t {.token_type=TokenType::STRING, .token_name="TokenType::STRING"};
            t.string_value = add(left.get_token().string_value, right.get_token().number_value);
            t.line_number = left.get_token().line_number;
            result = Term(t);
            //return result;
        }
    }
    else if(left.get_token_type() ==TokenType::NUMBER){
        if(right.get_token_type() ==TokenType::STRING){
            Token t {.token_type=TokenType::STRING, .token_name="TokenType::STRING"};
            t.string_value = add(left.get_token().number_value, right.get_token().string_value);
            t.line_number = left.get_token().line_number;
            result = Term(t);
            //return result;
        }
        else{
            Token t {.token_type=TokenType::NUMBER, .token_name="TokenType::NUMBER"};

            t.number_value = add(left.get_token().number_value, right.get_token().number_value);
            t.line_number = left.get_token().line_number;
            result = Term(t);
            //return result;
        }
    }
    else{
        //result.token_type = TokenType::ERROR;
        QString error = "cannot add types on line "+ QString::number(left.get_token().line_number);;
        throw std::logic_error(error.toStdString());
    }

    return result;

}

Term Expression::expr(const QMap<QString, QStringList>& data_rows, bool get)
{
    Term left = term(data_rows, get);

    //std::cout<<"expr() left: "<<left.get_token().number_value<<"\n";

    while(current_term != terms.end()){ // read and compute all terms in column expression
        //std::cout<<"expr() current term: "<<current_term->get_token().to_string().toStdString()<<"\n";

        if(current_term->get_token().token_type == TokenType::PLUS){
            left = add(left, term(data_rows, true));
        }
       else if(current_term->get_token().token_type == TokenType::MINUS){
            left = minus(left, term(data_rows, true));
        }
        else{
            break;
        }
    }

    return left;
}


Term Expression::term(const QMap<QString, QStringList>& data_rows, bool get)
{
    Term left = primary(data_rows, get);

    //std::cout<<"term() current left: "<<left.get_token().to_string().toStdString()<<"\n";

    while(current_term != terms.end()){
        //std::cout<<"In term() loop!\n";
        //std::cout<<"item pos "<<get_iterator_pos()<<"\n";


        if(current_term->get_token().token_type == TokenType::MULT){
            left = mult(left, primary(data_rows, true));
        }
        else if(current_term->get_token().token_type == TokenType::DIV){
            left = div(left, primary(data_rows, true));
        }
        else{
            break;
        }
    }

    return left;
}


Term Expression::primary(const QMap<QString, QStringList>& data_rows, bool get)
{
    if(get){
        move_to_next_term();
    }
    Term left = *get_current_term();

    //auto left_column_term = get_column_term(true);
    Token left_token = left.get_token();


    //left = left_column_term.eval(row);


    if(left_token.token_type == TokenType::NUMBER){
        move_to_next_term();

    }
    else if(left_token.token_type == TokenType::STRING){
        move_to_next_term();
    }
    else if(left_token.token_type == TokenType::NAME){
        Token token = left_token;

        if(!symbol_table.contains(token.string_value.toLower())){

            QStringList name_parts = token.string_value.split('.');

            if(name_parts.size() == 2){ //check if name is format file.number
                bool is_number;
                double number = name_parts[1].toDouble(&is_number);
                if(is_number){
                    left_token.token_type = TokenType::COLUMNNAME;
                    left_token.number_value = number;
                    //left_token.string_value = name_parts[0];

                    Term t(left_token);
                    left = t;
                    left = left.eval(data_rows);
                    move_to_next_term();
                }
                else if(name_parts[1] == "*"){
                    left = left.eval(data_rows);
                    move_to_next_term();
                }
                else{
                    //error
                    std::string error = "Uknown name on line ";
                    error += QString::number(token.line_number).toStdString();
                    throw std::logic_error(error);
                }
            }
            else{
                //error
                std::string error = "Uknown name on line ";
                error += QString::number(token.line_number).toStdString();
                throw std::logic_error(error);
            }

            //QString error = "Unknown variable on line "+ QString::number(token.line_number);
            //throw std::logic_error(error.toStdString());
        }
        else{
            TokenType name_type = symbol_table[token.string_value.toLower()];
            if(name_type == TokenType::STRING){
                QString string_value = strings_table[token.string_value.toLower()];
                token.string_value = string_value;
                token.token_type = TokenType::STRING;
                token.token_name = "TokenType::STRING";

                left = Term(token);
                move_to_next_term();
            }
            else if(name_type == TokenType::NUMBER){
                double number_value = numbers_table[token.string_value.toLower()];
                token.number_value = number_value;
                token.token_type = TokenType::NUMBER;
                token.token_name = "TokenType::NUMBER";

                left = Term(token);
                move_to_next_term();
            }
            else if(name_type == TokenType::FUNCTION){
                //std::cout<<"Expression::primary() : checking if this is reached...\n";
            }
        }
    }
    else if(left_token.token_type == TokenType::COLUMNNAME){
        left = left.eval(data_rows);
        move_to_next_term();
        //left.line_number = left_column_term.get_line_number();
    }
    else if(left_token.token_type == TokenType::COLUMNNUMBER){
        left = left.eval(data_rows);
        move_to_next_term();
        //left.line_number = left_column_term.get_line_number();
    }
    else if(left_token.token_type == TokenType::MINUS){
        Token t = {.token_type = TokenType::NUMBER, .string_value="-1", .number_value=-1, .line_number=left_token.line_number, .token_name="TokenType::NUMBER"};
        Term neg_1(t);

        left = mult(neg_1, primary(data_rows, true));
        //left.line_number = left_column_term.get_line_number();
    }
    else if(left_token.token_type == TokenType::FUNCTION){
        std::function<Term(QList<Term>)> f= left_token.func;

        //std::cout<<"Expression::primary() : evaluating function \n";
        QList<Token> func_args_tokens = left_token.func_args;
        QList<Expression> exprs;
        //Expression exp{};
        QList<Term> ts;
        //std::cout<<"print function tokens...\n";
        foreach(auto t, func_args_tokens){
            Term et(t);
            //std::cout<<"arg: "<<t.to_string().toStdString()<<"\n";
            if(t.token_type == TokenType::COMMA){
                Expression exp(ts);
                exprs.append(exp);
                ts = {};
            }
            ts.append(et);
        }
        Expression exp(ts);
        exprs.append(exp); //add last expression

        QList<Term> arg_results;
        foreach(auto ce, exprs){
            auto r = ce.eval(data_rows);
            arg_results.append(r);
        }

        //call function
        left = f(arg_results);
        move_to_next_term();
    }
    else if(left_token.token_type == TokenType::LBRACKET){
        auto e = expr(data_rows, true);

        if(current_term->get_token().token_type != TokenType::RBRACKET){
            QString error = "Expected a ) on line "+ QString::number(current_term->get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else{
            left = e;
            move_to_next_term(); //eat )
        }
    }
    //else if(end_of_experssion.contains(left_token.to_string())){
        //don't do anything return
    //}
    else {
        //std::cout<<"Unexpected token "<< left_token.to_string().toStdString()<<"\n";
        QString error = "$Syntax error on line "+ QString::number(current_term->get_token().line_number);
        throw std::logic_error(error.toStdString());
    }
    return left;
}

Term Expression::eval(const QMap<QString, QStringList>& data_rows ){

    if(is_star()){

        Term res = eval_star_term(data_rows);
        reset_iterators();
        return res;
    }

    Token t = {.token_type =  TokenType::END, .token_name = "TokenType::END"};
    Term result(t);

    //next_term();

    result = expr(data_rows, false);

    /*
    while(current_term != terms.end()){
        if(current_term->get_token().token_type == TokenType::END){
            break;
        }
        result = expr(row, false);
        next_term();
    }*/

    reset_iterators();

    return result;

}


