//--------------------------------------------------------------------------------------------------
// Created by Ulysses Carlos on 11/22/21 using Ulysses-H270-HD3.
//--------------------------------------------------------------------------------------------------


#ifndef CSC4110_PROJECT_SENSORDATE_H
#define CSC4110_PROJECT_SENSORDATE_H

#include "Project_Base.h"
//------------------------------------------------------------------------------
// Date class
// This class handles the user-specified email date (As in when the daily email
// is supposed to be sent). The default time is at midnight every day (12:00am)
// and is stored in default_time. user_time is defined in a text file
// (Haven't implemented a encryption/decryption function yet)
//------------------------------------------------------------------------------
class SensorDate {
public:
    // Default Constructor
    SensorDate(){
        user_time = default_time;
    }

    explicit SensorDate(std::string &d_t, std::string &u_t) :
        default_time{d_t}{
        // Set seconds:
        change_user_time(u_t);

    }
    // String constructor
    explicit SensorDate(std::string &defined_user_time){
        change_user_time(defined_user_time);
    }

    // Copy Constructor:
    SensorDate(const SensorDate &s_d);

    void change_user_time(std::string &n_t);
    void reset_user_time();

    // Handle reading from ifstreams
    friend std::istream& operator>>(std::ifstream &ifs, SensorDate &sd);
    SensorDate& operator=(const SensorDate &sd); // Copy Assignment

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
// SensorDate functions
//------------------------------------------------------------------------------
// HEY! YOU NEED TO MAKE SURE THAT THE TIME ZONE IS SET BEFORE
// DOING ANYTHING WITH THE SENSOR DATE! OTHERWISE, THE EMAIL WON'T BE
// SENT AT THE RIGHT TIME!

//static int8_t time_zone = -4; // We're in Eastern Standard Time. (UTC -4)
int64_t return_time_in_seconds(std::string &time);
void read_user_time(SensorDate &sd);
bool verify_time_zone(const int32_t &time_zone);
std::string twelve_hour_clock(const std::string &time);
bool verify_time(const std::string &time);
bool verify_date(const std::string &date);

[[maybe_unused]] std::string string_to_seconds(int64_t &sec);
#endif //CSC4110_PROJECT_SENSORDATE_H
