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