/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/01/2020 at 11:37 PM
 * 
 * Project.h
 * 
 * -----------------------------------------------------------------------------
 */

#ifndef CSC4110_PROJECT
#define CSC4110_PROJECT
// Standard Library Headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <string>
#include <new>
#include <cmath>
#include <memory>
#include <cstdint>
#include <map>
#include <algorithm>
#include <stdexcept>
using namespace std;

// Postgresql library headers
#include <pqxx/pqxx>
#include <pqxx/field>

// SMTP Library (C++ Wrapper)
#include "./smtp/SMTPMail.h"

// Project version number
#include "./include/Project_Version.h"



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

//------------------------------------------------------------------------------
// Log class
//------------------------------------------------------------------------------

// Since each log contains id, time_stamp and other mic. columns,
// This offset is used to know where we can take info from a row to a log
// class member. Change it if the log table has changes in its column.
constexpr int log_offset = 4;


class Log{
public:
    Log()=default;
    explicit Log(pqxx::row &row);
    // Log(bool fsc, bool rsc, bool on_s, bool on_b, double b_l, double r_l, string com)
    // 	: float_sensor_connected{fsc}, rain_sensor_connected{rsc},
    // 	  on_solar_energy{on_s}, on_battery{on_b}, battery_level{b_l},
    // 	  rain_level{r_l}, comment{com} { }
    
    Log(map<string, bool> &sensor, map<string, double> &levl, string comm);

    // bool float_sensor_connected{false};
    // bool rain_sensor_connected{false};
    // bool on_solar_energy{false};
    // bool on_battery{false};
    map<string, bool> sensor_check = {{"float_sensor", false},
				      {"rain_sensor", false},
				      {"solar_panel", false},
				      {"battery_on", false}};

    map<string, double> level = {{"battery_level", 100}, {"rain_level", 4.2}};
    
    // double battery_level{NAN};
    // double rain_level{NAN};
    string comment;
    pqxx::row& row(){ return *raw_data; }
    [[nodiscard]] const pqxx::row& row() const { return *raw_data; }

    bool extract_row();
    
private:
    pqxx::row *raw_data {nullptr};
};

Log::Log(map<string, bool> &sensor, map<string, double> &levl, string comm){
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

}

Log::Log(pqxx::row &row){ // Extract the contents of row and fill the log.
    raw_data = &row;
    this->extract_row();
}

bool Log::extract_row() {
    if (!raw_data) // Prevent access of empty raw_data
        return false;
    
    // First, added the booleans
    auto it = raw_data->begin() + log_offset;

    bool b = false;
    pqxx::field f;
    // Bool
    for (auto i = sensor_check.begin(); (i != sensor_check.end() && it != raw_data->end()); i++, it++){
        f = *it;
        i->second = f.as(b);
    }

    double d{NAN};
    for (auto i = level.begin(); (i != level.end() && it != raw_data->end()); i++, it++){
        f = *it;
        i->second = f.as(d);
    }

    // This assumes that the column that it points to is a boolean value.
    if (it != raw_data->end()){
        string temp;
        f = *it;
        comment = f.as(temp);
    }

    return true;


}

void extract_row_to_log(Log &l, pqxx::row &row){
    if (l.row() == row){
	l.extract_row();
	return;
    }
    
    l.row() = row;
    l.extract_row();

}






// Any templates/ Classes?
#endif
