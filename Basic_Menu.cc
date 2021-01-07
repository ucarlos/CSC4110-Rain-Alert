/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/16/2020 at 03:55 PM
 * 
 * Basic_Menu.cc
 * 
 * This program is a simple menu to be used with the project. It should provide
 * the necessary functionality, but ideally, I would like to have this as a
 * menu of last resort if I don't get the dialog menu completed in time.
 * -----------------------------------------------------------------------------
 */

#include "./Project.h"
using namespace std;

void toggle_sensor_tracking(const Sensor_Date &s_d);
void list_menu(const vector<string> &v);

/**
 * @brief return_to_menu(): Inline function to "return" to menu.
 * @param void
 */
inline void return_to_menu(void){
	void (*function_pointer) () = menu;
	system("clear");
	function_pointer();
}



/**
 * @brief list_menu(): Display the items in vector<string> as an option.
 * @param v vector of strings that contain menu options
 */
void list_menu(const vector<string> & v){
    for (int i = 0;  i < v.size(); i++){
		cout << static_cast<char>(('a' + i)) << ") "
			 << v[i] << endl;
	
    }

}


/**
 * \brief A prompt called before sensor_tracking is called to enable or disable
 * sensor tracking for the program.
 * @param s_d Sensor_Date object containing the time when emails are sent.
 */
void toggle_sensor_tracking(const Sensor_Date &s_d) {
    cout << main_menu_options[0] << endl;
    string time = s_d.get_user_time();
    cout << "Current Sensor Status: " << project_file->get_tracking_status() << endl;
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
		if (project_file->get_tracking_status())
			cerr << "Tracking is already enabled.\n";
		else
			project_file->set_tracking_status(true);
    }
    else if (input == "disable"){
		if (!project_file->get_tracking_status())
			cerr << "Tracking is already disabled.\n";
		else
			project_file->set_tracking_status(false);
    }
    else
		cerr << "Continuing...";

}



/**
 * \brief Enables or disables sensor tracking
 * When Tracking is enabled, a pthread is created that handles reading values from
 * the rain sensor. Daily sensor reports are sent at the time specified by the
 * sensor date. Errors that occur are also logged and sent as a Error email.
 * When Tracking is disabled, pthread_join is called, ending the thread.
 * TODO: Allow both threads to run in the background.
 * @param void
 * @returns void
 */
void sensor_tracking(void){
    Sensor_Date sd;
    read_user_time(sd);
    
    toggle_sensor_tracking(sd);

    if (!project_file->get_tracking_status()){
		cerr << "Tracking is currently disabled. Returning back to the"
			 << " main menu.\n";
		return_to_menu();
	
    }

    //Initalize the mutex first
    int mutex_check = pthread_mutex_init(&log_mutex, nullptr);

    string error_msg = "Could not initialize the mutex for some reason.";
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

//------------------------------------------------------------------------------
// show_status(): Show the current values of the sensors (Enabled /Disabled)
// alongside the current rain/battery levels.
//------------------------------------------------------------------------------
void show_status(void){
    cout << main_menu_options[1] << endl;
    char ch;

    ofstream ofs{log_status_path, ios_base::trunc};
    ofs << "You are in \"less\" mode. In order to escape, press q" << endl;
    ofs << "Current Status: " << endl;
    
    pthread_mutex_t temp_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&temp_mutex);
    ofs << project_log << endl;
    pthread_mutex_unlock(&temp_mutex);

    string sys_call = "less ";
    sys_call = sys_call + " " + log_status_path;
    system(sys_call.c_str());

    return_to_menu();
}

//------------------------------------------------------------------------------
// search_logs(): Given a date (mm/dd/yyyy), search the database for that log.
// If found, output the result to a file/stdout. Otherwise, display "Not Found."
//
//------------------------------------------------------------------------------
/**
 * Given a date in (mm/dd/yyyy) format, search the database for any logs.
 * If found, output the result to file or display it to stdout. Otherwise,
 * display "Not Found."
 * @param void
 * @returns void
 */
void search_logs(void){
    cout << main_menu_options[2] << endl;
    string input;
    

    cout << "Please enter a date (mm/dd/yyyy):" << endl;
    string date;
    cin >> date;
    bool check;

    while (!(check = verify_date(date))){
		cout << "Invalid date. Remember to input a date in "
			 << "mm/dd/yyyy format and make sure that it is a valid date "
			 << "(i.e no Feb 29 on an non leap year).\n";
		cin >> date;
    }
	
    // Now send the query.
    cout << "Sending Query..." << endl;
    open_connection(db_connect);

    pqxx::result query = search_database(db_connect, date);
    show_result_contents(query);
    close_connection(db_connect);
    return_to_menu();
}

/**
 * Allows the user to test the project components. This includes:
 *     - Testing whether the SMTP server can send an test email
 *     - Connecting to the database and retrieving a query
 *     - Testing whether two pthreads can be made.
 * @param void
 * @returns void
 */
void test_sensors(void){
#ifdef SENSOR_READINGS_RNG
    cerr << "This project currently uses a random number generator "
    	 << "to simulate sensor output.\nThis is intended to be used "
    	 << "as a last resort or for debugging purposes.\nI apologize "
		 << "for the inconvenience.\n" << endl;
#endif
    
    cout << main_menu_options[3] << endl;
    list_menu(test_sensor_menu_options);
    cout << "\n";
    cout << "r) Return to Main Menu" << endl;
    cout << "Please choose a selection." << endl;
    
    // Options should be a, b, c, r, and default:

    char input;
    
    //cin >> input;
    while ((cin >> input && input != 'r')){
		switch (input){
		case 'a': // Test Email Sending:
			test_smtp();
			break;
		case 'b': // Test Database Connection:
			test_connection();
			break;
		case 'c': // Test Thread creation:
			test_pthread();
			break;
		case 'r': // Return
			break;
		default:
			cout << "Invalid Selection. Try again." << endl;
		}
	
		system("clear");	
		cout << main_menu_options[3] << endl;
		list_menu(test_sensor_menu_options);
		cout << "\n";
		cout << "r) Return to Main Menu" << endl;
		cout << "Please choose a selection." << endl;
    
    }

    system("clear");
    return_to_menu();

    
    
}

/**
 * Option function that displays current log and database information.
 * @param void
 * @returns void
 */
void database_options(){
    cout << main_menu_options[4] << endl;
    // Lock project_file.
	pthread_mutex_t temp_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&temp_mutex);

    cout << "You are currently connected to the database "
    	 << project_file->get_database_info().at("psql_database")
    	 << " on " << project_file->get_database_info().at("psql_ip")
    	 << endl;

	pthread_mutex_unlock(&temp_mutex);
	// Unlock project_file.
    cout << "Press any key to continue." << endl;
    char input;
    cin >> input;
    return_to_menu();
}

//------------------------------------------------------------------------------
// email_menu_options() : Handles what email address the log is sent to, as well
// as the type of email sent (HTML or Basic Text)
//------------------------------------------------------------------------------

/**
 * Allows the user to alter the email address logs are sent to, as well as the
 * type of email sent (HTML or Basic Text)
 * @param void
 * @returns void
 */
void email_settings(){
	cout << version_info() << endl;

    char input;
    string str;
    std::regex regex_test;
    pthread_mutex_t temp_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&temp_mutex);
    int32_t val;
 
    do {
		cout << main_menu_options[5] << endl;
		list_menu(email_menu_options);
		cout << "r) Return to main menu" << endl;
		cout << "\n";
		cout << "Please choose an option." << endl;
		cin >> input;
		switch(input){
		case 'a': // Change Recipient Email
			cout << "Current email: " 
				 << project_file->get_smtp_info().at("receiver_email")
				 << endl;
	    
			cout << "Enter a new recipient email:" << endl;
			cin >> str;

			while (!verify_username(regex_test, str)){
				cout << "Invalid address. "
					 << "Username should follow "
					 << "name@domain_name.domain." << endl;
		
				cin >> str;
			}

			// Otherwise set the recipient email
			project_file->get_smtp_info().at("receiver_email") = str;
			break;
	    
		case 'b': // Change Email Time

			cout << "A Daily report will be sent at "
				 << project_file->get_email_time()
				 << " ("
				 << twelve_hour_clock(project_file->get_email_time())
				 << ")"
				 << endl;
	    		    
			cout << "Enter a new time to send the email:" << endl;
			cin >> str;

			while (!verify_time(str)){
				cout << "Invalid time. Time should be in hh::mm"
					 << " format and be in the range "
					 << "00:00 to 23:59" << endl;
				cin >> str;
			}

			// Otherwise set email time
			project_file->set_email_time(str);
			break;
		case 'c': // Change Timezone
			cout << "Current Timezone: UTC "
				 << project_file->get_time_zone()
				 << endl;
	    
			cout << "Enter the timezone this device is on. Time zone should be in "
				"the range [-12: 12]." << endl;
			cin >> val;

			while (!verify_time_zone(val)){
				cout << "Invalid time zone. Time zone should "
					 << "be in the range [-12: 12]."
					 << endl;
				cin >> val;
			}
	   
			// Otherwise set the time_zone
			project_file->set_time_zone(val);
			break;
		case 'd': // Change Email Type
			cout << "Enter the email type that is sent [HTML or Text]" << endl;
			cin >> str;
			string_to_lower(str);
	    
			if (str == "html")
				project_file->set_email_type(0);
			else if (str == "text")
				project_file->set_email_type(1);
			else {
				cout << "Invalid input." << endl;
				break;
			}

			break;

		case 'r': // Return
			break;
		default:
			cout << "Invalid input. Try again." << endl;
		}

    } while (input != 'r');

    pthread_mutex_unlock(&temp_mutex);
    
    project_file->save_file(xml_path);
    return_to_menu();
}

/**
 * \brief Basic Text menu that displays all program options.
 * @param void
 * @returns void
 */
void menu(void){
    cout << version_info() << endl;
    list_menu(main_menu_options);
    // Quit Option
    cout << "q) Quit" << endl;
    // Function Pointer to each option.
    void (*function_pointer)(void);

    cout << endl;
    cout << "Please choose a selection. To quit, press \"q\"."
		 << endl;

    char input;
    cin >> input;

    char max = static_cast<char>('a' + main_menu_options.size() - 1);
    // Make sure that input is a valid selection.
    while (!('a' <= input && input <= max) && (input != 'q')){
		cout << "Invalid input. Try again." << endl;
		cin >> input;
    }

    switch(input){
	case 'a':
	    function_pointer = sensor_tracking;
	    break;
	case 'b':
	    function_pointer = show_status;
	    break;
	case 'c':
	    function_pointer = search_logs;
	    break;
	case 'd':
	    function_pointer = test_sensors;
	    break;
	case 'e':
	    function_pointer = database_options;
	    break;
	case 'f':
	    function_pointer = email_settings;
	    break;
	default:
	    exit(EXIT_SUCCESS);
    }
    
    // Clear screen and go to the function:
    system("clear");
    function_pointer();
     
}
