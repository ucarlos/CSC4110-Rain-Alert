/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/26/2020 at 10:33 PM
 * 
 * Configuration.h
 *
 * This class handles reading the needed information from Project_Settings.xml
 * This method replaces reading from smtp_info.txt, settings.txt, and 
 * database_info.txt. This relays on the boost library, so use the former
 * If boost can't be installed on your system.
 * -----------------------------------------------------------------------------
 */

#ifndef CSC4110_PROJECT_XML_CONFIGURATION_H
#define CSC4110_PROJECT_XML_CONFIGURATION_H
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <filesystem>
#include <map>
#include <stdexcept>

bool verify_xml_path(const std::string &path);

class settings_file{
public:

	explicit settings_file(const std::string &filename){
		bool check = verify_xml_path(filename);
		if (!check) {
			throw std::runtime_error(filename + " could not be found. "
									   "Please make sure that the file exists"
			" in the root directory.");
		}
		xml_filename = filename;
		load_file(xml_filename);
	}
	// Ideally, I'd like to make this constant, but it seems more of a hassle to
	// implement set_smtp_info() and set_datebase_info.
	// So, TODO: Implement set_smtp_info() and set_datebase_info with bounds checking.
	std::map<std::string, std::string>& get_smtp_info()  { return smtp_info; }
	std::map<std::string, std::string>& get_database_info() { return database_info; }


	const std::string& get_email_time()  { return daily_email_time; }
	void set_email_time(std::string &new_filename) { daily_email_time = new_filename; }
	void load_file(const std::string &filename);
	void save_file(const std::string &filename);
private:
	std::map<std::string, std::string>smtp_info;
	std::map<std::string, std::string> database_info;
	std::string daily_email_time;
	std::string xml_filename{};
};


#endif //CSC4110_PROJECT_XML_CONFIGURATION_H
