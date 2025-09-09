#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "Types.h"
#include "term.h"
#include <QHash>

class Expression
{
protected:
    QList<Term> terms;
    QList<Term>::iterator current_term;
    double item_left = 0;

    QHash<QString, QList<Term>::iterator> end_of_experssion;


    bool iscolumn_or_literal(Token t) const;

    double add(double num1, double num2);

    QString add(double num1, QString str);

    QString add(QString str, double num2);

    QString add(QString str, QString str2);

    Term mult(Term left, Term right);
    Term div(Term left, Term right);
    Term add(Term left, Term right);
    Term minus(Term left, Term right);

    void move_to_next_term();

    QList<Term>::iterator peak_next_term();

    QList<Term>::iterator get_current_term() const;

    QList<Term>::iterator end() const;

    Term expr(const QStringList& row, bool get);
    Term term(const QStringList& row, bool get);
    Term primary(const QStringList& row, bool get);

public:
    Expression(const QList<Term>& ts);

    void add(const Term t){
        terms.append(t);
    }

    bool is_star(){ // is * ? (select all columns)
        bool r = false;

        if(terms.length() == 1){
            Term t = terms.front();
            if(t.get_token().token_type == TokenType::MULT){
                return true;
            }
        }else{
            r= false;
        }

        return r;
    }

    Term eval_star_term(const QStringList& row ){ // is * ? (select all columns)
        Term t = terms.front();

        //if(is_star()){
        return t.eval(row);
        //}
    }

    double number_of_terms() const{
        return terms.length();
    }

    double get_iterator_pos() const {
        return item_left;
    }

    Term eval(const QStringList& row );
};

#endif // EXPRESSION_H
