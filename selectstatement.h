#ifndef SELECTSTATEMENT_H
#define SELECTSTATEMENT_H
#include <QFileInfo>
#include <QBuffer>
#include <memory>
#include "Types.h"
#include "conditionalexpression.h"
#include "csvfile.h"


class SelectStatement
{
    QList<Expression> column_exprs; // columns
    std::shared_ptr<ConditionalExpression> conditional_expr; // where clause
    std::shared_ptr<ConditionalExpression> on_clause; // on clause for joins

    QString out_file; // output file

    std::shared_ptr<CSVFile> left_file;
    std::shared_ptr<CSVFile> right_file;

    QList<Token> tokens; // select statement tokens excluding select. Last token is a semi-colon
    QList<Token>::const_iterator last_token_pos;

    bool has_join;
    bool has_where_clause;
    TokenType join_type;
    void throw_exception_if_unexpected_end();
public:
    SelectStatement(const QList<Token>& tks);

    QList<Expression> read_column_expressions();

    std::shared_ptr<CSVFile> read_file();
    std::shared_ptr<ConditionalExpression> read_on_clause();

    void parse();

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


    Result eval();

    void execute_and_save_to_file();
    QList<QStringList> execute_and_return(int number_of_rows); // make it a coroutine
};

#endif // SELECTSTATEMENT_H
