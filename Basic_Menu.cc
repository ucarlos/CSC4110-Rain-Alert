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

void menu(void);
// Static Functions:
inline void return_to_menu(void){
    void (*function_pointer) (void) = menu;
    function_pointer();
}


// Project_File for everything:
std::unique_ptr<settings_file> project_file(new settings_file{xml_path});
//pqxx::connection db_connect;
vector<string> options = {"Enable/Disable Tracking",
								 "Status",
								 "Search Logs",
								 "Test Sensors",
								 "Database Options",
								 "Email Options",
								 "Quit"};

Log project_log{};
//------------------------------------------------------------------------------
// show_status(): Show the current values of the sensors (Enabled /Disabled)
// alongside the current rain/battery levels.
//------------------------------------------------------------------------------
void show_status(void){
    cout << options[1] << endl;
    char ch;

	ofstream ofs{log_status_path, ios_base::trunc};
	ofs << "You are in \"less\" mode. In order to escape, press q" << endl;
    ofs << "Current Status: " << endl;

	//pthread_mutex_t temp_mutex = PTHREAD_MUTEX_INITIALIZER;
	//pthread_mutex_lock(&temp_mutex);
	ofs << project_log << endl;
	//pthread_mutex_unlock(&temp_mutex);

	string sys_call = "less ";
	sys_call = sys_call + " " + log_status_path;
	system(sys_call.c_str());

    return_to_menu();
}

//------------------------------------------------------------------------------
// search_logs(): Given a date and time, search the database for that log.
// If found, output the result to a file/stdout. Otherwise, display "Not Found."
//
//------------------------------------------------------------------------------
void search_logs(void){
    cout << options[2] << endl;
    string input;

    // cout << "If you want to go back to the main menu, input \"back\" ." << endl;
    // cin >> input;
    // string_to_lower(input);
    // if (input == "back")
    // 	return_to_menu();

	cout << "Please enter a date (mm/dd/yyyy) and time (24 hour clock hh:mm) : " << endl;
	string date, time;
	cin >> date >> time;
	bool check;

	while (!(check = verify_date(date))){
		cout << "Invalid date. Remember to input a date in mm/dd/yyyy format.\n";
		cin >> date;
	}

	while (!(check = verify_time(time))){
		cout << "Invalid time. Remember to input time in hh:mm (24 hour clock).\n";
		cin >> time;
	}
	// Now send the query.
	cout << "Sending Query..." << endl;
	open_connection(db_connect);
	int64_t start = return_time_in_seconds(time) + 60;
	string end = string_to_seconds(start);
	pqxx::result query = search_database(db_connect, date, time, end);
	show_result_contents(query);
	close_connection(db_connect);
    return_to_menu();
}

//------------------------------------------------------------------------------
// test_sensors() : Test all sensors to see if they're functional. If not,
// Disable tracking and send an error log.
//------------------------------------------------------------------------------
void test_sensors(void){

#ifdef SENSOR_READINGS_RNG
    cerr << "This project currently uses a random number generator "
    	 << "to simulate sensor output.\nThis is intended to be used "
    	 << "as a last resort or for debugging purposes.\nI apologize for the inconvenience.\n" << endl;
#else
	cout << options[3] << endl;
#endif
    cout << "Press any key to continue." << endl;
    char ch;
    cin >> ch;
    return_to_menu();
}

//------------------------------------------------------------------------------
// database_options(): Function handles the current log information and
// a connection test to the database.
//------------------------------------------------------------------------------
void database_options(void){
    cout << options[4] << endl;
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
// email_options() : Handles what email address the log is sent to, as well
// as the type of email sent (HTML or Basic Text)
//------------------------------------------------------------------------------
void email_options(void){
    cout << options[5] << endl;
    char input;
    cout << "Would you like to change the recipient email? [y/n]" << endl;
    cin >> input;

    if (input == 'n')
		return_to_menu();
	cout << "Enter the new recipient email: ";
    string new_recipient_email;
    std::regex test;
	cin >> new_recipient_email;

    while (!verify_username(test, new_recipient_email)){
    	cout << "Invalid email. Username should follow name@domain_name.domain." << endl;
    	cin >> new_recipient_email;
    }

    // If valid, change email.
    // Place mutex here.
	pthread_mutex_t temp_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&temp_mutex);

    project_file->get_smtp_info().at("receiver_email") = new_recipient_email;
    project_file->save_file(xml_path);

    pthread_mutex_unlock(&temp_mutex);

    cout << "Setting saved in " << xml_path << endl
    	 << "For now, restart the program." << endl;
    sleep(2);
    // End mutex here.
    return_to_menu();
}


std::string version_info(void){
    ostringstream os;
    os << "CSC 4110 Project Menu (Version "
       << CSC4110_Project_VERSION_MAJOR
       << "."
       << CSC4110_Project_VERSION_MINOR << ")" << endl;

    return os.str();
}

void info(){
    cout << "This entire menu screen is an temporary screen in order"
	 << "to provide some form that can be worked with." << endl;
    
    cout << "In the future, I plan on making this menu in ncurses, "
	 << "which each option fully functional.\n";


}

void list_menu(void){

    for (int i = 0;  i < options.size() - 1; i++){
	cout << static_cast<char>(('a' + i)) << ") "
	     << options[i] << endl;
	
    }
    cout << "q) Quit" << endl;
}

void menu(void){
    cout << version_info() << endl;
    list_menu();
    // Function Pointer to each option.
    void (*function_pointer)(void);

    cout << endl;
    cout << "Please choose a selection. To quit, press \"q\"."
	 	 << endl;

    char input;
    cin >> input;

    char max = static_cast<char>('a' + options.size() - 1);
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
	        function_pointer = email_options;
	        break;
        default:
	        exit(EXIT_SUCCESS);
    }
    
    // Clear screen and go to the function:
    system("clear");
    function_pointer();
     
}

//------------------------------------------------------------------------------
// get_credentials(): Function to get all the variables needed for the
// project. Depending on whether boost is installed, either the credentials
// will be read from a xml file or various txt files.
//------------------------------------------------------------------------------

void get_credentials(void){
	if (can_use_boost){
		get_smtp_info_from_xml(project_file->get_smtp_info());
		get_database_info_from_xml(project_file->get_database_info());
	}
	else {
		get_smtp_credentials();
		get_database_info_from_file();
	}
}

int main(void){
    // Get the credentials for the SMTP and the database first.
	get_credentials();
    menu();
}
