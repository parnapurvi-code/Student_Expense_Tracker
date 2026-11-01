/*
 * Student Expense Tracker
 */

#include <iostream>
#include <fstream>
#include <cstring>    
#include <cctype>      
#include <cmath>       
#include <vector>
#include <string>
#include <algorithm>   
#include <map>
#include <sstream>     
#include <iomanip>     
using namespace std;

// Helper functions (used by the classes below)
// Returns true only if the string is not empty and every character is a digit.
// Used to make sure a password contains numbers only.
bool onlyDigits(const string& s){
    if(s.empty()) return false;
    for(int i = 0; i < (int)s.length(); i++){
        if(!isdigit((unsigned char)s[i])) return false;
    }
    return true;
}

// Returns true if the username is already saved in students.txt.
// Prevents two users from sharing one name (and therefore one data file).
bool userExists(const string& u){
    ifstream file("students.txt");
    string user, pass;
    while(file >> user >> pass){
        if(user == u){
            return true;
        }
    }
    return false;
}

// Checks that the date has the exact format DD-MM-YYYY with a valid
// day (1-31) and month (1-12). Protects the later substr() calls from crashing.
bool validDate(const string& d){
    if(d.length() != 10) return false;
    for(int i = 0; i < 10; i++){
        if(i == 2 || i == 5){
            if(d[i] != '-') return false;   // dashes must be at positions 2 and 5
        }
        else if(!isdigit((unsigned char)d[i])){
            return false;                   // all other positions must be digits
        }
    }
    int day = stoi(d.substr(0,2));
    int month = stoi(d.substr(3,2));
    if(month < 1 || month > 12 || day < 1 || day > 31) return false;
    return true;
}

// Class student : account creation and login
class student{
protected:
    char username[100];   // protected so that all derived classes can use it
    string password;      // kept as a string so the saved and typed values match exactly
    char x[20];           // stores the answer to "are you new?"
public:
    // Setter: stores the logged-in username in this object
    void setUsername(string u){
        strcpy(username, u.c_str());
    }

    // Getter: returns the stored username
    string getUsername(){
        return username;
    }

    // Asks whether the user is new, then calls create() or login().
    // Returns the result of whichever function was called.
    int display(){
        cout << "are you new? (yes or no) ";
        cin >> setw(20) >> x;       // setw limits the input so the array cannot overflow
        if(strcmp(x, "yes") == 0){
            return create();
        }
        else{
            return login();
        }
    }

    // Creates a new account. Returns 1 on success, 0 on failure.
    int create(){
        cout << "enter your username: (maximum 100 characters) ";
        cin >> setw(100) >> username;   // setw limits the input so the array cannot overflow

        // Reject a name that is taken; "students" is reserved because
        // students.txt must never be used as a user's expense file
        if(userExists(username) || strcmp(username, "students") == 0){
            cout << "Username already taken, try another one\n";
            return 0;
        }

        cout << "enter your password: (only numbers) ";
        cin >> password;
        if(!onlyDigits(password)){
            cout << "Password must contain only numbers\n";
            return 0;
        }

        // Append the new account to students.txt
        ofstream file("students.txt", ios::app);
        if(file.is_open()){
            file << username << " " << password << endl;
            file.close();

            // Create the user's own empty expense file right away
            string userFile = string(username) + ".txt";
            ofstream uf(userFile, ios::app);
            uf.close();

            cout << "Account successfully created\n";
            return 1;
        }
        else{
            cout << "error opening file\n";
            return 0;
        }
    }

    // Logs an existing user in.
    // Returns: 1 = login ok, 0 = username not found (ask again),
    //         -1 = 3 wrong passwords (program must exit)
    int login(){
        string user, pass;
        string savedPass;
        bool found = false;

        cout << "Enter username: ";
        cin >> setw(100) >> username;

        // Search the whole file for this user and remember the saved password
        ifstream file("students.txt");
        while(file >> user >> pass){
            if(user == username){
                savedPass = pass;
                found = true;
                break;
            }
        }
        file.close();

        if(!found){
            cout << "Username not found\n";
            return 0;
        }

        // Only 3 attempts are allowed for the password
        for(int attempt = 1; attempt <= 3; attempt++){
            cout << "Enter password: ";
            cin >> password;
            if(password == savedPass){
                cout << "login successful\n";
                return 1;
            }
            cout << "Wrong password. Attempts left: " << 3 - attempt << endl;
        }
        cout << "Too many wrong attempts\n";
        return -1;
    }
};


// Class Expense : add, view, update and delete expense records
// Inherits student to reuse the protected username
class Expense : public student {
    // Private data of one expense entry
    double amount;
    string category;
    string date;
public:
    // Adds one expense to the end of the user's file
    void add(){
        string filename = string(username) + ".txt";   // every user has a separate file
        cout << "enter the amount spent: ";
        cin >> amount;
        // Reject letters, zero and negative amounts
        if(cin.fail() || amount <= 0){
            cin.clear();                // clear the error state of cin
            cin.ignore(1000, '\n');     // throw away the wrong input
            cout << "Invalid amount\n";
            return;
        }
        cout << "choose expense category:";
        cout << "\nChoose Category:\n";
        cout << "1. Food & Drinks\n";
        cout << "2. Travel / Transport\n";
        cout << "3. Books & Study Materials\n";
        cout << "4. Hostel / Rent\n";
        cout << "5. Mobile & Internet\n";
        cout << "6. Entertainment\n";
        cout << "7. Shopping\n";
        cout << "8. Health / Medical\n";
        cout << "9. Fees & Education\n";
        cout << "10. Miscellaneous\n";
        int choice;

        cin >> choice;

        // Convert the menu number into a one-word category name
        // (one word, because records are read back with the >> operator)
        switch(choice)
        {
            case 1: category = "Food"; break;
            case 2: category = "Travel"; break;
            case 3: category = "Books"; break;
            case 4: category = "Hostel"; break;
            case 5: category = "Internet"; break;
            case 6: category = "Entertainment"; break;
            case 7: category = "Shopping"; break;
            case 8: category = "Medical"; break;
            case 9: category = "Education"; break;
            case 10: category = "Miscellaneous"; break;
            default: cout << "Invalid Choice\n"; return;
        }
        cout << "enter the date(DD-MM-YYYY): ";
        cin >> date;
        // Keep asking until the date format is correct
        while(!validDate(date)){
            if(cin.eof()) return;       // input closed: stop instead of looping forever
            cout << "invalid date, enter again (DD-MM-YYYY): ";
            cin >> date;
        }
        ofstream file(filename, ios::app);   // ios::app adds at the end of the file
        // fixed + setprecision(2) saves amounts like 250.00, not 2.5e+02
        file << fixed << setprecision(2) << amount << " " << category << " " << date << endl;
        file.close();
        cout << "entry stored successfully\n";
    }

    // Prints every saved record of the user
    void view(){
        string filename = string(username) + ".txt";
        ifstream file(filename);
        string line;
        bool any = false;
        while(getline(file, line)){
            cout << line << endl;
            any = true;
        }
        if(!any){
            cout << "no entries have been made" << endl;
        }
        file.close();
    }

    // Deletes one record chosen by its number.
    // A text file cannot be edited in the middle, so the whole file is
    // loaded into a vector, changed in memory and written back.
    void DeleteExpense(){
        string filename = string(username) + ".txt";
        ifstream file(filename);
        vector<string> records;
        string line;
        while(getline(file, line)){
            records.push_back(line);
        }
        file.close();
        if(records.empty()){
            cout << "no entries have been made" << endl;
            return;
        }
        // Show the records with numbers so the user can choose one
        for(int i = 0; i<(int)records.size(); i++){
            cout << i+1 << ". " << records[i] << endl;
        }
        int choice;
        cout << "enter the expense number to delete ";
        cin >> choice;
        // Reject letters and numbers outside the list
        if(cin.fail() || choice < 1 || choice > (int)records.size()){
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "invalid input" << endl;
            return;
        }
        records.erase(records.begin()+choice-1);   // numbers start at 1, the vector at 0
        // Rewrite the whole file without the deleted record
        ofstream out(filename);
        for(int i = 0; i < (int)records.size(); i++){
            out << records[i] << endl;
        }
        out.close();
        cout << "expense deleted successfully\n";
    }

    // Replaces one record chosen by its number with new values.
    // Uses the same load - change - rewrite method as DeleteExpense().
    void UpdateExpense(){
        string filename = string(username) + ".txt";
        ifstream file(filename);
        string line;
        vector<string> records;
        while(getline(file, line)){
            records.push_back(line);
        }
        file.close();
        // Nothing to update when there are no entries
        if(records.empty()){
            cout << "no entries have been made" << endl;
            return;
        }
        for(int i=0; i < (int)records.size(); i++){
            cout << i+1 << ". " << records[i] << endl;
        }
        int fchoice;
        cout << "enter the entry number you want to edit: ";
        cin >> fchoice;
        if(cin.fail() || fchoice < 1 || fchoice > (int)records.size()){
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "invalid input" << endl;
            return;
        }
        // The new values are read with the same checks as in add()
        cout << "enter the amount spent: ";
        cin >> amount;
        if(cin.fail() || amount <= 0){
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid amount\n";
            return;
        }
        cout << "choose expense category:";
        cout << "\nChoose Category:\n";
        cout << "1. Food & Drinks\n";
        cout << "2. Travel / Transport\n";
        cout << "3. Books & Study Materials\n";
        cout << "4. Hostel / Rent\n";
        cout << "5. Mobile & Internet\n";
        cout << "6. Entertainment\n";
        cout << "7. Shopping\n";
        cout << "8. Health / Medical\n";
        cout << "9. Fees & Education\n";
        cout << "10. Miscellaneous\n";
        int choice;

        cin >> choice;

        switch(choice)
        {
            case 1: category = "Food"; break;
            case 2: category = "Travel"; break;
            case 3: category = "Books"; break;
            case 4: category = "Hostel"; break;
            case 5: category = "Internet"; break;
            case 6: category = "Entertainment"; break;
            case 7: category = "Shopping"; break;
            case 8: category = "Medical"; break;
            case 9: category = "Education"; break;
            case 10: category = "Miscellaneous"; break;
            default: cout << "Invalid Choice\n"; return;
        }
        cout << "enter the date(DD-MM-YYYY): ";
        cin >> date;
        while(!validDate(date)){
            if(cin.eof()) return;
            cout << "invalid date, enter again (DD-MM-YYYY): ";
            cin >> date;
        }
        // Build the new line in the same format as add() (amount with 2 decimals)
        ostringstream oss;
        oss << fixed << setprecision(2) << amount << " " << category << " " << date;
        records[fchoice-1] = oss.str();
        // Rewrite the whole file with the changed record
        ofstream out(filename);
        for(int i = 0; i < (int)records.size(); i++){
            out << records[i] << endl;
        }
        out.close();
        cout << "entry updated successfully\n";
    }
};


// Class ExpenseSearch : search records by category, date or amount
class ExpenseSearch : public Expense {
public :
    // Shows all records of the chosen category
    void Search_by_Category(){
        string filename = string(username) + ".txt";
        ifstream file(filename);
        string s_category;
        cout << "select the category:";
        cout << "\nChoose Category:\n";
        cout << "1. Food & Drinks\n";
        cout << "2. Travel / Transport\n";
        cout << "3. Books & Study Materials\n";
        cout << "4. Hostel / Rent\n";
        cout << "5. Mobile & Internet\n";
        cout << "6. Entertainment\n";
        cout << "7. Shopping\n";
        cout << "8. Health / Medical\n";
        cout << "9. Fees & Education\n";
        cout << "10. Miscellaneous\n";
        int choice;

        cin >> choice;

        switch(choice)
        {
            case 1: s_category = "Food"; break;
            case 2: s_category = "Travel"; break;
            case 3: s_category = "Books"; break;
            case 4: s_category = "Hostel"; break;
            case 5: s_category = "Internet"; break;
            case 6: s_category = "Entertainment"; break;
            case 7: s_category = "Shopping"; break;
            case 8: s_category = "Medical"; break;
            case 9: s_category = "Education"; break;
            case 10: s_category = "Miscellaneous"; break;
            default: cout << "Invalid Choice\n"; return;
        }
        string line;
        bool found = false;
        while (getline(file, line)){
            // Split the line into its three fields and compare only the category,
            // so text in other fields cannot give a false match
            istringstream iss(line);
            double a;
            string c, d;
            iss >> a >> c >> d;
            if(c == s_category){
                cout << line << endl;
                found = true;
            }
        }
        if(!found){
            cout << "no matching expense found\n";
        }
        file.close();
    }

    // Shows all records of the entered date
    void Search_by_Date(){
        string filename = string(username) + ".txt";
        ifstream file(filename);
        string s_date;
        cout << "enter the date(DD-MM-YYYY): ";
        cin >> s_date;
        string line;
        bool found = false;
        while (getline(file, line)){
            // Compare only the date field
            istringstream iss(line);
            double a;
            string c, d;
            iss >> a >> c >> d;
            if(d == s_date){
                cout << line << endl;
                found = true;
            }
        }
        if(!found){
            cout << "no matching expense found\n";
        }
        file.close();
    }

    // Shows all records with the entered amount
    void Search_by_Amount(){
        string filename = string(username) + ".txt";
        ifstream file(filename);
        double s_amount;        // read as a number, so searching 10 does not also match 100
        cout << "enter the amount: ";
        cin >> s_amount;
        if(cin.fail()){
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid amount\n";
            return;
        }
        string line;
        bool found = false;
        while (getline(file, line)){
            istringstream iss(line);
            double a;
            string c, d;
            iss >> a >> c >> d;
            // Decimal numbers must not be compared with ==, so a small tolerance is used
            if(fabs(a - s_amount) < 0.005){
                cout << line << endl;
                found = true;
            }
        }
        if(!found){
            cout << "no matching expense found\n";
        }
        file.close();
    }
};

// Converts DD-MM-YYYY into YYYYMMDD so that dates can be compared
// as strings in correct calendar order
string dateKey(string d){
    if(d.length() < 10) return d;       // too short: avoid a substr() crash
    return d.substr(6,4) + d.substr(3,2) + d.substr(0,2);
}


// Class ExpenseSort : display records sorted by amount, date or category
// Uses a simple exchange sort, O(n^2), which is enough for personal data
class ExpenseSort : public Expense{
    // Private structure that holds one record in memory
    struct ExpenseData{
        double amount;
        string category;
        string date;
    };
public:
    // Sorts in ascending order of amount
    void sortAmount(){
        string filename = string(username) + ".txt";
        ifstream file(filename);
        vector<ExpenseData> expenses;
        ExpenseData temp;

        // Read every record from the file into the vector
        while(file >> temp.amount
                >> temp.category
                >> temp.date)
        {
            expenses.push_back(temp);
        }
        file.close();
        // Compare each element with all later elements and swap if out of order
        for(int i=0; i<(int)expenses.size()-1; i++){
            for(int j=i+1; j<(int)expenses.size(); j++){
                if(expenses[i].amount > expenses[j].amount){
                    swap(expenses[i], expenses[j]);
                }
            }
        }
        cout << "\nExpenses Sorted By Amount:\n";

        for(int i=0; i<(int)expenses.size(); i++)
        {
            cout << expenses[i].amount << " "
                << expenses[i].category << " "
                << expenses[i].date << endl;
        }
    }

    // Sorts in ascending order of date (oldest first)
    void sortDate(){
        string filename = string(username) + ".txt";
        ifstream file(filename);
        vector<ExpenseData> expenses;
        ExpenseData temp;
        while(file >> temp.amount >> temp.category >> temp.date){
            expenses.push_back(temp);
        }
        file.close();
        for(int i = 0; i<(int)expenses.size()-1; i++){
            for(int j = i+1; j<(int)expenses.size(); j++){
                // dateKey() makes the string comparison follow the calendar (year first)
                if(dateKey(expenses[i].date) > dateKey(expenses[j].date)){
                    swap(expenses[i], expenses[j]);
                }
            }
        }
        cout << "\nExpenses Sorted By Date:\n";
        for(int i=0; i<(int)expenses.size(); i++){
            cout << expenses[i].date << " " << expenses[i].amount << " " << expenses[i].category << endl;
        }
    }

    // Sorts alphabetically by category
    void sortCategory(){
        string filename = string(username) + ".txt";
        ifstream file(filename);
        vector<ExpenseData> expenses;
        ExpenseData temp;
        while(file >> temp.amount >> temp.category >> temp.date){
            expenses.push_back(temp);
        }
        file.close();
        for(int i = 0; i<(int)expenses.size()-1; i++){
            for(int j = i+1; j<(int)expenses.size(); j++){
                if(expenses[i].category > expenses[j].category){
                    swap(expenses[i], expenses[j]);
                }
            }
        }
        cout << "\nExpenses Sorted By Category:\n";
        for(int i=0; i<(int)expenses.size(); i++){
            cout << expenses[i].date << " " << expenses[i].amount << " " << expenses[i].category << endl;
        }
    }
};


// Class ReportAnalysis : totals, summaries, graphs, budget,
// export and backup
class ReportAnalysis : public Expense{
    // Private structure that holds one record in memory
    struct ExpenseData{
        double amount;
        string category;
        string date;
    };

    double budget;      // kept only in memory; 0 means "not set"

public:

    // Constructor: the budget starts as "not set"
    ReportAnalysis(){
        budget = 0;
    }

    // Adds up the amounts of all records
    void TotalExpense(){
        double total=0;

        string filename = string(username) + ".txt";
        ifstream file(filename);

        ExpenseData temp;

        while(file >> temp.amount >> temp.category >> temp.date){
            total += temp.amount;
        }

        file.close();

        cout << "\nTotal Expense: " << total << endl;
    }

    // Finds the record with the largest amount
    void HighestExpense(){

        string filename = string(username) + ".txt";

        ifstream file(filename);

        ExpenseData temp;
        ExpenseData highest;

        // The first record is the starting "highest"; if there is none the file is empty
        if(file >> highest.amount >> highest.category >> highest.date){

            while(file >> temp.amount >> temp.category >> temp.date){

                if(temp.amount > highest.amount){
                    highest = temp;
                }

            }

            cout << "\nHighest Expense\n";

            cout << highest.amount << " "
                 << highest.category << " "
                 << highest.date << endl;
        }
        else{
            cout << "\nno entries have been made\n";
        }

        file.close();
    }

    // Finds the record with the smallest amount (same method as HighestExpense)
    void LowestExpense(){

        string filename = string(username) + ".txt";

        ifstream file(filename);

        ExpenseData temp;
        ExpenseData lowest;

        if(file >> lowest.amount >> lowest.category >> lowest.date){

            while(file >> temp.amount >> temp.category >> temp.date){

                if(temp.amount < lowest.amount){
                    lowest = temp;
                }

            }

            cout << "\nLowest Expense\n";

            cout << lowest.amount << " "
                 << lowest.category << " "
                 << lowest.date << endl;
        }
        else{
            cout << "\nno entries have been made\n";
        }

        file.close();
    }

    // Counts the number of records
    void ExpenseCount(){

        int count = 0;

        string filename = string(username) + ".txt";

        ifstream file(filename);

        ExpenseData temp;

        while(file >> temp.amount
                   >> temp.category
                   >> temp.date){

            count++;
        }

        file.close();

        cout << "\nTotal Entries: "
             << count << endl;
    }

    // Shows the total spent in each of the 10 categories
    void CategorySummary(){

        // One accumulator for each category
        double food = 0;
        double travel = 0;
        double books = 0;
        double hostel = 0;
        double internet = 0;
        double entertainment = 0;
        double shopping = 0;
        double medical = 0;
        double education = 0;
        double miscellaneous = 0;

        string filename = string(username) + ".txt";

        ifstream file(filename);

        ExpenseData temp;

        // Add each amount to the accumulator of its category
        while(file >> temp.amount
                   >> temp.category
                   >> temp.date){

            if(temp.category == "Food")
                food += temp.amount;

            else if(temp.category == "Travel")
                travel += temp.amount;

            else if(temp.category == "Books")
                books += temp.amount;

            else if(temp.category == "Hostel")
                hostel += temp.amount;

            else if(temp.category == "Internet")
                internet += temp.amount;

            else if(temp.category == "Entertainment")
                entertainment += temp.amount;

            else if(temp.category == "Shopping")
                shopping += temp.amount;

            else if(temp.category == "Medical")
                medical += temp.amount;

            else if(temp.category == "Education")
                education += temp.amount;

            else if(temp.category == "Miscellaneous")
                miscellaneous += temp.amount;
        }

        file.close();

        cout << "\nCategory Wise Summary\n";

        cout << "Food : " << food << endl;
        cout << "Travel : " << travel << endl;
        cout << "Books : " << books << endl;
        cout << "Hostel : " << hostel << endl;
        cout << "Internet : " << internet << endl;
        cout << "Entertainment : " << entertainment << endl;
        cout << "Shopping : " << shopping << endl;
        cout << "Medical : " << medical << endl;
        cout << "Education : " << education << endl;
        cout << "Miscellaneous : "
             << miscellaneous << endl;
    }

    // Shows the total spent in one month entered as MM-YYYY
    void MonthlySummary(){

        string month;

        cout << "Enter Month-Year (MM-YYYY): ";

        cin >> month;

        double total = 0;

        string filename = string(username) + ".txt";

        ifstream file(filename);

        ExpenseData temp;

        while(file >> temp.amount
                   >> temp.category
                   >> temp.date){

            if(temp.date.length() < 10) continue;   // skip a bad date, substr() would crash

            // MM-YYYY is the part of DD-MM-YYYY that starts at position 3
            string expenseMonth =
            temp.date.substr(3,7);

            if(expenseMonth == month){

                total += temp.amount;

            }
        }

        file.close();

        cout << "\nMonthly Expense : "
             << total << endl;
    }

    // Shows total, count, average and two text bar graphs
    // (month-wise and category-wise)
    void ExpenseStatistics(){
        double total = 0;
        int count = 0;

        // A map keeps its keys sorted, so months and categories print in order
        map<string, double> monthlyTotal;
        map<string, double> categoryTotal;

        string filename = string(username) + ".txt";
        ifstream file(filename);

        ExpenseData temp;

        while(file >> temp.amount
                >> temp.category
                >> temp.date)
        {
            total += temp.amount;
            count++;

            categoryTotal[temp.category] += temp.amount;

            // Extract MM-YYYY from DD-MM-YYYY
            if(temp.date.length() >= 10){       // skip a bad date, substr() would crash
                string month = temp.date.substr(3);
                monthlyTotal[month] += temp.amount;
            }
        }

        file.close();

        cout << "\n=================================\n";
        cout << "       EXPENSE STATISTICS\n";
        cout << "=================================\n";

        cout << "Total Expense   : " << total << endl;
        cout << "Total Entries   : " << count << endl;

        // Avoid division by zero when there are no records
        if(count != 0)
        {
            cout << "Average Expense : "
                << total / count << endl;
        }

        // Monthly Graph: one * for every 100 units spent
        cout << "\n=================================\n";
        cout << "      MONTHLY EXPENSE GRAPH\n";
        cout << "=================================\n";

        for(auto x : monthlyTotal)
        {
            cout << left << setw(12)
                << x.first << " : ";

            int stars = x.second / 100;     // number of stars for this month

            for(int i = 0; i < stars; i++)
            {
                cout << "*";
            }

            cout << " (" << x.second << ")";   // exact total after the bar
            cout << endl;
        }

        // Category Graph: same scale as the monthly graph
        cout << "\n=================================\n";
        cout << "     CATEGORY EXPENSE GRAPH\n";
        cout << "=================================\n";

        for(auto x : categoryTotal)
        {
            cout << left << setw(20)
                << x.first << " : ";

            int stars = x.second / 100;

            for(int i = 0; i < stars; i++)
            {
                cout << "*";
            }

            cout << " (" << x.second << ")";
            cout << endl;
        }
    }

    // Shows the last 5 records
    void RecentTransactions(){

        string filename = string(username) + ".txt";

        ifstream file(filename);

        vector<string> records;

        string line;

        while(getline(file,line)){

            records.push_back(line);

        }

        file.close();

        // size() is unsigned, so it is cast to int before subtracting
        int start = (int)records.size()-5;

        // Fewer than 5 records: show all of them
        if(start < 0){

            start = 0;

        }

        cout << "\nRecent Transactions\n";

        for(int i=start;i<(int)records.size();i++){

            cout << records[i] << endl;

        }
    }

    // Stores the budget entered by the user
    void SetBudget(){

        cout << "Enter Budget Amount : ";

        cin >> budget;

        // Reject letters and a negative budget
        if(cin.fail() || budget < 0){
            cin.clear();
            cin.ignore(1000, '\n');
            budget = 0;         // back to "not set"
            cout << "Invalid budget\n";
            return;
        }

        cout << "Budget Set Successfully\n";
    }

    // Compares total spending with the budget
    void BudgetWarning(){

        // Without this check the program would always say "Budget Exceeded"
        if(budget == 0){
            cout << "\nBudget is not set. Use Set Budget first.\n";
            return;
        }

        double total = 0;

        string filename =
        string(username) + ".txt";

        ifstream file(filename);

        ExpenseData temp;

        while(file >> temp.amount
                   >> temp.category
                   >> temp.date){

            total += temp.amount;
        }

        file.close();

        if(total > budget){

            cout << "\nBudget Exceeded\n";

            cout << "Budget : "
                 << budget << endl;

            cout << "Spent : "
                 << total << endl;
        }
        else{

            cout << "\nRemaining Budget : "
                 << budget-total << endl;
        }
    }

    // Writes a formatted report of this user to <username>_report.txt
    void ExportReport(){
        string filename = string(username) + ".txt";

        ifstream file(filename);

        // Each user gets his own report file
        string reportName = string(username) + "_report.txt";
        ofstream report(reportName);

        double total = 0;
        int count = 0;

        map<string,double> categoryTotal;

        ExpenseData temp;

        // Report heading
        report << "=======================================\n";
        report << "       STUDENT EXPENSE REPORT\n";
        report << "=======================================\n\n";

        report << "Username : " << username << endl;
        report << "Budget   : " << budget << endl;

        report << "\n---------------------------------------\n";
        report << "EXPENSE STATISTICS\n";
        report << "---------------------------------------\n\n";

        // First pass over the file: calculate total, count and category totals
        while(file >> temp.amount
                >> temp.category
                >> temp.date)
        {
            total += temp.amount;
            count++;

            categoryTotal[temp.category] += temp.amount;
        }

        report << "Total Expenses   : "
            << total << endl;

        report << "Total Entries    : "
            << count << endl;

        if(count != 0)
        {
            report << "Average Expense  : "
                << total/count << endl;
        }

        report << "Remaining Budget : "
            << budget - total << endl;

        report << "\n---------------------------------------\n";
        report << "CATEGORY SUMMARY\n";
        report << "---------------------------------------\n\n";

        for(auto x : categoryTotal)
        {
            report << left
                << setw(20)
                << x.first
                << " : "
                << x.second
                << endl;
        }

        // Second pass: close and reopen the file to read it from the start
        file.close();

        file.open(filename);

        report << "\n---------------------------------------\n";
        report << "EXPENSE RECORDS\n";
        report << "---------------------------------------\n\n";

        string line;

        // Copy every record into the report
        while(getline(file,line))
        {
            report << line << endl;
        }

        report << "\n=======================================\n";
        report << "          END OF REPORT\n";
        report << "=======================================\n";

        file.close();
        report.close();

        cout << "\nReport Exported Successfully to " << reportName << endl;
    }

    // Copies the user's expense file to <username>_backup.txt
    void BackupData(){

        string filename =
        string(username) + ".txt";

        ifstream file(filename);

        string backup =
        string(username) + "_backup.txt";

        ofstream out(backup);

        string line;

        while(getline(file,line)){

            out << line << endl;

        }

        file.close();
        out.close();

        cout << "Backup Complete\n";
    }
};


// main : login, then the main menu loop
int main(){
    cout << fixed << setprecision(2);   // all numbers print with 2 decimals, e.g. 1500.00

    student s;
    int loggedIn = 0;

    // Repeat until login or account creation succeeds (result 0 means try again)
    while(loggedIn == 0){
        loggedIn = s.display();
        if(cin.eof()) return 0;         // input closed: stop instead of looping forever
    }

    // 3 wrong passwords: close the program
    if(loggedIn == -1){
        cout << "Program closed.\n";
        return 0;
    }

    // Create the working objects and give each the logged-in username,
    // which selects this user's own file
    ReportAnalysis report;      // also used for add / view / update / delete (inherited from Expense)
    ExpenseSearch search;
    ExpenseSort sorter;
    report.setUsername(s.getUsername());
    search.setUsername(s.getUsername());
    sorter.setUsername(s.getUsername());

    int choice = -1;

    // Main menu: repeats until the user chooses 0
    while(choice != 0){
        cout << "\n========== MAIN MENU ==========\n";
        cout << "1. Add Expense\n";
        cout << "2. View Expenses\n";
        cout << "3. Update Expense\n";
        cout << "4. Delete Expense\n";
        cout << "5. Search Expenses\n";
        cout << "6. Sort Expenses\n";
        cout << "7. Reports & Analysis\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        // A letter instead of a number would leave cin in an error state
        // and make the menu loop forever, so the error is cleared here
        if(cin.eof()) break;
        if(cin.fail()){
            cin.clear();
            cin.ignore(1000, '\n');
            choice = -1;
        }

        if(choice == 1){
            report.add();
        }
        else if(choice == 2){
            report.view();
        }
        else if(choice == 3){
            report.UpdateExpense();
        }
        else if(choice == 4){
            report.DeleteExpense();
        }
        else if(choice == 5){
            // Search sub-menu
            int c;
            cout << "\n1. By Category\n2. By Date\n3. By Amount\n";
            cout << "Enter your choice: ";
            cin >> c;
            if(c == 1) search.Search_by_Category();
            else if(c == 2) search.Search_by_Date();
            else if(c == 3) search.Search_by_Amount();
            else cout << "Invalid choice\n";
        }
        else if(choice == 6){
            // Sort sub-menu
            int c;
            cout << "\n1. By Amount\n2. By Date\n3. By Category\n";
            cout << "Enter your choice: ";
            cin >> c;
            if(c == 1) sorter.sortAmount();
            else if(c == 2) sorter.sortDate();
            else if(c == 3) sorter.sortCategory();
            else cout << "Invalid choice\n";
        }
        else if(choice == 7){
            // Reports and analysis sub-menu
            int c;
            cout << "\n1. Total Expense\n";
            cout << "2. Highest Expense\n";
            cout << "3. Lowest Expense\n";
            cout << "4. Number of Entries\n";
            cout << "5. Category Summary\n";
            cout << "6. Monthly Summary\n";
            cout << "7. Statistics\n";
            cout << "8. Recent Transactions\n";
            cout << "9. Set Budget\n";
            cout << "10. Budget Status\n";
            cout << "11. Export Report\n";
            cout << "12. Backup Data\n";
            cout << "Enter your choice: ";
            cin >> c;
            if(c == 1) report.TotalExpense();
            else if(c == 2) report.HighestExpense();
            else if(c == 3) report.LowestExpense();
            else if(c == 4) report.ExpenseCount();
            else if(c == 5) report.CategorySummary();
            else if(c == 6) report.MonthlySummary();
            else if(c == 7) report.ExpenseStatistics();
            else if(c == 8) report.RecentTransactions();
            else if(c == 9) report.SetBudget();
            else if(c == 10) report.BudgetWarning();
            else if(c == 11) report.ExportReport();
            else if(c == 12) report.BackupData();
            else cout << "Invalid choice\n";
        }
        else if(choice == 0){
            cout << "Goodbye!\n";
        }
        else{
            cout << "Invalid choice, try again\n";
        }
    }

    return 0;
}