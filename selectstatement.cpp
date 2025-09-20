#include "selectstatement.h"
#include <stdexcept>
#include <QFileInfo>


SelectStatement::SelectStatement(const QList<Token>& tks)
    :
    tokens{tks}
{
    last_token_pos = tokens.cbegin();

    optional_actions[TokenType::CROSSJOIN] = [this](){handle_cross_join();};
    optional_actions[TokenType::INNERJOIN] = [this](){handle_inner_join();};
    optional_actions[TokenType::OUTERJOIN] = [this](){handle_outer_join();};
    optional_actions[TokenType::INTO] = [this](){handle_into_clause();};
    optional_actions[TokenType::WHERE] = [this](){handle_where_clause();};
    optional_actions[TokenType::GROUPBY] = [this](){handle_groupby_clause();};

    parse();
}

void SelectStatement::throw_exception_if_unexpected_end()
{
    if(last_token_pos == tokens.cend()){
        --last_token_pos; //get last but one token
        double line_numer = (*last_token_pos).line_number;
        QString str_num = QString::number(line_numer);

        throw std::logic_error("Unexpected end to SELECT statement on line "+ str_num.toStdString());
    }
}

QList<Expression> SelectStatement::read_column_expressions()
{
    QList<Term> terms;
    QList<Expression> exps;

    for(; last_token_pos != tokens.cend(); ++last_token_pos){

        if((last_token_pos->token_type == TokenType::SEMICOLON) || (last_token_pos->token_type == TokenType::FROM)){
            Expression exp(terms);
            exps.append(exp);
            break;
        }

        if(last_token_pos->token_type == TokenType::COMMA){
            Expression exp(terms);
            exps.append(exp);
            terms = {};
        }
        else{

            Term t(*last_token_pos);
            terms.append(t);
        }
    }

    throw_exception_if_unexpected_end();

    if((last_token_pos->token_type != TokenType::SEMICOLON) || (last_token_pos->token_type != TokenType::FROM)){
        double line_numer = (*last_token_pos).line_number;
        QString str_num = QString::number(line_numer);

        throw std::logic_error("Unexpected end to column list on line "+ str_num.toStdString());
    }

    return exps;
}


std::shared_ptr<ConditionalExpression> SelectStatement::read_where()
{
    QList<Term> cond_terms;

    for(; last_token_pos != tokens.cend(); ++last_token_pos){
        if((last_token_pos->token_type == TokenType::SEMICOLON) || (last_token_pos->token_type == TokenType::END)){
            break;
        }

        Term t(*last_token_pos);
        cond_terms.append(t);
    }

    return std::make_shared<ConditionalExpression>(cond_terms);
}


 std::shared_ptr<CSVFile> SelectStatement::read_file(QIODeviceBase::OpenMode m)
{

    throw_exception_if_unexpected_end();

     QString f;
     if((last_token_pos->token_type == TokenType::NAME) || (last_token_pos->token_type == TokenType::STRING)){
         if(last_token_pos->token_type == TokenType::NAME){ // name
             if(!symbol_table.contains(last_token_pos->string_value.toLower())){
                 double line_numer = (*last_token_pos).line_number;
                 QString str_num = QString::number(line_numer);

                 throw std::logic_error("Unknown name on line "+ str_num.toStdString());
             }

             TokenType token_type = symbol_table[last_token_pos->string_value.toLower()];
             if(token_type != TokenType::STRING){
                 double line_numer = (*last_token_pos).line_number;
                 QString str_num = QString::number(line_numer);

                 throw std::logic_error("Invalid name on line "+ str_num.toStdString());
             }



             f = strings_table[last_token_pos->string_value.toLower()];
         }
         else{ //String
             f = last_token_pos->string_value.toLower();
         }

         // check if provided string is a valid file
         QFileInfo fileInfo(f);
         if( !fileInfo.exists() || !fileInfo.isFile()){
             double line_numer = (*last_token_pos).line_number;
             QString str_num = QString::number(line_numer);

             throw std::logic_error("Invalid file provided on line "+ str_num.toStdString());
         }

         //left_file = std::make_shared<CSVFile>(f);
     }

     CSVFile csv(f, m);
     csv.set_token(*last_token_pos);

     return std::make_shared<CSVFile>(csv);
}

std::shared_ptr<ConditionalExpression> SelectStatement::read_on_clause()
{
    throw_exception_if_unexpected_end();

    QList<Term> terms;

    if(last_token_pos->token_type != TokenType::ON){
        //error
    }

    ++last_token_pos; //next token
    throw_exception_if_unexpected_end();

    if(last_token_pos->token_type != TokenType::COLUMNNAME ){
        //error
        std::string error = "Expected a column in ON clause on line ";
        error += std::to_string(last_token_pos->line_number);
        throw std::logic_error(error);
    }


    Term left_t(*last_token_pos);

    QString column_index = left_t.get_token().string_value.split(',')[1];
    if(column_index == "*"){
        //error
        std::string error = "Unexpected column '*' in ON clause on line ";
        error += std::to_string(last_token_pos->line_number);
        throw std::logic_error(error);
    }

    terms.append(left_t);

    QString rhs_file_name = join_files_list["right"];
    QString filename1 = left_t.get_token().string_value.split(',')[0];
    if(filename1 == rhs_file_name){
        if(columns_table.contains(left_t.get_token().string_value.toLower())){
            this->query_index = columns_table[left_t.get_token().string_value.toLower()];
        }
        else{ // check if column is of the form file.0
            bool is_number;
            double number = column_index.toDouble(&is_number);
            if(is_number){
                this->query_index = number;
            }
            else{
                //error
                std::string error = "Invalid column in ON clause on line ";
                error += std::to_string(last_token_pos->line_number);
                throw std::logic_error(error);
            }
        }
    }

    //check if column belongs to the joined files
    QStringList column_name_parts = left_t.get_token().string_value.split('.');
    if(column_name_parts.size() != 2){
        std::string error = "Ambigious column name in ON clause on line ";
        error += std::to_string(last_token_pos->line_number);
        throw std::logic_error(error);
    }
    else{
        if(!join_files_list.values().contains(column_name_parts[0])){
            std::string error = "Column name does not refer to either files in the join statement on line ";
            error += std::to_string(last_token_pos->line_number);
            throw std::logic_error(error);
        }
    }

    ++last_token_pos; //next token
    throw_exception_if_unexpected_end();

    if(last_token_pos->token_type != TokenType::ASSIGN){
        //error
        std::string error = "Comparison operator in an ON clause should be '=' on line ";
        error += std::to_string(last_token_pos->line_number);
        throw std::logic_error(error);
    }

    Term op_t(*last_token_pos);
    terms.append(op_t);

    ++last_token_pos; //next token
    throw_exception_if_unexpected_end();

    if(last_token_pos->token_type != TokenType::COLUMNNAME ){
        //error
        std::string error = "Expected a column in ON clause on line ";
        error += std::to_string(last_token_pos->line_number);
        throw std::logic_error(error);
    }

    Term right_t(*last_token_pos);
    column_index = right_t.get_token().string_value.split(',')[1];
    if(column_index == "*"){
        //error
        std::string error = "Unexpected column '*' in ON clause on line ";
        error += std::to_string(last_token_pos->line_number);
        throw std::logic_error(error);
    }
    terms.append(right_t);

    QString filename2 = right_t.get_token().string_value.split(',')[0];
    if(filename2 == rhs_file_name){
        if(columns_table.contains(right_t.get_token().string_value.toLower())){
            this->query_index = columns_table[right_t.get_token().string_value.toLower()];
        }
        else{ // check if column is of the form file.0
            bool is_number;
            double number = column_index.toDouble(&is_number);
            if(is_number){
                this->query_index = number;
            }
            else{
                //error
                std::string error = "Invalid column in ON clause on line ";
                error += std::to_string(last_token_pos->line_number);
                throw std::logic_error(error);
            }
        }
    }

    //check if column belongs to the joined files
    QStringList column_name_parts2 = right_t.get_token().string_value.split('.');
    if(column_name_parts2.size() != 2){
        std::string error = "Ambigious column name in ON clause on line ";
        error += std::to_string(last_token_pos->line_number);
        throw std::logic_error(error);
    }
    else{
        if(!join_files_list.values().contains(column_name_parts2[0])){
            std::string error = "Column name does not refer to either files in the join statement on line ";
            error += std::to_string(last_token_pos->line_number);
            throw std::logic_error(error);
        }
    }

    return std::make_shared<ConditionalExpression>(terms);

}



void SelectStatement::handle_into_clause()
{
    ++last_token_pos; // next token

    this->out_file = read_file(QIODevice::WriteOnly);
    this->write_to_file = true;

    QString of = out_file->get_token().string_value;
    foreach (auto file_name, join_files_list) {
        if(of == file_name){
            std::string error = "Output cannot be the same as a file being read from in Select statement on line ";
            error += std::to_string(last_token_pos->line_number);
            throw std::logic_error(error);
        }
    }
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

    if(last_token_pos == tokens.cend()){
        return;
    }

    if(last_token_pos->token_type == TokenType::END){
        return;
    }

    QList<TokenType> valid_next_tokens = {TokenType::INTO, TokenType::WHERE, TokenType::GROUPBY};
    if(!valid_next_tokens.contains(last_token_pos->token_type)){
        //throw error
        std::string error = "Unexpected token on line ";
        error += std::to_string(last_token_pos->line_number);
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

    if(last_token_pos == tokens.cend()){
        return;
    }

    if(last_token_pos->token_type == TokenType::END){
        return;
    }

    QList<TokenType> valid_next_tokens = {TokenType::INTO, TokenType::WHERE, TokenType::GROUPBY};
    if(!valid_next_tokens.contains(last_token_pos->token_type)){
        //throw error
        std::string error = "Unexpected token on line ";
        error += std::to_string(last_token_pos->line_number);
        throw std::logic_error(error);
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

    if(last_token_pos == tokens.cend()){
        return;
    }

    if(last_token_pos->token_type == TokenType::END){
        return;
    }

    QList<TokenType> valid_next_tokens = {TokenType::INTO, TokenType::WHERE, TokenType::GROUPBY};
    if(!valid_next_tokens.contains(last_token_pos->token_type)){
        //throw error
        std::string error = "Unexpected token on line ";
        error += std::to_string(last_token_pos->line_number);
        throw std::logic_error(error);
    }

    optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
}

void SelectStatement::handle_where_clause()
{
    this->has_where_clause = true;
    ++last_token_pos; // next token

    this->conditional_expr = read_where(); //clause clause

    ++last_token_pos; // next token

    if(last_token_pos == tokens.cend()){
        return;
    }

    if(last_token_pos->token_type == TokenType::END){
        return;
    }

    QList<TokenType> valid_next_tokens = {TokenType::GROUPBY, TokenType::INTO};
    if(!valid_next_tokens.contains(last_token_pos->token_type)){
        //throw error
        std::string error = "Unexpected token on line ";
        error += std::to_string(last_token_pos->line_number);
        throw std::logic_error(error);
    }

    optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
}

void SelectStatement::handle_groupby_clause()
{

}


void SelectStatement::parse()
{
    //eat select
    ++last_token_pos;

    //read columns
    column_exprs = read_column_expressions();

    if(last_token_pos->token_type == TokenType::SEMICOLON){
        return;
    }

    // FROM Clause
    // check if there is a next token after FROM
    ++last_token_pos;
    if(last_token_pos == tokens.cend()){
        --last_token_pos; //get last but one token
        double line_numer = (*last_token_pos).line_number;
        QString str_num = QString::number(line_numer);

        throw std::logic_error("Unexpected end to SELECT statement on line "+ str_num.toStdString());
    }


    // read file name or path string and open file
    this->left_file = read_file();
    join_files_list["left"] = left_file->get_token().string_value;

    ++last_token_pos; // next token

    if((last_token_pos == tokens.cend()) || (last_token_pos->token_type == TokenType::SEMICOLON)){
        return;
    }

    // What is the next token?
    if(last_token_pos->token_type == TokenType::INNERJOIN){
        optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
    }
    else if(last_token_pos->token_type == TokenType::OUTERJOIN){
        optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
    }
    else if(last_token_pos->token_type == TokenType::CROSSJOIN){
        optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
    }
    else if(last_token_pos->token_type == TokenType::WHERE){
        optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
    }
    else if(last_token_pos->token_type == TokenType::GROUPBY){
        optional_actions[last_token_pos->token_type](); //call handler function for the next valid token
    }
    else{ // invalid token
        double line_numer = (*last_token_pos).line_number;
        QString str_num = QString::number(line_numer);

        throw std::logic_error("Unexpected end to SELECT statement on line "+ str_num.toStdString());
    }
}

QString SelectStatement::selected_rows()
{

}

QStringList SelectStatement::compute_columns(const QMap<QString, QStringList>& data_rows)
{
    QStringList columns;
    foreach(auto e, column_exprs){
        Term ct = e.eval(data_rows);
        QString column;
        if(ct.get_token().token_type == TokenType::STRING){
            column = ct.get_token().string_value;
        }
        else if(ct.get_token().token_type == TokenType::NUMBER){
            column = QString::number(ct.get_token().number_value);
        }
        columns.append(column);
    }
    return columns;
}

std::optional<QList<QStringList>> SelectStatement::select_with_no_join()
{
    QList<QStringList> result;

    while(!left_file->end_of_file()){
        QStringList row = left_file->readRow();

        QMap<QString, QStringList> data_rows;
        data_rows["$"] = row;

        QStringList columns;

        if(has_where_clause){
            Term t = conditional_expr->eval(data_rows);

            if(t.get_token().boolean_value == true){

                columns = compute_columns(data_rows);
            }
        }
        else{ //no where clause
            columns = compute_columns(data_rows);
        }

        //write to file?
        if(write_to_file){
            out_file->writeLine(columns.join(','));
        }
        else{
            result.append(columns);
        }

        columns = {}; //reset
    }

    if(write_to_file){
        return std::nullopt;
    }

    return result;
}

std::shared_ptr<QHash<QString,QList<qint64>> > SelectStatement::build_index(const std::shared_ptr<CSVFile>& rhs, const int& column_index)
{
    std::shared_ptr<QHash<QString,QList<qint64>> > index = std::make_shared<QHash<QString,QList<qint64>> >();

    while(!rhs->end_of_file()){
        qint64 stream_pos = rhs->get_pos();
        QStringList row = rhs->readRow();

        (*index)[row[column_index]].append(stream_pos);
    }

    return index;
}

std::optional<QList<QStringList>> SelectStatement::select_with_inner_join()
{
    QList<QStringList> result;


    if(left_file->end_of_file()){
        return std::nullopt;
    }

    if(right_file->end_of_file()){
        return std::nullopt;
    }

    // build index
    std::shared_ptr<QHash<QString,QList<qint64>> > query_lookup_index = std::make_shared<QHash<QString,QList<qint64>> >();
    bool indexing_done = false;

    //loop over files
    while(!left_file->end_of_file()){
        QStringList row = left_file->readRow();

        QMap<QString, QStringList> data_rows;
        data_rows[join_files_list["left"]] = row;

        QStringList columns;


        if(indexing_done){ //use indexing
            QList<qint64> indices = (*query_lookup_index)[row[query_index]];

            foreach(auto index, indices){
                right_file->seek_to(index);
                QStringList row = right_file->readRow();

                data_rows[join_files_list["right"]] = row;

                if(has_where_clause){
                    Term t = conditional_expr->eval(data_rows);

                    if(t.get_token().boolean_value == true){

                        columns = compute_columns(data_rows);
                    }
                }
                else{ //no where clause
                    columns = compute_columns(data_rows);
                }
            }
        }
        else{
            while(!right_file->end_of_file()){ //loop over file
                (*query_lookup_index)[row[query_index]].append(right_file->get_pos()); //save start of row in index

                QStringList row = right_file->readRow();
                data_rows[join_files_list["right"]] = row;

                if(has_where_clause){
                    Term t = conditional_expr->eval(data_rows);

                    if(t.get_token().boolean_value == true){

                        columns = compute_columns(data_rows);
                    }
                }
                else{ //no where clause
                    columns = compute_columns(data_rows);
                }

            }
            indexing_done = true; //loop runs just once over the file on the right hand side
        }


        //write to file?
        if(write_to_file){
            out_file->writeLine(columns.join(','));
        }
        else{
            result.append(columns);
        }

        columns = {}; //reset
    }

    if(write_to_file){
        return std::nullopt;
    }

    return result;
}

std::optional<QList<QStringList>> SelectStatement::execute()
{
    QList<QStringList> result;

    if(!has_join){ //read from single file; no join
        return select_with_no_join();
    }
    else{ // has a join
        if(join_type == TokenType::INNERJOIN){

        }
        else if(join_type == TokenType::OUTERJOIN){

        }
        else if(join_type == TokenType::CROSSJOIN){

        }

    }
}

