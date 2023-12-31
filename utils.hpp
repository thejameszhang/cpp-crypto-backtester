#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <thread>
#include <mutex>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

typedef long double ld;

#ifndef UTILS_H
#define UTILS_H

// Helper function for all other functions to make calls to the MySQL database.
std::vector<std::vector<ld>> execute_query(std::string query);

// Gets the names of the universe when the backtester is created.
std::unordered_map<int, std::string> get_universe(std::string factor, 
                                                std::string exchange);

// Crafts the appropriate MySQL query given start, end, factor, exchange.
std::string create_query(std::string start_date, std::string end_date,
                        std::string factor, std::string exchange);

// Given start, end, factor, and exchange, get the data.
std::vector<std::vector<ld>> get_data(std::string start_date, 
                                      std::string end_date, 
                                      std::string factor, 
                                      std::string exchange);

// Given start, end, exchange, and list of factors, use threads
// to get the data concurrently.
std::unordered_map<std::string, std::vector<std::vector<ld>>>
    get_data(std::string start_date, std::string end_date, 
            std::string exchange,  std::vector<std::string> factors);

// Find the next end time for the chunk given the start time.
std::string time_shift(std::string current_date, int minutes);

// Slice the data in the chunked data so that the alpha only has the lookback
// it requests.
std::vector<std::vector<ld>>
slice(std::vector<std::vector<ld>> data, int begin, int end);

// Computes returns function for a single alpha backtester.
std::vector<std::vector<ld>> 
compute_returns(std::vector<std::vector<ld>>& close_prices, 
                            std::vector<std::vector<ld>>& holdings);

// Computes returns function for a multiple alpha backtester.
std::vector<std::vector<std::vector<ld>>>
compute_returns(std::vector<std::vector<ld>>& close_prices,
                std::vector<std::vector<std::vector<ld>>>& holdings);

#endif