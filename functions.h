#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Types.h"
#include "term.h"

/**
* Steps to add a new function:
* 1. create a function prototype that takes in a list of terms and returns a term in this header file
* 2. implement the function in the corresponding .cpp file
* 3. add the function to the list of built-in functions in the set_builtin_funcs() function in main.cpp
* 4. create a compile function prototype that takes in a list of terms and returns a function that takes in a data row and returns a term
* 5. implement the compile function in the corresponding .cpp file
* 6. Add function name to list of commands in the types.cpp file; This is for the tab completion in the CLI 
* 7. Add function name to list of keywords in main.cpp; This is used for syntax highlighting in the CLI
*/

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

	Term modulo(QList<Term>); // modulo of 2 numbers

	Term case_function(QList<Term>); // handles the CASE switch logic for constant THEN values and a variable or constant CASE key


	

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
	std::function<Term(const QList<Term>&)> comp_trim(const QList<Term>&);

	std::function<Term(const QList<Term>&)> comp_length(const QList<Term>&);

	std::function<Term(const QList<Term>&)> comp_substring(const QList<Term>&);

	std::function<Term(const QList<Term>&)> comp_left(const QList<Term>&);

	std::function<Term(const QList<Term>&)> comp_right(const QList<Term>&);

	std::function<Term(const QList<Term>&)> comp_strip_quotes(const QList<Term>&); //remove doubles from column value surrounded by double quotes

	std::function<Term(const QList<Term>&)> comp_date_gt(const QList<Term>&); // date greater then
															 
	std::function<Term(const QList<Term>&)> comp_date_lt(const QList<Term>&); // date less than
															 
	std::function<Term(const QList<Term>&)> comp_date_ge(const QList<Term>&); // date greater than or equal
															 
	std::function<Term(const QList<Term>&)> comp_date_le(const QList<Term>&); // date less than or equal
															 
	std::function<Term(const QList<Term>&)> comp_date_eq(const QList<Term>&); // date equal

	std::function<Term(const QList<Term>&)> comp_number(const QList<Term>&); // convert string to number or throw exception

	std::function<Term(const QList<Term>& args)> comp_modulo(const QList<Term>& args);

	std::function<Term(const QList<Term>&)> comp_case_function(const QList<Term>& args); // handles the CASE switch logic for constant THEN values and a variable or constant CASE key


	//enum class AggregFuncType { count, sum, min, max, avg };
	std::function<Term(const QList<Term>& args)> comp_execute_aggregation(const QList<Term>& args, const AggregFuncType& func_type);

	std::function<Term(const QList<Term>&)> comp_count(const QList<Term>&); //aggregate function

	std::function<Term(const QList<Term>&)> comp_sum(const QList<Term>&); //aggregate function
								  
	std::function<Term(const QList<Term>&)> comp_min(const QList<Term>&); //aggregate function
								  
	std::function<Term(const QList<Term>&)> comp_max(const QList<Term>&); //aggregate function
								  
	std::function<Term(const QList<Term>&)> comp_avg(const QList<Term>&); //aggregate function

}

#endif // FUNCTIONS_H
