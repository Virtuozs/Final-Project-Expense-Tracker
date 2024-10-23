#include "database.hpp"
#include "expense.hpp"
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <limits>

using namespace std;

Expense ExpenseManager::createExpense(){
    Expense expense;

    string category, description, date;

    cout << "Enter Category: ";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    getline(cin, category);
    cout << "Enter Description: ";
    getline(cin, description);
    cout << "Enter date with format (YYYY-MM-DD): ";
    cin >> date;

    expense.category = category;
    expense.description = description;
    expense.date = date;

    return expense;
}

vector<ExpenseDetail> ExpenseManager::createExpenseDetail(int expenseId){
    vector<ExpenseDetail> expenseDetail;
    char addMore = 'y';
    double totalAmount = 0.0;

    while (addMore == 'y' || addMore == 'Y'){
        string name, detailDescription, unit;
        int qty, tax;
        double unitPrice;

        cout << "\nAdding Expense Detail\n";

        cout << "Enter Expense Detail Name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, name);

        cout << "Enter Expense Detail Description: ";
        getline(cin, detailDescription);

        cout << "Enter Unit Price: ";
        cin >> unitPrice;

        cout << "Enter Quantity: ";
        cin >> qty;

        cout << "Enter Unit Measurement: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, unit);

        cout << "Enter Tax in Percentage(%): ";
        cin >> tax;

        expenseDetail.emplace_back(expenseId, name, detailDescription, qty, unitPrice, unit, tax);

        cout << "ExpenseDetail.ExpenseId: " <<expenseDetail.front().expenseId <<endl;

        totalAmount += expenseDetail.back().price;

        cout << "\n\nDo you want to add another detail? (y/n): ";
        cin >> addMore;
    }

    return expenseDetail;
}

bool ExpenseManager::updateExpenseTotalAmount(sqlite3 *db,int expenseId, double totalAmount){
    const char *updateTotalAmountQuerySql = "UPDATE Expense SET totalAmount = ? WHERE id = ?";

    sqlite3_stmt *updateStmt;

    if (sqlite3_prepare_v2(db, updateTotalAmountQuerySql, -1, &updateStmt, nullptr) != SQLITE_OK){
        cerr << "Error preparing update statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_double(updateStmt, 1, totalAmount);
    sqlite3_bind_int(updateStmt, 2, expenseId);

    if(sqlite3_step(updateStmt) != SQLITE_DONE){
        cerr << "Error updating totalAmount in Expense: " << sqlite3_errmsg(db) << endl;
        return false;
    }else{
        cout << "Total amount updated successfully." << endl;
    }

    sqlite3_finalize(updateStmt);
    return true;
}