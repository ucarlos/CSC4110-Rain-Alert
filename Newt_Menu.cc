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
const uint32_t box_width = 64;
const uint32_t box_height = 16;

// Since all buttons will be at the end,
// Only the x value has to be changed (Multiplied by 1, 2, or 3)
const uint32_t button_y = 7 * (box_height / 8);
const uint32_t button_x = (box_width / 8);



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


//------------------------------------------------------------------------------
// return_to_menu_button(): function called by a button that returns to the
// main menu.
//------------------------------------------------------------------------------

void return_to_menu_button(newtComponent_struct *button, void *form){
    // Destroy the form
    // call return_to_menu
    auto it = reinterpret_cast<newtComponent>(form);
    newtFormDestroy(it);
    return_to_menu();
}


//------------------------------------------------------------------------------
// error_box() : Create an error box that either returns to main_menu or
// a specified function pointer
//------------------------------------------------------------------------------

void error_box(const string &error_message, void (*function_pointer)()){
    newtCls();
    newtCenteredWindow(box_width, box_height / 4, "Error!");
    newtDrawRootText(0, 0, version_info().c_str());
    newtPushHelpLine("Forgive me father, for I have sinned deeply.");
    // Set up box components
    
    int32_t align_x = ((box_width / 2) - (error_message.size() / 2));
    newtComponent error_label = newtLabel(align_x, 0, error_message.c_str());
    
    const char *ok_message = "OK";
    align_x = ((box_width / 2) - ((strlen(ok_message) + 2) / 2) - 1);
    newtComponent ok_button = newtCompactButton(align_x,
						box_height / 8, ok_message);

    // Now add all components
    newtComponent error_form = newtForm(nullptr, nullptr, 0);
    newtFormAddComponents(error_form, error_label, ok_button, nullptr);

    newtRunForm(error_form);	
    newtFormDestroy(error_form);

    // Pop the window and go to wherever the pointer leads to
    if (function_pointer != nullptr){
	newtPopWindow();
	function_pointer();
    }
    else // by default, return to main menu.
	return_to_menu();
    

}

//------------------------------------------------------------------------------
// run_sensor_tracking(): Creates the pthreads that control email sending
// sensor reading. Anagalous of Tracking() in Basic_Menu.cc
//------------------------------------------------------------------------------

void run_sensor_tracking(Sensor_Date &sd){
    // First clear the screen and pause newt
    newtSuspend();
    newtCls();
    
    string mutex_error_message;
    mutex_error_message = "Could not initialize the mutex while creating";
    mutex_error_message += " the pthreads.";
    int32_t mutex_check = pthread_mutex_init(&log_mutex, nullptr);
    
    check_pthread_creation(mutex_check, mutex_error_message);

    // Create a temp log
    Log temp_log;
    int32_t pthread_check = pthread_create(&sensor, nullptr,
					   handle_sensor_thread,
					   static_cast<void*>(&temp_log));
    
    string pthread_error_message;
    pthread_error_message = "Could not create the Sensor reading pthread.";
    
    check_pthread_creation(pthread_check, pthread_error_message);

    // Now create the Email Sending pthread
    pthread_error_message = "Could not create the Email Sending pthread.";
    pthread_check = pthread_create(&email, nullptr, send_email_thread,
				   static_cast<void*>(&sd));
    
    check_pthread_creation(pthread_check, pthread_error_message);

    // Now join both pthreads at the end.
    pthread_join(email, nullptr);
    pthread_join(sensor, nullptr);

    mutex_check = pthread_mutex_destroy(&log_mutex);
    mutex_error_message  = "Could not destroy the mutex while";
    mutex_error_message += " creating the pthreads.";
    check_pthread_creation(mutex_check, mutex_error_message);

    // Now resume newt.
    newtResume();
    return_to_menu();

}


//------------------------------------------------------------------------------
// sensor_tracking(): Handles Enabling/Disabling Tracking. 
//------------------------------------------------------------------------------

void sensor_tracking(void){
    // Read user time
    Sensor_Date sd;
    read_user_time(sd);
    
    // Make sure to pop the last window:
    newtPopWindow();
    newtComponent form, window, n_list, back, message_label;
    // Wait a moment to refresh screen
    newtCls();
    
    
    newtDrawRootText(0, 0, version_info().c_str());
    
    newtCenteredWindow(box_width, box_height, main_menu_options[0].c_str());

    // Get when the email will be sent:
    const string time = sd.get_user_time();
    string time_info;
    time_info = "Status: A Daily email report will be sent at " + time + " (" + twelve_hour_clock(time) +
	")";
    
    newtPushHelpLine(time_info.c_str());
    
    // Always create a new form for every screen.
    form = newtForm(nullptr, nullptr, 0);
    
    // Write main label:
    const string message = "Please select an option.";
    message_label = newtLabel(((box_width / 2) - (message.size() / 2)),
			      0, message.c_str());

    const string enable_str = "Enable Tracking";
    const string disabled_str = "Disable Tracking";

    // Set which radio button is selected by default:
	bool current_status = project_file->get_tracking_status();
    // Set where the enable radio will be placed

    int32_t align = (3 * (box_width / 8));
    newtComponent enable_button = newtRadiobutton(align , (box_height / 8),
						  enable_str.c_str(),  static_cast<int32_t>(current_status), nullptr);

    //align = (5 * (box_width / 8) - (disabled_str.size() / 2));	
    newtComponent disable_button = newtRadiobutton(align, ((box_height / 8) + 1),
						   disabled_str.c_str(),
						   !(static_cast<int32_t>(current_status)), enable_button);


    
    const string return_message = "Return";
    align = ((box_width / 2) - (return_message.size() / 2) - 2);
    
    back = newtCompactButton(align, button_y, return_message.c_str());
    
    //cont = newtCompactButton(6 * button_x, button_y, "Forward!");
    newtFormAddComponents(form, back, message_label, enable_button,
			  disable_button, nullptr);

    newtRunForm(form);
    // Get the user selected radio button.
    // If tracking_is disabled and button is disable or tracking_enabled and button is enabled,
    //create a message saying "No change" and return to menu.
    // Otherwise take action.
    newtComponent user_input = newtRadioGetCurrent(enable_button);

    // If tracking is enabled/disabled and the user checks the same radio button,
    // then return to the main menu.

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    string mutex_error_message;
    
    if (current_status){ // Disable Tracking
	if (user_input == enable_button)
	    error_box("Tracking is already enabled.", nullptr);
	// Set tracking to false and kill any threads working.
	
	int32_t mutex_check = pthread_mutex_init(&mutex, nullptr);
	mutex_error_message = "Could not initialize the mutex.";
	check_pthread_creation(mutex_check, mutex_error_message);
	
	pthread_mutex_lock(&mutex);
	project_file->set_tracking_status(false);
	end_all_threads = true;
	pthread_mutex_unlock(&mutex);
	
	newtFormDestroy(form);
	mutex_error_message = "Could not destroy the mutex while disabling tracking.";
	mutex_check = pthread_mutex_destroy(&mutex);
	check_pthread_creation(mutex_check, mutex_error_message);
	return_to_menu();
	
    }
    else{ // Enable Tracking
	if (user_input == disable_button)
	    error_box("Tracking is already disabled.", nullptr);

	int32_t mutex_check = pthread_mutex_init(&mutex, nullptr);
	mutex_error_message = "Could not initialize the mutex.";
	check_pthread_creation(mutex_check, mutex_error_message);


	pthread_mutex_lock(&mutex);
	project_file->set_tracking_status(true);
	end_all_threads = false;
	pthread_mutex_unlock(&mutex);

	newtFormDestroy(form);
	mutex_error_message = "Could not destroy the mutex while enabling tracking.";
	mutex_check = pthread_mutex_destroy(&mutex);
	
	check_pthread_creation(mutex_check, mutex_error_message);
	run_sensor_tracking(sd);
    }

}

//------------------------------------------------------------------------------
// show_status(): Shows the current values reported by the sensors.
// The output is stored in a temp file that is opened by less.
//------------------------------------------------------------------------------
void show_status(void){
    // Make sure to stop newt for this section:
    newtPopWindow();
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

//------------------------------------------------------------------------------
// search_logs(): Allows the user to search the database for logs on
// a specified date (mm/dd/yyyy).
//------------------------------------------------------------------------------
void search_logs(void){
    // Wait a moment to refresh screen
    newtPopWindow();
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
    int32_t search_len = 10;
    
    log_result = newtEntry(((box_width / 2) - (search_len / 2)),
			   (box_height/4),
			   "mm/dd/yyyy", search_len,
			   reinterpret_cast<const char **>(message.get()),
			   NEWT_FLAG_RETURNEXIT);
    


    // Always create a new form for every screen.
    form = newtForm(nullptr, nullptr, 0);


    // Make sure back button returns to main menu:
    newtCallback ret = &return_to_menu_button;
    newtComponentAddCallback(back_but, ret, form);


    newtFormAddComponents(form, log_message_label, back_but,log_result, nullptr);
    newtRunForm(form);
    
    // Suspend newt() and Make a query.
    string error;
    std::string result = newtEntryGetValue(log_result);
    newtSuspend();

    // Shit hack to exit screen:
    if (result == "mm/dd/yyyy"){
	newtResume();
	return_to_menu();
    }
	
    while (!verify_date(result)){

	newtResume();
	string error_message = "Make sure that the";
	error_message += " date format is in mm/dd/yyyy and is valid.";
	void (*function_pointer)() = search_logs;
	error_box(error_message, function_pointer);
	//goto confession;
		  
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

void test_sensor_selection(const int &option){
    newtCls();
    newtSuspend();
    
    int32_t selection = (option + 'a');
    switch(selection){
    case 'a':
	test_smtp();
	break;
    case 'b':
	test_connection();
	break;
    case 'c':
	test_pthread();
	break;
    default:
	newtResume();
	return_to_menu();
    }
    // Sleep for 3 seconds, clear the screen and return to menu.
    sleep(3);
    system("clear");
    newtResume();
    return_to_menu();
}

//------------------------------------------------------------------------------
// test_sensors(): Provides the user to test the project components, such as
//     * Testing whether the SMTP server can send an test email
//     * Connecting to the database and retrieving a query.
//     * Testing whether two pthreads can be made.
//------------------------------------------------------------------------------
void test_sensors(){

    newtComponent form, list_box;
    newtPopWindow();
    newtCls();
    newtPushHelpLine("You can test each component here.");
    newtDrawRootText(0, 0, version_info().c_str());
    newtCenteredWindow(box_width, box_height, main_menu_options[3].c_str());
    list_box = newtListbox((box_width / 4), 2, (box_width / 8), 1);

    int val = test_sensor_menu_options.size() + 1;
    vector<int> list(val);

    for (int i = 0; i < list.size(); i++)
	list[i] = i;

    string temp;

    for (int i = 0; i < test_sensor_menu_options.size(); i++){
	temp = static_cast<char>((i + 1) + '0');
	newtListboxAppendEntry(list_box,
			       string{temp + " " + test_sensor_menu_options[i]}.c_str(),
			       reinterpret_cast<void *>(&list[i]));

    }


    string last_option = "  ";
    last_option[0] = static_cast<char>(val + '0');
    last_option[1] = ' ';
    last_option += "Return to Main Menu";

    newtListboxAppendEntry(list_box, last_option.c_str(),
			   reinterpret_cast<void *>(&list[val - 1]));

    //Set up the form:
    form = newtForm(nullptr, nullptr, 0);
    newtFormAddComponents(form, list_box, nullptr);
    newtRunForm(form);
    
    // Take the option, and make the selection:
    auto option_input =
	*reinterpret_cast<int *>(newtListboxGetCurrent(list_box));

    newtFormDestroy(form);
    test_sensor_selection(option_input);
    

}

//------------------------------------------------------------------------------
// database_options(): Displays information about the current database.
//------------------------------------------------------------------------------
void database_options(){
    newtComponent form, window, label;
    // Wait a moment to refresh screen
    newtPopWindow();
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
    int32_t select = (option + 'a');
    int32_t val;

    switch(select){
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

//------------------------------------------------------------------------------
// email_settings(): Opens a menu to allow the user to adjust the following
// options:
//     * Recipient Email Address
//     * What time the email will be sent
//     * What timezone should be used (UTC)
//     * What type of Email will be sent (Either HTML or Plain Text)
// This is then passed to email_selection(), which will execute whichever
// option was selected.
//------------------------------------------------------------------------------
void email_settings(){
    newtComponent form, window;
    // Wait a moment to refresh screen
    newtPopWindow();
    newtCls();
    newtPushHelpLine("Select an email option to configure.");
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

//------------------------------------------------------------------------------
// main_menu(): Creates a menu that allows the user to access the rest of
// the program.
//------------------------------------------------------------------------------
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
    // char exit_name[100];
    // exit_name[0] = static_cast<char>(menu_pointers.size() + 1 + '0');
    // exit_name[1] = ' ';
    string exit_name = "  ";
    exit_name[0] = static_cast<char>(menu_pointers.size() + 1 + '0');
    exit_name[1] = ' ';
    exit_name += "Exit Program";
    //strncpy(exit_name + 2, "Exit Program", 30);

    newtListboxAppendEntry(window, exit_name.c_str(),
			   reinterpret_cast<void *>(exit_program));


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
