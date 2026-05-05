# CSVQuery

> A lightweight SQL-like language for querying and manipulating CSV files directly — no database required.

---

## ✨ Overview

**CSVQuery** is a scripting language designed to make working with CSV files simple, fast, and familiar.

It combines the power of SQL with the simplicity of flat files, allowing you to:
- Query CSV data
- Perform transformations
- Join multiple files
- Export results

---

## 🚀 Features

- SQL-like syntax (easy to learn)
- No database setup required
- Works directly on CSV files
- Supports joins, grouping, and aggregation
- Built-in CLI with:
  - Syntax highlighting
  - Tab completion
  - Command history

---

## 📦 Installation

### Option 1: Download Binary
1. Download the latest release
2. Extract files
3. Run:

```bash
csvq.exe
```

---

### Option 2: Add to PATH
Add the executable directory to your system PATH:

```bash
set PATH=%PATH%;C:\path\to\csvquery
```

---

## ⚡ Quick Start

### 1. Prepare CSV File

```csv
1,John,Doe
2,Jane,Smith
3,Bob,Brown
```

---

### 2. Run a Query

```sql
select * from "users.csv";
```

---

### 3. Save Output

```sql
select * from "users.csv" into "output.csv";
```

---

## 🧠 Core Concepts

### Statements

| Statement | Description |
|----------|------------|
| IMPORT | Load column definitions |
| ASSIGNMENT | Store values in variables |
| SELECT | Query data |
| UPDATE | Modify data |

---

### Column Access

- Index-based: `[0]`, `[1]`
- Named columns: `users.id`
- Wildcard: `*`

---

## 📘 Examples

### Basic Query

```sql
select [0], [1] from "users.csv";
```

---

### Using Column Definitions

```sql
import "users.def" as defs;
users = "users.csv" with defs;

select users.id, users.first_name from users;
```

---

### Expressions

```sql
select 12 + 3 * 8 from "users.csv";
```

---

### Aggregation

```sql
select count(*) from "users.csv";
```

---

### Update Data

```sql
update users
set [1] = 'Araba'
where [0] = '2'
into "updated.csv";
```

---

### Join Example

```sql
select u.*, s.*
from users u
inner join salaries s
on u.0 = s.0;
```

---

## 🧩 Supported Functions

### String Functions
- `Trim()`
- `Length()`
- `Substring()`
- `Left()`
- `Right()`
- `Strip_quotes()`
- `Number()`

---

### Date Functions
- `Date_gt`
- `Date_ge`
- `Date_lt`
- `Date_le`
- `Date_eq`

---

### Aggregate Functions
- `Max()`
- `Min()`
- `Avg()`
- `Sum()`
- `Count()`

---

## 🖥️ CLI Preview

```
csvq> select * from users;

+----+----------+-----------+
| id | first    | last      |
+----+----------+-----------+
| 1  | John     | Doe       |
| 2  | Jane     | Smith     |
+----+----------+-----------+
```

---

## 📸 Screenshots

### CLI Interface
![CLI Screenshot](docs/images/cli.png)

### Query Example
![Query Screenshot](docs/images/query.png)

> Add your screenshots to `docs/images/` directory.

---

## 📝 Notes

- Case-insensitive language
- Column index starts at 0
- `INTO` is required for UPDATE
- Supports comments using `#`

---

## 🔐 Keywords

```
IMPORT, AS, SELECT, FROM, INNER JOIN, OUTER JOIN,
ON, WHERE, AND, OR, LIMIT, GROUP BY, HAVING, INTO,
TRIM, LEFT, RIGHT, SUBSTRING, LENGTH, NUMBER, STRIP_QUOTES,
DATE_EQ, DATE_GT, DATE_GE, DATE_LT, DATE_LE,
MAX, MIN, AVG, COUNT, SUM
```

---

## 🛣️ Roadmap

- Custom delimiters (non-comma support)
- Performance optimizations
- More SQL compatibility
- Extended function library

---

## 🤝 Contributing

Contributions are welcome!

1. Fork the repo
2. Create a feature branch
3. Submit a PR

---

## 📄 License

MIT License

---

## 👤 Author

Kwame Yeboah-Gyan

---

## ⭐ Support

If you find this useful, consider giving it a star ⭐ on GitHub!
