#ifndef CSC4110_CONNECTION
#define CSC4110_CONNECTION
#include <string>
#include <pqxx/pqxx>
#include <sstream>
#include <fstream>
using namespace std;

//------------------------------------------------------------------------------
// Database Variables
//------------------------------------------------------------------------------
// Change depending on which system to compile to:
const string database_info_path = "../sql/database_info.txt";


static string database_address;
static string database_name;
static string database_table;

static string database_path;
void get_database_info_from_file(void);

//------------------------------------------------------------------------------
// Connection functions -- Defined in Connection.cc
//------------------------------------------------------------------------------

void initialize_connection(pqxx::connection &c);
void close_connection(pqxx::connection &c);
void open_connection(pqxx::connection &c);
pqxx::result search_database(pqxx::connection &c, const std::string& date, std::string time);


#endif
