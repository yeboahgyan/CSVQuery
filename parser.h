#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include "columnexpression.h"
#include "Types.h"
#include "selectstatement.h"
#include "tokenizer.h"



class Parser
{
    static inline QHash<QString, TokenType> keywords = {{"select",TokenType::SELECT}, {"from",TokenType::FROM}, {"where", TokenType::WHERE},
                                                 {"and",TokenType::AND}, {"or",TokenType::OR}, {"into",TokenType::INTO}, {"update",TokenType::UPDATE},
                                                 {"delete",TokenType::DELETE}, {"import",TokenType::IMPORT}};

    static inline QHash<QString, QHash<QString, unsigned int>> type_table; //csv_name and map of {column name, column number}

    static inline QList<QChar> operators = {',', '+', '-', '*', '/', ')', '('};

    std::shared_ptr<QTextStream> text_stream;

    QList<Token> read_statement(); //reads characters till ';'
    Token compose_token();
    Token read_token(); //reads tokens from stream and sets current_token

    Token current_token = {TokenType::END};

    //Result expression(QString statement);

    //void importDefinition(); //reads csv definition file and loads into named_columns_number in the format {csv_name.column_name, column_number_in_csv}

    Result import_statement(QList<Token>& tokens);
    Result assignment_statement(Token name, QList<Token>& tokens);
    SelectStatement select_statement(QList<Token>& tokens);
    Result delete_statement(QList<Token>& tokens);
    Result update_statement(QList<Token>& tokens);

    QList<ColumnExpression> read_column_expr(QList<Token> tokens);

    double line_number = 0;

public:
    Parser(std::shared_ptr<QTextStream> ts);
    QList<Result> execute();
};

#endif // PARSER_H
