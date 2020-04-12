#ifndef CSC4110_CONNECTION
#define CSC4110_CONNECTION
#include <string>
#include <pqxx/pqxx>
#include <sstream>
using namespace std;

//------------------------------------------------------------------------------
// Database Variables
//------------------------------------------------------------------------------
// Change depending on which system to compile to:
const string database_address = "192.168.1.93";
// Database user and password; I know it's unsafe;
const string database_name = "project:Pi_Project";
// Name of the table on the database.
const string database_table = "csc4110_project";

const string database_path = "postgresql://" + database_name + "@"
                             + database_address + "/" + database_table;

void get_database_info_from_file(void);

//------------------------------------------------------------------------------
// Connection functions -- Defined in Connection.cc
//------------------------------------------------------------------------------


void open_connection(pqxx::connection &c);
pqxx::result search_database(pqxx::connection &c, const std::string& date, std::string time);


#endif
