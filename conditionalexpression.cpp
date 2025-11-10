#include "conditionalexpression.h"
#include <stdexcept>
#include <iostream>
#include <QRegularExpression>
#include "term.h"


namespace csvquery {

    ConditionalExpression::ConditionalExpression(const QList<Term>& ts)
        : terms(ts)
    {
        current_term = terms.begin();
        item_left = (terms.length() == 0) ? 0 : terms.length() - 1;
    }

    void ConditionalExpression::move_to_next_term()
    {
        if (current_term == terms.end()) {
            return;
        }

        //auto n = current_term;

        if (item_left == 0) {
            current_term = terms.end();
            return;
        }

        ++current_term;
        --item_left;
        //return n;
    }

    QList<Term>::iterator ConditionalExpression::get_current_term() const {
        return current_term;
    }

    Term ConditionalExpression::get_current_term()
    {
        if (current_term == terms.end()) {
            //QList<Term> previous_term = 
            Token t = { .token_type = TokenType::END };
            return Term(t);
        }

        return *current_term;
    }

    QList<Term>::iterator ConditionalExpression::peak_next_term() {
        QList<Term>::iterator n = current_term + 1;
        return n;
    }

    //Where clause
    Term ConditionalExpression::or_op(Term left, Term right) // logical OR
    {
        Term result;

        if (left.get_token().token_type == TokenType::NUMBER) {
            if (right.get_token().token_type != TokenType::NUMBER) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }

        }
        else if (left.get_token().token_type == TokenType::STRING) {
            if (right.get_token().token_type != TokenType::STRING) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a string!";
                throw std::logic_error(error.toStdString());
            }
        }
        else if (left.get_token().token_type == TokenType::BOOLEAN) {
            if (right.get_token().token_type != TokenType::BOOLEAN) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a boolean!";
                throw std::logic_error(error.toStdString());
            }
        }

        Token t;
        t.token_type = TokenType::BOOLEAN;
        t.boolean_value = left.get_token().boolean_value || right.get_token().boolean_value;
        result = Term(t);

        return result;
    }

    Term ConditionalExpression::and_op(Term left, Term right) // logical AND
    {
        Term result;

        if (left.get_token().token_type == TokenType::BOOLEAN) {
            if (right.get_token().token_type != TokenType::BOOLEAN) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a boolean!";
                throw std::logic_error(error.toStdString());
            }

            Token t;
            t.token_type = TokenType::BOOLEAN;
            t.boolean_value = left.get_token().boolean_value && right.get_token().boolean_value;
            result = Term(t);
        }
        else {
            QString error = "Left operand of AND on line ";
            error += QString::number(right.get_line_number());
            error += " should be a boolean!";
            throw std::logic_error(error.toStdString());
        }

        return result;
    }

    Term ConditionalExpression::eq(Term left, Term right) // Equal
    {
        Term result;

        Token t;
        t.token_type = TokenType::BOOLEAN;

        if (left.get_token().token_type == TokenType::NUMBER) {
            if (right.get_token().token_type != TokenType::NUMBER) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }
            t.boolean_value = left.get_token().number_value == right.get_token().number_value;
        }
        else if (left.get_token().token_type == TokenType::STRING) {
            if (right.get_token().token_type != TokenType::STRING) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a string!";
                throw std::logic_error(error.toStdString());
            }

            t.boolean_value = left.get_token().string_value.toLower() == right.get_token().string_value.toLower();
        }
        else if (left.get_token().token_type == TokenType::BOOLEAN) {
            if (right.get_token().token_type != TokenType::BOOLEAN) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a boolean!";
                throw std::logic_error(error.toStdString());
            }
            t.boolean_value = left.get_token().boolean_value == right.get_token().boolean_value;
        }
        else {
            QString error = "Unexpected left operand on line ";
            error += QString::number(right.get_line_number());
            throw std::logic_error(error.toStdString());
        }

        result = Term(t);

        return result;
    }

    Term ConditionalExpression::neq(Term left, Term right) // Not Equal
    {
        Term result;

        Token t;
        t.token_type = TokenType::BOOLEAN;

        if (left.get_token().token_type == TokenType::NUMBER) {
            if (right.get_token().token_type != TokenType::NUMBER) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }
            t.boolean_value = left.get_token().number_value != right.get_token().number_value;
        }
        else if (left.get_token().token_type == TokenType::STRING) {
            if (right.get_token().token_type != TokenType::STRING) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a string!";
                throw std::logic_error(error.toStdString());
            }

            t.boolean_value = left.get_token().string_value.toLower() != right.get_token().string_value.toLower();
        }
        else if (left.get_token().token_type == TokenType::BOOLEAN) {
            if (right.get_token().token_type != TokenType::BOOLEAN) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a boolean!";
                throw std::logic_error(error.toStdString());
            }
            t.boolean_value = left.get_token().boolean_value != right.get_token().boolean_value;
        }
        else {
            QString error = "Unexpected left operand on line ";
            error += QString::number(right.get_line_number());
            throw std::logic_error(error.toStdString());
        }

        result = Term(t);

        return result;
    }

    Term ConditionalExpression::gt(Term left, Term right) // Greater than
    {
        Term result;
        if (left.get_token().token_type == TokenType::NUMBER) {

            if (right.get_token().token_type == TokenType::NUMBER) {
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = left.get_token().number_value > right.get_token().number_value;
                result = Term(t);
            }
            else {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }
        }
        else {
            QString error = "Left operand on line ";
            error += QString::number(left.get_line_number());
            error += " should be a number!";
            throw std::logic_error(error.toStdString());
        }

        return result;
    }

    Term ConditionalExpression::lt(Term left, Term right) // Less than
    {
        Term result;
        if (left.get_token().token_type == TokenType::NUMBER) {

            if (right.get_token().token_type == TokenType::NUMBER) {
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = left.get_token().number_value < right.get_token().number_value;
                result = Term(t);
            }
            else {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }
        }
        else {
            QString error = "Left operand on line ";
            error += QString::number(left.get_line_number());
            error += " should be a number!";
            throw std::logic_error(error.toStdString());
        }

        return result;
    }

    Term ConditionalExpression::ge(Term left, Term right) // Greater than or equal to
    {
        Term result;
        if (left.get_token().token_type == TokenType::NUMBER) {

            if (right.get_token().token_type == TokenType::NUMBER) {
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = left.get_token().number_value >= right.get_token().number_value;
                result = Term(t);
            }
            else {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }
        }
        else {
            QString error = "Left operand on line ";
            error += QString::number(left.get_line_number());
            error += " should be a number!";
            throw std::logic_error(error.toStdString());
        }

        return result;
    }

    Term ConditionalExpression::le(Term left, Term right) // Less than or equal to
    {
        Term result;
        if (left.get_token().token_type == TokenType::NUMBER) {

            if (right.get_token().token_type == TokenType::NUMBER) {
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = left.get_token().number_value <= right.get_token().number_value;
                result = Term(t);
            }
            else {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }
        }
        else {
            QString error = "Left operand on line ";
            error += QString::number(left.get_line_number());
            error += " should be a number!";
            throw std::logic_error(error.toStdString());
        }

        return result;
    }

    // SQL Like pattern matching; left term is value string and right term is pattern string
    Term ConditionalExpression::like(Term left, Term right) 
    {
        Term result;

        Token t;
        t.token_type = TokenType::BOOLEAN;

        if (left.get_token().token_type != TokenType::STRING || right.get_token().token_type != TokenType::STRING) {
            QString error = "Like operator expects String operands on line ";
            error += QString::number(right.get_line_number());
            //error += " should also be a number!";
            throw std::logic_error(error.toStdString());
        }

        QString regexPattern;
        QString pattern = right.get_token().string_value;
        regexPattern.reserve(pattern.size() * 2);

        // Escape regex special characters, then replace SQL wildcards
        for (QChar c : pattern) {
            switch (c.unicode()) {
            case '%': regexPattern += ".*"; break;
            case '_': regexPattern += ".";  break;
                // Escape regex metacharacters
            case '.': case '^': case '$': case '|': case '(': case ')':
            case '[': case ']': case '{': case '}': case '+': case '?': case '\\':
                regexPattern += '\\';
                regexPattern += c;
                break;
            default:
                regexPattern += c;
                break;
            }
        }

        // Add anchors to match the whole string
        regexPattern.prepend('^');
        regexPattern.append('$');

        QRegularExpression regex(regexPattern, QRegularExpression::CaseInsensitiveOption);
        t.boolean_value = regex.match(left.get_token().string_value).hasMatch();

        result = Term(t);

        return result;
    }

    Term ConditionalExpression::not_like(Term left, Term right)
    {
        Term result = like(left, right);
        Token t = result.get_token();
        t.boolean_value = !t.boolean_value; // reverse boolean result from like test
        result = Term(t);

        return result;
    }


    // Used for Where clause
    Term ConditionalExpression::cond_expr(const QMap<QString, QStringList>& data_rows, bool get)
    {
        Term left = cond_term(data_rows, get);

        //std::cout<<"cond_expr() left2: "<<left.get_token().to_string().toStdString()<<", value:"<<left.get_token().string_value.toStdString()<<"\n";

        while (current_term != terms.end()) { // read and compute all terms in column expression
            //std::cout<<"expr() current term: "<<current_term->get_token().to_string().toStdString()<<"\n";
            if (get_current_term().get_token().token_type == TokenType::AND) {
                Term right = cond_term(data_rows, true);
                //std::cout<<"cond_expr() AND right: "<<right.get_token().to_string().toStdString()<<"\n";
                left = and_op(left, right);
            }
            else if (get_current_term().get_token().token_type == TokenType::OR) {
                left = or_op(left, cond_term(data_rows, true));
            }
            else {
                break;
            }

        }

        return left;
    }

    Term ConditionalExpression::cond_term(const QMap<QString, QStringList>& data_rows, bool get)
    {
        Term left = cond_primary(data_rows, get);

       // std::cout<<"cond_term() current left: "<<left.get_token().to_string().toStdString()<<" value:"<<left.get_token().string_value.toStdString()<<"\n";
        //qDebug() << "current term:" << current_term->get_token().to_string();

        while (current_term != terms.end()) {
            //qDebug() << "current term:" << current_term->get_token().to_string();

            if (get_current_term().get_token().token_type == TokenType::ASSIGN) { //Equal
                Term right = cond_primary(data_rows, true);
                //std::cout<<"cond_term() == right: "<<right.get_token().to_string().toStdString()<<"\n";
                left = eq(left, right);
            }
            else if (get_current_term().get_token().token_type == TokenType::NOTEQUALTO) {
                Term right = cond_primary(data_rows, true);
                //std::cout<<"cond_term() != right: "<<right.get_token().to_string().toStdString()<<"\n";
                left = neq(left, right);
                //std::cout<<"Rsult: "<<left.get_token().to_string().toStdString()<<"\n";
            }
            else if (get_current_term().get_token().token_type == TokenType::LESSTHAN) {
                left = lt(left, cond_primary(data_rows, true));
            }
            else if (get_current_term().get_token().token_type == TokenType::GREATERTHAN) {
                left = gt(left, cond_primary(data_rows, true));
            }
            else if (get_current_term().get_token().token_type == TokenType::LESSTHANOREQUAL) {
                left = le(left, cond_primary(data_rows, true));
            }
            else if (get_current_term().get_token().token_type == TokenType::GREATERTHANOREQUAL) {
                left = ge(left, cond_primary(data_rows, true));
            }
            else if (get_current_term().get_token().token_type == TokenType::LIKE) {
                //qDebug() << "evaluating like!";
                left = like(left, cond_primary(data_rows, true));
            }
            else if (get_current_term().get_token().token_type == TokenType::NOTLIKE) {
                //qDebug() << "evaluating not like!";
                left = not_like(left, cond_primary(data_rows, true));
            }
            else {
                break;
            }
        }

        return left;
    }


    Term ConditionalExpression::cond_primary(const QMap<QString, QStringList>& data_rows, bool get)
    {
        if (get) {
            move_to_next_term();
        }
        Term left = get_current_term();

        ////std::cout<<"cond_primary() current left: "<<left.get_token().to_string().toStdString()<<"\n";

        if (left.get_token().token_type == TokenType::LBRACKET) {
            //auto e = cond_expr(row, true);
            //left = e;
            move_to_next_term(); //eat (
            //std::cout<<" token after ( is "<< current_term->get_token().to_string().toStdString()<<"\n";
            auto ts = read_cond_expression();
            ConditionalExpression ce(ts);
            Term t = ce.eval(data_rows);

            if (current_term->get_token().token_type != TokenType::RBRACKET) {
                //std::cout<<"next token "<<current_term->get_token().to_string().toStdString()<<"\n";
                QString error = "Expected a ) on line " + QString::number(current_term->get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                //left = e;

                //std::cout<<"ce result="<<t.get_token().to_string().toStdString()<<" {"<<t.get_token().string_value.toStdString()<<"}\n";

                left = t;
                move_to_next_term(); //eat )
            }
        }
        //else if(left.get_token().token_type == TokenType::RBRACKET){
            //move_to_next_term();
        //}
        else {
            //left = expr(row, get);
            //move_to_next_term();
            //std::cout<<"Calling read_expression()\n";
            Expression e = read_expression();
            //std::cout<<"evaluating left side of conditional expression\n";
            left = e.eval(data_rows);
            //std::cout<<"ex result="<<left.get_token().to_string().toStdString()<<" {"<<left.get_token().string_value.toStdString()<<"}\n";
            //move_to_next_term();
            //std::cout<<"done.\n";
        }

        return left;
    }

    Term ConditionalExpression::eval(const QMap<QString, QStringList>& data_rows)
    {
        Token t = { .token_type = TokenType::END, .token_name = "TokenType::END" };
        Term result(t);

        //std::cout<<"ConditionalExpression::eval(): cond_expr()\n";
        result = cond_expr(data_rows, false);
        //std::cout<<"done.\n";

        if (get_current_term().get_token().token_type != TokenType::END) {
            QString error = "Incorrect syntax in conditional expression '";
            error += get_current_term().get_token().string_value;
            error += "' on line ";
            error += QString::number(get_current_term().get_token().line_number);
            throw std::logic_error(error.toStdString());
        }

        //reset iterators
        reset_iterators();


        return result;
    }

    QList<Term> ConditionalExpression::read_cond_expression()
    {
        QList<Term> ts;

        /*
        QList<QString> relational_tokens = {"TokenType::ASSIGN", "TokenType::AND", "TokenType::OR", "TokenType::LESSTHAN", "TokenType::GREATERTHAN",
                                            "TokenType::LESSTHANOREQUAL", "TokenType::GREATERTHANOREQUAL", "TokenType::NOTEQUALTO", "TokenType::RBRACKET"};
        */
        QList<QString> relational_tokens = { "TokenType::RBRACKET" };

        while (current_term != terms.end()) {
            if (relational_tokens.contains(current_term->get_token().to_string())) {
                break;
            }
            Term t(*current_term);
            ts.append(t);
            move_to_next_term();
        }
        //Expression e(ts);
        //return e;
        return ts;
    }

    Expression ConditionalExpression::read_expression()
    {
        QList<Term> ts;


        QList<QString> relational_tokens = { "TokenType::ASSIGN", "TokenType::AND", "TokenType::OR", "TokenType::LESSTHAN", "TokenType::GREATERTHAN",
                                            "TokenType::LESSTHANOREQUAL", "TokenType::GREATERTHANOREQUAL", "TokenType::NOTEQUALTO", "TokenType::LIKE", "TokenType::NOTLIKE"};

        //QList<QString> relational_tokens = {"TokenType::RBRACKET"};

        while (current_term != terms.end()) {
            if (relational_tokens.contains(current_term->get_token().to_string())) {
                break;
            }
            Term t(*current_term);
            ts.append(t);
            move_to_next_term();
        }

        if (ts.isEmpty()) {
            QString error = "Unexpected end to expression in Where clause!";
            throw std::logic_error(error.toStdString());
        }

        Expression e(ts);
        return e;
        //return ts;
    }

    std::function<Term(const QMap<QString, QStringList>& data_rows)> ConditionalExpression::comp_cond_expr(const QMap<QString, QStringList> data_rows, bool get)
    {
        //Term left = cond_term(data_rows, get);
        auto left_func = comp_cond_term(data_rows, get);

        //std::cout<<"cond_expr() left2: "<<left.get_token().to_string().toStdString()<<", value:"<<left.get_token().string_value.toStdString()<<"\n";

        while (current_term != terms.end()) { // read and compute all terms in column expression
            //std::cout<<"expr() current term: "<<current_term->get_token().to_string().toStdString()<<"\n";
            if (get_current_term().get_token().token_type == TokenType::AND) {
                //Term right = cond_term(data_rows, true);
                auto right_func = comp_cond_term(data_rows, true);
                //std::cout<<"cond_expr() AND right: "<<right.get_token().to_string().toStdString()<<"\n";
                //left = and_op(left, right);

                auto comp_and_f = comp_and_op(left_func(data_rows), right_func(data_rows));

                left_func = [comp_and_f, left_func, right_func](const QMap<QString, QStringList>& data_rows) {
                    return comp_and_f(left_func(data_rows), right_func(data_rows));
                    };
            }
            else if (get_current_term().get_token().token_type == TokenType::OR) {
                //left = or_op(left, cond_term(data_rows, true));

                auto right_func = comp_cond_term(data_rows, true);
                auto comp_or_f = comp_or_op(left_func(data_rows), right_func(data_rows));

                left_func = [comp_or_f, left_func, right_func](const QMap<QString, QStringList>& data_rows) {
                    return comp_or_f(left_func(data_rows), right_func(data_rows));
                    };
            }
            else {
                break;
            }

        }

        return left_func;
    }

    std::function<Term(const QMap<QString, QStringList>& data_rows)> ConditionalExpression::comp_cond_term(const QMap<QString, QStringList> data_rows, bool get)
    {
        //Term left = cond_primary(data_rows, get);
        //std::function<Term(const QMap<QString, QStringList>& data_rows)> result_func;

        auto left_func = comp_cond_primary(data_rows, get);
        //result_func = left_func;

        // std::cout<<"cond_term() current left: "<<left.get_token().to_string().toStdString()<<" value:"<<left.get_token().string_value.toStdString()<<"\n";
         //qDebug() << "current term:" << current_term->get_token().to_string();


        while (current_term != terms.end()) {
            //qDebug() << "current term:" << current_term->get_token().to_string();

            if (get_current_term().get_token().token_type == TokenType::ASSIGN) { //Equal
                //Term right = cond_primary(data_rows, true);
                auto right_func = comp_cond_primary(data_rows, true);

                //std::cout<<"cond_term() == right: "<<right.get_token().to_string().toStdString()<<"\n";
                //left = eq(left, right);

                auto comp_f_eq = comp_eq(left_func(data_rows), right_func(data_rows));

                left_func = [comp_f_eq, left_func, right_func](const QMap<QString, QStringList>& data_rows) {
                    //return eq(left_func(data_rows), right_func(data_rows));
                    auto res = comp_f_eq(left_func(data_rows), right_func(data_rows));
                    //eq_count[res.get_token().boolean_value]++; //debugging
                    return res;
                    };
                
            }
            else if (get_current_term().get_token().token_type == TokenType::NOTEQUALTO) {
                //Term right = cond_primary(data_rows, true);
                auto right_func = comp_cond_primary(data_rows, true);
                //std::cout<<"cond_term() != right: "<<right.get_token().to_string().toStdString()<<"\n";
                //left = neq(left, right);

                auto comp_neq_f = comp_neq(left_func(data_rows), right_func(data_rows));

                left_func = [comp_neq_f, left_func, right_func](const QMap<QString, QStringList>& data_rows) {
                    //return neq(left_func(data_rows), right_func(data_rows));
                    return comp_neq_f(left_func(data_rows), right_func(data_rows));
                    };
                
            }
            else if (get_current_term().get_token().token_type == TokenType::LESSTHAN) {
                //left = lt(left, cond_primary(data_rows, true));

                auto right_func = comp_cond_primary(data_rows, true);
                auto comp_lt_f = comp_lt(left_func(data_rows), right_func(data_rows));

                left_func  = [comp_lt_f, left_func, right_func](const QMap<QString, QStringList>& data_rows) {
                    //return lt(left_func(data_rows), right_func(data_rows));
                    return comp_lt_f(left_func(data_rows), right_func(data_rows));
                    };
                
            }
            else if (get_current_term().get_token().token_type == TokenType::GREATERTHAN) {
                //left = gt(left, cond_primary(data_rows, true));
                auto right_func = comp_cond_primary(data_rows, true);

                auto comp_gt_f = comp_gt(left_func(data_rows), right_func(data_rows));

                left_func = [comp_gt_f, left_func, right_func](const QMap<QString, QStringList>& data_rows) {
                    return comp_gt_f(left_func(data_rows), right_func(data_rows));
                    };
                
            }
            else if (get_current_term().get_token().token_type == TokenType::LESSTHANOREQUAL) {
                //left = le(left, cond_primary(data_rows, true));
                auto right_func = comp_cond_primary(data_rows, true);

                auto comp_le_f = comp_le(left_func(data_rows), right_func(data_rows));

                left_func = [comp_le_f, left_func, right_func](const QMap<QString, QStringList>& data_rows) {
                    return comp_le_f(left_func(data_rows), right_func(data_rows));
                    };
                
            }
            else if (get_current_term().get_token().token_type == TokenType::GREATERTHANOREQUAL) {
                //left = ge(left, cond_primary(data_rows, true));

                auto right_func = comp_cond_primary(data_rows, true);

                auto comp_ge_f = comp_ge(left_func(data_rows), right_func(data_rows));

                left_func = [comp_ge_f, left_func, right_func](const QMap<QString, QStringList>& data_rows) {
                    return comp_ge_f(left_func(data_rows), right_func(data_rows));
                    };
                
            }
            else if (get_current_term().get_token().token_type == TokenType::LIKE) {
                //qDebug() << "evaluating like!";
                //left = like(left, cond_primary(data_rows, true));

                auto right_func = comp_cond_primary(data_rows, true);
                auto comp_like_f = comp_like(left_func(data_rows), right_func(data_rows));

                left_func = [comp_like_f, left_func, right_func](const QMap<QString, QStringList>& data_rows) {
                    return comp_like_f(left_func(data_rows), right_func(data_rows));
                    };
                
            }
            else if (get_current_term().get_token().token_type == TokenType::NOTLIKE) {
                //qDebug() << "evaluating not like!";
                //left = not_like(left, cond_primary(data_rows, true));

                auto right_func = comp_cond_primary(data_rows, true);
                auto comp_notlike_f = comp_not_like(left_func(data_rows), right_func(data_rows));

                left_func = [comp_notlike_f, left_func, right_func](const QMap<QString, QStringList>& data_rows) {
                    return comp_notlike_f(left_func(data_rows), right_func(data_rows));
                    };
                
            }
            else {
                
                break;
            }
        }
        
        return left_func;
    }

    std::function<Term(const QMap<QString, QStringList>& data_rows)> ConditionalExpression::comp_cond_primary(const QMap<QString, QStringList> data_rows, bool get)
    {
        if (get) {
            move_to_next_term();
        }
        Term left = get_current_term();

        std::function<Term(const QMap<QString, QStringList>& data_rows)> left_func;

        ////std::cout<<"cond_primary() current left: "<<left.get_token().to_string().toStdString()<<"\n";

        if (left.get_token().token_type == TokenType::LBRACKET) {
            //auto e = cond_expr(row, true);
            //left = e;
            move_to_next_term(); //eat (
            //std::cout<<" token after ( is "<< current_term->get_token().to_string().toStdString()<<"\n";
            auto ts = read_cond_expression();
            ConditionalExpression ce(ts);
            //Term t = ce.eval(data_rows);

            left_func = ce.compile(data_rows);

            if (current_term->get_token().token_type != TokenType::RBRACKET) {
                //std::cout<<"next token "<<current_term->get_token().to_string().toStdString()<<"\n";
                QString error = "Expected a ) on line " + QString::number(current_term->get_token().line_number);
                throw std::logic_error(error.toStdString());
            }
            else {
                //left = e;

                //std::cout<<"ce result="<<t.get_token().to_string().toStdString()<<" {"<<t.get_token().string_value.toStdString()<<"}\n";

               // left = t;
                move_to_next_term(); //eat )
                //return f;
            }
        }
        //else if(left.get_token().token_type == TokenType::RBRACKET){
            //move_to_next_term();
        //}
        else {
            //left = expr(row, get);
            //move_to_next_term();
            //std::cout<<"Calling read_expression()\n";
            Expression e = read_expression();
            //std::cout<<"evaluating left side of conditional expression\n";
            //left = e.eval(data_rows);
            left_func = e.compile(data_rows);
            //std::cout<<"ex result="<<left.get_token().to_string().toStdString()<<" {"<<left.get_token().string_value.toStdString()<<"}\n";
            //move_to_next_term();
            //std::cout<<"done.\n";
        }

        return left_func;
    }

    std::function<Term(const QMap<QString, QStringList>& data_rows)> ConditionalExpression::compile(const QMap<QString, QStringList> data_rows)
    {
        //Token t = { .token_type = TokenType::END, .token_name = "TokenType::END" };
        //Term result(t);

        //std::cout<<"ConditionalExpression::eval(): cond_expr()\n";
        //qDebug() << "compiling conditional";
        //result = cond_expr(data_rows, false);
        auto result_func = comp_cond_expr(data_rows, false);
        //std::cout<<"done.\n";

        if (get_current_term().get_token().token_type != TokenType::END) {
            QString error = "Incorrect syntax in conditional expression '";
            error += get_current_term().get_token().string_value;
            error += "' on line ";
            error += QString::number(get_current_term().get_token().line_number);
            throw std::logic_error(error.toStdString());
        }

        //reset iterators
        reset_iterators();


        return result_func;
    }

    std::function < Term(const Term&, const Term&)> ConditionalExpression::comp_or_op(const Term& left, const Term& right)
    {
        std::function < Term(const Term&, const Term&)> result_func;
        

        if (left.get_token().token_type == TokenType::NUMBER) {
            if (right.get_token().token_type != TokenType::NUMBER) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }

        }
        else if (left.get_token().token_type == TokenType::STRING) {
            if (right.get_token().token_type != TokenType::STRING) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a string!";
                throw std::logic_error(error.toStdString());
            }
        }
        else if (left.get_token().token_type == TokenType::BOOLEAN) {
            if (right.get_token().token_type != TokenType::BOOLEAN) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a boolean!";
                throw std::logic_error(error.toStdString());
            }
        }

        result_func = [](const Term& left, const Term& right) {

            Term result;
            Token t;
            t.token_type = TokenType::BOOLEAN;
            t.boolean_value = left.get_token().boolean_value || right.get_token().boolean_value;
            result = Term(t);

            return result;

            };

        return result_func;
    }

    std::function < Term(const Term&, const Term&)> ConditionalExpression::comp_and_op(const Term& left, const Term& right)
    {
        std::function < Term(const Term&, const Term&)> result_func;
        

        if (left.get_token().token_type == TokenType::BOOLEAN) {
            if (right.get_token().token_type != TokenType::BOOLEAN) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a boolean!";
                throw std::logic_error(error.toStdString());
            }

            result_func = [](const Term& left, const Term& right) {
                Term result;
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = left.get_token().boolean_value && right.get_token().boolean_value;
                result = Term(t);

                return result;
                };

            
        }
        else {
            QString error = "Left operand of AND on line ";
            error += QString::number(right.get_line_number());
            error += " should be a boolean!";
            throw std::logic_error(error.toStdString());
        }

        return result_func;
    }

    std::function < Term(const Term&, const Term&)> ConditionalExpression::comp_eq(const Term& left, const Term& right)
    {
        std::function < Term(const Term&, const Term&)> result_func;
        Term result;

        Token t;
        t.token_type = TokenType::BOOLEAN;

        if (left.get_token().token_type == TokenType::NUMBER) {
            if (right.get_token().token_type != TokenType::NUMBER) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }

            result_func = [](const Term& left, const Term& right) {
                Term result;
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = left.get_token().number_value == right.get_token().number_value;
                result = Term(t);
                return result;

                };
            //t.boolean_value = left.get_token().number_value == right.get_token().number_value;
        }
        else if (left.get_token().token_type == TokenType::STRING) {
            if (right.get_token().token_type != TokenType::STRING) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a string!";
                throw std::logic_error(error.toStdString());
            }

            result_func = [](const Term& left, const Term& right) {
                Term result;
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = left.get_token().string_value.toLower() == right.get_token().string_value.toLower();
                result = Term(t);
                return result;

                };

            //t.boolean_value = left.get_token().string_value.toLower() == right.get_token().string_value.toLower();
        }
        else if (left.get_token().token_type == TokenType::BOOLEAN) {
            if (right.get_token().token_type != TokenType::BOOLEAN) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a boolean!";
                throw std::logic_error(error.toStdString());
            }
            //t.boolean_value = left.get_token().boolean_value == right.get_token().boolean_value;

            result_func = [](const Term& left, const Term& right) {
                Term result;
                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = left.get_token().boolean_value == right.get_token().boolean_value;
                result = Term(t);
                return result;

                };
        }
        else {
            QString error = "Unexpected left operand on line ";
            error += QString::number(right.get_line_number());
            throw std::logic_error(error.toStdString());
        }

        //result = Term(t);

        return result_func;
    }

    std::function < Term(const Term&, const Term&)> ConditionalExpression::comp_neq(const Term& left, const Term& right)
    {
        std::function < Term(const Term&, const Term&)> result_func;
        Term result;

        Token t;
        t.token_type = TokenType::BOOLEAN;

        if (left.get_token().token_type == TokenType::NUMBER) {
            if (right.get_token().token_type != TokenType::NUMBER) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }
            //t.boolean_value = left.get_token().number_value != right.get_token().number_value;

            result_func = [](const Term& left, const Term& right) {
                Term result;

                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = left.get_token().number_value != right.get_token().number_value;
                result = Term(t);
                return result;
                };
        }
        else if (left.get_token().token_type == TokenType::STRING) {
            if (right.get_token().token_type != TokenType::STRING) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a string!";
                throw std::logic_error(error.toStdString());
            }

            //t.boolean_value = left.get_token().string_value.toLower() != right.get_token().string_value.toLower();
            result_func = [](const Term& left, const Term& right) {
                Term result;

                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = left.get_token().string_value.toLower() != right.get_token().string_value.toLower();
                result = Term(t);
                return result;
                };
        }
        else if (left.get_token().token_type == TokenType::BOOLEAN) {
            if (right.get_token().token_type != TokenType::BOOLEAN) {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a boolean!";
                throw std::logic_error(error.toStdString());
            }
            //t.boolean_value = left.get_token().boolean_value != right.get_token().boolean_value;
            result_func = [](const Term& left, const Term& right) {
                Term result;

                Token t;
                t.token_type = TokenType::BOOLEAN;
                t.boolean_value = left.get_token().boolean_value != right.get_token().boolean_value;
                result = Term(t);
                return result;
                };
        }
        else {
            QString error = "Unexpected left operand on line ";
            error += QString::number(right.get_line_number());
            throw std::logic_error(error.toStdString());
        }

        //result = Term(t);

        return result_func;
    }

    std::function < Term(const Term&, const Term&)> ConditionalExpression::comp_gt(const Term& left, const Term& right)
    {
        std::function < Term(const Term&, const Term&)> result_func;
        Term result;
        if (left.get_token().token_type == TokenType::NUMBER) {

            if (right.get_token().token_type == TokenType::NUMBER) {
                

                result_func = [](const Term& left, const Term& right) {
                    Term result;
                    Token t;
                    t.token_type = TokenType::BOOLEAN;
                    t.boolean_value = left.get_token().number_value > right.get_token().number_value;
                    result = Term(t);

                    return result;
                    };
            }
            else {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }
        }
        else {
            QString error = "Left operand on line ";
            error += QString::number(left.get_line_number());
            error += " should be a number!";
            throw std::logic_error(error.toStdString());
        }

        return result_func;
    }

    std::function < Term(const Term&, const Term&)> ConditionalExpression::comp_lt(const Term& left, const Term& right)
    {
        std::function < Term(const Term&, const Term&)> result_func;
        
        if (left.get_token().token_type == TokenType::NUMBER) {

            if (right.get_token().token_type == TokenType::NUMBER) {
                

                result_func = [](const Term& left, const Term& right) {
                    Term result;

                    Token t;
                    t.token_type = TokenType::BOOLEAN;
                    t.boolean_value = left.get_token().number_value < right.get_token().number_value;
                    result = Term(t);

                    return result;

                    };
            }
            else {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }
        }
        else {
            QString error = "Left operand on line ";
            error += QString::number(left.get_line_number());
            error += " should be a number!";
            throw std::logic_error(error.toStdString());
        }

        return result_func;
    }

    std::function < Term(const Term&, const Term&)> ConditionalExpression::comp_ge(const Term& left, const Term& right)
    {
        std::function < Term(const Term&, const Term&)> result_func;
        
        if (left.get_token().token_type == TokenType::NUMBER) {

            if (right.get_token().token_type == TokenType::NUMBER) {
                

                result_func = [](const Term& left, const Term& right){
                    Term result;
                    Token t;
                    t.token_type = TokenType::BOOLEAN;
                    t.boolean_value = left.get_token().number_value >= right.get_token().number_value;
                    result = Term(t);

                    return result;
                    };
            }
            else {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }
        }
        else {
            QString error = "Left operand on line ";
            error += QString::number(left.get_line_number());
            error += " should be a number!";
            throw std::logic_error(error.toStdString());
        }

        return result_func;
    }

    std::function < Term(const Term&, const Term&)> ConditionalExpression::comp_le(const Term& left, const Term& right)
    {
        std::function < Term(const Term&, const Term&)> result_func;

        
        if (left.get_token().token_type == TokenType::NUMBER) {

            if (right.get_token().token_type == TokenType::NUMBER) {
                

                result_func = [](const Term& left, const Term& right) {
                    Term result;
                    Token t;
                    t.token_type = TokenType::BOOLEAN;
                    t.boolean_value = left.get_token().number_value <= right.get_token().number_value;
                    result = Term(t);

                    return result;
                    };
            }
            else {
                QString error = "Right operand on line ";
                error += QString::number(right.get_line_number());
                error += " should also be a number!";
                throw std::logic_error(error.toStdString());
            }
        }
        else {
            QString error = "Left operand on line ";
            error += QString::number(left.get_line_number());
            error += " should be a number!";
            throw std::logic_error(error.toStdString());
        }

        return result_func;
    }

    std::function < Term(const Term&, const Term&)> ConditionalExpression::comp_like(const Term& left, const Term& right)
    {
        std::function < Term(const Term&, const Term&)> result_func;
        

        if (left.get_token().token_type != TokenType::STRING || right.get_token().token_type != TokenType::STRING) {
            QString error = "Like operator expects String operands on line ";
            error += QString::number(right.get_line_number());
            //error += " should also be a number!";
            throw std::logic_error(error.toStdString());
        }

        result_func = [](const Term& left, const Term& right) {
            Term result;

            Token t;
            t.token_type = TokenType::BOOLEAN;

            QString regexPattern;
            QString pattern = right.get_token().string_value;
            regexPattern.reserve(pattern.size() * 2);

            // Escape regex special characters, then replace SQL wildcards
            for (QChar c : pattern) {
                switch (c.unicode()) {
                case '%': regexPattern += ".*"; break;
                case '_': regexPattern += ".";  break;
                    // Escape regex metacharacters
                case '.': case '^': case '$': case '|': case '(': case ')':
                case '[': case ']': case '{': case '}': case '+': case '?': case '\\':
                    regexPattern += '\\';
                    regexPattern += c;
                    break;
                default:
                    regexPattern += c;
                    break;
                }
            }

            // Add anchors to match the whole string
            regexPattern.prepend('^');
            regexPattern.append('$');

            QRegularExpression regex(regexPattern, QRegularExpression::CaseInsensitiveOption);
            t.boolean_value = regex.match(left.get_token().string_value).hasMatch();

            result = Term(t);

            return result;

            
            };

        

        return result_func;
    }

    std::function < Term(const Term&, const Term&)> ConditionalExpression::comp_not_like(const Term& left, const Term& right)
    {
        
        auto comp_f = comp_like(left, right);

        std::function < Term(const Term&, const Term&)> result_func = [comp_f](const Term& left, const Term& right) {
            Term result = comp_f(left, right);
            Token t = result.get_token();
            t.boolean_value = !t.boolean_value; // reverse boolean result from like test
            result = Term(t);

            return result;
            };

        return result_func;
    }
}

