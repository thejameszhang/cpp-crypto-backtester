import pandas as pd
import mysql.connector
from sqlalchemy import create_engine
import pymysql
pymysql.install_as_MySQLdb()
import MySQLdb
import os

db = mysql.connector.connect(
    host="localhost", 
    user="james", 
    passwd = "mf2g^SD837$",
    database="crypto"
)

mycursor = db.cursor()

def check_tables_existence() -> None:
    """
    Checks that the 24 tables exist within the crypto mysql database.
    """
    mycursor.execute("SHOW TABLES")
    for table in mycursor:
        print(table)

def print_table(table: str) -> None:
    """
    Prints out the contents in one of the sql tables
    """
    mycursor.execute(f"SHOW columns FROM `{table}`")
    print([column[0] for column in mycursor.fetchall()])
    mycursor.execute(f"SELECT * FROM `{table}` LIMIT 5")
    result = mycursor.fetchall()
    for row in result:
        print(row)
        print("\n")

def convert_timestamps_to_str() -> None:
    # Get the list of tables in the database
    mycursor.execute("SHOW TABLES")
    tables = [table[0] for table in mycursor.fetchall()]

    for table in tables:
        # Get the list of columns in the table
        mycursor.execute(f"SHOW COLUMNS FROM `{table}`")
        columns = [column[0] for column in mycursor.fetchall()]

        # Check if any column is of type 'datetime' and store the timestamp column name
        timestamp_column = None
        for column in columns:
            mycursor.execute(f"SHOW COLUMNS FROM `{table}` WHERE Field = %s", (column,))
            column_info = mycursor.fetchone()
            data_type = column_info[1]
            if data_type == 'datetime':
                timestamp_column = column
                print("found")
                break

        if timestamp_column:
            # Execute the UPDATE query to convert the timestamp column to string
            update_query = f"UPDATE `{table}` SET {timestamp_column} = DATE_FORMAT({timestamp_column}, '%Y-%m-%d %H:%i:%s')"
            mycursor.execute(update_query)

    print("Timestamp columns converted to strings successfully!")

path = "/mnt/c/Users/James Zhang/Desktop/crypto_data/"
factors = ["open", "high", "low", "close", "volume", "vwap"]
exchanges = ["binance_futures", "binance_spot", "binanceus", "okx"]
engine = create_engine(f"mysql+mysqldb://james:mf2g^SD837$@localhost:3306/crypto", echo = False)

for factor in factors:
    for exchange in exchanges:
        files = [file for file in os.listdir(path) if factor in file and exchange in file]
        files_in_order = [f"{exchange}-{factor}-{i}.parquet" for i in range(200)]
        for file in files_in_order:
            assert file in files
            print(file)
            df = pd.read_parquet(f"{path}{file}")
            df.to_sql(name=f"{exchange}-{factor}", con=engine, if_exists='append', index=True)

