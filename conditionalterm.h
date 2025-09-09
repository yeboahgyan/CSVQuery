#ifndef CONDITIONALTERM_H
#define CONDITIONALTERM_H

#include "Types.h"

class ConditionalTerm{
    Token token;
    double line_number;
public:
    ConditionalTerm(Token t, double line_num): token{t}, line_number{line_num}{}

    Token get_token() const {
        return token;
    }

    double get_line_number() const{
        return line_number;
    }

    ColumnResult eval(const QStringList& row){
        ColumnResult result;
        double index = 0;

        //literal as is


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
            if(!columns_table.contains(token.string_value)){
                result.token_type = TokenType::ERROR;
                result.error = "Invalid column name "+token.string_value+" on line "+ QString::number(line_number);
                return result;
            }
            index = columns_table[token.string_value];
        }

        if(index < 0 || index > row.length()){
            result.token_type = TokenType::ERROR;
            result.error = "Invalid column index "+QString::number(token.number_value)+" on line "+ QString::number(line_number);
            return result;
        }

        //return value at row index
        result.token_type = TokenType::STRING;
        result.string_value = row.at(index);
        return result;
    }
};

#endif // CONDITIONALTERM_H
