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
#include <pthread.h>
#include <memory>
#include <chrono>
#include <ctime>
#include <ncurses.h>
using namespace std;

// Log Library
#include "./log/Log.h"
// Sensor Library
#include "./sensors/Sensor.h"
// Connection Library
#include "./sql/PSQL_Connection.h"

// Project version number
#include "./include/Project_Version.h"


// XML Parsing Library
#include "./XML_Configuration.h"
// Macros
#define MAX_SECONDS_IN_DAY (86400)

//------------------------------------------------------------------------------
// File paths: If boost can't be installed, this system will be used instead.
//------------------------------------------------------------------------------
const bool can_use_boost = true;

const string time_path = "..log/Settings.txt";
const string xml_path = "../settings/Project_Settings.xml";
const string database_results_path = "../Database_results.txt";
const string log_status_path = "../log_status.txt";

//extern std::unique_ptr<settings_file> project_file(new settings_file{xml_path});
extern std::unique_ptr<settings_file> project_file;
//------------------------------------------------------------------------------
// Menu Functions and variables
//------------------------------------------------------------------------------
void sensor_tracking(void);
extern inline void return_to_menu(void);
void show_status(void);
void search_logs(void);
void test_sensors(void);
void database_options(void);
void email_settings(void);
void string_to_lower(string &str);
void menu(void);
std::string version_info(void);
inline void check_pthread_creation(int &return_val, string &error_msg);
void get_credentials(void);


// Log used for the project.
extern Log project_log;
extern const vector<string> main_menu_options;
extern const vector<string> email_menu_options;
//------------------------------------------------------------------------------
// Testing Functions: For debugging purposes.
//------------------------------------------------------------------------------
void test_smtp(void);
void show_result_contents(pqxx::result &r);
//------------------------------------------------------------------------------
// Global Database variables:
// Should only be used by Sensor_Tracking, Email Sending, and Database
// Searching functions.
//------------------------------------------------------------------------------
static pqxx::connection db_connect;
//------------------------------------------------------------------------------
// Global Pthreads:
// They are only to be used for the Sensor Tracking and Emailing Sending
// Functions.
//------------------------------------------------------------------------------
static pthread_t sensor, email;

// This mutex is used to prevent reading/writing to a log class
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
static bool end_all_threads = false;
//------------------------------------------------------------------------------
// Sensor Tracking functions
//------------------------------------------------------------------------------
void* handle_sensor_thread(void *temp_log);
void* send_email_thread(void *s_d);
// False: Disabled, True: Enabled

inline void check_pthread_creation(int &return_val, string &error_msg){
	if (return_val)
	    throw runtime_error(error_msg);
	else
	    return;
}


//------------------------------------------------------------------------------
// Date class
// This class handles the user-specified email date (As in when the daily email
// is supposed to be sent). The default time is at midnight every day (12:00am)
// and is stored in default_time. user_time is defined in a text file
// (Haven't implemented a encryption/decryption function yet)
//------------------------------------------------------------------------------
class Sensor_Date{
public:
	// Default Constructor
    Sensor_Date(){
		user_time = default_time;
    }

    explicit Sensor_Date(string &d_t, string &u_t) :
    	default_time{d_t}{
	    // Set seconds:
	    change_user_time(u_t);

	}
    // String constructor
    explicit Sensor_Date(string &defined_user_time){
    	change_user_time(defined_user_time);
    }

    // Copy Constructor:
    Sensor_Date(const Sensor_Date &s_d);

    void change_user_time(string &n_t);
    void reset_user_time();

    // Handle reading from ifstreams
    friend istream& operator>>(ifstream &ifs, Sensor_Date &sd);
    Sensor_Date& operator=(const Sensor_Date &sd); // Copy Assignment

    std::chrono::seconds& get_email_time() { return email_time; }
    [[nodiscard]] string get_user_time() const { return user_time; }
private:
    string default_time{"00:00"};
    string user_time{};
    std::chrono::seconds email_time{MAX_SECONDS_IN_DAY};
    uint32_t email_seconds{MAX_SECONDS_IN_DAY};
    void set_email_time(uint32_t &seconds);


};


//------------------------------------------------------------------------------
// Sensor_Date functions
//------------------------------------------------------------------------------
// HEY! YOU NEED TO MAKE SURE THAT THE TIME ZONE IS SET BEFORE
// DOING ANYTHING WITH THE SENSOR DATE! OTHERWISE, THE EMAIL WON'T BE
// SENT AT THE RIGHT TIME!

//static int8_t time_zone = -4; // We're in Eastern Standard Time. (UTC -4)
int64_t return_time_in_seconds(string &time);
void read_user_time(Sensor_Date &sd);
bool verify_time_zone(const int32_t &time_zone);
std::string twelve_hour_clock(const string &time);
bool verify_time(const string &time);
bool verify_date(const string &date);
std::string string_to_seconds(int64_t &sec);

//------------------------------------------------------------------------------
// Project_Settings.xml functions can be found in Log.h and PSQL_Connection.h
//
//------------------------------------------------------------------------------



#endif
