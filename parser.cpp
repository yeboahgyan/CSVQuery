
#include "parser.h"
#include "conditionalexpression.h"
#include <QFileInfo>

Parser::Parser(std::shared_ptr<QTextStream> ts): text_stream(ts)
{

}


QList<Token> Parser::read_statement()
{
    QList<Token> tokens;

    Token token = compose_token();
    while(token.token_type != TokenType::END){
        if(token.token_type == TokenType::SEMICOLON){
            break;
        }
        tokens.append(token);
        token = compose_token();
    }

    return tokens;

}

Token Parser::compose_token()
{
    Token tk = read_token();

    if(tk.token_type == TokenType::SINGLEQOUTE){
        tk = read_token(); //string token between quotes
        tk.token_type = TokenType::STRING;

        Token next_token = read_token();
        if(next_token.token_type != TokenType::SINGLEQOUTE){
            tk.token_type = TokenType::ERROR;
        }
    }
    else if(tk.token_type == TokenType::DOUBLEQUOTE){
        tk = read_token(); //string token between quotes
        tk.token_type = TokenType::STRING;

        Token next_token = read_token();
        if(next_token.token_type != TokenType::DOUBLEQUOTE){
            tk.token_type = TokenType::ERROR;
        }
    }
    else if(tk.token_type == TokenType::LSQBRACKET){
        tk = read_token(); //string token between square brackets
        Token next_token = read_token();
        if(next_token.token_type != TokenType::RSQBRACKET){
            tk.token_type = TokenType::ERROR;
        }
        else{ //parse string inbetween square brackets
            bool ok;
            QString val = tk.string_value;

            double number = val.toDouble(&ok);

            if(ok){
                tk.number_value = number;
                tk.token_type = TokenType::COLUMNNUMBER;
                //tk.line_number = line_number;
            }
            else{
                tk.string_value = val;
                tk.number_value = numbers_table[tk.string_value.toLower()]; // get column index
                tk.token_type = TokenType::COLUMNNAME;
                //tk.line_number = line_number;
            }
        }
    }
    else if(tk.token_type == TokenType::NAME){
        if(symbol_table.contains(tk.string_value.toLower())){
            tk.token_type = symbol_table[tk.string_value.toLower()]; // possible values TokenType::FUNCTION, TokenType::NAME

            if(tk.token_type == TokenType::COLUMNNAME){
                tk.number_value = numbers_table[tk.string_value.toLower()]; // can column be specified without brackets ([t.name])?
            }
            else if(tk.token_type == TokenType::STRING){
                tk.string_value = strings_table[tk.string_value.toLower()];
            }
            else if(tk.token_type == TokenType::FUNCTION){
                //do nothing here?
                tk.func = funcs_table[tk.string_value];
            }
        }
        else if(keywords.contains(tk.string_value.toLower())){
            tk.token_type = keywords[tk.string_value.toLower()]; // possible values keywords like SELECT, WHERE ...
        }
        else{
            tk.token_type = TokenType::ERROR;
        }

    }

    current_token = tk;

    return tk;
}

Token Parser::read_token()
{

    QChar ch;
    QList<QChar> ops = {';', ':', '+', '-', '*', '/', '=', '(', ')', '[', ']', ','};
    QHash<QChar, TokenType> ch_to_token;
    ch_to_token[';'] = TokenType::SEMICOLON;
    ch_to_token[':'] = TokenType::COLON;
    ch_to_token['+'] = TokenType::PLUS;
    ch_to_token['-'] = TokenType::MINUS;
    ch_to_token['*'] = TokenType::MULT;
    ch_to_token['/'] = TokenType::DIV;
    ch_to_token['='] = TokenType::ASSIGN;
    ch_to_token['('] = TokenType::RBRACKET;
    ch_to_token[')'] = TokenType::RBRACKET;
    ch_to_token['['] = TokenType::LSQBRACKET;
    ch_to_token[']'] = TokenType::RSQBRACKET;
    ch_to_token[','] = TokenType::COMMA;

    if(text_stream->atEnd()){
        current_token = {TokenType::END};
        return current_token;
    }

    (*text_stream) >> ch;

    while(ch.isSpace()){ //skip whitespace
        if(ch == '\n'){
            line_number ++; //track current line number
        }
        (*text_stream) >> ch;
    }


    if(ch.isNull())
    {
        //return current_token = {TokenType::END};
        current_token = {TokenType::END};
    }
    else if(ops.contains(ch))
    {
        //return current_token = {static_cast<TokenType>(ch.toLatin1())};
        current_token = {ch_to_token[ch]};
        current_token.line_number = line_number;
    }
    else if (ch.isDigit()){
        text_stream->seek(text_stream->pos() - 1); //put char back in stream
        double number;
        (*text_stream)>>number;

        current_token.token_type = TokenType::NUMBER;
        current_token.line_number = line_number;
        current_token.number_value = number;
    }
    else if (ch.isLetter()){

        QString name;
        name.append(ch);

        (*text_stream) >> ch;
        while(!text_stream->atEnd()){
            if(ch.isSpace() || ops.contains(ch)){ // handle variable names and function names (names that end with a'(' )
                break;
            }
            name.append(ch);
            (*text_stream) >> ch;
        }

        text_stream->seek(text_stream->pos() - 1); //put char back in stream

        current_token.string_value = name;
        current_token.token_type = TokenType::NAME;
        current_token.line_number = line_number;
            //return current_token;

    }

    return current_token;
}

Result Parser::import_statement(QList<Token>& tokens)
{
    Result result;
    QString file;

    if(tokens.empty()){
        result.hasOutput=false;
        result.successful=false;
        result.error= "Unexpected end to import statement!";
        return result;
    }

    Token token = tokens.front();
    tokens.pop_front();

    if(token.token_type == TokenType::STRING){

        file = token.string_value;
    }
    else{
        result.successful = false;
        result.hasOutput = false;
        result.error = "Expected a string at line "+ QString::number(token.line_number);
    }


    QFileInfo fileInfo(file);
    if( !fileInfo.exists() || !fileInfo.isFile()){
        result.successful = false;
        result.hasOutput = false;
        result.error = "String is not a valid file at line "+ QString::number(token.line_number);
       // return result;
    }

    //TODO...
    //load file definitions

    return result;
}

Result Parser::assignment_statement(Token name, QList<Token>& tokens)
{
    Result result;
    result.hasOutput=false;
    result.successful=false;


    if(tokens.empty()){
        result.error= "Unexpected end to assignment statement!";
        return result;
    }


    Token token = tokens.front();
    tokens.pop_front();

    QString name_prefix;

    if(token.token_type == TokenType::COLON){
        if(tokens.empty()){
            result.error= "Unexpected end to assignment statement!";
            return result;
        }
        token = tokens.front();
        tokens.pop_front();
    }
    else if(token.token_type == TokenType::ASSIGN){
        if(tokens.empty()){
            result.error= "Unexpected end to assignment statement!";
            return result;
        }
        token = tokens.front();
        tokens.pop_front();
    }

    //Get value to assign
    if(token.token_type == TokenType::NUMBER){
        symbol_table[name.string_value] = TokenType::NUMBER;

        numbers_table[name.string_value] = token.number_value;

        result.hasOutput=false;
        result.successful=true;
    }
    else if(token.token_type == TokenType::STRING){
        symbol_table[name.string_value] = TokenType::STRING;

        strings_table[name.string_value] = token.string_value;

        result.hasOutput=false;
        result.successful=true;
    }
    else{
        result.hasOutput=false;
        result.successful=false;
        result.error= "Expected to assign a string or number!";
    }



    return result;
}

QList<ColumnExpression> Parser::read_column_expr(QList<Token> tokens)
{
    QList<ColumnExpression> column_expressions;

    while(!tokens.empty()){

        Token current_token = tokens.front();
        tokens.pop_front();

        if(current_token.token_type == TokenType::FROM){
            //tokens.push_front(current_token);
            break;
        }

        ColumnExpression col_expr;
        while(current_token.token_type != TokenType::COMMA){ // get column terms

            if(current_token.token_type == TokenType::FROM){
                //tokens.push_front(current_token);
                break;
            }

            ColumnTerm term(current_token);
            col_expr.add(term);

            if(tokens.empty()){
                break;
            }

            current_token = tokens.front();
            tokens.pop_front();
        }

        column_expressions.append(col_expr);
        if(current_token.token_type != TokenType::FROM){
            //tokens.push_front(current_token);
            break;
        }

    }

    return column_expressions;
}

SelectStatement Parser::select_statement(QList<Token>& tokens)
{
    SelectStatement select(tokens);
    Result result;
    if(tokens.empty()){
        result.hasOutput=false;
        result.successful=false;
        result.error= "Unexpected end to select statement!";
        return result;
    }

    QList<ColumnExpression> column_expressions = read_column_expr(tokens);
    ConditionalExpression where_clause;
    QString out_file;

    if(column_expressions.empty()){
        result.hasOutput=false;
        result.successful=false;
        result.error= "Unexpected end of column list!";
        return result;
    }

    //get WHERE token
    Token current_token;
    if(!tokens.empty()){
        current_token = tokens.front();
        tokens.pop_front();

        if(current_token.token_type == TokenType::WHERE){
            //read where clause
        }
    }

    //get INTO clause
    if(!tokens.empty()){
        current_token = tokens.front();
        tokens.pop_front();

        if(current_token.token_type == TokenType::INTO){
            //read INTO clause
            if(tokens.empty()){
                result.hasOutput=false;
                result.successful=false;
                result.error= "No output file provided!";
                return result;
            }

            current_token = tokens.front();
            tokens.pop_front();

            if(current_token.token_type == TokenType::STRING || current_token.token_type == TokenType::NAME){
                out_file = current_token.string_value;
            }

        }
    }

    if(!tokens.empty()){
        current_token = tokens.front();
        tokens.pop_front();
        if(current_token.token_type != TokenType::SEMICOLON){
            result.hasOutput=false;
            result.successful=false;
            result.error= "There was no terminating ';'!";
            return result;
        }
    }
    else{
        result.hasOutput=false;
        result.successful=false;
        result.error= "There was no terminating ';'!";
        return result;
    }


    return result;
}

Result Parser::delete_statement(QList<Token>& tokens)
{
    Result result;
    if(tokens.empty()){
        result.hasOutput=false;
        result.successful=false;
        result.error= "Unexpected end to delete statement!";
        return result;
    }
    //TODO

    return result;
}

Result Parser::update_statement(QList<Token>& tokens)
{
    Result result;
    if(tokens.empty()){
        result.hasOutput=false;
        result.successful=false;
        result.error= "Unexpected end to update statement!";
        return result;
    }
    //TODO

    return result;
}

QList<Result> Parser::execute()
{
    QList<Result> results;

    while(!(*text_stream).atEnd()){
        Result res;
        QList<Token> tokens = read_statement();
        if(tokens.empty()){
            res.hasOutput = false;
            res.successful = false;
            results.append(res);
            continue;
        }


        foreach(auto token, tokens){ //statement has an error
            if(token.token_type == TokenType::ERROR){
                res.hasOutput = false;
                res.successful = false;
                res.error = "Unexpected token on line "+QString::number(token.line_number);
                results.append(res);
                return results;
            }
        }

        Token first_token = tokens.front();
        tokens.pop_front();

        if(first_token.token_type != TokenType::STRING){

            res.hasOutput = false;
            res.successful = false;
            res.error = "Unexpected token on line "+QString::number(first_token.line_number);

            results.append(res);
        }
        else{
            if(first_token.string_value.toLower() == "import"){ // import statement
                res = import_statement(tokens);
                results.append(res);
            }
            else if(first_token.string_value.toLower() == "select"){ //select statement
                res = select_statement(tokens);
                results.append(res);
            }
            else if(first_token.string_value.toLower() == "delete"){ //delete statement
                res = delete_statement(tokens);
                results.append(res);
            }
            else{ //assigment statement
                res = assignment_statement(first_token, tokens);
                results.append(res);
            }
        }

    }

    return results;
}

