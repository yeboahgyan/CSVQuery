#include "parser.h"
#include "importstatement.h"
#include "assignstatement.h"
#include "selectstatement.h"
#include "updatestatement.h"
#include <stdexcept>

namespace csvquery {

    Parser::Parser(std::shared_ptr<QTextStream>& stream)
        : tokenizer(stream)
    {

    }

    QList<Token> Parser::read_statement()
    {
        QList<Token> tokens;
        //QList<Expression> exps;

        // Read import statement and Print tokens //
        Token token = tokenizer.get();
        while (token.token_type != TokenType::END) {
            if (token.token_type == TokenType::SEMICOLON) {
                //std::cout<<";%%\n";
                tokens.append(token);
                break;
            }//else{
                //std::cout<< token << "{" <<token.string_value.toStdString() <<"} ";
            //}

            //Term t(token);
            tokens.append(token);
            token = tokenizer.get();
        }

        return tokens;
    }


    std::pair<int, std::optional<QList<QStringList>>> Parser::execute(const QList<Token>& statement_tokens)
    {
        std::pair<int, std::optional<QList<QStringList>>> result = { 0, std::nullopt };

        if (statement_tokens.front().token_type == TokenType::IMPORT) {
            ImportStatement import(statement_tokens);
            import.execute();
            result.first = import.num_of_columns_loaded();
        }
        else if (statement_tokens.front().token_type == TokenType::ASSIGN) {
            AssignStatement assign(statement_tokens);
            assign.execute();
        }
        else if (statement_tokens.front().token_type == TokenType::SELECT) {
            SelectStatement select(statement_tokens);
            result.second = select.execute();
            result.first = select.get_number_of_rows();
        }
        else if (statement_tokens.front().token_type == TokenType::UPDATE) {
            UpdateStatement update(statement_tokens);
            update.execute();
            result.first = update.get_number_of_rows();
        }
        else {
            QString error = "Unkown statement on line ";
            error += QString::number(statement_tokens.front().line_number);
            throw std::logic_error(error.toStdString());
        }

        return result;
    }

}