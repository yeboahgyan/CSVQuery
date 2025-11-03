#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QTextStream>
#include <QList>
#include <QHash>
//#include <any>
//#include "term.h"
#include <functional>
#include <QMap>
#include <QDebug>
#include <limits>

namespace csvquery {

    enum class TokenType : char /* char16_t*/ {
        NAME, NUMBER, STRING, BOOLEAN, SELECT, FROM, WHERE, AND, OR, INTO, UPDATE, DELETE, IMPORT, END, COLUMNNAME, COLUMNNUMBER, ERROR, FUNCTION,
        ON, INNERJOIN, OUTERJOIN, CROSSJOIN, DOT, NOTEQUALTO, NOT, SET, GROUPBY, DOLLAR = '$', AS, LIKE, 
        PLUS = '+', MULT = '*', MINUS = '-', DIV = '/', ASSIGN = '=', LBRACKET = '(', RBRACKET = ')', LSQBRACKET = '[', RSQBRACKET = ']', COMMENT = '#',
        SINGLEQOUTE = '\'', DOUBLEQUOTE = '"', COMMA = ',', SEMICOLON = ';', COLON = ':', LESSTHAN = '<', GREATERTHAN = '>', LESSTHANOREQUAL, GREATERTHANOREQUAL,
        NOTLIKE, LIMIT, HAVING
    };




    class Function {

        QList<TokenType> func_args;
        TokenType return_type;

    public:
        Function(QList<TokenType> args, TokenType r_type) :
            func_args(args), return_type(r_type)
        {
        }

        int num_of_args() const {
            return func_args.length();
        }

        void add_arg(TokenType type) {
            func_args.append(type);
        }


        QList<TokenType> get_args() const {
            return func_args;
        }

        TokenType get_return_type() const {
            return return_type;
        }
    };


    struct Result {
        bool successful;
        bool hasOutput;
        QString error;
        QString output;
    };

    struct ColumnResult {
        TokenType token_type; //possible values: TokenType::STRING, TokenType::NUMBER, and TokenType::ERROR
        QString error;

        QString string_value;
        double number_value;
        bool boolean_value;
        double line_number;
    };

    class Term;

    struct Token {
        TokenType token_type;

        QString string_value;
        double number_value;

        QList<Token> func_args;
        std::function<Term(QList<Term>)> func;
        std::function<std::function<Term(QList<Term>)>(QList<Term>)> comp_func;

        double line_number;
        QString error_msg;
        QString token_name;

        QString to_string() const;

        bool boolean_value;
    };



    extern QHash<QString, double> columns_table; // {COLUMNNANME, index}

    extern QHash<QString, double> numbers_table; // {name, number}

    extern QHash<QString, QString> strings_table; // {name, string}

    extern QHash<QString, std::function<Term(QList<Term>)> > funcs_table; // function name, "pointer to function"

    extern QHash<QString,
        std::function< std::function<Term(QList<Term>)>(QList<Term>)> >
        funcs_compiler_table; // "pointer to function" "pointer to function compiler"

    //extern QHash<QString, int> func_args_number_table; // information on the number of arguments for a function

    extern QHash<QString, QList<TokenType>> func_args_type_list; // returns list of function argument type list

    extern QHash<QString, TokenType> symbol_table; //possible types, STRING, NUMBER, FUNCTION, COLUMNAME

    //extern QHash<QString, TokenType> variables_table; //possible types, STRING, NUMBER

    extern QHash<QString, int> out_file_use_count; //select out file, delete file, update file use count; enable future concurrency?

    extern QHash<QString, QList<QString>> import_defs; // def and column list; import statement loads data into it; read during annotated assigments


    // This is used for tab completion in main.cpp
    extern std::vector<std::string> commands;



    //Aggregate counters for Aggregate functions

    struct AggregateCounter {
        virtual void process_data(const QString& data) = 0;
        virtual double get_value() const = 0;
        virtual ~AggregateCounter() = default;
    };

    struct CountCounter : public AggregateCounter {
        double count = 0;

        void process_data(const QString& data) override {
            //qDebug() << "data: " << data;
            if (data != "") {
                ++count;
            }
        }

        double get_value() const override  {
            return count;
        }
    };

    struct SumCounter : public AggregateCounter {
        double sum = 0;

        void process_data(const QString& data) override {
            if (data != "") {
                bool is_number = false;
                double num = data.toDouble(&is_number);

                if (is_number) {
                    sum += num;
                }
            }
        }

        double get_value() const override {
            return sum;
        }
    };


    struct AvgCounter : public AggregateCounter {
        double sum = 0;
        double count = 0;

        void process_data(const QString& data) override {
            if (data != "") {
                bool is_number = false;
                double num = data.toDouble(&is_number);

                if (is_number) {
                    
                    sum += num;
                }
                ++count; //counts even non-numbers
            }
        }

        double get_value() const override {
            return (count != 0)? sum/count : 0;  //check for divide by zero
        }
    };

    struct MinCounter : public AggregateCounter {
        double min = std::numeric_limits<double>::max();

        void process_data(const QString& data) override {
            if (data != "") {
                bool is_number = false;
                double num = data.toDouble(&is_number);

                if (is_number) {

                    min = (min > num) ? num : min;
                }
            }
        }

        double get_value() const override {
            return min;
        }
    };

    struct MaxCounter : public AggregateCounter {
        double max = std::numeric_limits<double>::min();

        void process_data(const QString& data) override {
            if (data != "") {
                bool is_number = false;
                double num = data.toDouble(&is_number);

                if (is_number) {

                    max = (max > num) ? max : num;
                }
            }
        }

        double get_value() const override {
            return max;
        }
    };

    extern QString aggregate_expression_reg_key; // This is set within SelectStatement class and  used in aggregate functions 
    extern  QMap<QString, QMap<QString,std::shared_ptr<AggregateCounter> > > aggregate_expression_reg; // select statement initiates this with aggregate functions; it also empties it when done
    extern QMap<QString, QMap<QString, bool>> check_if_aggregate_done; // used to prevent multiple aggregation in a select execution loop {group_by_key, {function , bool}}

    extern qint64 NUMBER_OF_ROWS_IN_CSV; // This is use for count(*); counting is done in SelectStatement::process_data and reset to 0 in the destructor of SelectStatement

    extern QHash<bool, qint64> eq_count; //This is used for debugging conditional eq()
}

#endif // TYPES_H
