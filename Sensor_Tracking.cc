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

void toggle_sensor_tracking(void) {
    cout << options[0] << endl;

    cout << "Current Sensor Status: " << get_tracking_status() << endl;
    
    cout << "Input \"Enable\" to enable Sensor Checking."
	 << endl
	 << "Input \"Disable\" to disable Sensor Checking."
	 << endl
	 << "To continue, enter anything else."
	 << endl;

	string input;
    cin >> input;
    string_to_lower(input);
    
    if (input == "enable"){
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
}


//------------------------------------------------------------------------------
// handle_sensor_thread() : Sensor pthread will execute this until
// the sensors can't be read from.
//------------------------------------------------------------------------------
void* handle_sensor_thread(void *temp_log){
    // Now convert the temp_log back to a Log
    Log *log = static_cast<Log*>(temp_log);
    // Block access to mutex
    
    bool check_rain_sensor;
    int check_float_sensor;
    bool check_all;

    std::mt19937 merse;
    std::uniform_real_distribution<double> rain_values;
    initialize_rain_sensor(merse, rain_values);
    int stop = 1;
    do {
	check_rain_sensor = check_rain_connection();
	check_float_sensor = get_float_sensor_readings();
	check_all = check_rain_sensor && check_float_sensor;


	// Read anyway.
	log->sensor_check["Rain Sensor"] = check_rain_sensor;
	log->sensor_check["Float Sensor"] = static_cast<bool>(check_float_sensor);
	if (check_all && stop < 1000){
	    // Read the temperature values
	    log->level["rain_level"] = get_rain_sensor_readings(merse,
								rain_values);
	    project_log = *log;
	    stop++;
	}
	else { 
	    // Can't read values from rain_sensor, so stop the program
	    // and set a comment detailing the problem.
	    log->comment = "<strong>Critical Error: Cannot read from the rain sensor. "
					"Please send an technician to readjust the system.</strong>";
	    project_log = *log;
	    break;
		
	}
	

    } while (true);

    cerr << "Cannot read from the rain sensor."
	 << "I will send an email detailing this problem to"
	 << smtp_receiver_address
	 << ". This has also been added to the database."
	 << endl;
    
    // Note this in the database.
    open_connection(db_connect);
    pqxx::work transaction{db_connect};
    add_log(transaction, *log);
    close_connection(db_connect);

    // Now send an email:
    string error_message = "<strong>CRITICAL ERROR!</strong>";
    send_log_as_HTML(project_log, error_message);
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
    toggle_sensor_tracking();
    
    if (!tracking_status){
	cerr << "Tracking is currently disabled. Return back to the"
	     << " main menu.\n";
	return_to_menu();

    }


    Log temp_log;
    int pthread_check;
    pthread_check = pthread_create(&sensor, nullptr,
				   handle_sensor_thread,
				   static_cast<void*>(&temp_log));



    string error_msg = "Could not create pthread for sensor tracking.";
    
    check_pthread_creation(pthread_check, error_msg);
    
    // Now create the pthread for email sending.
    pthread_join(sensor, nullptr);
}
