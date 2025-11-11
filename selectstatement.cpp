#include "selectstatement.h"
#include <stdexcept>
#include <QFileInfo>
#include <cmath>
#include <QThread>


namespace csvquery {

    SelectStatement::SelectStatement(const QList<Token>& tks, unsigned int max_rows_per_page)
        :
        tokens{ tks }
        , NUMBER_OF_ROWS_PER_PAGE{ max_rows_per_page }
    {
        queue = std::make_unique<boost::lockfree::spsc_queue<std::vector<csv::CSVRow>, boost::lockfree::capacity<128>>>();

        //queue = std::make_unique<std::queue<QList<csv::CSVRow>> >();
        write_queue = std::make_unique<boost::lockfree::spsc_queue< std::vector<QString>, boost::lockfree::capacity<128>>>();

        //reserve_queue = std::make_unique<std::queue<QList<csv::CSVRow>> >();

        write_batch.reserve(WRITE_BATCH_ROWS);

        last_token_pos = tokens.cbegin();

        optional_actions[TokenType::CROSSJOIN] = [this]() {handle_cross_join(); };
        optional_actions[TokenType::INNERJOIN] = [this]() {handle_inner_join(); };
        optional_actions[TokenType::OUTERJOIN] = [this]() {handle_outer_join(); };
        optional_actions[TokenType::INTO] = [this]() {handle_into_clause(); };
        optional_actions[TokenType::WHERE] = [this]() {handle_where_clause(); };
        optional_actions[TokenType::GROUPBY] = [this]() {handle_groupby_clause(); };
        optional_actions[TokenType::HAVING] = [this]() {handle_having_clause(); };
        optional_actions[TokenType::LIMIT] = [this]() {handle_limit_clause(); };

        ////////////qDebug()()<<"constructing select statement...";
        try {
            parse();
        }
        catch (...) {
            // Ensure any started threads are signaled to stop and joined safely
            canceled.store(true);
            done_consuming.store(true, std::memory_order_release);

            if (file_reader_thread && file_reader_thread->joinable()) {
                try { file_reader_thread->join(); }
                catch (...) { /* swallow */ }
            }
            if (file_writer_thread) {
                // push remaining batches then join
                try { flush_write_queue(); }
                catch (...) {}
                if (file_writer_thread->joinable()) {
                    try { file_writer_thread->join(); }
                    catch (...) {}
                }
            }
            throw; // rethrow original exception
        }
        ////////////qDebug()()<<"Done.";

        //foreach(auto t, tokens) {
        //    ////////qDebug()() << t.to_string();
        //}
    }

    void SelectStatement::throw_exception_if_unexpected_end()
    {
        if (last_token_pos == tokens.cend()) {
            --last_token_pos; //get last but one token
            double line_numer = (*last_token_pos).line_number;
            QString str_num = QString::number(line_numer);

            throw std::logic_error("Unexpected end to SELECT statement on line " + str_num.toStdString());
        }
    }

    void SelectStatement::save_column_names(QList<Term>& terms)
    {
        QString column_name;

        int index = 0;
        foreach(auto t, terms) { // for each term in column expression
            
            if (t.get_token().token_type == TokenType::FUNCTION) {
                column_name += t.get_token().string_value;
                column_name += "(";
                auto f_args = t.get_token().func_args;

                QString func_name;
                func_name += t.get_token().string_value;



                func_name += "(";

                if (f_args.length() > 0) {
                    foreach(auto arg, f_args)
                    {
                        column_name += arg.string_value;
                        column_name += ",";

                        func_name += arg.string_value;
                        func_name += ",";
                    }
                    column_name.chop(1); //remove trailing comma ','
                    column_name += ")";

                    func_name.chop(1);
                    func_name += ")";
                }
                Token func_token = t.get_token();
                func_token.string_value = func_name.toLower(); // e.g. count(*) or count([0])
                terms[index] = Term(func_token);

                //mark select as doing aggregation
                if (func_token.string_value.toLower().contains("count(")
                    || func_token.string_value.toLower().contains("sum(")
                    || func_token.string_value.toLower().contains("avg(")
                    || func_token.string_value.toLower().contains("min(")
                    || func_token.string_value.toLower().contains("max(")
                    )
                {
                    is_aggregation = true;
                }

            }
            else {
                //column_exprs_terms.append(t);
                column_name += t.get_token().string_value;


            }
            ++index;
        }
        this->column_names.append(column_name);
    }

    QList<Expression> SelectStatement::read_column_expressions()
    {
        QList<Term> terms;
        QList<Expression> exps;

        for (; last_token_pos != tokens.cend(); ++last_token_pos) {

            if ((last_token_pos->token_type == TokenType::SEMICOLON) || (last_token_pos->token_type == TokenType::FROM)) {
                if (terms.isEmpty()) {
                    QString error = "Unexpected end to column list on line " + QString::number(last_token_pos->line_number);
                    throw std::logic_error(error.toStdString());
                }

                //save column names
                save_column_names(terms);

                Expression exp(terms);
                column_exprs_terms.append(terms);
                exps.append(exp);

                break;
            }

            if (last_token_pos->token_type == TokenType::COMMA) {
                if (terms.isEmpty()) {
                    QString error = "Unexpected end to column list on line " + QString::number(last_token_pos->line_number);
                    throw std::logic_error(error.toStdString());
                }

                //save column names
                save_column_names(terms);

                Expression exp(terms);
                
                exps.append(exp);
                column_exprs_terms.append(terms);

                terms = {};
            }
            else {

                Term t(*last_token_pos);
                terms.append(t);
            }
        }

        //throw_exception_if_unexpected_end();
        if (last_token_pos->token_type == TokenType::FROM) {
            has_from_clause = true;
        }
        else if (last_token_pos->token_type == TokenType::SEMICOLON) {
            has_from_clause = false;
        }

        else {
            double line_numer = (*last_token_pos).line_number;
            QString str_num = QString::number(line_numer);

            throw std::logic_error("Unexpected end to column list on line " + str_num.toStdString() + " token seen was " + (*last_token_pos).to_string().toStdString());
        }

        return exps;
    }


    std::shared_ptr<ConditionalExpression> SelectStatement::read_where()
    {
        QList<Term> cond_terms;

        for (; last_token_pos != tokens.cend(); ++last_token_pos) {
            if ((last_token_pos->token_type == TokenType::SEMICOLON) || (last_token_pos->token_type == TokenType::END) 
                || (last_token_pos->token_type == TokenType::INTO) || (last_token_pos->token_type == TokenType::GROUPBY)
                || (last_token_pos->token_type == TokenType::LIMIT)
                )
            {
                break;
            }

            Term t(*last_token_pos);
            ////qDebug()()<<"where term: "<<last_token_pos->to_string();
            cond_terms.append(t);
        }

        if (cond_terms.isEmpty()) {
            QString error = "Unexpected end to where clause!";
            throw std::logic_error(error.toStdString());
        }

        return std::make_shared<ConditionalExpression>(cond_terms);
    }

    std::shared_ptr<ConditionalExpression> SelectStatement::read_having_clause()
    {
        QList<Term> cond_terms;

        for (; last_token_pos != tokens.cend(); ++last_token_pos) {
            if ((last_token_pos->token_type == TokenType::SEMICOLON) || (last_token_pos->token_type == TokenType::END)
                || (last_token_pos->token_type == TokenType::INTO) 
                || (last_token_pos->token_type == TokenType::LIMIT)
                )
            {
                break;
            }

            Term t(*last_token_pos);
            //////////////qDebug()()<<"where term: "<<last_token_pos->to_string();
            cond_terms.append(t);
        }

        if (cond_terms.isEmpty()) {
            QString error = "Unexpected end to Having clause!";
            throw std::logic_error(error.toStdString());
        }

        return std::make_shared<ConditionalExpression>(cond_terms);
    }


    std::shared_ptr<CSVFile> SelectStatement::read_file(QIODeviceBase::OpenMode m)
    {

        throw_exception_if_unexpected_end();

		//qDebug() << "Reading file with mode " << m;

        QString f;
        if ((last_token_pos->token_type == TokenType::NAME) || (last_token_pos->token_type == TokenType::STRING)) {
			//qDebug() << "Token type is " << (last_token_pos->token_type == TokenType::NAME ? "NAME" : "STRING");
            if (last_token_pos->token_type == TokenType::NAME) { // name

				//qDebug() << "Token is a NAME: " << last_token_pos->string_value <<" in symbol table?: "<< symbol_table.contains(last_token_pos->string_value.toLower());
                if (!symbol_table.contains(last_token_pos->string_value.toLower())) {
                    double line_numer = (*last_token_pos).line_number;
                    QString str_num = QString::number(line_numer);

                    //qDebug() << "throwing";

                    throw std::logic_error("Unknown name '"+ last_token_pos->string_value.toStdString() + "' on line " + str_num.toStdString());
                }

                TokenType token_type = symbol_table[last_token_pos->string_value.toLower()];
                if (token_type != TokenType::STRING) {
                    double line_numer = (*last_token_pos).line_number;
                    QString str_num = QString::number(line_numer);

                    throw std::logic_error("Invalid file name on line " + str_num.toStdString());
                }



                f = strings_table[last_token_pos->string_value.toLower()];
            }
            else { //String
                f = last_token_pos->string_value.toLower();
            }

            // check if provided string is a valid file
            if (m != QIODevice::WriteOnly) {
                QFileInfo fileInfo(f);
                if (!fileInfo.exists() || !fileInfo.isFile()) {
                    double line_numer = (*last_token_pos).line_number;
                    QString str_num = QString::number(line_numer);

                    throw std::logic_error("Invalid file provided on line " + str_num.toStdString());
                }
            }


            //left_file = std::make_shared<CSVFile>(f);
        }
        else {
            QString error = "Expected a string or variable referencing a file!";
            throw std::logic_error(error.toStdString());
        }

        if (f.trimmed().isEmpty()) {
            QString error = "No name was provided for file!";
            throw std::logic_error(error.toStdString());
        }

		//qDebug() << "Opening file: " << f << " with mode " << m;
        std::shared_ptr<CSVFile> csv = std::make_shared<CSVFile>(f, m);
        csv->set_token(*last_token_pos);

        return csv;
    }


    std::shared_ptr<CSVFile2> SelectStatement::read_file2()
    {
        throw_exception_if_unexpected_end();

        QString f;
        if ((last_token_pos->token_type == TokenType::NAME) || (last_token_pos->token_type == TokenType::STRING)) {
            if (last_token_pos->token_type == TokenType::NAME) { // name
                if (!symbol_table.contains(last_token_pos->string_value.toLower())) {
                    double line_numer = (*last_token_pos).line_number;
                    QString str_num = QString::number(line_numer);

                    throw std::logic_error("Unknown file name on line " + str_num.toStdString());
                }

                TokenType token_type = symbol_table[last_token_pos->string_value.toLower()];
                if (token_type != TokenType::STRING) {
                    double line_numer = (*last_token_pos).line_number;
                    QString str_num = QString::number(line_numer);

                    throw std::logic_error("Invalid file name on line " + str_num.toStdString());
                }



                f = strings_table[last_token_pos->string_value.toLower()];
            }
            else { //String
                f = last_token_pos->string_value.toLower();
            }

            // check if provided string is a valid file
            QFileInfo fileInfo(f);
            if (!fileInfo.exists() || !fileInfo.isFile()) {
                double line_numer = (*last_token_pos).line_number;
                QString str_num = QString::number(line_numer);

                throw std::logic_error("Invalid file '"+ f.toStdString() + "' provided on line " + str_num.toStdString());
            }


            //left_file = std::make_shared<CSVFile>(f);
        }
        else {
            QString error = "Expected a string or variable referencing a file!";
            throw std::logic_error(error.toStdString());
        }

        if (f.trimmed().isEmpty()) {
            QString error = "No name was provided for file!";
            throw std::logic_error(error.toStdString());
        }

        std::shared_ptr<CSVFile2> csv = std::make_shared<CSVFile2>(f.toStdString());
        csv->set_token(*last_token_pos);

        return csv;
    }


    void SelectStatement::read_out_file()
    {
        throw_exception_if_unexpected_end();

        QString f;
        if ((last_token_pos->token_type == TokenType::NAME) || (last_token_pos->token_type == TokenType::STRING)) {
            if (last_token_pos->token_type == TokenType::NAME) { // name
                if (!symbol_table.contains(last_token_pos->string_value.toLower())) {
                    double line_numer = (*last_token_pos).line_number;
                    QString str_num = QString::number(line_numer);

                    throw std::logic_error("Unknown file name on line " + str_num.toStdString());
                }

                TokenType token_type = symbol_table[last_token_pos->string_value.toLower()];
                if (token_type != TokenType::STRING) {
                    double line_numer = (*last_token_pos).line_number;
                    QString str_num = QString::number(line_numer);

                    throw std::logic_error("Invalid file name on line " + str_num.toStdString());
                }



                f = strings_table[last_token_pos->string_value.toLower()];
            }
            else { //String
                f = last_token_pos->string_value.toLower();
            }


            //left_file = std::make_shared<CSVFile>(f);
        }
        else {
            QString error = "Expected a string or variable referencing a file!";
            throw std::logic_error(error.toStdString());
        }

        if (f.trimmed().isEmpty()) {
            QString error = "No name was provided for file!";
            throw std::logic_error(error.toStdString());
        }

        out_file2 = std::make_unique<QFile>(f);

        if (!out_file2->open(QIODevice::WriteOnly | QIODevice::Text)) {
            double line_numer = (*last_token_pos).line_number;
            QString str_num = QString::number(line_numer);

            throw std::logic_error("Failed to open file '" + f.toStdString() + "' for writing!");
        }
    }

    void SelectStatement::set_query_index(const Term& left_t, int& file_index)
    {
        // check if column name is in columns table
        if (symbol_table.contains(left_t.get_token().string_value.toLower())) {

            if (columns_table.contains(left_t.get_token().string_value.toLower())) {

                if (columns_table.contains(left_t.get_token().string_value.toLower())) {
                    file_index = columns_table[left_t.get_token().string_value.toLower()];
                }
                else {
                    if (left_t.get_token().string_value.contains(".")) {
                        QStringList name_parts = left_t.get_token().string_value.split(".");
                        bool is_number;
                        int col_index = name_parts[1].toInt(&is_number);
                        if (is_number) {
                            file_index = col_index;
                        }
                        else {
                            //error
                            QString error = "Error getting column index for '";
                            error += left_t.get_token().string_value;
                            error += "' on line ";
                            error += QString::number(left_t.get_token().line_number);

                            throw std::logic_error(error.toStdString());
                        }
                    }
                    else {
                        //error
                        QString error = "Error getting column index for '";
                        error += left_t.get_token().string_value;
                        error += "' on line ";
                        error += QString::number(left_t.get_token().line_number);

                        throw std::logic_error(error.toStdString());
                    }
                }
                file_index = columns_table[left_t.get_token().string_value.toLower()];
                //////////qDebug()() << "[.] Query index number set: " << columns_table[left_t.get_token().string_value.toLower()];
            }
            else {
                QString error = "Error getting column index for '";
                error += left_t.get_token().string_value;
                error += "' on line ";
                error += QString::number(left_t.get_token().line_number);

                throw std::logic_error(error.toStdString());
            }

        }
        else { // check if column name is of format file.number
            QStringList column_name_parts = left_t.get_token().string_value.split('.');
            bool is_number;
            double number = column_name_parts[1].toDouble(&is_number);  // try and convert number part of name

            if (is_number) {
                //if (filename1 == join_files_list["right"]) {
                //////////qDebug()() << "[.] Query index number set: " << number;
                file_index = number;
                //}

            }
            else {
                //error
                std::string error = "Invalid column in ON clause on line ";
                error += QString::number(last_token_pos->line_number).toStdString();
                throw std::logic_error(error);
            }
        }

		//qDebug() << "Set query index for " << left_t.get_token().string_value << " to " << file_index;
    }

    Term SelectStatement::read_join_column()
    {
        Term left_t(*last_token_pos);
        //QStringList name_parts = left_t.get_token().string_value.split(',');

        //check if column belongs to the joined files
        QStringList column_name_parts = left_t.get_token().string_value.split('.');
        if (column_name_parts.size() != 2) {
            std::string error = "Ambigious column name in ON clause on line ";
            error += QString::number(last_token_pos->line_number).toStdString();
            throw std::logic_error(error);
        }
        else {
            QString file_alias = column_name_parts[0].toLower();
            if (!symbol_table.contains(file_alias)) {
                std::string error = "1 Column name does not refer to either files in the join statement on line ";
                error += QString::number(last_token_pos->line_number).toStdString();
                throw std::logic_error(error);
            }

            if (symbol_table[file_alias] != TokenType::STRING) {
                std::string error = "2 Column name does not refer to either files in the join statement on line ";
                error += QString::number(last_token_pos->line_number).toStdString();
                throw std::logic_error(error);
            }

            QString file_path = strings_table[file_alias];
            if (!join_files_list.values().contains(file_path)) {
            //if (!join_files_list.values().contains(file_alias)) {
                std::string error = "Column name does not refer to either files in the join statement on line ";
                error += QString::number(last_token_pos->line_number).toStdString();
                throw std::logic_error(error);
            }

            if (file_path == join_files_list["right"]) {

                set_query_index(left_t, this->query_index);  
            }
            else if (file_path == join_files_list["left"]) {
                set_query_index(left_t, this->left_query_index);
            }
            
        }

		//qDebug() << "Join column read: " << left_t.get_token().string_value << " index: " << this->query_index;

        /*
        if (last_token_pos->token_type == TokenType::NAME) {


            QString filename1 = left_t.get_token().string_value.split('.')[0].toLower();
            QString column_index = left_t.get_token().string_value.split('.')[1];
            if (column_index == "*") {
                //error
                std::string error = "Unexpected column '*' in ON clause on line ";
                error += QString::number(last_token_pos->line_number).toStdString();
                throw std::logic_error(error);
            }
            else {// check if column is of the form file.0
                bool is_number;
                double number = column_index.toDouble(&is_number);
                if (is_number) {
                    if (filename1 == join_files_list["right"]) {
                        ////////qDebug()() << "[NAME] Query index number set: " << number;
                        this->query_index = number;
                    }

                }
                else {
                    //error
                    std::string error = "Invalid column in ON clause on line ";
                    error += QString::number(last_token_pos->line_number).toStdString();
                    throw std::logic_error(error);
                }
            }
        }
        else if (last_token_pos->token_type == TokenType::COLUMNNAME) {
            QString filename1 = left_t.get_token().string_value.split(',')[0].toLower();
            if (filename1 == join_files_list["right"]) {
                if (columns_table.contains(left_t.get_token().string_value.toLower())) {
                    ////////qDebug()() << "[COLUMNNAME] Query index number set: " << columns_table[left_t.get_token().string_value.toLower()];;
                    this->query_index = columns_table[left_t.get_token().string_value.toLower()];
                }
                else {
                    //error
                    std::string error = "Unknown column in ON clause on line ";
                    error += QString::number(last_token_pos->line_number).toStdString();
                    throw std::logic_error(error);
                }
            }
        }
        else {
            //error
            std::string error = "Expected a column in ON clause on line ";
            error += QString::number(last_token_pos->line_number).toStdString();
            throw std::logic_error(error);
        }
        */
        return left_t;

    }

    std::shared_ptr<ConditionalExpression> SelectStatement::read_on_clause()
    {
        throw_exception_if_unexpected_end();

        QList<Term> terms;

        ////////////qDebug()()<<"joined files: "<<join_files_list;

        if (last_token_pos->token_type != TokenType::ON) {
            //error
            std::string error = "1. Invalid syntax on line ";
            error += QString::number(last_token_pos->line_number).toStdString();
            error += " expected an ON clause ";
            error += last_token_pos->string_value.toStdString();
            throw std::logic_error(error);
        }

        ++last_token_pos; //next token
        throw_exception_if_unexpected_end();

        // read join_column();
        Term left_t = read_join_column();

        terms.append(left_t);




        ++last_token_pos; //next token
        throw_exception_if_unexpected_end();

        if (last_token_pos->token_type != TokenType::ASSIGN) {
            //error
            std::string error = "Operator in an ON clause should be '=' on line ";
            error += QString::number(last_token_pos->line_number).toStdString();
            throw std::logic_error(error);
        }

        Term op_t(*last_token_pos);
        terms.append(op_t);


        ++last_token_pos; //next token
        throw_exception_if_unexpected_end();

        Term right_t = read_join_column();
        terms.append(right_t);

        return std::make_shared<ConditionalExpression>(terms);

    }

    void SelectStatement::handle_limit_clause()
    {
        has_limit_clause = true;

        ++last_token_pos; // next token
        throw_exception_if_unexpected_end();

        if (last_token_pos->token_type == TokenType::NUMBER || last_token_pos->token_type == TokenType::NAME) {

            int limit = 0;

            if (last_token_pos->token_type == TokenType::NAME) {
                if (!numbers_table.contains(last_token_pos->string_value.toLower())) {
                    QString error = "Number expected in limit clause on line ";
                    error += QString::number(last_token_pos->line_number);
                    error += "!";
                    throw std::logic_error(error.toStdString());
                }
                
                limit = numbers_table[last_token_pos->string_value.toLower()];

                
            }
            else if (last_token_pos->token_type == TokenType::NUMBER) {
                limit = last_token_pos->number_value;
            }

            //check limit value provided
            if (limit > 0) {
                LIMIT_VAL = limit;
            }
            else {
                QString error = "Number in limit clause on line ";
                error += QString::number(last_token_pos->line_number);
                error += " should be greater than zero!";
                throw std::logic_error(error.toStdString());
            }

        }

        ++last_token_pos; // next token

        if (last_token_pos == tokens.cend()) {
            return;
        }

        if (last_token_pos->token_type == TokenType::END || last_token_pos->token_type == TokenType::SEMICOLON) {
            return;
        }

        QList<TokenType> valid_next_tokens = {  TokenType::INTO };
        if (!valid_next_tokens.contains(last_token_pos->token_type)) {
            //throw error
            std::string error = "Unexpected token (";
            error += last_token_pos->to_string().toStdString();
            error += ") on line ";
            error += QString::number(last_token_pos->line_number).toStdString();
            throw std::logic_error(error);
        }

        optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
    }

    void SelectStatement::handle_into_clause()
    {
        //qDebug() << "Handling INTO clause...";
        ++last_token_pos; // next token

        this->out_file = read_file(QIODevice::WriteOnly);
        this->write_to_file = true;

        //file_writer_thread = std::make_unique<std::thread>(&SelectStatement::csv_file_writer, this); // run csv file reader in different thread
        //qDebug() << "write thread created";

        //QString of = out_file->get_token().string_value.toLower();
        QString of = out_file->get_file_name();

        //qDebug() << "File name: " << out_file->get_file_name() << " join_files_list:"<<join_files_list;
        foreach(auto file_name, join_files_list) {
            if (of.toLower() == file_name.toLower()) {
                std::string error = "Output cannot be the same as a file being read from in Select statement on line ";
                error += std::to_string(last_token_pos->line_number);
                throw std::logic_error(error);
            }
        }

        //file_writer_thread = std::make_unique<std::thread>(&SelectStatement::csv_file_writer, this); // run csv file reader in different thread

        //////////qDebug()() << "Done.";
        ++last_token_pos; // next token

        if (last_token_pos == tokens.cend()) {
            file_writer_thread = std::make_unique<std::thread>(&SelectStatement::csv_file_writer, this); // run csv file reader in different thread
            return;
        }

        if (last_token_pos->token_type == TokenType::END || last_token_pos->token_type == TokenType::SEMICOLON ) {
            file_writer_thread = std::make_unique<std::thread>(&SelectStatement::csv_file_writer, this); // run csv file reader in different thread
            return;
        }

        QString error = "Invalid syntax: unexpected '";
        error += last_token_pos->string_value;
        error += "' after INTO CLAUSE!";
        throw std::logic_error(error.toStdString());

        //file_writer_thread = std::make_unique<std::thread>(&SelectStatement::csv_file_writer, this); // run csv file reader in different thread

        
    }

    void SelectStatement::handle_inner_join()
    {
        this->has_join = true;
        this->join_type = last_token_pos->token_type;

        ++last_token_pos; // next token

        this->right_file2 = read_file2();
        join_files_list["right"] = strings_table[right_file2->get_token().string_value.toLower()];

        if (join_files_list["right"] == join_files_list["left"]) {
            QString error = "Invalid join on same file '";
            error += join_files_list["right"];
            error += "!'";
            throw std::logic_error(error.toStdString());
        }

        //////////qDebug()() << "join files:" << join_files_list;

        ++last_token_pos; // next token
        this->on_clause = read_on_clause();

        ++last_token_pos; // next token

        if (last_token_pos == tokens.cend()) {
            return;
        }

        if (last_token_pos->token_type == TokenType::END || last_token_pos->token_type == TokenType::SEMICOLON) {
            return;
        }

        QList<TokenType> valid_next_tokens = { TokenType::HAVING, TokenType::LIMIT, TokenType::INTO, TokenType::WHERE, TokenType::GROUPBY };
        if (!valid_next_tokens.contains(last_token_pos->token_type)) {
            //throw error
            std::string error = "Unexpected token (";
            error += last_token_pos->to_string().toStdString();
            error += ") on line ";
            error += QString::number(last_token_pos->line_number).toStdString();
            throw std::logic_error(error);
        }

        optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
    }

    void SelectStatement::handle_outer_join()
    {
        this->has_join = true;
        this->join_type = last_token_pos->token_type;

        ++last_token_pos; // next token

        this->right_file2 = read_file2();
        join_files_list["right"] = strings_table[right_file2->get_token().string_value.toLower()];

        if (join_files_list["right"] == join_files_list["left"]) {
            QString error = "Invalid join on same file '";
            error += join_files_list["right"];
            error += "'!";
            throw std::logic_error(error.toStdString());
        }

        ++last_token_pos; // next token
        this->on_clause = read_on_clause();

        //////////qDebug()() << "done reading on clause";

        ++last_token_pos; // next token

        //////////qDebug()() << "done moving to next token";

        if (last_token_pos == tokens.cend()) {
            return;
        }

        if (last_token_pos->token_type == TokenType::END || last_token_pos->token_type == TokenType::SEMICOLON ) {
            //////////qDebug()() << "done parsing outer join";
            return;
        }

        QList<TokenType> valid_next_tokens = { TokenType::HAVING, TokenType::LIMIT, TokenType::INTO, TokenType::WHERE, TokenType::GROUPBY };
        if (!valid_next_tokens.contains(last_token_pos->token_type)) {
            //throw error
            std::string error = "Unexpected token (";
            error += last_token_pos->to_string().toStdString();
            error += ") on line ";
            error += QString::number(last_token_pos->line_number).toStdString();
        }

        optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
    }

    void SelectStatement::handle_cross_join()
    {
        this->has_join = true;
        this->join_type = last_token_pos->token_type;

        ++last_token_pos; // next token

        this->right_file2 = read_file2();
        join_files_list["right"] = right_file2->get_token().string_value.toLower();

        if (join_files_list["right"] == join_files_list["left"]) {
            QString error = "Invalid join on same file '";
            error += join_files_list["right"];
            error += "'!";
            throw std::logic_error(error.toStdString());
        }

        ++last_token_pos; // next token

        if (last_token_pos == tokens.cend()) {
            return;
        }

        if (last_token_pos->token_type == TokenType::END || last_token_pos->token_type == TokenType::SEMICOLON) {
            return;
        }

        QList<TokenType> valid_next_tokens = { TokenType::HAVING, TokenType::LIMIT, TokenType::INTO, TokenType::WHERE, TokenType::GROUPBY };
        if (!valid_next_tokens.contains(last_token_pos->token_type)) {
            //throw error
            std::string error = "Unexpected token (";
            error += last_token_pos->to_string().toStdString();
            error += ") on line ";
            error += QString::number(last_token_pos->line_number).toStdString();
        }

        optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
    }

    void SelectStatement::handle_where_clause()
    {
        //////qDebug()() << "----WHERE HANDLER CALLED!";
        this->has_where_clause = true;
        ++last_token_pos; // next token

        this->conditional_expr = read_where(); //clause clause

        if (last_token_pos == tokens.cend()) {
            return;
        }

        //++last_token_pos; // next token

        if (last_token_pos == tokens.cend()) {
            //////////qDebug()() << "-------End of select found in where clause handler";
            return;
        }

        if (last_token_pos->token_type == TokenType::END || last_token_pos->token_type == TokenType::SEMICOLON) {
            return;
        }

        QList<TokenType> valid_next_tokens = { TokenType::HAVING, TokenType::LIMIT, TokenType::GROUPBY, TokenType::INTO };
        if (!valid_next_tokens.contains(last_token_pos->token_type)) {
            //throw error
            std::string error = "Unexpected token (";
            error += last_token_pos->to_string().toStdString();
            error += ") on line ";
            error += QString::number(last_token_pos->line_number).toStdString();
        }
        //////////qDebug()() << "Done with WHERE clause moving on to next clause...";
        //////////qDebug()() << "Token after where clause is " << last_token_pos->to_string() << " with string value: " << last_token_pos->string_value;
        optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
    }

    void SelectStatement::handle_having_clause()
    {
        //////////qDebug()() << "----HAVING Clause HANDLER CALLED!";
        this->has_having_clause = true;
        ++last_token_pos; // next token

        this->having_conditional_expr = read_having_clause(); //clause clause

        if (last_token_pos == tokens.cend()) {
            return;
        }

        //++last_token_pos; // next token

        if (last_token_pos == tokens.cend()) {
            //////////qDebug()() << "-------End of select found in having clause handler";
            return;
        }

        if (last_token_pos->token_type == TokenType::END || last_token_pos->token_type == TokenType::SEMICOLON) {
            return;
        }

        QList<TokenType> valid_next_tokens = { TokenType::LIMIT, TokenType::INTO };
        if (!valid_next_tokens.contains(last_token_pos->token_type)) {
            //throw error
            std::string error = "Unexpected token (";
            error += last_token_pos->to_string().toStdString();
            error += ") on line ";
            error += QString::number(last_token_pos->line_number).toStdString();
        }

        optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
    }

    void SelectStatement::handle_groupby_clause()
    {
        //////////qDebug()() << "group by found!";

        has_group_by = true;

        ++last_token_pos; // next token

        throw_exception_if_unexpected_end();
        //////////qDebug()() << "Here!" << last_token_pos->to_string();

        while (last_token_pos != tokens.cend()) {
            //////////qDebug()() << "Here!" << last_token_pos->to_string();
            if (last_token_pos->token_type == TokenType::SEMICOLON || last_token_pos->token_type == TokenType::END 
                || last_token_pos->token_type ==TokenType::INTO
                || last_token_pos->token_type == TokenType::LIMIT
                || last_token_pos->token_type == TokenType::HAVING
                ) 
            {
                //////////qDebug()() << "exit Here!" << last_token_pos->to_string();
                break;
            }
            //////////qDebug()() << "token: " << last_token_pos->to_string();
            if (last_token_pos->token_type == TokenType::COLUMNNUMBER) {

                if (has_join) {
                    QString error = "Ambigious column ";
                    error += last_token_pos->string_value;
                    error += " on line ";
                    error += QString::number(last_token_pos->line_number);
                    throw std::logic_error(error.toStdString());
                }

                this->group_by_columns.append(last_token_pos->string_value.toLower());
                //////////qDebug()() << "group by columns"<<group_by_columns;
            }
            else if (last_token_pos->token_type == TokenType::COLUMNNAME) {
                this->group_by_columns.append(last_token_pos->string_value.toLower());
            }
            else if (last_token_pos->token_type == TokenType::NAME) { // look out for column names of format variable.number e.g. variable.2
                QString column_name = last_token_pos->string_value;

                QStringList name_parts = column_name.split('.');

                if (name_parts.count() != 2) {
                    QString error = "Unknown column in group by clause ";
                    error += last_token_pos->string_value;
                    error += " on line ";
                    error += QString::number(last_token_pos->line_number);
                    throw std::logic_error(error.toStdString());
                }

                if (!symbol_table.contains(name_parts[0].toLower()) && strings_table.contains(name_parts[0].toLower())) {
                    QString error = "0 Unknown column in group by clause ";
                    error += last_token_pos->string_value;
                    error += " on line ";
                    error += QString::number(last_token_pos->line_number);
                    throw std::logic_error(error.toStdString());
                }
                
                QString file_name = strings_table[name_parts[0].toLower()];
                //////////qDebug()() << "join table " << join_files_list;
                if (file_name != join_files_list["left"] && file_name != join_files_list["right"]) {
                    QString error = "1 Unknown column in group by clause ";
                    error += last_token_pos->string_value;
                    error += " on line ";
                    error += QString::number(last_token_pos->line_number);
                    throw std::logic_error(error.toStdString());
                }

                //check if second part of name is a number
                bool is_number = false;
                name_parts[1].toInt(&is_number);

                if (!is_number) {
                    QString error = "2 Unknown column in group by clause ";
                    error += last_token_pos->string_value;
                    error += " on line ";
                    error += QString::number(last_token_pos->line_number);
                    throw std::logic_error(error.toStdString());
                }

                this->group_by_columns.append(last_token_pos->string_value.toLower());

            }

            ++last_token_pos; // next token
        }

        if (group_by_columns.isEmpty()) {
            QString error = "No column provided to Group by on line ";
            error += QString::number(last_token_pos->line_number);
            throw std::logic_error(error.toStdString());
        }

        //++last_token_pos; // next token
        //////////qDebug()() << "group by columns: " << group_by_columns;

        if (last_token_pos == tokens.cend()) {
            return;
        }

        if (last_token_pos->token_type == TokenType::END || last_token_pos->token_type == TokenType::SEMICOLON) {
            return;
        }

        QList<TokenType> valid_next_tokens = { TokenType::HAVING, TokenType::LIMIT, TokenType::INTO };
        if (!valid_next_tokens.contains(last_token_pos->token_type)) {
            //throw error
            std::string error = "Unexpected token (";
            error += last_token_pos->to_string().toStdString();
            error += ") on line ";
            error += QString::number(last_token_pos->line_number).toStdString();
            throw std::logic_error(error);
        }

        optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
    }


    void SelectStatement::parse()
    {
        //eat select
        ++last_token_pos;

        //read columns
        column_exprs = read_column_expressions();

        if (last_token_pos->token_type == TokenType::SEMICOLON) {
            return;
        }

        // FROM Clause
        // check if there is a next token after FROM
        ++last_token_pos;
        if (last_token_pos == tokens.cend()) {
            --last_token_pos; //get last but one token
            double line_numer = (*last_token_pos).line_number;
            QString str_num = QString::number(line_numer);

            throw std::logic_error("Unexpected end to SELECT statement on line " + str_num.toStdString());
        }


        // read file name or path string and open file
        this->left_file2 = read_file2();
        join_files_list["left"] = strings_table[left_file2->get_token().string_value.toLower()];

        file_reader_thread = std::make_unique<std::thread>(&SelectStatement::csv_file_reader, this); // run csv file reader in different thread

        ++last_token_pos; // next token
        ////////////qDebug()()<<"1. token after left file is "<< last_token_pos->to_string() <<" {"<<last_token_pos->string_value<<"}";

        if ((last_token_pos == tokens.cend()) || (last_token_pos->token_type == TokenType::SEMICOLON)) {
            //file_reader_thread = std::make_unique<std::thread>(&SelectStatement::csv_file_reader, this); // run csv file reader in different thread
            return;
        }

        ////////////qDebug()()<<"2. token after left file is "<< last_token_pos->to_string()<<" {"<<last_token_pos->string_value<<"}";
        // What is the next token?

        std::function<void()> action;

        if (last_token_pos->token_type == TokenType::INNERJOIN) {
            action = optional_actions[last_token_pos->token_type]; //call handler function for the next valid token
        }
        else if (last_token_pos->token_type == TokenType::OUTERJOIN) {
            action = optional_actions[last_token_pos->token_type]; //call handler function for the next valid token
        }
        else if (last_token_pos->token_type == TokenType::CROSSJOIN) {
            action = optional_actions[last_token_pos->token_type]; //call handler function for the next valid token
        }
        else if (last_token_pos->token_type == TokenType::WHERE) {
            action = optional_actions[last_token_pos->token_type]; //call handler function for the next valid token
        }
        else if (last_token_pos->token_type == TokenType::GROUPBY) {
            action = optional_actions[last_token_pos->token_type]; //call handler function for the next valid token
        }
        else if (last_token_pos->token_type == TokenType::HAVING) {
            action = optional_actions[last_token_pos->token_type]; //call handler function for the next valid token
        }
        else if (last_token_pos->token_type == TokenType::INTO) {
            action = optional_actions[last_token_pos->token_type]; //call handler function for the next valid token
        }
        else if (last_token_pos->token_type == TokenType::LIMIT) {
            action = optional_actions[last_token_pos->token_type]; //call handler function for the next valid token
        }
        else { // invalid token
            double line_numer = (*last_token_pos).line_number;
            QString str_num = QString::number(line_numer);
            

            throw std::logic_error("2. Unexpected end to SELECT statement on line " + str_num.toStdString());
        }
        //////////qDebug()("parsing select done.");
        //file_reader_thread = std::make_unique<std::thread>(&SelectStatement::csv_file_reader, this); // run csv file reader in different thread
        action();
    }



    QStringList SelectStatement::compute_columns(const QMap<QString, QStringList>& data_rows)
    {
        QStringList columns;
        columns.reserve(10);
        foreach(auto e, column_exprs) {
            Term ct = e.eval(data_rows);
            QString column;
            if (ct.get_token().token_type == TokenType::STRING) {
                column = ct.get_token().string_value;
            }
            else if (ct.get_token().token_type == TokenType::NUMBER) {
                //////////qDebug()() << "number: " << ct.get_token().number_value;

                const double epsilon = 1e-9;
                double num = ct.get_token().number_value;
                bool isWhole = std::fabs(num - std::round(num)) < epsilon;

                QString num_string = isWhole ? QString::number(num, 'f', 0)
                    : QString::number(num, 'g', 2);

                column = num_string;

            }
            columns.append(column);
        }
        return columns;
    }

    std::function<QStringList(const QMap<QString, QStringList>&)> SelectStatement::compile_columns(const QMap<QString, QStringList> data_rows)
    {
        QList<std::function<Term(const QMap<QString, QStringList>&)>> compiled_columns;
        compiled_columns.reserve(column_exprs.size());

        //const QMap<QString, QStringList> data_rows = data_rows;

        for (auto& expr : column_exprs) {
            compiled_columns.append(expr.compile(data_rows)); // Must capture data_rows by value!
        }

        return [compiled_columns = std::move(compiled_columns)](
            const QMap<QString, QStringList>& data_row) mutable -> QStringList
            {
                QStringList result;
                result.reserve(compiled_columns.size());

                for (const auto& func : compiled_columns) {
                    Term term = func(data_row);
                    const Token& token = term.get_token();
                    QString col;

                    switch (token.token_type) {
                    case TokenType::STRING:
                        col = token.string_value;
                        break;
                    case TokenType::NUMBER: {
                        double num = token.number_value;
                        bool is_integer = std::fabs(num - std::round(num)) < 1e-9;
                        col = is_integer ? QString::number(num, 'f', 0)
                            : QString::number(num, 'f', 6);
                        break;
                    }
                    default:
                        col = ""; // or "<null>", or throw
                        break;
                    }
                    result.append(col);
                }
                return result;
            };
    }

    QString SelectStatement::create_group_by_key(const QMap<QString, QStringList>& data_rows)
    {
        QStringList row;
        QString key;

        if (this->group_by_columns.isEmpty()) {
            key = "$";
            return key;
        }
        else {
            QStringList vals;
            foreach(auto c, this->group_by_columns) {
                Token tk;
                //////////qDebug()() << "creating group by key with column: "<< c;
                if (c.front() == "[") {
                    tk.token_type = TokenType::COLUMNNUMBER;
                    tk.string_value = c;
                    c = c.trimmed();
                    c.remove(0, 1); //delete [
                    c.chop(1); // delete trailing ]
                    tk.number_value = c.toInt();
                }
                else {
                    tk.token_type = TokenType::NAME;
                    tk.string_value = c;
                }
                Term tm(tk);
                //////////qDebug()() << "creating group by key with column (tm.eval(data_rows).string_value): " << tm.eval(data_rows).string_value;
                vals.append(tm.eval(data_rows).string_value); //get current value for group by column
            }
            key = vals.join(",");
            //////////qDebug()() << "key is " << key;
        }


        return key;
    }

    std::function < QString(const QMap<QString, QStringList>&)> SelectStatement::compile_group_by_key(const QMap<QString, QStringList>& data_rows)
    {

        std::function < QString(const QMap<QString, QStringList>&)> group_by_fuc;

        QStringList row;
        QString key;

        if (this->group_by_columns.isEmpty()) {

            group_by_fuc = [](const QMap<QString, QStringList>& data_rows) {
                QString key = "$";
                return key;
                };

            return group_by_fuc;
        }
        else {
            //QStringList vals;
            QList<std::function<Token(const QMap<QString, QStringList>& )>> vals_func;
            foreach(auto c, this->group_by_columns) {
                Token tk;
                //////////qDebug()() << "creating group by key with column: "<< c;
                if (c.front() == "[") {
                    tk.token_type = TokenType::COLUMNNUMBER;
                    tk.string_value = c;
                    c = c.trimmed();
                    c.remove(0, 1); //delete [
                    c.chop(1); // delete trailing ]
                    tk.number_value = c.toInt();
                }
                else {
                    tk.token_type = TokenType::NAME;
                    tk.string_value = c;
                }
                Term tm(tk);
                //////////qDebug()() << "creating group by key with column (tm.eval(data_rows).string_value): " << tm.eval(data_rows).string_value;
                //vals.append(tm.eval(data_rows).string_value); //get current value for group by column
                vals_func.append(tm.compile(data_rows));
            }
            //key = vals.join(",");
            //////////qDebug()() << "key is " << key;
            group_by_fuc = [vals_func](const QMap<QString, QStringList>& data_rows) {
                QStringList vals;
                foreach(auto& f, vals_func) {
                    vals.append(f(data_rows).string_value);
                }

                return vals.join(",");
            };
        }


        return group_by_fuc;
    }

    void SelectStatement::validate_aggregate_query()
    {
        // validate if columns in select are in group by column list
        int illegal_column_count = 0;
        QString column_name;
        int line_number = 0;
        
        int aggreg_func_count = 0; // check if select has at least 1 aggregation function
        int select_statement_line_num = column_exprs_terms.front().get_token().line_number;

        if (has_group_by || is_aggregation) {
            foreach(auto col_term, column_exprs_terms) {
                if (col_term.get_token().token_type == TokenType::COLUMNNUMBER
                    || col_term.get_token().token_type == TokenType::COLUMNNAME
                    || col_term.get_token().token_type == TokenType::NAME // HANDLE variable.number ? e.g. f.0
                    )
                {
                    column_name = col_term.get_token().string_value.toLower();

                    if (!group_by_columns.contains(column_name)) {
                        
                        line_number = col_term.get_token().line_number;
                        ++illegal_column_count;
                    }
                }

                if (col_term.get_token().token_type == TokenType::FUNCTION) {
                    if (col_term.get_token().string_value.toLower().contains("count")
                        || col_term.get_token().string_value.toLower().contains("sum")
                        || col_term.get_token().string_value.toLower().contains("avg")
                        || col_term.get_token().string_value.toLower().contains("min")
                        || col_term.get_token().string_value.toLower().contains("max")
                        )
                    {
                        ++aggreg_func_count;
                    }
                }
            }

            if (illegal_column_count > 0) {
                QString error = "column (";
                error += column_name;
                error += ") ";
                error += "not in group by list ";
                error += "on line ";
                error += QString::number(line_number);
                throw std::logic_error(error.toStdString());
            }

            if (aggreg_func_count == 0) {
                QString error = "There was no aggregation function found in select statement on line ";
                error += QString::number(select_statement_line_num);
                throw std::logic_error(error.toStdString());
            }

        }
    }

    bool SelectStatement::process_data(const QMap<QString, QStringList>& data_rows)
    {
        //QMap<QString, QStringList> data_rows;
        //data_rows["$"] = row;
        QStringList columns;

        //std::function<Term(const QMap<QString, QStringList>&)> compiled_conditional_func;
        //std::function < QString(const QMap<QString, QStringList>&)> compiled_group_by_key_func;
        //std::function<QStringList(const QMap<QString, QStringList>&)> compiled_columns_func;

        //////qDebug()() << "row size: " << row.size() << " row:"<< row;
        //////qDebug()() << "row number: " << NUMBER_OF_ROWS;

        if (has_where_clause) {
            //Term t = conditional_expr->eval(data_rows);
            Term t;

            if (!is_conditional_compiled) {
                compiled_conditional_func = conditional_expr->compile(data_rows);
                t = compiled_conditional_func(data_rows);
                is_conditional_compiled = true;
            }
            else {
                t = compiled_conditional_func(data_rows);
            }
            
            ////////////qDebug()()<<"Conditional evaluation done.";

            if (t.get_token().boolean_value == true) {

                //columns = compute_columns(data_rows);

                if (has_group_by || is_aggregation) {

                    //////qDebug()() << "processing group by with where clause";
                    //QString result_key = create_group_by_key(data_rows);
                    QString result_key;

                    if (!is_group_by_key_compiled) {
                        compiled_group_by_key_func = compile_group_by_key(data_rows);
                        result_key = compiled_group_by_key_func(data_rows);
                        is_group_by_key_compiled = true;
                    }
                    else {
                        result_key = compiled_group_by_key_func(data_rows);
                    }

                    aggregate_expression_reg_key = result_key;

                    //columns = compute_columns(data_rows);

                    if (!are_columns_compiled) {
                        compiled_columns_func = compile_columns(data_rows);
                        columns = compiled_columns_func(data_rows);
                        are_columns_compiled = true;
                    }
                    else {
                        columns = compiled_columns_func(data_rows);
                    }

                    group_by_result[result_key] = columns;

                    //////qDebug()() << "Done.";
                }
                else {

                    if (has_limit_clause && LIMIT_VAL <= NUMBER_OF_ROWS) { // handle limit clause
                        limit_done = true;
                        
                        //break;
                        done_consuming.store(true, std::memory_order_release);
                        return true; //end processing
                    }

                    //columns = compute_columns(data_rows);
                    if (!are_columns_compiled) {
                        compiled_columns_func = compile_columns(data_rows);
                        columns = compiled_columns_func(data_rows);
                        are_columns_compiled = true;
                    }
                    else {
                        columns = compiled_columns_func(data_rows);
                    }

                    //write to file?
                    if (write_to_file) {
                        //out_file->writeLine(columns.join(','));
                       // write_queue->push(columns.join(','));

                        push_to_write_queue(columns.join(','));
                       

                        ++NUMBER_OF_ROWS;
                        //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                        //NUMBER_OF_ROWS.fetch_add(1);

                        //if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                        //    return result;
                        //}
                    }
                    else {
                        result->append(columns);
                        ++NUMBER_OF_ROWS;
                        //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                        //NUMBER_OF_ROWS.fetch_add(1);

                        if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                            if (NUMBER_OF_ROWS == 0) {
                                //return std::nullopt;
                                result = std::nullopt;
                                done_consuming.store(true, std::memory_order_release);
                                
                            }
                            //return result;
                            //query_done.store(false, std::memory_order_release);
                            paused.store(true, std::memory_order_release);
                            return true; // end processing
                        }
                    }
                }

            }
        }
        else { //no where clause
            //columns = compute_columns(data_rows);

            if (has_group_by || is_aggregation) {
                //QString result_key = create_group_by_key(data_rows);
                

                QString result_key;

                if (!is_group_by_key_compiled) {
                    compiled_group_by_key_func = compile_group_by_key(data_rows);
                    result_key = compiled_group_by_key_func(data_rows);
                    is_group_by_key_compiled = true;
                }
                else {
                    result_key = compiled_group_by_key_func(data_rows);
                }

                aggregate_expression_reg_key = result_key;

                //columns = compute_columns(data_rows);
                if (!are_columns_compiled) {
                    compiled_columns_func = compile_columns(data_rows);
                    columns = compiled_columns_func(data_rows);
                    are_columns_compiled = true;
                }
                else {
                    columns = compiled_columns_func(data_rows);
                }

                group_by_result[result_key] = columns;
            }
            else {
				//qDebug() << "processing row without where clause 2";

                if (has_limit_clause && LIMIT_VAL <= NUMBER_OF_ROWS) { // handle limit clause
                    limit_done = true;
                    //qDebug() << "limit reached limit: " << LIMIT_VAL <<", number of rows read:"<< NUMBER_OF_ROWS;
                    //break;
                    done_consuming.store(true, std::memory_order_release);
                    return true; // end processing
                }

                //columns = compute_columns(data_rows);
                if (!are_columns_compiled) {
                    //////////qDebug()() << "compiling columns";
                    compiled_columns_func = compile_columns(data_rows);
                    //////////qDebug()() << "Done.";
                    //////////qDebug()() << "using compiled function";
                    columns = compiled_columns_func(data_rows);
                    //////////qDebug()() << "Done.";
                    //////////qDebug()() << "columns: " << columns;
                    are_columns_compiled = true;
                }
                else {
                    //////////qDebug()() << "calling compiled function again.";
                    columns = compiled_columns_func(data_rows);
                    //////////qDebug()() << "Done.";
                    //////////qDebug()() << "columns: "<<columns;
                }

				//qDebug() << "computed columns: " << columns;

                //write to file?
                if (write_to_file) {
                    //out_file->writeLine(columns.join(','));
                    //write_queue->push(columns.join(','));
                    push_to_write_queue(columns.join(','));
                    ++NUMBER_OF_ROWS;

                    //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                    //NUMBER_OF_ROWS.fetch_add(1);

                    //if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                    //    return result;
                    //}
                }
                else {
                    result->append(columns);
                    ++NUMBER_OF_ROWS;
                    //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                    //NUMBER_OF_ROWS.fetch_add(1);

                    if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                        if (NUMBER_OF_ROWS == 0) {
                            //return std::nullopt;
                            result = std::nullopt;
                            done_consuming.store(true, std::memory_order_release);
                        }
                        //query_done.store(false, std::memory_order_release);
                        paused.store(true, std::memory_order_release);
                        return true; // end processing
                        //return result;
                    }
                }
            }

        }

        //columns = {}; //reset
        ////////qDebug()() << "Resetting is_aggregation";
        if (has_group_by || is_aggregation) {
            for (auto it = check_if_aggregate_done.begin(); it != check_if_aggregate_done.end(); ++it) {
                //it.value().value() = false;
                for (auto i = it.value().begin(); i != it.value().end(); ++i) {
                    i.value() = false;
                }
            }
        }
        //qDebug() << "\nDone. Result is "<<result;

        return false; //continue processing
    }
    
    /*
    void SelectStatement::consume_data()
    {
        QList<QStringList>* batch;
        //batch.reserve(PROCESS_BATCH_ROWS);
        bool stop = false;
        while (true) {

            while (processing_queue.pop(batch)) {
                foreach(auto& row, (*batch)) {
                    stop = process_data(row);
                    if ((query_done && processing_queue.empty()) || stop) {
                        delete batch;
                        return;
                    }
                }
            }

            if ((query_done && processing_queue.empty()) || stop) {
                delete batch;
                return;
            }
            QThread::msleep(10);
        }

        
    }
    */

    void SelectStatement::csv_file_writer()
    {
        //qDebug() << "write thread started";
        while(!done_consuming.load(std::memory_order_acquire) || !write_queue->empty()){
            //QString row;
			std::vector<QString> rows;

            if (write_queue->pop(rows)) {
                //write_queue->pop();
                //////////qDebug()() << "writing row";

                for (auto& row : rows) {
                    if (row.trimmed() == "") {
                        continue;
                    }

                    out_file->writeLine(row);
                    //qDebug() << "Written row: " << row;
                }
                
                //////////qDebug()() << "done.";
            }
            else {
                std::this_thread::yield();
            }
            
        }

        if (!write_batch.empty()) {
            // After reading BATCH_ROWS items
            std::reverse(write_batch.begin(), write_batch.end()); // FIFO
            for (auto& row : write_batch) {
                if (row.trimmed() == "") {
                    continue;
                }

                out_file->writeLine(row);
                //qDebug() << "Written row: " << row;
            }
            write_batch = std::vector<QString>();
        }

        //qDebug() << "exiting write thread";
    }

    void SelectStatement::push_to_write_queue(const QString& row)
    {
        if (write_batch.size() >= WRITE_BATCH_ROWS) {

            // After reading BATCH_ROWS items
            std::reverse(write_batch.begin(), write_batch.end()); // FIFO

            while (!write_queue->push(write_batch)) {
                std::this_thread::yield();
                //if (canceled.load() || done_consuming.load()) return;
            }
            //rows_produced += BATCH_ROWS;
            //++batches_produced;
            write_batch = std::vector<QString>();
            write_batch.reserve(WRITE_BATCH_ROWS);
			write_batch.push_back(row);
        }
        else {
            write_batch.push_back(row);
        }
    }

    void SelectStatement::flush_write_queue()
    {
        if (!write_batch.empty()) {
            // After reading BATCH_ROWS items
            std::reverse(write_batch.begin(), write_batch.end()); // FIFO
            while (!write_queue->push(write_batch)) {
                std::this_thread::yield();
                //if (canceled.load() || done_consuming.load()) return;
            }
            //rows_produced += BATCH_ROWS;
            //++batches_produced;
            write_batch = std::vector<QString>();
		}
    }

    void SelectStatement::csv_file_reader()
    {
        csv::CSVRow row;
        std::vector<csv::CSVRow> batch;
        batch.reserve(BATCH_ROWS);

        while (left_file2->readRow(row)) {
            if (canceled.load() || done_consuming.load(std::memory_order_acquire))
                break;

            batch.push_back(std::move(row));
            //++rows_produced;
            row = csv::CSVRow();

            /*
            while (paused.load()) {
                if (canceled.load() || done_consuming.load()) return;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }*/

            {
                std::unique_lock<std::mutex> lock(mtx);
                // Wait while paused
                cv.wait(lock, [this] { return !paused.load() || canceled.load() || done_consuming.load(); });

                if (canceled.load() || done_consuming.load()) return;
            }

            if (batch.size() >= BATCH_ROWS) {

                // After reading BATCH_ROWS items
                std::reverse(batch.begin(), batch.end()); // FIFO

                while (!queue->push(batch)) {
                    std::this_thread::yield();
                    if (canceled.load() || done_consuming.load()) return;
                }
                rows_produced += BATCH_ROWS;
                ++batches_produced;
                batch = std::vector<csv::CSVRow>();
                batch.reserve(BATCH_ROWS);
            }
        }

        if (done_consuming.load()) {
            return;
        }

        if (!batch.empty() && !canceled.load() && !done_consuming.load()) {

            // After reading BATCH_ROWS items
            std::reverse(batch.begin(), batch.end()); // FIFO

            while (!queue->push(batch)) {
                std::this_thread::yield();
                if (canceled.load() || done_consuming.load()) return;
            }
            //queue->push(std::move(batch));
            rows_produced += batch.size();
        }

        done_producing.store(true, std::memory_order_release);
    }

    std::optional<QList<QStringList>> SelectStatement::select_with_no_join()
    {
        result = QList<QStringList>();
        result->reserve(NUMBER_OF_ROWS_PER_PAGE);

        if (limit_done) {
            return std::nullopt; //prevent select being run again because of pagination code in main.cpp
        }

        if (!has_group_by && !is_aggregation && has_having_clause) {
            QString error = "Invalid Having clause in a non-aggregate Select statement!";
            throw std::logic_error(error.toStdString());
        }

        // validate columns in select with group by or aggregate function
        if (has_group_by || is_aggregation) {
            group_by_result.reserve(BATCH_ROWS);
            validate_aggregate_query();
        }

        //QStringList r;
        //r.reserve(10);
        bool stop = false;

        //////////qDebug()() << "select called";
        //////////qDebug()() << "queue size: " << queue->size();

        std::vector<csv::CSVRow> batch;

        for (;;) {
            if (canceled.load()) break;

            if (!reserve_batch.empty()) {
                batch = std::move(reserve_batch);
                reserve_batch = std::vector<csv::CSVRow>();
            }
            else if (!queue->pop(batch)) {
                if (done_producing.load(std::memory_order_acquire) || done_consuming.load())
                    break; // Nothing more will come
                std::this_thread::yield();
                continue;
            }

            while (!batch.empty()) {
                csv::CSVRow csvrow = batch.back(); //batch.takeFirst(); 
                batch.pop_back(); //delete last element; vector reversed to have FIFO

                QStringList row;
                for (auto& field : csvrow)
                    row.append(QString::fromStdString(field.get<std::string>()));

                if (row.count() == 1 && row.at(0).trimmed().isEmpty()) {
                    //++skipped_rows;
                    continue;
                }

                QMap<QString, QStringList> data_rows;
                data_rows["$"] = row;

                stop = process_data(data_rows);
                ++rows_consumed;

                if (stop) {
                    //////qDebug()() << "Stop requested";
                    if (paused.load()) {
                        reserve_batch = std::move(batch);
                    }
                    break;
                }
            }
            //reserve_batch = batch;

            if (stop) {
                break;
            }
        }

        if (!has_group_by && !is_aggregation)
            done_consuming.store(true, std::memory_order_release);

        //try {
            
        //}
        //catch (std::exception& e) {
            ////qDebug()() << "Exception: "<< e.what();
        //}
        //catch (...) {
            ////qDebug()() << "Exception!";
        //}

        //////qDebug()() << "query done.";
        //file_reader_thread->join();
        //////////qDebug()() << "file_reader_thread done.";
        //////////qDebug()() << "result size: " << result.value_or({}).count();

        // process aggregation result
        if (has_group_by || is_aggregation) {
            //////qDebug()() << "Processing group by result for display...";
            //////////qDebug()() << "Number of rows in group result: "<<NUMBER_OF_ROWS;
            group_by_result_loc = (paginate) ? group_by_result_loc : group_by_result.begin();

            if(group_by_result_loc == group_by_result.end()){
                //////////qDebug()() << "At group by result end";
                return std::nullopt; //end pagination; all rows shown
            }

            for (; group_by_result_loc != group_by_result.end(); ++group_by_result_loc) {
                //foreach(auto row, group_by_result) {
                QStringList& row = group_by_result_loc.value();

                QMap<QString, QStringList> data_rows;
                data_rows["$"] = row;

                if (has_limit_clause && LIMIT_VAL == NUMBER_OF_ROWS) { // handle limit clause
                    limit_done = true;
                    break;
                }

                if (has_having_clause) {
                    Term t; // = having_conditional_expr->eval(data_rows);
                    //////////qDebug()()<<"Having clause...";

                    
                    if (!is_having_conditional_compiled) {
                        compiled_having_conditional_func = having_conditional_expr->compile(data_rows);
                        t = compiled_having_conditional_func(data_rows);
                        is_having_conditional_compiled = true;
                    }
                    else {
                        t = compiled_having_conditional_func(data_rows);
                    }
                    
                    //////////qDebug()() << "having clause is " << t.get_token().boolean_value <<" for: "<< row;

                    if (t.get_token().boolean_value == true) {
                        if (write_to_file) {
                            //out_file->writeLine(row.join(','));
                            push_to_write_queue(row.join(','));
                            ++NUMBER_OF_ROWS;
                            //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                            //NUMBER_OF_ROWS.fetch_add(1);
                        }
                        else {
                            result->append(row);
                            ++NUMBER_OF_ROWS;
                            //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                            //NUMBER_OF_ROWS.fetch_add(1);

                            if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate

                                if (NUMBER_OF_ROWS == 0) {
                                    paginate = true;
                                    return std::nullopt;
                                }
                                paginate = true;
                                return result;
                            }
                        }
                    }
                }
                else {
                    if (write_to_file) {
                        //out_file->writeLine(row.join(','));
                        push_to_write_queue(row.join(','));
                        ++NUMBER_OF_ROWS;
                        //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                        //NUMBER_OF_ROWS.fetch_add(1);
                    }
                    else {
                        result->append(row);
                        ++NUMBER_OF_ROWS;
                        //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                        //NUMBER_OF_ROWS.fetch_add(1);

                        if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate

                            if (NUMBER_OF_ROWS == 0) {
                                paginate = true;
                                return std::nullopt;
                            }
                            paginate = true;
                            return result;
                        }
                    }
                }
            }
        }
        
        if (has_group_by || is_aggregation) { //signal reader and writer that aggregation query is done
            done_consuming.store(true, std::memory_order_release);
        }


        if (!paginate) {
            group_by_result.clear();
        }
        
        //////qDebug()() << "result size:" << result->size();

        if (write_to_file) {
            return std::nullopt;
        }

        if (result->isEmpty()) {
            return std::nullopt;
        }



        return result;
    }

    std::shared_ptr<QHash<QString, QList<qint64>> > SelectStatement::build_index(const std::shared_ptr<CSVFile>& rhs, const int& column_index)
    {
        std::shared_ptr<QHash<QString, QList<qint64>> > index = std::make_shared<QHash<QString, QList<qint64>> >();

        while (!rhs->end_of_file()) {
            qint64 stream_pos = rhs->get_pos();
            QStringList row = rhs->readRow();

            (*index)[row[column_index]].append(stream_pos);
        }

        return index;
    }

    std::shared_ptr<QHash<QString, QList<QStringList>> > SelectStatement::build_index2(const std::shared_ptr<CSVFile2>& rhs, const int& column_index)
    {
        auto index = std::make_shared<QHash<QString, QList<QStringList>>>();
        csv::CSVRow csvrow;

        //unsigned int rhs_rows_read = 0;
        //unsigned int failed_read_count = 0;

        while (rhs->readRow(csvrow)) {
            QStringList row;
            row.reserve(csvrow.size());

            for (auto& field : csvrow)
                row.append(QString::fromStdString(field.get<std::string>()));

            if (row.size() == 1 && row.at(0).trimmed().isEmpty())
                continue;

            //++rhs_rows_read;

            if (column_index < row.size())
                (*index)[row[column_index]].append(row);
            //else
                //++failed_read_count;

            number_of_columns_for_rhs_csv = row.size();
        }

        ////qDebug()() << "RHS File rows read:" << rhs_rows_read;
        ////qDebug()() << "Failed read count:" << failed_read_count;

        return index;
    }

    void SelectStatement::process_select(QHash<QString, QStringList>& group_by_result, QMap<QString, QStringList>& data_rows)
    {
        QStringList columns;
        if (has_where_clause) {
            Term t = conditional_expr->eval(data_rows);

            if (t.get_token().boolean_value == false) {
                return;
            }

            QString result_key = create_group_by_key(data_rows);
            aggregate_expression_reg_key = result_key;
            columns = compute_columns(data_rows);
            group_by_result[result_key] = columns;

            //////////qDebug()() << "group by result:" << columns;

            //write to file?
            /*
            if (write_to_file) {
                out_file->writeLine(columns.join(','));
                ++NUMBER_OF_ROWS;
            }
            else {
                result.append(columns);
                ++NUMBER_OF_ROWS;
            }
            */
        }
        else { //no where clause
            QString result_key = create_group_by_key(data_rows);
            aggregate_expression_reg_key = result_key;
            columns = compute_columns(data_rows);
            group_by_result[result_key] = columns;
            //////////qDebug()() << "group by result:" <<columns;

            //write to file?
            /*
            if (write_to_file) {
                out_file->writeLine(columns.join(','));
                ++NUMBER_OF_ROWS;
            }
            else {
                result.append(columns);
                ++NUMBER_OF_ROWS;
            }
            */
        }
    }


    void SelectStatement::process_select(QList<QStringList>& result, QMap<QString, QStringList>& data_rows)
    {
        QStringList columns;
        if (has_where_clause) {
            Term t = conditional_expr->eval(data_rows);

            if (t.get_token().boolean_value == false) {
                return;
            }

            columns = compute_columns(data_rows);

            //write to file?
            if (write_to_file) {
                out_file->writeLine(columns.join(','));
                ++NUMBER_OF_ROWS;
            }
            else {
                result.append(columns);
                ++NUMBER_OF_ROWS;
            }
        }
        else { //no where clause
            columns = compute_columns(data_rows);

            //write to file?
            if (write_to_file) {
                out_file->writeLine(columns.join(','));
                ++NUMBER_OF_ROWS;
            }
            else {
                result.append(columns);
                ++NUMBER_OF_ROWS;
            }
        }

    }


    std::optional<QList<QStringList>> SelectStatement::execute()
    {
        std::optional<QList<QStringList>> result = std::nullopt;

        if (has_from_clause == false) {
            if (run_only_once) {
                return std::nullopt;
            }

            QMap<QString, QStringList> data_rows;
            QList<QStringList> res;
            data_rows["$"] = { "" };
            auto comp_f = compile_columns(data_rows);
            res.append(comp_f(data_rows));
            //res.append(compute_columns(data_rows));
            result = res;
            ++NUMBER_OF_ROWS;

            run_only_once = true;
            return result;
        }

        if (!has_join) { //read from single file; no join
            return select_with_no_join();
        }
        else { // has a join
            if (join_type == TokenType::INNERJOIN) {
                return select_with_inner_join2();
            }
            else if (join_type == TokenType::OUTERJOIN) {
                return select_with_outer_join2();
            }
            else if (join_type == TokenType::CROSSJOIN) {

            }

        }

        return result;
    }


    std::optional<QList<QStringList>> SelectStatement::select_with_inner_join2()
    {
        ////qDebug()() << "inner join called";
        result = QList<QStringList>();
        result->reserve(NUMBER_OF_ROWS_PER_PAGE);

        if (limit_done) {
            return std::nullopt; //prevent select being run again because of pagination code in main.cpp
        }

        if (!has_group_by && !is_aggregation && has_having_clause) {
            QString error = "Invalid Having clause in a non-aggregate Select statement!";
            throw std::logic_error(error.toStdString());
        }

        // validate columns in select with group by or aggregate function
        if (has_group_by || is_aggregation) {
            group_by_result.reserve(BATCH_ROWS);
            validate_aggregate_query();
        }

        // build index
        if (!is_indexing_done) {
			//qDebug() << "Building index for join right file "<<this->right_file2->get_file_name()<<" index:"<<this->query_index;
            query_lookup_index2 = build_index2(this->right_file2, this->query_index);
            ////qDebug()() << "indexing done.";
			//qDebug() << "Index built. Number of keys in index: " << *query_lookup_index2;
            is_indexing_done = true;
        }

        //QStringList r;
        //r.reserve(10);
        bool stop = false;

        //////////qDebug()() << "select called";
        //////////qDebug()() << "queue size: " << queue->size();

        //process any outstanding rows in index
        if (!matching_rhs_rows.empty()) {
            ////qDebug()() << "Processing outstanding rows ("<<matching_rhs_rows.size()<<") ...";

            QMap<QString, QStringList> data_rows = outstanding_data_rows; //retrieve saved data_rows before pause

            while (!matching_rhs_rows.isEmpty()) {
                QStringList rhs_row = matching_rhs_rows.front();
                matching_rhs_rows.pop_front();

                data_rows[join_files_list["right"]] = rhs_row;

                stop = process_data(data_rows);

                if (stop) {
                    //////qDebug()() << "Stop requested";
                    if (paused.load()) {
                        join_paginate = true;
                        //reserve_batch = std::move(batch);
                        outstanding_data_rows = data_rows; // saved data_rows
                        //join_paginate = true;
                    }
                    else {
                        done_consuming.store(true, std::memory_order_release);
                        return result;
                    }
                    break;
                }

            }
        }

        try {
            std::vector<csv::CSVRow> batch;
            ////qDebug()() << "starting processing...";

            unsigned int common_rows_found = 0;

            for (;;) {
                if (canceled.load()) break;

                if (!reserve_batch.empty()) {
                    batch = std::move(reserve_batch);
                    reserve_batch = std::vector<csv::CSVRow>();
                }
                else if (!queue->pop(batch)) {
                    if (done_producing.load(std::memory_order_acquire))
                        break; // Nothing more will come
                    std::this_thread::yield();
                    continue;
                }

                while (!batch.empty()) {
                    csv::CSVRow csvrow = batch.back(); //batch.takeFirst(); 
                    batch.pop_back(); //delete last element; vector reversed to have FIFO

                    QStringList row;
                    for (auto& field : csvrow)
                        row.append(QString::fromStdString(field.get<std::string>()));

                    if (row.count() == 1 && row.at(0).trimmed().isEmpty()) {
                        ++skipped_rows;
                        continue;
                    }

                    QMap<QString, QStringList> data_rows;
                    data_rows[join_files_list["left"]] = row;

                    //QList<qint64> indices = (*query_lookup_index)[row[query_index]];
                    //qDebug() << "\nindex: " << query_lookup_index2->keys() << "\n";
					//qDebug() << "\nLooking for join key: " << row[left_query_index] <<" in index?: "<< query_lookup_index2->contains(row[left_query_index]);

                    //is join key present in rhs file?
                    if (query_lookup_index2->contains(row[left_query_index])) {
                        matching_rhs_rows = (*query_lookup_index2)[row[left_query_index]];

                        //data_rows[join_files_list["right"]] = row;
                        while (!matching_rhs_rows.isEmpty()) {
                            QStringList rhs_row = matching_rhs_rows.front();
                            matching_rhs_rows.pop_front();

                            data_rows[join_files_list["right"]] = rhs_row;
							//qDebug() << "Joining rows: \n" << data_rows[join_files_list["left"]] << "\n AND \n" << data_rows[join_files_list["right"]];
                            stop = process_data(data_rows);

                            if (stop) {
                                //////qDebug()() << "Stop requested";
                                if (paused.load()) {
                                    join_paginate = true;
                                    reserve_batch = std::move(batch);
                                    outstanding_data_rows = data_rows; // saved data_rows
                                    join_paginate = true;
                                    //paginate = true;
                                }
                                else {
                                    done_consuming.store(true, std::memory_order_release);
                                }
                                break;
                            }

                        }
                        ////qDebug()() << "Outstanding rows (" << matching_rhs_rows.size() << ") ...";
                        

                        if (stop) { //break from outer while loop
                            //join_paginate = false;
                            break;
                        }


                    }

                    //stop = process_data(data_rows);
                    //++rows_consumed;

                    
                }
                //reserve_batch = batch;

                if (stop) { //break from for loop
                    break;
                }
            }

            ////qDebug()() << "common rows found: " << common_rows_found;

            if (!has_group_by && !is_aggregation)
                done_consuming.store(true, std::memory_order_release);
        }
        catch (std::exception& e) {
            std::cerr << "Exception: " << e.what();
        }
        catch (...) {
            std::cerr << "Exception!";
        }

        //////qDebug()() << "query done.";
        //file_reader_thread->join();
        //////////qDebug()() << "file_reader_thread done.";
        //////////qDebug()() << "result size: " << result.value_or({}).count();

        // process aggregation result
        if (has_group_by || is_aggregation) {
            //////qDebug()() << "Processing group by result for display...";
            //////////qDebug()() << "Number of rows in group result: "<<NUMBER_OF_ROWS;
            group_by_result_loc = (paginate) ? group_by_result_loc : group_by_result.begin();

            if (group_by_result_loc == group_by_result.end()) {
                //////////qDebug()() << "At group by result end";
                return std::nullopt; //end pagination; all rows shown
            }

            for (; group_by_result_loc != group_by_result.end(); ++group_by_result_loc) {
                //foreach(auto row, group_by_result) {
                QStringList& row = group_by_result_loc.value();

                QMap<QString, QStringList> data_rows;
                data_rows["$"] = row;

                if (has_limit_clause && LIMIT_VAL == NUMBER_OF_ROWS) { // handle limit clause
                    limit_done = true;
                    break;
                }

                if (has_having_clause) {
                    Term t; // = having_conditional_expr->eval(data_rows);
                    //////////qDebug()()<<"Having clause...";


                    if (!is_having_conditional_compiled) {
                        compiled_having_conditional_func = having_conditional_expr->compile(data_rows);
                        t = compiled_having_conditional_func(data_rows);
                        is_having_conditional_compiled = true;
                    }
                    else {
                        t = compiled_having_conditional_func(data_rows);
                    }

                    //////////qDebug()() << "having clause is " << t.get_token().boolean_value <<" for: "<< row;

                    if (t.get_token().boolean_value == true) {
                        if (write_to_file) {
                            //out_file->writeLine(row.join(','));
                            push_to_write_queue(row.join(','));
                            ++NUMBER_OF_ROWS;
                            //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                            //NUMBER_OF_ROWS.fetch_add(1);
                        }
                        else {
                            result->append(row);
                            ++NUMBER_OF_ROWS;
                            //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                            //NUMBER_OF_ROWS.fetch_add(1);

                            if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate

                                if (NUMBER_OF_ROWS == 0) {
                                    paginate = true;
                                    return std::nullopt;
                                }
                                paginate = true;
                                return result;
                            }
                        }
                    }
                }
                else {
                    if (write_to_file) {
                        //out_file->writeLine(row.join(','));
                        push_to_write_queue(row.join(','));
                        ++NUMBER_OF_ROWS;
                        //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                        //NUMBER_OF_ROWS.fetch_add(1);
                    }
                    else {
                        result->append(row);
                        ++NUMBER_OF_ROWS;
                        //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                        //NUMBER_OF_ROWS.fetch_add(1);

                        if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate

                            if (NUMBER_OF_ROWS == 0) {
                                paginate = true;
                                return std::nullopt;
                            }
                            paginate = true;
                            return result;
                        }
                    }
                }
            }
        }

        if (has_group_by || is_aggregation) { //signal reader and writer that aggregation query is done
            done_consuming.store(true, std::memory_order_release);
        }


        if (!paginate) {
            group_by_result.clear();
        }

        //////qDebug()() << "result size:" << result->size();

        if (write_to_file) {
            return std::nullopt;
        }

        if (result->isEmpty()) {
            return std::nullopt;
        }



        return result;
    }

    std::optional<QList<QStringList>> SelectStatement::select_with_outer_join2()
    {
        result = QList<QStringList>();
        result->reserve(NUMBER_OF_ROWS_PER_PAGE);

        if (limit_done) {
            return std::nullopt; //prevent select being run again because of pagination code in main.cpp
        }

        if (!has_group_by && !is_aggregation && has_having_clause) {
            QString error = "Invalid Having clause in a non-aggregate Select statement!";
            throw std::logic_error(error.toStdString());
        }

        // validate columns in select with group by or aggregate function
        if (has_group_by || is_aggregation) {
            group_by_result.reserve(BATCH_ROWS);
            validate_aggregate_query();
        }

        // build index
        if (!is_indexing_done) {

            query_lookup_index2 = build_index2(this->right_file2, this->query_index);
            ////qDebug()() << "indexing done.";
            is_indexing_done = true;
        }

        //QStringList r;
        //r.reserve(10);
        bool stop = false;

        //////////qDebug()() << "select called";
        //////////qDebug()() << "queue size: " << queue->size();

        //process any outstanding rows in index
        if (!matching_rhs_rows.empty()) {
            ////qDebug()() << "Processing outstanding rows ("<<matching_rhs_rows.size()<<") ...";

            QMap<QString, QStringList> data_rows = outstanding_data_rows; //retrieve saved data_rows before pause

            while (!matching_rhs_rows.isEmpty()) {
                QStringList rhs_row = matching_rhs_rows.front();
                matching_rhs_rows.pop_front();

                data_rows[join_files_list["right"]] = rhs_row;

                stop = process_data(data_rows);

                if (stop) {
                    //////qDebug()() << "Stop requested";
                    if (paused.load()) {
                        join_paginate = true;
                        //reserve_batch = std::move(batch);
                        outstanding_data_rows = data_rows; // saved data_rows
                        //join_paginate = true;
                    }
                    else {
                        done_consuming.store(true, std::memory_order_release);
                        return result;
                    }
                    break;
                }

            }
        }

        try {
            std::vector<csv::CSVRow> batch;
            ////qDebug()() << "starting processing...";

            unsigned int common_rows_found = 0;

            for (;;) {
                if (canceled.load()) break;

                if (!reserve_batch.empty()) {
                    batch = std::move(reserve_batch);
                    reserve_batch = std::vector<csv::CSVRow>();
                }
                else if (!queue->pop(batch)) {
                    if (done_producing.load(std::memory_order_acquire))
                        break; // Nothing more will come
                    std::this_thread::yield();
                    continue;
                }

                while (!batch.empty()) {
                    csv::CSVRow csvrow = batch.back(); //batch.takeFirst(); 
                    batch.pop_back(); //delete last element; vector reversed to have FIFO

                    QStringList row;
                    for (auto& field : csvrow)
                        row.append(QString::fromStdString(field.get<std::string>()));

                    if (row.count() == 1 && row.at(0).trimmed().isEmpty()) {
                        ++skipped_rows;
                        continue;
                    }

                    QMap<QString, QStringList> data_rows;
                    data_rows[join_files_list["left"]] = row;

                    //QList<qint64> indices = (*query_lookup_index)[row[query_index]];

                    //is join key present in rhs file?
                    if (query_lookup_index2->contains(row[left_query_index])) {
                        matching_rhs_rows = (*query_lookup_index2)[row[left_query_index]];

                        //data_rows[join_files_list["right"]] = row;
                        while (!matching_rhs_rows.isEmpty()) {
                            QStringList rhs_row = matching_rhs_rows.front();
                            matching_rhs_rows.pop_front();

                            data_rows[join_files_list["right"]] = rhs_row;

                            stop = process_data(data_rows);

                            if (stop) {
                                //////qDebug()() << "Stop requested";
                                if (paused.load()) {
                                    join_paginate = true;
                                    reserve_batch = std::move(batch);
                                    outstanding_data_rows = data_rows; // saved data_rows
                                    join_paginate = true;
                                    //paginate = true;
                                }
                                else {
                                    done_consuming.store(true, std::memory_order_release);
                                }
                                break;
                            }

                        }
                    
                        ////qDebug()() << "Outstanding rows (" << matching_rhs_rows.size() << ") ...";


                        if (stop) { //break from outer while loop
                            //join_paginate = false;
                            break;
                        }


                    }
                    else { //lhs not in rhs, make nulls

                        QStringList rhs_row;

                        for (int i = 0; i < number_of_columns_for_rhs_csv; ++i) {
                            rhs_row.append("");
                        }

                        data_rows[join_files_list["right"]] = rhs_row;

                        stop = process_data(data_rows);

                        if (stop) {

                            if (paused.load()) {

                                reserve_batch = std::move(batch);
                                outstanding_data_rows = data_rows; // saved data_rows

                            }
                            else {
                                done_consuming.store(true, std::memory_order_release);
                            }
                            break;
                        }
                    }
                }

                if (stop) { //break from for loop
                    break;
                }
            }

            ////qDebug()() << "common rows found: " << common_rows_found;

            if (!has_group_by && !is_aggregation)
                done_consuming.store(true, std::memory_order_release);
        }
        catch (std::exception& e) {
            std::cerr << "Exception: " << e.what();
        }
        catch (...) {
            std::cerr << "Exception!";
        }

        //////qDebug()() << "query done.";
        //file_reader_thread->join();
        //////////qDebug()() << "file_reader_thread done.";
        //////////qDebug()() << "result size: " << result.value_or({}).count();

        // process aggregation result
        if (has_group_by || is_aggregation) {
            //////qDebug()() << "Processing group by result for display...";
            //////////qDebug()() << "Number of rows in group result: "<<NUMBER_OF_ROWS;
            group_by_result_loc = (paginate) ? group_by_result_loc : group_by_result.begin();

            if (group_by_result_loc == group_by_result.end()) {
                //////////qDebug()() << "At group by result end";
                return std::nullopt; //end pagination; all rows shown
            }

            for (; group_by_result_loc != group_by_result.end(); ++group_by_result_loc) {
                //foreach(auto row, group_by_result) {
                QStringList& row = group_by_result_loc.value();

                QMap<QString, QStringList> data_rows;
                data_rows["$"] = row;

                if (has_limit_clause && LIMIT_VAL == NUMBER_OF_ROWS) { // handle limit clause
                    limit_done = true;
                    break;
                }

                if (has_having_clause) {
                    Term t; // = having_conditional_expr->eval(data_rows);
                    //////////qDebug()()<<"Having clause...";


                    if (!is_having_conditional_compiled) {
                        compiled_having_conditional_func = having_conditional_expr->compile(data_rows);
                        t = compiled_having_conditional_func(data_rows);
                        is_having_conditional_compiled = true;
                    }
                    else {
                        t = compiled_having_conditional_func(data_rows);
                    }

                    //////////qDebug()() << "having clause is " << t.get_token().boolean_value <<" for: "<< row;

                    if (t.get_token().boolean_value == true) {
                        if (write_to_file) {
                            //out_file->writeLine(row.join(','));
                            push_to_write_queue(row.join(','));
                            ++NUMBER_OF_ROWS;
                            //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                            //NUMBER_OF_ROWS.fetch_add(1);
                        }
                        else {
                            result->append(row);
                            ++NUMBER_OF_ROWS;
                            //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                            //NUMBER_OF_ROWS.fetch_add(1);

                            if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate

                                if (NUMBER_OF_ROWS == 0) {
                                    paginate = true;
                                    return std::nullopt;
                                }
                                paginate = true;
                                return result;
                            }
                        }
                    }
                }
                else {
                    if (write_to_file) {
                        //out_file->writeLine(row.join(','));
                        push_to_write_queue(row.join(','));
                        ++NUMBER_OF_ROWS;
                        //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                        //NUMBER_OF_ROWS.fetch_add(1);
                    }
                    else {
                        result->append(row);
                        ++NUMBER_OF_ROWS;
                        //++NUMBER_OF_ROWS_IN_CSV; // used for count(*)
                        //NUMBER_OF_ROWS.fetch_add(1);

                        if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate

                            if (NUMBER_OF_ROWS == 0) {
                                paginate = true;
                                return std::nullopt;
                            }
                            paginate = true;
                            return result;
                        }
                    }
                }
            }
        }

        if (has_group_by || is_aggregation) { //signal reader and writer that aggregation query is done
            done_consuming.store(true, std::memory_order_release);
        }


        if (!paginate) {
            group_by_result.clear();
        }

        //////qDebug()() << "result size:" << result->size();

        if (write_to_file) {
            return std::nullopt;
        }

        if (result->isEmpty()) {
            return std::nullopt;
        }



        return result;
    }

}