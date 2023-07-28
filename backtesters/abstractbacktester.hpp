#include <iostream>
#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <abstractalpha.hpp>
typedef long double ld;

#ifndef ABSTRACTBACKTESTER_H
#define ABSTRACTBACKTESTER_H

// Abstract class definition
class AbstractBacktester {
public:
    
    AbstractBacktester();

    // Default constructor for the AbstractBacktester
    AbstractBacktester(boost::gregorian::date& _start_date, 
                       boost::gregorian::date& _end_date, 
                        int _max_lookback);

    // Save the current backtester object.
    void save(const std::string& filename);

    // Load the current backtester object.
    void load(const std::string& filename);

    // Pure virtual function for backtesting
    virtual std::pair<std::vector<ld>, std::vector<std::vector<ld>>> 
            backtest(AbstractAlpha alpha) = 0;

    // Virtual destructor (needed for polymorphic behavior)
    virtual ~AbstractBacktester() {}

private:
    boost::gregorian::date start_date;
    boost::gregorian::date end_date;
    int max_lookback;
    /*
    Further along the line, I can implement more features in both the 
    alphas and the backtesters for additional trading customizability.
    std::string exchange(s);
    std::vector<std::string>> factors;
    int universe_size;
    */
};

#endif // ABSTRACTBACKTESTER_H