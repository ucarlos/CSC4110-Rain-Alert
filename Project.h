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
// Requires Openssl to be to the system; Usually installed on Linux
#include "./smtp/SMTPMail.h"

// Project version number
#include "./include/Project_Version.h"


// Macro and inline functions

inline static double inches_to_cm(double in) { return in * 2.54; }

// SMTP information
// const string smtp_username = "csc4110.project@gmail.com";
// const string smtp_port = "587"; // Using TLS?
// const string smtp_address = "smtp.gmail.com";
// const string smtp_password = "A password for our project.";

const string smtp_username = "ucarlos1@student.gsu.edu";
const string smtp_port = "587";
const string smtp_address = "smtp.office365.com";
const string smtp_password = "Terminus est 2";

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
    
    Log(map<string, bool> &sensor, map<string, double> &levl, string comm, string time_stamp);

    // bool float_sensor_connected{false};
    // bool rain_sensor_connected{false};
    // bool on_solar_energy{false};
    // bool on_battery{false};
    map<string, bool> sensor_check = {{"Float Sensor", false},
				      {"Rain Sensor", false},
				      {"Solar Panel", false},
				      {"Battery", false}};

    map<string, double> level = {{"battery_level", 100}, {"rain_level", 4.2}};
    
    // double battery_level{NAN};
    // double rain_level{NAN};
    string comment;
    string time_stamp{"[NOT SPECIFIED]"};
    string date{"[NOT SPECIFIED]"};
    pqxx::row& row(){ return *raw_data; }
    [[nodiscard]] const pqxx::row& row() const { return *raw_data; }

    bool extract_row();
    void write_to_file(ofstream &ofs) { ofs << this; }
    friend ostream& operator<<(ostream &os, const Log &l);
    
private:
    pqxx::row *raw_data {nullptr};
};

void add_log(pqxx::transaction_base &trans, const Log &l);
void send_log_as_SMTP_body(const Log &l, ifstream &ifs);
void send_log_as_SMTP_attachment(const Log &l, ifstream &ifs);
bool check_smtp_connection(SMTPMail &mail);

//------------------------------------------------------------------------------
// Connection functions -- Defined in Connection.cc
//------------------------------------------------------------------------------

void open_connection(pqxx::connection &c);
pqxx::result search_database(pqxx::connection &c, std::string date, std::string time);
// Any templates/ Classes?
#endif
