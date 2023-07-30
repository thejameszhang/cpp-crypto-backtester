#include <abstractbacktester.hpp>

/* 
The AbstractBacktester class serves as a foundation for other variations of 
backtesting classes, such as server-side vs. client-side. This class contains 
common helper functions, such as getting the next chunk of data that will be 
backtested and both saving and loading the current state of the backtester.
*/

AbstractBacktester::AbstractBacktester(std::string _start_date, 
                                       std::string _end_date, 
                                       int _max_lookback, 
                                       std::string _exchange, 
                                       std::vector<std::string> _factors) {
    /* 
    Researchers will be using this as their default constructor for 
    maximum customization.
    :param start_date: starting date of backtesting period
    :param end_date: ending date of backtesting period
    :param max_lookback: maximum lookback of an alpha instance
    :param exchange: binance_futures, binanece_spot, binanceus, or okx
    :param factors: any of the power sets of ["open", "high", "low", "close", 
                                                "volume", "vwap"]
    */

    // Checking for valid parameters.
    if (_start_date > _end_date || _max_lookback < 0) 
        throw std::logic_error("Backtester parameters are invalid");
    
    // Check if exchanges is in the list of exchanges.
    if (exchanges_set.find(_exchange) == exchanges_set.end())
        throw std::logic_error("Exchange not found.");
    
    // Check if all factors are in the list of factors. 
    for (auto factor : _factors) {
        if (factors_set.find(factor) == factors_set.end()) {
            throw std::logic_error("Factor " + factor + " not found.");
        }
    }
    // If close price is not found in the list of factors, then add it.
    if (std::find(_factors.begin(), _factors.end(), "close") == _factors.end()) {
        _factors.push_back("close");
    }
    
    // Initialize the instance variables.
    this->start_date = _start_date;
    this->end_date = _end_date;
    this->max_lookback = _max_lookback;
    this->exchange = _exchange;
    this->factors = _factors;
    this->data_reload_interval = 5000;
    // All functions that must interact with the MySQL database is in utils.cpp.
    this->universe = get_universe("close", exchange);
}

std::unordered_map<std::string, std::vector<std::vector<ld>>>
AbstractBacktester::get_data_chunk(const std::string& start_date) {
    /* 
    Gets the next chunk of data in the backtesting period. 
    :param start_date: the chunk starting date
    :return: a dictionary so for example
        - close -> 2d vector of close price data for all currencies on an exchange
        - volume -> 2d vector of volume data for all currency pairs on an exchange 
        ...
    */
    // Get the end date of the chunk, given the start date.
    std::string end_date = time_shift(start_date, this->data_reload_interval);
    // If the end date is later than the end of the backtesting period.
    end_date = std::min(end_date, this->end_date);
    // Query database for the data for each factor.
    const std::string real_start = time_shift(start_date, -this->max_lookback);
    std::unordered_map<std::string, std::vector<std::vector<ld>>> res;
    // Use threads depending on the length of factors.
    if (this->factors.size() == 1) {
        // No need to use threads if only one factor.
        res[this->factors[0]] = get_data(real_start, end_date, this->factors[0], this->exchange); 
    } else {
        // Uses threads.
        res = get_data(real_start, end_date, this->exchange, this->factors);
    }
    return res;
}


std::string AbstractBacktester::get_start_date() {
    return this->start_date;
}

std::string AbstractBacktester::get_end_date() {
    return this->end_date;
}

int AbstractBacktester::get_max_lookback() {
    return this->max_lookback;
}

int AbstractBacktester::get_data_reload_interval() {
    return this->data_reload_interval;
}

void AbstractBacktester::save(const std::string& filename) {
    /* 
    The researcher can saves the current state of the backtester.
    :param filename: path that the backtester will be saved to
    */
    std::ofstream outFile(filename, std::ios::binary);
    if (outFile) {
        outFile.write(reinterpret_cast<const char*>(this), sizeof(*this));
    }
}

void AbstractBacktester::load(const std::string& filename) {
    /* 
    The researcher can load the backtester from a previous session, significantly
    saving time from having to redefine it.
    :param filename: current path of the backtester
    */
    std::ifstream inFile(filename, std::ios::binary);
    if (inFile) {
        inFile.read(reinterpret_cast<char*>(this), sizeof(*this));
    }
}