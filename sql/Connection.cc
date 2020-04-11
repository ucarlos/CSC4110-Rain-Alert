/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/06/2020 at 06:48 PM
 * 
 * Connection.cc
 * 
 * -----------------------------------------------------------------------------
 */

#include "../Project.h"


//------------------------------------------------------------------------------
// open_connections() : Handles opening a pqxx::connection to the database
// specified in database_path.
//------------------------------------------------------------------------------
void open_connection(pqxx::connection &c){
    if (c.is_open()){
        c.close();
    }
    c = pqxx::connection(database_path);
}

//------------------------------------------------------------------------------
// search_database() : returns a pqxx::result contains any rows that contain
// the specified date and time of a query. The returned pqxx::result may
// contain no rows at all.
//------------------------------------------------------------------------------
pqxx::result search_database(pqxx::connection &c, std::string date, std::string time){
    // Time format should be default (yyyy/mm/dd)
    // Some form of checking for time

    // If all goes well, do a query on the database.
    pqxx::work work(c);

    // For now, only return one query,
    ostringstream os;
    os << "SELECT * FROM log WHERE log_date = " << work.esc(date) << " AND log_time = " << work.esc(time);
    pqxx::result result = work.exec(os.str());

}
