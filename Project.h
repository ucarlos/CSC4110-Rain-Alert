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

// Connection Library
#include "./sql/Connection.h"
// Project version number
#include "./include/Project_Version.h"

//------------------------------------------------------------------------------
// Menu Functions:
//------------------------------------------------------------------------------
void sensor_tracking(void);

void show_status(void);
void search_logs(void);
void test_sensors(void);
void database_options(void);
void email_options(void);


//------------------------------------------------------------------------------
// Testing Functions: For debugging purposes.
//------------------------------------------------------------------------------
void test_smtp(void);


// Any templates/ Classes?

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
	user_time{default_time};
    }
    explicit Sensor_Date(string default_t, string set_t) : default_time{d_t},
						   set_time{s_t} { }
    
    explicit Sensor_Date(string defined_user_time){
	this->user_time = defined_user_time;
    }
    
    void change_user_time(string n_t) { user_time = n_t; }
    void reset_user_time() { user_time = default_time; }

    // Handle reading from ifstreams
    friend ifstream& operator>>(Sensor_Date &sd);
private:
    string default_time;
    string user_time{};
    string time_in_seconds{};
    
    
};

//------------------------------------------------------------------------------
// PThread functions
//------------------------------------------------------------------------------


#endif
