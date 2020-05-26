/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/22/2020 at 05:22 PM
 * 
 * Sensor_Date.cc
 * Definition of member functions and operator functions for the
 * Sensor_Date class.
 * -----------------------------------------------------------------------------
 */

#include "./Project.h"


//------------------------------------------------------------------------------
// twelve_hour_clock(): Quick and unsafe function to get an 12-hour clock.
// string time should be a 24-hour clock in the form hh::mm. Other time
// formats won't work.
//------------------------------------------------------------------------------

std::string twelve_hour_clock(const string &time){
	ostringstream os;
	uint32_t hour_val = (10 * (time[0] - '0')) + (time[1] - '0');
	if (!hour_val)
		os << "12:" << time[3] << time[4] << " AM";
	else if (hour_val >= 12)
		os << (hour_val - 12) << ":" << time[3] << time[4] << " PM";
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
    int64_t hour = 10 * (time[0] - '0') + (time[1] - '0');
    int64_t min = 10 * (time[3] - '0') + (time[4] - '0');

    // Check if date is valid
    return ((0 <= hour && hour <= 23) && (0 <= min && min <= 59));
    

}
//------------------------------------------------------------------------------
// verify_date(): Check whether the date is in the form mm/dd/yyyy using a
// regex expression. Not very accurate.
//------------------------------------------------------------------------------
bool verify_date(const string &date){
	bool check;
	string regex_str{R"(^\d{2}[\/]\d{2}[\/]\d{4}$)"};

	static std::regex test{regex_str};
	check = regex_match(date, test);
	if (!check) return false;

	//Get month:
	uint32_t month = 10 * (date[0] - '0') + (date[1] - '0');
	uint32_t day = 10 * (date[3] - '0') + (date[4] - '0');
	uint32_t year = 1000 * (date[6] - '0') + 100 * (date[7] - '0')
			+ 10 * (date[8] - '0') + (date[9] - '0');

	check = (1 <= month && month <= 12);
	check &= (1 <= day && day <= 31);
	check &= (2000 <= year && day <= 9999);

	// Now check for leap years
	bool is_leap_year = (!(year % 4) || !(year % 400));

	// No Feb 29 on an non-leap year
	if (!is_leap_year && ((month == 2) && (day == 29)))
		check = false;

	return check;
}

//------------------------------------------------------------------------------
// verify_time_zone(): Check whether time zone is in range of UTC [-12: 12]
// Please don't use this when in Australia or some country that has fractional
// time zones or whatever. I know that Australia has something like UTC + 9 1/2 
//------------------------------------------------------------------------------
bool verify_time_zone(const int32_t &time_z){
    return -12 <= time_z && time_z <= 12;
}

//------------------------------------------------------------------------------
// return_time_in_seconds() : UNSAFE function to convert a string of hh:mm
// to seconds for the std::chrono::seconds variable.
// Assume that the string is valid.
//------------------------------------------------------------------------------
int64_t return_time_in_seconds(string &time){
    int64_t unit = 60; // 60 seconds;
    int64_t hour = 10 * (time[0] - '0') + (time[1] - '0');
    int64_t min = 10 * (time[3] - '0') + (time[4] - '0');

    // Don't remove the - time_zone. It accounts for UTC - x time.
    // So, since we're in UTC - 4 (EST), it will add 4 hours.
    // Make sure that UTC - timezone fits 00: 23, so
    int64_t utc_time = (hour - project_file->get_time_zone());
    int64_t adjusted_hour = (utc_time > 23) ? ((utc_time % 24))
    		: (utc_time < 0) ? (24 - utc_time) : utc_time;

	return (!hour && !min) ? 0 :
		   ((unit * unit * (adjusted_hour)) + (unit * min));

}

[[maybe_unused]] std::string string_to_seconds(int64_t &sec){
	ostringstream os;
	int64_t hours = (sec / 3600) + project_file->get_time_zone();
	int64_t minutes = ((sec % 3600) / 60);

	os << hours << ":";
	if (minutes < 10)
		os << "0" << minutes;
	else
		os << minutes;

	return os.str();
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

void read_user_time(Sensor_Date &sd){
	if (can_use_boost) {
		string temp = project_file->get_email_time();
		sd = Sensor_Date{temp};
	}
	else
		get_time_from_file(sd);

}
