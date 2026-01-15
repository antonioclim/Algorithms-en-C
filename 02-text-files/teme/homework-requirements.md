# Week 02 Homework: Text File Processing

## 📋 General Information

- **Deadline:** End of Week 03
- **Points:** 100 (10% of final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11`
- **Submission:** Upload source files to the course portal

---

## 📝 Homework 1: Log File Analyser (50 points)

### Description

Create a programme that analyses web server access logs in the Common Log Format (CLF) and generates a comprehensive statistics report.

### Input Format (Common Log Format)

Each line follows this pattern:
```
IP - - [DATE] "METHOD PATH PROTOCOL" STATUS SIZE
```

Example:
```
192.168.1.100 - - [15/Jan/2025:10:30:45 +0000] "GET /index.html HTTP/1.1" 200 1234
10.0.0.50 - - [15/Jan/2025:10:31:02 +0000] "POST /api/login HTTP/1.1" 401 89
192.168.1.100 - - [15/Jan/2025:10:31:15 +0000] "GET /images/logo.png HTTP/1.1" 200 5678
```

### Requirements

1. **Parse log entries** (15 points)
   - Extract IP address, HTTP method, requested path, status code and response size
   - Handle malformed lines gracefully (skip with warning)
   - Count total lines, successful parses and failed parses

2. **Calculate statistics** (20 points)
   - Total number of requests
   - Requests per HTTP method (GET, POST, PUT, DELETE, etc.)
   - Requests per status code category (2xx, 3xx, 4xx, 5xx)
   - Top 5 most requested paths
   - Top 5 most active IP addresses
   - Total bytes transferred

3. **Generate report** (10 points)
   - Write formatted report to output file
   - Include all calculated statistics
   - Use proper alignment and formatting

4. **Error handling** (5 points)
   - Check all file operations
   - Report line numbers for parse errors
   - Clean exit on fatal errors

### Example Usage

```bash
./homework1 access.log report.txt
```

### Output Format

```
═══════════════════════════════════════════════════════════════
                    WEB SERVER LOG ANALYSIS
═══════════════════════════════════════════════════════════════

Summary
-------
Total requests:     1,234
Successful parses:  1,230
Failed parses:      4
Total bytes:        5,678,901

Requests by Method
------------------
GET:    890 (72.2%)
POST:   280 (22.7%)
PUT:     45 (3.7%)
DELETE:  15 (1.2%)

Status Code Distribution
------------------------
2xx (Success):      1,050 (85.4%)
3xx (Redirect):        80 (6.5%)
4xx (Client Error):    85 (6.9%)
5xx (Server Error):    15 (1.2%)

Top 5 Requested Paths
---------------------
1. /index.html          (234 requests)
2. /api/users           (156 requests)
3. /images/logo.png     (98 requests)
4. /css/style.css       (87 requests)
5. /js/main.js          (76 requests)

Top 5 Active IPs
----------------
1. 192.168.1.100        (345 requests)
2. 10.0.0.50            (234 requests)
...

═══════════════════════════════════════════════════════════════
```

### File: `homework1_log_analyser.c`

---

## 📝 Homework 2: Configuration File Manager (50 points)

### Description

Create a programme that reads, modifies and writes INI-style configuration files whilst preserving comments and structure.

### Input Format (INI File)

```ini
; This is a comment
# This is also a comment

[database]
host = localhost
port = 5432
name = myapp_db

[server]
address = 0.0.0.0
port = 8080
max_connections = 100

[logging]
level = INFO
file = /var/log/app.log
```

### Requirements

1. **Parse INI file** (15 points)
   - Support sections in `[section_name]` format
   - Support key-value pairs with `=` separator
   - Preserve comments (lines starting with `;` or `#`)
   - Handle whitespace around keys and values

2. **Provide operations** (20 points)
   - Get value: retrieve value by section and key
   - Set value: add or update a key-value pair
   - Delete key: remove a key from a section
   - List sections: show all section names
   - List keys: show all keys in a section

3. **Write modified file** (10 points)
   - Preserve original structure and comments
   - Write changes back to file
   - Maintain section order

4. **Command-line interface** (5 points)
   - Support operations via command-line arguments
   - Provide usage help

### Example Usage

```bash
# Get a value
./homework2 config.ini get database host
# Output: localhost

# Set a value
./homework2 config.ini set server port 9090

# Delete a key
./homework2 config.ini delete logging file

# List sections
./homework2 config.ini list-sections

# List keys in a section
./homework2 config.ini list-keys database
```

### File: `homework2_config_manager.c`

---

## 📊 Evaluation Criteria

| Criterion | Homework 1 | Homework 2 |
|-----------|------------|------------|
| Functional correctness | 20 | 20 |
| Proper file I/O | 10 | 10 |
| Error handling | 5 | 5 |
| Code structure and readability | 10 | 10 |
| Memory management (no leaks) | 5 | 5 |
| **Total** | **50** | **50** |

### Penalties

| Issue | Penalty |
|-------|---------|
| Compiler warnings | -5 points per warning (max -10) |
| Memory leaks (Valgrind) | -10 points |
| Crashes on valid input | -15 points |
| Does not compile | -25 points |
| Late submission (per day) | -10 points (max -30) |
| Plagiarism | -50 points + disciplinary action |

---

## 📤 Submission Guidelines

1. **Files to submit:**
   - `homework1_log_analyser.c`
   - `homework2_config_manager.c`
   - Any additional header files (if used)
   - Sample input files used for testing

2. **Naming convention:**
   - Use exactly the filenames specified
   - Include your student ID in a comment at the top of each file

3. **Testing before submission:**
   ```bash
   # Compile with strict flags
   gcc -Wall -Wextra -std=c11 -o homework1 homework1_log_analyser.c
   gcc -Wall -Wextra -std=c11 -o homework2 homework2_config_manager.c
   
   # Check for memory leaks
   valgrind --leak-check=full ./homework1 sample.log output.txt
   valgrind --leak-check=full ./homework2 sample.ini get section key
   ```

---

## 💡 Tips for Success

1. **Start with parsing** — Get the file reading and parsing working first before adding features.

2. **Test incrementally** — Create small test files and verify each feature works before moving on.

3. **Handle edge cases** — Empty files, missing sections, duplicate keys, very long lines.

4. **Use defensive programming** — Check every return value, especially from `fopen()`, `malloc()` and `fscanf()`.

5. **Comment your code** — Explain your parsing logic and any non-obvious decisions.

6. **Read the spec carefully** — Points are allocated to specific features; make sure you implement them all.

---

## 🆘 Getting Help

- **Office hours:** Wednesday 14:00-16:00
- **Course forum:** Post questions (without sharing solution code)
- **Lab sessions:** Ask teaching assistants during Thursday lab

Good luck! 🍀
