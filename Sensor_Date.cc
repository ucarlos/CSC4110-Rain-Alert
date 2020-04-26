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
// twelve_hour_clock(): Quick and unsafe function to get an 12-hour clock.
// string time should be a 24-hour clock in the form hh::mm. Other time
// formats won't work.
//------------------------------------------------------------------------------

std::string twelve_hour_clock(string &time){
	ostringstream os;
	uint32_t hour_val = (10 * (time[0] - '0')) + (time[1] - '0');
	if (!hour_val)
		os << "12:" << time[3] << time[4] << " AM";
	else if (hour_val >= 12)
		os << hour_val << ":" << time[3] << time[4] << " PM";
	else os << hour_val << ":" << time[3] << time[4] << " AM";

	return os.str();

}
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

    //  hack to get values of hour and min
    uint32_t hour = 10 * (time[0] - '0') + (time[1] - '0');
    uint32_t min = 10 * (time[3] - '0') + (time[4] - '0');

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

    // Don't remove the - time_zone. It accounts for UTC - x time.
    // So, since we're in UTC - 4 (EST), it will add 4 hours.
	return (!hour && !min) ? 0 :
		   ((unit * unit * (hour - time_zone)) + (unit * min));

}

//------------------------------------------------------------------------------
// Sensor_Date class definitions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// Copy Constructor:
Sensor_Date::Sensor_Date(const Sensor_Date &s_d) {
	user_time = s_d.user_time;
	default_time = s_d.default_time;
	email_time = s_d.email_time;
	email_seconds = s_d.email_seconds;
}

void Sensor_Date::set_email_time(uint32_t &seconds){
    email_time = std::chrono::seconds{seconds};
}

void Sensor_Date::change_user_time(string &n_t){
    // First check is n_t is valid;
    bool check = verify_time(n_t);
    if (!check)
	reset_user_time();
    else {
    	user_time = n_t;
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
// Copy Assignment
Sensor_Date& Sensor_Date::operator=(const Sensor_Date &sd)= default;

void get_time_from_file(Sensor_Date &sd) {
	ifstream ifs{time_path};
	if (!ifs){
		throw runtime_error("Could not open " + time_path);
	}
	string temp;
	getline(ifs, temp);

	sd = Sensor_Date{temp};
}


