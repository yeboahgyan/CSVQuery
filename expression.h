#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "Types.h"
#include "term.h"
#include <QHash>


namespace csvquery {

    class Expression
    {
    protected:
        QList<Term> terms;
        QList<Term>::iterator current_term;
        double item_left = 0;

        QHash<QString, QList<Term>::iterator> end_of_experssion;

        void reset_iterators() {
            current_term = terms.begin();
            item_left = (terms.length() == 0) ? 0 : terms.length() - 1;
        }


        bool iscolumn_or_literal(Token t) const;

        static double add(double num1, double num2);

        static QString add(double num1, QString str);

        static QString add(QString str, double num2);

        static QString add(QString str, QString str2);

        static Term mult(Term left, Term right);
        static Term div(Term left, Term right);
        static Term add(Term left, Term right);
        static Term minus(Term left, Term right);

        void move_to_next_term();

        QList<Term>::iterator peak_next_term();

        QList<Term>::iterator get_current_term() const;

        Term get_current_term();

        QList<Term>::iterator end() const;

        //Term expr(const QStringList& row, bool get);
        //Term term(const QStringList& row, bool get);
        //Term primary(const QStringList& row, bool get);

        Term expr(const QMap<QString, QStringList>& data_rows, bool get);
        Term term(const QMap<QString, QStringList>& data_rows, bool get);
        Term primary(const QMap<QString, QStringList>& data_rows, bool get);

        bool is_star() { // is * ? (select all columns)
            bool r = false;

            if (terms.length() != 1) {
                return r;
            }

            Term t = terms.front();
            if (t.get_token().token_type == TokenType::MULT) {
                return true;
            }
            else if (t.get_token().token_type == TokenType::NAME) { // check for name of format, file.*
                QStringList name_parts = t.get_token().string_value.split(".");

                if (name_parts.size() != 2) {
                    r = false;
                }

                if (name_parts[1] != '*') {
                    r = false;
                }
                r = true;
            }

            return r;
        }

        Term eval_star_term(const QMap<QString, QStringList>& data_rows) { // is * ? (select all columns)
            Term t = terms.front();

            //if(is_star()){
            return t.eval(data_rows);
            //}
        }

        std::function<Term(const QMap<QString, QStringList>& data_rows)> comp_eval_star_term(const QMap<QString, QStringList>& data_rows) { // is * ? (select all columns)
            Term t = terms.front();

            //if(is_star()){
            auto t_func = t.compile(data_rows);
            auto func = [t_func](const QMap<QString, QStringList>& data_rows) {
                Token t;
                t = t_func(data_rows);

                return Term(t);
                };
            
            return func;
        }

        std::function<Term(const QMap<QString, QStringList>& data_rows)> comp_expr(const QMap<QString, QStringList>& data_rows, bool get);
        std::function<Term(const QMap<QString, QStringList>& data_rows)> comp_term(const QMap<QString, QStringList>& data_rows, bool get);
        std::function<Term(const QMap<QString, QStringList>& data_rows)> comp_primary(const QMap<QString, QStringList>& data_rows, bool get);

        double get_iterator_pos() const {
            return item_left;
        }

    public:
        Expression(const QList<Term>& ts);

        double number_of_terms() const {
            return terms.length();
        }

        void add(const Term t) {
            terms.append(t);
        }

        //Term eval(const QStringList& row );

        Term eval(const QMap<QString, QStringList>& data_rows);
        std::function<Term(const QMap<QString, QStringList>& data_rows)> compile(const QMap<QString, QStringList>& data_rows);
    };

}

#endif // EXPRESSION_H
