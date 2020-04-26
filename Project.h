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

#include <chrono>
#include <ctime>
using namespace std;

// Log Library
#include "./log/Log.h"
// Sensor Library
#include "./sensors/Sensor.h"
// Connection Library
#include "./sql/Connection.h"
// Project version number
#include "./include/Project_Version.h"

// Macros
#define MAX_SECONDS_IN_DAY (86400)
const string time_path = "../Settings.txt";

//------------------------------------------------------------------------------
// Menu Functions and variables
//------------------------------------------------------------------------------
void sensor_tracking(void);
inline void return_to_menu(void);
void show_status(void);
void search_logs(void);
void test_sensors(void);
void database_options(void);
void email_options(void);

const static vector<string> options = {"Enable/Disable Tracking",
									   "Status",
									   "Search Logs",
									   "Test Sensors",
									   "Database Options",
									   "Email Options",
									   "Quit"};


// Log used for the project.
static Log project_log;

//------------------------------------------------------------------------------
// Testing Functions: For debugging purposes.
//------------------------------------------------------------------------------
void test_smtp(void);

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

// False: Disabled, True: Enabled
static bool tracking_status = false;

//------------------------------------------------------------------------------
// Date class
// This class handles the user-specified email date (As in when the daily email
// is supposed to be sent). The default time is at midnight every day (12:00am)
// and is stored in default_time. user_time is defined in a text file
// (Haven't implemented a encryption/decryption function yet)
//------------------------------------------------------------------------------
class Sensor_Date{
public:
    Sensor_Date(){
		user_time = default_time;
    }

    explicit Sensor_Date(string d_t, string u_t) :
    	default_time{d_t}{
	    // Set seconds:
	    change_user_time(u_t);

	}
    
    explicit Sensor_Date(string defined_user_time){
    	change_user_time(defined_user_time);
    }
    
    void change_user_time(string &n_t);
    void reset_user_time();

    // Handle reading from ifstreams
    friend istream& operator>>(ifstream &ifs, Sensor_Date &sd);
    Sensor_Date& operator=(const Sensor_Date &sd); // Copy Assignment
    std::chrono::seconds& get_email_time() { return email_time; }
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
static 	int8_t time_zone = -4; // We're in Eastern Standard Time. (UTC -4)
uint32_t return_time_in_seconds(string &time);
void get_time_from_file(Sensor_Date &sd);



#endif
