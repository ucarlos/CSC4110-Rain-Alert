/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/26/2020 at 10:14 PM
 * 
 * XML_Configuration.cc
 * 
 * Reads from configuration.xml instead of the three files used for the project
 * (smtp_info.txt, database_info.txt, settings.txt). Adapted from the Boost
 * xml example.
 * -----------------------------------------------------------------------------
 */

#include "./Settings.h"


/**
 * Load the contents of the xml file located in filename into
 * a Settings object.
 * @param filename The path of the file to load from.
 * 
 */
void Settings::load_file(const std::string &filename){
    
    boost::property_tree::ptree tree;

    // Load the XML file into the property tree. An exception will be
    // thrown if it doesn't work.
    boost::property_tree::read_xml(filename, tree);
    
    // Iterate over configuration.smtp_info first


    for (auto &i : tree.get_child("settings.smtp_info"))
	smtp_info.insert(std::pair<std::string, std::string>(
			     i.first.data(), i.second.data()));

    // // Next, database info

    for (auto &i: tree.get_child("settings.database_info"))
		database_info.insert(std::pair<std::string, std::string>(
					 i.first.data(), i.second.data()));
    

    // Time zone:
    time_zone = tree.get<int8_t>("settings.sensor_log.time_zone");
    // Email type
    auto temp = tree.get<int8_t>("settings.sensor_log.email_type");
    set_email_type(temp);
    // Lastly, get the time.
    daily_email_time = tree.get<std::string>("settings.sensor_log.daily_email_time");

}

/**
 * Saves the contents of a Settings object into an xml file located
 * in the path specified in filename.
 * @param filename Name of the file to save information to.
 */
void Settings::save_file(const std::string &filename){
    
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
    tree.put("settings.sensor_log.email_type", (int)e_t);
    tree.put("settings.sensor_log.time_zone", time_zone);
    boost::property_tree::xml_writer_settings<std::string> xml_indent('\t', 1);
    
    boost::property_tree::write_xml(filename, tree, std::locale(), xml_indent);
    //write_xml(filename, tree);
}


/**
 * Toggle between sending an HTML email or a Plain text HTML.
 * @param val integer value between 0 and 1.
 */
void Settings::set_email_type(int val){
	// Do nothing if the value is out of range.
	if (val != 0 && val != 1) return;

    e_t = (!val) ? email_type::html : email_type::plain_text;
}

// Wrapper around filesystem::exists
bool verify_xml_path(const std::string &path){
    return std::filesystem::exists(path);
}
