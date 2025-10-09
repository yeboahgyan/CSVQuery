#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Types.h"
#include "term.h"

namespace csvquery {

	Term trim(QList<Term>);

	Term length(QList<Term>);

	Term substring(QList<Term>);

	Term left(QList<Term>);

	Term right(QList<Term>);

	Term date_gt(QList<Term>); // date greater then

	Term date_lt(QList<Term>); // date less than

	Term date_ge(QList<Term>); // date greater than or equal

	Term date_le(QList<Term>); // date less than or equal

	Term date_eq(QList<Term>); // date equal

	Term number(QList<Term>); // convert string to number or throw exception

	Term count(QList<Term>); //aggregate function

	Term sum(QList<Term>); //aggregate function

	Term min(QList<Term>); //aggregate function

	Term max(QList<Term>); //aggregate function

	Term avg(QList<Term>); //aggregate function


}

#endif // FUNCTIONS_H
