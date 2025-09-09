#ifndef SELECTSTATEMENT_H
#define SELECTSTATEMENT_H
#include <QFileInfo>
#include <QBuffer>

#include "Types.h"
#include "columnexpression.h"
#include "conditionalexpression.h"


class SelectStatement
{
    QList<ColumnExpression> column_exprs;
    ConditionalExpression conditional_expr; // where clause
    ColumnExpression on_clause;
    QString out_file;
    QList<QString> csv_files;
    QList<Token> tokens;
    QString error_msg;
    bool has_join;
    bool has_where_clause;
    TokenType join_type;
public:
    SelectStatement(const QList<Token>& tks);

    bool read_column_expressions();

    bool get_file(bool is_out_file = false);

    bool read_join(TokenType join_type);

    bool read_where();

    bool read_into();

    void enable_join(bool yes_no){
        has_join = yes_no;
    }

    void enable_where_clause(bool yes_no){
        has_where_clause = yes_no;
    }

    void set_join_type(TokenType jt){
        join_type = jt;
    }

    Result no_join_eval();

    Result inner_join_eval();

    Result outer_join_eval();

    Result cross_join_eval();

    bool get_file_mem_map(std::shared_ptr<QFile> f, std::shared_ptr<QBuffer> b);


    QString get_error() const{
        return error_msg;
    }

    void add_column_expression(const ColumnExpression& col_expr){
        column_exprs.append(col_expr);
    }

    void add_conditional_expr(const ConditionalExpression& cond_expr){
        conditional_expr = cond_expr;
    }

    Result eval();
};

#endif // SELECTSTATEMENT_H
