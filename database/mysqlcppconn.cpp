#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

int main() {
    // Replace these with your MySQL server credentials
    std::string server = "localhost";
    std::string username = "james";
    std::string password = "mf2g^SD837$";
    std::string database = "crypto";

    try {
        sql::mysql::MySQL_Driver *driver;
        sql::Connection *con;

        // Create a MySQL driver instance
        driver = sql::mysql::get_mysql_driver_instance();

        // Connect to the MySQL server
        con = driver->connect(server, username, password);

        // Connect to the specified database
        con->setSchema(database);

        // Prepare and execute a query to fetch the first 5 rows from the "table"
        sql::PreparedStatement *prep_stmt;
        sql::ResultSet *res;

        std::string query = "SELECT * FROM binanceus LIMIT 5;";
        prep_stmt = con->prepareStatement(query);
        res = prep_stmt->executeQuery();

        // Get the result set metadata
        sql::ResultSetMetaData *meta = res->getMetaData();

        // Print out the column names
        int numColumns = meta->getColumnCount();
        for (int i = 1; i <= numColumns; i++) {
            std::cout << meta->getColumnName(i) << "\t";
        }
        std::cout << std::endl;

        // Print out the fetched rows
        while (res->next()) {
            for (int i = 1; i <= numColumns; i++) {
                if (i == 1) {
                    std::cout << res->getString(i) << std::endl;                
                } 
                //else {
                //     // std::cout << typeid(std::stod(res->getString(i))).name() << std::endl;
                //     std::cout << typeid(res->getDouble(i)).name() << std::endl;
                //     std::cout << res->getDouble(i) << "\t";
                // }
            }
            std::cout << std::endl;
        }

        delete res;
        delete prep_stmt;
        delete con;

    } catch (sql::SQLException &e) {
        std::cerr << "MySQL Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}