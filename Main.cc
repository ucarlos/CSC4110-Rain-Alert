/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/30/2020 at 04:50 PM
 * 
 * Main.cc
 * 
 * Regardless of what menu is used (Basic Menu or Dialog)
 * the appropriate function will be called here.
 * Also defined here are the needed function/ variables that are common
 * in both versions.
 * -----------------------------------------------------------------------------
 */

#include "./Project.h"


//------------------------------------------------------------------------------
// Definition of external variables declared in Project.h
//------------------------------------------------------------------------------

// Settings file used throughout the program
std::unique_ptr<settings_file> project_file(new settings_file{xml_path});

// Menu main_menu_options
const vector<string> main_menu_options = {"Sensor Tracking",
					  "Sensor Status",
					  "Search Logs",
					  "Test Sensors",
					  "Database Information",
					  "Email Options"};

// Global Log File
Log project_log{};

const vector<string> email_menu_options = {"Change Recipient Mail", "Change Time",
					   "Change Timezone", "Change Email Type"};



const vector<string> test_sensor_menu_options = {"Test Email Sending",
												 "Test Database Connection",
												 "Test Thread Creation"};
					    

//------------------------------------------------------------------------------
// Function Definitions
//------------------------------------------------------------------------------

std::string version_info(void){
    ostringstream os;
    os << "Rain Alert (Version "
       << CSC4110_Project_VERSION_MAJOR
       << "."
       << CSC4110_Project_VERSION_MINOR << ")" << endl;

    return os.str();
}



//------------------------------------------------------------------------------
// get_credentials(): Function to get all the variables needed for the
// project. Depending on whether boost is installed, either the credentials
// will be read from a xml file or various txt files.
//------------------------------------------------------------------------------

void get_credentials(void){
    // if (can_use_boost){
    // 	get_smtp_info_from_xml(project_file->get_smtp_info());
    // 	get_database_info_from_xml(project_file->get_database_info());
    // }
    // else {
    // 	get_smtp_credentials();
    // 	get_database_info_from_file();
    // }

    // Just use Boost
    get_smtp_info_from_xml(project_file->get_smtp_info());
    get_database_info_from_xml(project_file->get_database_info());
}


int main(void){
    // First, retrieve the credentials and then call menu.
    get_credentials();
    menu();
}
