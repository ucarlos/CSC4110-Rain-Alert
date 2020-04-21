/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/16/2020 at 03:55 PM
 * 
 * Basic_Menu.cc
 * 
 * This program is a simple menu to be used with the project.
 * -----------------------------------------------------------------------------
 */

#include "./Project.h"

void menu(void);
// Static Functions:
inline void return_to_menu(void){
    void (*function_pointer) (void) = menu;
    function_pointer();
}

const static vector<string> options = {"Enable/Disable Tracking",
				       "Status",
				       "Search Logs",
				       "Test Sensors",
				       "Database Options",
				       "Email Options",
				       "Quit"};   			      


// Menu Function definitions

//------------------------------------------------------------------------------
// Tracking() : Enables / Disable Sensor Tracking
// When Tracking is enabled, Make a pthread that handles reading the values
// Of the Float Sensor/Rain Sensor. When an error occurs or when it is time
// To send a daily report, send an email.
// When Tracking is disabled, close the pthread.
//------------------------------------------------------------------------------
void tracking(void) {
    cout << options[0] << endl;
    return_to_menu();
}

//------------------------------------------------------------------------------
// show_status(): Show the current values of the sensors (Enabled /Disabled)
// alongside the current rain/battery levels.
//------------------------------------------------------------------------------
void show_status(void){
    cout << options[1] << endl;
    return_to_menu();
}

//------------------------------------------------------------------------------
// search_logs(): Given a date and time, search the database for that log.
// If found, output the result to a file/stdout. Otherwise, display "Not Found."
//
//------------------------------------------------------------------------------
void search_logs(void){
    cout << options[2] << endl;
    return_to_menu();
}

//------------------------------------------------------------------------------
// test_sensors() : Test all sensors to see if they're functional. If not,
// Disable tracking and send an error log.
//------------------------------------------------------------------------------
void test_sensors(void){
    cout << options[3] << endl;
    return_to_menu();
}

//------------------------------------------------------------------------------
// database_options(): Function handles the current log information and
// a connection test to the database.
//------------------------------------------------------------------------------
void database_options(void){
    cout << options[4] << endl;
    return_to_menu();
}

//------------------------------------------------------------------------------
// email_options() : Handles what email address the log is sent to, as well
// as the type of email sent (HTML or Basic Text)
//------------------------------------------------------------------------------
void email_options(void){
    cout << options[5] << endl;
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
	        function_pointer = tracking;
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

int main(void){
    // Get the credentials for the SMTP and the database first.
    get_smtp_credentials();
    get_database_info_from_file();
    test_smtp();
    //menu();


}
