#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QTextStream>
#include <QList>
#include <QHash>
#include <any>
//#include "term.h"
#include <functional>
//#include <map>


enum class TokenType  : char /* char16_t*/ {
    NAME, NUMBER, STRING, BOOLEAN, SELECT, FROM, WHERE, AND, OR, INTO, UPDATE, DELETE, IMPORT, END, COLUMNNAME, COLUMNNUMBER, ERROR, FUNCTION,
    ON, INNERJOIN, OUTERJOIN, CROSSJOIN, DOT, NOTEQUALTO, NOT, SET, GROUPBY,
    PLUS='+', MULT='*', MINUS='-', DIV='/', ASSIGN='=', LBRACKET='(', RBRACKET=')', LSQBRACKET='[', RSQBRACKET=']', COMMENT='#',
    SINGLEQOUTE='\'', DOUBLEQUOTE='"', COMMA=',', SEMICOLON=';', COLON=':', LESSTHAN='<', GREATERTHAN='>', LESSTHANOREQUAL, GREATERTHANOREQUAL
};




class Function{

    QList<TokenType> func_args;
    TokenType return_type;

public:
    Function(QList<TokenType> args, TokenType r_type):
        func_args(args), return_type(r_type)
    {}

    int num_of_args() const {
        return func_args.length();
    }

    void add_arg(TokenType type){
        func_args.append(type);
    }


    QList<TokenType> get_args() const {
        return func_args;
    }

    TokenType get_return_type() const {
        return return_type;
    }
};


struct Result{
    bool successful;
    bool hasOutput;
    QString error;
    QString output;
};

struct ColumnResult{
    TokenType token_type; //possible values: TokenType::STRING, TokenType::NUMBER, and TokenType::ERROR
    QString error;

    QString string_value;
    double number_value;
    bool boolean_value;
    double line_number;
};

class Term;

struct Token{
    TokenType token_type;

    QString string_value;
    double number_value;

    QList<Token> func_args;
    std::function<Term(QList<Term>)> func;

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

//extern QHash<QString, int> func_args_number_table; // information on the number of arguments for a function

extern QHash<QString, QList<TokenType>> func_args_type_list; // returns list of function argument type list

extern QHash<QString, TokenType> symbol_table; //possible types, STRING, NUMBER, FUNCTION, COLUMNAME

extern QHash<QString, int> out_file_use_count; //select out file, delete file, update file use count; enable future concurrency?



#endif // TYPES_H
