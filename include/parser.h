// CSVQuery - An SQL-like query language for CSV files
// Copyright (c) 2025-2026 Kwame Yeboah-Gyan
// Distributed under the MIT License.

#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include "Types.h"
#include "tokenizer.h"

namespace csvquery {

    class Parser
    {
        Tokenizer tokenizer;

    public:
        Parser(std::shared_ptr<QTextStream>& stream);
        QList<Token> read_statement();
        std::pair<int, std::optional<QList<QStringList>> > execute(const QList<Token>& statement_tokens);

        Token current_token() const {
            return tokenizer.current_token();
        }
    };

}

#endif // PARSER_H
