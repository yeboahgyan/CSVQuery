#include "conditionalexpression.h"
#include <stdexcept>
#include <iostream>
#include "term.h"

ConditionalExpression::ConditionalExpression(const QList<Term>& ts)
    : terms(ts)
{
    current_term = terms.begin();
    item_left = (terms.length() == 0)? 0 : terms.length() - 1;
}

void ConditionalExpression::move_to_next_term()
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

QList<Term>::iterator ConditionalExpression::get_current_term() const{
    return current_term;
}

QList<Term>::iterator ConditionalExpression::peak_next_term() {
    QList<Term>::iterator n = current_term + 1;
    return n;
}

//Where clause
Term ConditionalExpression::or_op(Term left, Term right) // logical OR
{
    Term result;

    if(left.get_token().token_type == TokenType::NUMBER){
        if(right.get_token().token_type != TokenType::NUMBER){
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a number!";
            throw std::logic_error(error.toStdString());
        }

    }
    else if(left.get_token().token_type == TokenType::STRING){
        if(right.get_token().token_type != TokenType::STRING){
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a string!";
            throw std::logic_error(error.toStdString());
        }
    }
    else if(left.get_token().token_type == TokenType::BOOLEAN){
        if(right.get_token().token_type != TokenType::BOOLEAN){
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a boolean!";
            throw std::logic_error(error.toStdString());
        }
    }

    Token t;
    t.token_type = TokenType::BOOLEAN;
    t.boolean_value = left.get_token().boolean_value || right.get_token().boolean_value;
    result = Term(t);

    return result;
}

Term ConditionalExpression::and_op(Term left, Term right) // logical AND
{
    Term result;

    if(left.get_token().token_type == TokenType::BOOLEAN){
        if(right.get_token().token_type != TokenType::BOOLEAN){
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a boolean!";
            throw std::logic_error(error.toStdString());
        }

        Token t;
        t.token_type = TokenType::BOOLEAN;
        t.boolean_value = left.get_token().boolean_value && right.get_token().boolean_value;
        result = Term(t);
    }
    else{
        QString error = "Left operand of AND on line ";
        error += QString::number(right.get_line_number());
        error+= " should be a boolean!";
        throw std::logic_error(error.toStdString());
    }

    return result;
}

Term ConditionalExpression::eq(Term left, Term right) // Equal
{
    Term result;

    Token t;
    t.token_type = TokenType::BOOLEAN;

    if(left.get_token().token_type == TokenType::NUMBER){
        if(right.get_token().token_type != TokenType::NUMBER){
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a number!";
            throw std::logic_error(error.toStdString());
        }
        t.boolean_value = left.get_token().number_value == right.get_token().number_value;
    }
    else if(left.get_token().token_type == TokenType::STRING){
        if(right.get_token().token_type != TokenType::STRING){
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a string!";
            throw std::logic_error(error.toStdString());
        }

        t.boolean_value = left.get_token().string_value == right.get_token().string_value;
    }
    else if(left.get_token().token_type == TokenType::BOOLEAN){
        if(right.get_token().token_type != TokenType::BOOLEAN){
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a boolean!";
            throw std::logic_error(error.toStdString());
        }
        t.boolean_value = left.get_token().boolean_value == right.get_token().boolean_value;
    }
    else{
        QString error = "Unexpected left operand on line ";
        error += QString::number(right.get_line_number());
        throw std::logic_error(error.toStdString());
    }

    result = Term(t);

    return result;
}

Term ConditionalExpression::neq(Term left, Term right) // Not Equal
{
    Term result;

    Token t;
    t.token_type = TokenType::BOOLEAN;

    if(left.get_token().token_type == TokenType::NUMBER){
        if(right.get_token().token_type != TokenType::NUMBER){
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a number!";
            throw std::logic_error(error.toStdString());
        }
        t.boolean_value = left.get_token().number_value != right.get_token().number_value;
    }
    else if(left.get_token().token_type == TokenType::STRING){
        if(right.get_token().token_type != TokenType::STRING){
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a string!";
            throw std::logic_error(error.toStdString());
        }

        t.boolean_value = left.get_token().string_value != right.get_token().string_value;
    }
    else if(left.get_token().token_type == TokenType::BOOLEAN){
        if(right.get_token().token_type != TokenType::BOOLEAN){
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a boolean!";
            throw std::logic_error(error.toStdString());
        }
        t.boolean_value = left.get_token().boolean_value != right.get_token().boolean_value;
    }
    else{
        QString error = "Unexpected left operand on line ";
        error += QString::number(right.get_line_number());
        throw std::logic_error(error.toStdString());
    }

    result = Term(t);

    return result;
}

Term ConditionalExpression::gt(Term left, Term right) // Greater than
{
    Term result;
    if(left.get_token().token_type == TokenType::NUMBER){

        if(right.get_token().token_type == TokenType::NUMBER){
            Token t;
            t.token_type = TokenType::BOOLEAN;
            t.boolean_value = left.get_token().number_value > right.get_token().number_value;
            result = Term(t);
        }
        else{
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a number!";
            throw std::logic_error(error.toStdString());
        }
    }
    else{
        QString error = "Left operand on line ";
        error += QString::number(left.get_line_number());
        error+= " should be a number!";
        throw std::logic_error(error.toStdString());
    }

    return result;
}

Term ConditionalExpression::lt(Term left, Term right) // Less than
{
    Term result;
    if(left.get_token().token_type == TokenType::NUMBER){

        if(right.get_token().token_type == TokenType::NUMBER){
            Token t;
            t.token_type = TokenType::BOOLEAN;
            t.boolean_value = left.get_token().number_value < right.get_token().number_value;
            result = Term(t);
        }
        else{
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a number!";
            throw std::logic_error(error.toStdString());
        }
    }
    else{
        QString error = "Left operand on line ";
        error += QString::number(left.get_line_number());
        error+= " should be a number!";
        throw std::logic_error(error.toStdString());
    }

    return result;
}

Term ConditionalExpression::ge(Term left, Term right) // Greater than or equal to
{
    Term result;
    if(left.get_token().token_type == TokenType::NUMBER){

        if(right.get_token().token_type == TokenType::NUMBER){
            Token t;
            t.token_type = TokenType::BOOLEAN;
            t.boolean_value = left.get_token().number_value >= right.get_token().number_value;
            result = Term(t);
        }
        else{
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a number!";
            throw std::logic_error(error.toStdString());
        }
    }
    else{
        QString error = "Left operand on line ";
        error += QString::number(left.get_line_number());
        error+= " should be a number!";
        throw std::logic_error(error.toStdString());
    }

    return result;
}

Term ConditionalExpression::le(Term left, Term right) // Less than or equal to
{
    Term result;
    if(left.get_token().token_type == TokenType::NUMBER){

        if(right.get_token().token_type == TokenType::NUMBER){
            Token t;
            t.token_type = TokenType::BOOLEAN;
            t.boolean_value = left.get_token().number_value <= right.get_token().number_value;
            result = Term(t);
        }
        else{
            QString error = "Right operand on line ";
            error += QString::number(right.get_line_number());
            error+= " should also be a number!";
            throw std::logic_error(error.toStdString());
        }
    }
    else{
        QString error = "Left operand on line ";
        error += QString::number(left.get_line_number());
        error+= " should be a number!";
        throw std::logic_error(error.toStdString());
    }

    return result;
}


// Used for Where clause
Term ConditionalExpression::cond_expr(const QStringList& row, bool get)
{
    Term left = cond_term(row, get);

    std::cout<<"cond_expr() left: "<<left.get_token().to_string().toStdString()<<"\n";

    while(current_term != terms.end()){ // read and compute all terms in column expression
        //std::cout<<"expr() current term: "<<current_term->get_token().to_string().toStdString()<<"\n";
        if(current_term->get_token().token_type == TokenType::AND){
            Term right = cond_term(row, true);
            std::cout<<"cond_expr() AND right: "<<right.get_token().to_string().toStdString()<<"\n";
            left = and_op(left, right);
        }
        else if(current_term->get_token().token_type == TokenType::OR){
            left = or_op(left, cond_term(row, true));
        }
        else{
            break;
        }

    }

    return left;
}

Term ConditionalExpression::cond_term(const QStringList& row, bool get)
{
    Term left = cond_primary(row, get);

    std::cout<<"cond_term() current left: "<<left.get_token().to_string().toStdString()<<"\n";

    while(current_term != terms.end()){

        if(current_term->get_token().token_type == TokenType::ASSIGN){ //Equal
            Term right = cond_primary(row, true);
            std::cout<<"cond_term() == right: "<<right.get_token().to_string().toStdString()<<"\n";
            left = eq(left, right);
        }
        else if(current_term->get_token().token_type == TokenType::NOTEQUALTO){
            Term right = cond_primary(row, true);
            std::cout<<"cond_term() != right: "<<right.get_token().to_string().toStdString()<<"\n";
            left = neq(left, right);
        }
        else if(current_term->get_token().token_type == TokenType::LESSTHAN){
            left = lt(left, cond_primary(row, true));
        }
        else if(current_term->get_token().token_type == TokenType::GREATERTHAN){
            left = gt(left, cond_primary(row, true));
        }
        else if(current_term->get_token().token_type == TokenType::LESSTHANOREQUAL){
            left = le(left, cond_primary(row, true));
        }
        else if(current_term->get_token().token_type == TokenType::GREATERTHANOREQUAL){
            left = ge(left, cond_primary(row, true));
        }
        else{
            break;
        }
    }

    return left;
}


Term ConditionalExpression::cond_primary(const QStringList& row, bool get)
{
    if(get){
        move_to_next_term();
    }
    Term left = *get_current_term();

    //std::cout<<"cond_primary() current left: "<<left.get_token().to_string().toStdString()<<"\n";

    if(left.get_token().token_type == TokenType::LBRACKET){
        //auto e = cond_expr(row, true);
        //left = e;
        move_to_next_term(); //eat (
        std::cout<<" token after ( is "<< current_term->get_token().to_string().toStdString()<<"\n";
        auto ts = read_cond_expression();
        ConditionalExpression ce(ts);
        Term t = ce.eval(row);

        if(current_term->get_token().token_type != TokenType::RBRACKET){
            std::cout<<"next token "<<current_term->get_token().to_string().toStdString()<<"\n";
            QString error = "Expected a ) on line "+ QString::number(current_term->get_token().line_number);
            throw std::logic_error(error.toStdString());
        }
        else{
            //left = e;

            std::cout<<"ce result="<<t.get_token().to_string().toStdString()<<" {"<<t.get_token().string_value.toStdString()<<"}\n";

            left = t;
            move_to_next_term(); //eat )
        }
    }
    //else if(left.get_token().token_type == TokenType::RBRACKET){
        //move_to_next_term();
    //}
    else{
        //left = expr(row, get);
        //move_to_next_term();
        //std::cout<<"Calling read_expression()\n";
        Expression e = read_expression();
        //std::cout<<"evaluating left side of conditional expression\n";
        left = e.eval(row);
        std::cout<<"ex result="<<left.get_token().to_string().toStdString()<<" {"<<left.get_token().string_value.toStdString()<<"}\n";
        //move_to_next_term();
        //std::cout<<"done.\n";
    }

    return left;
}

Term ConditionalExpression::eval(const QStringList& row)
{
    Token t = {.token_type =  TokenType::END, .token_name = "TokenType::END"};
    Term result(t);

    //std::cout<<"ConditionalExpression::eval(): cond_expr()\n";
    result = cond_expr(row, false);
    //std::cout<<"done.\n";


    return result;
}

QList<Term> ConditionalExpression::read_cond_expression()
{
    QList<Term> ts;

    /*
    QList<QString> relational_tokens = {"TokenType::ASSIGN", "TokenType::AND", "TokenType::OR", "TokenType::LESSTHAN", "TokenType::GREATERTHAN",
                                        "TokenType::LESSTHANOREQUAL", "TokenType::GREATERTHANOREQUAL", "TokenType::NOTEQUALTO", "TokenType::RBRACKET"};
    */
    QList<QString> relational_tokens = {"TokenType::RBRACKET"};

    while(current_term != terms.end()){
        if(relational_tokens.contains(current_term->get_token().to_string())){
            break;
        }
        Term t(*current_term);
        ts.append(t);
        move_to_next_term();
    }
    //Expression e(ts);
    //return e;
    return ts;
}

Expression ConditionalExpression::read_expression()
{
    QList<Term> ts;


    QList<QString> relational_tokens = {"TokenType::ASSIGN", "TokenType::AND", "TokenType::OR", "TokenType::LESSTHAN", "TokenType::GREATERTHAN",
                                        "TokenType::LESSTHANOREQUAL", "TokenType::GREATERTHANOREQUAL", "TokenType::NOTEQUALTO"};

    //QList<QString> relational_tokens = {"TokenType::RBRACKET"};

    while(current_term != terms.end()){
        if(relational_tokens.contains(current_term->get_token().to_string())){
            break;
        }
        Term t(*current_term);
        ts.append(t);
        move_to_next_term();
    }
    Expression e(ts);
    return e;
    //return ts;
}


