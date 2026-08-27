// CSVQuery - An SQL-like query language for CSV files
// Copyright (c) 2025-2026 Kwame Yeboah-Gyan
// Distributed under the MIT License.

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "Types.h"
#include <iostream>


namespace csvquery {

    class Tokenizer
    {
        QHash<QChar, TokenType> char_table;
        std::shared_ptr<QTextStream> stream;
        Token token;
        double line_number = 1;

        static inline QHash<QString, TokenType> keywords = { {"select", TokenType::SELECT}, {"from", TokenType::FROM}, {"where", TokenType::WHERE},
                                                            {"and", TokenType::AND}, {"or", TokenType::OR}, {"into", TokenType::INTO}, {"update", TokenType::UPDATE},
                                                            {"delete", TokenType::DELETE}, {"import", TokenType::IMPORT}, {"into", TokenType::INTO}, {"set", TokenType::SET},
                                                             {"inner join", TokenType::INNERJOIN}, {"outer join", TokenType::OUTERJOIN}, {"cross join", TokenType::CROSSJOIN},
                                                            {"as", TokenType::AS}, {"on", TokenType::ON}, {"like", TokenType::LIKE} , {"not like", TokenType::NOTLIKE},
                                                            {"group by", TokenType::GROUPBY }, {"limit", TokenType::LIMIT }, {"having", TokenType::HAVING }
                                                            /*,{"case", TokenType::CASE}*/ , {"when", TokenType::WHEN}, {"then", TokenType::THEN}, {"else", TokenType::ELSE}, {"endcase", TokenType::ENDCASE}
        };

        //QList<TokenType> read_func_args();

    public:
        Tokenizer(std::shared_ptr<QTextStream> str);

        Token get();

        Token read(QChar quote = '*'); // * implies read till whitespace

        Token current_token() const
        {
            return token;
        }

        QList<Token> read_statement();

    };


    std::ostream& operator<<(std::ostream& out, const Token& t);

}
#endif // TOKENIZER_H
