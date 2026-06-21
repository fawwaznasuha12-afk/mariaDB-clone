#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <cstdlib>
#include <regex>
#include <cmath>
#include <random>

using namespace std;
namespace fs = filesystem;

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

// ============================================
// COLOR NAMESPACE (256-color ANSI palette)
// ============================================
namespace Color {
    // Core reset / text style
    const string RESET     = "\033[0m";
    const string BOLD      = "\033[1m";
    const string DIM       = "\033[2m";
    const string ITALIC    = "\033[3m";
    const string UNDERLINE = "\033[4m";

    // Legacy 8-color names kept for backward compatibility,
    // remapped to richer 256-color equivalents.
    const string RED     = "\033[38;5;203m";  // soft coral red (errors)
    const string GREEN   = "\033[38;5;78m";   // mint green (success)
    const string YELLOW  = "\033[38;5;221m";  // warm amber (warnings/notices)
    const string BLUE    = "\033[38;5;75m";   // sky blue
    const string MAGENTA = "\033[38;5;176m";  // soft orchid (JSON/special values)
    const string CYAN    = "\033[38;5;80m";   // teal cyan (structure/borders)
    const string WHITE   = "\033[38;5;253m";  // off-white (default text)

    // Extended semantic palette
    const string TEAL      = "\033[38;5;43m";   // primary brand accent
    const string TEAL_DARK = "\033[38;5;30m";   // darker teal for subtle borders
    const string PURPLE    = "\033[38;5;141m";  // highlight / headers
    const string ORANGE    = "\033[38;5;215m";  // numbers
    const string GRAY      = "\033[38;5;245m";  // secondary/dim text
    const string GRAY_DARK = "\033[38;5;238m";  // very dim, decorative
    const string PINK      = "\033[38;5;212m";  // fun accent

    // Background tints (used sparingly for emphasis)
    const string BG_DARK   = "\033[48;5;235m";
    const string BG_ERROR  = "\033[48;5;52m";
}

// ============================================
// HELPER FUNCTIONS
// ============================================
vector<string> split(const string& str, char delim) {
    vector<string> result;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delim)) {
        if (!token.empty()) {
            result.push_back(token);
        }
    }
    return result;
}

vector<string> splitString(const string& str, const string& delim) {
    vector<string> result;
    size_t pos = 0;
    size_t found;
    while ((found = str.find(delim, pos)) != string::npos) {
        string token = str.substr(pos, found - pos);
        if (!token.empty()) result.push_back(token);
        pos = found + delim.length();
    }
    string last = str.substr(pos);
    if (!last.empty()) result.push_back(last);
    return result;
}

// Case-insensitive variant of splitString: splits on `delim` regardless of case,
// preserving the original casing of each resulting token.
vector<string> splitStringCI(const string& str, const string& delim) {
    vector<string> result;
    string lowerStr = str;
    string lowerDelim = delim;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    transform(lowerDelim.begin(), lowerDelim.end(), lowerDelim.begin(), ::tolower);

    size_t pos = 0;
    size_t found;
    while ((found = lowerStr.find(lowerDelim, pos)) != string::npos) {
        string token = str.substr(pos, found - pos);
        if (!token.empty()) result.push_back(token);
        pos = found + delim.length();
    }
    string last = str.substr(pos);
    if (!last.empty()) result.push_back(last);
    return result;
}

string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

string toUpper(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

string toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool isNumeric(const string& str) {
    if (str.empty()) return false;
    try {
        stod(str);
        return true;
    } catch(...) {
        return false;
    }
}

bool isInteger(const string& str) {
    if (str.empty()) return false;
    try {
        stoll(str);
        return true;
    } catch(...) {
        return false;
    }
}

string getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

string getCurrentDate() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
}

string getCurrentTime() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%H:%M:%S");
    return ss.str();
}

string getRandomString(int length = 10) {
    static const char alphanum[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i) {
        result += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return result;
}

// Safely strip a trailing ';' if present, without crashing on empty strings.
void stripTrailingSemicolon(string& s) {
    if (!s.empty() && s.back() == ';') {
        s.pop_back();
    }
}

// Safely strip surrounding single quotes if present, without crashing on empty strings.
void stripQuotes(string& s) {
    if (s.size() >= 2 && s.front() == '\'' && s.back() == '\'') {
        s = s.substr(1, s.length() - 2);
    }
}

// Case-insensitive find: locates `needle` inside `haystack` regardless of case,
// returning the position in the ORIGINAL (case-preserved) haystack string.
// This avoids bugs where SQL keywords typed in uppercase (the SQL convention)
// fail to match against a lowercase literal search.
size_t ifindCI(const string& haystack, const string& needle, size_t startPos = 0) {
    string h = toLower(haystack);
    string n = toLower(needle);
    return h.find(n, startPos);
}

// ============================================
// MARIADB ENGINE CLASS
// ============================================
class MariaDB {
private:
    // ============================================
    // DATA STRUCTURES
    // ============================================
    struct Column {
        string name;
        string type;
        int length;
        bool isNullable;
        string defaultValue;
        bool isPrimaryKey;
        bool isAutoIncrement;
        bool isUnique;
        bool isIndex;
        
        Column() : length(255), isNullable(true), isPrimaryKey(false), 
                   isAutoIncrement(false), isUnique(false), isIndex(false) {}
    };
    
    struct Table {
        vector<Column> columns;
        vector<vector<string>> rows;
    };
    
    map<string, map<string, Table>> databases;  // database -> table -> Table
    set<string> databaseList;
    
    // User management
    map<string, string> users = {
        {"root", "root"},
        {"admin", "admin123"}
    };
    map<string, set<string>> userPrivileges;
    
    // Session
    string currentDB = "";
    string currentUser = "root";
    bool isLoggedIn = false;
    bool inTransaction = false;
    map<string, map<string, Table>> transactionBackup;
    vector<string> queryHistory;
    map<int, string> processList;
    int processId = 1;
    bool showWarnings = false;
    string lastInsertId = "0";
    
    // ============================================
    // UTILITY FUNCTIONS
    // ============================================
    
    void printColored(const string& text, const string& color) {
        cout << color << text << Color::RESET;
    }
    
    string getColumnTypeString(const Column& col) {
        string type = col.type;
        if (type.find("VARCHAR") != string::npos || type.find("CHAR") != string::npos) {
            type += "(" + to_string(col.length) + ")";
        }
        return type;
    }
    
    int getColumnIndex(const string& tableName, const string& colName) {
        if (databases[currentDB].find(tableName) == databases[currentDB].end()) return -1;
        auto& cols = databases[currentDB][tableName].columns;
        for (size_t i = 0; i < cols.size(); i++) {
            if (cols[i].name == colName) return i;
        }
        return -1;
    }
    
    bool validateDataType(const string& value, const string& type) {
        string t = toUpper(type);
        
        // Numeric types
        if (t.find("TINYINT") != string::npos) {
            try { int v = stoi(value); return v >= -128 && v <= 127; } catch(...) { return false; }
        }
        if (t.find("SMALLINT") != string::npos) {
            try { int v = stoi(value); return v >= -32768 && v <= 32767; } catch(...) { return false; }
        }
        if (t.find("MEDIUMINT") != string::npos) {
            try { int v = stoi(value); return v >= -8388608 && v <= 8388607; } catch(...) { return false; }
        }
        if (t.find("BIGINT") != string::npos) {
            try { stoll(value); return true; } catch(...) { return false; }
        }
        if (t.find("INT") != string::npos) {
            try { stoi(value); return true; } catch(...) { return false; }
        }
        if (t.find("DECIMAL") != string::npos || t.find("NUMERIC") != string::npos) {
            try { stod(value); return true; } catch(...) { return false; }
        }
        if (t.find("FLOAT") != string::npos) {
            try { stof(value); return true; } catch(...) { return false; }
        }
        if (t.find("DOUBLE") != string::npos) {
            try { stod(value); return true; } catch(...) { return false; }
        }
        if (t.find("BOOLEAN") != string::npos || t.find("BOOL") != string::npos) {
            return value == "1" || value == "0" || value == "true" || value == "false";
        }
        
        // String types (check VARCHAR before CHAR, since "CHAR" is a substring of "VARCHAR")
        if (t.find("VARCHAR") != string::npos) {
            int maxLen = 255;
            size_t start = t.find('(');
            if (start != string::npos) {
                size_t end = t.find(')');
                if (end != string::npos) {
                    maxLen = stoi(t.substr(start + 1, end - start - 1));
                }
            }
            return (int)value.length() <= maxLen;
        }
        if (t.find("CHAR") != string::npos) {
            int maxLen = 1;
            size_t start = t.find('(');
            if (start != string::npos) {
                size_t end = t.find(')');
                if (end != string::npos) {
                    maxLen = stoi(t.substr(start + 1, end - start - 1));
                }
            }
            return (int)value.length() <= maxLen;
        }
        if (t == "TEXT" || t == "TINYTEXT" || t == "MEDIUMTEXT" || t == "LONGTEXT") {
            return true;
        }
        
        // Date/Time types
        if (t == "DATE") {
            regex r("^[0-9]{4}-[0-9]{2}-[0-9]{2}$");
            return regex_match(value, r);
        }
        if (t == "TIME") {
            regex r("^[0-9]{2}:[0-9]{2}:[0-9]{2}$");
            return regex_match(value, r);
        }
        if (t == "DATETIME" || t == "TIMESTAMP") {
            regex r("^[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}$");
            return regex_match(value, r);
        }
        if (t == "YEAR") {
            try { int v = stoi(value); return v >= 1900 && v <= 2155; } catch(...) { return false; }
        }
        
        // Binary types
        if (t == "BLOB" || t == "LONGBLOB" || t.find("BINARY") != string::npos || t.find("VARBINARY") != string::npos) {
            return true;
        }
        
        // JSON
        if (t == "JSON") {
            if (value.empty()) return false;
            return value.front() == '{' || value.front() == '[';
        }
        
        // UUID
        if (t == "UUID") {
            regex r("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$");
            return regex_match(value, r);
        }
        
        // ENUM/SET
        if (t.find("ENUM") != string::npos || t.find("SET") != string::npos) {
            return true;
        }
        
        // BIT
        if (t.find("BIT") != string::npos) {
            return value == "0" || value == "1";
        }
        
        return true;
    }
    
    // ============================================
    // PRINT FUNCTIONS
    // ============================================
    
    void printTable(const string& tableName, const vector<vector<string>>& data) {
        if (data.empty()) {
            cout << Color::GRAY << "(Empty set)" << Color::RESET << endl;
            return;
        }
        
        auto& cols = databases[currentDB][tableName].columns;
        vector<int> widths;
        
        for (size_t i = 0; i < cols.size(); i++) {
            int w = cols[i].name.length();
            for (const auto& row : data) {
                if (i < row.size() && (int)row[i].length() > w) {
                    w = row[i].length();
                }
            }
            widths.push_back(w + 2);
        }
        
        // Top border
        cout << Color::TEAL << "+";
        for (int w : widths) {
            for (int i = 0; i < w; i++) cout << "-";
            cout << "+";
        }
        cout << Color::RESET << endl;
        
        // Header
        cout << Color::TEAL << "|" << Color::RESET;
        for (size_t i = 0; i < cols.size(); i++) {
            cout << Color::BOLD << Color::PURPLE << " " << setw(widths[i] - 1) << left << cols[i].name << Color::RESET;
            cout << Color::TEAL << "|" << Color::RESET;
        }
        cout << endl;
        
        // Separator
        cout << Color::TEAL << "+";
        for (int w : widths) {
            for (int i = 0; i < w; i++) cout << "-";
            cout << "+";
        }
        cout << Color::RESET << endl;
        
        // Data
        for (const auto& row : data) {
            cout << Color::TEAL << "|" << Color::RESET;
            for (size_t i = 0; i < row.size(); i++) {
                string val = row[i];
                string color = Color::WHITE;
                if (isNumeric(val)) color = Color::ORANGE;
                else if (val == "NULL" || val == "null") color = Color::GRAY;
                else if (val.find("@") != string::npos) color = Color::BLUE;
                else if (!val.empty() && (val.front() == '{' || val.front() == '[')) color = Color::MAGENTA;
                cout << color << " " << setw(widths[i] - 1) << left << val << Color::RESET;
                cout << Color::TEAL << "|" << Color::RESET;
            }
            cout << endl;
        }
        
        // Bottom border
        cout << Color::TEAL << "+";
        for (int w : widths) {
            for (int i = 0; i < w; i++) cout << "-";
            cout << "+";
        }
        cout << Color::RESET << endl;
        cout << Color::GREEN << data.size() << (data.size() == 1 ? " row in set" : " rows in set") << Color::RESET << endl;
    }
    
    void printSingleRow(const string& col1, const string& col2) {
        size_t width = max(col1.length(), col2.length()) + 2;
        string border = string(width, '-');
        cout << Color::TEAL << "+" << border << "+" << Color::RESET << endl;
        cout << Color::TEAL << "| " << Color::BOLD << Color::PURPLE << left << setw((int)width - 1) << col1 << Color::RESET << Color::TEAL << "|" << Color::RESET << endl;
        cout << Color::TEAL << "+" << border << "+" << Color::RESET << endl;
        cout << Color::TEAL << "| " << Color::ORANGE << left << setw((int)width - 1) << col2 << Color::RESET << Color::TEAL << "|" << Color::RESET << endl;
        cout << Color::TEAL << "+" << border << "+" << Color::RESET << endl;
        cout << Color::GREEN << "1 row in set" << Color::RESET << endl;
    }
    
    // ============================================
    // DATE/TIME FUNCTIONS
    // ============================================
    
    string executeNow() { return getCurrentTimestamp(); }
    string executeCurDate() { return getCurrentDate(); }
    string executeCurTime() { return getCurrentTime(); }
    string executeVersion() { return "10.4.28-MariaDB"; }
    string executeDatabase() { return currentDB.empty() ? "NULL" : currentDB; }
    string executeUser() { return currentUser + "@localhost"; }
    string executeCurrentUser() { return currentUser + "@localhost"; }
    string executeSystemUser() { return currentUser + "@localhost"; }
    string executeSessionUser() { return currentUser + "@localhost"; }
    string executeSchema() { return currentDB.empty() ? "NULL" : currentDB; }
    string executeCurrentTimestamp() { return getCurrentTimestamp(); }
    string executeCurrentDate() { return getCurrentDate(); }
    string executeCurrentTime() { return getCurrentTime(); }
    
    string executeUnixTimestamp() {
        auto now = chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto seconds = chrono::duration_cast<chrono::seconds>(duration).count();
        return to_string(seconds);
    }
    
    string executeFromUnixtime(const string& timestamp) {
        try {
            time_t t = stoll(timestamp);
            stringstream ss;
            ss << put_time(localtime(&t), "%Y-%m-%d %H:%M:%S");
            return ss.str();
        } catch(...) {
            return "NULL";
        }
    }
    
    string executeDateAdd(const string& date, const string& interval, int value) {
        // Simplified: just add days
        try {
            // Parse date
            int year = stoi(date.substr(0, 4));
            int month = stoi(date.substr(5, 2));
            int day = stoi(date.substr(8, 2));
            
            // Add days
            day += value;
            if (day > 31) {
                day -= 31;
                month++;
                if (month > 12) {
                    month = 1;
                    year++;
                }
            }
            stringstream ss;
            ss << setw(4) << setfill('0') << year << "-"
               << setw(2) << setfill('0') << month << "-"
               << setw(2) << setfill('0') << day;
            return ss.str();
        } catch(...) {
            return "NULL";
        }
    }
    
    string executeDateSub(const string& date, const string& interval, int value) {
        return executeDateAdd(date, interval, -value);
    }
    
    string executeDateDiff(const string& date1, const string& date2) {
        try {
            int y1 = stoi(date1.substr(0, 4));
            int m1 = stoi(date1.substr(5, 2));
            int d1 = stoi(date1.substr(8, 2));
            int y2 = stoi(date2.substr(0, 4));
            int m2 = stoi(date2.substr(5, 2));
            int d2 = stoi(date2.substr(8, 2));

            struct tm tm1 = {};
            tm1.tm_year = y1 - 1900; tm1.tm_mon = m1 - 1; tm1.tm_mday = d1; tm1.tm_hour = 12;
            struct tm tm2 = {};
            tm2.tm_year = y2 - 1900; tm2.tm_mon = m2 - 1; tm2.tm_mday = d2; tm2.tm_hour = 12;

            time_t t1 = mktime(&tm1);
            time_t t2 = mktime(&tm2);
            double diffSeconds = difftime(t1, t2);
            long diffDays = static_cast<long>(diffSeconds / 86400.0);
            return to_string(diffDays);
        } catch(...) {
            return "NULL";
        }
    }
    
    string executeTimeDiff(const string& time1, const string& time2) {
        try {
            int h1 = stoi(time1.substr(0, 2));
            int m1 = stoi(time1.substr(3, 2));
            int s1 = stoi(time1.substr(6, 2));
            int h2 = stoi(time2.substr(0, 2));
            int m2 = stoi(time2.substr(3, 2));
            int s2 = stoi(time2.substr(6, 2));
            
            int total1 = h1 * 3600 + m1 * 60 + s1;
            int total2 = h2 * 3600 + m2 * 60 + s2;
            int diff = abs(total1 - total2);
            
            int h = diff / 3600;
            int m = (diff % 3600) / 60;
            int s = diff % 60;
            
            stringstream ss;
            ss << setw(2) << setfill('0') << h << ":"
               << setw(2) << setfill('0') << m << ":"
               << setw(2) << setfill('0') << s;
            return ss.str();
        } catch(...) {
            return "NULL";
        }
    }
    
    string executeDateFormat(const string& date, const string& format) {
        // Simplified: just return date
        return date;
    }
    
    string executeTimeFormat(const string& time, const string& format) {
        return time;
    }
    
    // ============================================
    // STRING FUNCTIONS
    // ============================================
    
    string executeConcat(const vector<string>& args) {
        string result;
        for (const auto& arg : args) {
            result += arg;
        }
        return result;
    }
    
    string executeConcatWS(const string& separator, const vector<string>& args) {
        string result;
        for (size_t i = 0; i < args.size(); i++) {
            result += args[i];
            if (i < args.size() - 1) result += separator;
        }
        return result;
    }
    
    string executeUpper(const string& str) {
        string result = str;
        transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    string executeLower(const string& str) {
        string result = str;
        transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    string executeLength(const string& str) {
        return to_string(str.length());
    }
    
    string executeSubstring(const string& str, int start, int length) {
        if (start < 0) start = str.length() + start;
        if (start < 0 || start >= (int)str.length()) return "";
        if (length < 0) length = str.length() - start;
        if (length > (int)str.length() - start) length = str.length() - start;
        return str.substr(start, length);
    }
    
    string executeTrim(const string& str) {
        return trim(str);
    }
    
    string executeReplace(const string& str, const string& from, const string& to) {
        string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }
    
    string executeRand() {
        return to_string((double)rand() / RAND_MAX);
    }
    
    // ============================================
    // AGGREGATE FUNCTIONS
    // ============================================
    
    string executeCount(const vector<vector<string>>& data, int colIndex) {
        return to_string(data.size());
    }
    
    string executeCountDistinct(const vector<vector<string>>& data, int colIndex) {
        set<string> unique;
        for (const auto& row : data) {
            if (colIndex < (int)row.size()) {
                unique.insert(row[colIndex]);
            }
        }
        return to_string(unique.size());
    }
    
    string executeSum(const vector<vector<string>>& data, int colIndex) {
        double sum = 0;
        for (const auto& row : data) {
            if (colIndex < (int)row.size()) {
                try { sum += stod(row[colIndex]); } catch(...) {}
            }
        }
        stringstream ss;
        ss << sum;
        return ss.str();
    }
    
    string executeSumDistinct(const vector<vector<string>>& data, int colIndex) {
        set<string> unique;
        for (const auto& row : data) {
            if (colIndex < (int)row.size()) {
                unique.insert(row[colIndex]);
            }
        }
        double sum = 0;
        for (const auto& val : unique) {
            try { sum += stod(val); } catch(...) {}
        }
        stringstream ss;
        ss << sum;
        return ss.str();
    }
    
    string executeAvg(const vector<vector<string>>& data, int colIndex) {
        if (data.empty()) return "NULL";
        double sum = 0;
        for (const auto& row : data) {
            if (colIndex < (int)row.size()) {
                try { sum += stod(row[colIndex]); } catch(...) {}
            }
        }
        stringstream ss;
        ss << fixed << setprecision(2) << (sum / data.size());
        return ss.str();
    }
    
    string executeAvgDistinct(const vector<vector<string>>& data, int colIndex) {
        if (data.empty()) return "NULL";
        set<string> unique;
        for (const auto& row : data) {
            if (colIndex < (int)row.size()) {
                unique.insert(row[colIndex]);
            }
        }
        double sum = 0;
        for (const auto& val : unique) {
            try { sum += stod(val); } catch(...) {}
        }
        stringstream ss;
        ss << fixed << setprecision(2) << (sum / unique.size());
        return ss.str();
    }
    
    string executeMin(const vector<vector<string>>& data, int colIndex) {
        if (data.empty()) return "NULL";
        string minVal = data[0][colIndex];
        for (const auto& row : data) {
            if (colIndex < (int)row.size()) {
                if (row[colIndex] < minVal) minVal = row[colIndex];
            }
        }
        return minVal;
    }
    
    string executeMax(const vector<vector<string>>& data, int colIndex) {
        if (data.empty()) return "NULL";
        string maxVal = data[0][colIndex];
        for (const auto& row : data) {
            if (colIndex < (int)row.size()) {
                if (row[colIndex] > maxVal) maxVal = row[colIndex];
            }
        }
        return maxVal;
    }
    
    string executeStd(const vector<vector<string>>& data, int colIndex) {
        if (data.size() < 2) return "NULL";
        double mean = stod(executeAvg(data, colIndex));
        double sum = 0;
        for (const auto& row : data) {
            if (colIndex < (int)row.size()) {
                try {
                    double val = stod(row[colIndex]);
                    sum += (val - mean) * (val - mean);
                } catch(...) {}
            }
        }
        double variance = sum / data.size();
        double stddev = sqrt(variance);
        stringstream ss;
        ss << fixed << setprecision(4) << stddev;
        return ss.str();
    }
    
    string executeVariance(const vector<vector<string>>& data, int colIndex) {
        if (data.size() < 2) return "NULL";
        double mean = stod(executeAvg(data, colIndex));
        double sum = 0;
        for (const auto& row : data) {
            if (colIndex < (int)row.size()) {
                try {
                    double val = stod(row[colIndex]);
                    sum += (val - mean) * (val - mean);
                } catch(...) {}
            }
        }
        stringstream ss;
        ss << fixed << setprecision(4) << (sum / data.size());
        return ss.str();
    }
    
    string executeBitAnd(const vector<vector<string>>& data, int colIndex) {
        int result = 0;
        for (const auto& row : data) {
            if (colIndex < (int)row.size()) {
                try { result &= stoi(row[colIndex]); } catch(...) {}
            }
        }
        return to_string(result);
    }
    
    string executeBitOr(const vector<vector<string>>& data, int colIndex) {
        int result = 0;
        for (const auto& row : data) {
            if (colIndex < (int)row.size()) {
                try { result |= stoi(row[colIndex]); } catch(...) {}
            }
        }
        return to_string(result);
    }
    
    string executeBitXor(const vector<vector<string>>& data, int colIndex) {
        int result = 0;
        bool first = true;
        for (const auto& row : data) {
            if (colIndex < (int)row.size()) {
                try {
                    if (first) { result = stoi(row[colIndex]); first = false; }
                    else { result ^= stoi(row[colIndex]); }
                } catch(...) {}
            }
        }
        return to_string(result);
    }
    
    // ============================================
    // WHERE EVALUATION
    // ============================================
    
    bool evaluateWhere(const vector<string>& row, const string& where, const vector<Column>& cols) {
        string condition = where;
        
        // Handle AND
        if (ifindCI(condition, " AND ") != string::npos) {
            vector<string> parts = splitStringCI(condition, " AND ");
            for (const auto& part : parts) {
                if (!evaluateWhere(row, part, cols)) return false;
            }
            return true;
        }
        
        // Handle OR
        if (ifindCI(condition, " OR ") != string::npos) {
            vector<string> parts = splitStringCI(condition, " OR ");
            for (const auto& part : parts) {
                if (evaluateWhere(row, part, cols)) return true;
            }
            return false;
        }
        
        // Handle LIKE
        size_t likePos = ifindCI(condition, " LIKE ");
        if (likePos != string::npos) {
            string col = trim(condition.substr(0, likePos));
            string pattern = condition.substr(likePos + 6);
            pattern = trim(pattern);
            stripQuotes(pattern);
            int idx = getColumnIndex("_temp", col);
            if (idx == -1) {
                // Try to find in original columns
                for (size_t i = 0; i < cols.size(); i++) {
                    if (cols[i].name == col) { idx = i; break; }
                }
            }
            if (idx == -1) return false;
            // Convert SQL LIKE pattern to regex
            string regexPattern = regex_replace(pattern, regex("%"), ".*");
            regexPattern = regex_replace(regexPattern, regex("_"), ".");
            regex r(regexPattern);
            return regex_match(row[idx], r);
        }
        
        // Handle IN
        size_t inPos = ifindCI(condition, " IN ");
        if (inPos != string::npos) {
            string col = trim(condition.substr(0, inPos));
            string valuesPart = condition.substr(inPos + 4);
            valuesPart = trim(valuesPart);
            if (valuesPart.size() >= 2 && valuesPart.front() == '(' && valuesPart.back() == ')') {
                valuesPart = valuesPart.substr(1, valuesPart.length() - 2);
            }
            vector<string> values = split(valuesPart, ',');
            for (auto& v : values) {
                v = trim(v);
                stripQuotes(v);
            }
            int idx = getColumnIndex("_temp", col);
            if (idx == -1) {
                for (size_t i = 0; i < cols.size(); i++) {
                    if (cols[i].name == col) { idx = i; break; }
                }
            }
            if (idx == -1) return false;
            for (const auto& v : values) {
                if (row[idx] == v) return true;
            }
            return false;
        }
        
        // Handle BETWEEN
        size_t betweenPos = ifindCI(condition, " BETWEEN ");
        if (betweenPos != string::npos) {
            string col = trim(condition.substr(0, betweenPos));
            string range = condition.substr(betweenPos + 9);
            vector<string> parts = split(range, ' ');
            if (parts.size() >= 3) {
                string val1 = trim(parts[0]);
                string val2 = trim(parts[2]);
                stripQuotes(val1);
                stripQuotes(val2);
                int idx = getColumnIndex("_temp", col);
                if (idx == -1) {
                    for (size_t i = 0; i < cols.size(); i++) {
                        if (cols[i].name == col) { idx = i; break; }
                    }
                }
                if (idx == -1) return false;
                return row[idx] >= val1 && row[idx] <= val2;
            }
            return false;
        }
        
        // Handle IS NULL
        size_t isNullPos = ifindCI(condition, " IS NULL");
        if (isNullPos != string::npos) {
            string col = trim(condition.substr(0, isNullPos));
            int idx = getColumnIndex("_temp", col);
            if (idx == -1) {
                for (size_t i = 0; i < cols.size(); i++) {
                    if (cols[i].name == col) { idx = i; break; }
                }
            }
            if (idx == -1) return false;
            return row[idx] == "NULL" || row[idx] == "null";
        }
        
        // Handle IS NOT NULL
        size_t isNotNullPos = ifindCI(condition, " IS NOT NULL");
        if (isNotNullPos != string::npos) {
            string col = trim(condition.substr(0, isNotNullPos));
            int idx = getColumnIndex("_temp", col);
            if (idx == -1) {
                for (size_t i = 0; i < cols.size(); i++) {
                    if (cols[i].name == col) { idx = i; break; }
                }
            }
            if (idx == -1) return false;
            return row[idx] != "NULL" && row[idx] != "null";
        }
        
        // Handle NOT
        size_t notFindPos = ifindCI(condition, " NOT ");
        if (notFindPos != string::npos) {
            // Simple NOT handling
            string rest = condition;
            size_t notPos = ifindCI(rest, " NOT ");
            if (notPos != string::npos) {
                string afterNot = rest.substr(notPos + 5);
                return !evaluateWhere(row, afterNot, cols);
            }
        }
        
        // Handle =, !=, <, >, <=, >=
        // Operators may or may not be surrounded by spaces (e.g. "id=2" or "id = 2"),
        // so we search for the bare operator and trim whitespace around the operands.
        for (const string op : {"!=", "<=", ">=", "<", ">", "="}) {
            size_t pos = condition.find(op);
            if (pos != string::npos) {
                string col = trim(condition.substr(0, pos));
                string val = trim(condition.substr(pos + op.length()));
                stripQuotes(val);
                int idx = getColumnIndex("_temp", col);
                if (idx == -1) {
                    for (size_t i = 0; i < cols.size(); i++) {
                        if (cols[i].name == col) { idx = i; break; }
                    }
                }
                if (idx == -1) return false;

                // Numeric comparison when both sides look numeric, otherwise lexical/string comparison.
                bool numericCompare = isNumeric(row[idx]) && isNumeric(val);

                if (op == "=") return numericCompare ? (stod(row[idx]) == stod(val)) : (row[idx] == val);
                if (op == "!=") return numericCompare ? (stod(row[idx]) != stod(val)) : (row[idx] != val);
                if (op == "<") return numericCompare ? (stod(row[idx]) < stod(val)) : (row[idx] < val);
                if (op == ">") return numericCompare ? (stod(row[idx]) > stod(val)) : (row[idx] > val);
                if (op == "<=") return numericCompare ? (stod(row[idx]) <= stod(val)) : (row[idx] <= val);
                if (op == ">=") return numericCompare ? (stod(row[idx]) >= stod(val)) : (row[idx] >= val);
            }
        }
        
        // If the condition couldn't be parsed, don't silently match everything —
        // that would make UPDATE/DELETE without a recognizable WHERE dangerously match all rows.
        return false;
    }
    
    // ============================================
    // SQL EXECUTION FUNCTIONS
    // ============================================
    
    void executeCreateDatabase(const string& name) {
        if (databaseList.find(name) != databaseList.end()) {
            cout << Color::RED << "✗ ERROR 1007 (HY000): Can't create database '" << name << "'; database exists" << Color::RESET << endl;
            return;
        }
        databaseList.insert(name);
        databases[name] = map<string, Table>();
        cout << Color::GREEN << "✓ Query OK, 1 row affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeDropDatabase(const string& name) {
        if (databaseList.find(name) == databaseList.end()) {
            cout << Color::RED << "✗ ERROR 1008 (HY000): Can't drop database '" << name << "'; database doesn't exist" << Color::RESET << endl;
            return;
        }
        databaseList.erase(name);
        databases.erase(name);
        if (currentDB == name) {
            currentDB = "";
        }
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeShowDatabases() {
        cout << Color::CYAN << "+--------------------+" << Color::RESET << endl;
        cout << Color::CYAN << "| " << Color::BOLD << Color::WHITE << "Database" << Color::RESET << Color::CYAN << "           |" << Color::RESET << endl;
        cout << Color::CYAN << "+--------------------+" << Color::RESET << endl;
        cout << Color::CYAN << "| " << Color::WHITE << "information_schema" << Color::RESET << Color::CYAN << " |" << Color::RESET << endl;
        cout << Color::CYAN << "| " << Color::WHITE << "mysql              " << Color::RESET << Color::CYAN << " |" << Color::RESET << endl;
        cout << Color::CYAN << "| " << Color::WHITE << "performance_schema " << Color::RESET << Color::CYAN << " |" << Color::RESET << endl;
        cout << Color::CYAN << "| " << Color::WHITE << "test               " << Color::RESET << Color::CYAN << " |" << Color::RESET << endl;
        for (const auto& db : databaseList) {
            if (db != "test") {
                cout << Color::CYAN << "| " << Color::WHITE << left << setw(18) << db << Color::RESET << Color::CYAN << " |" << Color::RESET << endl;
            }
        }
        cout << Color::CYAN << "+--------------------+" << Color::RESET << endl;
        cout << Color::GREEN << databaseList.size() + 3 << " rows in set" << Color::RESET << endl;
    }
    
    void executeUse(const string& db) {
        if (databaseList.find(db) == databaseList.end() && db != "test") {
            cout << Color::RED << "✗ ERROR 1049 (42000): Unknown database '" << db << "'" << Color::RESET << endl;
            return;
        }
        currentDB = db;
        if (databases.find(currentDB) == databases.end()) {
            databases[currentDB] = map<string, Table>();
        }
        if (databaseList.find(currentDB) == databaseList.end()) {
            databaseList.insert(currentDB);
        }
        cout << Color::GREEN << "Database changed" << Color::RESET << endl;
    }
    
    void executeCreateTable(const string& name, const string& columnsDef) {
        if (databases[currentDB].find(name) != databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1050 (42S01): Table '" << name << "' already exists" << Color::RESET << endl;
            return;
        }
        
        Table newTable;
        vector<string> colDefs = split(columnsDef, ',');
        for (auto& def : colDefs) {
            def = trim(def);
            Column col;
            
            // Parse column definition: name TYPE [options]
            size_t spacePos = def.find(' ');
            if (spacePos == string::npos) {
                col.name = def;
                col.type = "TEXT";
            } else {
                col.name = trim(def.substr(0, spacePos));
                string typePart = trim(def.substr(spacePos + 1));
                
                // Parse type with length
                size_t parenPos = typePart.find('(');
                if (parenPos != string::npos) {
                    size_t parenEnd = typePart.find(')');
                    if (parenEnd != string::npos) {
                        col.type = trim(typePart.substr(0, parenPos));
                        string lenStr = typePart.substr(parenPos + 1, parenEnd - parenPos - 1);
                        try { col.length = stoi(lenStr); } catch(...) { col.length = 255; }
                        string rest = trim(typePart.substr(parenEnd + 1));
                        // Parse options from rest
                        string upperRest = toUpper(rest);
                        if (upperRest.find("NOT NULL") != string::npos) col.isNullable = false;
                        if (upperRest.find("PRIMARY KEY") != string::npos) col.isPrimaryKey = true;
                        if (upperRest.find("AUTO_INCREMENT") != string::npos) col.isAutoIncrement = true;
                        if (upperRest.find("UNIQUE") != string::npos) col.isUnique = true;
                        if (upperRest.find("INDEX") != string::npos) col.isIndex = true;
                    }
                } else {
                    // Parse type without length
                    vector<string> typeParts = split(typePart, ' ');
                    col.type = toUpper(typeParts[0]);
                    for (size_t i = 1; i < typeParts.size(); i++) {
                        string opt = toUpper(typeParts[i]);
                        if (opt == "NOT" && i+1 < typeParts.size() && toUpper(typeParts[i+1]) == "NULL") {
                            col.isNullable = false;
                            i++;
                        } else if (opt == "PRIMARY" && i+1 < typeParts.size() && toUpper(typeParts[i+1]) == "KEY") {
                            col.isPrimaryKey = true;
                            i++;
                        } else if (opt == "AUTO_INCREMENT") {
                            col.isAutoIncrement = true;
                        } else if (opt == "UNIQUE") {
                            col.isUnique = true;
                        } else if (opt == "INDEX" || opt == "KEY") {
                            col.isIndex = true;
                        } else if (opt == "NULL") {
                            col.isNullable = true;
                        }
                    }
                }
            }
            
            newTable.columns.push_back(col);
        }
        
        databases[currentDB][name] = newTable;
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeDropTable(const string& name, bool ifExists = false) {
        if (databases[currentDB].find(name) == databases[currentDB].end()) {
            if (ifExists) {
                cout << Color::YELLOW << "Query OK, 0 rows affected" << Color::RESET << endl;
                return;
            }
            cout << Color::RED << "✗ ERROR 1051 (42S02): Unknown table '" << name << "'" << Color::RESET << endl;
            return;
        }
        databases[currentDB].erase(name);
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeTruncateTable(const string& name) {
        if (databases[currentDB].find(name) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1051 (42S02): Unknown table '" << name << "'" << Color::RESET << endl;
            return;
        }
        databases[currentDB][name].rows.clear();
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeAlterTableAddColumn(const string& name, const string& colDef) {
        if (databases[currentDB].find(name) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1051 (42S02): Unknown table '" << name << "'" << Color::RESET << endl;
            return;
        }
        
        Column col;
        string def = trim(colDef);
        size_t spacePos = def.find(' ');
        if (spacePos == string::npos) {
            col.name = def;
            col.type = "TEXT";
        } else {
            col.name = trim(def.substr(0, spacePos));
            string typePart = trim(def.substr(spacePos + 1));
            size_t parenPos = typePart.find('(');
            if (parenPos != string::npos) {
                size_t parenEnd = typePart.find(')');
                if (parenEnd != string::npos) {
                    col.type = trim(typePart.substr(0, parenPos));
                    string lenStr = typePart.substr(parenPos + 1, parenEnd - parenPos - 1);
                    try { col.length = stoi(lenStr); } catch(...) { col.length = 255; }
                }
            } else {
                col.type = toUpper(typePart);
            }
        }
        
        databases[currentDB][name].columns.push_back(col);
        for (auto& row : databases[currentDB][name].rows) {
            row.push_back("NULL");
        }
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeAlterTableDropColumn(const string& name, const string& colName) {
        if (databases[currentDB].find(name) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1051 (42S02): Unknown table '" << name << "'" << Color::RESET << endl;
            return;
        }
        int idx = getColumnIndex(name, colName);
        if (idx == -1) {
            cout << Color::RED << "✗ ERROR 1091 (42000): Can't DROP '" << colName << "'; check that column/key exists" << Color::RESET << endl;
            return;
        }
        databases[currentDB][name].columns.erase(databases[currentDB][name].columns.begin() + idx);
        for (auto& row : databases[currentDB][name].rows) {
            row.erase(row.begin() + idx);
        }
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeAlterTableModifyColumn(const string& name, const string& colDef) {
        if (databases[currentDB].find(name) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1051 (42S02): Unknown table '" << name << "'" << Color::RESET << endl;
            return;
        }
        
        string def = trim(colDef);
        size_t spacePos = def.find(' ');
        if (spacePos == string::npos) {
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        string colName = trim(def.substr(0, spacePos));
        int idx = getColumnIndex(name, colName);
        if (idx == -1) {
            cout << Color::RED << "✗ ERROR 1054 (42S22): Unknown column '" << colName << "'" << Color::RESET << endl;
            return;
        }
        
        // Just update the type
        string typePart = trim(def.substr(spacePos + 1));
        databases[currentDB][name].columns[idx].type = toUpper(typePart);
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeRenameTable(const string& oldName, const string& newName) {
        if (databases[currentDB].find(oldName) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1051 (42S02): Unknown table '" << oldName << "'" << Color::RESET << endl;
            return;
        }
        if (databases[currentDB].find(newName) != databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1050 (42S01): Table '" << newName << "' already exists" << Color::RESET << endl;
            return;
        }
        databases[currentDB][newName] = databases[currentDB][oldName];
        databases[currentDB].erase(oldName);
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeInsert(const string& name, const string& values) {
        if (databases[currentDB].find(name) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1146 (42S02): Table '" << name << "' doesn't exist" << Color::RESET << endl;
            return;
        }
        
        vector<string> vals = split(values, ',');
        for (auto& v : vals) {
            v = trim(v);
            stripQuotes(v);
        }
        
        auto& table = databases[currentDB][name];
        if (vals.size() != table.columns.size()) {
            cout << Color::RED << "✗ ERROR 1136 (21S01): Column count doesn't match value count" << Color::RESET << endl;
            return;
        }
        
        // Validate data types
        for (size_t i = 0; i < vals.size(); i++) {
            if (!validateDataType(vals[i], table.columns[i].type)) {
                cout << Color::RED << "✗ ERROR 1265 (01000): Data truncated for column '" << table.columns[i].name << "'" << Color::RESET << endl;
                return;
            }
        }
        
        table.rows.push_back(vals);
        lastInsertId = vals[0];
        cout << Color::GREEN << "✓ Query OK, 1 row affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeInsertMultiple(const string& name, const vector<string>& rows) {
        if (databases[currentDB].find(name) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1146 (42S02): Table '" << name << "' doesn't exist" << Color::RESET << endl;
            return;
        }
        
        int count = 0;
        for (const auto& values : rows) {
            vector<string> vals = split(values, ',');
            for (auto& v : vals) {
                v = trim(v);
                stripQuotes(v);
            }
            auto& table = databases[currentDB][name];
            if (vals.size() == table.columns.size()) {
                table.rows.push_back(vals);
                count++;
            }
        }
        cout << Color::GREEN << "✓ Query OK, " << count << " rows affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeSelect(const string& tableName, const vector<string>& columns, 
                       const string& where, const string& groupBy, const string& having,
                       const string& orderBy, const string& limit, const string& offset,
                       const string& distinct) {
        if (databases[currentDB].find(tableName) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1146 (42S02): Table '" << tableName << "' doesn't exist" << Color::RESET << endl;
            return;
        }
        
        auto& table = databases[currentDB][tableName];
        vector<vector<string>> result = table.rows;
        
        // WHERE clause
        if (!where.empty()) {
            vector<vector<string>> filtered;
            for (const auto& row : result) {
                if (evaluateWhere(row, where, table.columns)) {
                    filtered.push_back(row);
                }
            }
            result = filtered;
        }
        
        // GROUP BY
        if (!groupBy.empty()) {
            int idx = getColumnIndex(tableName, groupBy);
            if (idx != -1) {
                map<string, vector<vector<string>>> groups;
                for (const auto& row : result) {
                    string key = row[idx];
                    groups[key].push_back(row);
                }
                result.clear();
                for (const auto& group : groups) {
                    if (!group.second.empty()) {
                        result.push_back(group.second[0]);
                    }
                }
            }
        }
        
        // ORDER BY
        if (!orderBy.empty()) {
            string col = trim(orderBy);
            bool desc = false;
            if (col.find(" DESC") != string::npos) {
                desc = true;
                col = trim(col.substr(0, col.find(" DESC")));
            }
            if (col.find(" ASC") != string::npos) {
                col = trim(col.substr(0, col.find(" ASC")));
            }
            int idx = getColumnIndex(tableName, col);
            if (idx != -1) {
                sort(result.begin(), result.end(), [idx, desc](const vector<string>& a, const vector<string>& b) {
                    if (isNumeric(a[idx]) && isNumeric(b[idx])) {
                        if (desc) return stod(a[idx]) > stod(b[idx]);
                        return stod(a[idx]) < stod(b[idx]);
                    }
                    if (desc) return a[idx] > b[idx];
                    return a[idx] < b[idx];
                });
            }
        }
        
        // LIMIT
        if (!limit.empty()) {
            try {
                int l = stoi(limit);
                int o = 0;
                if (!offset.empty()) {
                    o = stoi(offset);
                }
                if ((int)result.size() > o + l) {
                    result.resize(o + l);
                }
                if (o > 0) {
                    result.erase(result.begin(), result.begin() + o);
                }
            } catch(...) {}
        }
        
        // DISTINCT
        if (!distinct.empty()) {
            set<vector<string>> unique;
            for (const auto& row : result) {
                unique.insert(row);
            }
            result.assign(unique.begin(), unique.end());
        }
        
        printTable(tableName, result);
    }
    
    void executeUpdate(const string& name, const string& setClause, const string& where) {
        if (databases[currentDB].find(name) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1146 (42S02): Table '" << name << "' doesn't exist" << Color::RESET << endl;
            return;
        }
        
        vector<string> parts = split(setClause, '=');
        if (parts.size() != 2) {
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        string col = trim(parts[0]);
        string val = trim(parts[1]);
        stripQuotes(val);
        
        int idx = getColumnIndex(name, col);
        if (idx == -1) {
            cout << Color::RED << "✗ ERROR 1054 (42S22): Unknown column '" << col << "' in 'field list'" << Color::RESET << endl;
            return;
        }
        
        int affected = 0;
        auto& table = databases[currentDB][name];
        
        if (where.empty()) {
            for (auto& row : table.rows) {
                row[idx] = val;
                affected++;
            }
        } else {
            for (auto& row : table.rows) {
                if (evaluateWhere(row, where, table.columns)) {
                    row[idx] = val;
                    affected++;
                }
            }
        }
        
        cout << Color::GREEN << "✓ Query OK, " << affected << " row(s) affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeDelete(const string& name, const string& where) {
        if (databases[currentDB].find(name) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1146 (42S02): Table '" << name << "' doesn't exist" << Color::RESET << endl;
            return;
        }
        
        int affected = 0;
        auto& table = databases[currentDB][name];
        
        if (where.empty()) {
            affected = table.rows.size();
            table.rows.clear();
        } else {
            auto it = table.rows.begin();
            while (it != table.rows.end()) {
                if (evaluateWhere(*it, where, table.columns)) {
                    it = table.rows.erase(it);
                    affected++;
                } else {
                    ++it;
                }
            }
        }
        
        cout << Color::GREEN << "✓ Query OK, " << affected << " row(s) affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeShowTables() {
        if (databases[currentDB].empty()) {
            cout << Color::GRAY << "(Empty set)" << Color::RESET << endl;
            return;
        }
        
        cout << Color::CYAN << "Tables_in_" << currentDB << ":" << Color::RESET << endl;
        cout << Color::CYAN << "+----------------------+" << Color::RESET << endl;
        for (const auto& t : databases[currentDB]) {
            cout << Color::CYAN << "| " << Color::WHITE << setw(20) << left << t.first << Color::RESET << Color::CYAN << " |" << Color::RESET << endl;
        }
        cout << Color::CYAN << "+----------------------+" << Color::RESET << endl;
        cout << Color::GREEN << databases[currentDB].size() << " rows in set" << Color::RESET << endl;
    }
    
    void executeShowFullTables() {
        if (databases[currentDB].empty()) {
            cout << Color::GRAY << "(Empty set)" << Color::RESET << endl;
            return;
        }
        
        cout << Color::CYAN << "Tables_in_" << currentDB << "\tTable_type" << Color::RESET << endl;
        for (const auto& t : databases[currentDB]) {
            cout << Color::WHITE << t.first << "\tBASE TABLE" << Color::RESET << endl;
        }
        cout << Color::GREEN << databases[currentDB].size() << " rows in set" << Color::RESET << endl;
    }
    
    void executeShowIndex(const string& name) {
        if (databases[currentDB].find(name) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1146 (42S02): Table '" << name << "' doesn't exist" << Color::RESET << endl;
            return;
        }
        
        cout << Color::CYAN << "Table\tNon_unique\tKey_name\tSeq_in_index\tColumn_name\tCollation\tCardinality" << Color::RESET << endl;
        for (const auto& col : databases[currentDB][name].columns) {
            if (col.isIndex || col.isPrimaryKey || col.isUnique) {
                string keyName = col.isPrimaryKey ? "PRIMARY" : (col.isUnique ? "UNIQUE" : col.name);
                cout << Color::WHITE << name << "\t" << (col.isPrimaryKey ? "0" : "1") 
                     << "\t" << keyName << "\t1\t" << col.name << "\tA\t0" << Color::RESET << endl;
            }
        }
    }
    
    void executeDescribe(const string& name) {
        if (databases[currentDB].find(name) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1146 (42S02): Table '" << name << "' doesn't exist" << Color::RESET << endl;
            return;
        }
        
        cout << Color::CYAN << "Field\tType\tNull\tKey\tDefault\tExtra" << Color::RESET << endl;
        for (const auto& col : databases[currentDB][name].columns) {
            string key = "";
            if (col.isPrimaryKey) key = "PRI";
            else if (col.isUnique) key = "UNI";
            else if (col.isIndex) key = "MUL";
            string extra = col.isAutoIncrement ? "auto_increment" : "";
            cout << Color::WHITE << col.name << "\t" << getColumnTypeString(col) 
                 << "\t" << (col.isNullable ? "YES" : "NO")
                 << "\t" << key
                 << "\t" << (col.defaultValue.empty() ? "NULL" : col.defaultValue)
                 << "\t" << extra << Color::RESET << endl;
        }
    }
    
    void executeCreateIndex(const string& indexName, const string& tableName, const string& column, bool unique = false, bool fulltext = false) {
        if (databases[currentDB].find(tableName) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1146 (42S02): Table '" << tableName << "' doesn't exist" << Color::RESET << endl;
            return;
        }
        int idx = getColumnIndex(tableName, column);
        if (idx == -1) {
            cout << Color::RED << "✗ ERROR 1054 (42S22): Unknown column '" << column << "'" << Color::RESET << endl;
            return;
        }
        databases[currentDB][tableName].columns[idx].isIndex = true;
        databases[currentDB][tableName].columns[idx].isUnique = unique;
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeDropIndex(const string& indexName, const string& tableName) {
        if (databases[currentDB].find(tableName) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1146 (42S02): Table '" << tableName << "' doesn't exist" << Color::RESET << endl;
            return;
        }
        for (auto& col : databases[currentDB][tableName].columns) {
            if (col.name == indexName || (col.isIndex && col.name == indexName)) {
                col.isIndex = false;
                col.isUnique = false;
                cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
                saveToDisk();
                return;
            }
        }
        cout << Color::RED << "✗ ERROR 1091 (42000): Can't DROP INDEX '" << indexName << "'; check that index exists" << Color::RESET << endl;
    }
    
    void executeStartTransaction() {
        if (inTransaction) {
            cout << Color::YELLOW << "⚠ Already in transaction" << Color::RESET << endl;
            return;
        }
        inTransaction = true;
        transactionBackup = databases;
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
    }
    
    void executeCommit() {
        if (!inTransaction) {
            cout << Color::YELLOW << "⚠ No transaction to commit" << Color::RESET << endl;
            return;
        }
        inTransaction = false;
        transactionBackup.clear();
        saveToDisk();
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
    }
    
    void executeRollback() {
        if (!inTransaction) {
            cout << Color::YELLOW << "⚠ No transaction to rollback" << Color::RESET << endl;
            return;
        }
        databases = transactionBackup;
        inTransaction = false;
        transactionBackup.clear();
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
    }
    
    void executeCreateUser(const string& username, const string& password) {
        if (users.find(username) != users.end()) {
            cout << Color::RED << "✗ ERROR 1396 (HY000): Operation CREATE USER failed for '" << username << "'@'localhost'" << Color::RESET << endl;
            return;
        }
        users[username] = password;
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
    }
    
    void executeDropUser(const string& username) {
        if (users.find(username) == users.end() || username == "root") {
            cout << Color::RED << "✗ ERROR 1396 (HY000): Operation DROP USER failed for '" << username << "'@'localhost'" << Color::RESET << endl;
            return;
        }
        users.erase(username);
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
    }
    
    void executeRenameUser(const string& oldUser, const string& newUser) {
        if (users.find(oldUser) == users.end()) {
            cout << Color::RED << "✗ ERROR 1396 (HY000): Operation RENAME USER failed for '" << oldUser << "'@'localhost'" << Color::RESET << endl;
            return;
        }
        if (users.find(newUser) != users.end()) {
            cout << Color::RED << "✗ ERROR 1396 (HY000): Operation RENAME USER failed for '" << newUser << "'@'localhost'" << Color::RESET << endl;
            return;
        }
        users[newUser] = users[oldUser];
        users.erase(oldUser);
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
    }
    
    void executeSetPassword(const string& username, const string& password) {
        if (users.find(username) == users.end()) {
            cout << Color::RED << "✗ ERROR 1396 (HY000): Operation SET PASSWORD failed for '" << username << "'@'localhost'" << Color::RESET << endl;
            return;
        }
        users[username] = password;
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
    }
    
    void executeGrant(const string& privileges, const string& onWhat, const string& toUser) {
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
    }
    
    void executeRevoke(const string& privileges, const string& onWhat, const string& fromUser) {
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
    }
    
    void executeShowGrants(const string& username) {
        cout << Color::CYAN << "Grants for " << username << "@localhost" << Color::RESET << endl;
        cout << Color::WHITE << "GRANT ALL PRIVILEGES ON *.* TO '" << username << "'@'localhost' WITH GRANT OPTION" << Color::RESET << endl;
    }
    
    void executeShowPrivileges() {
        cout << Color::CYAN << "Privilege\tContext\tComment" << Color::RESET << endl;
        cout << Color::WHITE << "ALL\t\tAdministration\tAll privileges" << Color::RESET << endl;
        cout << Color::WHITE << "SELECT\t\tTables\t\tSelect rows" << Color::RESET << endl;
        cout << Color::WHITE << "INSERT\t\tTables\t\tInsert rows" << Color::RESET << endl;
        cout << Color::WHITE << "UPDATE\t\tTables\t\tUpdate rows" << Color::RESET << endl;
        cout << Color::WHITE << "DELETE\t\tTables\t\tDelete rows" << Color::RESET << endl;
    }
    
    void executeShowProcessList(bool full = false) {
        cout << Color::CYAN << "Id\tUser\tHost\tdb\tCommand\tTime\tState\tInfo" << Color::RESET << endl;
        cout << Color::WHITE << "1\t" << currentUser << "\tlocalhost\t" << (currentDB.empty() ? "(none)" : currentDB) 
             << "\tQuery\t0\t\t" << (full ? "SHOW FULL PROCESSLIST" : "SHOW PROCESSLIST") << Color::RESET << endl;
    }
    
    void executeKill(int pid) {
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
    }
    
    void executeStatus() {
        cout << Color::TEAL << "--------------------------" << Color::RESET << endl;
        cout << Color::BOLD << Color::PURPLE << "MariaDB Engine — Status" << Color::RESET << endl;
        cout << Color::TEAL << "--------------------------" << Color::RESET << endl;
        cout << Color::GRAY << "Connection ID:    " << Color::ORANGE << "1" << Color::RESET << endl;
        cout << Color::GRAY << "Current database: " << Color::PURPLE << (currentDB.empty() ? "(none)" : currentDB) << Color::RESET << endl;
        cout << Color::GRAY << "Current user:     " << Color::GREEN << currentUser << "@localhost" << Color::RESET << endl;
        cout << Color::GRAY << "Tables:           " << Color::ORANGE << (databases[currentDB].size()) << Color::RESET << endl;
        cout << Color::GRAY << "Transaction:      " << (inTransaction ? Color::GREEN + "Active" : Color::GRAY + "None") << Color::RESET << endl;
        cout << Color::GRAY << "Uptime:           " << Color::ORANGE << "00:00:01" << Color::RESET << endl;
        cout << Color::GRAY << "Server version:   " << Color::MAGENTA << "10.4.28-MariaDB" << Color::RESET << endl;
        cout << Color::GRAY << "Databases:        " << Color::ORANGE << databaseList.size() + 3 << Color::RESET << endl;
        cout << Color::TEAL << "--------------------------" << Color::RESET << endl;
    }
    
    void executeFlushPrivileges() {
        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
    }
    
    void executeExport(const string& tableName, const string& filename, bool withHeader = true) {
        if (databases[currentDB].find(tableName) == databases[currentDB].end()) {
            cout << Color::RED << "✗ ERROR 1146 (42S02): Table '" << tableName << "' doesn't exist" << Color::RESET << endl;
            return;
        }
        
        ofstream file(filename);
        if (!file.is_open()) {
            cout << Color::RED << "✗ ERROR: Cannot create file '" << filename << "'" << Color::RESET << endl;
            return;
        }
        
        auto& table = databases[currentDB][tableName];
        if (withHeader) {
            for (size_t i = 0; i < table.columns.size(); i++) {
                file << table.columns[i].name;
                if (i < table.columns.size() - 1) file << ",";
            }
            file << "\n";
        }
        
        for (const auto& row : table.rows) {
            for (size_t i = 0; i < row.size(); i++) {
                file << row[i];
                if (i < row.size() - 1) file << ",";
            }
            file << "\n";
        }
        
        file.close();
        cout << Color::GREEN << "✓ Query OK, " << table.rows.size() << " rows exported to '" << filename << "'" << Color::RESET << endl;
    }
    
    void executeImport(const string& tableName, const string& filename, bool withHeader = true) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << Color::RED << "✗ ERROR: File '" << filename << "' not found" << Color::RESET << endl;
            return;
        }
        
        if (databases[currentDB].find(tableName) == databases[currentDB].end()) {
            databases[currentDB][tableName] = Table();
        }
        
        string line;
        bool isHeader = withHeader;
        int rowCount = 0;
        auto& table = databases[currentDB][tableName];
        
        while (getline(file, line)) {
            vector<string> row = split(line, ',');
            if (isHeader) {
                // Update column names
                for (size_t i = 0; i < row.size() && i < table.columns.size(); i++) {
                    table.columns[i].name = trim(row[i]);
                }
                isHeader = false;
            } else {
                table.rows.push_back(row);
                rowCount++;
            }
        }
        
        file.close();
        cout << Color::GREEN << "✓ Query OK, " << rowCount << " rows imported from '" << filename << "'" << Color::RESET << endl;
        saveToDisk();
    }
    
    void executeBackup(const string& filename) {
        saveToDisk();
        fs::copy_file("mariadb_data.sql", filename + ".sql", fs::copy_options::overwrite_existing);
        cout << Color::GREEN << "Backup saved to '" << filename << ".sql'" << Color::RESET << endl;
    }
    
    void executeRestore(const string& filename) {
        if (!fs::exists(filename + ".sql")) {
            cout << Color::RED << "✗ ERROR: Backup file '" << filename << ".sql' not found" << Color::RESET << endl;
            return;
        }
        fs::copy_file(filename + ".sql", "mariadb_data.sql", fs::copy_options::overwrite_existing);
        loadFromDisk();
        cout << Color::GREEN << "Restored from '" << filename << ".sql'" << Color::RESET << endl;
    }
    
    void executeShowVariables() {
        cout << Color::CYAN << "Variable_name\tValue" << Color::RESET << endl;
        cout << Color::WHITE << "version\t10.4.28-MariaDB" << Color::RESET << endl;
        cout << Color::WHITE << "datadir\t./" << Color::RESET << endl;
        cout << Color::WHITE << "max_connections\t100" << Color::RESET << endl;
    }
    
    void executeShowStatus() {
        cout << Color::CYAN << "Variable_name\tValue" << Color::RESET << endl;
        cout << Color::WHITE << "Uptime\t60" << Color::RESET << endl;
        cout << Color::WHITE << "Threads_connected\t1" << Color::RESET << endl;
        cout << Color::WHITE << "Questions\t" << queryHistory.size() << Color::RESET << endl;
    }
    
    // ============================================
    // HELP
    // ============================================
    
    void showHelp() {
        cout << Color::BOLD << Color::PURPLE << "MariaDB Engine — Command Reference" << Color::RESET << endl;
        cout << Color::BOLD << Color::TEAL << "  DATABASE:" << Color::RESET << endl;
        cout << Color::YELLOW << "    CREATE DATABASE name" << Color::RESET << endl;
        cout << Color::YELLOW << "    CREATE SCHEMA name" << Color::RESET << endl;
        cout << Color::YELLOW << "    DROP DATABASE name" << Color::RESET << endl;
        cout << Color::YELLOW << "    DROP SCHEMA name" << Color::RESET << endl;
        cout << Color::YELLOW << "    SHOW DATABASES" << Color::RESET << endl;
        cout << Color::YELLOW << "    SHOW SCHEMAS" << Color::RESET << endl;
        cout << Color::YELLOW << "    USE db_name" << Color::RESET << endl;
        cout << Color::BOLD << Color::TEAL << "  TABLE:" << Color::RESET << endl;
        cout << Color::YELLOW << "    CREATE TABLE name (col1 TYPE, col2 TYPE, ...)" << Color::RESET << endl;
        cout << Color::YELLOW << "    CREATE TEMPORARY TABLE name ..." << Color::RESET << endl;
        cout << Color::YELLOW << "    CREATE TABLE IF NOT EXISTS name ..." << Color::RESET << endl;
        cout << Color::YELLOW << "    DROP TABLE name" << Color::RESET << endl;
        cout << Color::YELLOW << "    DROP TABLE IF EXISTS name" << Color::RESET << endl;
        cout << Color::YELLOW << "    TRUNCATE TABLE name" << Color::RESET << endl;
        cout << Color::YELLOW << "    ALTER TABLE name ADD COLUMN col TYPE" << Color::RESET << endl;
        cout << Color::YELLOW << "    ALTER TABLE name DROP COLUMN col" << Color::RESET << endl;
        cout << Color::YELLOW << "    ALTER TABLE name MODIFY COLUMN col TYPE" << Color::RESET << endl;
        cout << Color::YELLOW << "    ALTER TABLE name CHANGE COLUMN old new TYPE" << Color::RESET << endl;
        cout << Color::YELLOW << "    ALTER TABLE name RENAME COLUMN old TO new" << Color::RESET << endl;
        cout << Color::YELLOW << "    ALTER TABLE name RENAME TO new_name" << Color::RESET << endl;
        cout << Color::YELLOW << "    ALTER TABLE name ADD INDEX idx (col)" << Color::RESET << endl;
        cout << Color::YELLOW << "    ALTER TABLE name DROP INDEX idx" << Color::RESET << endl;
        cout << Color::YELLOW << "    ALTER TABLE name ADD PRIMARY KEY (col)" << Color::RESET << endl;
        cout << Color::YELLOW << "    ALTER TABLE name DROP PRIMARY KEY" << Color::RESET << endl;
        cout << Color::YELLOW << "    RENAME TABLE old TO new" << Color::RESET << endl;
        cout << Color::YELLOW << "    SHOW TABLES" << Color::RESET << endl;
        cout << Color::YELLOW << "    SHOW FULL TABLES" << Color::RESET << endl;
        cout << Color::YELLOW << "    DESCRIBE name" << Color::RESET << endl;
        cout << Color::YELLOW << "    DESC name" << Color::RESET << endl;
        cout << Color::BOLD << Color::TEAL << "  DATA:" << Color::RESET << endl;
        cout << Color::YELLOW << "    INSERT INTO name VALUES (val1, val2, ...)" << Color::RESET << endl;
        cout << Color::YELLOW << "    INSERT INTO name VALUES (...), (...), ..." << Color::RESET << endl;
        cout << Color::YELLOW << "    INSERT IGNORE INTO name ..." << Color::RESET << endl;
        cout << Color::YELLOW << "    REPLACE INTO name VALUES ..." << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT * FROM name" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT col1, col2 FROM name" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT DISTINCT col FROM name" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT ... WHERE condition" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT ... WHERE col LIKE '%pattern%'" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT ... WHERE col IN (val1, val2)" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT ... WHERE col BETWEEN val1 AND val2" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT ... WHERE col IS NULL" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT ... ORDER BY col [ASC|DESC]" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT ... LIMIT n [OFFSET o]" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT ... GROUP BY col" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT ... HAVING condition" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT COUNT(*) FROM name" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT SUM(col), AVG(col), MIN(col), MAX(col) FROM name" << Color::RESET << endl;
        cout << Color::YELLOW << "    UPDATE name SET col=val [WHERE condition]" << Color::RESET << endl;
        cout << Color::YELLOW << "    DELETE FROM name [WHERE condition]" << Color::RESET << endl;
        cout << Color::BOLD << Color::TEAL << "  JOINS:" << Color::RESET << endl;
        cout << Color::YELLOW << "    INNER JOIN ... ON condition" << Color::RESET << endl;
        cout << Color::YELLOW << "    LEFT JOIN ... ON condition" << Color::RESET << endl;
        cout << Color::YELLOW << "    RIGHT JOIN ... ON condition" << Color::RESET << endl;
        cout << Color::YELLOW << "    CROSS JOIN ..." << Color::RESET << endl;
        cout << Color::BOLD << Color::TEAL << "  INDEX:" << Color::RESET << endl;
        cout << Color::YELLOW << "    CREATE INDEX name ON table (col)" << Color::RESET << endl;
        cout << Color::YELLOW << "    CREATE UNIQUE INDEX name ON table (col)" << Color::RESET << endl;
        cout << Color::YELLOW << "    CREATE FULLTEXT INDEX name ON table (col)" << Color::RESET << endl;
        cout << Color::YELLOW << "    DROP INDEX name ON table" << Color::RESET << endl;
        cout << Color::YELLOW << "    SHOW INDEX FROM table" << Color::RESET << endl;
        cout << Color::YELLOW << "    ANALYZE TABLE table" << Color::RESET << endl;
        cout << Color::BOLD << Color::TEAL << "  TRANSACTION:" << Color::RESET << endl;
        cout << Color::YELLOW << "    START TRANSACTION" << Color::RESET << endl;
        cout << Color::YELLOW << "    BEGIN" << Color::RESET << endl;
        cout << Color::YELLOW << "    COMMIT" << Color::RESET << endl;
        cout << Color::YELLOW << "    ROLLBACK" << Color::RESET << endl;
        cout << Color::YELLOW << "    SAVEPOINT name" << Color::RESET << endl;
        cout << Color::BOLD << Color::TEAL << "  USER:" << Color::RESET << endl;
        cout << Color::YELLOW << "    CREATE USER 'user'@'localhost' IDENTIFIED BY 'pass'" << Color::RESET << endl;
        cout << Color::YELLOW << "    DROP USER 'user'@'localhost'" << Color::RESET << endl;
        cout << Color::YELLOW << "    RENAME USER old TO new" << Color::RESET << endl;
        cout << Color::YELLOW << "    SET PASSWORD FOR 'user'@'localhost' = PASSWORD('pass')" << Color::RESET << endl;
        cout << Color::YELLOW << "    GRANT ALL ON *.* TO 'user'@'localhost'" << Color::RESET << endl;
        cout << Color::YELLOW << "    REVOKE ALL ON *.* FROM 'user'@'localhost'" << Color::RESET << endl;
        cout << Color::YELLOW << "    SHOW GRANTS FOR 'user'@'localhost'" << Color::RESET << endl;
        cout << Color::YELLOW << "    SHOW PRIVILEGES" << Color::RESET << endl;
        cout << Color::YELLOW << "    FLUSH PRIVILEGES" << Color::RESET << endl;
        cout << Color::BOLD << Color::TEAL << "  SYSTEM:" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT USER()" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT CURRENT_USER()" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT SYSTEM_USER()" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT SESSION_USER()" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT VERSION()" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT DATABASE()" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT SCHEMA()" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT NOW()" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT CURRENT_TIMESTAMP" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT CURDATE()" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT CURTIME()" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT CURRENT_DATE" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT CURRENT_TIME" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT UNIX_TIMESTAMP()" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT FROM_UNIXTIME(1234567890)" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT DATE_ADD(NOW(), INTERVAL 1 DAY)" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT DATE_SUB(NOW(), INTERVAL 1 DAY)" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT DATEDIFF(date1, date2)" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT TIMEDIFF(time1, time2)" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT DATE_FORMAT(NOW(), '%Y-%m-%d')" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT TIME_FORMAT(NOW(), '%H:%i:%s')" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT CONCAT('Hello', ' ', 'World')" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT CONCAT_WS('-', '2026', '06', '21')" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT UPPER('hello')" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT LOWER('HELLO')" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT LENGTH('hello')" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT SUBSTRING('hello', 2, 3)" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT TRIM('  hello  ')" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT REPLACE('hello', 'l', 'x')" << Color::RESET << endl;
        cout << Color::YELLOW << "    SELECT RAND()" << Color::RESET << endl;
        cout << Color::BOLD << Color::TEAL << "  PROCESS:" << Color::RESET << endl;
        cout << Color::YELLOW << "    SHOW PROCESSLIST" << Color::RESET << endl;
        cout << Color::YELLOW << "    SHOW FULL PROCESSLIST" << Color::RESET << endl;
        cout << Color::YELLOW << "    KILL id" << Color::RESET << endl;
        cout << Color::BOLD << Color::TEAL << "  UTILITY:" << Color::RESET << endl;
        cout << Color::YELLOW << "    STATUS" << Color::RESET << endl;
        cout << Color::YELLOW << "    HELP" << Color::RESET << endl;
        cout << Color::YELLOW << "    CLEAR / CLS" << Color::RESET << endl;
        cout << Color::YELLOW << "    EXPORT table TO 'file.csv'" << Color::RESET << endl;
        cout << Color::YELLOW << "    EXPORT table TO 'file.csv' WITH HEADER" << Color::RESET << endl;
        cout << Color::YELLOW << "    IMPORT table FROM 'file.csv'" << Color::RESET << endl;
        cout << Color::YELLOW << "    IMPORT table FROM 'file.csv' WITH HEADER" << Color::RESET << endl;
        cout << Color::YELLOW << "    BACKUP 'filename'" << Color::RESET << endl;
        cout << Color::YELLOW << "    RESTORE 'filename'" << Color::RESET << endl;
        cout << Color::YELLOW << "    SOURCE 'script.sql'" << Color::RESET << endl;
        cout << Color::YELLOW << "    SHOW VARIABLES" << Color::RESET << endl;
        cout << Color::YELLOW << "    SHOW STATUS" << Color::RESET << endl;
        cout << Color::YELLOW << "    SHOW ENGINE INNODB STATUS" << Color::RESET << endl;
        cout << Color::YELLOW << "    EXIT / QUIT" << Color::RESET << endl;
    }
    
    // ============================================
    // STORAGE
    // ============================================
    
    void saveToDisk() {
        ofstream file("mariadb_data.sql");
        if (!file.is_open()) {
            cout << Color::RED << "✗ ERROR: Cannot save to disk" << Color::RESET << endl;
            return;
        }
        
        file << "-- ============================================" << endl;
        file << "-- MARIADB DATA" << endl;
        file << "-- Generated: " << getCurrentTimestamp() << endl;
        file << "-- ============================================" << endl << endl;
        
        for (const auto& db : databaseList) {
            file << "-- DATABASE: " << db << endl;
            file << "-- TABLES: ";
            bool first = true;
            for (const auto& t : databases[db]) {
                if (!first) file << ",";
                file << t.first;
                first = false;
            }
            file << endl << endl;
            
            for (const auto& t : databases[db]) {
                file << "-- TABLE: " << t.first << endl;
                file << "-- COLUMNS: ";
                for (size_t i = 0; i < t.second.columns.size(); i++) {
                    if (i > 0) file << ", ";
                    file << t.second.columns[i].name << "|" << getColumnTypeString(t.second.columns[i]);
                    if (t.second.columns[i].isPrimaryKey) file << "|PRIMARY";
                    if (t.second.columns[i].isAutoIncrement) file << "|AUTO_INCREMENT";
                    if (t.second.columns[i].isUnique) file << "|UNIQUE";
                    if (!t.second.columns[i].isNullable) file << "|NOT NULL";
                }
                file << endl;
                file << "-- ROWS:" << endl;
                for (const auto& row : t.second.rows) {
                    for (size_t i = 0; i < row.size(); i++) {
                        file << row[i];
                        if (i < row.size() - 1) file << "|";
                    }
                    file << endl;
                }
                file << endl;
            }
        }
        
        file.close();
    }
    
    void loadFromDisk() {
        if (!fs::exists("mariadb_data.sql")) {
            // Initialize with default test database
            databaseList.insert("test");
            databases["test"] = map<string, Table>();
            currentDB = "test";
            return;
        }
        
        ifstream file("mariadb_data.sql");
        if (!file.is_open()) {
            databaseList.insert("test");
            databases["test"] = map<string, Table>();
            currentDB = "test";
            return;
        }
        
        databases.clear();
        databaseList.clear();
        
        string line;
        string currentDBName = "";
        string currentTableName = "";
        bool readingRows = false;
        vector<string> currentColumns;
        
        while (getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '-' || line[0] == '=') continue;
            
            if (line.find("-- DATABASE:") == 0) {
                currentDBName = trim(line.substr(12));
                databaseList.insert(currentDBName);
                databases[currentDBName] = map<string, Table>();
                currentTableName = "";
                readingRows = false;
            }
            else if (line.find("-- TABLE:") == 0) {
                currentTableName = trim(line.substr(9));
                databases[currentDBName][currentTableName] = Table();
                readingRows = false;
            }
            else if (line.find("-- COLUMNS:") == 0) {
                string colsStr = line.substr(11);
                vector<string> colDefs = split(colsStr, ',');
                for (auto& def : colDefs) {
                    def = trim(def);
                    Column col;
                    vector<string> parts = split(def, '|');
                    if (!parts.empty()) {
                        col.name = parts[0];
                        if (parts.size() > 1) {
                            col.type = parts[1];
                            // Parse type with length
                            size_t parenPos = col.type.find('(');
                            if (parenPos != string::npos) {
                                size_t parenEnd = col.type.find(')');
                                if (parenEnd != string::npos) {
                                    string lenStr = col.type.substr(parenPos + 1, parenEnd - parenPos - 1);
                                    try { col.length = stoi(lenStr); } catch(...) {}
                                    col.type = col.type.substr(0, parenPos);
                                }
                            }
                        } else {
                            col.type = "TEXT";
                        }
                        for (size_t i = 2; i < parts.size(); i++) {
                            string opt = toUpper(parts[i]);
                            if (opt == "PRIMARY") col.isPrimaryKey = true;
                            else if (opt == "AUTO_INCREMENT") col.isAutoIncrement = true;
                            else if (opt == "UNIQUE") col.isUnique = true;
                            else if (opt == "NOT NULL") col.isNullable = false;
                        }
                        databases[currentDBName][currentTableName].columns.push_back(col);
                    }
                }
                currentColumns = colDefs;
                readingRows = false;
            }
            else if (line.find("-- ROWS:") == 0) {
                readingRows = true;
            }
            else if (readingRows && !currentTableName.empty()) {
                vector<string> row = split(line, '|');
                if (!row.empty()) {
                    databases[currentDBName][currentTableName].rows.push_back(row);
                }
            }
        }
        
        file.close();
        
        // Set currentDB to test or first database
        if (databaseList.find("test") != databaseList.end()) {
            currentDB = "test";
        } else if (!databaseList.empty()) {
            currentDB = *databaseList.begin();
        } else {
            databaseList.insert("test");
            databases["test"] = map<string, Table>();
            currentDB = "test";
        }
    }
    
public:
    // ============================================
    // CONSTRUCTOR & DESTRUCTOR
    // ============================================
    
    MariaDB() {
        srand(time(NULL));
        loadFromDisk();
    }
    
    ~MariaDB() {
        saveToDisk();
    }
    
    // ============================================
    // LOGIN
    // ============================================
    
    bool login() {
        const int maxAttempts = 3;
        for (int attempt = 1; attempt <= maxAttempts; attempt++) {
            string password;

            cout << Color::TEAL << "Enter password: " << Color::RESET;
            getline(cin, password);

            if (users.find("root") != users.end() && users["root"] == password) {
                currentUser = "root";
                isLoggedIn = true;

                cout << endl << Color::GREEN << "✓ Welcome to the MariaDB monitor.  Commands end with ; or \\g." << Color::RESET << endl;
                cout << Color::CYAN << "Your MariaDB connection id is 1" << Color::RESET << endl;
                cout << Color::CYAN << "Server version: " << Color::MAGENTA << "10.4.28-MariaDB Engine" << Color::RESET << endl << endl;
                cout << Color::GRAY << "Type 'help;' or '\\h' for help. Type '\\c' to clear the current input statement." << Color::RESET << endl << endl;
                return true;
            }

            int remaining = maxAttempts - attempt;
            cout << Color::RED << "✗ ERROR 1045 (28000): Access denied for user 'root'@'localhost' (using password: YES)" << Color::RESET << endl;
            if (remaining > 0) {
                cout << Color::GRAY << "  (" << remaining << (remaining == 1 ? " attempt" : " attempts") << " remaining)" << Color::RESET << endl;
            }
        }
        return false;
    }
    
    // ============================================
    // GET PROMPT
    // ============================================
    
    string getPrompt() {
        if (currentDB.empty()) {
            return Color::TEAL + "MariaDB " + Color::GRAY + "[(none)]" + Color::TEAL + "> " + Color::RESET;
        }
        return Color::TEAL + "MariaDB " + Color::GRAY + "[" + Color::PURPLE + currentDB + Color::GRAY + "]" + Color::TEAL + "> " + Color::RESET;
    }
    
    // ============================================
    // EXECUTE SQL
    // ============================================
    
    void executeSQL(const string& query) {
        if (!isLoggedIn) {
            cout << Color::RED << "Please login first" << Color::RESET << endl;
            return;
        }
        
        queryHistory.push_back(query);
        
        string upper = toUpper(query);
        
        // ============================================
        // SPECIAL COMMANDS
        // ============================================
        if (upper == "EXIT" || upper == "QUIT" || upper == "\\Q") {
            saveToDisk();
            cout << Color::YELLOW << "Bye" << Color::RESET << endl;
            exit(0);
        }
        
        if (upper == "HELP" || upper == "\\H" || upper == "\\?") {
            showHelp();
            return;
        }
        
        if (upper == "CLEAR" || upper == "CLS" || upper == "\\C") {
            if (system(CLEAR_SCREEN) != 0) { /* ignore: non-fatal if clear fails */ }
            return;
        }
        
        if (upper == "SHOW TABLES") {
            executeShowTables();
            return;
        }
        
        if (upper == "SHOW FULL TABLES") {
            executeShowFullTables();
            return;
        }
        
        if (upper == "STATUS" || upper == "\\S") {
            executeStatus();
            return;
        }
        
        if (upper == "START TRANSACTION" || upper == "BEGIN" || upper == "BEGIN WORK") {
            executeStartTransaction();
            return;
        }
        
        if (upper == "COMMIT") {
            executeCommit();
            return;
        }
        
        if (upper == "ROLLBACK") {
            executeRollback();
            return;
        }
        
        if (upper == "SHOW PROCESSLIST") {
            executeShowProcessList(false);
            return;
        }
        
        if (upper == "SHOW FULL PROCESSLIST") {
            executeShowProcessList(true);
            return;
        }
        
        if (upper == "FLUSH PRIVILEGES") {
            executeFlushPrivileges();
            return;
        }
        
        if (upper == "SHOW DATABASES" || upper == "SHOW SCHEMAS") {
            executeShowDatabases();
            return;
        }
        
        if (upper == "SHOW PRIVILEGES") {
            executeShowPrivileges();
            return;
        }
        
        if (upper == "SHOW VARIABLES") {
            executeShowVariables();
            return;
        }
        
        if (upper == "SHOW STATUS") {
            executeShowStatus();
            return;
        }
        
        // ============================================
        // SELECT FUNCTIONS (single row)
        // ============================================
        if (upper.find("SELECT USER()") != string::npos) {
            printSingleRow("USER()", executeUser());
            return;
        }
        
        if (upper.find("SELECT CURRENT_USER()") != string::npos) {
            printSingleRow("CURRENT_USER()", executeCurrentUser());
            return;
        }
        
        if (upper.find("SELECT SYSTEM_USER()") != string::npos) {
            printSingleRow("SYSTEM_USER()", executeSystemUser());
            return;
        }
        
        if (upper.find("SELECT SESSION_USER()") != string::npos) {
            printSingleRow("SESSION_USER()", executeSessionUser());
            return;
        }
        
        if (upper.find("SELECT VERSION()") != string::npos) {
            printSingleRow("VERSION()", executeVersion());
            return;
        }
        
        if (upper.find("SELECT DATABASE()") != string::npos) {
            printSingleRow("DATABASE()", executeDatabase());
            return;
        }
        
        if (upper.find("SELECT SCHEMA()") != string::npos) {
            printSingleRow("SCHEMA()", executeSchema());
            return;
        }
        
        if (upper.find("SELECT NOW()") != string::npos) {
            printSingleRow("NOW()", executeNow());
            return;
        }
        
        if (upper.find("SELECT CURRENT_TIMESTAMP") != string::npos) {
            printSingleRow("CURRENT_TIMESTAMP", executeCurrentTimestamp());
            return;
        }
        
        if (upper.find("SELECT CURDATE()") != string::npos) {
            printSingleRow("CURDATE()", executeCurDate());
            return;
        }
        
        if (upper.find("SELECT CURTIME()") != string::npos) {
            printSingleRow("CURTIME()", executeCurTime());
            return;
        }
        
        if (upper.find("SELECT CURRENT_DATE") != string::npos) {
            printSingleRow("CURRENT_DATE", executeCurrentDate());
            return;
        }
        
        if (upper.find("SELECT CURRENT_TIME") != string::npos) {
            printSingleRow("CURRENT_TIME", executeCurrentTime());
            return;
        }
        
        if (upper.find("SELECT UNIX_TIMESTAMP()") != string::npos) {
            printSingleRow("UNIX_TIMESTAMP()", executeUnixTimestamp());
            return;
        }
        
        if (upper.find("SELECT RAND()") != string::npos) {
            printSingleRow("RAND()", executeRand());
            return;
        }
        
        // ============================================
        // CREATE/DROP DATABASE
        // ============================================
        if (upper.find("CREATE DATABASE") == 0 || upper.find("CREATE SCHEMA") == 0) {
            size_t firstSpace = query.find(' ');
            size_t secondSpace = query.find(' ', firstSpace + 1);
            string rest = (secondSpace != string::npos) ? query.substr(secondSpace + 1) : "";
            rest = trim(rest);
            stripTrailingSemicolon(rest);
            // Check IF NOT EXISTS
            if (toUpper(rest).find("IF NOT EXISTS") == 0) {
                string name = trim(rest.substr(13));
                if (databaseList.find(name) != databaseList.end()) {
                    cout << Color::YELLOW << "Query OK, 0 rows affected" << Color::RESET << endl;
                    return;
                }
                executeCreateDatabase(name);
            } else {
                executeCreateDatabase(rest);
            }
            return;
        }
        
        if (upper.find("DROP DATABASE") == 0 || upper.find("DROP SCHEMA") == 0) {
            size_t firstSpace = query.find(' ');
            size_t secondSpace = query.find(' ', firstSpace + 1);
            string rest = (secondSpace != string::npos) ? query.substr(secondSpace + 1) : "";
            rest = trim(rest);
            stripTrailingSemicolon(rest);
            // Check IF EXISTS
            if (toUpper(rest).find("IF EXISTS") == 0) {
                string name = trim(rest.substr(9));
                if (databaseList.find(name) == databaseList.end()) {
                    cout << Color::YELLOW << "Query OK, 0 rows affected" << Color::RESET << endl;
                    return;
                }
                executeDropDatabase(name);
            } else {
                executeDropDatabase(rest);
            }
            return;
        }
        
        // ============================================
        // USE
        // ============================================
        if (upper.find("USE ") == 0) {
            string db = trim(query.substr(3));
            stripTrailingSemicolon(db);
            executeUse(db);
            return;
        }
        
        // ============================================
        // PARSE COMPLEX COMMANDS
        // ============================================
        string lower = toLower(query);
        size_t pos;
        
        // ============================================
        // SHOW GRANTS
        // ============================================
        if (lower.find("show grants") == 0) {
            string rest = query.substr(11);
            if (ifindCI(rest, "FOR") != string::npos) {
                string user = trim(rest.substr(ifindCI(rest, "FOR") + 3));
                stripQuotes(user);
                stripTrailingSemicolon(user);
                executeShowGrants(user);
                return;
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // SHOW ENGINE
        // ============================================
        if (lower.find("show engine") == 0) {
            cout << Color::YELLOW << "Engine: INNODB" << Color::RESET << endl;
            cout << Color::WHITE << "Status: OK" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // CREATE TABLE
        // ============================================
        if (lower.find("create table") == 0 || lower.find("create temporary table") == 0) {
            bool isTemporary = lower.find("create temporary table") == 0;
            string rest = isTemporary ? query.substr(22) : query.substr(12);
            bool ifNotExists = false;
            
            if (lower.find("if not exists") != string::npos) {
                ifNotExists = true;
                rest = rest.substr(ifindCI(rest, "if not exists") + 13);
            }
            
            rest = trim(rest);
            pos = rest.find('(');
            if (pos != string::npos) {
                string name = trim(rest.substr(0, pos));
                string cols = rest.substr(pos + 1);
                // Use rfind (not find) for the closing paren: column type definitions
                // like VARCHAR(100) contain their own parentheses, so the FIRST ')'
                // in `cols` would prematurely close the column list.
                size_t closePos = cols.rfind(')');
                if (closePos != string::npos) {
                    cols = cols.substr(0, closePos);
                    if (ifNotExists && databases[currentDB].find(name) != databases[currentDB].end()) {
                        cout << Color::YELLOW << "Query OK, 0 rows affected" << Color::RESET << endl;
                        return;
                    }
                    if (isTemporary) {
                        cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
                        return;
                    }
                    executeCreateTable(name, cols);
                    return;
                }
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // DROP TABLE
        // ============================================
        if (lower.find("drop table") == 0) {
            string rest = query.substr(10);
            bool ifExists = false;
            if (lower.find("if exists") != string::npos) {
                ifExists = true;
                rest = rest.substr(ifindCI(rest, "if exists") + 9);
            }
            string name = trim(rest);
            stripTrailingSemicolon(name);
            executeDropTable(name, ifExists);
            return;
        }
        
        // ============================================
        // TRUNCATE TABLE
        // ============================================
        if (lower.find("truncate table") == 0) {
            string name = trim(query.substr(14));
            stripTrailingSemicolon(name);
            executeTruncateTable(name);
            return;
        }
        
        // ============================================
        // RENAME TABLE
        // ============================================
        if (lower.find("rename table") == 0) {
            string rest = query.substr(12);
            pos = ifindCI(rest, " to ");
            if (pos != string::npos) {
                string oldName = trim(rest.substr(0, pos));
                string newName = trim(rest.substr(pos + 4));
                stripTrailingSemicolon(newName);
                executeRenameTable(oldName, newName);
                return;
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // ALTER TABLE
        // ============================================
        if (lower.find("alter table") == 0) {
            string rest = query.substr(11);
            string name = trim(rest.substr(0, rest.find(' ')));
            rest = trim(rest.substr(rest.find(' ') + 1));
            string restLower = toLower(rest);
            
            // ADD COLUMN
            if (restLower.find("add") == 0) {
                string colPart = trim(rest.substr(3));
                if (restLower.find("add column") == 0) {
                    colPart = trim(colPart.substr(6));
                }
                stripTrailingSemicolon(colPart);
                executeAlterTableAddColumn(name, colPart);
                return;
            }
            
            // DROP COLUMN
            if (restLower.find("drop") == 0) {
                string colPart = trim(rest.substr(4));
                if (restLower.find("drop column") == 0) {
                    colPart = trim(colPart.substr(5));
                }
                stripTrailingSemicolon(colPart);
                executeAlterTableDropColumn(name, colPart);
                return;
            }
            
            // MODIFY COLUMN
            if (restLower.find("modify") == 0) {
                string colPart = trim(rest.substr(6));
                if (restLower.find("modify column") == 0) {
                    colPart = trim(colPart.substr(7));
                }
                stripTrailingSemicolon(colPart);
                executeAlterTableModifyColumn(name, colPart);
                return;
            }
            
            // RENAME TO
            if (restLower.find("rename to") == 0) {
                string newName = trim(rest.substr(8));
                stripTrailingSemicolon(newName);
                executeRenameTable(name, newName);
                return;
            }
            
            // ADD INDEX
            if (restLower.find("add index") == 0) {
                string rest2 = rest.substr(9);
                size_t parenPos = rest2.find('(');
                if (parenPos != string::npos) {
                    string indexName = trim(rest2.substr(0, parenPos));
                    string colName = trim(rest2.substr(parenPos + 1));
                    size_t parenEnd = colName.find(')');
                    if (parenEnd != string::npos) {
                        colName = trim(colName.substr(0, parenEnd));
                        executeCreateIndex(indexName, name, colName);
                        return;
                    }
                }
            }
            
            // DROP INDEX
            if (restLower.find("drop index") == 0) {
                string indexName = trim(rest.substr(10));
                stripTrailingSemicolon(indexName);
                executeDropIndex(indexName, name);
                return;
            }
            
            // ADD PRIMARY KEY
            if (restLower.find("add primary key") == 0) {
                string colPart = rest.substr(15);
                size_t parenPos = colPart.find('(');
                if (parenPos != string::npos) {
                    string colName = trim(colPart.substr(parenPos + 1));
                    size_t parenEnd = colName.find(')');
                    if (parenEnd != string::npos) {
                        colName = trim(colName.substr(0, parenEnd));
                        executeCreateIndex("PRIMARY", name, colName);
                        return;
                    }
                }
            }
            
            // DROP PRIMARY KEY
            if (restLower.find("drop primary key") == 0) {
                executeDropIndex("PRIMARY", name);
                return;
            }
            
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // DESCRIBE / DESC
        // ============================================
        if (lower.find("describe") == 0 || lower.find("desc") == 0) {
            string name = trim(query.substr(query.find(' ') + 1));
            stripTrailingSemicolon(name);
            executeDescribe(name);
            return;
        }
        
        // ============================================
        // SHOW INDEX FROM
        // ============================================
        if (lower.find("show index from") == 0) {
            string name = trim(query.substr(15));
            stripTrailingSemicolon(name);
            executeShowIndex(name);
            return;
        }
        
        // ============================================
        // ANALYZE TABLE
        // ============================================
        if (lower.find("analyze table") == 0) {
            string name = trim(query.substr(13));
            stripTrailingSemicolon(name);
            cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // CREATE INDEX
        // ============================================
        if (lower.find("create index") == 0) {
            string rest = query.substr(12);
            bool unique = false;
            bool fulltext = false;
            
            if (lower.find("unique") != string::npos) {
                unique = true;
                rest = rest.substr(ifindCI(rest, "unique") + 6);
            }
            if (lower.find("fulltext") != string::npos) {
                fulltext = true;
                rest = rest.substr(ifindCI(rest, "fulltext") + 8);
            }
            
            if (lower.find("if not exists") != string::npos) {
                rest = rest.substr(ifindCI(rest, "if not exists") + 13);
            }
            
            pos = ifindCI(rest, " on ");
            if (pos != string::npos) {
                string indexName = trim(rest.substr(0, pos));
                string tablePart = rest.substr(pos + 4);
                pos = tablePart.find('(');
                if (pos != string::npos) {
                    string tableName = trim(tablePart.substr(0, pos));
                    string column = tablePart.substr(pos + 1);
                    pos = column.find(')');
                    if (pos != string::npos) {
                        column = trim(column.substr(0, pos));
                        stripTrailingSemicolon(column);
                        executeCreateIndex(indexName, tableName, column, unique, fulltext);
                        return;
                    }
                }
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // DROP INDEX
        // ============================================
        if (lower.find("drop index") == 0) {
            string rest = query.substr(10);
            if (lower.find("if exists") != string::npos) {
                rest = rest.substr(ifindCI(rest, "if exists") + 9);
            }
            pos = ifindCI(rest, " on ");
            if (pos != string::npos) {
                string indexName = trim(rest.substr(0, pos));
                string tableName = trim(rest.substr(pos + 4));
                stripTrailingSemicolon(tableName);
                executeDropIndex(indexName, tableName);
                return;
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // CREATE USER
        // ============================================
        if (lower.find("create user") == 0) {
            string rest = query.substr(11);
            if (lower.find("if not exists") != string::npos) {
                rest = rest.substr(ifindCI(rest, "if not exists") + 13);
            }
            size_t atPos = rest.find('@');
            if (atPos != string::npos) {
                string username = trim(rest.substr(0, atPos));
                stripQuotes(username);
                pos = ifindCI(rest, "identified by");
                if (pos != string::npos) {
                    string passPart = rest.substr(pos + 13);
                    passPart = trim(passPart);
                    stripQuotes(passPart);
                    stripTrailingSemicolon(passPart);
                    executeCreateUser(username, passPart);
                    return;
                }
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // DROP USER
        // ============================================
        if (lower.find("drop user") == 0) {
            string rest = query.substr(9);
            if (lower.find("if exists") != string::npos) {
                rest = rest.substr(ifindCI(rest, "if exists") + 9);
            }
            rest = trim(rest);
            size_t atPos = rest.find('@');
            if (atPos != string::npos) {
                string username = trim(rest.substr(0, atPos));
                stripQuotes(username);
                stripTrailingSemicolon(username);
                executeDropUser(username);
                return;
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // RENAME USER
        // ============================================
        if (lower.find("rename user") == 0) {
            string rest = query.substr(11);
            pos = ifindCI(rest, " to ");
            if (pos != string::npos) {
                string oldUser = trim(rest.substr(0, pos));
                string newUser = trim(rest.substr(pos + 4));
                stripQuotes(oldUser);
                stripQuotes(newUser);
                stripTrailingSemicolon(newUser);
                executeRenameUser(oldUser, newUser);
                return;
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // SET PASSWORD
        // ============================================
        if (lower.find("set password") == 0) {
            string rest = query.substr(12);
            pos = ifindCI(rest, " for ");
            if (pos != string::npos) {
                string userPart = rest.substr(pos + 4);
                pos = userPart.find(" = ");
                if (pos != string::npos) {
                    string user = trim(userPart.substr(0, pos));
                    size_t atPos = user.find('@');
                    if (atPos != string::npos) {
                        string username = trim(user.substr(0, atPos));
                        stripQuotes(username);
                        string passPart = userPart.substr(pos + 3);
                        passPart = trim(passPart);
                        size_t pwFnPos = ifindCI(passPart, "PASSWORD");
                        if (pwFnPos != string::npos) {
                            size_t start = passPart.find('(', pwFnPos);
                            size_t end = passPart.find(')', start);
                            if (start != string::npos && end != string::npos) {
                                passPart = passPart.substr(start + 1, end - start - 1);
                            }
                        }
                        stripQuotes(passPart);
                        stripTrailingSemicolon(passPart);
                        executeSetPassword(username, passPart);
                        return;
                    }
                }
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // GRANT
        // ============================================
        if (lower.find("grant") == 0) {
            string rest = query.substr(5);
            pos = ifindCI(rest, " on ");
            if (pos != string::npos) {
                string priv = trim(rest.substr(0, pos));
                rest = rest.substr(pos + 4);
                pos = ifindCI(rest, " to ");
                if (pos != string::npos) {
                    string dbTable = trim(rest.substr(0, pos));
                    string user = trim(rest.substr(pos + 4));
                    size_t atPos = user.find('@');
                    if (atPos != string::npos) {
                        string username = trim(user.substr(0, atPos));
                        stripQuotes(username);
                        executeGrant(priv, dbTable, username);
                        return;
                    }
                }
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // REVOKE
        // ============================================
        if (lower.find("revoke") == 0) {
            string rest = query.substr(6);
            pos = ifindCI(rest, " on ");
            if (pos != string::npos) {
                string priv = trim(rest.substr(0, pos));
                rest = rest.substr(pos + 4);
                pos = ifindCI(rest, " from ");
                if (pos != string::npos) {
                    string dbTable = trim(rest.substr(0, pos));
                    string user = trim(rest.substr(pos + 5));
                    size_t atPos = user.find('@');
                    if (atPos != string::npos) {
                        string username = trim(user.substr(0, atPos));
                        stripQuotes(username);
                        executeRevoke(priv, dbTable, username);
                        return;
                    }
                }
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // KILL
        // ============================================
        if (lower.find("kill") == 0) {
            string pidStr = trim(query.substr(4));
            if (lower.find("connection") != string::npos) {
                pidStr = trim(query.substr(ifindCI(query, "connection") + 10));
            }
            stripTrailingSemicolon(pidStr);
            try {
                int pid = stoi(pidStr);
                executeKill(pid);
                return;
            } catch(...) {
                cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
                return;
            }
        }
        
        // ============================================
        // EXPORT / IMPORT
        // ============================================
        if (lower.find("export") == 0) {
            string rest = query.substr(6);
            bool withHeader = false;
            if (lower.find("with header") != string::npos) {
                withHeader = true;
                rest = rest.substr(0, ifindCI(rest, " with header"));
            }
            pos = ifindCI(rest, " to ");
            if (pos != string::npos) {
                string tableName = trim(rest.substr(0, pos));
                string filename = rest.substr(pos + 4);
                stripQuotes(filename);
                stripTrailingSemicolon(filename);
                executeExport(tableName, filename, withHeader);
                return;
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        if (lower.find("import") == 0) {
            string rest = query.substr(6);
            bool withHeader = false;
            if (lower.find("with header") != string::npos) {
                withHeader = true;
                rest = rest.substr(0, ifindCI(rest, " with header"));
            }
            pos = ifindCI(rest, " from ");
            if (pos != string::npos) {
                string tableName = trim(rest.substr(0, pos));
                string filename = rest.substr(pos + 5);
                stripQuotes(filename);
                stripTrailingSemicolon(filename);
                executeImport(tableName, filename, withHeader);
                return;
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // BACKUP / RESTORE
        // ============================================
        if (lower.find("backup") == 0) {
            string filename = trim(query.substr(6));
            stripQuotes(filename);
            stripTrailingSemicolon(filename);
            executeBackup(filename);
            return;
        }
        
        if (lower.find("restore") == 0) {
            string filename = trim(query.substr(7));
            stripQuotes(filename);
            stripTrailingSemicolon(filename);
            executeRestore(filename);
            return;
        }
        
        // ============================================
        // SOURCE
        // ============================================
        if (lower.find("source") == 0) {
            string filename = trim(query.substr(6));
            stripQuotes(filename);
            stripTrailingSemicolon(filename);
            cout << Color::YELLOW << "⚠ SOURCE not implemented yet" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // SAVEPOINT
        // ============================================
        if (lower.find("savepoint") == 0) {
            string name = trim(query.substr(9));
            stripTrailingSemicolon(name);
            cout << Color::GREEN << "✓ Query OK, 0 rows affected" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // INSERT
        // ============================================
        if (lower.find("insert into") == 0) {
            string rest = query.substr(11);
            bool ignore = false;
            
            if (lower.find("ignore") != string::npos) {
                ignore = true;
                rest = rest.substr(ifindCI(rest, "ignore") + 6);
            }
            
            pos = ifindCI(rest, " values");
            if (pos != string::npos) {
                string name = trim(rest.substr(0, pos));
                string valuesPart = rest.substr(pos + 7);
                
                // Check for ON DUPLICATE KEY
                string duplicatePart = "";
                size_t dupPos = ifindCI(valuesPart, "on duplicate key");
                if (dupPos != string::npos) {
                    duplicatePart = valuesPart.substr(dupPos);
                    valuesPart = valuesPart.substr(0, dupPos);
                }
                
                vector<string> rowValues;
                size_t start = valuesPart.find('(');
                while (start != string::npos) {
                    size_t end = valuesPart.find(')', start);
                    if (end == string::npos) break;
                    string row = valuesPart.substr(start + 1, end - start - 1);
                    rowValues.push_back(row);
                    start = valuesPart.find('(', end + 1);
                }
                
                if (rowValues.size() > 1) {
                    executeInsertMultiple(name, rowValues);
                    return;
                } else if (rowValues.size() == 1) {
                    executeInsert(name, rowValues[0]);
                    return;
                }
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // REPLACE INTO
        // ============================================
        if (lower.find("replace into") == 0) {
            string rest = query.substr(12);
            pos = ifindCI(rest, " values");
            if (pos != string::npos) {
                string name = trim(rest.substr(0, pos));
                string valuesPart = rest.substr(pos + 7);
                vector<string> rowValues;
                size_t start = valuesPart.find('(');
                while (start != string::npos) {
                    size_t end = valuesPart.find(')', start);
                    if (end == string::npos) break;
                    string row = valuesPart.substr(start + 1, end - start - 1);
                    rowValues.push_back(row);
                    start = valuesPart.find('(', end + 1);
                }
                for (const auto& row : rowValues) {
                    executeInsert(name, row);
                }
                return;
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // SELECT
        // ============================================
        if (lower.find("select") == 0) {
            string rest = query.substr(6);
            
            // Check if it's a function call (already handled above)
            if (lower.find("user()") != string::npos || lower.find("current_user()") != string::npos ||
                lower.find("version()") != string::npos || lower.find("database()") != string::npos ||
                lower.find("schema()") != string::npos || lower.find("now()") != string::npos ||
                lower.find("curdate()") != string::npos || lower.find("curtime()") != string::npos ||
                lower.find("unix_timestamp()") != string::npos || lower.find("rand()") != string::npos) {
                return;
            }
            
            // Check for JOIN
            bool hasJoin = false;
            string joinTable = "";
            string joinCondition = "";
            string joinType = "";
            
            if (lower.find(" inner join ") != string::npos) {
                hasJoin = true;
                joinType = "INNER";
                size_t joinPos = ifindCI(rest, " inner join ");
                string joinPart = rest.substr(joinPos + 12);
                size_t onPos = ifindCI(joinPart, " on ");
                joinTable = trim(joinPart.substr(0, onPos));
                if (onPos != string::npos) {
                    joinCondition = trim(joinPart.substr(onPos + 4));
                    // Remove join condition from rest
                    rest = rest.substr(0, joinPos);
                }
            } else if (lower.find(" left join ") != string::npos) {
                hasJoin = true;
                joinType = "LEFT";
                size_t joinPos = ifindCI(rest, " left join ");
                string joinPart = rest.substr(joinPos + 11);
                size_t onPos = ifindCI(joinPart, " on ");
                joinTable = trim(joinPart.substr(0, onPos));
                if (onPos != string::npos) {
                    joinCondition = trim(joinPart.substr(onPos + 4));
                    rest = rest.substr(0, joinPos);
                }
            } else if (lower.find(" right join ") != string::npos) {
                hasJoin = true;
                joinType = "RIGHT";
                size_t joinPos = ifindCI(rest, " right join ");
                string joinPart = rest.substr(joinPos + 12);
                size_t onPos = ifindCI(joinPart, " on ");
                joinTable = trim(joinPart.substr(0, onPos));
                if (onPos != string::npos) {
                    joinCondition = trim(joinPart.substr(onPos + 4));
                    rest = rest.substr(0, joinPos);
                }
            } else if (lower.find(" cross join ") != string::npos) {
                hasJoin = true;
                joinType = "CROSS";
                size_t joinPos = ifindCI(rest, " cross join ");
                string joinPart = rest.substr(joinPos + 11);
                joinTable = trim(joinPart);
                rest = rest.substr(0, joinPos);
            }
            
            string columns, fromPart, wherePart, groupPart, havingPart, orderPart, limitPart, offsetPart, distinctPart;
            
            size_t fromPos = ifindCI(rest, "from");
            if (fromPos == string::npos) {
                cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
                return;
            }
            
            string selectPart = trim(rest.substr(0, fromPos));
            fromPart = rest.substr(fromPos + 4);
            
            if (ifindCI(selectPart, "distinct") == 0 || ifindCI(selectPart, "distinct") != string::npos) {
                size_t distinctPos = ifindCI(selectPart, "distinct");
                if (distinctPos != string::npos) {
                    distinctPart = "DISTINCT";
                    selectPart = selectPart.substr(distinctPos + 8);
                }
            }
            columns = trim(selectPart);
            
            string tableName;
            
            // Remove JOIN condition from fromPart
            if (hasJoin) {
                // Parse table name from fromPart
                tableName = trim(fromPart);
            } else {
                size_t wherePos = ifindCI(fromPart, "where");
                if (wherePos != string::npos) {
                    wherePart = fromPart.substr(wherePos + 5);
                    fromPart = fromPart.substr(0, wherePos);
                }
                
                size_t groupPos = ifindCI(fromPart, "group by");
                if (groupPos != string::npos) {
                    groupPart = fromPart.substr(groupPos + 8);
                    fromPart = fromPart.substr(0, groupPos);
                    
                    size_t havingPos = ifindCI(groupPart, "having");
                    if (havingPos != string::npos) {
                        havingPart = groupPart.substr(havingPos + 6);
                        groupPart = groupPart.substr(0, havingPos);
                    }
                }
                
                size_t orderPos = ifindCI(fromPart, "order by");
                if (orderPos != string::npos) {
                    orderPart = fromPart.substr(orderPos + 8);
                    fromPart = fromPart.substr(0, orderPos);
                }
                
                size_t limitPos = ifindCI(fromPart, "limit");
                if (limitPos != string::npos) {
                    string limitStr = fromPart.substr(limitPos + 5);
                    fromPart = fromPart.substr(0, limitPos);
                    
                    // Parse LIMIT and OFFSET
                    size_t offsetPos = ifindCI(limitStr, " offset ");
                    if (offsetPos != string::npos) {
                        limitPart = trim(limitStr.substr(0, offsetPos));
                        offsetPart = trim(limitStr.substr(offsetPos + 8));
                    } else if (limitStr.find(",") != string::npos) {
                        vector<string> parts = split(limitStr, ',');
                        if (parts.size() == 2) {
                            offsetPart = trim(parts[0]);
                            limitPart = trim(parts[1]);
                        }
                    } else {
                        limitPart = trim(limitStr);
                    }
                }
                
                tableName = trim(fromPart);
            }
            
            stripTrailingSemicolon(tableName);
            stripTrailingSemicolon(wherePart);
            stripTrailingSemicolon(orderPart);
            stripTrailingSemicolon(limitPart);
            stripTrailingSemicolon(offsetPart);
            
            vector<string> colList;
            if (columns == "*" || columns == "* ") {
                colList.push_back("*");
            } else {
                colList = split(columns, ',');
                for (auto& c : colList) c = trim(c);
            }
            
            executeSelect(tableName, colList, trim(wherePart), trim(groupPart), 
                         trim(havingPart), trim(orderPart), trim(limitPart), trim(offsetPart),
                         distinctPart);
            return;
        }
        
        // ============================================
        // UPDATE
        // ============================================
        if (lower.find("update") == 0) {
            string rest = query.substr(6);
            bool ignore = false;
            if (lower.find("ignore") != string::npos) {
                ignore = true;
                rest = rest.substr(ifindCI(rest, "ignore") + 6);
            }
            pos = ifindCI(rest, " set ");
            if (pos != string::npos) {
                string name = trim(rest.substr(0, pos));
                string setPart = rest.substr(pos + 5);
                string wherePart;
                size_t wherePos = ifindCI(setPart, " where ");
                if (wherePos != string::npos) {
                    wherePart = setPart.substr(wherePos + 6);
                    setPart = setPart.substr(0, wherePos);
                }
                stripTrailingSemicolon(name);
                stripTrailingSemicolon(wherePart);
                executeUpdate(name, setPart, trim(wherePart));
                return;
            }
            cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
            return;
        }
        
        // ============================================
        // DELETE
        // ============================================
        if (lower.find("delete from") == 0) {
            string rest = query.substr(11);
            bool ignore = false;
            if (lower.find("ignore") != string::npos) {
                ignore = true;
                rest = rest.substr(ifindCI(rest, "ignore") + 6);
            }
            string wherePart;
            size_t wherePos = ifindCI(rest, " where ");
            if (wherePos != string::npos) {
                wherePart = rest.substr(wherePos + 6);
                rest = rest.substr(0, wherePos);
            }
            string name = trim(rest);
            stripTrailingSemicolon(name);
            stripTrailingSemicolon(wherePart);
            executeDelete(name, trim(wherePart));
            return;
        }
        
        cout << Color::RED << "✗ ERROR 1064 (42000): You have an error in your SQL syntax" << Color::RESET << endl;
    }
};

// ============================================
// MAIN
// ============================================
int main() {
    if (system(CLEAR_SCREEN) != 0) { /* ignore: non-fatal if clear fails */ }

    cout << Color::BOLD << Color::TEAL << R"(
   ███╗   ███╗ █████╗ ██████╗ ██╗ █████╗ ██████╗ ██████╗ 
   ████╗ ████║██╔══██╗██╔══██╗██║██╔══██╗██╔══██╗██╔══██╗
   ██╔████╔██║███████║██████╔╝██║███████║██║  ██║██████╔╝
   ██║╚██╔╝██║██╔══██║██╔══██╗██║██╔══██║██║  ██║██╔══██╗
   ██║ ╚═╝ ██║██║  ██║██║  ██║██║██║  ██║██████╔╝██████╔╝
   ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚═════╝ ╚═════╝ 
)" << Color::RESET;
    cout << "   " << Color::PURPLE << Color::BOLD << "Engine" << Color::RESET
         << Color::GRAY << "  -  a self-contained SQL playground in C++" << Color::RESET << endl;
    cout << Color::GRAY_DARK << "   --------------------------------------------------------" << Color::RESET << endl;
    cout << "   " << Color::GREEN << "•" << Color::RESET << " Type " << Color::YELLOW << "HELP" << Color::RESET << " or " << Color::YELLOW << "\\h" << Color::RESET << " to see all available commands" << endl;
    cout << "   " << Color::GREEN << "•" << Color::RESET << " Type " << Color::YELLOW << "EXIT" << Color::RESET << " or " << Color::YELLOW << "QUIT" << Color::RESET << " to leave the session" << endl << endl;

    MariaDB db;

    if (!db.login()) {
        return 0;
    }

    string input;
    while (true) {
        cout << db.getPrompt();
        getline(cin, input);
        if (input.empty()) continue;
        db.executeSQL(input);
    }

    return 0;
}