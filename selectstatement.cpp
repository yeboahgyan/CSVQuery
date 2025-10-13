#include "selectstatement.h"
#include <stdexcept>
#include <QFileInfo>


namespace csvquery {

    SelectStatement::SelectStatement(const QList<Token>& tks, unsigned int max_rows_per_page)
        :
        tokens{ tks }
        , NUMBER_OF_ROWS_PER_PAGE{ max_rows_per_page }
    {
        last_token_pos = tokens.cbegin();

        optional_actions[TokenType::CROSSJOIN] = [this]() {handle_cross_join(); };
        optional_actions[TokenType::INNERJOIN] = [this]() {handle_inner_join(); };
        optional_actions[TokenType::OUTERJOIN] = [this]() {handle_outer_join(); };
        optional_actions[TokenType::INTO] = [this]() {handle_into_clause(); };
        optional_actions[TokenType::WHERE] = [this]() {handle_where_clause(); };
        optional_actions[TokenType::GROUPBY] = [this]() {handle_groupby_clause(); };
        //qDebug()<<"constructing select statement...";
        parse();
        //qDebug()<<"Done.";

        //foreach(auto t, tokens) {
        //    qDebug() << t.to_string();
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
                || (last_token_pos->token_type == TokenType::INTO) || (last_token_pos->token_type == TokenType::GROUPBY))
            {
                break;
            }

            Term t(*last_token_pos);
            //qDebug()<<"where term: "<<last_token_pos->to_string();
            cond_terms.append(t);
        }

        return std::make_shared<ConditionalExpression>(cond_terms);
    }


    std::shared_ptr<CSVFile> SelectStatement::read_file(QIODeviceBase::OpenMode m)
    {

        throw_exception_if_unexpected_end();

        QString f;
        if ((last_token_pos->token_type == TokenType::NAME) || (last_token_pos->token_type == TokenType::STRING)) {
            if (last_token_pos->token_type == TokenType::NAME) { // name
                if (!symbol_table.contains(last_token_pos->string_value.toLower())) {
                    double line_numer = (*last_token_pos).line_number;
                    QString str_num = QString::number(line_numer);

                    throw std::logic_error("Unknown name on line " + str_num.toStdString());
                }

                TokenType token_type = symbol_table[last_token_pos->string_value.toLower()];
                if (token_type != TokenType::STRING) {
                    double line_numer = (*last_token_pos).line_number;
                    QString str_num = QString::number(line_numer);

                    throw std::logic_error("Invalid name on line " + str_num.toStdString());
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

        CSVFile csv(f, m);
        csv.set_token(*last_token_pos);

        return std::make_shared<CSVFile>(csv);
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
                std::string error = "Column name does not refer to either files in the join statement on line ";
                error += QString::number(last_token_pos->line_number).toStdString();
                throw std::logic_error(error);
            }

            if (symbol_table[file_alias] != TokenType::STRING) {
                std::string error = "Column name does not refer to either files in the join statement on line ";
                error += QString::number(last_token_pos->line_number).toStdString();
                throw std::logic_error(error);
            }

            QString file_path = strings_table[file_alias];
            if (!join_files_list.values().contains(file_path)) {
                std::string error = "Column name does not refer to either files in the join statement on line ";
                error += QString::number(last_token_pos->line_number).toStdString();
                throw std::logic_error(error);
            }
        }


        if (last_token_pos->token_type == TokenType::NAME) {


            QString filename1 = left_t.get_token().string_value.split('.')[0];
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
            QString filename1 = left_t.get_token().string_value.split(',')[0];
            if (filename1 == join_files_list["right"]) {
                if (columns_table.contains(left_t.get_token().string_value.toLower())) {
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

        return left_t;

    }

    std::shared_ptr<ConditionalExpression> SelectStatement::read_on_clause()
    {
        throw_exception_if_unexpected_end();

        QList<Term> terms;

        //qDebug()<<"joined files: "<<join_files_list;

        if (last_token_pos->token_type != TokenType::ON) {
            //error
            std::string error = "1. Invalid syntax on line ";
            error += QString::number(last_token_pos->line_number).toStdString();
            error += " token seen ";
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



    void SelectStatement::handle_into_clause()
    {
        //qDebug() << "Handling INTO clause...";
        ++last_token_pos; // next token

        this->out_file = read_file(QIODevice::WriteOnly);
        this->write_to_file = true;

        QString of = out_file->get_token().string_value;
        foreach(auto file_name, join_files_list) {
            if (of == file_name) {
                std::string error = "Output cannot be the same as a file being read from in Select statement on line ";
                error += std::to_string(last_token_pos->line_number);
                throw std::logic_error(error);
            }
        }
        //qDebug() << "Done.";
    }

    void SelectStatement::handle_inner_join()
    {
        this->has_join = true;
        this->join_type = last_token_pos->token_type;

        ++last_token_pos; // next token

        this->right_file = read_file();
        join_files_list["right"] = right_file->get_token().string_value;

        ++last_token_pos; // next token
        this->on_clause = read_on_clause();

        ++last_token_pos; // next token

        if (last_token_pos == tokens.cend()) {
            return;
        }

        if (last_token_pos->token_type == TokenType::END || last_token_pos->token_type == TokenType::SEMICOLON) {
            return;
        }

        QList<TokenType> valid_next_tokens = { TokenType::INTO, TokenType::WHERE, TokenType::GROUPBY };
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

        this->right_file = read_file();
        join_files_list["right"] = right_file->get_token().string_value;

        ++last_token_pos; // next token
        this->on_clause = read_on_clause();

        ++last_token_pos; // next token

        if (last_token_pos == tokens.cend()) {
            return;
        }

        if (last_token_pos->token_type == TokenType::END || last_token_pos->token_type == TokenType::SEMICOLON) {
            return;
        }

        QList<TokenType> valid_next_tokens = { TokenType::INTO, TokenType::WHERE, TokenType::GROUPBY };
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

        this->right_file = read_file();
        join_files_list["right"] = right_file->get_token().string_value;

        ++last_token_pos; // next token

        if (last_token_pos == tokens.cend()) {
            return;
        }

        if (last_token_pos->token_type == TokenType::END || last_token_pos->token_type == TokenType::SEMICOLON) {
            return;
        }

        QList<TokenType> valid_next_tokens = { TokenType::INTO, TokenType::WHERE, TokenType::GROUPBY };
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
        //qDebug() << "----WHERE HANDLER CALLED!";
        this->has_where_clause = true;
        ++last_token_pos; // next token

        this->conditional_expr = read_where(); //clause clause

        if (last_token_pos == tokens.cend()) {
            return;
        }

        //++last_token_pos; // next token

        if (last_token_pos == tokens.cend()) {
            //qDebug() << "-------End of select found in where clause handler";
            return;
        }

        if (last_token_pos->token_type == TokenType::END || last_token_pos->token_type == TokenType::SEMICOLON) {
            return;
        }

        QList<TokenType> valid_next_tokens = { TokenType::GROUPBY, TokenType::INTO };
        if (!valid_next_tokens.contains(last_token_pos->token_type)) {
            //throw error
            std::string error = "Unexpected token (";
            error += last_token_pos->to_string().toStdString();
            error += ") on line ";
            error += QString::number(last_token_pos->line_number).toStdString();
        }
        //qDebug() << "Done with WHERE clause moving on to next clause...";
        //qDebug() << "Token after where clause is " << last_token_pos->to_string() << " with string value: " << last_token_pos->string_value;
        optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
    }

    void SelectStatement::handle_groupby_clause()
    {
        //qDebug() << "group by found!";

        has_group_by = true;

        ++last_token_pos; // next token

        throw_exception_if_unexpected_end();
        //qDebug() << "Here!" << last_token_pos->to_string();

        while (last_token_pos != tokens.cend()) {
            //qDebug() << "Here!" << last_token_pos->to_string();
            if (last_token_pos->token_type == TokenType::SEMICOLON || last_token_pos->token_type == TokenType::END || last_token_pos->token_type ==TokenType::INTO) {
                //qDebug() << "exit Here!" << last_token_pos->to_string();
                break;
            }
            //qDebug() << "token: " << last_token_pos->to_string();
            if (last_token_pos->token_type == TokenType::COLUMNNUMBER) {

                if (has_join) {
                    QString error = "Ambigious column ";
                    error += last_token_pos->string_value;
                    error += " on line ";
                    error += QString::number(last_token_pos->line_number);
                    throw std::logic_error(error.toStdString());
                }

                this->group_by_columns.append(last_token_pos->string_value.toLower());
                //qDebug() << "group by columns"<<group_by_columns;
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
                    QString error = "Unknown column in group by clause ";
                    error += last_token_pos->string_value;
                    error += " on line ";
                    error += QString::number(last_token_pos->line_number);
                    throw std::logic_error(error.toStdString());
                }
                
                QString file_name = strings_table[name_parts[0].toLower()];
                if (file_name != join_files_list["left"] && file_name != join_files_list["right"]) {
                    QString error = "Unknown column in group by clause ";
                    error += last_token_pos->string_value;
                    error += " on line ";
                    error += QString::number(last_token_pos->line_number);
                    throw std::logic_error(error.toStdString());
                }

                //check if second part of name is a number
                bool is_number = false;
                name_parts[1].toInt(&is_number);

                if (!is_number) {
                    QString error = "Unknown column in group by clause ";
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
        //qDebug() << "group by columns: " << group_by_columns;

        if (last_token_pos == tokens.cend()) {
            return;
        }

        if (last_token_pos->token_type == TokenType::END || last_token_pos->token_type == TokenType::SEMICOLON) {
            return;
        }

        QList<TokenType> valid_next_tokens = { TokenType::INTO };
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
        this->left_file = read_file();
        join_files_list["left"] = left_file->get_token().string_value;

        ++last_token_pos; // next token
        //qDebug()<<"1. token after left file is "<< last_token_pos->to_string() <<" {"<<last_token_pos->string_value<<"}";

        if ((last_token_pos == tokens.cend()) || (last_token_pos->token_type == TokenType::SEMICOLON)) {
            return;
        }

        //qDebug()<<"2. token after left file is "<< last_token_pos->to_string()<<" {"<<last_token_pos->string_value<<"}";
        // What is the next token?
        if (last_token_pos->token_type == TokenType::INNERJOIN) {
            optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
        }
        else if (last_token_pos->token_type == TokenType::OUTERJOIN) {
            optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
        }
        else if (last_token_pos->token_type == TokenType::CROSSJOIN) {
            optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
        }
        else if (last_token_pos->token_type == TokenType::WHERE) {
            optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
        }
        else if (last_token_pos->token_type == TokenType::GROUPBY) {
            optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
        }
        else if (last_token_pos->token_type == TokenType::INTO) {
            optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
        }
        else { // invalid token
            double line_numer = (*last_token_pos).line_number;
            QString str_num = QString::number(line_numer);

            throw std::logic_error("2. Unexpected end to SELECT statement on line " + str_num.toStdString());
        }
        //qDebug("parsing select done.");
    }



    QStringList SelectStatement::compute_columns(const QMap<QString, QStringList>& data_rows)
    {
        QStringList columns;
        foreach(auto e, column_exprs) {
            Term ct = e.eval(data_rows);
            QString column;
            if (ct.get_token().token_type == TokenType::STRING) {
                column = ct.get_token().string_value;
            }
            else if (ct.get_token().token_type == TokenType::NUMBER) {
                column = QString::number(ct.get_token().number_value);
            }
            columns.append(column);
        }
        return columns;
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
                //qDebug() << "creating group by key with column: "<< c;
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
                //qDebug() << "creating group by key with column (tm.eval(data_rows).string_value): " << tm.eval(data_rows).string_value;
                vals.append(tm.eval(data_rows).string_value); //get current value for group by column
            }
            key = vals.join(",");
            //qDebug() << "key is " << key;
        }


        return key;
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

    std::optional<QList<QStringList>> SelectStatement::select_with_no_join()
    {
        QList<QStringList> result;
        QHash<QString, QStringList> group_by_result;

        //qDebug()<<"Executing select with no join";

        //qDebug() << "group by list: " << group_by_columns;

        // validate columns in select with group by or aggregate function
        validate_aggregate_query();

        while (!left_file->end_of_file()) {
            QStringList row = left_file->readRow();

            QMap<QString, QStringList> data_rows;
            data_rows["$"] = row;

            QStringList columns;

            if (has_where_clause) {
                Term t = conditional_expr->eval(data_rows);
                //qDebug()<<"Conditional evaluation done.";

                if (t.get_token().boolean_value == true) {

                    //columns = compute_columns(data_rows);

                    if (has_group_by || is_aggregation) {
                        QString result_key = create_group_by_key(data_rows);
                        aggregate_expression_reg_key = result_key;

                        columns = compute_columns(data_rows);

                        group_by_result[result_key] = columns;
                    }
                    else {
                        columns = compute_columns(data_rows);
                        //write to file?
                        if (write_to_file) {
                            out_file->writeLine(columns.join(','));
                            ++NUMBER_OF_ROWS;

                            //if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                            //    return result;
                            //}
                        }
                        else {
                            result.append(columns);
                            ++NUMBER_OF_ROWS;

                            if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                                if (NUMBER_OF_ROWS == 0) {
                                    return std::nullopt;
                                }
                                return result;
                            }
                        }
                    }
                    
                }
            }
            else { //no where clause
                //columns = compute_columns(data_rows);

                if (has_group_by || is_aggregation) {
                    QString result_key = create_group_by_key(data_rows);
                    aggregate_expression_reg_key = result_key;

                    columns = compute_columns(data_rows);
                    group_by_result[result_key] = columns;
                }
                else {
                    columns = compute_columns(data_rows);

                    //write to file?
                    if (write_to_file) {
                        out_file->writeLine(columns.join(','));
                        ++NUMBER_OF_ROWS;

                        //if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                        //    return result;
                        //}
                    }
                    else {
                        result.append(columns);
                        ++NUMBER_OF_ROWS;

                        if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                            if (NUMBER_OF_ROWS == 0) {
                                return std::nullopt;
                            }
                            return result;
                        }
                    }
                }

            }

            columns = {}; //reset
            if (has_group_by || is_aggregation) {
                for (auto it = check_if_aggregate_done.begin(); it != check_if_aggregate_done.end(); ++it) {
                    //it.value().value() = false;
                    for (auto i = it.value().begin(); i != it.value().end(); ++i) {
                        i.value() = false;
                    }
                }
            }
            
        }

        if (has_group_by || is_aggregation) {
            foreach(auto row, group_by_result) {
                if (write_to_file) {
                    out_file->writeLine(row.join(','));
                    ++NUMBER_OF_ROWS;
                }
                else {
                    result.append(row);
                    ++NUMBER_OF_ROWS;

                    if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                        if (NUMBER_OF_ROWS == 0) {
                            return std::nullopt;
                        }
                        return result;
                    }
                }
            }
        }

        if (write_to_file) {
            return std::nullopt;
        }

        if (result.isEmpty()) {
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

    std::optional<QList<QStringList>> SelectStatement::select_with_inner_join()
    {
        QList<QStringList> result;
        QHash<QString, QStringList> group_by_result;


        if (left_file->end_of_file()) {
            return std::nullopt;
        }

        if (right_file->end_of_file()) {
            return std::nullopt;
        }


        // build index
        std::shared_ptr<QHash<QString, QList<qint64>> > query_lookup_index = std::make_shared<QHash<QString, QList<qint64>> >();
        query_lookup_index = build_index(this->right_file, this->query_index);

        //bool indexing_done = false;

        //loop over files
        while (!left_file->end_of_file()) {
            QStringList row = left_file->readRow();

            QMap<QString, QStringList> data_rows;
            data_rows[join_files_list["left"]] = row;

            QStringList columns;

            QList<qint64> indices = (*query_lookup_index)[row[query_index]];

            foreach(auto index, indices) {
                right_file->seek_to(index);
                QStringList row = right_file->readRow();

                data_rows[join_files_list["right"]] = row;

                /*
                Term on_term = on_clause->eval(data_rows);
                if(on_term.get_token().boolean_value == false){
                    continue;
                }*/

                if (has_group_by || is_aggregation) {
                    process_select(group_by_result, data_rows);
                }
                else {
                    process_select(result, data_rows);
                }

                

                if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                    if (NUMBER_OF_ROWS == 0) {
                        return std::nullopt;
                    }
                    return result;
                }
            }
            //columns = {}; //reset; is it necessary since it is redefined in the loop?
            if (has_group_by || is_aggregation) {
                for (auto it = check_if_aggregate_done.begin(); it != check_if_aggregate_done.end(); ++it) {
                    //it.value().value() = false;
                    for (auto i = it.value().begin(); i != it.value().end(); ++i) {
                        i.value() = false;
                    }
                }
            }
        }


        if (has_group_by || is_aggregation) {
            foreach(auto row, group_by_result) {
                if (write_to_file) {
                    out_file->writeLine(row.join(','));
                    ++NUMBER_OF_ROWS;
                }
                else {
                    result.append(row);
                    ++NUMBER_OF_ROWS;

                    if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                        if (NUMBER_OF_ROWS == 0) {
                            return std::nullopt;
                        }
                        return result;
                    }
                }
            }
        }

        if (write_to_file) {
            return std::nullopt;
        }

        if (result.isEmpty()) {
            return std::nullopt;
        }

        return result;
    }



    std::optional<QList<QStringList>> SelectStatement::select_with_outer_join()
    {
        QList<QStringList> result;
        QHash<QString, QStringList> group_by_result;

        //qDebug() << "Selecting with outer join";


        if (left_file->end_of_file()) {
            return std::nullopt;
        }

        if (right_file->end_of_file()) {
            return std::nullopt;
        }

        // build index
        std::shared_ptr<QHash<QString, QList<qint64>> > query_lookup_index = std::make_shared<QHash<QString, QList<qint64>> >();
        query_lookup_index = build_index(this->right_file, this->query_index);

        //bool indexing_done = false;

        //loop over files
        while (!left_file->end_of_file()) {
            QStringList row = left_file->readRow();

            QMap<QString, QStringList> data_rows;
            data_rows[join_files_list["left"]] = row;

            //QStringList columns;

            if (!query_lookup_index->contains(row[query_index])) { // key does not exist in right file

                for (int i = 0; i < row.size(); ++i) {
                    row[i] = ""; // make columns null
                }
                data_rows[join_files_list["right"]] = row;

                if (has_group_by || is_aggregation) {
                    process_select(group_by_result, data_rows);
                }
                else {
                    process_select(result, data_rows);
                }

                

                if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                    if (NUMBER_OF_ROWS == 0) {
                        return std::nullopt;
                    }
                    return result;
                }

                continue;
            }

            QList<qint64> indices = (*query_lookup_index)[row[query_index]];

            foreach(auto index, indices) {
                right_file->seek_to(index);
                QStringList row = right_file->readRow();

                data_rows[join_files_list["right"]] = row;

                /*
                Term on_term = on_clause->eval(data_rows);
                if(on_term.get_token().boolean_value == false){
                    continue;
                }*/

                process_select(result, data_rows);

                if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                    if (NUMBER_OF_ROWS == 0) {
                        return std::nullopt;
                    }
                    return result;
                }
            }
            //columns = {}; //reset; is it necessary since it is redefined in the loop?
            if (has_group_by || is_aggregation) {
                for (auto it = check_if_aggregate_done.begin(); it != check_if_aggregate_done.end(); ++it) {
                    //it.value().value() = false;
                    for (auto i = it.value().begin(); i != it.value().end(); ++i) {
                        i.value() = false;
                    }
                }
            }
        }

        if (has_group_by || is_aggregation) {
            foreach(auto row, group_by_result) {
                if (write_to_file) {
                    out_file->writeLine(row.join(','));
                    ++NUMBER_OF_ROWS;
                }
                else {
                    result.append(row);
                    ++NUMBER_OF_ROWS;

                    if ((write_to_file == false) && (NUMBER_OF_ROWS % NUMBER_OF_ROWS_PER_PAGE == 0)) { // paginate
                        if (NUMBER_OF_ROWS == 0) {
                            return std::nullopt;
                        }
                        return result;
                    }
                }
            }
        }

        if (write_to_file) {
            return std::nullopt;
        }

        if (result.isEmpty()) {
            return std::nullopt;
        }

        return result;
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
            res.append(compute_columns(data_rows));
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
                return select_with_inner_join();
            }
            else if (join_type == TokenType::OUTERJOIN) {
                return select_with_outer_join();
            }
            else if (join_type == TokenType::CROSSJOIN) {

            }

        }

        return result;
    }

}