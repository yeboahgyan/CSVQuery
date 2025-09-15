#include "selectstatement.h"
#include <stdexcept>
#include <QFileInfo>
#include <iterator>

SelectStatement::SelectStatement(const QList<Token>& tks)
    :
    tokens{tks}
{
    last_token_pos = tokens.cbegin();
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


 std::shared_ptr<CSVFile> SelectStatement::read_file()
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

     return std::make_shared<CSVFile>(f);
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
    }

    Term left_t(*last_token_pos);
    terms.append(left_t);

    ++last_token_pos; //next token
    throw_exception_if_unexpected_end();

    if(last_token_pos->token_type != TokenType::ASSIGN){
        //error
    }

    Term op_t(*last_token_pos);
    terms.append(op_t);

    ++last_token_pos; //next token
    throw_exception_if_unexpected_end();

    if(last_token_pos->token_type != TokenType::COLUMNNAME ){
        //error
    }

    Term right_t(*last_token_pos);
    terms.append(right_t);
    terms.append(right_t);

    return std::make_shared<ConditionalExpression>(terms);

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
        this->join_type = last_token_pos->token_type;

        ++last_token_pos; // next token

        this->right_file = read_file();

        ++last_token_pos; // next token
        this->on_clause = read_on_clause();
    }
    else if(last_token_pos->token_type == TokenType::OUTERJOIN){
        this->join_type = last_token_pos->token_type;

        ++last_token_pos; // next token

        this->right_file = read_file();

        ++last_token_pos; // next token
        this->on_clause = read_on_clause();
    }
    else if(last_token_pos->token_type == TokenType::CROSSJOIN){
        this->join_type = last_token_pos->token_type;

        ++last_token_pos; // next token

        this->right_file = read_file();
    }
    else if(last_token_pos->token_type == TokenType::WHERE){

    }
    else if(last_token_pos->token_type == TokenType::GROUPBY){

    }
    else{ // invalid token
        double line_numer = (*last_token_pos).line_number;
        QString str_num = QString::number(line_numer);

        throw std::logic_error("Unexpected end to SELECT statement on line "+ str_num.toStdString());
    }
}


Result SelectStatement::eval()
{
    //open files
    //iterate over files
    //pass row to conditionalexpression
    //if true then pass row to columnexpression
    //save result to result object or out_file
    //return
    Result result;

    if(has_join){

        if(join_type == TokenType::INNERJOIN){
            result = inner_join_eval();
        }
        else if(join_type == TokenType::OUTERJOIN){
            result = outer_join_eval();
        }
        else if(join_type == TokenType::CROSSJOIN){
            result = cross_join_eval();
        }

    }else{
        // no join
        result = no_join_eval();
    }

    return result;
}

bool SelectStatement::get_file_mem_map(std::shared_ptr<QFile> f, std::shared_ptr<QBuffer> b)
{
    if (!f->open(QIODevice::ReadOnly)) {
        // Handle error
        return false;
    }

    uchar *mappedData = f->map(0, f->size());
    if (!mappedData) {
        // Handle error
        f->close();
        return false;
    }

    b->setData(reinterpret_cast<const char*>(mappedData), f->size());
    b->open(QIODevice::ReadOnly);

    return true;
}

Result SelectStatement::no_join_eval()
{
    Result result;

    std::shared_ptr<QFile>f = std::make_shared<QFile>(csv_files.first());

    std::shared_ptr<QBuffer> b = std::make_shared<QBuffer>();

    if(!get_file_mem_map(f,b)){
        result.successful =false;
        result.error = "Error mapping "+csv_files.first() +" to memory";
        return result;
    }

    QTextStream in(b.get());
    while(!in.atEnd()){
        QStringList row = in.readLine().split(',');

        if(has_where_clause){
            ColumnResult cres = conditional_expr.eval(row);
            if(cres.token_type != TokenType::BOOLEAN){
                result.successful = false;
                result.error = "error parsing where clause";
                return result;
            }

            if(cres.boolean_value == false){
                continue; //skip row
            }

            //select column values
            // write to out file if set
        }
    }

    return result;
}

Result SelectStatement::inner_join_eval()
{
    Result result;



    return result;
}


bool SelectStatement::get_file(bool is_out_file)
{
    Token token = tokens.front();
    tokens.pop_front();

    QList<TokenType> acceptable_tokens = {TokenType::NAME, TokenType::STRING};


    if(!acceptable_tokens.contains(token.token_type)){
        error_msg = "Unexpected value for file on line "+ QString::number(token.line_number);
        return false;
    }

    QString file;
    if(token.token_type == TokenType::STRING){
        file = token.string_value;
    }
    else if(token.token_type == TokenType::NAME){
        if(!symbol_table.contains(token.string_value)){
            error_msg = "Unknown name "+ token.string_value + " on line "+ QString::number(token.line_number);
            return false;
        }

        TokenType name_type = symbol_table[token.string_value];
        if(name_type != TokenType::STRING){
            error_msg = token.string_value + " is not an alias for a file path on line "+ QString::number(token.line_number);
            return false;
        }

        file = strings_table[token.string_value];
    }

    QFileInfo fileInfo(file);
    if( !fileInfo.exists() || !fileInfo.isFile()){

        error_msg = "Provided path is not a valid file at line "+ QString::number(token.line_number);
        return false;
    }

    if(is_out_file){
        out_file = file;
    }else{
        csv_files.append(file);
    }

    return true;
}

bool SelectStatement::read_join(TokenType join_type)
{
    //get joined file
    bool success = get_file();
    if(!success){
        return false;
    }

    if(join_type == TokenType::CROSSJOIN){
        return true;
    }

    //eat ON token
    Token token = tokens.front();
    tokens.pop_front();

    if(token.token_type != TokenType::ON){
        error_msg = "Unexpected token after JOIN on line "+ QString::number(token.line_number);
        return false;
    }

    //get first column term
    token = tokens.front();
    tokens.pop_front();

    QList<TokenType> acceptable_tokens = {TokenType::COLUMNNAME, TokenType::COLUMNNUMBER};

    if(!acceptable_tokens.contains(token.token_type)){
        error_msg = "Expected a column after ON clause on line "+ QString::number(token.line_number);
        return false;
    }

    //ColumnExpression col_expr;
    ColumnTerm ct1{token};

    on_clause.add(ct1);

    //get assign term
    token = tokens.front();
    tokens.pop_front();

    if(token.token_type != TokenType::ASSIGN){
        error_msg = "Unexpected token after column in ON clause on line "+ QString::number(token.line_number);
        return false;
    }

    //handle outer join
    token.token_type = (join_type == TokenType::OUTERJOIN)? TokenType::NOTEQUALTO : TokenType::ASSIGN;

    ColumnTerm ct2{token};
    on_clause.add(ct2);

    //get second column term
    token = tokens.front();
    tokens.pop_front();

    if(!acceptable_tokens.contains(token.token_type)){
        error_msg = "Expected a column after ON clause on line "+ QString::number(token.line_number);
        return false;
    }

    ColumnTerm ct3{token};
    on_clause.add(ct3);


    return true;
}

bool SelectStatement::read_where()
{
    while(!tokens.empty()){
        Token token = tokens.front();
        tokens.pop_front();

        if(token.token_type == TokenType::SEMICOLON){
            break;
        }
        ColumnTerm ct(token);
        conditional_expr.add(ct);
    }
    return true;
}

bool SelectStatement::read_into()
{
    //get joined file
    bool success = get_file(true); // read out file name into this->out_file
    if(!success){
        return false;
    }
    return true;
}
