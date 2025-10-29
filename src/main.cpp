#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <chrono>

using namespace std;

// file name incase you want to use a different file or add files later
string FILE_NAME = "../open_close_date_SPY.txt";


// class to store a trading day object (date strings are "mm/dd/yyyy")
class TradingDay {
private:
    // stored data
    double open;
    double close;
    string date;
public:
    // constructor
    TradingDay(double o, double c, string d) : open{o}, close{c}, date{d} {}
    TradingDay() {}

    // getters to access private data
    double get_open() const {return open;}
    double get_close() const {return close;}
    string get_date() const {return date;}

    // get calculated data
    double get_change() const {return close - open;}
    double get_avg() const {return (open + close)/2;}
    bool went_up() const { return close > open; }
    bool went_down() const { return open > close; }
    bool stayed_same() const { return open == close; }

    // cout appearance
    friend ostream& operator<<(ostream& os, const TradingDay& td) {
        return os << "On " << td.date << " the stock opened at " << td.open << " and closed at " << td.close;
    }
};


// function to read the file into a vector of TradingDays
vector<TradingDay> read_days(const string& fn) {
    vector<TradingDay> this_vec;
    ifstream infile {fn};
    string  holder, d;
    double x, y;

    while (getline(infile, holder)) {
        stringstream ss {holder};
        ss >> x >> y >> d;
        TradingDay this_day {x, y, d};
        this_vec.push_back(this_day);
    }
    return this_vec;
}


// measure the total number of days between two trading days (earlier day goes first)
double days_between(const TradingDay& td1, const TradingDay& td2) {
    // Get date strings from both objects
    string date1 = td1.get_date();
    string date2 = td2.get_date();

    // Parse first date (mm/dd/yyyy)
    int month1 = stoi(date1.substr(0, 2));
    int day1 = stoi(date1.substr(3, 2));
    int year1 = stoi(date1.substr(6, 4));

    // Parse second date (mm/dd/yyyy)
    int month2 = stoi(date2.substr(0, 2));
    int day2 = stoi(date2.substr(3, 2));
    int year2 = stoi(date2.substr(6, 4));

    // Create tm structs for both dates
    struct tm time1 = {0, 0, 0, day1, month1 - 1, year1 - 1900};
    struct tm time2 = {0, 0, 0, day2, month2 - 1, year2 - 1900};

    // Convert to time_t (seconds since epoch)
    time_t t1 = mktime(&time1);
    time_t t2 = mktime(&time2);

    // Calculate difference in days (signed)
    return (t2 - t1) / 86400;
}

double days_between(const string& date1, const string& date2) {
    // Parse first date (mm/dd/yyyy)
    int month1 = stoi(date1.substr(0, 2));
    int day1 = stoi(date1.substr(3, 2));
    int year1 = stoi(date1.substr(6, 4));

    // Parse second date (mm/dd/yyyy)
    int month2 = stoi(date2.substr(0, 2));
    int day2 = stoi(date2.substr(3, 2));
    int year2 = stoi(date2.substr(6, 4));

    // Create tm structs for both dates
    struct tm time1 = {0, 0, 0, day1, month1 - 1, year1 - 1900};
    struct tm time2 = {0, 0, 0, day2, month2 - 1, year2 - 1900};

    // Convert to time_t (seconds since epoch)
    time_t t1 = mktime(&time1);
    time_t t2 = mktime(&time2);

    // Calculate difference in days (signed)
    return (t2 - t1) / 86400;
}


// calculate the percent change between two days using close prices (return value is in decimal format)
double percent_change(const TradingDay& td1, const TradingDay& td2) {
    return td2.get_close() / td1.get_close() - 1;
}

// calculate annualized percentage change
double annualized_pc(const TradingDay& td1, const TradingDay& td2) {
    double days = days_between(td1, td2);
    return pow( td2.get_close()/td1.get_close(), 365/days) - 1;
}
// annualize a raw percentage change
double annualized_pc(const double& pc, const double& days) {
    return pow((1+pc), 365/days) - 1 ;
}


// count the number of days a stock increased in a given period of time
double count_ups(const vector<TradingDay>& vec, const string& start, const string& end) {
    double ups = 0;
    for (const auto& td : vec) {
        if (days_between(start, td.get_date()) >= 0) {
            if (days_between(td.get_date(), end) >= 0) {
                if (td.went_up()) ++ups;
            }
        }
    }
    return ups;
}

// count the number of days a stock decreased in a given period of time
double count_downs(const vector<TradingDay>& vec, const string& start, const string& end) {
    double downs = 0;
    for (const auto& td : vec) {
        if (days_between(start, td.get_date()) >= 0) {
            if (days_between(td.get_date(), end) >= 0) {
                if (td.went_down()) ++downs;
            }
        }
    }
    return downs;
}

// count the number of days a stock decreased in a given period of time
double count_sames(const vector<TradingDay>& vec, const string& start, const string& end) {
    double sames = 0;
    for (const auto& td : vec) {
        if (days_between(start, td.get_date()) >= 0) {
            if (days_between(td.get_date(), end) >= 0) {
                if (td.stayed_same()) ++sames;
            }
        }
    }
    return sames;
}


// GOLDEN FORMULAS

// count the days spent above the connecting price line
double days_above(const vector<TradingDay>& vec, const string& start, const string& end) {
    double aboves = 0;
    TradingDay start_day;
    TradingDay end_day;

    // find start and end days
    for (const auto& td : vec) {
        if (td.get_date() == start) start_day = td;
        else if (td.get_date() == end) end_day = td;
    }

    // total percentage change from start to end
    double total_pc = percent_change(start_day, end_day);

    // count days above
    for (const auto& td : vec) {
        if (days_between(start, td.get_date()) >= 0) {
            if (days_between(td.get_date(), end) >= 0) {

                double actual_pc = percent_change(start_day, td);
                double scaled_total_pc = total_pc * (days_between(start_day, td)/days_between(start_day, end_day));

                if (actual_pc > scaled_total_pc) ++aboves;
            }
        }
    }
    return aboves;
}

// count the days spent above the connecting price line
double days_below(const vector<TradingDay>& vec, const string& start, const string& end) {
    double belows = 0;
    TradingDay start_day;
    TradingDay end_day;

    // find start and end days
    for (const auto& td : vec) {
        if (td.get_date() == start) start_day = td;
        else if (td.get_date() == end) end_day = td;
    }

    // total percentage change from start to end
    double total_pc = percent_change(start_day, end_day);

    // count days above
    for (const auto& td : vec) {
        if (days_between(start, td.get_date()) >= 0) {
            if (days_between(td.get_date(), end) >= 0) {

                double actual_pc = percent_change(start_day, td);
                double scaled_total_pc = total_pc * (days_between(start_day, td)/days_between(start_day, end_day));

                if (actual_pc < scaled_total_pc) ++belows;
            }
        }
    }
    return belows;
}

// count the days spent above the connecting price line
double days_on(const vector<TradingDay>& vec, const string& start, const string& end) {
    double ons = 0;
    TradingDay start_day;
    TradingDay end_day;

    // find start and end days
    for (const auto& td : vec) {
        if (td.get_date() == start) start_day = td;
        else if (td.get_date() == end) end_day = td;
    }

    // total percentage change from start to end
    double total_pc = percent_change(start_day, end_day);

    // count days above
    for (const auto& td : vec) {
        if (days_between(start, td.get_date()) >= 0) {
            if (days_between(td.get_date(), end) >= 0) {

                double actual_pc = percent_change(start_day, td);
                double scaled_total_pc = total_pc * (days_between(start_day, td)/days_between(start_day, end_day));

                if (actual_pc == scaled_total_pc) ++ons;
            }
        }
    }
    return ons;
}


// return a weighted version by dollar amount difference




//
// // versions weighted by the differential off the line
// double weighted_days_above(const string& fn, const string& start, const string& end) {
//     ifstream infile {fn};
//
//     string holder;
//     string x;
//     string y;
//     string d;
//
//     double waboves = 0;
//
//     double stepc = percent_change(get_avg(fn, start), get_avg(fn, end));
//     double stedp = days_passed(start, end);
//
//     while (getline(infile, holder)) {
//         stringstream ss {holder};
//         ss >> x >> y >> d;
//
//         if (days_passed(start, d) >= 0 && days_passed(d, end) >= 0) {
//             double stdpc = percent_change(get_avg(fn, start), get_avg(fn, d));
//             double stddp = days_passed(start, d);
//
//             if (stdpc > ( stepc * (stddp / stedp) )) {
//
//                 double true_price = get_avg(fn, d);
//                 double line_price = get_avg(fn, start) * (1 + stepc * (stddp / stedp));
//
//                 waboves += (true_price - line_price);
//             }
//         }
//     }
//
//     return waboves;
// }

int main() {

    // Start timing
    auto start = chrono::high_resolution_clock::now();


    // read the file in and make a vector of TradingDay objects
    vector<TradingDay> my_days;
    my_days = read_days(FILE_NAME);

    // test code
    cout << my_days[0] << "\n";

    cout << my_days[0].get_open() << "\n";
    cout << my_days[0].get_close() << "\n";
    cout << my_days[0].get_date() << "\n";
    cout << my_days[0].get_avg() << "\n";
    cout << my_days[0].went_up() << "\n";
    cout << my_days[0].went_down() << "\n";
    cout << my_days[0].stayed_same() << "\n";
    cout << days_between(my_days[4], my_days[0]) << "\n";
    cout << days_between("11/05/2023", "04/20/2025") << "\n";
    cout << count_ups(my_days, "10/01/2025", "10/09/2025") << "\n";

    cout << days_above(my_days, "10/20/2017", "10/21/2019") << "\n";
    cout << days_below(my_days, "10/20/2017", "10/21/2019") << "\n";
    cout << days_on(my_days, "10/20/2017", "10/21/2019") << "\n";


    // End timing
    auto end = chrono::high_resolution_clock::now();
    // Calculate duration
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "\nExecution time: " << duration.count()/1000 << " milliseconds" << "\n";


    return 0;
}