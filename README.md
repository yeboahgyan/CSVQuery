# CSVQuery

CSVQuery is an SQL-like scripting language designed to simplify querying and manipulating data stored in CSV files. It provides a familiar SQL-inspired syntax tailored specifically for CSV data, without the overhead of importing data into a full database system.

---

## Table of Contents

- [CSVQuery](#csvquery)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Key Features](#key-features)
  - [Getting Started](#getting-started)
    - [Step 1: Importing Column Definitions](#step-1-importing-column-definitions)
    - [Step 2: Associating Definitions with a CSV File](#step-2-associating-definitions-with-a-csv-file)
    - [Step 3: Selecting Data](#step-3-selecting-data)
    - [Step 4: Using Expressions](#step-4-using-expressions)
    - [Step 5: Assigning Variables](#step-5-assigning-variables)
    - [Step 6: Updating Data](#step-6-updating-data)
    - [Step 7: Direct CSV Queries](#step-7-direct-csv-queries)
    - [Step 8: Join Queries](#step-8-join-queries)
    - [Step 9: CASE Expressions](#step-9-case-expressions)
  - [Statements](#statements)
  - [Supported Functions](#supported-functions)
    - [String Functions](#string-functions)
    - [Mathematical Functions](#mathematical-functions)
    - [Date Comparison Functions](#date-comparison-functions)
    - [Aggregate Functions](#aggregate-functions)
  - [Notes](#notes)
  - [Keywords](#keywords)
  - [Credits](#credits)

---

## Overview

Comma-separated values (CSV) is one of the most widely used formats for tabular data. Despite its ubiquity, querying CSV files can be inconvenient — common approaches involve importing data into a full database system or writing custom scripts, both of which can be unnecessarily complex for simple tasks.

CSVQuery solves this by providing a dynamic, SQL-inspired scripting language built specifically for CSV data. It supports four core statements: `IMPORT`, `ASSIGNMENT`, `SELECT`, and `UPDATE`. Every statement is terminated with a semicolon (`;`). The language is **case-insensitive**.

---

## Key Features

- **Column Access** — Columns are zero-indexed and accessible via square brackets (e.g., `[0]` for the first column), or via dot notation when using named variables (e.g., `d.0`).
- **Column Naming** — Define human-readable column names in a separate `.def` file and map them to CSV columns on import.
- **Wildcard Support** — Use `*` to select all columns (e.g., `select * from users_csv;`).
- **Expressions** — Supports basic arithmetic (e.g., `12 + 3 * (10 - 2)`) and aggregate functions with grouping.
- **String Handling** — Single or double quotes are interchangeable. Multiplying a string by zero or a negative number yields an empty string.
- **Comments** — Python-style `#` comments are supported and ignored during execution.
- **File Export** — Use the `INTO` clause to save query results to a new CSV file.
- **Join Queries** — Supports `INNER JOIN` and `OUTER JOIN` across multiple CSV files.
- **CLI** — A built-in command-line interface with tab completion, syntax highlighting, and command history.

---

## Getting Started

The examples below use a sample CSV file `d:\users.csv`:

```
1,Alice,Smith
2,Bob,Jones
3,Charlie,Brown
```

And a column definition file `c:\users.def`:

```
id
first_name
last_name
```

### Step 1: Importing Column Definitions

Load column name definitions from a `.def` file:

```sql
import "c:\users.def" as user_def;
-- Number of names loaded: 3
```

### Step 2: Associating Definitions with a CSV File

Link the imported definitions to a CSV file:

```sql
users_csv : user_def = "d:\users.csv";
```

You can now use `users_csv.id`, `users_csv.first_name`, and `users_csv.last_name` instead of `[0]`, `[1]`, and `[2]`.

### Step 3: Selecting Data

Query using column names:

```sql
select users_csv.id, users_csv.last_name from users_csv;
```

Using column indices:

```sql
select [0], [2] from users_csv;
```

```sql
select users_csv.0, users_csv.2 from users_csv;
```

Using wildcard to select all columns:

```sql
select * from users_csv;
```

```sql
select users_csv.* from users_csv;
```

Saving results to a file with the `INTO` clause:

```sql
select * from users_csv into 'd:\users_copy.csv';
```

### Step 4: Using Expressions

Perform calculations inside a `SELECT` statement:

```sql
select 12 + 3 * (10 - 2), count(*) from users_csv group by [0];
```

This evaluates `12 + 3 × 8 = 36` for each row and counts rows per id.

### Step 5: Assigning Variables

Assign values to variables for reuse:

```sql
a = 'a' * 2;  # assigns 'aa' to a
```

Use `SELECT` to display variable contents or evaluate expressions:

```sql
select a;
select 'Hurray!!! Ghana is ' + (2025 - 1957) + ' years old!';
```

### Step 6: Updating Data

Modify rows using an `UPDATE` statement (the `INTO` clause is **required**):

```sql
update users_csv set [1] = 'Araba', users_csv.2 = 'Awesome'
where users_csv.id = '2'
into 'd:\updated_users.csv';
```

This updates the row where `id = 2` and saves the result to a new file.

### Step 7: Direct CSV Queries

Query a CSV file directly without importing definitions:

```sql
select * from 'd:\users.csv';
```

### Step 8: Join Queries

**Inner Join** — returns only rows with matching keys in both files:

```sql
select users_csv.*, salaries.*
from users_csv inner join salaries
on users_csv.0 = salaries.0;
```

**Outer Join** — returns all rows from the first file, with empty values for unmatched rows from the second:

```sql
select users_csv.*, salaries.*
from users_csv outer join salaries
on users_csv.0 = salaries.0;
```

---

### Step 9: CASE Expressions
The language currently supports a simple CASE expression that takes an input expression but only supports a string or a number literal for the WHEN and ELSE clauses.

```sql
select case modulo(users_csv.id, 2)
when '0' then 'Even'
when '1' then 'Odd'
endcase
from users_csv;
```

```sql
select case users_csv.first_name
when 'Alice' then 'Hi Alice!'
else  'Hi!'
endcase
from users_csv;
```

---

## Statements

| Statement    | Description |
|--------------|-------------|
| `IMPORT`     | Loads column name definitions from a `.def` file. E.g., `import "c:\users.def" as user_def;` |
| `ASSIGNMENT` | Assigns a string or numeric expression to a variable. E.g., `a = 'a' * 2;` |
| `SELECT`     | Queries data from a CSV file. Supports column selection, expressions, grouping, and optional `INTO` export. |
| `UPDATE`     | Modifies rows based on an optional `WHERE` condition. Requires an `INTO` clause to define the output file. |

---

## Supported Functions

### String Functions

| Function | Description |
|----------|-------------|
| `Trim(string)` | Removes leading and trailing whitespace. E.g., `trim(' hello ')` |
| `Length(string)` | Returns the number of characters in the string. |
| `Substring(string, index, length)` | Returns a subset of the string starting at `index` with `length` characters. |
| `Left(string, length)` | Returns the first `length` characters of the string. |
| `Right(string, length)` | Returns the last `length` characters of the string. |
| `Strip_quotes(string)` | Removes double quotes from the beginning and end of the string. |
| `Number(string)` | Converts a string to a number; returns `0` if conversion fails. Useful since CSV data is stored as strings. |


### Mathematical Functions

| Function | Description |
|----------|-------------|
| `modulo(string, number) or modulo(string, string) or modulo(number, number)` |  Returns the modulo of two integers. |


### Date Comparison Functions

All date functions accept three arguments: `(date_string1, format, date_string2)`.

Example: `date_le("2020-12-12", "yyyy-MM-dd", "2023-01-12")`

| Function | Description |
|----------|-------------|
| `Date_gt` | Returns `true` if `date_string1` is greater than `date_string2`. |
| `Date_ge` | Returns `true` if `date_string1` is greater than or equal to `date_string2`. |
| `Date_lt` | Returns `true` if `date_string1` is less than `date_string2`. |
| `Date_le` | Returns `true` if `date_string1` is less than or equal to `date_string2`. |
| `Date_eq` | Returns `true` if `date_string1` is equal to `date_string2`. |

**Date Format Expressions:**

| Expression | Description |
|------------|-------------|
| `d` | Day without leading zero (1–31) |
| `dd` | Day with leading zero (01–31) |
| `ddd` | Abbreviated day name (Mon–Sun) |
| `dddd` | Full day name (Monday–Sunday) |
| `M` | Month without leading zero (1–12) |
| `MM` | Month with leading zero (01–12) |
| `MMM` | Abbreviated month name (Jan–Dec) |
| `MMMM` | Full month name (January–December) |
| `yy` | Two-digit year (00–99) |
| `yyyy` | Four-digit year |
| `hh:mm:ss.zzz` | Time with milliseconds (e.g., 14:13:09.120) |
| `h:m:s ap` | 12-hour time with am/pm (e.g., 2:13:9 pm) |

### Aggregate Functions

| Function | Description |
|----------|-------------|
| `Max(column)` | Maximum numeric value in a column. Non-numeric values are treated as zero. |
| `Min(column)` | Minimum numeric value in a column. Non-numeric values are treated as zero. |
| `Avg(column)` | Average of numeric values in a column. Non-numeric values are treated as zero. |
| `Sum(column)` | Sum of numeric values in a column. Non-numeric values are treated as zero. |
| `Count(column)` | Counts rows where the column is not empty. |
| `Count(*)` | Counts all rows. |

---

## Notes

- Ensure file paths are valid and accessible.
- Single and double quotes are interchangeable for strings.
- The `INTO` clause is optional for `SELECT` but **required** for `UPDATE`.
- Column indices start at `0` — `[0]` is the first column.
- Comments using `#` are ignored during execution.
- The language is **case-insensitive** (`select`, `Select`, and `SELECT` are equivalent).
- Scripts can be written in any text editor and run via the interpreter:
  ```
  csvq.exe my_script
  ```
- **Notepad++** users can enable syntax highlighting by importing the CSVQuery user-defined language XML from the `notepad++` folder in the CSVQuery installation directory.
- The **CSVQuery CLI** supports syntax highlighting, tab completion, and tabular display.

---

## Keywords

The following are reserved keywords and cannot be used as user-defined names:

```
IMPORT        AS            SELECT        FROM
INNER JOIN    OUTER JOIN    ON            WHERE
AND           OR            LIMIT         GROUP BY
HAVING        INTO          TRIM          LEFT
RIGHT         SUBSTRING     LENGTH        NUMBER
STRIP_QUOTES  DATE_EQ       DATE_GT       DATE_GE
DATE_LT       DATE_LE       MAX           MIN
AVG           COUNT         SUM           STRIP_QUOTES
CASE          WHEN          THEN          ELSE
ENDCASE       MODULO
```

---

## Credits

This project makes use of the following open-source libraries:

- [Qt Framework](https://www.qt.io/)
- [csv-parser](https://github.com/vincentlaucsb/csv-parser) by Vincent La
- [tabulate](https://github.com/p-ranav/tabulate) by p-ranav
- [Replxx](https://github.com/AmokHuginnsson/replxx) by AmokHuginnsson

---

*CSVQuery was designed and built by Kwame Yeboah-Gyan in late 2025.*
