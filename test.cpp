#include "test.h"



void print_table(const std::optional<QList<QStringList>>& res, const QStringList& column_names = {}); // defined in main.cpp

void test_main()
{
    //std::string input;
    //std::cin >> input;
    std::cout << "running..." << "\n";
    //QString source = "a='d:\\software\\test_csv\\customers.csv'; select count(a.*) from a where a.3 = 'mumbai';";
    //QString source = "c='d:\\software\\test_csv\\customers_11.csv'; o='d:\\software\\test_csv\\orders_11.csv'; select c.*, o.* from c inner join o on c.0=o.0;";
    ///QString source = " out='D:\\software\\test_csv\\2020-12-08_orders.csv'; orders='D:\\software\\test_csv\\orders.csv'; select orders.2, orders.* from orders where date_eq(orders.2, 'yyyy-mm-dd','2020-12-08') into out;";
    QString source = "c='D:\\software\\test_csv\\customers.csv';select count(*) from c where (c.0 != '50' and (c.3 = 'paris' or c.3 = 'tokyo')); ";
    //QString source = " o='d:\\software\\test_csv\\test_update.csv'; update o set o.1='Abena' into 'd:\\software\\test_csv\\test_update2.csv';";
    //QString source = "select * from 'd:\\software\\test_csv\\customers.csv' where [3] = 'london' into 'd:\\software\\test_csv\\london_customers.csv';";
    try {
        std::shared_ptr<QTextStream> ts = std::make_shared<QTextStream>(&source);
        csvquery::Parser parser(ts);

        QList<csvquery::Token> tokens = parser.read_statement(); // customers assignment

        csvquery::AssignStatement assign(tokens);
        assign.execute();

        //QList<csvquery::Token> tokens2 = parser.read_statement();

        //csvquery::AssignStatement assign2(tokens2); // orders assignment
        //assign2.execute();

        QList<csvquery::Token> select_tokens = parser.read_statement();

        int MAX_ROWS_PER_PAGE = 100;

        int max_rows_per_page = MAX_ROWS_PER_PAGE;
        csvquery::SelectStatement select(select_tokens, max_rows_per_page);
        //csvquery::UpdateStatement update(select_tokens);

        auto start = std::chrono::system_clock::now();
        std::optional<QList<QStringList>> result = select.execute();
        //update.execute();
        auto end = std::chrono::system_clock::now();
        
        if (result.has_value()) {
            //print result
            //print(result);
            print_table(result, select.get_column_names());
            //auto end = std::chrono::system_clock::now();


            std::cout << "\n    Number of rows read: " << select.get_number_of_rows() << "\n";
            std::chrono::duration<double> diff = end - start;
            std::cout << "    Duration: " << std::fixed << std::setprecision(6) << diff.count() << "s\n\n";
        }
        else {

            std::cout << "\n    Number of rows read: " << select.get_number_of_rows() << "\n";
            std::chrono::duration<double> diff = end - start;
            std::cout << "    Duration: " << std::fixed << std::setprecision(6) << diff.count() << "s\n\n";
        }
        
        std::cout << "\n done.";
    }
    catch (std::logic_error l) {
        std::cout << "  Parser error 2 " << l.what() << "\n";
    }
    catch (const std::exception& e) {
        std::cout << "  There was an exception - " << e.what() << "\n";
    }
    catch (...) {
        std::cout << "  There was an exception!\n";
    }


}

void test_tokenizer()
{
    QString source = "import 'hello.def'; select *.araba, *, file.0, 2*(fi), [0] from x #where [len] <= 2;\n\n\n  delete from x where [0] > 20 and [1] = 'hello'; select table.* from y\n\n";
    std::shared_ptr<QTextStream> stream = std::make_shared<QTextStream>(&source);

    csvquery::Tokenizer tokenizer(stream);

    csvquery::Token token = tokenizer.get();
    while (token.token_type != csvquery::TokenType::END) {
        if (token.token_type == csvquery::TokenType::SEMICOLON) {
            std::cout << ";%%\n";
        }
        else {
            std::cout << token << "{" << token.string_value.toStdString() << "} ";
        }
        token = tokenizer.get();
    }
    std::cout << token << "\n";
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
    while (token.token_type != csvquery::TokenType::END) {
        if (token.token_type == csvquery::TokenType::SEMICOLON) {
            std::cout << ";%%\n";
        }
        else {
            std::cout << token << "{" << token.string_value.toStdString() << "} ";
        }

        if (token.token_type == csvquery::TokenType::COMMA) {
            csvquery::Expression exp(terms);
            exps.append(exp);
            terms = {};
        }
        else {

            csvquery::Term t(token);
            terms.append(t);
        }
        token = tokenizer.get();
    }

    if (token.token_type == csvquery::TokenType::END) {
        csvquery::Expression exp(terms);
        exps.append(exp);
    }
    std::cout << token << "\n"; //print END token

    //evaluate expressions
    //QStringList row = {"empty string", "20", "hello"};
    QList<csvquery::Term> results;
    foreach(auto e, exps) {
        //if(e.is_star()){
        //    results.append(e.eval_star_term(row));
        //}else{
        QString num = QString::number(e.number_of_terms());
        std::cout << "evaluating expression with " << num.toStdString() << " terms... \n";
        results.append(e.eval(data_rows));

        //}
    }

    //print results
    foreach(auto t, results) {
        std::cout << "Answer: " << t.get_token().to_string().toStdString() << "(";
        if (t.get_token().token_type == csvquery::TokenType::NUMBER) {
            std::cout << t.get_token().number_value << ")";
        }
        else if (t.get_token().token_type == csvquery::TokenType::STRING) {
            std::cout << t.get_token().string_value.toStdString() << ")";
        }
        std::cout << "\n";
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
    while (token.token_type != csvquery::TokenType::END) {
        if (token.token_type == csvquery::TokenType::SEMICOLON) {
            std::cout << ";%%\n";
            break;
        }
        else {
            std::cout << token << "{" << token.string_value.toStdString() << "} ";
        }

        csvquery::Term t(token);
        terms.append(t);
        token = tokenizer.get();
    }

    csvquery::ConditionalExpression cond_expr(terms);

    std::cout << token << "\n"; //print END token

    //evaluate expressions
    //QStringList row = {"empty string", "20", "hello"};
    //std::cout<<"evaluating cond_expr.eval(row)\n";
    csvquery::Term result = cond_expr.eval(data_rows);
    //std::cout<<"done.\n";

    //Print result
    std::cout << "Answer: " << result.get_token().to_string().toStdString() << "(";
    if (result.get_token().token_type == csvquery::TokenType::BOOLEAN) {
        QString str = (result.get_token().boolean_value == true) ? "TRUE" : "FALSE";
        std::cout << str.toStdString() << ")";
    }

}

void test_import(QString& source) {
    std::shared_ptr<QTextStream> stream = std::make_shared<QTextStream>(&source);
    csvquery::Tokenizer tokenizer(stream);
    QList<csvquery::Term> terms;
    QList<csvquery::Token> tokens;
    //QList<Expression> exps;

    // Read and Print tokens //
    csvquery::Token token = tokenizer.get();
    while (token.token_type != csvquery::TokenType::END) {
        if (token.token_type == csvquery::TokenType::SEMICOLON) {
            std::cout << ";%%\n";
            break;
        }
        else {
            std::cout << token << "{" << token.string_value.toStdString() << "} ";
        }

        //Term t(token);
        tokens.append(token);
        token = tokenizer.get();
    }

    csvquery::ImportStatement import(tokens);
import.execute();

    qDebug() << csvquery::import_defs;
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
    while (token.token_type != csvquery::TokenType::END) {
        if (token.token_type == csvquery::TokenType::SEMICOLON) {
            std::cout << ";%%\n";
            tokens.append(token);
            break;
        }
        else {
            std::cout << token << "{" << token.string_value.toStdString() << "} ";
        }

        //Term t(token);
        tokens.append(token);
        token = tokenizer.get();
    }

    csvquery::ImportStatement import(tokens);
import.execute();

    qDebug() << "Imported defs: " << csvquery::import_defs;

    // Read assigment statement and Print tokens //
    token = tokenizer.get();
    tokens = {};
    while (token.token_type != csvquery::TokenType::END) {
        if (token.token_type == csvquery::TokenType::SEMICOLON) {
            std::cout << ";%%\n";
            tokens.append(token);
            break;
        }
        else {
            std::cout << token << "{" << token.string_value.toStdString() << "} ";
        }

        //Term t(token);
        tokens.append(token);
        token = tokenizer.get();
    }

    qDebug() << "Evaluating assigment";

    csvquery::AssignStatement assignment(tokens);
    assignment.execute();
    qDebug() << "columns table" << csvquery::columns_table;
    qDebug() << "symbols table";

    foreach(auto s, csvquery::symbol_table.keys()) {
        csvquery::TokenType t = csvquery::symbol_table[s];
        csvquery::Token tk(t);
        qDebug() << s << ": " << tk.to_string();
    }

    qDebug() << "Strings table: " << csvquery::strings_table;
    qDebug() << "numbers table: " << csvquery::numbers_table;
}

void test_csv_parser(const QString& csv)
{
    csvquery::CSVFile f(csv);

    while (!f.end_of_file()) {
        QStringList row = f.readLine();
        qDebug() << "Number of rows: " << row.size();
        qDebug() << row;
        foreach(auto c, row) {
            std::cout << '*' << c.toStdString() << '*' << '\n';
        }
        qDebug() << "----------";
    }

}


QList<csvquery::Token> read_statement(csvquery::Tokenizer& t)
{
    QList<csvquery::Token> tokens;
    //QList<Expression> exps;

    // Read import statement and Print tokens //
    csvquery::Token token = t.get();
    while (token.token_type != csvquery::TokenType::END) {
        if (token.token_type == csvquery::TokenType::SEMICOLON) {
            std::cout << ";%%\n";
            tokens.append(token);
            break;
        }
        else {
            std::cout << token << "{" << token.string_value.toStdString() << "} ";
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




void print_with_margin(tabulate::Table& table, int margin) {

    if (table.size() != 0) {
        //qDebug() << "formatting header!";
        for (size_t i = 0; i < table[0].size(); ++i)
            table[0][i].format()
            .font_color(tabulate::Color::yellow)
            .font_style({ tabulate::FontStyle::bold });
    }

    //table.format().ccolor_policy(tabulate::ColorPolicy::always);

    std::string indent(margin, ' ');
    //std::ostringstream os;
    //os << table;
    //std::string table_str = os.str();

    // add margin manually, preserving ANSI codes
    //std::string out;
    //std::istringstream in(table_str);
    //std::string line;

    //std::cout << table << std::endl;

    std::stringstream buffer;
    std::streambuf* prevcoutbuf = std::cout.rdbuf(buffer.rdbuf());
    std::cout << table << std::endl;
    std::string text = buffer.str();
    //printf("%s", text.c_str()); // formatting and colors are good
    std::cout.rdbuf(prevcoutbuf);

    std::istringstream in2(text);
    std::string line;
    while (std::getline(in2, line))
        std::cout << indent << line << '\n';

    //std::cout << out;

    //table.format().padding_left(20);
    //std::cout << table.str();
}



