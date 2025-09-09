#include "selectstatement.h"


SelectStatement::SelectStatement(const QList<Token>& tks)
    :
    tokens{tks}
{}


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


bool SelectStatement::read_column_expressions()
{
    ColumnExpression col_expr;
    Token from_token;

    //get column tokens
    QList<Token> col_tokens;
    while(!tokens.empty()){
        Token token = tokens.front();
        tokens.pop_front();
        if(token.token_type == TokenType::FROM){
            from_token = token;
            //tokens.push_front(token);
            break;
        }
        col_tokens.append(token);
    }

    if(col_tokens.empty()){
        error_msg = "No columns provided!";
        return false;
    }

    Token last_token = col_tokens.last();
    if(last_token.token_type == TokenType::COMMA){
        error_msg = "Unexpected ',' at end of columns on line "+ QString::number(last_token.line_number);
        return false;
    }

    // create column expressions
    while(!col_tokens.empty()){
        Token token = tokens.front();
        tokens.pop_front();

        ColumnTerm ct(token);

        if(token.token_type == TokenType::COMMA){
            column_exprs.append(col_expr);
            col_expr = {};
            continue;
        }
        col_expr.add(ct);

    }

    tokens.push_back(from_token); // from token back to token list

    return true;
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
