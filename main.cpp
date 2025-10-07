#include <QCoreApplication>
#include <QHash>
#include <QList>
#include <QString>
#include <QTextStream>
#include <QMap>
#include <QDebug>
#include <algorithm> // For std::transform
#include <cctype>    // For std::toupper
#include <regex>

#include "tokenizer.h"
#include "functions.h"
#include "Expression.h"
#include "conditionalexpression.h"
#include "importstatement.h"
#include "assignstatement.h"
#include "csvfile.h"
#include "selectstatement.h"
#include "updatestatement.h"
#include "types.h"
#include "parser.h"
#include <QTextStream>
//#include "pretty.h"

#include <tabulate/table.hpp>
//using namespace tabulate;

//#include "tabulate.h"

#include <QFileInfo>


//import pretty;
#include <replxx.hxx>

void test_tokenizer();

void test_expression(QString& source, const QMap<QString, QStringList>& data_rows);

void test_conditional_expression(QString& source, const QMap<QString, QStringList>& data_rows);

void test_import(QString& source);

void test_import_and_assigment();

void test_csv_parser(const QString& csv);

QList<csvquery::Token> read_statement(csvquery::Tokenizer& t);

void test_select_statement();

void set_builtin_funcs();

//void print(const std::optional<QList<QStringList>>& res);

void print_table(const std::optional<QList<QStringList>>& res);

void paginate(csvquery::SelectStatement& select, replxx::Replxx& rx);

void execute_source_file(QFile& source);

int MAX_ROWS_PER_PAGE = 100;

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

    std::cout << "\n\n";
    std::cout << "\t\t\t\t CSVQuery\n\n";
    std::cout << "\t\t\t\t[version 0.1.1]\n\n";
    std::cout << "\t\t\t\tKwame Yeboah-Gyan, 2025\n\n\n";

    set_builtin_funcs();

    try{
        //QStringList row = {"empty string", "20", "hello"};
        //QMap<QString, QStringList> data_rows;
        //data_rows["$"] = row;

        //QString source = "2+2,\n left('super', 3),\n 4*3,\n 3*length('super')";
        //QString source1 = "*,[0], 2+2, 5+2,(2+10)/2,\nlength([1]+ [1]),length(trim(' kwame'))";
        //test_expression(source1, data_rows);


        //QString source = "([2]='hello') and -1 !=-1*-1";
        //test_conditional_expression(source, data_rows);

        //test_tokenizer();

        //QString source = "import 'D:/Downloads/test_input/courses.def' as courses;";
        //test_import(source);

        //test_import_and_assigment();

        //QString csv = "D:/Downloads/2018-census-totals-by-topic-national-highlights-csv/sex-2018-census-csv.csv";

        //test_csv_parser(csv);
        //test_select_statement();

        //std::cout<<"CSVQuery 0.1\n\n\n";
        //std::cout << ">>";
        //std::string input;
        //std::cin >> input;

        if (a.arguments().length() > 1) {
            QString source_path = a.arguments().at(1);

            QFileInfo fileInfo(source_path);

            if (!fileInfo.exists()) {
                std::cout << "The provided file does not exist!" << std::endl;
                return 0;
            }

            QFile f(source_path);

            if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                std::cout << "Failed to open the file!"<< std::endl;
                return -1;
            }

            execute_source_file(f);
            return 0;
        }

        replxx::Replxx rx;

        std::vector<std::string> keywords = {
        "SELECT", "FROM", "UPDATE", "IMPORT", "WHERE",
        "INTO", "INNER JOIN", "OUTER JOIN", "ON", "SET", "LIKE", "NOT LIKE",
        "TRIM", "LENGTH", "SUBSTRING", "LEFT", "RIGHT", "AS",
        "DATE_GT", "DATE_GE", "DATE_LT", "DATE_LE", "DATE_EQ",
        "QUIT", "EXIT", "AND", "OR"
        };
        

        // Completion callback
        rx.set_completion_callback(
            [/*&commands */ ](std::string const& context, int& contextLen) -> replxx::Replxx::completions_t {
                replxx::Replxx::completions_t results;

                // Find the current "word" (last token)
                size_t start = context.find_last_of(" \t\n");
                std::string prefix = (start == std::string::npos) ? context : context.substr(start + 1);
                contextLen = static_cast<int>(prefix.size());

                std::transform(prefix.begin(), prefix.end(), prefix.begin(),
                    [](unsigned char c) { return std::toupper(c); });

                // Suggest matches
                for (auto const& cmd : csvquery::commands) {
                    if (cmd.compare(0, prefix.size(), prefix) == 0) {
                        results.push_back(cmd);
                    }
                }
                return results;
            }
        );

        // Syntax highlighter
        rx.set_highlighter_callback(
            [&keywords](std::string const& context, replxx::Replxx::colors_t& colors) {
                colors.resize(context.size(), replxx::Replxx::Color::DEFAULT);

                // Highlight SQL-like keywords (case-insensitive)
                for (auto const& cmd : keywords) {
                    std::regex word("\\b" + cmd + "\\b", std::regex_constants::icase);
                    auto words_begin = std::sregex_iterator(context.begin(), context.end(), word);
                    auto words_end = std::sregex_iterator();

                    for (auto it = words_begin; it != words_end; ++it) {
                        auto pos = it->position();
                        auto len = it->length();
                        for (int i = 0; i < len; ++i) {
                            colors[pos + i] = replxx::Replxx::Color::BRIGHTCYAN;
                        }
                    }
                }
                
                // Highlight numbers
                /*
                std::regex num(R"((?<![A-Za-z_])\d+(?![A-Za-z_]))");
                //std::regex num(R"((?<![A-Za-z_])\d+(?![A-Za-z_]))");
                auto nums_begin = std::sregex_iterator(context.begin(), context.end(), num);
                auto nums_end = std::sregex_iterator();

                for (auto it = nums_begin; it != nums_end; ++it) {
                    auto pos = it->position();
                    auto len = it->length();
                    for (int i = 0; i < len; ++i) {
                        colors[pos + i] = replxx::Replxx::Color::BRIGHTBLUE;
                    }
                }*/

                /*
                // Highlight columns [0]
                std::regex column(R"(\[\d+\])");
                auto column_begin = std::sregex_iterator(context.begin(), context.end(), column);
                auto column_end = std::sregex_iterator();

                for (auto it = column_begin; it != column_end; ++it) {
                    auto pos = it->position();
                    auto len = it->length();
                    for (int i = 0; i < len; ++i) {
                        colors[pos + i] = replxx::Replxx::Color::LIGHTGRAY;
                    }
                }*/
                
                // Highlight strings
                std::regex strings(R"((['"])([^'"]*)\1)");
                auto strings_begin = std::sregex_iterator(context.begin(), context.end(), strings);
                auto strings_end = std::sregex_iterator();

                for (auto it = strings_begin; it != strings_end; ++it) {
                    auto pos = it->position();
                    auto len = it->length();
                    for (int i = 0; i < len; ++i) {
                        colors[pos + i] = replxx::Replxx::Color::BROWN;
                    }
                }

                

                // Find the first occurrence of '#'
                std::size_t pos = context.find('#');
                if (pos != std::string::npos) {

                    // Clear existing colors
                    //std::fill(colors.begin(), colors.end(), replxx::Replxx::Color::DEFAULT);

                    // Mark everything from '#' to end of line as a comment
                    for (std::size_t i = pos; i < context.size(); ++i) {
                        colors[i] = replxx::Replxx::Color::GREEN; 
                    }
                }
            }
        );

        std::string buffer;
        std::string newlines;

        int line_number = 1;
        while (true) {
            std::string prompt_main = "\n" + std::to_string(line_number) + " csvQ> ";
            std::string prompt_continue = std::to_string(line_number) + "->] ";
            const char* input = rx.input(buffer.empty() ? prompt_main : prompt_continue);

            if (!input) break;  // EOF

            if (buffer.empty()) {
                buffer = newlines; //help parser keep track of line numbers
            }

            buffer += input;
            buffer += "\n";
            newlines += "\n";
            ++line_number;

            //if (input == nullptr) {  // Ctrl+D / EOF
            //    break;
           // }

            std::string line(input);

            std::transform(line.begin(), line.end(), line.begin(),
                [](unsigned char c) { return std::tolower(c); });

            if (line == "exit" || line == "quit") {
                break;
            }
            //std::cout << "buffer.back:" << buffer.back() << "\n";


            QString source = QString::fromStdString(buffer);
            //std::cout << "'" << source.trimmed().back().toLatin1() << "'\n";

            QString statement = QString::fromStdString(line);


            QStringList line_parts = statement.split('#');
            if (line_parts.count() > 1) {
                if (line_parts.at(0).trimmed() != "") {

                    statement = line_parts.at(0);
                }
                else {
                    //qDebug() << "other branch";
                    statement = "";
                }
            }


            if (!statement.trimmed().isEmpty() && statement.trimmed().back() == ';') { //buffer.at(buffer.size() - 2) == ';')
                // Command complete
                //std::cout << "Executing: " << buffer << "\n";

                //statement_before_comment = false; //reset

                std::shared_ptr<QTextStream> ts = std::make_shared<QTextStream>(&source);
                csvquery::Parser parser(ts);

                while (true) {
                    try {
                        QList<csvquery::Token> tokens = parser.read_statement();

                        if (tokens.isEmpty()) {
                            break;
                        }
                        //std::pair<int, std::optional<QList<QStringList>>> result = parser.execute(tokens);

                        csvquery::Token action = tokens.front();

                        if (action.token_type == csvquery::TokenType::IMPORT) {
                            csvquery::ImportStatement import(tokens);
                            import.execute();
                            std::cout << "Number of names loaded: " << import.num_of_columns_loaded() << "\n";
                        }
                        else if (action.token_type == csvquery::TokenType::NAME) {
                            //std::cout << "assigning...\n";
                            csvquery::AssignStatement assign(tokens);
                            assign.execute();
                        }
                        else if (action.token_type == csvquery::TokenType::UPDATE) {
                            csvquery::UpdateStatement update(tokens);
                            update.execute();
                            std::cout << "Number of rows updated: " << update.get_number_of_rows() << "\n";
                        }
                        else if (action.token_type == csvquery::TokenType::SELECT) {
                            int max_rows_per_page = MAX_ROWS_PER_PAGE;
                            csvquery::SelectStatement select(tokens, max_rows_per_page);
                            std::optional<QList<QStringList>> result = select.execute();

                            if (result.has_value()) {
                                //print result
                                //print(result);
                                print_table(result);
                                std::cout << "\nNumber of rows read: " << select.get_number_of_rows() << "\n\n";
                                //if (max_rows_per_page < select.get_number_of_rows()) {
                                paginate(select, rx);
                                //}
                                
                            }
                            else {
                                std::cout << "Number of rows read: " << select.get_number_of_rows() << "\n\n";
                            }
                        }



                        if (parser.current_token().token_type == csvquery::TokenType::END) {
                            break;
                        }
                    }
                    catch (std::logic_error l) {
                        std::cout << "Parser error 1: " << l.what() << "\n";
                    }

                }
                // Add to history
                //if (!line.empty()) {
                //    rx.history_add(line);
                //}

                buffer.clear();
            }

            // Add to history
            if (!line.empty()) {
                rx.history_add(line);
            }
        }

    }
    catch(std::logic_error l){
        std::cout<<"Parser error 2 "<<l.what()<<"\n";
    }
    catch(...){
        std::cout<< "There was an exception!";
    }

    //test_tokenizer();

    return 0;
}

void test_tokenizer()
{
    QString source = "import 'hello.def'; select *.araba, *, file.0, 2*(fi), [0] from x #where [len] <= 2;\n\n\n  delete from x where [0] > 20 and [1] = 'hello'; select table.* from y\n\n";
    std::shared_ptr<QTextStream> stream = std::make_shared<QTextStream>(&source);

    csvquery::Tokenizer tokenizer(stream);

    csvquery::Token token = tokenizer.get();
    while(token.token_type != csvquery::TokenType::END){
        if(token.token_type == csvquery::TokenType::SEMICOLON){
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

    csvquery::Tokenizer tokenizer(stream);
    QList<csvquery::Term> terms;
    QList<csvquery::Expression> exps;

    csvquery::Token token = tokenizer.get();
    while(token.token_type != csvquery::TokenType::END){
        if(token.token_type == csvquery::TokenType::SEMICOLON){
            std::cout<<";%%\n";
        }else{
            std::cout<< token << "{" <<token.string_value.toStdString() <<"} ";
        }

        if(token.token_type == csvquery::TokenType::COMMA){
            csvquery::Expression exp(terms);
            exps.append(exp);
            terms = {};
        }
        else{

            csvquery::Term t(token);
            terms.append(t);
        }
        token = tokenizer.get();
    }

    if(token.token_type == csvquery::TokenType::END){
        csvquery::Expression exp(terms);
        exps.append(exp);
    }
    std::cout<< token << "\n"; //print END token

    //evaluate expressions
    //QStringList row = {"empty string", "20", "hello"};
    QList<csvquery::Term> results;
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
        if(t.get_token().token_type == csvquery::TokenType::NUMBER){
            std::cout<<t.get_token().number_value<<")";
        }
        else if(t.get_token().token_type == csvquery::TokenType::STRING){
            std::cout<<t.get_token().string_value.toStdString()<<")";
        }
        std::cout<< "\n";
    }
}

void test_conditional_expression(QString& source, const QMap<QString, QStringList>& data_rows)
{
    //QString source = "[2]='hello'";
    std::shared_ptr<QTextStream> stream = std::make_shared<QTextStream>(&source);

    csvquery::Tokenizer tokenizer(stream);
    QList<csvquery::Term> terms;
    //QList<Expression> exps;

    // Read and Print tokens //
    csvquery::Token token = tokenizer.get();
    while(token.token_type != csvquery::TokenType::END){
        if(token.token_type == csvquery::TokenType::SEMICOLON){
            std::cout<<";%%\n";
            break;
        }else{
            std::cout<< token << "{" <<token.string_value.toStdString() <<"} ";
        }

        csvquery::Term t(token);
        terms.append(t);
        token = tokenizer.get();
    }

    csvquery::ConditionalExpression cond_expr(terms);

    std::cout<< token << "\n"; //print END token

    //evaluate expressions
    //QStringList row = {"empty string", "20", "hello"};
    //std::cout<<"evaluating cond_expr.eval(row)\n";
    csvquery::Term result = cond_expr.eval(data_rows);
    //std::cout<<"done.\n";

    //Print result
    std::cout<<"Answer: "<<result.get_token().to_string().toStdString()<<"(";
    if(result.get_token().token_type == csvquery::TokenType::BOOLEAN){
        QString str = (result.get_token().boolean_value == true)? "TRUE" : "FALSE";
        std::cout<<str.toStdString()<<")";
    }

}

void test_import(QString& source){
    std::shared_ptr<QTextStream> stream = std::make_shared<QTextStream>(&source);
    csvquery::Tokenizer tokenizer(stream);
    QList<csvquery::Term> terms;
    QList<csvquery::Token> tokens;
    //QList<Expression> exps;

    // Read and Print tokens //
    csvquery::Token token = tokenizer.get();
    while(token.token_type != csvquery::TokenType::END){
        if(token.token_type == csvquery::TokenType::SEMICOLON){
            std::cout<<";%%\n";
            break;
        }else{
            std::cout<< token << "{" <<token.string_value.toStdString() <<"} ";
        }

        //Term t(token);
        tokens.append(token);
        token = tokenizer.get();
    }

    csvquery::ImportStatement import(tokens);
    import.execute();

    qDebug()<< csvquery::import_defs;
}

void test_import_and_assigment()
{
    QString source = "import 'D:/Downloads/test_input/courses.def' as courses; \n a:courses = 'D:/Downloads/test_input/hello.csv';";

    std::shared_ptr<QTextStream> stream = std::make_shared<QTextStream>(&source);
    csvquery::Tokenizer tokenizer(stream);
    QList<csvquery::Term> terms;
    QList<csvquery::Token> tokens;
    //QList<Expression> exps;

    // Read import statement and Print tokens //
    csvquery::Token token = tokenizer.get();
    while(token.token_type != csvquery::TokenType::END){
        if(token.token_type == csvquery::TokenType::SEMICOLON){
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

    csvquery::ImportStatement import(tokens);
    import.execute();

    qDebug()<<"Imported defs: "<< csvquery::import_defs;

    // Read assigment statement and Print tokens //
    token = tokenizer.get();
    tokens ={};
    while(token.token_type != csvquery::TokenType::END){
        if(token.token_type == csvquery::TokenType::SEMICOLON){
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

    csvquery::AssignStatement assignment(tokens);
    assignment.execute();
    qDebug()<<"columns table"<< csvquery::columns_table;
    qDebug()<<"symbols table";

    foreach (auto s, csvquery::symbol_table.keys()) {
        csvquery::TokenType t = csvquery::symbol_table[s];
        csvquery::Token tk(t);
        qDebug()<<s <<": "<<tk.to_string();
    }

    qDebug()<<"Strings table: "<< csvquery::strings_table;
    qDebug()<<"numbers table: "<< csvquery::numbers_table;
}

void test_csv_parser(const QString& csv)
{
    csvquery::CSVFile f(csv);

    while(!f.end_of_file()){
        QStringList row = f.readLine();
        qDebug()<<"Number of rows: "<<row.size();
        qDebug()<<row;
        foreach(auto c, row){
            std::cout<<'*'<<c.toStdString()<<'*'<<'\n';
        }
        qDebug()<<"----------";
    }

}


QList<csvquery::Token> read_statement(csvquery::Tokenizer& t)
{
    QList<csvquery::Token> tokens;
    //QList<Expression> exps;

    // Read import statement and Print tokens //
    csvquery::Token token = t.get();
    while(token.token_type != csvquery::TokenType::END){
        if(token.token_type == csvquery::TokenType::SEMICOLON){
            std::cout<<";%%\n";
            tokens.append(token);
            break;
        }else{
            std::cout<< token << "{" <<token.string_value.toStdString() <<"} ";
        }

        //Term t(token);
        tokens.append(token);
        token = t.get();
    }

    return tokens;
}

void test_select_statement()
{
    QString source = " a = 'D:/Downloads/test_input/hello.csv'; b='D:/Downloads/test_input/hello2.csv'; c='D:/Downloads/test_input/hello3.csv';";
        //source += " select a.1 +' '+ b.1, length(a.1 +' '+ b.1) from a inner join b on a.0 = b.0 where (a.0 != 'id') ;";
        //source += " select 'hello', 2+2, '= 4';";
        source += " update a set [1] = 'Kwadwo' where [0] = '2' into c;";

    std::shared_ptr<QTextStream> stream = std::make_shared<QTextStream>(&source);
    csvquery::Tokenizer tokenizer(stream);
    QList<csvquery::Term> terms;

    QList<csvquery::Token> assignment_tokens1 = read_statement(tokenizer);

    csvquery::AssignStatement assignment1(assignment_tokens1);
    assignment1.execute();

    QList<csvquery::Token> assignment_tokens2 = read_statement(tokenizer);

    csvquery::AssignStatement assignment2(assignment_tokens2);
    assignment2.execute();

    QList<csvquery::Token> assignment_tokens3 = read_statement(tokenizer);

    csvquery::AssignStatement assignment3(assignment_tokens3);
    assignment3.execute();

    //QList<Token> select_tokens = read_statement(tokenizer);
    QList<csvquery::Token> update_tokens = read_statement(tokenizer);

    /*
    qDebug() << "Select tokens: " << select_tokens.size();
    foreach(auto t, select_tokens) {
        qDebug() << "-" << t.to_string();
    }*/

    //SelectStatement select(select_tokens);
    //std::optional<QList<QStringList>> res = select.execute();
    //qDebug()<<"execution done.";
    csvquery::UpdateStatement update(update_tokens);
    update.execute();


    //tabulate::Table table;
    /*
    auto table = pretty::Table();


    if(res.has_value()){
        //qDebug()<<res.value();
        auto result = res.value();

        if (res.has_value()) {
            if (result.isEmpty()) {
                std::cout << "Number of rows:" << result.size();
                return;
            }
        }

        std::vector<std::string> header;
        int col_number = 1;
        int number_of_cols = result[0].size();

        for (int i = 0; i < number_of_cols; ++i) {
            std::string str = "Col. ";
            str += std::to_string(col_number);
            qDebug() << str;
            header.push_back(str);
            col_number++;
        }

        if(!result.isEmpty()){

            table.addRow(std::move(header));

            foreach(auto row, result){
                std::vector<std::string> r;

                qDebug() << row;
                if (row.isEmpty()) {
                    continue;
                }

                foreach(auto col, row){
                    //std::variant<std::string, const char *, string_view, tabulate::Table> c = col.toStdString();

                    r.emplace_back(col.toStdString());
                }
                table.addRow(std::move(r));
            }
            pretty::Printer print;
            //print.frame(pretty::FrameStyle::Basic);
            print.frame(pretty::FrameStyle::LineRounded);
            std::cout << print(table);
            std::cout << "Number of rows:" << result.size();
        }

        //table.addRow({"1", "one"});
        //table.addRow({ "2", "two" });
        
    }*/
}

/*
void print(const std::optional<QList<QStringList>>& res)
{
    auto table = pretty::Table();


    if (res.has_value()) {
        //qDebug()<<res.value();
        auto result = res.value();

        if (res.has_value()) {
            if (result.isEmpty()) {
                std::cout << "Number of rows:" << result.size();
                return;
            }
        }

        std::vector<std::string> header;
        int col_number = 1;
        int number_of_cols = result[0].size();

        for (int i = 0; i < number_of_cols; ++i) {
            std::string str = "Col. ";
            str += std::to_string(col_number);
            qDebug() << str;
            header.push_back(str);
            col_number++;
        }

        qDebug() << "header: " << header;

        if (!result.isEmpty()) {

            table.addRow(std::move(header));

            foreach(auto row, result) {
                std::vector<std::string> r;

                qDebug() << "row:" << row;
                if (row.isEmpty()) {
                    continue;
                }

                foreach(auto col, row) {
                    //std::variant<std::string, const char *, string_view, tabulate::Table> c = col.toStdString();
                    qDebug() << "Col:" << col;
                    r.emplace_back(col.toStdString());
                }
                table.addRow(std::move(r));
            }
            pretty::Printer print;
            //print.frame(pretty::FrameStyle::Basic);
            print.frame(pretty::FrameStyle::LineRounded);
            std::cout << print(table);
            //std::cout << "Number of rows:" << result.size();
        }

        //table.addRow({"1", "one"});
        //table.addRow({ "2", "two" });
    }
}*/

void print_table(const std::optional<QList<QStringList>>& res)
{
    tabulate::Table table;

    using Row_t = std::vector<std::variant<std::string, const char*, std::string_view, tabulate::Table>>;


    if (res.has_value()) {
        //qDebug()<<res.value();
        auto result = res.value();

        if (res.has_value()) {
            if (result.isEmpty()) {
                std::cout << "Number of rows:" << result.size();
                return;
            }
        }

        std::vector<std::string> header;
        int col_number = 1;
        int number_of_cols = result[0].size();
        Row_t header_row;
        for (int i = 0; i < number_of_cols; ++i) {
            std::string str = "Col. ";
            str += std::to_string(col_number);
            //qDebug() << str;
            header_row.push_back(str);
            col_number++;
        }

        //qDebug() << "header: " << header;

        if (!result.isEmpty()) {

            table.add_row(header_row);

            foreach(auto row, result) {
                //std::vector<std::string> r;
                Row_t r;

                //qDebug() << "row:" << row;
                if (row.isEmpty()) {
                    continue;
                }

                foreach(auto col, row) {
                    //std::variant<std::string, const char *, string_view, tabulate::Table> c = col.toStdString();
                    //qDebug() << "Col:" << col;
                    /*
                    qDebug() << col.front() << col.back();

                    if (col.front() == "\"" && col.back() == "\"") { // remove quotes from string
                        col.remove(0, 1);
                        col.chop(1);
                    }*/
                    r.emplace_back(col.toStdString());
                }
                table.add_row(r);
            }
            //pretty::Printer print;
            //print.frame(pretty::FrameStyle::Basic);
            //print.frame(pretty::FrameStyle::LineRounded);

            // Iterate over cells in the first row
            for (auto& cell : table[0]) {
                cell.format()
                    .font_style({ tabulate::FontStyle::bold })
                    .font_align(tabulate::FontAlign::center)
                    .font_color(tabulate::Color::yellow);
            }
            
          

            std::cout <<"\n" << table << "\n";
            //std::cout << "Number of rows:" << result.size();
        }

        //table.addRow({"1", "one"});
        //table.addRow({ "2", "two" });
    }
}

void paginate(csvquery::SelectStatement& select, replxx::Replxx& rx)
{
    /*
    std::cout << "Enter any key to show the next page or x to stop\n";

    const char* input = rx.input("->] ");

    if (!input) return;  // EOF

    std::string line(input);

    if (line == "x" || line == "X") {
        return;
    }*/

    //loop until end of file
    while (true) {
        std::optional<QList<QStringList>> result = select.execute();

        if (result.has_value()) {

            std::cout << "Enter any key to show the next page or x to stop\n";

            const char* input = rx.input("->] ");

            if (!input) return;  // EOF

            std::string line(input);

            if (line == "x" || line == "X") {
                return;
            }

            //print result
            print_table(result);
            std::cout << "Number of rows read:" << select.get_number_of_rows() << "\n";

            /*
            if (select.get_max_rows_per_page() > select.get_number_of_rows()) {
                std::cout << "Enter any key to show the next page or x to stop\n";

                const char* input = rx.input("->] ");

                if (!input) break;  // EOF

                std::string line(input);

                if (line == "x" || line == "X") {
                    break;
                }
                continue;
            }*/

        }
        else {
            break;
        }
    }
}

void execute_source_file(QFile& source) {
    replxx::Replxx rx;
    std::shared_ptr<QTextStream> ts = std::make_shared<QTextStream>(&source);
    csvquery::Parser parser(ts);

    while (true) {
        try {
            QList<csvquery::Token> tokens = parser.read_statement();

            if (tokens.isEmpty()) {
                break;
            }
            //std::pair<int, std::optional<QList<QStringList>>> result = parser.execute(tokens);

            csvquery::Token action = tokens.front();

            if (action.token_type == csvquery::TokenType::IMPORT) {
                csvquery::ImportStatement import(tokens);
                import.execute();
                std::cout << "Number of names loaded: " << import.num_of_columns_loaded() << "\n";
            }
            else if (action.token_type == csvquery::TokenType::NAME) {
                //std::cout << "assigning...\n";
                csvquery::AssignStatement assign(tokens);
                assign.execute();
            }
            else if (action.token_type == csvquery::TokenType::UPDATE) {
                csvquery::UpdateStatement update(tokens);
                update.execute();
                std::cout << "Number of rows updated: " << update.get_number_of_rows() << "\n";
            }
            else if (action.token_type == csvquery::TokenType::SELECT) {
                int max_rows_per_page = MAX_ROWS_PER_PAGE;
                csvquery::SelectStatement select(tokens, max_rows_per_page);
                std::optional<QList<QStringList>> result = select.execute();

                if (result.has_value()) {
                    //print result
                    //print(result);
                    print_table(result);
                    std::cout << "\nNumber of rows read: " << select.get_number_of_rows() << "\n\n";
                    //if (max_rows_per_page < select.get_number_of_rows()) {
                    paginate(select, rx);
                    //}

                }
                else {
                    std::cout << "Number of rows read: " << select.get_number_of_rows() << "\n\n";
                }
            }



            if (parser.current_token().token_type == csvquery::TokenType::END) {
                break;
            }
        }
        catch (std::logic_error l) {
            std::cout << "Parser error 1: " << l.what() << "\n";
        }

    }
}


void set_builtin_funcs()
{
    //extern QHash<QString, std::function<Term(QList<Term>)> > funcs_table; // function name, "pointer to function"


    //extern QHash<QString, QList<TokenType>> func_args_type_list; // returns list of function argument type list


    csvquery::symbol_table["trim"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["length"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["substring"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["left"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["right"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["date_gt"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["date_lt"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["date_ge"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["date_le"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["date_eq"] = csvquery::TokenType::FUNCTION;

    std::function<csvquery::Term(QList<csvquery::Term>)> _trim = csvquery::trim;
    std::function<csvquery::Term(QList<csvquery::Term>)> _length = csvquery::length;
    std::function<csvquery::Term(QList<csvquery::Term>)> _substring = csvquery::substring;
    std::function<csvquery::Term(QList<csvquery::Term>)> _left = csvquery::left;
    std::function<csvquery::Term(QList<csvquery::Term>)> _right = csvquery::right;
    std::function<csvquery::Term(QList<csvquery::Term>)> _date_gt = csvquery::date_gt;
    std::function<csvquery::Term(QList<csvquery::Term>)> _date_lt = csvquery::date_lt;
    std::function<csvquery::Term(QList<csvquery::Term>)> _date_ge = csvquery::date_ge;
    std::function<csvquery::Term(QList<csvquery::Term>)> _date_le = csvquery::date_le;
    std::function<csvquery::Term(QList<csvquery::Term>)> _date_eq = csvquery::date_eq;

    csvquery::funcs_table["trim"] = _trim;
    csvquery::funcs_table["length"] = _length;
    csvquery::funcs_table["substring"] = _substring;
    csvquery::funcs_table["left"] = _left;
    csvquery::funcs_table["right"] = _right;
    csvquery::funcs_table["date_gt"] = _date_gt;
    csvquery::funcs_table["date_lt"] = _date_lt;
    csvquery::funcs_table["date_ge"] = _date_ge;
    csvquery::funcs_table["date_le"] = _date_le;
    csvquery::funcs_table["date_eq"] = _date_eq;

    QList<csvquery::TokenType> only_string_arg = { csvquery::TokenType::STRING};
    QList<csvquery::TokenType> string_and_number = { csvquery::TokenType::STRING, csvquery::TokenType::NUMBER};
    QList<csvquery::TokenType> str_num_num = { csvquery::TokenType::STRING, csvquery::TokenType::NUMBER, csvquery::TokenType::NUMBER};
    QList<csvquery::TokenType> date_comp_arg_types = { csvquery::TokenType::STRING, csvquery::TokenType::STRING, csvquery::TokenType::STRING, csvquery::TokenType::STRING};

    csvquery::func_args_type_list["trim"] = only_string_arg;
    csvquery::func_args_type_list["length"] = only_string_arg;
    csvquery::func_args_type_list["substring"] = str_num_num;
    csvquery::func_args_type_list["left"] = string_and_number;
    csvquery::func_args_type_list["right"] = string_and_number;
    csvquery::func_args_type_list["date_gt"] = date_comp_arg_types;
    csvquery::func_args_type_list["date_lt"] = date_comp_arg_types;
    csvquery::func_args_type_list["date_ge"] = date_comp_arg_types;
    csvquery::func_args_type_list["date_le"] = date_comp_arg_types;
    csvquery::func_args_type_list["date_eq"] = date_comp_arg_types;


}
