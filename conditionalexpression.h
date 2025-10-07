#ifndef CONDITIONALEXPRESSION_H
#define CONDITIONALEXPRESSION_H
#include "Types.h"
#include "expression.h"

namespace csvquery {

    class ConditionalExpression
    {
        QList<Term> terms;
        QList<Term>::iterator current_term;
        double item_left = 0;

        //QHash<QString, QList<Term>::iterator> end_of_experssion;

        void reset_iterators() {
            current_term = terms.begin();
            item_left = (terms.length() == 0) ? 0 : terms.length() - 1;
        }


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
        Term like(Term left, Term right); // SQL Like pattern matching; left term is value string and right term is pattern string
        Term not_like(Term left, Term right);

        Term cond_expr(const QMap<QString, QStringList>& data_rows, bool get);
        Term cond_term(const QMap<QString, QStringList>& data_rows, bool get);
        Term cond_primary(const QMap<QString, QStringList>& data_rows, bool get);

        QList<Term> read_cond_expression();
        Expression read_expression();


    public:
        ConditionalExpression(const QList<Term>& ts);


        Term eval(const QMap<QString, QStringList>& data_rows);
    };

}

#endif // CONDITIONALEXPRESSION_H
