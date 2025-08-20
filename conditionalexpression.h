#ifndef CONDITIONALEXPRESSION_H
#define CONDITIONALEXPRESSION_H
#include "Types.h"
#include "columnterm.h"


class ConditionalExpression
{
    QList<ColumnTerm> column_terms;
    ColumnTerm current_column_term = {{TokenType::END}, 0};

    ColumnResult expr(const QStringList& row);
    ColumnResult term(const QStringList& row);
    ColumnResult primary(const QStringList& row);

    ColumnResult cond_expr(const QStringList& row); // Used for Where clause
    ColumnResult cond_term(const QStringList& row);
    ColumnResult cond_primary(const QStringList& row);

    ColumnTerm get_column_term(bool get){
        if(get){
            current_column_term = column_terms.front();
            column_terms.pop_front();
        }

        return current_column_term;
    }

    bool iscolumn_or_literal(Token t) const{
        if (t.token_type == TokenType::COLUMNNAME){
            return true;
        }

        if(t.token_type == TokenType::COLUMNNUMBER){
            return true;
        }

        if(t.token_type == TokenType::STRING){
            return true;
        }

        if(t.token_type == TokenType::NUMBER){
            return true;
        }

        return false;
    }

    double add(double num1, double num2){
        return num1 + num2;
    }

    QString add(double num1, QString str){
        return QString::number(num1) + str;
    }

    QString add(QString str, double num2){
        return str + QString::number(num2);
    }

    QString add(QString str, QString str2){
        return str + str2;
    }


    ColumnResult mult(ColumnResult left, ColumnResult right);
    ColumnResult div(ColumnResult left, ColumnResult right);
    ColumnResult add(ColumnResult left, ColumnResult right);
    ColumnResult minus(ColumnResult left, ColumnResult right);

    ColumnResult or_op(ColumnResult left, ColumnResult right); // logical OR
    ColumnResult and_op(ColumnResult left, ColumnResult right); // logical AND
    ColumnResult eq(ColumnResult left, ColumnResult right); // Equal
    ColumnResult gt(ColumnResult left, ColumnResult right); // Greater than
    ColumnResult lt(ColumnResult left, ColumnResult right); // Less than
    ColumnResult ge(ColumnResult left, ColumnResult right); // Greater than or equal to
    ColumnResult le(ColumnResult left, ColumnResult right); // Less than or equal to
public:
    ConditionalExpression();

    void add(const ColumnTerm c){
        column_terms.append(c);
    }

    ColumnResult eval(const QStringList& row );
};

#endif // CONDITIONALEXPRESSION_H
