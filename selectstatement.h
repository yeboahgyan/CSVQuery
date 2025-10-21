#ifndef SELECTSTATEMENT_H
#define SELECTSTATEMENT_H
#include <QFileInfo>
#include <QBuffer>
#include <memory>
#include "Types.h"
#include "conditionalexpression.h"
#include "csvfile.h"
#include <optional>


namespace csvquery {


    class SelectStatement
    {
        QList<Expression> column_exprs; // columns
        QList<Term> column_exprs_terms; // all terms in select columns except functions and function args; used in read_column_expressions
        std::shared_ptr<ConditionalExpression> conditional_expr; // where clause
        std::shared_ptr<ConditionalExpression> having_conditional_expr; // having clause in aggregate select 
        std::shared_ptr<ConditionalExpression> on_clause; // on clause for joins

        std::shared_ptr<CSVFile> out_file; // output file

        std::shared_ptr<CSVFile> left_file;
        std::shared_ptr<CSVFile> right_file;

        QMap<QString, QString> join_files_list; // {'left' : left_file_name, 'right' : right_file_name}

        QList<Token> tokens; // select statement tokens excluding select. Last token is a semi-colon
        QList<Token>::const_iterator last_token_pos;

        unsigned int NUMBER_OF_ROWS_PER_PAGE; //used for pagination
        unsigned int NUMBER_OF_ROWS = 0; //number of rows read
        unsigned int LIMIT_VAL = 0; // used for limit clause

        double query_index =0; // holds the column index for right hand side file; this is used in building the lookup index
        bool run_only_once = false; //used for when select is executed wihout a from clause

        bool has_from_clause = true;
        bool has_join = false;
        bool has_where_clause = false;

        bool has_having_clause = false;

        bool has_group_by = false;
        bool is_aggregation = false; //used for situations where there is no group by but there are aggregation functions in columnm list

        bool has_limit_clause = false;
        bool limit_done = false;

        TokenType join_type;
        bool write_to_file = false;
        void throw_exception_if_unexpected_end();

        Term read_join_column();
        QStringList column_names; //
        void save_column_names(QList<Term>& terms); // saves and modifies term with name for terms that are functions

        QStringList group_by_columns;
        //void read_group_by_columns();

        void handle_into_clause();
        void handle_inner_join();
        void handle_outer_join();
        void handle_cross_join();
        void handle_where_clause();
        void handle_groupby_clause();
        void handle_limit_clause();
        void handle_having_clause();
        QMap<TokenType, std::function<void()>> optional_actions;

        QList<Expression> read_column_expressions();
        std::shared_ptr<CSVFile> read_file(QIODeviceBase::OpenMode m = QIODevice::ReadOnly);
        std::shared_ptr<ConditionalExpression> read_on_clause();
        std::shared_ptr<ConditionalExpression> read_where();
        std::shared_ptr<ConditionalExpression> read_having_clause();

        bool paginate = false; // used for pagination of group by result
        QHash<QString, QStringList> group_by_result; // saves results if aggregation done in inner joins and outer joins
        QHash<QString, QStringList>::iterator group_by_result_loc; // this is used for pagination; keeps track of the next item
        std::shared_ptr<QHash<QString, QList<qint64>> > query_lookup_index; // this is used for looking up index; it is initialized in the inner join or outer join function
        void validate_aggregate_query(); // checks if coulms in select statement are in group by clause and also whether it has an aggregation function

        void parse();
        //QString selected_rows();
        QStringList compute_columns(const QMap<QString, QStringList>& data_rows);

        std::shared_ptr<QHash<QString, QList<qint64>>> build_index(const std::shared_ptr<CSVFile>& rhs, const int& column_index);

        QString create_group_by_key(const QMap<QString, QStringList>& data_rows);

        std::optional<QList<QStringList>> select_with_no_join();
        std::optional<QList<QStringList>> select_with_inner_join();
        std::optional<QList<QStringList>> select_with_outer_join();
        void process_select(QList<QStringList>& result_ptr, QMap<QString, QStringList>& data_rows);
        void process_select(QHash<QString, QStringList>& group_by_result, QMap<QString, QStringList>& data_rows);

    public:
        SelectStatement(const QList<Token>& tks, unsigned int max_rows_per_page = 100);

        ~SelectStatement() {
            //clean up global data structures
            //qDebug() << "destructor called";
            aggregate_expression_reg = {};
            check_if_aggregate_done = {};
        }

        // void execute(); //save result to file
        std::optional<QList<QStringList>> execute();

        unsigned int get_number_of_rows() const {
            return NUMBER_OF_ROWS;
        }

        unsigned int get_max_rows_per_page() const {
            return NUMBER_OF_ROWS_PER_PAGE;
        }

        QStringList get_column_names() const {
            return column_names;
        }
    };

}
#endif // SELECTSTATEMENT_H
