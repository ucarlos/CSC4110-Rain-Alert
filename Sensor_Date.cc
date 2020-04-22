/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/22/2020 at 05:22 PM
 * 
 * Sensor_Date.cc
 * Defintion of member functions and operator functions for the 
 * Sensor_Date class.
 * -----------------------------------------------------------------------------
 */

#include "./Project.h"


//------------------------------------------------------------------------------
// verify_time() : Checks whether the time is valid by first using a regex
// test and then determine whether hours are in [0 : 23] and minutes are in
// [0 : 59]
// To make things easier, use 24-hour clock (00:00 - 23:59)
//------------------------------------------------------------------------------

bool verify_time(const string &time){
    bool check;
    string regex_str = "\\d{2}:\\d{2}";
    
    static std::regex test{regex_str};
    check = regex_match(time, test);
    if (!check) return false;

    // Shitty hack to get values of hour and min
    int hour = 10 * (time[0] - '0') + (time[1] - '0');
    int min = 10 * (time[3] - '0') + (time[4] - '0');

    // Check if date is valid
    return ((0 <= hour && hour <= 23) || (0 <= min && min <= 59));
    

}

//------------------------------------------------------------------------------
// return_time_in_seconds() : UNSAFE function to convert a string of hh:mm
// to seconds for the std::chrono::seconds variable.
// Assume that the string is valid.
//------------------------------------------------------------------------------
uint32_t return_time_in_seconds(string &time){
    uint32_t unit = 60; // 60 seconds;
    uint32_t hour = 10 * (time[0] - '0') + (time[1] - '0');
    uint32_t min = 10 * (time[3] - '0') + (time[4] - '0');

    // In order to prevent division by 0, the default time
    // of 00:00 will be set as 86400, the maxiumum seconds in a day.
	return !hour && !min ? MAX_SECONDS_IN_DAY :
	    (unit * unit * hour) + (unit * min);

}

//------------------------------------------------------------------------------
// Sensor_Date class defintions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Sensor_Date::set_email_time(uint32_t &seconds){
    email_time = std::chrono::seconds{seconds};
}

void Sensor_Date::change_user_time(string &n_t){
    // First check is n_t is valid;
    bool check = verify_time(n_t);
    if (!check)
	reset_user_time();
    else {
	email_seconds = return_time_in_seconds(n_t);
	set_email_time(email_seconds);
    }

}
//------------------------------------------------------------------------------
// reset_user_time() : Resets the time back to 12:00am (00:00)
//------------------------------------------------------------------------------
void Sensor_Date::reset_user_time(){
    user_time = default_time;
    email_seconds = MAX_SECONDS_IN_DAY;
    set_email_time(email_seconds);

}

// Allows reading from cin and other istreams
istream& operator>>(ifstream &ifs, Sensor_Date &sd){
    string input;
    ifs >> input;

    bool check = verify_time(input);
    if (!check) // Make default time
	sd = Sensor_Date();
    else  // Otherwise, set the user_time to input.
	sd = Sensor_Date(input);	

    return ifs;
}




