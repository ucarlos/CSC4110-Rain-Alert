/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/01/2020 at 11:37 PM
 * 
 * Project.h
 * 
 * -----------------------------------------------------------------------------
 */
#pragma once
#ifndef CSC4110_PROJECT
#define CSC4110_PROJECT


//------------------------------------------------------------------------------
// Include Headers
//------------------------------------------------------------------------------


#include "Project_Base.h"
#include "SensorDate.h"
#include <pthread.h>
#include <ncurses.h>

// SensorLog Library
#include "./log/SensorLog.h"

// Sensor Library

#define SENSOR_READINGS_RNG (1)
#include "./sensors/Sensor.h"

// Connection Library
#include "./sql/PSQL_Connection.h"

// Project version number
#include "./include/Project_Version.h"

// XML Parsing Library
#include "./Settings.h"


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

//extern std::unique_ptr<Settings> project_file(new Settings{xml_path});
extern std::unique_ptr<Settings> project_file;

//------------------------------------------------------------------------------
// Menu Function Declaration
//------------------------------------------------------------------------------
void sensor_tracking();
void end_threads();
void quit_program();
extern inline void return_to_menu();
void show_status();
void search_logs();
void search_logs_by_date();
void search_logs_by_range();
void test_sensors();
void database_options();
void email_settings();
void string_to_lower(std::string &str);
void menu();
std::string version_info();
inline void check_pthread_creation(int &return_val, std::string &error_msg);
void get_credentials();


//------------------------------------------------------------------------------
// SensorLog used for the project.
//------------------------------------------------------------------------------
extern SensorLog project_log;

//------------------------------------------------------------------------------
// Option Vectors
//------------------------------------------------------------------------------
extern const std::vector<std::string> main_menu_options;
extern const std::vector<std::string> email_menu_options;
extern const std::vector<std::string> search_log_options;
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

static std::thread sensor_thread{};
static std::thread email_thread{};

static std::mutex main_mutex{};
static bool end_all_threads = false;

//------------------------------------------------------------------------------
// Class Declarations
//------------------------------------------------------------------------------
class SensorDate;

//------------------------------------------------------------------------------
// Sensor Tracking declaration
//------------------------------------------------------------------------------

bool read_alternative_sensor_values(SensorLog *log,
									std::mt19937 merse,
									std::uniform_real_distribution<double> rain_values);
void handle_sensor(SensorLog &log, SensorDate &sensorDate);
void send_email(SensorLog &log, SensorDate &sensor_date);
void * pthread_handle_sensor(void *args_struct);

// False: Disabled, True: Enabled

inline void check_pthread_creation(int &return_val, std::string &error_msg){
	if (return_val)
	    throw std::runtime_error(error_msg);
	else
	    return;
}



//------------------------------------------------------------------------------
// Structure to hold arguments for handle_sensor:
//------------------------------------------------------------------------------
struct Thread_Args {
	Thread_Args(SensorLog *l, SensorDate *sd) : log{l}, sensor_date{sd} { }
	SensorLog *log;
	SensorDate *sensor_date;
};


#endif
