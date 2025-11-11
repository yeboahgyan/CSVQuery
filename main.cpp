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
#include "license.h"
#include <Windows.h>
#include <format>
#include "test.h"



int MAX_ROWS_PER_PAGE = 10;

void set_builtin_funcs();

void setConsoleFont(const wchar_t* fontName, SHORT fontSize = 18);

void execute_source_file(QFile& source);

void print_table(const std::optional<QList<QStringList>>& res, const QStringList& column_names = {});

void paginate(csvquery::SelectStatement& select, replxx::Replxx& rx);

std::string format_duration(const double& duration);



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

    setConsoleFont(L"Cascadia Code");

    std::cout << "\n";
    std::cout << "--------------------------------------------------------------------------------------------------------------\n";
    std::cout << "  CSVQ for CSVQuery ";
    std::cout << "0.1.7";
    std::cout << " (c) 2025 Kwame Yeboah-Gyan\n";
	std::cout << "  A command-line tool for querying and manipulating CSV files using CSVQuery,  an SQL-like language.\n";
    if (a.arguments().length() == 1) {
        std::cout << "  Type \"exit\" or \"quit\" to quit, \"license\" for license information.\n";
    }

    std::cout << "--------------------------------------------------------------------------------------------------------------\n\n\n";

    set_builtin_funcs();

    //test_main();
    //return 0;

    try{


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

        std::vector<std::string> keywords = { //used in highlighting keywords
        "SELECT", "FROM", "UPDATE", "IMPORT", "WHERE", "HAVING",
        "INTO", "INNER JOIN", "OUTER JOIN", "ON", "SET", "LIKE", "NOT LIKE",
        "TRIM", "LENGTH", "SUBSTRING", "LEFT", "RIGHT", "AS","COUNT", "SUM", "AVG", "MIN", "MAX",
        "DATE_GT", "DATE_GE", "DATE_LT", "DATE_LE", "DATE_EQ", "STRIP_QUOTES",
        "QUIT", "EXIT", "AND", "OR", "NUMBER", "GROUP BY", "LIMIT"
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
                //std::regex strings(R"((['"])([^'"]*)\1)");
                std::regex strings(R"((["'])(?:\\.|(?!\1).)*\1)");
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

        // ANSI color codes
        const char* yellow = "\x1b[33m";
        const char* reset = "\x1b[0m";

        int line_number = 1;
        while (true) {
            std::string prompt_main = "\n  " + std::string(yellow) + std::to_string(line_number)  + std::string(reset)  +" csvQ> ";
            std::string prompt_continue = "  " + std::string(yellow) + std::to_string(line_number) + std::string(reset) + "->] ";
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

            if (line == "license") {
                print_license();
                buffer = "";
                line = ";";
                continue;
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

            //if (statement.trimmed() == ";" && source.trimmed().isEmpty()) {
            if (statement.trimmed() == ";" && (statement.trimmed() == source.trimmed())) {
                buffer.clear();
                continue; //empty statement
            }


            if (!statement.trimmed().isEmpty() && statement.trimmed().back() == ';') { //buffer.at(buffer.size() - 2) == ';')
                // Command complete
                //std::cout << "Executing: " << buffer << ", size:"<<buffer.size()<<"\n";

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
                            auto start = std::chrono::system_clock::now();

                            import.execute();

                            auto end = std::chrono::system_clock::now();

                            std::cout << "    Number of names loaded: " << import.num_of_columns_loaded() << "\n";
                            auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                            std::cout << "    Import Duration: " << format_duration(diff.count()) << "\n\n";
                        }
                        else if (action.token_type == csvquery::TokenType::NAME) {
                            //std::cout << "assigning...\n";
                            csvquery::AssignStatement assign(tokens);
                            auto start = std::chrono::system_clock::now();

                            assign.execute();

                            auto end = std::chrono::system_clock::now();
                            auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                            std::cout << "    Assignment Duration: " << format_duration(diff.count()) << "\n\n";
                        }
                        else if (action.token_type == csvquery::TokenType::UPDATE) {
                            csvquery::UpdateStatement update(tokens);

                            auto start = std::chrono::system_clock::now();
                            update.execute();

                            auto end = std::chrono::system_clock::now();
                            auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

                            std::cout << "    Number of rows updated: " << update.get_number_of_rows() << "\n";
                            std::cout << "    Update Duration: " << format_duration(diff.count()) << "\n\n";
                        }
                        else if (action.token_type == csvquery::TokenType::SELECT) {
                            int max_rows_per_page = MAX_ROWS_PER_PAGE;
                            csvquery::SelectStatement select(tokens, max_rows_per_page);

                            auto start = std::chrono::system_clock::now();
                            std::optional<QList<QStringList>> result = select.execute();
                            //qDebug() << "done with select";

                            auto end = std::chrono::system_clock::now();

                            if (result.has_value()) {
                                //print result
                                //print(result);
                                print_table(result, select.get_column_names());


                                std::cout << "    Number of rows read: " << select.get_number_of_rows() << "\n";
                                auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                                std::cout << "    Select Duration: " << format_duration(diff.count()) << "\n\n";

                                //qDebug() << select.get_column_names();
                                //if (max_rows_per_page < select.get_number_of_rows()) {
                                //select.onNextPressed();
                                //select.resume();
                                paginate(select, rx);
                                //}
                                
                            }
                            else {
                                std::cout << "    Number of rows read: " << select.get_number_of_rows() << "\n";
                                auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                                std::cout << "    Select Duration: " << format_duration(diff.count()) << "\n\n";
                            }
                        }
                        else {
                            //qDebug() << "invalid syntax: " << action.to_string();
                            QString error = "Invalid statement '";
                            error += tokens.front().string_value;
                            error += "' on line ";
                            error += QString::number(tokens.front().line_number);
                            error += "!";
                            throw std::logic_error(error.toStdString());
                        }



                        if (parser.current_token().token_type == csvquery::TokenType::END) {
                            break;
                        }
                    }
                    catch (std::logic_error l) {
                        std::cerr << "    Parser error: " << l.what() << "\n";
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
        std::cerr<<"    Parser error 2 "<<l.what()<<"\n";
    }
    catch (const std::exception& e) {
        std::cerr << "    There was an exception - " << e.what()  << "\n";
    }
    catch(...){
        std::cerr << "    There was an exception!\n";
    }

    //test_tokenizer();

    return 0;
}

void setConsoleFont(const wchar_t* fontName, SHORT fontSize) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX cfi = { sizeof(CONSOLE_FONT_INFOEX) };
    GetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
    wcscpy_s(cfi.FaceName, fontName);
    cfi.dwFontSize.Y = fontSize; // set height
    SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
}

void set_builtin_funcs()
{
    //extern QHash<QString, std::function<Term(QList<Term>)> > funcs_table; // function name, "pointer to function"


    //extern QHash<QString, QList<TokenType>> func_args_type_list; // returns list of function argument type list

    //Term strip_quotes(QList<Term>)


    csvquery::symbol_table["trim"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["length"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["substring"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["left"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["right"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["strip_quotes"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["date_gt"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["date_lt"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["date_ge"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["date_le"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["date_eq"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["number"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["count"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["max"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["min"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["sum"] = csvquery::TokenType::FUNCTION;
    csvquery::symbol_table["avg"] = csvquery::TokenType::FUNCTION;

    std::function<csvquery::Term(QList<csvquery::Term>)> _trim = csvquery::trim;
    std::function<csvquery::Term(QList<csvquery::Term>)> _length = csvquery::length;
    std::function<csvquery::Term(QList<csvquery::Term>)> _substring = csvquery::substring;
    std::function<csvquery::Term(QList<csvquery::Term>)> _left = csvquery::left;
    std::function<csvquery::Term(QList<csvquery::Term>)> _right = csvquery::right;
    std::function<csvquery::Term(QList<csvquery::Term>)> _strip_quotes = csvquery::strip_quotes;
    std::function<csvquery::Term(QList<csvquery::Term>)> _date_gt = csvquery::date_gt;
    std::function<csvquery::Term(QList<csvquery::Term>)> _date_lt = csvquery::date_lt;
    std::function<csvquery::Term(QList<csvquery::Term>)> _date_ge = csvquery::date_ge;
    std::function<csvquery::Term(QList<csvquery::Term>)> _date_le = csvquery::date_le;
    std::function<csvquery::Term(QList<csvquery::Term>)> _date_eq = csvquery::date_eq;
    std::function<csvquery::Term(QList<csvquery::Term>)> _number = csvquery::number;
    std::function<csvquery::Term(QList<csvquery::Term>)> _count = csvquery::count;
    std::function<csvquery::Term(QList<csvquery::Term>)> _sum = csvquery::sum;
    std::function<csvquery::Term(QList<csvquery::Term>)> _max = csvquery::max;
    std::function<csvquery::Term(QList<csvquery::Term>)> _min = csvquery::min;
    std::function<csvquery::Term(QList<csvquery::Term>)> _avg = csvquery::avg;

    csvquery::funcs_table["trim"] = _trim;
    csvquery::funcs_table["length"] = _length;
    csvquery::funcs_table["substring"] = _substring;
    csvquery::funcs_table["left"] = _left;
    csvquery::funcs_table["right"] = _right;
    csvquery::funcs_table["strip_quotes"] = _strip_quotes;
    csvquery::funcs_table["date_gt"] = _date_gt;
    csvquery::funcs_table["date_lt"] = _date_lt;
    csvquery::funcs_table["date_ge"] = _date_ge;
    csvquery::funcs_table["date_le"] = _date_le;
    csvquery::funcs_table["date_eq"] = _date_eq;
    csvquery::funcs_table["number"] = _number;
    csvquery::funcs_table["count"] = _count;
    csvquery::funcs_table["sum"] = _sum;
    csvquery::funcs_table["max"] = _max;
    csvquery::funcs_table["min"] = _min;
    csvquery::funcs_table["avg"] = _avg;

    QList<csvquery::TokenType> only_string_arg = { csvquery::TokenType::STRING};
    QList<csvquery::TokenType> string_and_number = { csvquery::TokenType::STRING, csvquery::TokenType::NUMBER};
    QList<csvquery::TokenType> str_num_num = { csvquery::TokenType::STRING, csvquery::TokenType::NUMBER, csvquery::TokenType::NUMBER};
    QList<csvquery::TokenType> date_comp_arg_types = { csvquery::TokenType::STRING, csvquery::TokenType::STRING, csvquery::TokenType::STRING, csvquery::TokenType::STRING};


    /* not using this at the moment? */
    csvquery::func_args_type_list["trim"] = only_string_arg;
    csvquery::func_args_type_list["length"] = only_string_arg;
    csvquery::func_args_type_list["substring"] = str_num_num;
    csvquery::func_args_type_list["left"] = string_and_number;
    csvquery::func_args_type_list["right"] = string_and_number;
    csvquery::func_args_type_list["strip_quotes"] = only_string_arg;
    csvquery::func_args_type_list["date_gt"] = date_comp_arg_types;
    csvquery::func_args_type_list["date_lt"] = date_comp_arg_types;
    csvquery::func_args_type_list["date_ge"] = date_comp_arg_types;
    csvquery::func_args_type_list["date_le"] = date_comp_arg_types;
    csvquery::func_args_type_list["date_eq"] = date_comp_arg_types;
    csvquery::func_args_type_list["number"] = only_string_arg;
    csvquery::func_args_type_list["count"] = only_string_arg;
    csvquery::func_args_type_list["sum"] = only_string_arg;
    csvquery::func_args_type_list["max"] = only_string_arg;
    csvquery::func_args_type_list["min"] = only_string_arg;
    csvquery::func_args_type_list["avg"] = only_string_arg;

    // Map functions to their compiler functions 
    csvquery::funcs_compiler_table["trim"] = csvquery::comp_trim;
    csvquery::funcs_compiler_table["length"] = csvquery::comp_length;
    csvquery::funcs_compiler_table["substring"] = csvquery::comp_substring;
    csvquery::funcs_compiler_table["left"] = csvquery::comp_left;
    csvquery::funcs_compiler_table["right"] = csvquery::comp_right;
    csvquery::funcs_compiler_table["strip_quotes"] = csvquery::comp_strip_quotes;
    csvquery::funcs_compiler_table["date_gt"] = csvquery::comp_date_gt;
    csvquery::funcs_compiler_table["date_lt"] = csvquery::comp_date_lt;
    csvquery::funcs_compiler_table["date_ge"] = csvquery::comp_date_ge;
    csvquery::funcs_compiler_table["date_le"] = csvquery::comp_date_le;
    csvquery::funcs_compiler_table["date_eq"] = csvquery::comp_date_eq;
    csvquery::funcs_compiler_table["number"] = csvquery::comp_number;
    csvquery::funcs_compiler_table["count"] = csvquery::comp_count;
    csvquery::funcs_compiler_table["sum"] = csvquery::comp_sum;
    csvquery::funcs_compiler_table["max"] = csvquery::comp_max;
    csvquery::funcs_compiler_table["min"] = csvquery::comp_min;
    csvquery::funcs_compiler_table["avg"] = csvquery::comp_avg;



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
            //std::pair<int, std::optional<QList<QStringList>>> result = parser.execute(tokens)
            

            csvquery::Token action = tokens.front();

            if (action.token_type == csvquery::TokenType::SEMICOLON) {
				continue; //empty statement
            }
            else if (action.token_type == csvquery::TokenType::IMPORT) {
                csvquery::ImportStatement import(tokens);
				auto start = std::chrono::system_clock::now();
                import.execute();
                auto end = std::chrono::system_clock::now();
                std::cout << "    Number of names loaded: " << import.num_of_columns_loaded() << "\n";

                auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                std::cout << "    Import Duration: " << format_duration(diff.count()) << "\n\n";
            }
            else if (action.token_type == csvquery::TokenType::NAME) {
                //std::cout << "assigning...\n";
                csvquery::AssignStatement assign(tokens);
                auto start = std::chrono::system_clock::now();
                assign.execute();
                auto end = std::chrono::system_clock::now();

                auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                std::cout << "    Assignemt Duration: " << format_duration(diff.count()) << "\n\n";
            }
            else if (action.token_type == csvquery::TokenType::UPDATE) {
                csvquery::UpdateStatement update(tokens);
                auto start = std::chrono::system_clock::now();
                update.execute();
                auto end = std::chrono::system_clock::now();

                std::cout << "    Number of rows updated: " << update.get_number_of_rows() << "\n";

                auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                std::cout << "    Update Duration: " << format_duration(diff.count()) << "\n\n";
            }
            else if (action.token_type == csvquery::TokenType::SELECT) {
                int max_rows_per_page = MAX_ROWS_PER_PAGE;
                csvquery::SelectStatement select(tokens, max_rows_per_page);

                auto start = std::chrono::system_clock::now();
                std::optional<QList<QStringList>> result = select.execute();
                auto end = std::chrono::system_clock::now();

                if (result.has_value()) {
                    //print result
                    //print(result);
                    print_table(result, select.get_column_names());
                    std::cout << "    Number of rows read: " << select.get_number_of_rows() << "\n";
                    //if (max_rows_per_page < select.get_number_of_rows()) {

                    auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                    std::cout << "    Select Duration: " << format_duration(diff.count()) << "\n\n";

                    paginate(select, rx);
                    //}

                }
                else {
                    std::cout << "    Number of rows read: " << select.get_number_of_rows() << "\n";

                    auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                    std::cout << "    Select Duration: " << format_duration(diff.count()) << "\n\n";
                }
            }
            else {
				QString error = "Invalid statement '";
				error += tokens.front().string_value;
				error += "' on line ";
				error += QString::number(tokens.front().line_number);
				error += "!";
				throw std::logic_error(error.toStdString());
            }



            if (parser.current_token().token_type == csvquery::TokenType::END) {
                break;
            }
        }
        catch (std::logic_error& l) {
            std::cout << "  Parser error 1: " << l.what() << "\n";
        }
        catch (std::exception& e) {
            std::cerr << "    Parser error 2: " << e.what() << "\n";
        }
        catch (...) {
            std::cerr << "    Parser error 3: Exception!";
        }

    }
}

void print_table(const std::optional<QList<QStringList>>& res, const QStringList& column_names)
{
    tabulate::Table table;

    using Row_t = std::vector<std::variant<std::string, const char*, std::string_view, tabulate::Table>>;


    if (res.has_value()) {
        //qDebug()<<res.value();
        auto result = res.value();

        if (res.has_value()) {
            if (result.isEmpty()) {
                std::cout << "    Number of rows:" << result.size();
                return;
            }
        }

        std::vector<std::string> header;
        Row_t header_row;
        if (column_names.isEmpty()) {
            int col_number = 1;
            int number_of_cols = result[0].size();
            //Row_t header_row;
            for (int i = 0; i < number_of_cols; ++i) {
                std::string str = "Col. ";
                str += std::to_string(col_number);
                //qDebug() << str;
                header_row.push_back(str);
                col_number++;
            }
        }
        else {
            //Row_t header_row;
            foreach(auto c, column_names) {
                header_row.push_back(c.toStdString());
            }
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



            std::cout << "\n" << table << "\n";

            //std::cout << "\n";
            //print_with_margin(table);
            //std::cout << "\n";

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

        auto start = std::chrono::system_clock::now();
        //qDebug() << "calling select in paginate";
        select.resume();
        std::optional<QList<QStringList>> result = select.execute();
        //qDebug() << "done.";

        auto end = std::chrono::system_clock::now();

        if (result.has_value()) {

            std::cout << "    Enter any key to show the next page or x to stop\n";

            const char* input = rx.input("  ->] ");

            if (!input) return;  // EOF

            std::string line(input);

            if (line == "x" || line == "X") {
                select.onCancelPressed();
                return;
            }

            //select.onNextPressed();

            //print result
            print_table(result, select.get_column_names());
            std::cout << "    Number of rows read:" << select.get_number_of_rows() << "\n";

            auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            std::cout << "    Select Duration: " << format_duration(diff.count()) << "\n\n";

            select.resume();

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

std::string format_duration(const double& duration_ns)
{
    std::string result;

    if (duration_ns < 1'000.0) { // < 1 µs
        result = std::format("{:.2f} nano sec.", duration_ns);
    }
    else if (duration_ns < 1'000'000.0) { // < 1 ms
        result = std::format("{:.2f} micro. sec.", duration_ns / 1'000.0);
    }
    else if (duration_ns < 1'000'000'000.0) { // < 1 s
        result = std::format("{:.2f} milli. sec.", duration_ns / 1'000'000.0);
    }
    else if (duration_ns < 60'000'000'000.0) { // < 1 min
        result = std::format("{:.3f} sec.", duration_ns / 1'000'000'000.0);
    }
    else if (duration_ns < 3'600'000'000'000.0) { // < 1 hr
        result = std::format("{:.2f} min.", duration_ns / 60'000'000'000.0);
    }
    else {
        result = std::format("{:.2f} hr.", duration_ns / 3'600'000'000'000.0);
    }

    return result;
}