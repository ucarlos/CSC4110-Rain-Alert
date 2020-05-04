/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/29/2020 at 01:20 AM
 * 
 * Dialog_Menu.cc
 * 
 * This is a Dialog Based UI for the Rain Alert Project
 * (CSC 4110 Spring Semester 2020)
 * -----------------------------------------------------------------------------
 */

#include "./Project.h"
#include <newt.h>
const int box_width = 60;
const int box_height = 16;

const vector<void (*)(void)>menu_pointers = {&sensor_tracking,
					     &show_status,
					     &search_logs,
					     &test_sensors,
					     &database_options,
					     &email_options};



void sensor_tracking(void){
	newtComponent form, window, n_list;
	newtCls();

	cout << options[0] << endl;
}

void show_status(void){
    cout << options[1] << endl;

}

void search_logs(void){
    cout << options[2] << endl;

}

void test_sensors(void){
    cout << options[3] << endl;

}

void database_options(void){
    cout << options[4] << endl;

}


void email_options(void){
    cout << options[5] << endl;

}

void menu(void){
    // Check this first.
    if (options.size() != menu_pointers.size())
	throw runtime_error("Error: Both the options and menu_pointer "
					 "vectors have unequal lengths. Check them again.");
    
    newtComponent form, window, n_list;

    newtCls();

    newtDrawRootText(0, 0, version_info().c_str());
    newtCenteredWindow(box_width, box_height, "Main Menu");

    // Create a listbox containing the appropriate options.
    form = newtForm(nullptr, nullptr, 0);
    window = newtListbox(box_width / 4, 2, 10, 1);
    

    newtListboxSetWidth(window, 30);
    
    // Set the options:
    for (int i = 0; i < menu_pointers.size(); i++)
	newtListboxAppendEntry(window, options[i].c_str(),
			       reinterpret_cast<void *>((menu_pointers[i])));

    
    // Now add all the components and run them.
    newtFormAddComponents(form, window, nullptr);
    newtRunForm(form);

    // Get the pointer and 
    auto option_point =
    		reinterpret_cast<void (*) ()>(newtListboxGetCurrent(window));
	// Now call the function:
	option_point();
    // Destroy the form.
    newtFormDestroy(form);



	
}
