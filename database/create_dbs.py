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
    Checks that the 4 databases exist within the crypto mysql database.
    """
    mycursor.execute("SHOW TABLES")
    for table in mycursor:
        print(table)

def print_table(table: str) -> None:
    """
    Prints out the contents in one of the sql tables
    """
    mycursor.execute("SHOW columns FROM binanceus")
    print([column[0] for column in mycursor.fetchall()])
    mycursor.execute(f"SELECT * FROM {table} LIMIT 5")
    result = mycursor.fetchall()
    for row in result:
        print(row)
        print("\n")

# mycursor.execute("DROP TABLE binanceus")
path = "/mnt/c/Users/James Zhang/Desktop/crypto_data/"
# files = [file for file in os.listdir(path) if "binanceus" in file]
# files_in_order = [f"binanceus_{i}.parquet" for i in range(100)]
# engine = create_engine(f"mysql+mysqldb://james:mf2g^SD837$@localhost:3306/crypto", echo = False)

# for file in files_in_order:
#     assert file in files
#     print(file)
#     df = pd.read_parquet(f"{path}{file}")
#     df.to_sql(name='binanceus', con=engine, if_exists='append', index=True)


# check_tables_existence()
# print_table("binanceus")

print(pd.read_parquet(f"{path}binanceus_0.parquet").iloc[0])
print(pd.read_parquet(f"{path}binanceus_99.parquet").iloc[-1])