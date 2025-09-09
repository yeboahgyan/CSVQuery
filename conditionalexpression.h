#ifndef CONDITIONALEXPRESSION_H
#define CONDITIONALEXPRESSION_H
#include "Types.h"
#include "expression.h"


class ConditionalExpression
{
    QList<Term> terms;
    QList<Term>::iterator current_term;
    double item_left = 0;

    //QHash<QString, QList<Term>::iterator> end_of_experssion;


    void move_to_next_term();

    QList<Term>::iterator peak_next_term();

    QList<Term>::iterator get_current_term() const;

    //QList<Term>::iterator end() const;

    Term or_op(Term left, Term right); // logical OR
    Term and_op(Term left, Term right); // logical AND
    Term eq(Term left, Term right); // Equal
    Term neq(Term left, Term right); // Not Equal
    Term gt(Term left, Term right); // Greater than
    Term lt(Term left, Term right); // Less than
    Term ge(Term left, Term right); // Greater than or equal to
    Term le(Term left, Term right); // Less than or equal to

    Term cond_expr(const QStringList& row, bool get);
    Term cond_term(const QStringList& row, bool get);
    Term cond_primary(const QStringList& row, bool get);

    QList<Term> read_cond_expression();
    Expression read_expression();


public:
    ConditionalExpression(const QList<Term>& ts);


    Term eval(const QStringList& row );
};

#endif // CONDITIONALEXPRESSION_H
