/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/01/2020 at 11:41 PM
 * 
 * [Test.cc]
 * 
 * This file is to simply hold all the tests that are needed by
 * Basic_Menu.cc/Menu.cc.
 * -----------------------------------------------------------------------------
 */

#include "./Project.h"

//------------------------------------------------------------------------------
// show_result_contents(): Given a pqxx::result variable, output
// all its contents into a file specified by database_results_path.
// Initially, this was used for testing, but I've modified it to work
// for the Basic Menu.
//------------------------------------------------------------------------------
void show_result_contents(pqxx::result &r){
	if (r.empty()){
		cerr << "No results found." << endl;
		sleep(2);
		return;
	}


	// Store into a file called Database_Results.txt
	// and then open in less/nano.
	ofstream ofs{database_results_path, ios_base::trunc};
	ofs << "You are in \"less\" mode. In order to escape, press q" << endl;
	ofs << "The results are listed below." << endl << endl;
	Log log;
	for (auto const &row : r){
		log.apply_raw_data(const_cast<pqxx::row &>(row));
		log.extract_row();
		ofs << log << endl;
	}
	// Now open up less.
	system("clear");
	string sys_call = "less ";
	sys_call = sys_call + " " + database_results_path;
	system(sys_call.c_str());
}


//------------------------------------------------------------------------------
// test_log_reading(): Function opens a connection to the database and
// searches for the first entry in the database. If found, everything's good.
// If not, please add the first entry specified in "./sql/db_populate.sql"
//------------------------------------------------------------------------------
bool test_log_reading(pqxx::connection &c){
    // Make sure that there's a connection:
    if (!c.is_open()){
	cerr << "Warning: This connection is not open."
	     << "Make sure that the connection is open before"
	     << " testing log readings.\n";
	return false;
    }

    pqxx::work trans{c};
    // Read the first log in the database.
    std::ostringstream query;
    query << "Select " << trans.quote("*") << "from log where log_comment = "
       << trans.quote("This is a dummy log that will always be added "
                      "to this table. Please ignore it.");

    pqxx::result result = trans.exec(query.str());
    // Now check is result is empty.
    if (result.empty()){
        cerr << "Warning: No result was return after executing a query "
	     << "for the first entry in the database. Please make sure that "
	     << "you add the first entry.\n";
	return false;
    }
    return true;
    
}

//------------------------------------------------------------------------------
// test_connection(): Make sure that a connection can be made to the database
// server using the info from "./sql/database_info.txt".
//------------------------------------------------------------------------------
void test_connection(void){
    cout << "Testing connection:" << endl;
    pqxx::connection connection;
    initialize_connection(connection);
    test_log_reading(connection);
    close_connection(connection);

}


//------------------------------------------------------------------------------
// test_smtp(): Tests whether an sample log can be sent to a recipient
// addresses specified by smtp_receiver_address.
//------------------------------------------------------------------------------
void test_smtp(void){
    Log l;
    ifstream ifs;
    l.comment = "<b>This is intended to be a test of the SMTP client."
                "Please disregard it.</b>";

    cout << "Testing SMTP HTML.." << endl;
    sleep(2);
    string type = "SMTP HTML TEST";
    send_log_as_HTML(l, type);

    type = "SMTP TEXT TEST";
    l.comment = "This is intended to be a test of the SMTP client."
                "Please disregard it.";
    cout << "Testing SMTP TEXT.." << endl;
    sleep(2);
    send_log_as_text(l, type);

}

//------------------------------------------------------------------------------
// test_pthread() : Tests creating a pthread and joining it.
//------------------------------------------------------------------------------
void * pthread_function1(void *val){
    cout << "This is thread 1!" << endl;
    cout << "Now, I'll make a basic loop for Thread 1:" << endl;
    for (int i = 0; i < 5; i++)
	cout << "Message " << i << " in Thread 1!" << endl;

    return nullptr;

}

void * pthread_function2(void *val){
    cout << "This is thread 2!" << endl;
    cout << "Now, I'll make a basic loop for Thread 2:" << endl;
    for (int i = 0; i < 5; i++)
	cout << "Message " << i << " in Thread 2!" << endl;

    return nullptr;
}


void test_pthread(void){
    // First create a pthread_t
    pthread_t thread1, thread2;
    int whatever_arg;
    int pthread_check = pthread_create(&thread1, nullptr, pthread_function1,
				       static_cast<void *>(&whatever_arg));

    string error_message{"Could not create Pthread 1."};
    check_pthread_creation(pthread_check, error_message);
    
    
    pthread_check = pthread_create(&thread2, nullptr, pthread_function2,
				   static_cast<void *>(nullptr));

    error_message = "Could not create Pthread 2.";
    check_pthread_creation(pthread_check, error_message);
    
    // Now join both
    pthread_join(email, nullptr);
    pthread_join(sensor, nullptr);

}
