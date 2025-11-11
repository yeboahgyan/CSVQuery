#ifndef SELECTSTATEMENT_H
#define SELECTSTATEMENT_H
#include <QFileInfo>
#include <QBuffer>
#include <memory>
#include "Types.h"
#include "conditionalexpression.h"
#include "csvfile.h"
#include <optional>
#include <mutex>
#include <thread>
#include <QFile>
#include "CSVFile2.h"
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/lockfree/queue.hpp>
#include <atomic>
#include <thread>
#include <queue>


namespace csvquery {


    class SelectStatement
    {
        QList<Expression> column_exprs; // columns
        QList<Term> column_exprs_terms; // all terms in select columns except functions and function args; used in read_column_expressions
        std::shared_ptr<ConditionalExpression> conditional_expr; // where clause
        std::shared_ptr<ConditionalExpression> having_conditional_expr; // having clause in aggregate select 
        std::shared_ptr<ConditionalExpression> on_clause; // on clause for joins

        std::shared_ptr<CSVFile> out_file; // output file

        std::unique_ptr<QFile> out_file2;

        std::unique_ptr<std::thread> file_reader_thread = nullptr;
        std::unique_ptr<std::thread> file_writer_thread = nullptr;

        //std::unique_ptr<std::thread> consumer_thread1;
        //std::unique_ptr<std::thread> consumer_thread2;
        //std::unique_ptr<std::thread> consumer_thread3;

        //const size_t BUFFER_SIZE = 32;
        const qsizetype BATCH_ROWS = 256;
        //boost::lockfree::spsc_queue<QList<csv::CSVRow>, boost::lockfree::capacity< 8'388'608 >> queue;
        
        std::unique_ptr<boost::lockfree::spsc_queue<std::vector<csv::CSVRow>, boost::lockfree::capacity<128>>> queue;
        std::unique_ptr<boost::lockfree::spsc_queue< std::vector<QStringList>, boost::lockfree::capacity<128>>> write_queue;

        const qsizetype WRITE_BATCH_ROWS = 256;
        std::vector<QStringList> write_batch;
        void push_to_write_queue(const QStringList& row);
		void flush_write_queue(); //writes partail batch to write queue; used in destructor

        //std::unique_ptr<std::queue<QList<csv::CSVRow>> > queue;
        //std::unique_ptr<std::queue<QString> > write_queue; //used by file writer thread

        std::vector<csv::CSVRow> reserve_batch; //used to save unprocessed rows in when pagination is done

        //const qsizetype PROCESS_BATCH_ROWS = 128;
        //boost::lockfree::queue<QList<QStringList>*> processing_queue{64}; // 128 rows / 64 batches
        std::atomic<bool> query_done{ false };
        std::atomic<bool> read_done{ false };

        std::atomic<bool> done_producing = false; // Producer finished reading
        std::atomic<bool> done_consuming = false; // Consumer finished query
        std::atomic<bool> paused = false;         // Paused (UI viewing)
        std::atomic<bool> canceled = false;       // User canceled

        std::mutex mtx;
        std::condition_variable cv;
        //bool paused = false;
        bool should_stop = false;


        //bool main_thread_signal = false;
        //bool process_thread_signal = false;
        //bool writer_thread_signal = false;

        std::optional<QList<QStringList>> result; // query result
        //QHash<QString, QStringList> group_by_result; // group by query result

        //std::shared_ptr<CSVFile> left_file;
        //std::shared_ptr<CSVFile> right_file;

        std::shared_ptr<CSVFile2> left_file2; // CVSFile2 uses Vince csv-parser
        std::shared_ptr<CSVFile2> right_file2;
        //std::shared_ptr<QFile> right_file2;

        QMap<QString, QString> join_files_list; // {'left' : left_file_name, 'right' : right_file_name}

        QList<Token> tokens; // select statement tokens excluding select. Last token is a semi-colon
        QList<Token>::const_iterator last_token_pos;

        unsigned int NUMBER_OF_ROWS_PER_PAGE; //used for pagination
        unsigned int NUMBER_OF_ROWS = 0; //number of rows read
        unsigned int LIMIT_VAL = 0; // used for limit clause

        int query_index =0; // holds the column index for right hand side file; this is used in building the lookup index
		int left_query_index = 0; // holds the column index for left hand side file; this is used in building the lookup index
        bool run_only_once = false; //used for when select is executed wihout a from clause

        bool has_from_clause = true;
        bool has_join = false;
        bool has_where_clause = false;

        bool has_having_clause = false;

        bool has_group_by = false;
        bool is_aggregation = false; //used for situations where there is no group by but there are aggregation functions in columnm list

        bool has_limit_clause = false;
        bool limit_done = false;

        bool is_conditional_compiled = false;
        bool are_columns_compiled = false;
        bool is_group_by_key_compiled = false;
        bool is_having_conditional_compiled = false;
        bool is_indexing_done = false;

        bool join_paginate = false; // used in inner and outer joins to help resume from index container if not all elements for key was processed
        QList<QStringList>::iterator rhs_row_it; // stores next read element iterator used with join_paginate
        QList < QStringList> matching_rhs_rows; // holds any matching rhs rows that were not processed before pause
        QMap<QString, QStringList> outstanding_data_rows; // used for continuation in inner and outer joins when pagination limit reached without processing all rows in rhs

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

		void set_query_index(const Term& t, int& file_index); //updates query_index or left_query_index based on file_index value

        QList<Expression> read_column_expressions();
        std::shared_ptr<CSVFile> read_file(QIODeviceBase::OpenMode m = QIODevice::ReadOnly);
        std::shared_ptr<CSVFile2> read_file2(); // used to sets left_file2 or right_file2
        void read_out_file(); // sets std::unique_ptr<QFile> out_file2;
        std::shared_ptr<ConditionalExpression> read_on_clause();
        std::shared_ptr<ConditionalExpression> read_where();
        std::shared_ptr<ConditionalExpression> read_having_clause();

        bool paginate = false; // used for pagination of group by result
        QHash<QString, QStringList> group_by_result; // saves results if aggregation done in inner joins and outer joins
        QHash<QString, QStringList>::iterator group_by_result_loc; // this is used for pagination; keeps track of the next item
        std::shared_ptr<QHash<QString, QList<qint64>> > query_lookup_index; // this is used for looking up index; it is initialized in the inner join or outer join function
        std::shared_ptr<QHash<QString, QList<QStringList>> > query_lookup_index2; // this is used for looking up index; it is initialized in the inner join or outer join function


        void validate_aggregate_query(); // checks if coulms in select statement are in group by clause and also whether it has an aggregation function

        bool process_data(const QMap<QString, QStringList>& data_rows); // this returns true to indicate that stop processing loop and return result
        //void consume_data(); // this is run in anothor thread to process the csv rows read
        void csv_file_reader(); // this is run in another thread to read csv rows from file
        void csv_file_writer();// this is run in another thread to write result rows to file by popping strings in write_queue

        void parse();
        //QString selected_rows();
        QStringList compute_columns(const QMap<QString, QStringList>& data_rows);
        std::function<QStringList(const QMap<QString, QStringList>&)> compile_columns(const QMap<QString, QStringList> data_rows);

        std::shared_ptr<QHash<QString, QList<qint64>>> build_index(const std::shared_ptr<CSVFile>& rhs, const int& column_index);
        std::shared_ptr<QHash<QString, QList<QStringList>> > build_index2(const std::shared_ptr<CSVFile2>& rhs, const int& column_index);

        QString create_group_by_key(const QMap<QString, QStringList>& data_rows);
        std::function < QString(const QMap<QString, QStringList>&)> compile_group_by_key(const QMap<QString, QStringList>& data_rows);

        std::optional<QList<QStringList>> select_with_no_join();
        //std::optional<QList<QStringList>> select_with_inner_join();
        //std::optional<QList<QStringList>> select_with_outer_join();

        std::optional<QList<QStringList>> select_with_inner_join2();
        std::optional<QList<QStringList>> select_with_outer_join2();

        void process_select(QList<QStringList>& result_ptr, QMap<QString, QStringList>& data_rows);
        void process_select(QHash<QString, QStringList>& group_by_result, QMap<QString, QStringList>& data_rows);

        //compiled functions
        std::function<Term(const QMap<QString, QStringList>&)> compiled_conditional_func;
        std::function<Term(const QMap<QString, QStringList>&)> compiled_having_conditional_func;
        std::function < QString(const QMap<QString, QStringList>&)> compiled_group_by_key_func;
        std::function<QStringList(const QMap<QString, QStringList>&)> compiled_columns_func;

        //these are used for debugging
        qint64 rows_consumed = 0;
        qint64 rows_produced = 0;
        qint64 batches_produced = 0;
        qint64 skipped_rows = 0;
        //end

        int number_of_columns_for_rhs_csv = 0; // this is used in outer join, where a row in lhs does not exist in rhs, to create the empty row list

        

    public:
        SelectStatement(const QList<Token>& tks, unsigned int max_rows_per_page = 100);

        ~SelectStatement() {
            //clean up global data structures
            //qDebug() << "destructor called";
            aggregate_expression_reg = {};
            check_if_aggregate_done = {};

            
            

            if (join_files_list.contains("left")) {
                done_consuming.store(true, std::memory_order_release);

                if (file_reader_thread != nullptr) {

                    if (file_reader_thread->joinable()) {
                        file_reader_thread->join();
                    }
                }
                
            }
            
            if (this->write_to_file) {
                if (file_writer_thread != nullptr) {
                    //flush_write_queue();
                    //qDebug() << "Waiting for file writer thread to join...";
                    done_consuming.store(true, std::memory_order_release);
                    if (file_writer_thread->joinable()) {
                        file_writer_thread->join();
                        //qDebug() << "File writer thread joined.";
                    }
                    //qDebug() << "File writer thread processing done.";
                }
				
            }
            
            NUMBER_OF_ROWS_IN_CSV = 0; // used for count(*)
            //qDebug() << "Rows produced: " << rows_produced << ", batches produced: " << batches_produced << ", rows consumed: " << rows_consumed;
            //qDebug() << "Skipped rows" << skipped_rows;
            //qDebug() << eq_count;
            //qDebug() << "destructor done.";
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

        void set_query_done(bool done) {
            query_done.store(done, std::memory_order_release);
        }

        // User requests "Next number of rows"
        void onNextPressed() {
            paused.store(false);
        }

        // User cancels query
        void onCancelPressed() {
            canceled.store(true);
            done_consuming.store(true);
        }

        void pause() {
            std::lock_guard<std::mutex> lock(mtx);
            paused = true;
            //std::cout << "Paused.\n";
        }

        void resume() {
            std::lock_guard<std::mutex> lock(mtx);
            paused = false;
            cv.notify_one();  // Wake up the waiting thread
            //std::cout << "Resumed.\n";
        }
    };

}
#endif  SELECTSTATEMENT_H
