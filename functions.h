#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Types.h"

ColumnResult trim(QList<TokenType>, QList<std::any>);

ColumnResult length(QList<TokenType>, QList<std::any>);

ColumnResult substring(QList<TokenType>, QList<std::any>);

ColumnResult left(QList<TokenType>, QList<std::any>);

ColumnResult right(QList<TokenType>, QList<std::any>);

ColumnResult date_gt(QList<TokenType>, QList<std::any>);

ColumnResult date_lt(QList<TokenType>, QList<std::any>);

ColumnResult date_ge(QList<TokenType>, QList<std::any>); //greater than or equal

ColumnResult date_le(QList<TokenType>, QList<std::any>); //less than or equal

ColumnResult date_eq(QList<TokenType>, QList<std::any>);




#endif // FUNCTIONS_H
