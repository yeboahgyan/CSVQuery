#ifndef SELECTSTATEMENT_H
#define SELECTSTATEMENT_H
#include <QFileInfo>
#include <QBuffer>
#include <memory>
#include "Types.h"
#include "conditionalexpression.h"
#include "csvfile.h"
#include <optional>


class SelectStatement
{
    QList<Expression> column_exprs; // columns
    std::shared_ptr<ConditionalExpression> conditional_expr; // where clause
    std::shared_ptr<ConditionalExpression> on_clause; // on clause for joins

    std::shared_ptr<CSVFile> out_file; // output file

    std::shared_ptr<CSVFile> left_file;
    std::shared_ptr<CSVFile> right_file;

    QMap<QString, QString> join_files_list; // {'left' : left_file_name, 'right' : right_file_name}

    QList<Token> tokens; // select statement tokens excluding select. Last token is a semi-colon
    QList<Token>::const_iterator last_token_pos;

    double query_index; // holds the column index for right hand side file; this is used in building the lookup index

    bool has_join;
    bool has_where_clause;
    TokenType join_type;
    bool write_to_file;
    void throw_exception_if_unexpected_end();

    void handle_into_clause();
    void handle_inner_join();
    void handle_outer_join();
    void handle_cross_join();
    void handle_where_clause();
    void handle_groupby_clause();
    QMap<TokenType, std::function<void()>> optional_actions;

    QList<Expression> read_column_expressions();
    std::shared_ptr<CSVFile> read_file(QIODeviceBase::OpenMode m = QIODevice::ReadOnly);
    std::shared_ptr<ConditionalExpression> read_on_clause();
    std::shared_ptr<ConditionalExpression> read_where();

    void parse();
    QString selected_rows();
    QStringList compute_columns(const QMap<QString, QStringList>& data_rows);

    std::shared_ptr<QHash<QString,QList<qint64>>> build_index(const std::shared_ptr<CSVFile>& rhs, const int& column_index);

    std::optional<QList<QStringList>> select_with_no_join();
    std::optional<QList<QStringList>> select_with_inner_join();

public:
    SelectStatement(const QList<Token>& tks);

   // void execute(); //save result to file
    std::optional<QList<QStringList>> execute();
};

#endif // SELECTSTATEMENT_H
