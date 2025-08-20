#include "conditionalexpression.h"

ConditionalExpression::ConditionalExpression() {}

ColumnResult ConditionalExpression::mult(ColumnResult left, ColumnResult right){
    ColumnResult result;

    if(left.token_type ==TokenType::NUMBER && right.token_type ==TokenType::NUMBER){
        result.token_type = TokenType::NUMBER;
        result.number_value = left.number_value * right.number_value;
    }
    else{
        result.token_type = TokenType::ERROR;
        result.error = "cannot multiply types";
    }
    return result;
}

ColumnResult ConditionalExpression::div(ColumnResult left, ColumnResult right){
    ColumnResult result;

    if(left.token_type ==TokenType::NUMBER && right.token_type ==TokenType::NUMBER){
        result.token_type = TokenType::NUMBER;
        if(right.number_value == 0){
            result.token_type = TokenType::ERROR;
            result.error = "divide by zero error";
        }else{
            result.number_value = left.number_value / right.number_value;
        }
    }
    else{
        result.token_type = TokenType::ERROR;
        result.error = "cannot divide types";
    }
    return result;
}


ColumnResult ConditionalExpression::minus(ColumnResult left, ColumnResult right){
    ColumnResult result;

    if(left.token_type ==TokenType::NUMBER && right.token_type ==TokenType::NUMBER){
        result.token_type = TokenType::NUMBER;
        result.number_value = left.number_value - right.number_value;
    }
    else{
        result.token_type = TokenType::ERROR;
        result.error = "cannot subtract types";
    }
    return result;
}


ColumnResult ConditionalExpression::add(ColumnResult left, ColumnResult right){

    ColumnResult result;

    if(left.token_type ==TokenType::STRING){
        if(right.token_type ==TokenType::STRING){
            result.token_type = TokenType::STRING;
            result.string_value = add(left.string_value, right.string_value);
            //return result;
        }
        else{
            result.token_type = TokenType::STRING;
            result.string_value = add(left.string_value, right.number_value);
            //return result;
        }
    }
    else if(left.token_type ==TokenType::NUMBER){
        if(right.token_type ==TokenType::STRING){
            result.token_type = TokenType::STRING;
            result.string_value = add(left.number_value, right.string_value);
            //return result;
        }
        else{
            result.token_type = TokenType::NUMBER;
            result.number_value = add(left.number_value, right.number_value);
            //return result;
        }
    }
    else{
        result.token_type = TokenType::ERROR;
        result.error = "cannot add types";
    }

    return result;

}

ColumnResult ConditionalExpression::expr(const QStringList& row)
{
    ColumnResult left;

    auto left_column_term = get_column_term(true); //get left column term
    left = left_column_term.eval(row); //evaluate the term

    if(left.token_type == TokenType::ERROR){
        left.error = "Error on line " + QString::number(left_column_term.get_line_number());
        return left;
    }


    while(!column_terms.empty()){ // read and compute all terms in column expression

        auto right_column_term = get_column_term(true);
        auto right_token = right_column_term.get_token();

        if(right_token.token_type == TokenType::PLUS){
            ColumnResult right = term(row);

            if(right.token_type == TokenType::ERROR){
                left.error = "Error on line " + QString::number(right_column_term.get_line_number());
                break;
            }


            left = add(left, right);

            if(left.token_type == TokenType::ERROR){
                left.error = "Error on line " + QString::number(right_column_term.get_line_number());
                break;
            }
        }
        else if(right_token.token_type == TokenType::MINUS){
            ColumnResult right = term(row);

            if(right.token_type == TokenType::ERROR){
                left.error = "Error on line " + QString::number(right_column_term.get_line_number());
                break;
            }

            left = minus(left, right);

            if(left.token_type == TokenType::ERROR){
                left.error = "Error on line " + QString::number(right_column_term.get_line_number());
                break;
            }
        }
        else{
            //return left;
            this->column_terms.push_front(right_column_term);
            break;
        }
        //left_token = left_column_term.get_token();
    }

    return left;
}


ColumnResult ConditionalExpression::term(const QStringList& row)
{
    ColumnResult left;
    auto left_column_term = get_column_term(true);
    left = left_column_term.eval(row);

    if(left.token_type == TokenType::ERROR){
        left.error = "Error on line " + QString::number(left_column_term.get_line_number());
        return left;
    }

    while(!column_terms.empty()){

        auto right_column_term = get_column_term(true);
        auto right_token = right_column_term.get_token();

        if(right_token.token_type == TokenType::MULT){
            ColumnResult right = primary(row);
            left = mult(left, right);

            if(left.token_type == TokenType::ERROR){
                break;
            }
        }
        else if(right_token.token_type == TokenType::DIV){
            ColumnResult right = primary(row);
            left = div(left, right);

            if(left.token_type == TokenType::ERROR){
                break;
            }
        }
        else{
            //return left;
            this->column_terms.push_front(right_column_term);
            break;
        }
        //right_token = left_column_term.get_token();
    }

    return left;
}


ColumnResult ConditionalExpression::primary(const QStringList& row)
{
    ColumnResult left;
    auto left_column_term = get_column_term(true);
    Token left_token = left_column_term.get_token();

    //left = left_column_term.eval(row);


    if(left_token.token_type == TokenType::NUMBER){
        left = left_column_term.eval(row);
    }
    else if(left_token.token_type == TokenType::STRING){
        left = left_column_term.eval(row);
    }
    else if(left_token.token_type == TokenType::COLUMNNAME){
        left = left_column_term.eval(row);
    }
    else if(left_token.token_type == TokenType::COLUMNNUMBER){
        left = left_column_term.eval(row);
    }
    else if(left_token.token_type == TokenType::MINUS){
        ColumnResult neg_1;
        neg_1.token_type = TokenType::NUMBER;
        neg_1.number_value = -1;

        left = mult(neg_1, primary(row));
    }
    else if(left_token.token_type == TokenType::FUNCTION){
        std::function<ColumnResult(QList<TokenType>, QList<std::any>)> f= funcs_table[left_token.string_value];

        //read function arguments
        auto next_column_term = get_column_term(true);
        if(next_column_term.get_token().token_type != TokenType::LBRACKET){
            left.error = "Expected a '('";
            left.token_type = TokenType::ERROR;
        }
        else{
            next_column_term = get_column_term(true);
            QList<TokenType> arg_types;
            QList<std::any> arg_vals;
            while(next_column_term.get_token().token_type != TokenType::RBRACKET){
                ColumnResult arg_result = this->expr(row);

                if(arg_result.token_type == TokenType::ERROR){
                    left.error = "Error reading arguments for function!";
                    left.token_type = TokenType::ERROR;
                }
                else{
                    arg_types.append(arg_result.token_type);
                    if(arg_result.token_type == TokenType::NUMBER){
                        arg_vals.append(arg_result.number_value);
                    }
                    else if(arg_result.token_type == TokenType::NUMBER){
                        arg_vals.append(arg_result.string_value);
                    }
                    else{
                        left.error = "Invalid token whilst reading function arguments!";
                        left.token_type = TokenType::ERROR;
                        break;
                    }
                }
            }
            left = f(arg_types, arg_vals); //function call
        }

    }
    else if(left_token.token_type == TokenType::LBRACKET){
        auto e = expr(row);
        auto next_token = get_column_term(true);
        if(next_token.get_token().token_type != TokenType::RBRACKET){
            left.error = "Unexpected token";
            left.token_type = TokenType::ERROR;
        }
        else{
            left = e;
        }
    }
    else {
        left.error = "Unexpected token";
        left.token_type = TokenType::ERROR;
    }


    return left;
}

ColumnResult ConditionalExpression::eval(const QStringList& row ){
    ColumnResult result;

    result = cond_expr(row);

    return result;
}

//Where clause
ColumnResult ConditionalExpression::or_op(ColumnResult left, ColumnResult right) // logical OR
{
    ColumnResult result;
    if(left.token_type != TokenType::BOOLEAN){
        result.token_type = TokenType::ERROR;
        result.error = "Left operand should be a boolean!";
    }
    else if(right.token_type != TokenType::BOOLEAN){
        result.token_type = TokenType::ERROR;
        result.error = "Right operand should be a boolean!";
    }
    else{
        result.token_type = TokenType::BOOLEAN;
        result.boolean_value = left.boolean_value || right.boolean_value;
    }

    return result;
}

ColumnResult ConditionalExpression::and_op(ColumnResult left, ColumnResult right) // logical AND
{
    ColumnResult result;
    if(left.token_type != TokenType::BOOLEAN){
        result.token_type = TokenType::ERROR;
        result.error = "Left operand should be a boolean!";
    }
    else if(right.token_type != TokenType::BOOLEAN){
        result.token_type = TokenType::ERROR;
        result.error = "Right operand should be a boolean!";
    }
    else{
        result.token_type = TokenType::BOOLEAN;
        result.boolean_value = left.boolean_value && right.boolean_value;
    }

    return result;
}

ColumnResult ConditionalExpression::eq(ColumnResult left, ColumnResult right) // Equal
{
    ColumnResult result;
    if(left.token_type == TokenType::NUMBER){

        if(right.token_type == TokenType::NUMBER){
            result.token_type = TokenType::BOOLEAN;
            result.boolean_value = left.number_value == right.number_value;
        }
        else{
            result.token_type = TokenType::ERROR;
            result.error = "Right operand should be a number!";
        }
    }
    else if(right.token_type != TokenType::STRING){

        if(right.token_type == TokenType::STRING){
            result.token_type = TokenType::BOOLEAN;
            result.boolean_value = left.string_value == right.string_value;
        }
        else{
            result.token_type = TokenType::ERROR;
            result.error = "Right operand should be a string!";
        }
    }
    else{
        result.token_type = TokenType::ERROR;
        result.error = "Left operand is should be a number or string!";
    }

    return result;
}

ColumnResult ConditionalExpression::gt(ColumnResult left, ColumnResult right) // Greater than
{
    ColumnResult result;
    if(left.token_type == TokenType::NUMBER){

        if(right.token_type == TokenType::NUMBER){
            result.token_type = TokenType::BOOLEAN;
            result.boolean_value = left.number_value > right.number_value;
        }
        else{
            result.token_type = TokenType::ERROR;
            result.error = "Right operand should be a number!";
        }
    }
    else{
        result.token_type = TokenType::ERROR;
        result.error = "Left operand is should be a number!";
    }

    return result;
}

ColumnResult ConditionalExpression::lt(ColumnResult left, ColumnResult right) // Less than
{
    ColumnResult result;
    if(left.token_type == TokenType::NUMBER){

        if(right.token_type == TokenType::NUMBER){
            result.token_type = TokenType::BOOLEAN;
            result.boolean_value = left.number_value < right.number_value;
        }
        else{
            result.token_type = TokenType::ERROR;
            result.error = "Right operand should be a number!";
        }
    }
    else{
        result.token_type = TokenType::ERROR;
        result.error = "Left operand is should be a number!";
    }

    return result;
}

ColumnResult ConditionalExpression::ge(ColumnResult left, ColumnResult right) // Greater than or equal to
{
    ColumnResult result;
    if(left.token_type == TokenType::NUMBER){

        if(right.token_type == TokenType::NUMBER){
            result.token_type = TokenType::BOOLEAN;
            result.boolean_value = left.number_value >= right.number_value;
        }
        else{
            result.token_type = TokenType::ERROR;
            result.error = "Right operand should be a number!";
        }
    }
    else{
        result.token_type = TokenType::ERROR;
        result.error = "Left operand is should be a number!";
    }

    return result;
}

ColumnResult ConditionalExpression::le(ColumnResult left, ColumnResult right) // Less than or equal to
{
    ColumnResult result;
    if(left.token_type == TokenType::NUMBER){

        if(right.token_type == TokenType::NUMBER){
            result.token_type = TokenType::BOOLEAN;
            result.boolean_value = left.number_value <= right.number_value;
        }
        else{
            result.token_type = TokenType::ERROR;
            result.error = "Right operand should be a number!";
        }
    }
    else{
        result.token_type = TokenType::ERROR;
        result.error = "Left operand is should be a number!";
    }

    return result;
}


// Used for Where clause
ColumnResult ConditionalExpression::cond_expr(const QStringList& row)
{
    ColumnResult left = expr(row);

    auto left_column_term = get_column_term(true); //get left column term
    left = left_column_term.eval(row); //evaluate the term

    if(left.token_type == TokenType::ERROR){
        left.error = "Error on line " + QString::number(left_column_term.get_line_number());
        return left;
    }


    while(!column_terms.empty()){ // read and compute all terms in column expression

        auto right_column_term = get_column_term(true);
        auto right_token = right_column_term.get_token();

        if(right_token.token_type == TokenType::AND){
            ColumnResult right = cond_term(row);

            if(right.token_type == TokenType::ERROR){
                left.error = "Error on line " + QString::number(right_column_term.get_line_number());
                break;
            }


            left = and_op(left, right);

            if(left.token_type == TokenType::ERROR){
                left.error = "Error on line " + QString::number(right_column_term.get_line_number());
                break;
            }
        }
        else if(right_token.token_type == TokenType::OR){
            ColumnResult right = cond_term(row);

            if(right.token_type == TokenType::ERROR){
                left.error = "Error on line " + QString::number(right_column_term.get_line_number());
                break;
            }

            left = or_op(left, right);

            if(left.token_type == TokenType::ERROR){
                left.error = "Error on line " + QString::number(right_column_term.get_line_number());
                break;
            }
        }
        else{
            //return left;
            this->column_terms.push_front(right_column_term);
            break;
        }
        //left_token = left_column_term.get_token();
    }

    return left;
}

ColumnResult ConditionalExpression::cond_term(const QStringList& row)
{
    ColumnResult left;
    auto left_column_term = get_column_term(true);
    left = left_column_term.eval(row);

    if(left.token_type == TokenType::ERROR){
        left.error = "Error on line " + QString::number(left_column_term.get_line_number());
        return left;
    }

    while(!column_terms.empty()){
        auto next_column_term = get_column_term(true);
        auto next_token = next_column_term.get_token();

        if(next_token.token_type == TokenType::ASSIGN){ //Equal to
            auto right = expr(row);
            left = eq(left, right);
        }
        else if(next_token.token_type == TokenType::LESSTHAN){
            auto right = expr(row);
            left = lt(left, right);
        }
        else if(next_token.token_type == TokenType::LESSTHANOREQUAL){
            auto right = expr(row);
            left = le(left, right);
        }
        else if(next_token.token_type == TokenType::GREATERTHAN){
            auto right = expr(row);
            left = gt(left, right);
        }
        else if(next_token.token_type == TokenType::GREATERTHANOREQUAL){
            auto right = expr(row);
            left = ge(left, right);
        }
        else{
            this->column_terms.push_front(next_column_term);
            break;
        }
    }

    return left;
}

ColumnResult ConditionalExpression::cond_primary(const QStringList& row)
{
    ColumnResult left;

    return left;
}
