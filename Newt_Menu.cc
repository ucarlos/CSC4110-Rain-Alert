/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/29/2020 at 01:20 AM
 * 
 * Newt_Menu.cc
 * 
 * This is a Newt Based UI for the Rain Alert Project
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

inline int32_t center_string(const char *str){
	return (strlen(str) / 2);
}

void return_to_menu_button(newtComponent_struct *button, void *form){
    // Destroy the form
    // call return_to_menu
    auto it = reinterpret_cast<newtComponent>(form);
    newtFormDestroy(it);
    return_to_menu();
}

void sensor_tracking(void){
    // Make sure to pop the last window:
    newtPopWindow();
    newtPopHelpLine();
    newtComponent form, window, n_list, back, cont;
    // Wait a moment to refresh screen
    newtCls();
    
    
    newtDrawRootText(0, 0, version_info().c_str());
    newtCenteredWindow(box_width, box_height, main_menu_options[0].c_str());

    // Always create a new form for every screen.
    form = newtForm(nullptr, nullptr, 0);
    back = newtCompactButton(button_x, button_y, "Back");
    cont = newtCompactButton(6 * button_x, button_y, "Forward!");
    newtFormAddComponents(form, back, cont, nullptr);


    // What do I want to do?
    // First, show when the email will be sent.
    // Next, allow the user to return or enable/disable tracking
    // 
    newtRunForm(form);
    newtFormDestroy(form);
    return_to_menu();

}

void show_status(void){
    // Make sure to stop newt for this section:
    newtSuspend();

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

    //Reinitialize Newt and return to menu
    newtResume();
    return_to_menu();

}

void search_logs(void){
    // Wait a moment to refresh screen
    
    // I will never sin again father
    confession:
	
    newtCls();
    newtPushHelpLine("Search the Database for a given date.");
    newtDrawRootText(0, 0, version_info().c_str());
    newtCenteredWindow(box_width, box_height, main_menu_options[2].c_str());
    // Contains the message.

    std::unique_ptr<char[]> message (new char [100]);
    newtComponent form, window, n_list, log_message_label, log_result, error_window;
    newtComponent back_but;
    
    const string log_message = "Please enter a date in mm/dd/yyyy format.";
    log_message_label = newtLabel(((box_width / 2) - (log_message.size() / 2)),
    		0, log_message.c_str());

    const char *button_text = "Return to Main Menu";
    
    back_but = newtCompactButton(((box_width / 2) - ((strlen(button_text) + 2) / 2) - 1),
    		button_y, button_text);

    // Form
    uint8_t search_len = 10;
    log_result = newtEntry(((box_width / 2) - (search_len / 2)),
			   (box_height/4),
			   "mm/dd/yyyy", search_len,
			   reinterpret_cast<const char **>(message.get()),
			   NEWT_FLAG_RETURNEXIT);
    


    // Always create a new form for every screen.
    form = newtForm(nullptr, nullptr, 0);


    
    newtFormAddComponents(form, log_message_label, back_but,log_result, nullptr);
    newtRunForm(form);

    // Make sure back button returns to main menu:
    newtCallback ret = &return_to_menu_button;
    newtComponentAddCallback(back_but, ret, form);
    std::string result = newtEntryGetValue(log_result);

    // Suspend newt() and Make a query.
    string error;
	newtSuspend();
    while (!verify_date(result)){

	newtResume();
	newtCls();
	newtCenteredWindow(box_width, box_height /2, "Error");
	newtComponent error_form = newtForm(nullptr, nullptr, 0);
	string error_message = "Invalid Date. Make sure that "
						"the date is in mm/dd/yyyy format.";
	newtComponent error_label = newtLabel(((box_width / 2) - (error_message.size() / 2)),
										  0, error_message.c_str());
	const char *ok = "Ok";
	newtComponent error_button =  newtCompactButton(((box_width / 2) - ((strlen(ok) + 2) / 2) - 1),
															(box_height / 4), ok);
	newtFormAddComponents(error_form, error_label, error_button, nullptr);
	newtDrawRootText(0, 0, version_info().c_str());
	newtPushHelpLine("Forgive me father for I have sinned.");
	newtRunForm(error_form);
	newtFormDestroy(form);
	newtPopWindow();

	// Repent
	goto confession;
    }

    // Now make the call.
    open_connection(db_connect);
    
    pqxx::result query = search_database(db_connect, result);
    show_result_contents(query);
    close_connection(db_connect);
    
    system("clear");
    newtResume();
    
    newtFormDestroy(form);
    return_to_menu();

}

void test_sensors(){
    newtSuspend();
#ifdef SENSOR_READINGS_RNG
    cerr << "This project currently uses a random number generator "
    	 << "to simulate sensor output.\nThis is intended to be used "
    	 << "as a last resort or for debugging purposes.\nI apologize "
	 << "for the inconvenience.\n" << endl;
#else
    cout << main_menu_options[3] << endl;
#endif
    cout << "Press any key to continue." << endl;
    char ch;
    cin >> ch;

    newtResume();
    
    void (*function_pointer) () = main_menu;
    function_pointer();

}

void database_options(){
    newtComponent form, window, label;
    // Wait a moment to refresh screen
    newtCls();
    	
    newtDrawRootText(0, 0, version_info().c_str());
    newtPushHelpLine("Shows database information");
    newtCenteredWindow(box_width, box_height, "Database Information");

    // Lock Project File:
    pthread_mutex_t temp_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&temp_mutex);
    ostringstream os;
    
     os  << "You are currently connected to the database "
    	 << project_file->get_database_info().at("psql_database")
    	 << " on " << project_file->get_database_info().at("psql_ip")
    	 << endl;
     
    pthread_mutex_unlock(&temp_mutex);
    label = newtTextbox(0, 0, box_width - 1,
    		box_height / 4, NEWT_FLAG_WRAP);
    newtTextboxSetText(label, os.str().c_str());
    // Always create a new form for every screen.
    const char *ok = "Ok";
    newtComponent button = newtCompactButton(((box_width / 2) -
    		((strlen(ok) + 2) / 2) - 1), (box_height / 4), ok);
    form = newtForm(nullptr, nullptr, 0);

    newtFormAddComponents(form, label, button, nullptr);
    newtRunForm(form);
    newtFormDestroy(form);
    
    return_to_menu();
}

void email_selection(const int &option){
	newtCls();
	newtSuspend();
	string str;
	std::regex regex_test;

	pthread_mutex_t temp_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&temp_mutex);
	int select = (option + 'a');
	int8_t val;

	switch(select){
	case 'a': // Change Recipient Email
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

	default:
		newtResume();
	    return_to_menu();
	}


	pthread_mutex_unlock(&temp_mutex);
	newtResume();
	project_file->save_file(xml_path);

	// Now make a call to email settings:
	void (*function_pointer)() = email_settings;
	newtPopWindow();
	function_pointer();

}
void email_settings(){
    newtComponent form, window;
    // Wait a moment to refresh screen
    newtCls();

    newtDrawRootText(0, 0, version_info().c_str());
    newtCenteredWindow(box_width, box_height, main_menu_options[5].c_str());
    window = newtListbox((box_width / 4), 2, (box_width / 8), 1);

    int val = email_menu_options.size() + 1;
	vector<int> list(val);

	for (int i = 0; i < list.size(); i++)
		list[i] = i;

    std::string temp;
    for (int i = 0; i < email_menu_options.size(); i++) {
    	temp = static_cast<char>((i + 1) + '0');
    	newtListboxAppendEntry(window, string{temp + " " + email_menu_options[i]}.c_str(),
							   reinterpret_cast<void *>((&list[i])));
	}

	// Awful method to center last option and add a number to it:
	// I also didn't want to create another string.
	char last_option[50];

	last_option[0] = static_cast<char>(val + '0');
	last_option[1] = ' ';
	strncpy(last_option + 2, "Return to Main Menu", 25);

	newtListboxAppendEntry(window, last_option, reinterpret_cast<void *>(&list[val - 1]));

    // Always create a new form for every screen.
    form = newtForm(nullptr, nullptr, 0);
	newtFormAddComponents(form, window, nullptr);
    newtRunForm(form);

	auto option_point =
			*reinterpret_cast<int *>(newtListboxGetCurrent(window));

	// Destroy the form.
	newtFormDestroy(form);
	email_selection(option_point);
}

void main_menu(){
    // Check this first.
    
    if (main_menu_options.size() != menu_pointers.size())
	throw runtime_error("Error: Both the options and menu_pointer "
					 "vectors have unequal lengths. Check them again.");
    
    newtComponent form, window, n_list, exit;

    newtCls();
    
    newtDrawRootText(0, 0, version_info().c_str());
    newtCenteredWindow(box_width, box_height, "Main Menu");
    newtPushHelpLine("Please select an option.");
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
		newtListboxAppendEntry(window, string{temp + " " + main_menu_options[i]}.c_str(),
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

inline void return_to_menu(){
    void (*function_pointer) () = main_menu;
    // Pop the current window:
    newtPopWindow();
    function_pointer();
}


//------------------------------------------------------------------------------
// Menu(): This function initializes newt, and calls main_menu in order to
// prevent newt from begin initialized constantly. 
//------------------------------------------------------------------------------
void menu(){
    newtInit();
    main_menu();
    newtFinished();
}
