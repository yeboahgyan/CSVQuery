#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include "Types.h"
#include "tokenizer.h"



class Parser
{
    Tokenizer tokenizer;

    QList<Token> read_statement();

public:
    Parser(std::shared_ptr<QTextStream>& stream);
    std::optional<QList<QStringList>> execute(const QList<Token>& statement_tokens);
};

#endif // PARSER_H
