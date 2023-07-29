#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <abstractalpha.hpp>
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

    // Define the list of all exchanges and factors.
    std::unordered_set<std::string> exchanges_set = {"binance_futures", "binanceus", 
    "binance_spot", "okx"};
    std::unordered_set<std::string> factors_set = {"open", "high", "low", "close", 
    "volume", "vwap"};

    // Gets a chunk of data for backtesting.
    std::unordered_map<std::string, std::vector<std::vector<ld>>> 
    get_data_chunk(std::string start_date);

    // Find the next end time for the chunk given the start time.
    std::string chunk_end_date(std::string start_date);
    
    // Save the current backtester object.
    void save(const std::string& filename);

    // Load the current backtester object.
    void load(const std::string& filename);

    // Pure virtual function for backtesting.
    virtual std::pair<std::vector<ld>, std::vector<std::vector<ld>>> 
            backtest(AbstractAlpha alpha) = 0;

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
};

#endif // ABSTRACTBACKTESTER_H