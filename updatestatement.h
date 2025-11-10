#ifndef UPDATESTATEMENT_H
#define UPDATESTATEMENT_H

#include "expression.h"
#include "Types.h"
#include "conditionalexpression.h"
#include "csvfile.h"
#include <utility>
#include "CSVFile2.h"
#include <boost/lockfree/spsc_queue.hpp>


namespace csvquery {

    class UpdateStatement
    {
        const QList<Token> tokens;
        QList<Token>::const_iterator last_token_pos;
        QList<std::pair<Token, Expression>> column_update_list;
        std::shared_ptr<ConditionalExpression> where_expr; // where clause
        std::shared_ptr<CSVFile2> left_file;
        std::shared_ptr<CSVFile> out_file; // output file

        std::unique_ptr<std::thread> file_writer_thread = nullptr;

        std::unique_ptr<boost::lockfree::spsc_queue<QString, boost::lockfree::capacity<128>>> write_queue;

        bool has_where_clause = false;
        bool is_conditional_compiled = false;
        bool are_columns_compiled = false;

        unsigned int NUMBER_OF_AFFECTED_ROWS = 0;

        void throw_exception_if_unexpected_end();

        Token read_column();

        Expression read_expression();

        void read_column_update_list();

        std::shared_ptr<ConditionalExpression> read_where_clause();

        std::atomic<bool> done_producing = false; // Producer finished reading
        void csv_file_writer();

        std::shared_ptr<CSVFile2> read_file2(/*QIODeviceBase::OpenMode m = QIODevice::ReadOnly*/);
        std::shared_ptr<CSVFile> read_file(QIODeviceBase::OpenMode m = QIODevice::ReadOnly);

        void process_expression(Expression& rhs);

        void parse();

        std::function<QStringList(const QMap<QString, QStringList>&)> compile_update_list(const QMap<QString, QStringList>&);

        //compiled functions
        std::function<Term(const QMap<QString, QStringList>&)> compiled_conditional_func;
        std::function<QStringList(const QMap<QString, QStringList>&)> compiled_columns_func;



    public:
        UpdateStatement(const QList<Token>& tks);

        ~UpdateStatement() {
            if (file_writer_thread != nullptr) {
                //done producing
                done_producing.store(true, std::memory_order_release);
                file_writer_thread->join();
            }
        }

        void execute();

        unsigned int get_number_of_rows() const {
            return NUMBER_OF_AFFECTED_ROWS;
        }
    };

}

#endif // UPDATESTATEMENT_H
