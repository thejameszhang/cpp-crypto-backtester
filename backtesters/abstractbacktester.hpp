#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <eigen3/Eigen/Dense>
#include <abstractalpha.hpp>
#include "../utils.hpp""
typedef long double ld;

#ifndef ABSTRACTBACKTESTER_H
#define ABSTRACTBACKTESTER_H

class AbstractBacktester {

public:
    // The default constructor for the AbstractBacktester.
    AbstractBacktester(std::string _start_date, 
                       std::string _end_date, 
                       int _max_lookback,
                       std::string _exchange, 
                       std::vector<std::string> _factors);
    
    // Start date getter.
    std::string get_start_date();

    // End date getter.
    std::string get_end_date();

    // Max lookback getter.
    int get_max_lookback();

    // Data reload interval getter.
    int get_data_reload_interval();
    
    // Gets a chunk of data for backtesting.
    std::unordered_map<std::string, std::vector<std::vector<ld>>> 
    get_data_chunk(std::string start_date);



    // Save the current backtester object.
    void save(const std::string& filename);

    // Load the current backtester object.
    void load(const std::string& filename);

    // Pure virtual function for backtesting.
    virtual std::pair<std::vector<std::vector<ld>>, std::vector<std::vector<ld>>> 
            backtest(AbstractAlpha* alpha) = 0;

    // Virtual destructor (needed for polymorphic behavior).
    virtual ~AbstractBacktester() {}

private:
    std::string start_date;
    std::string end_date;
    int max_lookback;
    std::string exchange;
    std::vector<std::string> factors;
    int data_reload_interval = 5000;
    std::unordered_map<int, std::string> universe;
    
    // Define the sets of all exchanges and factors.
    std::unordered_set<std::string> exchanges_set = {"binance_futures", "binanceus", 
    "binance_spot", "okx"};
    std::unordered_set<std::string> factors_set = {"open", "high", "low", "close", 
    "volume", "vwap"};
};

#endif // ABSTRACTBACKTESTER_H