# Expense Tracker App

This is an Expense Tracker application built in C++ that helps small businesses manage their expenses efficiently. The application provides features for expense categorization, monthly reports, exporting outputs, and setting budgets with alerts.

## Features

- Expense categorization
- Daily and monthly reports
- Exporting data to various formats
- Budget setting with alerts

## Directory Structure

```bash
expense-traker/
├── build/
│     └── expense_tracker #
│
├── include/
│     └── budget.hpp #
│     └── database.hpp #
│     └── expense.hpp #
│     └── report.hpp #
│     └── utilities.hpp #
│
├── src/
│     └── budget.cpp #
│     └── database.cpp #
│     └── expense.cpp #
│     └── main.cpp #
│     └── report.cpp #
│     └── utilities.cpp #
│
└── Makefile            # Makefile for building the project

## Requirements

- C++ Compiler (g++) with C++17 support
- SQLite3 development library

## Build Instructions only for Linux environment

To build the application, navigate to the project directory in your terminal and run:
```bash
make
```
This will compile the source files and create the executable `expense_tracker` in the `build` directory.

## Cleaning Up

To clean up the compiled files, run:
```bash
make clean
```

## Project TO-DO List

