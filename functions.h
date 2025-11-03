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

	Term strip_quotes(QList<Term>); //remove doubles from column value surrounded by double quotes

	Term date_gt(QList<Term>); // date greater then

	Term date_lt(QList<Term>); // date less than

	Term date_ge(QList<Term>); // date greater than or equal

	Term date_le(QList<Term>); // date less than or equal

	Term date_eq(QList<Term>); // date equal

	//Term now(QList<Term>); // current date time

	//Term days_between();

	//Term _add();

	//Term date_sub();

	Term number(QList<Term>); // convert string to number or throw exception

	enum class AggregFuncType{ count, sum, min, max, avg};
	Term execute_aggregation(const QList<Term>& args, AggregFuncType func_type);

	Term count(QList<Term>); //aggregate function

	Term sum(QList<Term>); //aggregate function

	Term min(QList<Term>); //aggregate function

	Term max(QList<Term>); //aggregate function

	Term avg(QList<Term>); //aggregate function


	//compile equivalents functions
	std::function<Term(QList<Term>)> comp_trim(QList<Term>);

	std::function<Term(QList<Term>)> comp_length(QList<Term>);

	std::function<Term(QList<Term>)> comp_substring(QList<Term>);

	std::function<Term(QList<Term>)> comp_left(QList<Term>);

	std::function<Term(QList<Term>)> comp_right(QList<Term>);

	std::function<Term(QList<Term>)> comp_strip_quotes(QList<Term>); //remove doubles from column value surrounded by double quotes

	std::function<Term(QList<Term>)> comp_date_gt(QList<Term>); // date greater then

	std::function<Term(QList<Term>)> comp_date_lt(QList<Term>); // date less than

	std::function<Term(QList<Term>)> comp_date_ge(QList<Term>); // date greater than or equal

	std::function<Term(QList<Term>)> comp_date_le(QList<Term>); // date less than or equal

	std::function<Term(QList<Term>)> comp_date_eq(QList<Term>); // date equal

	std::function<Term(QList<Term>)> comp_number(QList<Term>); // convert string to number or throw exception

	//enum class AggregFuncType { count, sum, min, max, avg };
	std::function<Term(const QList<Term> args)> comp_execute_aggregation(const QList<Term> args, AggregFuncType func_type);

	std::function<Term(QList<Term>)> comp_count(QList<Term>); //aggregate function

	std::function<Term(QList<Term>)> comp_sum(QList<Term>); //aggregate function

	std::function<Term(QList<Term>)> comp_min(QList<Term>); //aggregate function

	std::function<Term(QList<Term>)> comp_max(QList<Term>); //aggregate function

	std::function<Term(QList<Term>)> comp_avg(QList<Term>); //aggregate function

}

#endif // FUNCTIONS_H
