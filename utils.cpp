#include "utils.hpp"

// Define the MySQL login credentials.
const std::string server = "localhost";
const std::string username = "james";
const std::string password = "mf2g^SD837$";
const std::string database = "crypto";

// Define a mutex to synchronize access to data.
std::mutex mtx;

std::vector<std::vector<ld>> execute_query(std::string query) {
    std::vector<std::vector<ld>> data;

    std::lock_guard<std::mutex> lock(mtx);

    try {
        // Create a MySQL driver instance.
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        // Connect to the MySQL server.
        sql::Connection *con = driver->connect(server, username, password);
        // Connect to the specified database
        con->setSchema(database);
        // Prepare the statement and execute it.
        sql::PreparedStatement *prep_stmt = con->prepareStatement(query);
        sql::ResultSet *res = prep_stmt->executeQuery();
        // Get the result set metadata
        sql::ResultSetMetaData *meta = res->getMetaData();
        // Get the number of columns.
        int numColumns = meta->getColumnCount();
        // Get all of the values in the rows.
        while (res->next()) {
            data.push_back(std::vector<ld>(numColumns - 1));
            for (int i = 2; i <= numColumns; i++) {
                data[data.size() - 1][i - 2] = res->getDouble(i);
            }
        }
        delete driver;
        delete con;
        delete prep_stmt;
        delete res;
        return data;

    } catch (sql::SQLException &e) {
        std::cerr << "MySQL Error: " << e.what() << std::endl;
        return {{0.0}};
    }
}

std::unordered_map<int, std::string> get_universe(std::string factor, 
                                                std::string exchange) {
    std::unordered_map<int, std::string> universe;

    try {
        // Create a MySQL driver instance.
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        // Connect to the MySQL server.
        sql::Connection *con = driver->connect(server, username, password);
        // Connect to the specified database
        con->setSchema(database);
        // Prepare the statement and execute it.
        std::string query = "SHOW COLUMNS FROM " + exchange + "-" + factor;
        sql::PreparedStatement *prep_stmt = con->prepareStatement(query);
        sql::ResultSet *res = prep_stmt->executeQuery();
        // Get the result set metadata
        sql::ResultSetMetaData *meta = res->getMetaData();
        // Get the column names.
        int numColumns = meta->getColumnCount();
        for (int i = 1; i <= numColumns; i++) {
            universe[i] = meta->getColumnName(i);
        }
        delete res;
        delete prep_stmt;
        delete con;
        return universe;

    } catch (sql::SQLException &e) {
        std::cerr << "MySQL Error: " << e.what() << std::endl;
        return {};
    }
}

std::string create_query(std::string start_date, std::string end_date, 
                         std::string factor, std::string exchange) {
    /* 
    Creates the appropriate MySQL query given some information.
    :param start_date: start of backtesting period - max lookback
    :param end_date: end of backtesting period
    :param factor: open, high, low, close, volume, or vwap
    :param exchange: binance_future, binance_spot, binanceus, or okx
    :return: a MySQL query
    */
    std::string query = "SELECT * FROM `" + exchange + "-" + factor;
    query += "` WHERE `timestamp` BETWEEN '" + start_date + "' AND '" + end_date + "'";
    return query;
}

std::vector<std::vector<ld>> get_data(std::string start_date,
                                      std::string end_date, 
                                      std::string factor, 
                                      std::string exchange) {
    /* 
    Helper function for the backtester to get a chunk of data when only one
    factor is required.
    :param start_date: start of backtesting period - max lookback
    :param end_date: end of backtesting period
    :param factor: open, high, low, close, volume, or vwap
    :param exchange: binance_future, binance_spot, binanceus, or okx
    :return: 2d vector data
    */                                    
    std::string query = create_query(start_date, end_date, factor, exchange);
    auto res = execute_query(query);
    return res;
}

std::unordered_map<std::string, std::vector<std::vector<ld>>>
get_data(std::string start_date, std::string end_date, std::string exchange, 
std::vector<std::string> factors) {
    /* 
    USes threads to concurrently execute multiple queries required for a
    chunk of data. 
    :param start_date: start of backtesting period - max lookback
    :param end_date: end of backtesting period
    :param factors: any powerset of [open, high, low, close, volume, vwap]
    :param exchange: binance_future, binance_spot, binanceus, or okx
    :return: data chunk
    */
    // Make an unordered map keyed by the factor valued by the query.
    std::unordered_map<std::string, std::string> query_map;
    for (const auto& factor : factors) {
        query_map[factor] = create_query(start_date, end_date, factor, exchange);
    }
    // Make a threads vector that contains the threads.
    std::vector<std::thread> threads;
    // Now create the map representing the data chunk.
    std::unordered_map<std::string, std::vector<std::vector<ld>>> res;
    // Start the threads to fetch data from the database
    for (const auto& pair : query_map) {
        // Lambda function using reference capture
        threads.push_back(std::thread([&res, &pair]() {
            // Factor -> 2d vector
            res[pair.first] = execute_query(pair.second);
        }));
    }
    // Join all the threads to wait for them to finish
    for (std::thread& thread : threads) {
        thread.join();
    }
    return res;
}

std::string time_shift(std::string current_date, int minutes) {
    /* 
    Helper function to find the end date of the chunk given the start date.
    :param start_date: chunk starting date
    :return: chunk ending date
    */
    // Convert the timestamp string to a std::chrono::time_point object
    std::tm tmStruct = {};
    std::istringstream timestampStream(current_date);
    timestampStream >> std::get_time(&tmStruct, "%Y-%m-%d %H:%M:%S");
    std::chrono::system_clock::time_point timestamp;
    timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tmStruct));
    // Add the specified number of minutes to the time_point
    timestamp += std::chrono::minutes(minutes);
    // Convert the updated time_point back to a timestamp string
    std::time_t time_tTimestamp = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream resultStream;
    resultStream << std::put_time(std::localtime(&time_tTimestamp), "%Y-%m-%d %H:%M:%S");
    return resultStream.str();
}

std::vector<std::vector<ld>>
slice(std::vector<std::vector<ld>> data, int start, int end) {
    /* 
    Slices the chunked data to get only the period that the alpha requests.
    :param start: start of the alpha's lookback period
    :param end: one day after the current trading day
    :return: alpha lookback period and current trading day
    */
   if (end > data.size()) end = data.size();
   if (start >= end) return {{}};
   return std::vector<std::vector<ld>>(data.begin() + start, data.begin() + end);
}

std::vector<std::vector<ld>> 
compute_returns(const std::vector<std::vector<ld>>& close_prices, 
                const std::vector<std::vector<ld>>& holdings) {
    /* 
    Suppose M = number of minutes, and N = number of currencies. 
    Computes the trader's returns in one pass. 
    :param close_prices: close prices of the currencies - MxN
    :param holdings: the trader's portfolio's holdings - MxN
    :return: the trader's returns - MxN
    */

    // Get number of mintues and the number of assets.
    int rows = int(close_prices.size());
    int cols = int(close_prices[0].size());

    // Base check.
    if (rows != int(holdings.size()) || cols != int(holdings[0].size()))
        throw std::invalid_argument("Matrices are not the same dimension");

    std::vector<std::vector<ld>> returns(rows, std::vector<ld>(cols, 0.0));

    for (int r = rows - 1; r > -1; r--) {
        for (int c = cols - 1; c > -1; c--) {
            // Get the currency percent change (returns).
            returns[r][c] = (close_prices[r][c] - close_prices[r-1][c]) / close_prices[r-1][c];
            // Element wise multiplication by the holdings.
            returns[r][c] *= holdings[r][c];
        }
    }
    return returns;
}

std::vector<std::vector<std::vector<ld>>> 
compute_returns(const std::vector<std::vector<ld>>& close_prices, 
                const std::vector<std::vector<std::vector<ld>>>& holdings) {
    /* 
    Computes the returns all alphas in O(M * N * k) time, where k is the
    number of alphas.
    :param close_prices: close prices - MxN
    :param holdings: list of alpha holdings - MxNxk
    :returns: returns of each alpha - MxNxk
    */

    // Get number of mintues and the number of assets.
    int rows = int(close_prices.size());
    int cols = int(close_prices[0].size());
    int num_alphas = int(holdings.size());
    int pct_change = 0;

    // Base check.
    if (rows != int(holdings[0].size()) || cols != int(holdings[0][0].size()))
        throw std::invalid_argument("Matrices are not the same dimension");
    
    std::vector<std::vector<ld>> temp(rows, std::vector<ld>(cols, 0.0));
    std::vector<std::vector<std::vector<ld>>> returns(num_alphas, temp);

    // Iterate through the close prices, as all alphas returns need it.
    for (int r = rows - 1; r > -1; r--) {
        for (int c = cols - 1; c > -1; c--) {
            // Iterate through the alphas.
            // Get the currency percent change (returns).
            pct_change = (close_prices[r][c] - close_prices[r-1][c]) / close_prices[r-1][c];
            for (int k = 0; k < num_alphas; k++) {
                // Element wise multiplication by the holdings.
                returns[k][r][c] = pct_change * holdings[k][r][c];
            }
        }
    }
    return returns;
}