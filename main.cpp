/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/01/2020 at 11:41 PM
 * 
 * main.cpp
 * 
 * -----------------------------------------------------------------------------
 */

#include "./Project.h"

void info(){
    cout << "CSC 4110 Project Menu (Version " << CSC4110_Project_VERSION_MAJOR
	 << "." << CSC4110_Project_VERSION_MINOR << ")" << endl;
    
    cout << "This entire menu screen is an temporary screen in order to provide some form that can be worked with." << endl;
    
    cout << "In the future, I plan on making this menu in ncurses, "
	 << "which each option fully functional.\n";


}

void options(void){
    vector<string> choices = {"Check Sensor Status", "Enable/Disable Sensors",
			      "Sensor Test", "Search Logs", "Show warnings",
			      "Exit" };
    
			      


}



void test_connection(void){
    cout << "Testing connection:" << endl;
    // Make connection to postgresql
    //pqxx::connection connection("postgresql://project:Pi_Project@192.168.1.93/csc4110_project");
    pqxx::connection connection(database_path);

    pqxx::work transaction{connection};
    pqxx::result r = transaction.exec("SELECT * FROM log");

/*
    for (auto const &row: r)
    {
        for (auto const &field: row) std::cout << field.c_str() << '\t';
        std::cout << std::endl;
    }
*/
    pqxx::row row1 = r.begin();
    Log l{row1};

    cout << connection.connection_string() << endl;
    cout << "Output of Log: " <<  endl << l << "\n";
    //cout << "Check Output:\n";
    l.comment = "This should be the second log in the table. Please ignore it.";
    ifstream is;
    send_log_as_SMTP_body(l, is);
    //add_log(transaction, l);
    connection.close();



}

int main(void) {
    info();
    test_connection();

}
