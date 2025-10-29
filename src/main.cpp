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

    // getters to access private data
    double get_open() const {return open;}
    double get_close() const {return close;}
    string get_date() const {return date;}

    // get calculated data
    double get_change() const {return close - open;}
    double get_avg() const {return (open + close)/2;}
    bool went_up() const {
        if (close > open) return true;
        return false;
    }
    bool went_down() const {
        if (went_up()) return false;
        return true;
    }
    bool stayed_same() const {
        if (went_up() == false && went_down() == false) return true;
        return false;
    }

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



// return days above, below, and on the line
// return a weighted version by dollar amount difference



// // GOLDEN FORMULA
// double days_above(const string& fn, const string& start, const string& end) {
//     ifstream infile {fn};
//
//     string holder;
//     string x;
//     string y;
//     string d;
//
//     double aboves = 0;
//
//     double stepc = percent_change(get_avg(fn, start), get_avg(fn, end));
//     double stedp = days_passed(start, end);
//
//     while (getline(infile, holder)) {
//         stringstream ss {holder};
//         ss >> x >> y >> d;
//
//         if (days_passed(start, d) >= 0 && days_passed(d, end) >= 0) {
//
//             double stdpc = percent_change(get_avg(fn, start), get_avg(fn, d));
//             double stddp = days_passed(start, d);
//
//             if (stdpc > ( stepc * (stddp / stedp) )) ++aboves;
//         }
//     }
//
//     return aboves;
// }
//
// double days_below(const string& fn, const string& start, const string& end) {
//     ifstream infile {fn};
//
//     string holder;
//     string x;
//     string y;
//     string d;
//
//     double belows = 0;
//
//     double stepc = percent_change(get_avg(fn, start), get_avg(fn, end));
//     double stedp = days_passed(start, end);
//
//     while (getline(infile, holder)) {
//         stringstream ss {holder};
//         ss >> x >> y >> d;
//
//         if (days_passed(start, d) >= 0 && days_passed(d, end) >= 0) {
//
//             double stdpc = percent_change(get_avg(fn, start), get_avg(fn, d));
//             double stddp = days_passed(start, d);
//
//             if (stdpc < ( stepc * (stddp / stedp) )) ++belows;
//         }
//     }
//
//     return belows;
// }
//
// double days_on(const string& fn, const string& start, const string& end) {
//     ifstream infile {fn};
//
//     string holder;
//     string x;
//     string y;
//     string d;
//
//     double on_the_line = 0;
//
//     double stepc = percent_change(get_avg(fn, start), get_avg(fn, end));
//     double stedp = days_passed(start, end);
//
//     while (getline(infile, holder)) {
//         stringstream ss {holder};
//         ss >> x >> y >> d;
//
//         if (days_passed(start, d) >= 0 && days_passed(d, end) >= 0) {
//
//             double stdpc = percent_change(get_avg(fn, start), get_avg(fn, d));
//             double stddp = days_passed(start, d);
//
//             if (stdpc == ( stepc * (stddp / stedp) )) ++on_the_line;
//         }
//     }
//
//     return on_the_line;
// }
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


    // End timing
    auto end = chrono::high_resolution_clock::now();
    // Calculate duration
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "\nExecution time: " << duration.count()/1000 << " milliseconds" << "\n";



    //
    // string sdate = "03/21/2022";                                                // start date
    // string edate = "10/08/2024";                                                // end date
    //
    // double dp = days_passed(sdate, edate);                             // number of days passed
    // double first_close = get_close(FILE_NAME, sdate);                   // close price on start date
    // double last_close = get_close(FILE_NAME, edate);                    // close price on end date
    // double pc_decimal = percent_change(first_close, last_close);            // total % change (capital gain)
    // double apc_decimal = annual_p_change(pc_decimal, dp);                  // annualize return rate
    // double up_days = days_of_increases(FILE_NAME, sdate, edate);    // days open < close
    // double down_days = days_of_decreases(FILE_NAME, sdate, edate);  // days open > close
    //
    // double higher_days = days_above(FILE_NAME, sdate, edate);
    // double lower_days = days_below(FILE_NAME, sdate, edate);
    // double otl_days = days_on(FILE_NAME, sdate, edate);
    // double wda = weighted_days_above(FILE_NAME, sdate, edate);
    // double wdb = weighted_days_below(FILE_NAME, sdate, edate);
    //
    //
    // cout << dp << " days passed\n";
    // cout << first_close << " -> " << last_close << "\n";
    // cout << pc_decimal * 100 << "% change (in total)\n";
    // cout << apc_decimal * 100 << "% annualized return\n";
    // cout << up_days << " days of increases (close > open)\n";       // ups + downs will NOT equal total days
    // cout << down_days << " days of decreases (close < open)\n";     // up/down days only count trading days < total days
    //
    // cout << higher_days << " above the connecting line\n";
    // cout << lower_days << " below the connecting line\n";
    // cout << otl_days << " on the connecting line\n";
    // cout << wda << " <- weighted value of time spent above the connecting line\n";
    // cout << wdb << " <- weighted value of time spent below the connecting line\n";
    //
    //
    // cout << "\n\n\n";
    //
    // vector<string> sdates {"10/12/2015", "06/12/2017", "01/08/2021", "11/17/2022", "04/09/2024"};
    // vector<string> edates1 {"10/12/2016", "06/12/2018", "01/07/2022", "11/17/2023", "04/09/2025"};
    // vector<string> edates2 {"10/12/2017", "06/12/2019", "01/09/2023"};
    // edate = "10/09/2025";
    //
    // // 1-year time horizons
    // int x = 0;
    // int y = 0;
    // for (int i = 0; i != 5; ++i) {
    //     string sd = sdates[i];
    //     string ed = edates1[i];
    //     if (weighted_days_above(FILE_NAME, sd, ed) > weighted_days_below(FILE_NAME, sd, ed)) ++x;
    //     else ++y;
    // }
    // cout << "Number of 1-year periods (of 5) where wda exceeded wdb: " << x << "\n";
    // cout << "Number of 1-year periods (of 5) where wdb exceeded wda: " << y << "\n\n";
    //
    // // 2-year time horizons
    // int a = 0;
    // int b = 0;
    // for (int i = 0; i != 3; ++i) {
    //     string sd = sdates[i];
    //     string ed = edates2[i];
    //     if (weighted_days_above(FILE_NAME, sd, ed) > weighted_days_below(FILE_NAME, sd, ed)) ++a;
    //     else ++b;
    // }
    // cout << "Number of 2-year periods (of 3) where wda exceeded wdb: " << a << "\n";
    // cout << "Number of 2-year periods (of 3) where wdb exceeded wda: " << b << "\n\n";
    //
    // // then-to-now time horizons
    // int j = 0;
    // int k = 0;
    // for (int i = 0; i != 5; ++i) {
    //     string sd = sdates[i];
    //     if (weighted_days_above(FILE_NAME, sd, edate) > weighted_days_below(FILE_NAME, sd, edate)) ++j;
    //     else ++k;
    // }
    // cout << "Number of then-to-now periods (of 5) where wda exceeded wdb: " << j << "\n";
    // cout << "Number of then-to-now (of 5) where wdb exceeded wda: " << k << "\n\n";

    return 0;
}