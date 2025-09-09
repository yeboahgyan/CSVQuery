
#include "term.h"
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
