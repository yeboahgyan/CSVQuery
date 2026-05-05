"# The CSVQuery Language" 
CSVQuery is an SQL-like scripting language designed to simplify querying 
and manipulating data stored in CSV files. It provides a familiar SQL
inspired syntax tailored specifically for CSV data. The language supports 
four core statements: IMPORT, ASSIGNMENT, SELECT, and UPDATE. 
In CSVQuery, a statement is a command executed by the interpreter, and 
every statement is terminated with a semicolon (;). The language enables 
users to define column names, perform queries, and update data 
efficiently, with support for saving results to new CSV files. It also includes 
basic arithmetic and string operations. 
Key Features 
• Column Access 
Columns are zero-indexed and can be accessed using square 
brackets (e.g., [0] for the first column). When working with variables 
in the FROM clause, columns can also be accessed using dot 
notation (e.g., d.0 in select d.*, d.0 from d;). 
For improved readability, column names can be defined in a separate 
f
ile and imported. 
• Statements 
o IMPORT: Loads column name definitions from a file (e.g., 
import "c:\users.def" as user_def;). These definitions can then 
be associated with a CSV file. 
o ASSIGNMENT: Assigns a string or numeric value to a variable 
based on an expression (e.g., a = 'a' * 2; assigns 'aa' to a). 
o SELECT: Queries data from a CSV file, supporting column 
selection, expressions, grouping, and exporting results using 
the INTO clause (e.g., select [0], [2], users_csv.3 from 
users_csv into 'c:\output.csv';). 
o UPDATE: Modifies data based on conditions specified in an 
optional WHERE clause. An INTO clause is required to define 
the output file (e.g., update users_csv set [1] = 'Araba' where [0] 
= '12' into 'c:\updated.csv';). 
• Column Naming 
Column names can be defined in a separate file (one per line) and 
mapped to CSV columns when imported. These names can then 
replace numeric indices in queries (e.g., users_csv.id instead of 
users_csv.0). 
• String Handling 
Strings may use single or double quotes. Multiplying a string by zero 
or a negative number results in an empty string (e.g., c = "hello" * 0; 
assigns '' to c). 
• Comments 
Python-style comments are supported using #. 
• Wildcard Support 
The * wildcard selects all columns (e.g., select * from users_csv; or 
select users_csv.* from users_csv;). 
• Expressions 
Supports basic arithmetic (e.g., 12 + 3 * (10 - 2)) and aggregate 
functions such as count(*), including grouping.