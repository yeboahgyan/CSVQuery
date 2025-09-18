#ifndef IMPORTSTATEMENT_H
#define IMPORTSTATEMENT_H

#include "Types.h"

class ImportStatement
{
    const QList<Token> tokens;
    QList<Token>::const_iterator last_token_pos;
    QString def_alias;

    void throw_exception_if_unexpected_end();

    void read_def_file(const QString& file, const QString& alias); //reads def columns and saves in import_defs table


public:
    ImportStatement(const QList<Token>& tks);

    void execute();

};

#endif // IMPORTSTATEMENT_H
