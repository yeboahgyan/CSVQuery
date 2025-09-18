#ifndef ASSIGNSTATEMENT_H
#define ASSIGNSTATEMENT_H

#include "Types.h"
#include "expression.h"

class AssignStatement
{
    const QList<Token> tokens;
    QList<Token>::const_iterator last_token_pos;
    QString variable_name;
    Term assignment_value;

    void throw_exception_if_unexpected_end();

    Expression read_expression();

    void process_expression(Expression& rhs);

    void parse();

public:
    AssignStatement(const QList<Token>& tks);

    void execute();
};

#endif // ASSIGNSTATEMENT_H
