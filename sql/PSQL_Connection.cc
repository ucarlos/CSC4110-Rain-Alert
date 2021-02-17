/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/06/2020 at 06:48 PM
 * 
 * Connection.cc
 * 
 * -----------------------------------------------------------------------------
 */


#include "./PSQL_Connection.h"


//------------------------------------------------------------------------------
// initialize_connection(): Handles retrieving database info and
// then opens the postgresql connection.
//------------------------------------------------------------------------------

/**
 * Retrieves retrieving database information and then opens a postgresql
 * connection.
 * @param c A pqxx::connection object that handles a connection to a database.
 */
void initialize_connection(pqxx::connection &c){
    //get_database_info_from_file();
    open_connection(c);
}





/**
 * Opens a postgresql connection to the database specified in database_path.
 * @param c A pqxx::connection object that handles a connection to a database.
 */
void open_connection(pqxx::connection &c){
	// Close an open connection.
    if (c.is_open()){
        c.close();
    }

    if (database_path.empty()){
        std::string error{"Database path has not been initialized."};
        error += " Please run initialize_connection() first.";
        throw std::runtime_error(error);
    }
    c = pqxx::connection(database_path);
}

//------------------------------------------------------------------------------
// close_connection(): Thin wrapper around connection.close().
// 
//------------------------------------------------------------------------------

/**
 * Thin wrapper around connection.close().
 * @param c A pqxx::connection object that handles a connection to a database.
 */
void close_connection(pqxx::connection &c){
    c.close();
    // Anything else?
}


/**
 * Retrieve any rows that satify the specified date and time of a query. This
 * information is stored in a pqxx::result object.
 * @param c A pqxx::connection object that handles a connection to a database.
 * @param date A string containing a date in yyyy/mm/dd format.
 * @returns A pqxx::result object containing any rows or none at all.
 */
pqxx::result search_database(pqxx::connection &c, const std::string &date) {
    // Time format should be default (yyyy/mm/dd)
    // Some form of checking for time

    // If all goes well, do a query on the database.
    pqxx::work work(c);

    // For now, only return one query,
    std::ostringstream os;


    // TODO: Make sure query executes if (time + 1) rounds up a minute, hour, or day
    os << "SELECT * FROM log WHERE log_date = " << work.quote(date);

    return work.exec(os.str());

}


/**
 * Read the credentials into the database. The credentials are contained the
 * path specified by database_info_path.
 * @deprecated Use get_database_info_from_xml instead as the project's settings are stored in a single XML file.
 */
void get_database_info_from_file(void){
    // Read the data from
    std::ifstream file{database_info_path};
    if (!file){
        std::string error = "Cannot open " + database_info_path;
        error += " in order to retrive database credentials. Aborting.";
        throw std::runtime_error(error);
    }

    // Now check if the file is empty.
    if (file.peek() == std::ifstream::traits_type::eof()){
        throw std::runtime_error(database_info_path + "is emtpy.");
    }

    // Retrieve the credentials and assign them
    std::string db_address, db_name, db_table;
    getline(file, db_address);
    getline(file, db_name);
    getline(file, db_table);

    auto file_status = file.rdstate();

    // The file should only have the eof flag enabled after reading the file to the end.
    // If it doesn't meet those requirements, throw an error.
    if (file_status != std::ios_base::eofbit){
        std::string error = "Could not populate all variables from " + database_info_path + " .";
        error += " Please make sure that " + database_info_path + " is EXACTLY 3 lines.\n";
        throw std::runtime_error(error);
    }
    database_address = db_address;
    database_name = db_name;
    database_table = db_table;

    database_path = "postgresql://" + db_name + "@" + db_address +
	"/" + db_table;
    

}

/**
 * Populate the database information using the project setting file's database
 * information.
 * @param db_info A Map object containing all database information stored in the project's setting file.
 *
 */
void get_database_info_from_xml(const std::map<std::string, std::string> &db_info) {
	database_address = db_info.at("psql_ip");
	database_name = db_info.at("psql_login");
	database_table = db_info.at("psql_database");

	database_path = "postgresql://" + database_name + "@" + database_address +
					"/" + database_table;

}


