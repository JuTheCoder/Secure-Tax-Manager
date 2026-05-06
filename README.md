# Secure Tax Management System

## Project Description

The Secure Tax Management System is a terminal-based C application designed to help users manage financial records securely. The program allows users to add, edit, delete, view, save, and load income and expense records while also calculating tax estimates based on stored financial data.

---

# Features

* Add income and expense records
* Edit existing records
* Delete records
* View all stored records
* Save records to a text file
* Load records from a text file
* Calculate total income, expenses, and net totals
* Estimate taxes using tax brackets
* Detect deductible expense categories
* Generate a tax report file

---

# Security Features

This project addresses multiple software vulnerabilities including:

* Buffer Overruns
* Format String Problems
* Integer Overflows
* Failure to Handle Errors Correctly
* Poor Usability

Examples of protections implemented:

* Safe input handling using fgets()
* Safe string copying using strncpy()
* NULL pointer checks
* File open validation
* Integer overflow prevention
* Input validation and error reporting

---

# Files

* `tax.c`
  Main application logic and record management functions.

* `tax_logic.c`
  Tax calculation and tax report generation.

* `record.h`
  Structure definitions for records and dynamic lists.

* `tax_logic.h`
  Function prototypes for tax calculations.

* `utils.h`
  Utility function prototypes.

* `records.txt`
  Default file used for saving and loading records.

* `tax_report.txt`
  Automatically generated tax report output file.

---

# How to Compile

Using GCC:

gcc tax.c tax_logic.c utils.c -o tax


---

# How to Run

Run the program with:

./tax

Optional custom records file:

./tax myrecords.txt

---

# Example Menu

1. Add Record
2. View Record
3. Save to File
4. Load from File
5. Delete Record
6. Edit Record
7. Calculate Total
8. Calculate Tax
9. Exit

---

# Example Record Format

Income Salary 5000
Expense School 1200
Expense Medical 300

---

# Tax Calculation

The program calculates:

* Total Income
* Total Expenses
* Deductible Expenses
* Taxable Income
* Estimated Tax Owed
* Effective Tax Rate

Supported deductible categories:

* School
* Medical
* Business
* Charity

---

# Notes

* The program uses dynamic memory allocation for storing records.
* Input validation is performed throughout the application.
* Tax reports are automatically saved to tax_report.txt.
* The application is intended for educational purposes and does not replace professional tax software.

---
