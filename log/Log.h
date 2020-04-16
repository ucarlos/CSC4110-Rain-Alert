//--------------------------------------------------------------------------------------------------
// Created by Ulysses Carlos on 4/12/20 using Ulysses-H270-HD3.
//--------------------------------------------------------------------------------------------------


#ifndef CSC4110_PROJECT_LOG_H
#define CSC4110_PROJECT_LOG_H

// Macro and inline functions
#include <iostream>
#include <fstream>
#include <sstream>

//#include <utility>
#include <vector>
#include <string>
//#include <new>
#include <cmath>
//#include <memory>
#include <cstdint>
#include <map>
// #include <algorithm>
#include <stdexcept>
#include <cstdio>
#include <regex>

// Postgresql library headers
#include <pqxx/pqxx>
#include <pqxx/field>

// SMTP Library (C++ Wrapper)
// Requires Openssl to be to the system; Usually installed on Linux
#include <curl/curl.h>
using namespace std;
inline static double inches_to_cm(double in) { return in * 2.54; }

// SMTP information

const string smtp_address = "smtp.gmail.com";
const string smtp_port = "587"; // Using TLS?


static string smtp_username;
static string smtp_password;
static string smtp_receiver_address;


void get_smtp_credentials(void);

//------------------------------------------------------------------------------
// Filepaths
//------------------------------------------------------------------------------
const string text_file_path = "../log/Report.txt";
const string html_template_path = "../log/html_template.html";
const string html_file_path = "../log/Report.html";


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

    map<string, double> level = {{"battery_level", NAN}, {"rain_level", NAN}};

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
void send_html_through_SMTP(const Log &l, ostringstream &oss, string &message_type);
void send_log_as_HTML(const Log &l, string &message_type);
void send_log_as_text(const Log &l, string &message_type);
vector<std::string> create_html_header(string &message_type);
std::string system_call_to_string(const char* cmd);
std::string create_smtp_text_header(string &message_type);
void send_text_through_SMTP(ostringstream &oss, string &message_type);

bool verify_username(const string &user_name);
bool verify_password(const string &password);
bool verify_password(const string &password, int8_t &size);

#endif //CSC4110_PROJECT_LOG_H
