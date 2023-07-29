#include <abstractbacktester.hpp>
#include <../database/utils.hpp>

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
            throw std::logic_error("Factor not found.");
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
AbstractBacktester::get_data_chunk(std::string start_date) {
    /* 
    Gets the next chunk of data in the backtesting period. 
    :param start_date: the chunk starting date
    :return: a dictionary so for example
        - close -> 2d vector of close price data for all currencies on an exchange
        - volume -> 2d vector of volume data for all currency pairs on an exchange 
        ...
    */
    std::string end_date = AbstractBacktester::chunk_end_date(start_date);
    end_date = std::min(end_date, this->end_date);
    std::unordered_map<std::string, std::vector<std::vector<ld>>> res;
    for (auto factor : this->factors) {
        res[factor] = get_data(start_date, end_date, this->exchange, factor);
    }
    return res;
}

// Find the next end time for the chunk.
std::string AbstractBacktester::chunk_end_date(std::string start_date) {
    // Convert the timestamp string to a std::chrono::time_point object
    std::tm tmStruct = {};
    std::istringstream timestampStream(start_date);
    timestampStream >> std::get_time(&tmStruct, "%Y-%m-%d %H:%M:%S");
    std::chrono::system_clock::time_point timestamp;
    timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tmStruct));
    // Add the specified number of minutes to the time_point
    timestamp += std::chrono::minutes(this->data_reload_interval);
    // Convert the updated time_point back to a timestamp string
    std::time_t time_tTimestamp = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream resultStream;
    resultStream << std::put_time(std::localtime(&time_tTimestamp), "%Y-%m-%d %H:%M:%S");
    return resultStream.str();
}

// Saves the current backtester object.
void AbstractBacktester::save(const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (outFile) {
        outFile.write(reinterpret_cast<const char*>(this), sizeof(*this));
    }
}

// Loads the current backtester oject.
void AbstractBacktester::load(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (inFile) {
        inFile.read(reinterpret_cast<char*>(this), sizeof(*this));
    }
}