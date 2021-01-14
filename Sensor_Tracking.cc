/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/22/2020 at 03:24 PM
 * 
 * Sensor_Tracking.cc
 * 
 * I've separated all the functions related to the sensor_thread tracking into
 * a single c++ file.
 * -----------------------------------------------------------------------------
 */

#include "./Project.h"
#include "sensors/Sensor.h"


//------------------------------------------------------------------------------
// string_to_lower() : Pretty self-explanatory, used for toggling sensor_thread
// tracking on/off and any other function where strings need to be turned
// lowercase.
//------------------------------------------------------------------------------
void string_to_lower(std::string &str){
    char temp;
    for (char &ch : str){
		temp = ch;
		ch = tolower(temp);
    }
}

/**
 * Update the passed log object with data from the dummy sensors,
 * returning a boolean representing whether the operation succeeded.
 *
 * @param log A log object that composes all information to send an email.
 * @param merse Random number generator used to create values.
 * @param rain_values a number distribution of double values
 * @returns a boolean value representing if the operation succeeded or not.
 */
bool read_alternative_sensor_values(Log *log,
									std::mt19937 merse,
									std::uniform_real_distribution<double> rain_values) {

	if (log == nullptr) { // return an error immediately.
		return false;
	}

	bool check_rain_sensor = check_rain_connection();
	bool check_float_sensor = get_float_sensor_readings();
	bool check_sensors = check_rain_sensor && check_float_sensor;

	log->sensor_check["Rain Sensor"] = check_rain_sensor;
	log->sensor_check["Float Sensor"] = static_cast<bool>(check_float_sensor);

	// Done if sensor_thread connection cannot be made.
	if (!check_sensors) {
		const char *comment = "<strong>Critical Error: Cannot read from the "
							  "rain sensor_thread. Please send a technician to inspect the system."
							  "</strong>";
		const char *error_message = "<strong>CRITICAL ERROR</strong>";
		log->comment = comment;
		log->set_error(true);
		log->set_error_message(error_message);
		return false;
	}
	// Read the temperature values
	log->level["rain_level"] = get_rain_sensor_readings(merse,
														rain_values);

	if (log->level["rain_level"] >= rain_limit) {
		const char *comment = "<strong>Rain levels have exceeded the maximum limit. "
							  "Please contact a technician to inspect the device.</strong>";
		log->set_error(true);
		log->comment = comment;
		const char *error_message = "<strong>CRITICAL ERROR</strong>";
		log->set_error_message(error_message);
		return false;
	}

	// Return true if everything goes well.
	return true;
}


/**
 * \brief Function handled by the sensor_thread thread that sends emails and reads sensor_thread data.
 * Function that handles reading data from connected sensors and sending
 * an email at a specified date.
 * @param args_struct object containing a Log object that used to send emails
 * in the event of an error or for a daily update and a Sensor_Date object that
 * specifies when an daily email is to be sent.
 * @returns nullptr
 */
void *handle_sensor_thread(Thread_Args &args_struct) {
    // Now convert the temp_log back to a Log
    Thread_Args *args = &args_struct;

    Log *log = args->log;
    Sensor_Date *sdate = args->sensor_date;
	std::string error_message;
	int email_thread_check;
	bool read_check;
	time_t current_time;
	uint64_t check;
	std::ostringstream os;

	// Set up RNG
    std::mt19937 merse;
    std::uniform_real_distribution<double> rain_values;
    initialize_rain_sensor(merse, rain_values);

    do {
		current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		check = current_time % MAX_SECONDS_IN_DAY;
		auto ll = sdate->get_email_time().count();
		// Send daily email report at the user provided time.
		if (check == sdate->get_email_time().count()){
			std::cout << "Writing Email" << std::endl;
			email_thread = std::thread{send_email_thread, std::ref(*args)};
			email_thread.join();
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			continue;
		}

		pthread_mutex_lock(&log_mutex);
		read_check = read_alternative_sensor_values(log, merse, rain_values);


		// If read_check is false, create a email thread and wait for it to join.
		if (!read_check){
			pthread_mutex_unlock(&log_mutex);
			error_message = log->get_error_message();
			break;
		}

		// If another function needs to disable tracking (For example, Test_Sensors),
		// do so.
		if (project_file->threads_are_disabled() || project_file->is_tracking_disabled()){
			pthread_mutex_unlock(&log_mutex);
			return nullptr;
		}

		pthread_mutex_unlock(&log_mutex);
		// Now sleep for 1 second.
		std::this_thread::sleep_for(std::chrono::milliseconds (500));
		std::cout << "Looping again!" << std::endl;
    } while (true);

	//email_thread_check = pthread_create(&email_thread, nullptr, send_email_thread, args);
	//std::string str = "Could not create email thread.";
	//check_error_code(email_thread_check, str);
	email_thread = std::thread{send_email_thread, std::ref(*args)};
	email_thread.join();
	//pthread_join(email_thread, nullptr);
	std::cout << "Exiting..." << std::endl;
    return nullptr;
    
}

/**
 * \brief Thread function that handles sending an email.
 * @param args_struct Thread_Args object that contains a Sensor_Date and
 * Log pointer. This is used since pthread_create only accepts a single argument.
 *
 * @returns nullptr
 */
void * send_email_thread(Thread_Args &args_struct) {

	Thread_Args *thread_args = &args_struct;
	Sensor_Date *sdate = thread_args->sensor_date;
	Log *log = thread_args->log;

	time_t current_time;
	std::ostringstream os;

	pthread_mutex_lock(&log_mutex);
	open_connection(db_connect);
	pqxx::work transaction{db_connect};

	current_time = std::time(nullptr);
	os << std::put_time(std::localtime(&current_time), "%c");
	log->time_stamp = os.str();
	os.str("");
	os << std::put_time(std::localtime(&current_time), "%B %d %Y");
	log->date = os.str();

	std::string message;
	if (!log->has_error_ocurred()) {
		message = "Daily Report";
		log->comment = "This is the daily report generated at ";
		log->comment += sdate->get_user_time();
	}
	else
		message = log->get_error_message();

	project_log = *log;
	// Update database
	add_log(transaction, project_log);

	// Now send email (HTML or Plain Text)
	if (project_file->get_email_type() == email_type::html)
		send_log_as_HTML(project_log, message);
	else
		send_log_as_text(project_log, message);

	// Unlock the resources.
	pthread_mutex_unlock(&log_mutex);

	return nullptr;
}