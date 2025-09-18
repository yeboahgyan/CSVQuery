#include <QCoreApplication>
#include <QHash>
#include <QList>
#include <QString>
#include <QTextStream>
#include <QMap>
#include <QDebug>

#include "tokenizer.h"
#include "functions.h"
#include "Expression.h"
#include "conditionalexpression.h"
#include "importstatement.h"
#include "assignstatement.h"

void test_tokenizer();

void test_expression(QString& source, const QMap<QString, QStringList>& data_rows);

void test_conditional_expression(QString& source, const QMap<QString, QStringList>& data_rows);

void test_import(QString& source);

void test_import_and_assigment();

void set_builtin_funcs();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Set up code that uses the Qt event loop here.
    // Call a.quit() or a.exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    // If you do not need a running Qt event loop, remove the call
    // to a.exec() or use the Non-Qt Plain C++ Application template.

    set_builtin_funcs();

    try{
        QStringList row = {"empty string", "20", "hello"};
        QMap<QString, QStringList> data_rows;
        data_rows["$"] = row;

        //QString source = "2+2,\n left('super', 3),\n 4*3,\n 3*length('super')";
        //QString source1 = "*,[0], 2+2, 5+2,(2+10)/2,\nlength([1]+ [1]),length(trim(' kwame'))";
        //test_expression(source1, data_rows);


        //QString source = "([2]='hello') and -1 !=-1*-1";
        //test_conditional_expression(source, data_rows);

        //test_tokenizer();

        //QString source = "import 'D:/Downloads/test_input/courses.def' as courses;";
        //test_import(source);

        test_import_and_assigment();
    }
    catch(std::logic_error l){
        std::cout<<"Error "<<l.what()<<"\n";
        }
    //test_tokenizer();

    return 0;
}

void test_tokenizer()
{
    QString source = "import 'hello.def'; select *.araba, *, file.0, 2*(fi), [0] from x #where [len] <= 2;\n\n\n  delete from x where [0] > 20 and [1] = 'hello'; select table.* from y\n\n";
    std::shared_ptr<QTextStream> stream = std::make_shared<QTextStream>(&source);

    Tokenizer tokenizer(stream);

    Token token = tokenizer.get();
    while(token.token_type != TokenType::END){
        if(token.token_type == TokenType::SEMICOLON){
            std::cout<<";%%\n";
        }else{
            std::cout<< token << "{" <<token.string_value.toStdString() <<"} ";
        }
        token = tokenizer.get();
    }
    std::cout<< token << "\n";
}

void test_expression(QString& source, const QMap<QString, QStringList>& data_rows)
{
    //QString source = "2+2,\n left('super', 3),\n 4*3,\n 3*length('super')";
    //QString source = "*,[0], 2+2, 5+2,(2+10)/2,\nlength([1]+ [1]),length(trim(' kwame'))";
    std::shared_ptr<QTextStream> stream = std::make_shared<QTextStream>(&source);

    Tokenizer tokenizer(stream);
    QList<Term> terms;
    QList<Expression> exps;

    Token token = tokenizer.get();
    while(token.token_type != TokenType::END){
        if(token.token_type == TokenType::SEMICOLON){
            std::cout<<";%%\n";
        }else{
            std::cout<< token << "{" <<token.string_value.toStdString() <<"} ";
        }

        if(token.token_type == TokenType::COMMA){
            Expression exp(terms);
            exps.append(exp);
            terms = {};
        }
        else{

            Term t(token);
            terms.append(t);
        }
        token = tokenizer.get();
    }

    if(token.token_type == TokenType::END){
        Expression exp(terms);
        exps.append(exp);
    }
    std::cout<< token << "\n"; //print END token

    //evaluate expressions
    //QStringList row = {"empty string", "20", "hello"};
    QList<Term> results;
    foreach (auto e, exps) {
        //if(e.is_star()){
        //    results.append(e.eval_star_term(row));
        //}else{
            QString num = QString::number(e.number_of_terms());
            std::cout<<"evaluating expression with " << num.toStdString() <<" terms... \n";
            results.append(e.eval(data_rows));

        //}
    }

    //print results
    foreach(auto t, results){
        std::cout<<"Answer: "<<t.get_token().to_string().toStdString()<<"(";
        if(t.get_token().token_type == TokenType::NUMBER){
            std::cout<<t.get_token().number_value<<")";
        }
        else if(t.get_token().token_type == TokenType::STRING){
            std::cout<<t.get_token().string_value.toStdString()<<")";
        }
        std::cout<< "\n";
    }
}

void test_conditional_expression(QString& source, const QMap<QString, QStringList>& data_rows)
{
    //QString source = "[2]='hello'";
    std::shared_ptr<QTextStream> stream = std::make_shared<QTextStream>(&source);

    Tokenizer tokenizer(stream);
    QList<Term> terms;
    //QList<Expression> exps;

    // Read and Print tokens //
    Token token = tokenizer.get();
    while(token.token_type != TokenType::END){
        if(token.token_type == TokenType::SEMICOLON){
            std::cout<<";%%\n";
            break;
        }else{
            std::cout<< token << "{" <<token.string_value.toStdString() <<"} ";
        }

        Term t(token);
        terms.append(t);
        token = tokenizer.get();
    }

    ConditionalExpression cond_expr(terms);

    std::cout<< token << "\n"; //print END token

    //evaluate expressions
    //QStringList row = {"empty string", "20", "hello"};
    //std::cout<<"evaluating cond_expr.eval(row)\n";
    Term result = cond_expr.eval(data_rows);
    //std::cout<<"done.\n";

    //Print result
    std::cout<<"Answer: "<<result.get_token().to_string().toStdString()<<"(";
    if(result.get_token().token_type == TokenType::BOOLEAN){
        QString str = (result.get_token().boolean_value == true)? "TRUE" : "FALSE";
        std::cout<<str.toStdString()<<")";
    }

}

void test_import(QString& source){
    std::shared_ptr<QTextStream> stream = std::make_shared<QTextStream>(&source);
    Tokenizer tokenizer(stream);
    QList<Term> terms;
    QList<Token> tokens;
    //QList<Expression> exps;

    // Read and Print tokens //
    Token token = tokenizer.get();
    while(token.token_type != TokenType::END){
        if(token.token_type == TokenType::SEMICOLON){
            std::cout<<";%%\n";
            break;
        }else{
            std::cout<< token << "{" <<token.string_value.toStdString() <<"} ";
        }

        //Term t(token);
        tokens.append(token);
        token = tokenizer.get();
    }

    ImportStatement import(tokens);
    import.execute();

    qDebug()<<import_defs;
}

void test_import_and_assigment()
{
    QString source = "import 'D:/Downloads/test_input/courses.def' as courses; \n a:courses = 'D:/Downloads/test_input/hello.csv';";

    std::shared_ptr<QTextStream> stream = std::make_shared<QTextStream>(&source);
    Tokenizer tokenizer(stream);
    QList<Term> terms;
    QList<Token> tokens;
    //QList<Expression> exps;

    // Read import statement and Print tokens //
    Token token = tokenizer.get();
    while(token.token_type != TokenType::END){
        if(token.token_type == TokenType::SEMICOLON){
            std::cout<<";%%\n";
            tokens.append(token);
            break;
        }else{
            std::cout<< token << "{" <<token.string_value.toStdString() <<"} ";
        }

        //Term t(token);
        tokens.append(token);
        token = tokenizer.get();
    }

    ImportStatement import(tokens);
    import.execute();

    qDebug()<<"Imported defs: "<<import_defs;

    // Read assigment statement and Print tokens //
    token = tokenizer.get();
    tokens ={};
    while(token.token_type != TokenType::END){
        if(token.token_type == TokenType::SEMICOLON){
            std::cout<<";%%\n";
            tokens.append(token);
            break;
        }else{
            std::cout<< token << "{" <<token.string_value.toStdString() <<"} ";
        }

        //Term t(token);
        tokens.append(token);
        token = tokenizer.get();
    }

    qDebug()<<"Evaluating assigment";

    AssignStatement assignment(tokens);
    assignment.execute();
    qDebug()<<"columns table"<<columns_table;
    qDebug()<<"symbols table";

    foreach (auto s, symbol_table.keys()) {
        TokenType t = symbol_table[s];
        Token tk(t);
        qDebug()<<s <<": "<<tk.to_string();
    }

    qDebug()<<"Strings table: "<<strings_table;
    qDebug()<<"numbers table: "<<numbers_table;
}

void set_builtin_funcs()
{
    //extern QHash<QString, std::function<Term(QList<Term>)> > funcs_table; // function name, "pointer to function"


    //extern QHash<QString, QList<TokenType>> func_args_type_list; // returns list of function argument type list


    symbol_table["trim"] = TokenType::FUNCTION;
    symbol_table["length"] = TokenType::FUNCTION;
    symbol_table["substring"] = TokenType::FUNCTION;
    symbol_table["left"] = TokenType::FUNCTION;
    symbol_table["right"] = TokenType::FUNCTION;
    symbol_table["date_gt"] = TokenType::FUNCTION;
    symbol_table["date_lt"] = TokenType::FUNCTION;
    symbol_table["date_ge"] = TokenType::FUNCTION;
    symbol_table["date_le"] = TokenType::FUNCTION;
    symbol_table["date_eq"] = TokenType::FUNCTION;

    std::function<Term(QList<Term>)> _trim = trim;
    std::function<Term(QList<Term>)> _length = length;
    std::function<Term(QList<Term>)> _substring = substring;
    std::function<Term(QList<Term>)> _left = left;
    std::function<Term(QList<Term>)> _right = right;
    std::function<Term(QList<Term>)> _date_gt = date_gt;
    std::function<Term(QList<Term>)> _date_lt = date_lt;
    std::function<Term(QList<Term>)> _date_ge = date_ge;
    std::function<Term(QList<Term>)> _date_le = date_le;
    std::function<Term(QList<Term>)> _date_eq = date_eq;

    funcs_table["trim"] = _trim;
    funcs_table["length"] = _length;
    funcs_table["substring"] = _substring;
    funcs_table["left"] = _left;
    funcs_table["right"] = _right;
    funcs_table["date_gt"] = _date_gt;
    funcs_table["date_lt"] = _date_lt;
    funcs_table["date_ge"] = _date_ge;
    funcs_table["date_le"] = _date_le;
    funcs_table["date_eq"] = _date_eq;

    QList<TokenType> only_string_arg = {TokenType::STRING};
    QList<TokenType> string_and_number = {TokenType::STRING, TokenType::NUMBER};
    QList<TokenType> str_num_num = {TokenType::STRING, TokenType::NUMBER, TokenType::NUMBER};
    QList<TokenType> date_comp_arg_types = {TokenType::STRING, TokenType::STRING, TokenType::STRING, TokenType::STRING};

    func_args_type_list["trim"] = only_string_arg;
    func_args_type_list["length"] = only_string_arg;
    func_args_type_list["substring"] = str_num_num;
    func_args_type_list["left"] = string_and_number;
    func_args_type_list["right"] = string_and_number;
    func_args_type_list["date_gt"] = date_comp_arg_types;
    func_args_type_list["date_lt"] = date_comp_arg_types;
    func_args_type_list["date_ge"] = date_comp_arg_types;
    func_args_type_list["date_le"] = date_comp_arg_types;
    func_args_type_list["date_eq"] = date_comp_arg_types;


}
