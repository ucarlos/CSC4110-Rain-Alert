/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/26/2020 at 10:14 PM
 * 
 * Configuration.cc
 * 
 * Reads from configuration.xml instead of the three files used for the project
 * (smtp_info.txt, database_info.txt, settings.txt). Adapted from the Boost
 * xml example.
 * -----------------------------------------------------------------------------
 */

#include "./XML_Configuration.h"



void settings_file::load_file(const std::string &filename){
    
    boost::property_tree::ptree tree;

    // Load the XML file into the property tree. An exception will be
    // thrown if it doesn't work.
    read_xml(filename, tree);
    
    // Iterate over configuration.smtp_info first


    for (auto &i : tree.get_child("settings.smtp_info"))
	smtp_info.insert(std::pair<std::string, std::string>(
			     i.first.data(), i.second.data()));

    // // Next, database info

    for (auto &i: tree.get_child("settings.database_info"))
		database_info.insert(std::pair<std::string, std::string>(
					 i.first.data(), i.second.data()));
    
    
    // Lastly, get the time.
    daily_email_time = tree.get<std::string>("settings.sensor_log.daily_email_time");

}

void settings_file::save_file(const std::string &filename){
    
    boost::property_tree::ptree tree;

    // Iterate through the fields and place them into the tree.

	// Smtp_info
    tree.put("settings.smtp_info.sender_email",
    		smtp_info["sender_email"].data());
	tree.put("settings.smtp_info.sender_password",
			smtp_info["sender_password"].data());
	tree.put("settings.smtp_info.receiver_email",
			smtp_info["receiver_email"].data());

	//Database_info
	tree.put("settings.database_info.psql_ip",
			database_info["psql_ip"].data());
	tree.put("settings.database_info.psql_login",
			database_info["psql_login"].data());
	tree.put("settings.database_info.psql_database",
			database_info["psql_database"].data());
	tree.put("settings.sensor_log.daily_email_time",
			daily_email_time);

	write_xml(filename, tree);

}
// Wrapper around filesystem::exists
bool verify_xml_path(const std::string &path){
	return std::filesystem::exists(path);
}