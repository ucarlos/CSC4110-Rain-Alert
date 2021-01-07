/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/12/2020
 * 
 * Log.h
 * Header.
 * -----------------------------------------------------------------------------
 */


#ifndef CSC4110_PROJECT_LOG_H
#define CSC4110_PROJECT_LOG_H

// Macro and inline functions
// Include Project Base
#include "../Project_Base.h"
#include <cmath>
#include <regex>
#include <unistd.h>

// Postgresql library headers
#include <pqxx/pqxx>
//#include <pqxx/field>

// SMTP Library (C++ Wrapper)
// Requires Openssl to be to the system; Usually installed on Linux
#include <curl/curl.h>


//------------------------------------------------------------------------------
// Static variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------------
inline static double inches_to_cm(double in) { return in * 2.54; }


// SMTP information

const std::string smtp_address = "smtp.gmail.com";
const std::string smtp_port = "587"; // We're using TLS.


static std::string smtp_username;
static std::string smtp_password;
static std::string smtp_receiver_address;


[[maybe_unused]] void get_smtp_credentials();

//------------------------------------------------------------------------------
// Filepaths
//------------------------------------------------------------------------------
const std::string smtp_file_path = "../settings/smtp_info.txt";
const std::string text_file_path = "../log/Report.txt";
const std::string html_template_path = "../log/html_template.html";
[[maybe_unused]] const std::string html_file_path = "../log/Report.html";


//------------------------------------------------------------------------------
// Log class
//------------------------------------------------------------------------------

// Since each log contains id, time_stamp and other mic. columns,
// This offset is used to know where we can take info from a row to a log
// class member. Change it if the log table has changes in its column.
constexpr int log_offset = 4;


class Log{
public:
    // Default constructor
    Log()=default;
    explicit Log(pqxx::row &row);
    // Copy constructor
    Log (const Log &l);
    // Copy Assignment
    Log& operator=(const Log &b);
    
    Log(std::map<std::string, bool> &sensor, std::map<std::string, double> &levl,
	std::string comm, std::string time_stamp);
    // Comparision Operator:
    bool operator==(const Log &b);
    

    
    std::map<std::string, bool> sensor_check = {{"Float Sensor", false},
                                      {"Rain Sensor", false},
                                      {"Solar Panel", false},
                                      {"Battery", false}};

    std::map<std::string, double> level = {{"battery_level", NAN}, {"rain_level", NAN}};

    std::string comment;
    std::string time_stamp{"[NOT SPECIFIED]"};
    std::string date{"[NOT SPECIFIED]"};
    pqxx::row& row(){ return *raw_data; }
    [[nodiscard]] const pqxx::row& row() const { return *raw_data; }

    bool extract_row();
    void write_to_file(std::ofstream &ofs) { ofs << this; }
    friend std::ostream& operator<<(std::ostream &os, const Log &l);
	void apply_raw_data(pqxx::row &r_data){ raw_data = &r_data; this->extract_row(); }
private:
    pqxx::row *raw_data {nullptr};
};

void add_log(pqxx::transaction_base &trans, const Log &l);
void send_html_through_SMTP(const Log &l, std::ostringstream &oss, std::string &message_type);
void send_log_as_HTML(const Log &l, std::string &message_type);
void send_log_as_text(const Log &l, std::string &message_type);
std::vector<std::string> create_html_header(std::string &message_type);

[[maybe_unused]] std::string system_call_to_string(const char* cmd);
std::string create_date(std::string format_string);
std::string create_smtp_text_header(std::string &message_type);
void send_text_through_SMTP(std::ostringstream &oss, std::string &message_type);
bool verify_username(std::regex &test, const std::string &user_name);
bool verify_password(std::regex &test, const std::string &password);

[[maybe_unused]] bool verify_password(std::regex &test, int8_t &size, const std::string &password);

//------------------------------------------------------------------------------
// Project_Settings.xml functions
//------------------------------------------------------------------------------
void get_smtp_info_from_xml(const std::map<std::string, std::string> &smtp_info);
#endif //CSC4110_PROJECT_LOG_H
