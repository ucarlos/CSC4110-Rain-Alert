/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/05/2020 at 06:16 PM
 * 
 * log.cpp
 * 
 * -----------------------------------------------------------------------------
 */

#include "../Project.h"

Log::Log(map<string, bool> &sensor, map<string, double> &levl, string comm, string time_stamp) {
    // Maps should be the same size as default. Error otherwise.
    if ((this->sensor_check.size() != sensor.size()) ||
        (this->level.size() != levl.size())){
        ostringstream os;
        os << "Passed sensor and level arguments have different sizes"
           << " from the default maps in Log. Aborting.";
        throw runtime_error(os.str());
    }
    this->sensor_check = sensor;
    this->level = levl;
    this->comment = std::move(comm);
    this->time_stamp = time_stamp;

}

// constructor
Log::Log(pqxx::row &row){ // Extract the contents of row and fill the log.
    raw_data = &row;
    this->extract_row();
}


//------------------------------------------------------------------------------
// extract_row() -- extracts the appropriate fields from raw_data.
//------------------------------------------------------------------------------
bool Log::extract_row() {
    if (!raw_data) // Prevent access of empty raw_data
        return false;

    // First, added the booleans
    auto it = raw_data->begin() + (log_offset - 1);
    time_stamp = it->as(string{});

    it++;

    // pqxx::field f; // Uses field
    // Bool
    for (auto i = sensor_check.begin(); (i != sensor_check.end() && it != raw_data->end()); i++, it++){
        i->second = it->as(bool{});
    }

    for (auto i = level.begin(); (i != level.end() && it != raw_data->end()); i++, it++){
        i->second = it->as(double{});
    }

    // This assumes that the column that it points to is a boolean value.
    if (it != raw_data->end()){
        comment = it->as(string{});
    }

    return true;


}
//------------------------------------------------------------------------------
// extract_row_to_log - For any row
//------------------------------------------------------------------------------
void extract_row_to_log(Log &l, pqxx::row &row){
    if (l.row() == row){
        l.extract_row();
        return;
    }

    l.row() = row;
    l.extract_row();

}

//------------------------------------------------------------------------------
// operator<< - outputs all fields from log(except raw_data) to a ostream.
//------------------------------------------------------------------------------
ostream& operator<<(ostream &os, const Log &l){
    ostringstream o;

    o << "Timestamp: " << l.time_stamp << endl;

    for (auto & i : l.sensor_check){
        o << i.first << ": ";
        o << ((i.second) ? "Online\n" : "Offline\n");
    }

    // Battery level (In Percentage)
    double d = l.level.at("battery_level");
    o << "Battery Level: " << d << "%\n";
    d = l.level.at("rain_level");
    o << "Rain level: " << d << " in (" << inches_to_cm(d) << " cm)\n";
    o << "Comment: " << l.comment << endl;
    return os << o.str();

}
//------------------------------------------------------------------------------
// add_log(): Add a Log to a database.
//------------------------------------------------------------------------------
void add_log(pqxx::transaction_base &trans, const Log &l){
    ostringstream os;
    os << boolalpha;
    
    os << "INSERT INTO log(float_sensor_connected, rain_sensor_connected,"
       << " solar_panel_functional, battery_connected, battery_level, rain_level, log_comment)"
       << " VALUES (" ;

    for (const auto & i : l.sensor_check){
        os << trans.quote(i.second) << ", ";
    }


    // Use check to determine when the close the parenthesises.
    auto check = l.level.begin();
    advance(check, 1);

    for (auto i = l.level.begin(); i != l.level.end(); i++){
        if (check == l.level.end())
            os << trans.quote(i->second) << ")";
	    else
	        os << trans.quote(i->second) << ", ";
    }
    os << trans.quote(l.comment) << ")";

    string query = os.str();
    //cout << query;
    trans.exec0(query);
    
    // Now update the database.
    trans.commit();
}
