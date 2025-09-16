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

     return std::make_shared<CSVFile>(f, m);
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
    terms.append(left_t);

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
    terms.append(right_t);
    terms.append(right_t);

    return std::make_shared<ConditionalExpression>(terms);

}



void SelectStatement::handle_into_clause()
{
    ++last_token_pos; // next token

    this->out_file = read_file(QIODevice::WriteOnly);
    this->wite_to_file = true;
}

void SelectStatement::handle_inner_join()
{
    this->has_join = true;
    this->join_type = last_token_pos->token_type;

    ++last_token_pos; // next token

    this->right_file = read_file();

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

