#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>

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



// days passed function
// percent change functions
// annualized % change (2 versions)
// count increases, decreases, and sames
// return days above, below, and on the line
// return a weighted version by dollar amount difference







// return the number of days passed between two string dates entered as mm/dd/yyyy
// using the 30/365 estimation method for convenience
double days_passed(const string& start, const string& end) {

    int smonth = stoi(start.substr(0,2));
    int sday = stoi(start.substr(3, 2));
    int syear = stoi(start.substr(6, 4));

    int emonth = stoi(end.substr(0,2));
    int eday = stoi(end.substr(3, 2));
    int eyear = stoi(end.substr(6, 4));

    double days = (eday - sday);
    days += ( 30 * (emonth - smonth) );
    days += ( 365 * (eyear - syear) );

    return days;
}


// calculate %change
double percent_change(const double& a, const double& b) {
    return (b / a - 1);
}

// annual %change
double annual_p_change(const double& a, const double& b, const double& days) {
    return ( pow((b/a), 365/days) - 1 );
}

double annual_p_change(const double& pc, const double& days) {
    return ( pow((1+pc), 365/days) - 1 );
}

// number of days the stock increased (close > open)
double days_of_increases(const string& fn, const string& start, const string& end) {
    ifstream infile {fn};

    string holder;
    string x;
    string y;
    string d;

    double ups = 0;

    while (getline(infile, holder)) {
        stringstream ss {holder};
        ss >> x >> y >> d;

        if (days_passed(start, d) >= 0 && days_passed(d, end) >= 0) {
            if (stod(x) < stod(y)) ++ups;
        }
    }
    return ups;
}

// number of days the stock increased (close > open)
double days_of_decreases(const string& fn, const string& start, const string& end) {
    ifstream infile {fn};

    string holder;
    string x;
    string y;
    string d;

    double downs = 0;

    while (getline(infile, holder)) {
        stringstream ss {holder};
        ss >> x >> y >> d;

        if (days_passed(start, d) >= 0 && days_passed(d, end) >= 0) {
            if (stod(x) > stod(y)) ++downs;
        }
    }
    return downs;
}



// GOLDEN FORMULA
double days_above(const string& fn, const string& start, const string& end) {
    ifstream infile {fn};

    string holder;
    string x;
    string y;
    string d;

    double aboves = 0;

    double stepc = percent_change(get_avg(fn, start), get_avg(fn, end));
    double stedp = days_passed(start, end);

    while (getline(infile, holder)) {
        stringstream ss {holder};
        ss >> x >> y >> d;

        if (days_passed(start, d) >= 0 && days_passed(d, end) >= 0) {

            double stdpc = percent_change(get_avg(fn, start), get_avg(fn, d));
            double stddp = days_passed(start, d);

            if (stdpc > ( stepc * (stddp / stedp) )) ++aboves;
        }
    }

    return aboves;
}

double days_below(const string& fn, const string& start, const string& end) {
    ifstream infile {fn};

    string holder;
    string x;
    string y;
    string d;

    double belows = 0;

    double stepc = percent_change(get_avg(fn, start), get_avg(fn, end));
    double stedp = days_passed(start, end);

    while (getline(infile, holder)) {
        stringstream ss {holder};
        ss >> x >> y >> d;

        if (days_passed(start, d) >= 0 && days_passed(d, end) >= 0) {

            double stdpc = percent_change(get_avg(fn, start), get_avg(fn, d));
            double stddp = days_passed(start, d);

            if (stdpc < ( stepc * (stddp / stedp) )) ++belows;
        }
    }

    return belows;
}

double days_on(const string& fn, const string& start, const string& end) {
    ifstream infile {fn};

    string holder;
    string x;
    string y;
    string d;

    double on_the_line = 0;

    double stepc = percent_change(get_avg(fn, start), get_avg(fn, end));
    double stedp = days_passed(start, end);

    while (getline(infile, holder)) {
        stringstream ss {holder};
        ss >> x >> y >> d;

        if (days_passed(start, d) >= 0 && days_passed(d, end) >= 0) {

            double stdpc = percent_change(get_avg(fn, start), get_avg(fn, d));
            double stddp = days_passed(start, d);

            if (stdpc == ( stepc * (stddp / stedp) )) ++on_the_line;
        }
    }

    return on_the_line;
}

// versions weighted by the differential off the line
double weighted_days_above(const string& fn, const string& start, const string& end) {
    ifstream infile {fn};

    string holder;
    string x;
    string y;
    string d;

    double waboves = 0;

    double stepc = percent_change(get_avg(fn, start), get_avg(fn, end));
    double stedp = days_passed(start, end);

    while (getline(infile, holder)) {
        stringstream ss {holder};
        ss >> x >> y >> d;

        if (days_passed(start, d) >= 0 && days_passed(d, end) >= 0) {
            double stdpc = percent_change(get_avg(fn, start), get_avg(fn, d));
            double stddp = days_passed(start, d);

            if (stdpc > ( stepc * (stddp / stedp) )) {

                double true_price = get_avg(fn, d);
                double line_price = get_avg(fn, start) * (1 + stepc * (stddp / stedp));

                waboves += (true_price - line_price);
            }
        }
    }

    return waboves;
}

int main() {

    // read the file in and make a vector of TradingDay objects
    vector<TradingDay> my_days;
    my_days = read_days(FILE_NAME);

    // test code
    cout << my_days[0].get_open() << "\n";
    cout << my_days[0].get_close() << "\n";
    cout << my_days[0].get_date() << "\n";
    cout << my_days[0].get_avg() << "\n";
    cout << my_days[0].went_up() << "\n";
    cout << my_days[0].went_down() << "\n";
    cout << my_days[0].stayed_same() << "\n";




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