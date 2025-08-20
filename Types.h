#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QTextStream>
#include <QList>
#include <QHash>
#include <any>
#include <functional>

enum class TokenType : char16_t {
    NAME, NUMBER, STRING, BOOLEAN, SELECT, FROM, WHERE, AND, OR, INTO, UPDATE, DELETE, IMPORT, END, COLUMNNAME, COLUMNNUMBER, ERROR, FUNCTION,
    PLUS='+', MULT='*', MINUS='-', DIV='/', ASSIGN='=', LBRACKET='(', RBRACKET=')', LSQBRACKET='[', RSQBRACKET=']',
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
};

struct Token{
    TokenType token_type;

    QString string_value;
    double number_value;

    QList<TokenType> func_args;
    std::function<ColumnResult(QList<TokenType>, QList<std::any>)> func;

    double line_number;
};



extern QHash<QString, double> columns_table; // {COLUMNNANME, index}

extern QHash<QString, double> numbers_table; // {name, number}

extern QHash<QString, QString> strings_table; // {name, string}

extern QHash<QString, std::function<ColumnResult(QList<TokenType>, QList<std::any>)> > funcs_table; // function name, "pointer to function"

extern QHash<QString, TokenType> symbol_table; //possible types, STRING, NUMBER, FUNCTION, COLUMNAME

extern QHash<QString, int> out_file_use_count; //select out file, delete file, update file use count; enable future concurrency?




#endif // TYPES_H
