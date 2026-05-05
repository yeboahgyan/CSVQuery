// CSVQuery - An SQL-like query language for CSV files
// Copyright (c) 2025-2026 Kwame Yeboah-Gyan
// Distributed under the MIT License.

#include "importstatement.h"
#include <stdexcept>
#include <QFile>
#include <QDebug>

namespace csvquery {

    ImportStatement::ImportStatement(const QList<Token>& tks)
        :tokens{ tks }
    {
        last_token_pos = tokens.begin();
    }


    void ImportStatement::throw_exception_if_unexpected_end()
    {
        if (last_token_pos == tokens.cend()) {
            --last_token_pos; //get last but one token
            double line_numer = (*last_token_pos).line_number;
            QString str_num = QString::number(line_numer);
            QString last_token = last_token_pos->string_value;

            throw std::logic_error("Unexpected end to Import statement on line " + str_num.toStdString() + " after '" + last_token.toStdString() + "'");
        }
    }

    void ImportStatement::read_def_file(const QString& file, const QString& alias)
    {
        QFile f(file);

        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::string error = "Failed to open file provided for import statement on line ";
            error += last_token_pos->line_number;

            throw std::logic_error(error);
        }

        while (!f.atEnd()) {
            QByteArray line = f.readLine();
            line = line.trimmed();
            if (line == "") {
                continue;
            }
            ++NUMBER_OF_COLUMN_NAMES;
            import_defs[alias].append(line.toLower());
        }

    }


    void ImportStatement::execute()
    {
        //eat import
        ++last_token_pos; //next token

        throw_exception_if_unexpected_end();
        //qDebug()<<"ate import\n";

        if (last_token_pos->token_type != TokenType::STRING) {
            std::string error = "Expected a string in import statement on line ";
            error += last_token_pos->line_number;

            throw std::logic_error(error);
        }

        QString file_name = last_token_pos->string_value;

        ++last_token_pos; //next token
        throw_exception_if_unexpected_end();

        if (last_token_pos->token_type != TokenType::AS) {
            std::string error = "Invalid syntax in import statement on line ";
            error += last_token_pos->line_number;

            throw std::logic_error(error);
        }

        ++last_token_pos; //next token
        throw_exception_if_unexpected_end();

        if (last_token_pos->token_type != TokenType::NAME) {
            std::string error = "Invalid syntax in import statement on line ";
            error += last_token_pos->line_number;

            throw std::logic_error(error);
        }

        QString alias = last_token_pos->string_value.toLower();
        
        //strings_table[alias] = file_name;
        //symbol_table[alias] = TokenType::IMPORT;

        read_def_file(file_name, alias);

        symbol_table[alias] = TokenType::IMPORT;
    }

}