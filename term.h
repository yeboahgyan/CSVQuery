#ifndef TERM_H
#define TERM_H

#include "Types.h"

class Term{
    Token token;

public:
    Term()
    {
        token = {.token_type= TokenType::END, .token_name="TokenType::END"};
    };

    Term(Token t): token{t}{}

    Token get_token() const {
        return token;
    }

    TokenType get_token_type() const{
        return token.token_type;
    }

    double get_line_number() const{
        return token.line_number;
    }

    Token eval(const QStringList& row);
    Term eval_and_return_term(const QStringList& row);
};

#endif // TERM_H
