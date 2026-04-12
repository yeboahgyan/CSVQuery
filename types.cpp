#include "types.h"
#include <QHash>


namespace csvquery {

    QHash<QString, int> columns_table; // {COLUMNNANME, index}

    QHash<QString, double> numbers_table; // {name, number}

    QHash<QString, QString> strings_table; // {name, string}

    QHash<QString, std::function<Term(QList<Term>)> > funcs_table; // function name, "pointer to function"

    QHash<QString,
        std::function< std::function<Term(QList<Term>)>(QList<Term>)> >
        funcs_compiler_table; // "pointer to function" "pointer to function compiler"

    QHash<QString, TokenType> symbol_table; //possible types, STRING, NUMBER, FUNCTION, COLUMNAME

    QHash<QString, int> out_file_use_count; //select out file, delete file, update file use count; enable future concurrency?

    //QHash<QString, int> func_args_number_table; // information on the number of arguments for a function

    QHash<QString, QList<TokenType>> func_args_type_list; // returns list of function argument type list

    QHash<QString, QList<QString>> import_defs; // def and column list; import statement loads data into it; read during annotated assigments

    // This is used for tab completion in main.cpp
    std::vector<std::string> commands = {
            "SELECT", "FROM", "UPDATE", "IMPORT", "WHERE", "NOT LIKE",
            "INTO", "INNER JOIN", "OUTER JOIN", "ON", "SET", "LIKE", "HAVING",
            "TRIM", "LENGTH", "SUBSTRING", "LEFT", "RIGHT", "AS", "LIMIT",
            "DATE_GT", "DATE_GE", "DATE_LT", "DATE_LE", "DATE_EQ", "STRIP_QUOTES",
            "QUIT", "EXIT", "NUMBER", "GROUP BY", "COUNT", "SUM", "AVG", "MIN", "MAX"
    };
    
	//QString aggregate_expression_reg_key; // This is set within SelectStatement class and  used in aggregate functions. It is $ if there is no group by, otherwise it is the group by column values separated by commas. 
    //QMap<QString, QMap<QString, std::shared_ptr<AggregateCounter> > > aggregate_expression_reg; // select statement initiates this with aggregate functions; it also empties it when done
    //QMap<QString, QMap<QString, bool>> check_if_aggregate_done; // used to prevent multiple aggregation in a select execution loop

    QString aggregate_expression_reg_key; // This is set within SelectStatement class and  used in aggregate functions. It is $ if there is no group by, otherwise it is the group by column values separated by commas. 
    QHash<QString, std::shared_ptr<AggregateCounter> > aggregate_expression_reg; // select statement initiates this with aggregate functions; it also empties it when done
    QHash<QString, bool> check_if_aggregate_done; // used to prevent multiple aggregation in a select execution loop



    qint64 NUMBER_OF_ROWS_IN_CSV = 0;
    QHash<bool, qint64> eq_count; //debugging


    QString Token::to_string() const
    {
        QString str;

        switch (token_type) {
        case TokenType::NAME: { str = "TokenType::NAME"; break; }
        case TokenType::NUMBER: { str = "TokenType::NUMBER"; break; }
        case TokenType::STRING: { str = "TokenType::STRING"; break; }
        case TokenType::BOOLEAN: { str = "TokenType::BOOLEAN"; break; }
        case TokenType::SELECT: { str = "TokenType::SELECT"; break; }
        case TokenType::FROM: { str = "TokenType::FROM"; break; }
        case TokenType::WHERE: { str = "TokenType::WHERE"; break; }
        case TokenType::AND: { str = "TokenType::AND"; break; }
        case TokenType::OR: { str = "TokenType::OR"; break; }
        case TokenType::INTO: { str = "TokenType::INTO"; break; }
        case TokenType::UPDATE: { str = "TokenType::UPDATE"; break; }
        case TokenType::DELETE: { str = "TokenType::DELETE"; break; }
        case TokenType::IMPORT: { str = "TokenType::IMPORT"; break; }
        case TokenType::END: { str = "TokenType::END"; break; }
        case TokenType::COLUMNNAME: { str = "TokenType::COLUMNNAME"; break; }
        case TokenType::COLUMNNUMBER: { str = "TokenType::COLUMNNUMBER"; break; }
        case TokenType::ERROR: { str = "TokenType::ERROR"; break; }
        case TokenType::FUNCTION: { str = "TokenType::FUNCTION"; break; }
        case TokenType::ON: { str = "TokenType::ON"; break; }
        case TokenType::INNERJOIN: { str = "TokenType::INNERJOIN"; break; }
        case TokenType::OUTERJOIN: { str = "TokenType::OUTERJOIN"; break; }
        case TokenType::CROSSJOIN: { str = "TokenType::CROSSJOIN"; break; }
        case TokenType::DOT: { str = "TokenType::DOT"; break; }
        case TokenType::NOTEQUALTO: { str = "TokenType::NOTEQUALTO"; break; }
        case TokenType::NOT: { str = "TokenType::NOT"; break; }
        case TokenType::SET: { str = "TokenType::SET"; break; }
        case TokenType::PLUS: { str = "TokenType::PLUS"; break; }
        case TokenType::MULT: { str = "TokenType::MULT"; break; }
        case TokenType::MINUS: { str = "TokenType::MINUS"; break; }
        case TokenType::ASSIGN: { str = "TokenType::ASSIGN"; break; }
        case TokenType::DIV: { str = "TokenType::DIV"; break; }
        case TokenType::LBRACKET: { str = "TokenType::LBRACKET"; break; }
        case TokenType::RBRACKET: { str = "TokenType::RBRACKET"; break; }
        case TokenType::LSQBRACKET: { str = "TokenType::LSQBRACKET"; break; }
        case TokenType::RSQBRACKET: { str = "TokenType::RSQBRACKET"; break; }
        case TokenType::COMMENT: { str = "TokenType::COMMENT"; break; }
        case TokenType::SINGLEQOUTE: { str = "TokenType::SINGLEQOUTE"; break; }
        case TokenType::DOUBLEQUOTE: { str = "TokenType::DOUBLEQUOTE"; break; }
        case TokenType::COMMA: { str = "TokenType::COMMA"; break; }
        case TokenType::SEMICOLON: { str = "TokenType::SEMICOLON"; break; }
        case TokenType::COLON: { str = "TokenType::COLON"; break; }
        case TokenType::LESSTHAN: { str = "TokenType::LESSTHAN"; break; }
        case TokenType::GREATERTHAN: { str = "TokenType::GREATERTHAN"; break; }
        case TokenType::LESSTHANOREQUAL: { str = "TokenType::LESSTHANOREQUAL"; break; }
        case TokenType::GREATERTHANOREQUAL: { str = "TokenType::GREATERTHANOREQUAL"; break; }
        case TokenType::GROUPBY: { str = "TokenType::GROUPBY"; break; }
        case TokenType::DOLLAR: { str = "TokenType::DOLLAR"; break; }
        case TokenType::AS: { str = "TokenType::AS"; break; }
        case TokenType::LIKE: { str = "TokenType::LIKE"; break; }
        case TokenType::NOTLIKE: { str = "TokenType::NOTLIKE"; break; }
        case TokenType::LIMIT : { str = "TokenType::LIMIT"; break; }
        case TokenType::HAVING: { str = "TokenType::HAVING"; break; }
        }

        return str;
    }

}