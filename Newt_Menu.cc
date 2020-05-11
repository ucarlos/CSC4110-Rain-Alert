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

// Box width and height should be divisible by 4.
// For example, 72x20 or 60x16 .
void main_menu(void);
const unsigned int box_width = 64;
const unsigned int box_height = 16;

// Since all buttons will be at the end,
// Only the x value has to be changed (Multiplied by 1, 2, or 3)
const unsigned int button_y = 7 * (box_height / 8);
const unsigned int button_x = (box_width / 8);



const vector<void (*)(void)>menu_pointers = {&sensor_tracking,
					     &show_status,
					     &search_logs,
					     &test_sensors,
					     &database_options,
					     &email_settings};


//Small inline function to quit program.
inline void exit_program(){
    newtFinished();
    exit(EXIT_SUCCESS);
}



void sensor_tracking(void){
    // Make sure to pop the last window:
    newtPopWindow();
    newtComponent form, window, n_list, back, cont;
    // Wait a moment to refresh screen
    newtCls();
    
    
    newtDrawRootText(0, 0, version_info().c_str());
    newtCenteredWindow(box_width, box_height, options[0].c_str());

    // Always create a new form for every screen.
    form = newtForm(nullptr, nullptr, 0);
    back = newtCompactButton(button_x, button_y, "Back");
    cont = newtCompactButton(6 * button_x, button_y, "Forward!");
    newtFormAddComponents(form, back, cont, nullptr);
    newtRunForm(form);
    newtFormDestroy(form);
    return_to_menu();

}

void show_status(void){
    // Make sure to stop newt for this section:
    newtFinished();

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

    //Reinitialize Newt and return to menu
    newtInit();

}

void search_logs(void){
    newtComponent form, window, n_list;
    // Wait a moment to refresh screen
    newtCls();


    newtDrawRootText(0, 0, version_info().c_str());
    newtCenteredWindow(box_width, box_height, options[2].c_str());

    // Always create a new form for every screen.
    form = newtForm(nullptr, nullptr, 0);

    newtRunForm(form);

}

void test_sensors(void){
    newtComponent form, window, n_list;
    // Wait a moment to refresh screen
    newtCls();
	
    newtDrawRootText(0, 0, version_info().c_str());
    newtCenteredWindow(box_width, box_height, options[3].c_str());

    // Always create a new form for every screen.
    form = newtForm(nullptr, nullptr, 0);
    newtRunForm(form);

}

void database_options(void){
    newtComponent form, window, n_list;
    // Wait a moment to refresh screen
    newtCls();
    	
    newtDrawRootText(0, 0, version_info().c_str());
    newtCenteredWindow(box_width, box_height, options[4].c_str());

    // Always create a new form for every screen.
    form = newtForm(nullptr, nullptr, 0);
    newtRunForm(form);

}


void email_settings(void){
    newtComponent form, window, n_list;
    // Wait a moment to refresh screen
    newtCls();

	
    newtDrawRootText(0, 0, version_info().c_str());
    newtCenteredWindow(box_width, box_height, options[5].c_str());

    // Always create a new form for every screen.
    form = newtForm(nullptr, nullptr, 0);
    newtRunForm(form);

}

void main_menu(void){
    // Check this first.
    
    if (options.size() != menu_pointers.size())
	throw runtime_error("Error: Both the options and menu_pointer "
					 "vectors have unequal lengths. Check them again.");
    
    newtComponent form, window, n_list, exit;

    newtCls();

    newtDrawRootText(0, 0, version_info().c_str());
    newtCenteredWindow(box_width, box_height, "Main Menu");

    // Create a listbox containing the appropriate options.
    form = newtForm(nullptr, nullptr, 0);
    // 1 is the mod.
//    window = newtListbox(box_width / 4, 2, 10, 1);
    window = newtListbox(box_width / 4, 2, box_width / 8, 1);

    newtListboxSetWidth(window, 30);
    
    // Set the options:
    std::string temp;
    for (int i = 0; i < menu_pointers.size(); i++) {
    	temp = static_cast<char>((i + 1) + '0');
		newtListboxAppendEntry(window, string{temp + " " + options[i]}.c_str(),
							   reinterpret_cast<void *>((menu_pointers[i])));
	}

    // Awful method to center last option and add a number to it:
    // I also didn't want to create another string.
    char exit_name[100];
    exit_name[0] = static_cast<char>(menu_pointers.size() + 1 + '0');
    exit_name[1] = ' ';
    strncpy(exit_name + 2, "Exit Program", 30);

    newtListboxAppendEntry(window, exit_name, reinterpret_cast<void *>(exit_program));


    // Now add all the components and run them.
    newtFormAddComponents(form, window, nullptr);
    newtRunForm(form);

	// First check if button is exit. if so, 
    auto option_point =
	reinterpret_cast<void (*) ()>(newtListboxGetCurrent(window));
    // Destroy the form.
    newtFormDestroy(form);
    // newtFinished();
    option_point();
	
}



inline void return_to_menu(void){
    void (*function_pointer) () = main_menu;
    // Pop the current window:
    newtPopWindow();
    function_pointer();
}


//------------------------------------------------------------------------------
// Menu(): This function initializes newt, and calls main_menu in order to
// prevent newt from begin initialized constantly. 
//------------------------------------------------------------------------------
void menu(void){
    newtInit();
    main_menu();
    newtFinished();
}
