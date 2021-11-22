/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/22/2020 at 03:24 PM
 * 
 * Sensor_Tracking.cc
 * 
 * I've separated all the functions related to the sensor tracking into
 * a single c++ file.
 * -----------------------------------------------------------------------------
 */

#include "./Project.h"
#include "sensors/Sensor.h"


/**
 * Convert all characters in a string to lowercase.
 * @param str String to convert to lowercase.
 */
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
bool read_alternative_sensor_values(SensorLog& log,
									std::mt19937 merse,
									std::uniform_real_distribution<double> rain_values) {

    DebugLog& debug_file = DebugLog::instance();
    debug_file << DebugLevel::INFO
               << ": read_alternative_sensor_values(): Checking rain and float sensors"
               << std::endl;

	bool check_rain_sensor = check_rain_connection();
	bool check_float_sensor = get_float_sensor_readings();
	bool check_sensors = check_rain_sensor && check_float_sensor;

    debug_file << DebugLevel::INFO
               << ": read_alternative_sensor_values(): attempting to store values."
               << std::endl;
	log.sensor_check["Rain Sensor"] = check_rain_sensor;
	log.sensor_check["Float Sensor"] = static_cast<bool>(check_float_sensor);

	// Done if sensor connection cannot be made.
	if (!check_sensors) {

        debug_file << DebugLevel::ERROR
                   << ": read_alternative_sensor_values(): Cannot read sensors; Sending error message."
                   << std::endl;
		const char *comment = "<strong>Critical Error: Cannot read from the "
							  "rain sensor. Please send a technician to inspect the system."
							  "</strong>";
		const char *error_message = "<strong>CRITICAL ERROR</strong>";
		log.comment = comment;
		log.set_error(true);
		log.set_error_message(error_message);
		return false;
	}
	// Read the temperature values
	log.level["rain_level"] = get_rain_sensor_readings(merse,
														rain_values);

	if (log.level["rain_level"] >= rain_limit) {
        debug_file << DebugLevel::ERROR
                   << ": read_alternative_sensor_values(): Rain level is greater than rain limit."
                   << std::endl;
		const char *comment = "<strong>Rain levels have exceeded the maximum limit. "
							  "Please contact a technician to inspect the device.</strong>";
		log.set_error(true);
		log.comment = comment;
		const char *error_message = "<strong>CRITICAL ERROR</strong>";
		log.set_error_message(error_message);
		return false;
	}

	// Return true if everything goes well.
	return true;
}



void * pthread_handle_sensor(void *args_struct) {
    if (args_struct == nullptr)
        ;
    Thread_Args *arguments = static_cast<Thread_Args*>(args_struct);

    SensorLog *log = arguments->log;
    SensorDate *sensor_date = arguments->sensor_date;

    handle_sensor(*log, *sensor_date);
    return nullptr;
}

/**
 * \brief Function handled by the sensor thread that sends emails and reads sensor data.
 * Function that handles reading data from connected sensors and sending
 * an email at a specified date.
 * @param args_struct object containing a SensorLog object that used to send emails
 * in the event of an error or for a daily update and a SensorDate object that
 * specifes when an daily email is to be sent.
 * @returns nullptr
 */
void handle_sensor(SensorLog &log, SensorDate &sensorDate) {
    // Now convert the temp_log back to a SensorLog

    DebugLog& debug_file = DebugLog::instance();

	int email_thread_check;
	bool read_check;

	time_t current_time;
	uint64_t check;
	std::ostringstream os;

	// Set up RNG
    std::mt19937 merse;
    std::uniform_real_distribution<double> rain_values;
    initialize_rain_sensor(merse, rain_values);

    bool active;
    do {
		current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        debug_file << DebugLevel::INFO
                   << ": handle_sensor(): Current Time: " << current_time
                   << std::endl;
		check = current_time % MAX_SECONDS_IN_DAY;

        debug_file << DebugLevel::DEBUG <<
                   ": handle_sensor(): Current Time (Modulo) : " << check
                   << std::endl;

		// Send daily email report at the user provided time.
		if (check == sensorDate.get_email_time().count()){
            debug_file << DebugLevel::INFO
                       << ": handle_sensor(): Sending Email..."
                       << std::endl;
            email_thread = std::thread{send_email, std::ref(log), std::ref(sensorDate)};
			email_thread.join();
            std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}

        {
            std::lock_guard<std::mutex> lock{main_mutex};
            debug_file << DebugLevel::INFO
                       << ": handle_sensor(): Reading from Alternative Sensors"
                       << std::endl;
            read_check = read_alternative_sensor_values(log, merse, rain_values);
        }

		// If read_check is false, create a email thread and wait for it to join.
		if (!read_check){
			debug_file << log.get_error_message();
			break;
		}

		// If another function needs to disable tracking (For example, Test_Sensors),
		// do so.
        {
            std::lock_guard<std::mutex> lock{main_mutex};
            debug_file << DebugLevel::INFO
                       <<  ": handle_sensor(): Checking if Threads are disabled:"
                       << std::endl;

            active = project_file->are_threads_disabled();
            if (active) {
                debug_file << DebugLevel::INFO
                           << ": handle_sensor(): Threads are disabled; Returning."
                           << std::endl;
                break;
            }
        }


		// Now sleep for 1 second.
        debug_file << DebugLevel::DEBUG << ": handle_sensor(): Sleeping for 500ms" << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } while (!active);

    debug_file << DebugLevel::DEBUG << ": handle_sensor(): Now sending one last email.." << std::endl;

    std::lock_guard<std::mutex> lock(main_mutex);
    email_thread = std::thread{send_email, std::ref(log), std::ref(sensorDate)};
    email_thread.join();
}


/**
 * \brief Thread function that handles sending an email.
 * @param args_struct Thread_Args object that contains a SensorDate and
 * SensorLog pointer. This is used since pthread_create only accepts a single argument.
 *
 * @returns nullptr
 */
void send_email(SensorLog &log, SensorDate &sensor_date) {
    std::lock_guard<std::mutex> lock{main_mutex};
	time_t current_time;
	std::ostringstream os;

    DebugLog& debug_file = DebugLog::instance();
    debug_file << DebugLevel::INFO << ": send_email(): Opening connection and preparing transaction.";
	open_connection(db_connect);
	pqxx::work transaction{db_connect};

	current_time = std::time(nullptr);
	os << std::put_time(std::localtime(&current_time), "%c");

	os.str("");
	os << std::put_time(std::localtime(&current_time), "%B %d %Y");
	log.date = os.str();

	std::string message;
	if (!log.has_error_ocurred()) {
		message = "Daily Report";
		log.comment = "This is the daily report generated at ";
		log.comment += sensor_date.get_user_time();
	}
	else
		message = log.get_error_message();

	project_log = log;
    debug_file << std::boolalpha;
	// Update database
    debug_file << DebugLevel::INFO << ": send_email(): Adding log to the database.";
	add_log(transaction, project_log);

	// Now send email (HTML or Plain Text)
	if (project_file->get_email_type() == email_type::html) {
        debug_file << DebugLevel::INFO << ": send_email(): Sending HTML email.";
        send_log_as_HTML(project_log, message);
    }
	else {
        debug_file << DebugLevel::INFO << ": send_email(): Sending Text-based email.";
        send_log_as_text(project_log, message);

    }

	// Unlock the resources.

}

