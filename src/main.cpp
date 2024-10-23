#include <iostream>
#include <limits>
#include <map>
#include <functional>
#include <variant>
#include "database.hpp"
#include "utilities.hpp"
#include "expense.hpp"

using namespace std;

map<string,function<variant<string, int, double>(const ExpenseDetail&)>> expenseDetailFieldMapping = {
    {"expenseId", [](const ExpenseDetail &e) -> variant<string, int, double> {return e.expenseId;}},
    {"name", [](const ExpenseDetail &e) -> variant<string, int, double> {return e.name;}},
    {"description", [](const ExpenseDetail &e) -> variant<string, int, double> {return e.description;}},
    {"unitPrice", [](const ExpenseDetail &e) -> variant<string, int, double> {return e.unitPrice;}},
    {"qty", [](const ExpenseDetail &e) -> variant<string, int, double> {return e.qty;}},
    {"unitMeasurement", [](const ExpenseDetail &e) -> variant<string, int, double> {return e.unitMeasurement;}},
    {"tax", [](const ExpenseDetail &e) -> variant<string, int, double> {return e.tax;}},
    {"price", [](const ExpenseDetail &e) -> variant<string, int, double> {return e.price;}},
};

map<string,function<variant<string, int, double>(const Expense&)>> expenseFieldMapping = {
    {"category", [](const Expense &e) -> variant<string, int, double> {return e.category;}},
    {"description", [](const Expense &e) -> variant<string, int, double> {return e.description;}},
    {"totalAmount", [](const Expense &e) -> variant<string, int, double> {return e.totalAmount;}},
    {"date", [](const Expense &e) -> variant<string, int, double> {return e.date;}}
};

int main() {
    Database db("expense_tracker.db");
    db.init();

    bool running = true;

    Expense expense;
    vector<ExpenseDetail> expenseDetails;

    ExpenseManager expenseManager;

    while (running)
    {
        int choice;
        string searchTerm;
        // vector<Expense> expenses;
        cout << "1. Add Expense\n2. Search Expenses \n3. Exit\nChoose function: ";
        cin >> choice;

        string category;
        double totalAmount = 0.0;
        string description;
        string date;

        bool db_state;

        int expenseId;

        switch (choice){
            case 1:
                utilities::cleanScreen();

                expense = expenseManager.createExpense();

                db_state = db.insert<Expense>("Expense", expense, expenseFieldMapping);
                if (db_state){
                    cout << "Success Inserting data to Database";
                    expenseId = sqlite3_last_insert_rowid(db.getDBConnection());
                    cout << "Expense Id: "<< expenseId << endl;
                    // Add Expense Detail
                    expenseDetails = expenseManager.createExpenseDetail(expenseId);

                    for (const auto& expenseDetail : expenseDetails){
                        db_state = db.insert<ExpenseDetail>("ExpenseDetail", expenseDetail, expenseDetailFieldMapping);
                        if (db_state){
                            cout << "Success Inserting ExpenseDetail data to Database";
                        }
                        expense.totalAmount += expenseDetail.price;
                    }

                }
                else{
                    cout << sqlite3_errmsg(db.getDBConnection());
                }
                // Update Expense Total Amount
                cout << expense.totalAmount;
                db_state = expenseManager.updateExpenseTotalAmount(db.getDBConnection(), expenseId, expense.totalAmount);
                if (db_state){
                    cout << "Success Updating Expense Total Amount to Database";
                }
                else{
                    cout << "Error: "<< sqlite3_errmsg(db.getDBConnection()) << endl;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'  );
                    cin.get();
                }
                utilities::cleanScreen();

                cout << "Press any key to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n'  );
                cin.get();
                
                utilities::cleanScreen();
                break;
            case 2:
                utilities::cleanScreen();
                cout << "Enter category or description to search: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin,searchTerm);

                // expenses = db.searchExpense(searchTerm);
                // db.displayExpenses(expenses, 5);
                break;
            case 3:
                running=false;
                break;
            case 4:
                running=false;
                break;
            default:
                std::cout << "Invalid choice. Try again.\n";
        }
    }
    
}