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
        Term get_current_term();

        //QList<Term>::iterator end() const;

        static Term or_op(Term left, Term right); // logical OR
        static Term and_op(Term left, Term right); // logical AND
        static Term eq(Term left, Term right); // Equal
        static Term neq(Term left, Term right); // Not Equal
        static Term gt(Term left, Term right); // Greater than
        static Term lt(Term left, Term right); // Less than
        static Term ge(Term left, Term right); // Greater than or equal to
        static Term le(Term left, Term right); // Less than or equal to
        static Term like(Term left, Term right); // SQL Like pattern matching; left term is value string and right term is pattern string
        static Term not_like(Term left, Term right);

        Term cond_expr(const QMap<QString, QStringList>& data_rows, bool get);
        Term cond_term(const QMap<QString, QStringList>& data_rows, bool get);
        Term cond_primary(const QMap<QString, QStringList>& data_rows, bool get);

        //compile
        std::function<Term(const QMap<QString, QStringList>& data_rows)> comp_cond_expr(const QMap<QString, QStringList> data_rows, bool get);
        std::function<Term(const QMap<QString, QStringList>& data_rows)> comp_cond_term(const QMap<QString, QStringList> data_rows, bool get);
        std::function<Term(const QMap<QString, QStringList>& data_rows)> comp_cond_primary(const QMap<QString, QStringList> data_rows, bool get);

        std::function < Term(const Term&, const Term&)> comp_or_op(const Term& left, const Term& right); // logical OR
        std::function < Term(const Term&, const Term&)> comp_and_op(const Term& left, const Term& right); // logical AND
        std::function < Term(const Term&, const Term&)> comp_eq(const Term& left, const Term& right); // Equal
        std::function < Term(const Term&, const Term&)> comp_neq(const Term& left, const Term& right); // Not Equal
        std::function < Term(const Term&, const Term&)> comp_gt(const Term& left, const Term& right); // Greater than
        std::function < Term(const Term&, const Term&)> comp_lt(const Term& left, const Term& right); // Less than
        std::function < Term(const Term&, const Term&)> comp_ge(const Term& left, const Term& right); // Greater than or equal to
        std::function < Term(const Term&, const Term&)> comp_le(const Term& left, const Term& right); // Less than or equal to
        std::function < Term(const Term&, const Term&)> comp_like(const Term& left, const Term& right); // SQL Like pattern matching; left term is value string and right term is pattern string
        std::function < Term(const Term&, const Term&)> comp_not_like(const Term& left, const Term& right);

        QList<Term> read_cond_expression();
        Expression read_expression();


    public:
        ConditionalExpression(const QList<Term>& ts);


        Term eval(const QMap<QString, QStringList>& data_rows);
        std::function<Term(const QMap<QString, QStringList>& data_rows)> compile(const QMap<QString, QStringList> data_rows);
    };

}

#endif // CONDITIONALEXPRESSION_H
