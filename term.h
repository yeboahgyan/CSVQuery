#ifndef TERM_H
#define TERM_H

#include "Types.h"

namespace csvquery {

    class Term {
        Token token;

        std::function <Token(const QStringList& row)> compile_eval(const QStringList& row); // internal function

    public:
        Term()
        {
            token = { .token_type = TokenType::END, .token_name = "TokenType::END" };
        };

        Term(Token t) : token{ t } {}

        Token get_token() const {
            return token;
        }

        TokenType get_token_type() const {
            return token.token_type;
        }

        double get_line_number() const {
            return token.line_number;
        }

        Token eval(const QStringList& row);
        Term eval_and_return_term(const QStringList& row);

        Token eval(const QMap<QString, QStringList>& data_rows);

        

        std::function<Token(const QMap<QString, QStringList>& data_rows)> compile(const QMap<QString, QStringList>& data_rows);
    };

}
#endif // TERM_H
