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
// Project Base Library:
#include "Project_Base.h"
//#include <pthread.h>
#include <thread>
#include <ncurses.h>
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

const std::string time_path = "..log/Settings.txt";
const std::string xml_path = "../settings/Project_Settings.xml";
const std::string database_results_path = "../Database_results.txt";
const std::string log_status_path = "../log_status.txt";

//extern std::unique_ptr<settings_file> project_file(new settings_file{xml_path});
extern std::unique_ptr<settings_file> project_file;
//------------------------------------------------------------------------------
// Menu Functions and variables
//------------------------------------------------------------------------------
void sensor_tracking();
void end_threads();
void quit_program();
extern inline void return_to_menu();
void show_status();
void search_logs();
void test_sensors();
void database_options();
void email_settings();
void string_to_lower(std::string &str);
void menu();
std::string version_info();
inline void check_error_code(int &return_val, std::string &error_msg);
void get_credentials();


// Log used for the project.
extern Log project_log;
extern const std::vector<std::string> main_menu_options;
extern const std::vector<std::string> email_menu_options;
extern const std::vector<std::string> test_sensor_menu_options;
//------------------------------------------------------------------------------
// Testing Functions: For debugging purposes.
//------------------------------------------------------------------------------
void test_smtp();
void test_pthread();
void test_connection();
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
//static pthread_t sensor_thread, email_thread;

extern std::thread sensor_thread, email_thread;

// This mutex is used to prevent reading/writing to a log class
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
static bool end_all_threads = false;
//------------------------------------------------------------------------------
// Sensor Tracking functions
//------------------------------------------------------------------------------
struct Thread_Args;

bool read_alternative_sensor_values(Log *log,
									std::mt19937 merse,
									std::uniform_real_distribution<double> rain_values);
void * handle_sensor_thread(Thread_Args &args_struct);
void * send_email_thread(Thread_Args &args_struct);
// False: Disabled, True: Enabled

inline void check_error_code(int &return_val, std::string &error_msg){
	if (return_val)
	    throw std::runtime_error(error_msg);
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

    explicit Sensor_Date(std::string &d_t, std::string &u_t) :
    	default_time{d_t}{
	// Set seconds:
	change_user_time(u_t);

	}
    // String constructor
    explicit Sensor_Date(std::string &defined_user_time){
    	change_user_time(defined_user_time);
    }

    // Copy Constructor:
    Sensor_Date(const Sensor_Date &s_d);

    void change_user_time(std::string &n_t);
    void reset_user_time();

    // Handle reading from ifstreams
    friend std::istream& operator>>(std::ifstream &ifs, Sensor_Date &sd);
    Sensor_Date& operator=(const Sensor_Date &sd); // Copy Assignment

    std::chrono::seconds& get_email_time() { return email_time; }
    [[nodiscard]] std::string get_user_time() const { return user_time; }
private:
	std::string default_time{"00:00"};
	std::string user_time{};
    std::chrono::seconds email_time{MAX_SECONDS_IN_DAY};
    uint32_t email_seconds{MAX_SECONDS_IN_DAY};
    void set_email_time(uint32_t &seconds);


};
//------------------------------------------------------------------------------
// Structure to hold arguments for handle_sensor_thread:
//------------------------------------------------------------------------------
struct Thread_Args {
	Thread_Args(Log *l, Sensor_Date *sd) : log{l}, sensor_date{sd} { }
	Log *log;
	Sensor_Date *sensor_date;
};

//------------------------------------------------------------------------------
// Sensor_Date functions
//------------------------------------------------------------------------------
// HEY! YOU NEED TO MAKE SURE THAT THE TIME ZONE IS SET BEFORE
// DOING ANYTHING WITH THE SENSOR DATE! OTHERWISE, THE EMAIL WON'T BE
// SENT AT THE RIGHT TIME!

//static int8_t time_zone = -4; // We're in Eastern Standard Time. (UTC -4)
int64_t return_time_in_seconds(std::string &time);
void read_user_time(Sensor_Date &sd);
bool verify_time_zone(const int32_t &time_zone);
std::string twelve_hour_clock(const std::string &time);
bool verify_time(const std::string &time);
bool verify_date(const std::string &date);

[[maybe_unused]] std::string string_to_seconds(int64_t &sec);

//------------------------------------------------------------------------------
// Project_Settings.xml functions can be found in Log.h and PSQL_Connection.h
//
//------------------------------------------------------------------------------
#endif
