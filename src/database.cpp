#include "database.hpp"
#include <sqlite3.h>
#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <iomanip>

using namespace std;

Database::Database(const string &dbName) {
    if (sqlite3_open(dbName.c_str(), &db)) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
    }
}

Database::~Database() {
    sqlite3_close(db);
}

void Database::init(){
    // Initialize Expense Table
    const char *expenseTable = "CREATE TABLE IF NOT EXISTS Expense ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "category TEXT NOT NULL, "
                      "description TEXT,"
                      "totalAmount REAL NOT NULL, "
                      "date TEXT NOT NULL);";
    char *errMsg = 0;
    int execute_sql = sqlite3_exec(db, expenseTable, nullptr, nullptr, &errMsg);

    if (execute_sql != SQLITE_OK) {
        cerr << "Error Creating Expense Table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    // Initialize ExpenseDetail Table
    const char * expenseDetailTable = "CREATE TABLE IF NOT EXISTS ExpenseDetail ("
                                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                      "expenseId INTEGER NOT NULL, "
                                      "name TEXT NOT NULL, "
                                      "description TEXT, "
                                      "qty INTEGER NOT NULL, "
                                      "unitMeasurement TEXT NOT NULL, "
                                      "unitPrice REAL NOT NULL,"
                                      "tax INTEGER NOT NULL, "
                                      "price REAL NOT NULL, "
                                      "FOREIGN KEY (expenseId) REFERENCES Expense(id) ON DELETE CASCADE);";

    execute_sql = sqlite3_exec(db, expenseDetailTable, 0, 0 ,&errMsg);
    if (execute_sql != SQLITE_OK){
        cerr << "Error Creating Expense Detail Table: " << errMsg << endl;
    }

    // Enabling Foreign Key support on SQLite
    const char* foreignKeySupport = "PRAGMA foreign_keys = ON;";
    execute_sql = sqlite3_exec(db, foreignKeySupport, 0, 0, &errMsg);
    
    if (execute_sql!= SQLITE_OK) {
        cerr << "Error enabling foreign keys: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
}

sqlite3 *Database::getDBConnection() const{
    return db;
}

template <typename T>
vector<T> Database::select(const string &tableName, const map<string, function<void(const T&, variant<string, int, double>)>> &fieldMapping, const string &condition){
    vector<T> results;
    
    string sql = "SELECT * FROM " + tableName;
    if (!condition.empty()){
        sql += " WHERE " + condition;
    }

    sqlite3_stmt *stmt;
    if(sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK){
        cerr << "Error preparing select statement: " << sqlite3_errmsg(db) << endl;
        return results;
    }

    while(sqlite3_step(stmt) != SQLITE_ROW){
        T obj;

        for (const auto &[columnName, setter] : fieldMapping){
            int columnIndex = sqlite3_column_name(stmt, columnName.c_str());
            if (columnIndex < 0){
                cerr << "Column " << columnName << " not found in result set." << endl;
            }

            variant<string, int, double> value;

            switch (sqlite3_column_type(stmt, columnIndex))
            {
                case SQLITE_INTEGER:
                    value = sqlite3_column_int(stmt, columnIndex);
                    break;
                case SQLITE_FLOAT:
                    value = sqlite3_column_double(stmt, columnIndex);
                    break;
                case SQLITE_TEXT:
                    const char *text = sqlite3_column_text(stmt, columnIndex);
                    value = string(reinterpret_cast<const char *>(text));
                    break;
                default:
                    cerr << "Unsupported SQLite type for column" << columnName << endl;
                    break;
            }

            setter(obj, value);
        }
        results.pop_back(obj);
    }

    sqlite3_finalize(stmt);
    return results;
}

// /*
// Need improvement
// 1. Sort by date now
// 2. Sort by current month
// */
// vector<Expense> Database::searchExpense(const string &searchTerm){
//     vector<Expense> expenses;
//     const char *sql = "SELECT * FROM Expense "
//                       "WHERE category LIKE ? OR description LIKE ?;";

//     sqlite3_stmt *stmt;

//     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK){
//         cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
//         return expenses;
//     }

//     string wildcardSearch = "%" + searchTerm + "%";
//     sqlite3_bind_text(stmt, 1, wildcardSearch.c_str(), -1, SQLITE_STATIC);
//     sqlite3_bind_text(stmt, 2, wildcardSearch.c_str(), -1, SQLITE_STATIC);

//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         Expense exp;
//         exp.id = sqlite3_column_int(stmt, 0);
//         exp.category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
//         exp.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
//         exp.amount = sqlite3_column_double(stmt, 3);
//         exp.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
//         expenses.push_back(exp);
//     }

//     sqlite3_finalize(stmt);
//     return expenses;
// }

// vector<ExpenseDetail> Database::getExpenseDetails(int expenseId){
//     vector<ExpenseDetail> details;
//     const char *expenseDetailSqlQuery = "SELECT * FROM ExpenseDetail "
//                                         "WHERE expenseId = ?;";
//     sqlite3_stmt *stmt;
//     if(sqlite3_prepare_v2(db, expenseDetailSqlQuery, -1, &stmt, nullptr) != SQLITE_OK){
//         cerr << "Error preparing SELECT statement: "<< sqlite3_errmsg(db) << endl;
//         return details;
//     }

//     sqlite3_bind_int(stmt, 1, expenseId);

//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         ExpenseDetail expenseDetail;
//         expenseDetail.id = sqlite3_column_int(stmt, 0);
//         expenseDetail.expenseId = sqlite3_column_int(stmt, 1);
//         expenseDetail.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
//         expenseDetail.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
//         expenseDetail.qty = sqlite3_column_int(stmt, 4);
//         expenseDetail.unitPrice = sqlite3_column_double(stmt,5);
//         expenseDetail.tax = sqlite3_column_double(stmt,6);
//         expenseDetail.price = sqlite3_column_double(stmt,7);
//         details.push_back(expenseDetail);
//     }

//     sqlite3_finalize(stmt);
//     return details;
// }

// void Database::displayExpenseDetails(const vector<ExpenseDetail> &details, int pageSize){
//     int totalExpenses = details.size();
//     int totalPages = (totalExpenses + pageSize-1) / pageSize;
//     int currentPage = 1;

//             cout << "Showing page " << currentPage << " of " << totalPages << ": "<<endl;

//         cout << left << setw(5) << "ID"
//              << left << setw(30) << "Name"
//              << left << setw(30) << "Description"
//              << left << setw(15) << "Quantity"
//              << left << setw(15) << "Unit Price"
//              << left << setw(5) << "Tax"
//              << left << setw(15) << "Price" 
//              << endl;

//         cout << string(150, '-') << endl;

//         for (int i = (currentPage - 1) * pageSize; currentPage * pageSize && i < totalExpenses; i++){
//             const ExpenseDetail &detail = details[i];

//             cout << left << setw(5) << detail.id
//                  << left << setw(30) << detail.name
//                  << left << setw(30) << detail.description
//                  << left << setw(15) << detail.qty
//                  << left << setw(15) << detail.unitPrice 
//                  << left << setw(5) << detail.tax 
//                  << left << setw(15) << detail.price 
//                  << endl;
//         }

//         cout << string(150, '-') << '\n';
// }

// void Database::displayExpenses(const vector<Expense> &expenses, int pageSize){
//     int totalExpenses = expenses.size();
//     int totalPages = (totalExpenses + pageSize-1) / pageSize;
//     int currentPage = 1;

//     while (true)
//     {
//         cout << "Showing page " << currentPage << " of " << totalPages << ": "<<endl;

//         cout << left << setw(5) << "ID"
//              << left << setw(20) << "Category"
//              << left << setw(30) << "Description"
//              << left << setw(20) << "Amount"
//              << left << setw(15) << "Date" << endl;

//         cout << string(100, '-') << endl;

//         for (int i = (currentPage -1) * pageSize; currentPage * pageSize && i < totalExpenses; i++){
//             const Expense &expense = expenses[i];

//             cout << left << setw(5) << expense.id
//                  << left << setw(20) << expense.category
//                  << left << setw(30) << expense.description
//                  << left << setw(20) << expense.amount
//                  << left << setw(15) << expense.date << endl;
//         }

//         cout << std::string(100, '-') << '\n';


//         /// Options for managing ExpenseDetails and Pagination

//         cout << "Choose an option:\n";
//         cout << "1. Show Expense Details\n";
//         cout << "2. Next Page\n";
//         cout << "3. Previous Page\n";
//         cout << "4. Exit\n";

//         int choice;
//         cout << "Choose function: ";
//         cin >> choice;

//         int expenseId;
//         vector<ExpenseDetail> details;

//         switch (choice)
//         {
//             case 1:
//                 cout << "Enter Expense Id to show details: ";
//                 cin >> expenseId;
//                 details = getExpenseDetails(expenseId);
//                 displayExpenseDetails(details, 5);
//                 /// Need to add menu to edit and delete Expense Detail record
//                 cout << "Choose an option:\n";
//                 cout << "1. Edit Expense Detail\n";
//                 cout << "2. Delete Expense Detail\n";
//                 cout << "3. Next Page\n";
//                 cout << "4. Previous Page\n";
//                 cout << "5. Exit\n";

//                 int detailChoice;
//                 cout << "Choose Function: ";
//                 cin >> detailChoice;
//                 break;
//             default:
//                 break;
//         }
        
//         if (currentPage < totalPages) {
//             cout << "Press Enter to see more, or 'q' to quit...\n";
//             string input;
//             getline(cin, input);
//             if (input == "q") break;
//             currentPage++;
//         } else {
//             break;
//         }
//     }  
// }

// void Database::removeExpense(int id){
//     const char *sql = "DELETE FROM expenses WHERE id = ?;";
//     sqlite3_stmt *stmt;

//     if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK){
//         cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
//         return;
//     }

//     sqlite3_bind_int(stmt, 1, id);

//     if(sqlite3_step(stmt) != SQLITE_DONE){
//         cerr << "Error removing data from database: " << sqlite3_errmsg(db) << endl;
//     }else{
//         cout << "Expense with ID " << id << " removed successfully.\n";
//     }
// }