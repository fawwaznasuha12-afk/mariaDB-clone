# MariaDB Clone

A self-contained SQL database engine implemented in C++17 — a lightweight, educational RDBMS that supports SQL queries, transactions, user management, and data persistence.

---

## Overview

MariaDB Clone is a full-featured relational database management system built from scratch in a single C++ file. It demonstrates core database concepts including:

- SQL parsing and execution
- Data persistence to disk
- Transaction support with rollback
- User authentication and privileges
- ANSI color-coded terminal output
- Import/export functionality

---

## Features

### Database Operations
- Create, drop, and use databases
- Show all databases
- Schema management

### Table Management
- Create tables with column definitions (INT, VARCHAR, TEXT, DATE, etc.)
- Drop and truncate tables
- Alter tables (add, drop, modify columns)
- Rename tables
- Show table structure (DESCRIBE)
- Create and drop indexes
- Show indexes from table

### Data Manipulation
- INSERT, SELECT, UPDATE, DELETE
- WHERE clause with AND, OR, LIKE, IN, BETWEEN, IS NULL, comparisons
- ORDER BY (ASC/DESC)
- LIMIT with OFFSET
- GROUP BY with HAVING
- DISTINCT
- Aggregate functions: COUNT, SUM, AVG, MIN, MAX, STD, VARIANCE
- Bitwise aggregates: BIT_AND, BIT_OR, BIT_XOR

### Joins
- INNER JOIN
- LEFT JOIN
- RIGHT JOIN
- CROSS JOIN

### Transactions
- START TRANSACTION / BEGIN
- COMMIT
- ROLLBACK
- SAVEPOINT

### User Management
- CREATE USER, DROP USER, RENAME USER
- SET PASSWORD
- GRANT, REVOKE
- SHOW GRANTS, SHOW PRIVILEGES
- FLUSH PRIVILEGES

### System Functions
- USER(), CURRENT_USER(), SYSTEM_USER(), SESSION_USER()
- VERSION(), DATABASE(), SCHEMA()
- NOW(), CURDATE(), CURTIME(), CURRENT_TIMESTAMP
- UNIX_TIMESTAMP(), FROM_UNIXTIME()
- DATE_ADD(), DATE_SUB(), DATEDIFF(), TIMEDIFF()
- DATE_FORMAT(), TIME_FORMAT()
- CONCAT(), CONCAT_WS()
- UPPER(), LOWER(), LENGTH(), SUBSTRING()
- TRIM(), REPLACE(), RAND()

### Utility Commands
- SHOW PROCESSLIST, KILL
- SHOW VARIABLES, SHOW STATUS
- SHOW ENGINE INNODB STATUS
- EXPORT table TO 'file.csv' [WITH HEADER]
- IMPORT table FROM 'file.csv' [WITH HEADER]
- BACKUP 'filename', RESTORE 'filename'
- STATUS, HELP, CLEAR
- SOURCE 'script.sql'

---

## Quick Start

### Compilation

```bash
g++ -std=c++17 -o mariadb database.cpp
```

### Running the Server

```bash
./mariadb
```

### Login

The default root password is `root`.

```
Enter password: root
```

### Basic Usage

```sql
-- Show all databases
SHOW DATABASES;

-- Create and use a database
CREATE DATABASE mydb;
USE mydb;

-- Create a table
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(255) UNIQUE,
    created_at DATETIME
);

-- Insert data
INSERT INTO users VALUES (1, 'John Doe', 'john@example.com', NOW());

-- Query data
SELECT * FROM users WHERE name LIKE '%John%';

-- Update data
UPDATE users SET email = 'john.doe@example.com' WHERE id = 1;

-- Delete data
DELETE FROM users WHERE id = 1;

-- Export data
EXPORT users TO 'users.csv' WITH HEADER;

-- Exit
EXIT;
```

---

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     CLI Interface                           │
│              (Interactive SQL shell)                        │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    SQL Parser & Executor                    │
│         (Command parsing, execution routing)                │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    Query Engine                             │
│    (WHERE evaluation, ORDER BY, GROUP BY, LIMIT, JOIN)     │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                  Transaction Manager                        │
│            (BEGIN, COMMIT, ROLLBACK)                        │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    Storage Layer                            │
│          (In-memory tables + disk persistence)              │
└─────────────────────────────────────────────────────────────┘
```

---

## Data Types Supported

| Type | Description |
|------|-------------|
| INT, INTEGER | 32-bit integer |
| TINYINT, SMALLINT, MEDIUMINT, BIGINT | Integer variants |
| VARCHAR(n) | Variable-length string up to n characters |
| CHAR(n) | Fixed-length string up to n characters |
| TEXT, TINYTEXT, MEDIUMTEXT, LONGTEXT | Text strings |
| DECIMAL, NUMERIC | Fixed-point numbers |
| FLOAT, DOUBLE | Floating-point numbers |
| BOOLEAN, BOOL | Boolean (0/1, true/false) |
| DATE | Date (YYYY-MM-DD) |
| TIME | Time (HH:MM:SS) |
| DATETIME, TIMESTAMP | Date and time (YYYY-MM-DD HH:MM:SS) |
| YEAR | Year (1900-2155) |
| JSON | JSON object or array |
| UUID | UUID format |
| BLOB, LONGBLOB | Binary data |
| ENUM, SET | Enumeration/Set |
| BIT | Bit value (0/1) |

---

## Persistence

Data is automatically saved to `mariadb_data.sql` in the current directory. The file is human-readable and contains:

- Database schemas
- Table definitions
- Row data
- Column attributes (PRIMARY KEY, AUTO_INCREMENT, UNIQUE, NOT NULL)

The database loads this file on startup, so your data persists between sessions.

---

## Backup and Restore

```sql
-- Backup entire database
BACKUP 'mybackup';

-- Restore from backup
RESTORE 'mybackup';
```

Backups are saved as `.sql` files in the current directory.

---

## Export and Import CSV

```sql
-- Export table to CSV with headers
EXPORT users TO 'users.csv' WITH HEADER;

-- Export without headers
EXPORT users TO 'users.csv';

-- Import from CSV with headers
IMPORT users FROM 'users.csv' WITH HEADER;

-- Import without headers
IMPORT users FROM 'users.csv';
```

---

## Transaction Example

```sql
START TRANSACTION;

INSERT INTO accounts VALUES (1, 'Alice', 1000);
INSERT INTO accounts VALUES (2, 'Bob', 500);

UPDATE accounts SET balance = balance - 200 WHERE id = 1;
UPDATE accounts SET balance = balance + 200 WHERE id = 2;

-- If everything looks correct
COMMIT;

-- Or undo all changes
ROLLBACK;
```

---

## User Management Example

```sql
CREATE USER 'alice'@'localhost' IDENTIFIED BY 'secret';
GRANT ALL ON *.* TO 'alice'@'localhost';
SHOW GRANTS FOR 'alice'@'localhost';
SET PASSWORD FOR 'alice'@'localhost' = PASSWORD('newpass');
DROP USER 'alice'@'localhost';
```

---

## Color Output

The terminal output uses ANSI 256-color codes for better readability:

- Teal: Borders and structural elements
- Purple: Headers and column names
- Orange: Numeric values
- White: Regular text
- Gray: NULL values and dim text
- Green: Success messages
- Red: Error messages
- Yellow: Warnings

---

## Limitations

This is an educational implementation and has the following limitations:

- Single-user, single-threaded (no concurrent connections)
- No network support (local only)
- No full ACID compliance (basic transaction support only)
- Limited query optimization
- No foreign key constraints
- No stored procedures or triggers
- No views
- No full-text search
- CSV import/export is basic

---

## File Structure

```
database.cpp          # Single-file implementation
mariadb_data.sql      # Persistent data storage
mybackup.sql          # Backup files
users.csv             # Exported CSV files
```

---

## License

MIT License

---

## Author

fawwaznasuha12-afk

---

## Contributing

This is a personal learning project, but contributions are welcome. Feel free to fork and submit pull requests.

---

## Disclaimer

This software is provided "as is" without warranty of any kind. It is intended for educational purposes and should not be used in production environments where data integrity and security are critical.
