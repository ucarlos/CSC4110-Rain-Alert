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


//------------------------------------------------------------------------------
// string_to_lower() : Pretty self-explanatory, used for toggling sensor
// tracking on/off and any other function where strings need to be turned
// lowercase.
//------------------------------------------------------------------------------
void string_to_lower(string &str){
    char temp;
    for (char &ch : str){
	temp = ch;
	ch = tolower(temp);
    }
}




//------------------------------------------------------------------------------
// toggle_sensor_tracking(): Allows user to enable/disable tracking
// before using the pthreads.
//------------------------------------------------------------------------------
inline std::string get_tracking_status(){
    return (tracking_status)? "Enabled" : "Disabled";
}

void toggle_sensor_tracking(const Sensor_Date &s_d) {
    cout << options[0] << endl;
	string time = s_d.get_user_time();
    cout << "Current Sensor Status: " << get_tracking_status() << endl;
    cout << "Daily reports will be sent every day at "
    	 << time << " (" << twelve_hour_clock(time) <<")" << endl << endl;

    cout << "Input \"Enable\" to enable Sensor Checking."
	 << endl
	 << "Input \"Disable\" to disable Sensor Checking."
	 << endl
	 << "To return to the main menu, input \"back\"."
	 << endl
	 << "Enter any letter to continue."
	 << endl;

	string input;
    cin >> input;
    string_to_lower(input);

    if (input == "back")
    	return_to_menu();
    else if (input == "enable"){
		if (tracking_status)
	    	cerr << "Tracking is already enabled.\n";
		else
	    	tracking_status = true;
    }
    else if (input == "disable"){ 
		if (!tracking_status)
	    	cerr << "Tracking is already disabled.\n";
		else
	    	tracking_status = false;
    }
    else
		cerr << "Continuing...";

}

//------------------------------------------------------------------------------
// check_pthread_creation(): Quick inline function to determine
// if a pthread was created properly.
//------------------------------------------------------------------------------



inline void check_pthread_creation(int &return_val, string &error_msg){
    if (return_val)
		throw runtime_error(error_msg);
	else
		return;
}


//------------------------------------------------------------------------------
// handle_sensor_thread() : Sensor pthread will execute this until
// the sensors can't be read from.
//------------------------------------------------------------------------------
void* handle_sensor_thread(void *temp_log){
    // Now convert the temp_log back to a Log
    Log *log = static_cast<Log*>(temp_log);
    // Block access to mutex
    string error_message;
    bool check_rain_sensor;
    int check_float_sensor;
    bool check_all;

    std::mt19937 merse;
    std::uniform_real_distribution<double> rain_values;
    initialize_rain_sensor(merse, rain_values);

    
    do {
	check_rain_sensor = check_rain_connection();
	check_float_sensor = get_float_sensor_readings();
	check_all = check_rain_sensor && check_float_sensor;


	// Read anyway.
	log->sensor_check["Rain Sensor"] = check_rain_sensor;
	log->sensor_check["Float Sensor"] = static_cast<bool>(check_float_sensor);
	if (check_all){
	    // Read the temperature values
	    log->level["rain_level"] = get_rain_sensor_readings(merse,
								rain_values);
	    pthread_mutex_lock(&log_mutex);
	    project_log = *log;
	    pthread_mutex_unlock(&log_mutex);
	    
	    if (log->level["rain_level"] >= rain_limit){
		log->comment = "<strong>Rain levels have exceeded the maximum limit. "
		    "Please contact a technician to inspect the device.</strong>";
		error_message = "<strong>Exceeded Rain Levels</strong>";
		cerr << "Current rain level has exceeded the rain limit of "
		     << rain_limit
		     << endl;
		
		break;
	    }
	    

	    
	    
	}
	else { 
	    // Can't read values from rain_sensor, so stop the program
	    // and set a comment detailing the problem.
	    log->comment = "<strong>Critical Error: Cannot read from the rain sensor. "
		"Please send an technician to readjust the system.</strong>";
	    error_message = "<strong>CRITICAL ERROR!</strong>";
	    
	    cerr << "Cannot read from the rain sensor."
		 << "I will send an email detailing this problem to"
		 << smtp_receiver_address
		 << ". This has also been added to the database."
		 << endl;
	    
	    break;
		
	}
	
	// If another function needs to disable tracking (For example, Test_Sensors),
	// do so.
	if (end_all_threads)
		break;


    } while (true);


    // Repeat of above:
    if (end_all_threads){
    	cerr << "Disabling Tracking.." << endl;
    	return nullptr;
    }
    // Update project log:

	ostringstream os;
    // Note this in the database.
    pthread_mutex_lock(&log_mutex); // Set up Mutex
    // Populate Log with date and time:

	std::time_t time_stamp = std::time(nullptr);
	os << std::put_time(std::localtime(&time_stamp), "%c");
	log->time_stamp = os.str();

	os.str("");
	os << std::put_time(std::localtime(&time_stamp), "%B %d %Y");
	log->date = os.str();

	// Now update project_log
    project_log = *log;

    // Send to database.
    open_connection(db_connect);
    pqxx::work transaction{db_connect};
    // Set the time:

    add_log(transaction, *log);
    close_connection(db_connect);
    // Now send an email and close the thread.:
    send_log_as_HTML(project_log, error_message);
	// Also cancel the other thread.
    end_all_threads = true;
    pthread_mutex_unlock(&log_mutex); // Unlock Mutex



    return nullptr;
    
}

//------------------------------------------------------------------------------
// time_since_midnight(): Obnoxious function that returns a
// chrono::system_clock object with
// the amount of time since midnight(of any date). Used for sending email at
// the appropriate time. 
//------------------------------------------------------------------------------
std::chrono::system_clock::duration time_since_midnight() {
	auto now = std::chrono::system_clock::now();

	time_t tnow = std::chrono::system_clock::to_time_t(now);
	tm *date = std::localtime(&tnow);
	date->tm_hour = 0;
	date->tm_min = 0;
	date->tm_sec = 0;
	auto midnight = std::chrono::system_clock::from_time_t(std::mktime(date));

	return now-midnight;
}

//------------------------------------------------------------------------------
// send_email_thread(): Pthread function that handles sending email at the specified
// time 
//------------------------------------------------------------------------------
void* send_email_thread(void *s_d){
	Sensor_Date *sdate = static_cast<Sensor_Date*>(s_d);

	string message = "Daily Report.";
	time_t current_time;
	uint64_t check;
	ostringstream os;
	// Place mutex here
	pthread_mutex_lock(&log_mutex);
	open_connection(db_connect);
	pthread_mutex_unlock(&log_mutex);
	//end mutex


	// First, get the current time.
	// If the clock % Seconds ==
	while (true){
		// Convert all the dates
		current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		check = current_time % MAX_SECONDS_IN_DAY;

		if (check == sdate->get_email_time().count()){
			// First lock the resources
			pthread_mutex_lock(&log_mutex);
			open_connection(db_connect);
			pqxx::work transaction{db_connect};

			// Add date and timestamp to log.

			// Now use current_time again.
			current_time = std::time(nullptr);
			os << std::put_time(std::localtime(&current_time), "%c");
			project_log.time_stamp = os.str();

			os.str("");
			os << std::put_time(std::localtime(&current_time), "%B %d %Y");
			project_log.date = os.str();
			project_log.comment = "This is the daily report generated at ";
			project_log.comment += sdate->get_user_time();
			// Update database
			add_log(transaction, project_log);
			// Now send email
			send_log_as_HTML(project_log, message);
			// Unlock the resources.
			pthread_mutex_unlock(&log_mutex);
		}
		// If the other thread is closed, close this one too.
		if (end_all_threads)
			break;

	}

    

	return nullptr;


}

//------------------------------------------------------------------------------
// Tracking() : Enables / Disable Sensor Tracking
// When Tracking is enabled, Make a pthread that handles reading the values
// Of the Float Sensor/Rain Sensor. When an error occurs or when it is time
// To send a daily report, send an email.
// When Tracking is disabled, close the pthread.
// TODO: Allow both threads to run in the background.
//------------------------------------------------------------------------------
void sensor_tracking(void){
	Sensor_Date sd;
	read_user_time(sd);

	toggle_sensor_tracking(sd);
    
    if (!tracking_status){
	cerr << "Tracking is currently disabled. Return back to the"
	     << " main menu.\n";
	return_to_menu();

    }

	//Initalize the mutex first
	int mutex_check = pthread_mutex_init(&log_mutex, nullptr);

    string error_msg = "Could not initalize the mutex for some reason.";
    check_pthread_creation(mutex_check, error_msg);

    Log temp_log;
    int pthread_check;
    pthread_check = pthread_create(&sensor, nullptr,
    				   handle_sensor_thread,
    				   static_cast<void*>(&temp_log));



    error_msg = "Could not create pthread for sensor tracking.";
    
    check_pthread_creation(pthread_check, error_msg);
    
    // Now create the pthread for email sending.

    pthread_check = pthread_create(&email, nullptr,
								   send_email_thread,
								   static_cast<void *>(&sd));
    
    error_msg = "Could not create a pthread for sending email.";
    check_pthread_creation(pthread_check, error_msg);


    //Now join them at the end.
    pthread_join(email, nullptr);
	pthread_join(sensor, nullptr);

    mutex_check = pthread_mutex_destroy(&log_mutex);
    error_msg = "Could not destroy the mutex for some reason.";
    check_pthread_creation(mutex_check, error_msg);
    return_to_menu();
}
