/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/06/2020 at 06:48 PM
 * 
 * Connection.cc
 * 
 * -----------------------------------------------------------------------------
 */

#include "./Connection.h"


//------------------------------------------------------------------------------
// initialize_connection(): Handles retrieving database info and
// then opens the postgresql connection.
//------------------------------------------------------------------------------
void initialize_connection(pqxx::connection &c){
    get_database_info_from_file();
    open_connection(c);
}


//------------------------------------------------------------------------------
// open_connections() : Handles opening a pqxx::connection to the database
// specified in database_path.
//------------------------------------------------------------------------------
void open_connection(pqxx::connection &c){
    if (c.is_open()){
        c.close();
    }

    if (database_path.empty()){
        std::string error{"Database path has not been initialized."};
        error += " Please run initialize_connection() first.";
        throw runtime_error(error);
    }
    c = pqxx::connection(database_path);
}

//------------------------------------------------------------------------------
// search_database() : returns a pqxx::result contains any rows that contain
// the specified date and time of a query. The returned pqxx::result may
// contain no rows at all.
//------------------------------------------------------------------------------
pqxx::result search_database(pqxx::connection &c, const std::string& date, std::string time){
    // Time format should be default (yyyy/mm/dd)
    // Some form of checking for time

    // If all goes well, do a query on the database.
    pqxx::work work(c);

    // For now, only return one query,
    ostringstream os;
    os << "SELECT * FROM log WHERE log_date = " << work.esc(date) << " AND log_time = " << work.esc(time);
    pqxx::result result = work.exec(os.str());

}

//
void get_database_info_from_file(void){
    // Read the data from
    ifstream file{database_info_path};
    if (!file){
        std::string error = "Cannot open " + database_info_path;
        error += " in order to retrive database credentials. Aborting.";
        throw runtime_error(error);
    }

    // Now check if the file is empty.
    if (file.peek() == std::ifstream::traits_type::eof()){
        throw runtime_error(database_info_path + "is emtpy.");
    }

    // Retrieve the credentials and assign them
    string db_address, db_name, db_table;
    getline(file, db_address);
    getline(file, db_name);
    getline(file, db_table);

    auto file_status = file.rdstate();

    // The file should only have the eof flag enabled after reading the file to the end.
    // If it doesn't meet those requirements, throw an error.
    if (file_status != ios_base::eofbit){
        string error = "Could not populate all variables from " + database_info_path + " .";
        error += " Please make sure that " + database_info_path + " is EXACTLY 3 lines.\n";
        throw runtime_error(error);
    }
    database_address = db_address;
    database_name = db_name;
    database_table = db_table;

    database_path = "postgresql://" + db_name + "@" + db_address +
	"/" + db_table;
    
    /*
    database_path = "postgresql://" + database_name + "@"
                    + database_address + "/" + database_table;
                    */
}
