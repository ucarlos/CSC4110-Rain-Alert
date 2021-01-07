#ifndef CSC4110_CONNECTION
#define CSC4110_CONNECTION

// Default Project Include
#include "../Project_Base.h"
#include <pqxx/pqxx>

//using namespace std;

//------------------------------------------------------------------------------
// Database Variables
//------------------------------------------------------------------------------
// Change depending on which system to compile to:
const std::string database_info_path = "../settings/database_info.txt";


static std::string database_address;
static std::string database_name;
static std::string database_table;

static std::string database_path;
void get_database_info_from_file(void);

//------------------------------------------------------------------------------
// Connection functions -- Defined in Connection.cc
//------------------------------------------------------------------------------

void initialize_connection(pqxx::connection &c);
void close_connection(pqxx::connection &c);
void open_connection(pqxx::connection &c);
pqxx::result search_database(pqxx::connection &c, const std::string &date);

//------------------------------------------------------------------------------
// Project_Settings.xml functions
//------------------------------------------------------------------------------
void get_database_info_from_xml(const std::map<std::string, std::string> &db_info);

#endif
