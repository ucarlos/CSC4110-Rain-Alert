/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/01/2020 at 11:41 PM
 * 
 * Debug Functions.cc
 * 
 * This file is to simply hold all the tests that are needed by
 * Basic_Menu.cc/Menu.cc.
 * -----------------------------------------------------------------------------
 */

#include "./Project.h"


/**
 * Given a pqxx::result variable, output all its contents into a file specified by database_results_path.
 * Initially, this was used for testing, but I've modified it to work for the Basic Menu.
 * @param r An pqxx:result object that contains any rows found from a query.
 */
void show_result_contents(pqxx::result &r){
	if (r.empty()){
		std::cerr << "No results found." << std::endl;
		sleep(2);
		return;
	}


	// Store into a file called Database_Results.txt
	// and then open in less/nano.
	std::ofstream ofs{database_results_path, std::ios_base::trunc};
	ofs << "You are in \"less\" mode. In order to escape, press q" << std::endl;
	ofs << "The results are listed below." << std::endl << std::endl;
	SensorLog log;
	for (auto const &row : r){
		log.apply_raw_data(const_cast<pqxx::row &>(row));
		log.extract_row();
		ofs << log << std::endl;
	}
	// Now open up less.
	system("clear");
	std::string sys_call = "less ";
	sys_call = sys_call + " " + database_results_path;
	system(sys_call.c_str());
}



/**
 * Tests whether a connection can be made to the remote database stored in the
project's setting file and whether the database has been initialized with the
dummy row specified in sql/db_populate.sql.
 * @param c A pqxx::connection that represents a connection to a database.
 * @returns A boolean value indicating if the operation succeeded or not.
 */
bool test_log_reading(pqxx::connection &c){
    // Make sure that there's a connection:
    auto& debug_file{DebugLog::instance()};

    if (!c.is_open()){

		debug_file << DebugLevel::WARNING
                   << ": test_log_reading(): Warning: This connection is not open."
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
        debug_file << DebugLevel::WARNING
                   << ": Warning: No result was return after executing a query "
                   << "for the first entry in the database. Please make sure that "
                   << "you add the first entry.\n";
		return false;
    }
    return true;
    
}

/**
 * Make sure that a connection can be made to the database server using the info from "./sql/database_info.txt".
 *
 */
void test_connection(){
    auto& debug_file{DebugLog::instance()};
	debug_file << DebugLevel::INFO << "test_connection(): Testing connection:" << std::endl;
    pqxx::connection connection;
    initialize_connection(connection);
    test_log_reading(connection);
    debug_file << DebugLevel::INFO << "test_connection(): Closing Connection." << std::endl;
    close_connection(connection);

}

/**
 * Tests whether an sample log can be sent to a recipient addresses specified by smtp_receiver_address.
 */
void test_smtp(){
    SensorLog l;
    l.comment = "<b>This is intended to be a test of the SMTP client."
		"Please disregard it.</b>";

	std::cout << "Testing SMTP HTML.." << std::endl;
    sleep(2);

	std::string type = "SMTP HTML TEST";
    send_log_as_HTML(l, type);

    type = "SMTP TEXT TEST";
    l.comment = "This is intended to be a test of the SMTP client."
		"Please disregard it.";
	std::cout << "Testing SMTP TEXT.." << std::endl;
    sleep(2);
    send_log_as_text(l, type);

}


void pthread_function1(){
	std::cout << "This is thread 1!" << std::endl;
	std::cout << "Now, I'll make a basic loop for Thread 1:" << std::endl;
    for (int i = 0; i < 5; i++)
		std::cout << "Message " << i << " in Thread 1!" << std::endl;


}

void pthread_function2(){
	std::cout << "This is thread 2!" << std::endl;
	std::cout << "Now, I'll make a basic loop for Thread 2:" << std::endl;
    for (int i = 0; i < 5; i++)
		std::cout << "Message " << i << " in Thread 2!" << std::endl;

}


/**
 * Tests pthread creation and joining.
 */
void test_pthread(){
    // First create a pthread_t
    std::thread thread{pthread_function1};
    std::thread thread2{pthread_function2};

    thread.join();
    thread2.join();
}
