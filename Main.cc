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

// Menu options
vector<string> options = {"Enable/Disable Tracking",
			  "Status",
			  "Search Logs",
			  "Test Sensors",
			  "Database Options",
			  "Email Options",
			  "Quit"};

// Global Log File
Log project_log{};


//------------------------------------------------------------------------------
// Function Definitions
//------------------------------------------------------------------------------



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
    // First, retrieve the credentials:
    get_credentials();
    menu();
}
