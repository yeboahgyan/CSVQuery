#ifndef UPDATESTATEMENT_H
#define UPDATESTATEMENT_H

#include "expression.h"
#include "Types.h"
#include "conditionalexpression.h"
#include "csvfile.h"
#include <utility>

class UpdateStatement
{
    const QList<Token> tokens;
    QList<Token>::const_iterator last_token_pos;
    QList<std::pair<Token, Expression>> column_update_list;
    std::shared_ptr<ConditionalExpression> where_expr; // where clause
    std::shared_ptr<CSVFile> left_file;
    std::shared_ptr<CSVFile> out_file; // output file

    void throw_exception_if_unexpected_end();

    Token read_column();

    Expression read_expression();

    void read_column_update_list();

    std::shared_ptr<ConditionalExpression> read_where_clause();

    std::shared_ptr<CSVFile> read_file(QIODeviceBase::OpenMode m = QIODevice::ReadOnly);

    void process_expression(Expression& rhs);

    void parse();

public:
    UpdateStatement(const QList<Token>& tks);
};

#endif // UPDATESTATEMENT_H
