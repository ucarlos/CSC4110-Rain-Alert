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
#include <chrono>
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


//------------------------------------------------------------------------------
// Testing Functions: For debugging purposes.
//------------------------------------------------------------------------------
void test_smtp(void);


//------------------------------------------------------------------------------
// Global Pthreads:
// They are only to be used for the Sensor Tracking and Emailing Sending
// Functions.
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Sensor Tracking functions
//------------------------------------------------------------------------------


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
uint32_t return_time_in_seconds(string &time);
void get_time_from_file(void);



#endif
