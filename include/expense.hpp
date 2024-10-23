#ifndef EXPENSE_HPP
#define EXPENSE_HPP

#include <iostream>
#include <sqlite3.h>
#include <string>
#include "database.hpp"


struct Expense {
    int id;
    string category;
    string description;
    double totalAmount;
    string date;

    Expense() : description(""), totalAmount(0.0), date("") {} 

    Expense(const string& category, const string& desc, const string& date) : category(category), description(desc), totalAmount(0.0), date(date) {}
};

struct ExpenseDetail{
    int id;
    int expenseId;
    string name;
    string description;
    double unitPrice;
    int qty;
    string unitMeasurement;
    int tax;
    double price;

    ExpenseDetail() 
    : name(""), description(""), qty(0), unitMeasurement(""), unitPrice(0.0), tax(0), price(0.0) {}

    ExpenseDetail(int expenseID, const string& n, const string& desc, int q, double up,const string& unit, int t) 
    : expenseId(expenseID), name(n), description(desc), qty(q), unitPrice(up), unitMeasurement(unit), tax(t) {
        price = (unitPrice*qty)+((unitPrice*qty)*(static_cast<double>(tax)/100));
    }
};



class ExpenseManager {
    public:
        Expense createExpense();

        vector<ExpenseDetail> createExpenseDetail(int expenseId);
        
        bool updateExpenseTotalAmount(sqlite3 *db, int expenseId, double totalAmount);
        // map<string,function<variant<string, int, double>(const Expense&)>> getExpenseFieldMapping() const;
        // map<string,function<variant<string, int, double>(const ExpenseDetail&)>> getExpenseDetailFieldMapping() const;

};

#endif