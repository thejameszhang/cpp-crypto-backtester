#include <abstractbacktester.hpp>

AbstractBacktester::AbstractBacktester() {
    // If no parameters are provided, then initalize fields to the earliest time,
    // latest time, and 100 day lookback.
    this->start_date = boost::posix_time::time_from_string("2021-01-01 00:00:00");
    this->end_date = boost::posix_time::time_from_string("2023-01-31 23:59:00");
    this->max_lookback = 100;
}

AbstractBacktester::AbstractBacktester(boost::posix_time::ptime& _start_date, 
                    boost::posix_time::ptime& _end_date, int _max_lookback) {

    // Checking for valid parameters.
    if (_start_date > _end_date || _max_lookback < 0) 
        throw std::logic_error("Backtester parameters are invalid");
    
    // Initialize the instance variables.
    this->start_date = _start_date;
    this->end_date = _end_date;
    this->max_lookback = _max_lookback;
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