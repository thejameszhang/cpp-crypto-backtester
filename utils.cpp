#include "utils.hpp"

std::vector<std::vector<ld>> execute_query(std::string query) {
    // Define the MySQL login credentials.
    std::string server = "localhost";
    std::string username = "james";
    std::string password = "mf2g^SD837$";
    std::string database = "crypto";
    std::vector<std::vector<ld>> data;

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
    // Define the MySQL login credentials.
    std::string server = "localhost";
    std::string username = "james";
    std::string password = "mf2g^SD837$";
    std::string database = "crypto";
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

std::vector<std::vector<ld>> get_data(std::string start_date,
                                      std::string end_date, 
                                      std::string factor, 
                                      std::string exchange) {
    std::string query = "SELECT * FROM `" + exchange + "-" + factor;
    query += "` WHERE `timestamp` BETWEEN '" + start_date + "' AND '" + end_date + "'";
    auto result = execute_query(query);
    return result;
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

std::vector<std::vector<ld>> pct_change(std::vector<std::vector<ld>> close_prices) {
    std::vector<std::vector<ld>> returns;
    int rows = int(close_prices.size());
    int cols = int(close_prices[0].size());
    // Calculate percentage change for each element
    for (int i = 1; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // Calculate percentage change from the previous element
            returns[i][j] = (close_prices[i][j] - close_prices[i-1][j]) / close_prices[i-1][j];
        }
    }
    return returns;
}

void element_wise_multiplication(std::vector<std::vector<ld>>& returns, 
std::vector<std::vector<ld>>& holdings) {
    int rows = int(returns.size());
    int cols = int(returns[0].size());

    if (rows != int(holdings.size()) || cols != int(holdings[0].size()))
        throw std::invalid_argument("Matrices are not the same dimension");

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            returns[i][j] = returns[i][j] * holdings[i][j];
        }
    }
}