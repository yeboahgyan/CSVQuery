// CSVQuery - An SQL-like query language for CSV files
// Copyright (c) 2025-2026 Kwame Yeboah-Gyan
// Distributed under the MIT License.

#ifndef IMPORTSTATEMENT_H
#define IMPORTSTATEMENT_H

#include "Types.h"

namespace csvquery {

    class ImportStatement
    {
        const QList<Token> tokens;
        QList<Token>::const_iterator last_token_pos;
        QString def_alias;

        unsigned int NUMBER_OF_COLUMN_NAMES = 0;

        void throw_exception_if_unexpected_end();

        void read_def_file(const QString& file, const QString& alias); //reads def columns and saves in import_defs table


    public:
        ImportStatement(const QList<Token>& tks);

        void execute();

        unsigned int num_of_columns_loaded() const {
            return NUMBER_OF_COLUMN_NAMES;
        }

    };

}

#endif // IMPORTSTATEMENT_H
