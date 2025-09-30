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

	Term date_gt(QList<Term>);

	Term date_lt(QList<Term>);

	Term date_ge(QList<Term>); //greater than or equal

	Term date_le(QList<Term>); //less than or equal

	Term date_eq(QList<Term>);


}

#endif // FUNCTIONS_H
