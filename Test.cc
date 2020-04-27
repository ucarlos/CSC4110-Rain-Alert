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


void show_result_contents(pqxx::result &r){
	if (r.empty()){
		cerr << "No results found." << endl;
		return;
	}
	cout << "Results found:" << endl;
	for (auto const &row : r){
		for (auto const &field : row)
			std::cout << field.c_str() << "\t";
		std::cout << std::endl;
	}
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

