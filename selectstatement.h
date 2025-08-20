#ifndef SELECTSTATEMENT_H
#define SELECTSTATEMENT_H
#include "Types.h"
#include "columnexpression.h"
#include "conditionalexpression.h"

class SelectStatement
{
    QList<ColumnExpression> column_exprs;
    ConditionalExpression conditional_expr;
    QString out_file;
    QList<QString> csv_files;
public:
    SelectStatement(const QList<QString>& in_files, const QString& out_file);

    void add_column_expression(const ColumnExpression& col_expr){
        column_exprs.append(col_expr);
    }

    void add_conditional_expr(const ConditionalExpression& cond_expr){
        conditional_expr = cond_expr;
    }

    Result eval();
};

#endif // SELECTSTATEMENT_H
