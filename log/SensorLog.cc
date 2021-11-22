/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/05/2020 at 06:16 PM
 * 
 * log.cpp
 * SensorLog class definitions, database log functions and smtp send functions are
 * contained in this file.
 * -----------------------------------------------------------------------------
 */

#include "./SensorLog.h"


//------------------------------------------------------------------------------
// SensorLog Class Definitions
//
//
//------------------------------------------------------------------------------

SensorLog::SensorLog(std::map<std::string, bool> &sensor, std::map<std::string, double> &levl,
	 std::string comm, std::string time_stamp) {
    // Maps should be the same size as default. Error otherwise.
    if ((this->sensor_check.size() != sensor.size()) ||
        (this->level.size() != levl.size())){
        std::ostringstream os;
        os << "Passed sensor and level arguments have different sizes"
           << " from the default maps in SensorLog. Aborting.";
        throw std::runtime_error(os.str());
    }
    this->sensor_check = sensor;
    this->level = levl;
    this->comment = std::move(comm);
    this->time_stamp = std::move(time_stamp);

}

// constructor
SensorLog::SensorLog(pqxx::row &row){ // Extract the contents of row and fill the log.
    raw_data = &row;
    this->extract_row();
}

// Copy Constructor
SensorLog::SensorLog(const SensorLog &l){
    this->comment = l.comment;
    this->time_stamp = l.time_stamp;
    this->date = l.date;
    this->sensor_check = l.sensor_check;
    this->level = l.level;
    this->raw_data = l.raw_data;

}

// Copy assignment
SensorLog& SensorLog::operator=(const SensorLog &l)= default;

// Comparison Operator:
bool SensorLog::operator==(const SensorLog &b){
    return (comment == b.comment) && (time_stamp == b.time_stamp)
	&& (date == b.date) && (sensor_check == b.sensor_check)
	&& (level == b.level) && (raw_data == b.raw_data);

}

/**
 * Extracts a row from the pqxx::row member variable and populates the
 * maps containing sensor data.
 * @returns A boolean determining whether the operation succeeded or not.
 */
bool SensorLog::extract_row() {
    if (!raw_data){ // Prevent access of empty raw_data
	std::cerr << "Warning: SensorLog does not point to any row.\n";
        return false;
    }

    // First, fill the date and timestamp.
    date = (raw_data->begin() + 1)->as(std::string{});
    auto it = raw_data->begin() + (log_offset - 1);
    time_stamp = it->as(std::string{});

    it++;

    // pqxx::field f; // Uses field
    // Now traverse through the boolean sensor_check
    for (auto i = sensor_check.begin();
	 (i != sensor_check.end() && it != raw_data->end()); i++, it++){
        i->second = it->as(bool{});
    }
    // Then the double level
    for (auto i = level.begin();
	 (i != level.end() && it != raw_data->end()); i++, it++){
        i->second = it->as(double{});
    }

    // This assumes that the column that it points to is a boolean value.
    if (it != raw_data->end()){
        comment = it->as(std::string{});
    }

    return true;


}

/**
 * Enables setting a psqxx::row object to a log object and then immediately
 * calls the log object's extract_row.
 * @param l An object of type SensorLog.
 * @param row An object of type row
 */
void extract_row_to_log(SensorLog &l, pqxx::row &row){
    if (l.row() == row){
        l.extract_row();
        return;
    }

    l.row() = row;
    l.extract_row();

}


/**
 * Outputs all sensor data information from a log object to a ostream.
 * @param os An ostream object that will be written to
 * @param l A constant reference to a log object
 * @returns A reference to a ostream object.
 */
std::ostream& operator<<(std::ostream &os, const SensorLog &l){
    std::string html_break = "<br>";
    std::ostringstream o;
    o << "Date: " << l.date << html_break << std::endl;
    o << "Timestamp: " << l.time_stamp << html_break << std::endl;

    for (auto & i : l.sensor_check){
        o << i.first << ": ";
        o << ((i.second) ? "Online" : "Offline");
        o << html_break << std::endl;
    }

    // Battery level (In Percentage)
    double d = l.level.at("battery_level");
    o << "Battery Level: " << d << "%" << html_break << std::endl;
    d = l.level.at("rain_level");
    o << "Rain level: " << d << " inches (" << inches_to_cm(d) << " cm)"
      << html_break << std::endl;
    o << "Comment: " << l.comment << html_break << std::endl;
    return os << o.str();

}

//------------------------------------------------------------------------------
// Database log functions
//
//
//------------------------------------------------------------------------------



/**
 * Inserts a log row into a database via a pqxx::transaction_base object.
 *
 * @param trans An pqxx::transaction_base object that establishes a database connection.
 * @param l The log object whose sensor data will be inserted into a database.
 */
void add_log(pqxx::transaction_base &trans, const SensorLog &l){
    std::ostringstream os;
    os << std::boolalpha;
    
    os << "INSERT INTO log(float_sensor_connected, rain_sensor_connected,"
       << " solar_panel_functional, battery_connected, battery_level, rain_level, log_comment)"
       << " VALUES (" ;

    for (const auto & i : l.sensor_check){
        os << trans.quote(i.second) << ", ";
    }


    // Use check to determine when the close the parenthesises.
    auto check = l.level.begin();
    advance(check, 1);

    for (const auto & i : l.level){
        os << trans.quote(i.second) << ", ";
    }
    os << trans.quote(l.comment) << ")";

    std::string query = os.str();
    //cout << query;
    trans.exec0(query);
    
    // Now update the database.
    trans.commit();
}


//------------------------------------------------------------------------------
// SMTP SensorLog Functions
//
//
//------------------------------------------------------------------------------

/**
 * Returns a string displaying the current time. The string is formatted as
 * follows: [Day_Name], Day, Month Year [Date in HH:MM:SS]
 * @returns A std::string object containing the date in the above format.
 */
std::string create_date(){
    std::time_t t = std::time(nullptr);
    char str[100];
    std::strftime(str, sizeof(str), "%a, %d %b %Y %T", std::localtime(&t));

    return std::string{str};
}

/**
 * Returns a string displaying the current time, following the format
 * specified in the format_string argument.
 * @param format_string Contains the format that will be passed in order to
 * generate a date string.
 * @returns A std::string object containing the date in a specified format.
 */
std::string create_date(std::string format_string) {
	std::time_t time1 = std::time(nullptr);
	char c_string[100];
	std::strftime(c_string,
				  sizeof(c_string),
				  format_string.c_str(),
				  std::localtime(&time1));

	return std::string{c_string};

}


/**
 * Populates a vector of std::string containing a stmp header formatted for an
 * HTML email.
 * @param message_type A std::string object that specifies the type of message passed.
 *
 * @returns A vector of strings that represent a SMTP header for an HTML email.
 */
std::vector<std::string> create_html_header(std::string &message_type) {

    std::ostringstream os;
    // find the newline:
    std::string date_str = create_date();
    std::vector<std::string> header_data;
    header_data.push_back(std::string{"Date: " + date_str});
    header_data.push_back(std::string{"To: " + smtp_receiver_address});
    header_data.push_back(std::string{"From: " + smtp_username});
    header_data.push_back(std::string{"Subject: Rain Alert Report: " + message_type});

    return header_data;
}

/**
 * Creates a SMTP Header to be used when sending a text only email.
 * Each line must be ended with '\r\n' to follow SMTP specifications.
 * 
 * SMTP Headers are structured in the following format:
 * Date: [CONTENT] \r\n
 * From: [CONTENT] \r\n
 * Message-ID:
 * Subject: [CONTENT] \r\n
 * Email Body:
 * \r\n
 *
 * @param message_type A std::string object that specifies the type of message
passed.
 * @returns a std::string object containing the SMTP Header.
 */
std::string create_smtp_text_header(std::string &message_type) {
    const std::string end_line = "\r\n";
    std::string date_str = create_date();

    std::ostringstream os;

    os << "Date: " << ""
       << date_str << end_line
       << "To: " << smtp_receiver_address << end_line
       << "From: " << smtp_username << end_line
       << "Subject: " << "Rain Alert Report: " << message_type <<  end_line
       << end_line;

    return os.str();
}

/**
 * Establish a SMTP Connection by providing the credentials found in the
settings file.
 * @param curl A CURL object to be used to prepare the connection.
 */
void set_up_stmp_connection(CURL *curl) {
    // Set up username and password:
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtp_username.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, smtp_password.c_str());

    // Set URL of mailserver.
    std::string url = "smtp://" + smtp_address + ":" + smtp_port;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Use TLS
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long) CURLUSESSL_ALL);

    // Don't have certificate, so disable some protection.
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    // Mail FROM:
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, smtp_username.c_str());



}

/**
 * Send an purely text-based email to the recipient address through SMTP.
 * This is done by opening up a connection, filling in the required fields,
 * and then sending the email.
 * @param oss An ostringstream object containing the text of the email.
 * @param message_type A std::string object that specifies the type of message
passed.
 * 
 */
void send_text_through_SMTP(std::ostringstream &oss, std::string &message_type) {
	// Write the following to the file in text_file_path:
	// * SMTP Header Specifying the appropriate information
	// * Output of SensorLog
    std::ofstream ofs{text_file_path, std::ios_base::trunc};
    ofs << create_smtp_text_header(message_type);
    ofs << oss.str();

    // Now close and reopen file as FILE
    ofs.close();
    FILE *fs = fopen(text_file_path.c_str(), "r");

    if (!fs){
        throw std::runtime_error("Could not open " + text_file_path);
    }

    CURL *curl = curl_easy_init();

    CURLcode result = CURLE_OK;

    if (curl) {
        struct curl_slist *recipients = nullptr;

        set_up_stmp_connection(curl);

        // Mail TO:
        recipients = curl_slist_append(recipients, smtp_receiver_address.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Read from file.
        curl_easy_setopt(curl, CURLOPT_READDATA, fs);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        // Enable debug info:
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        // Now send the message:
        result = curl_easy_perform(curl);

        // Now check for errors:
        if (result != CURLE_OK)
            std::cerr << "curl_easy_preform() failed : " << curl_easy_strerror(result)
                 << std::endl;

        curl_slist_free_all(recipients);

        // Always cleanup:
        curl_easy_cleanup(curl);
        fclose(fs);

    }
}


/**
 * Send an HTML based email to the recipient address through SMTP.
 * This is done by opening up a connection, filling in the required fields,
 * and then sending the email. If in the event that the HTML cannot be viewed,
 * the log object will call its operator<< to output its message contents as
 * text.
 * @param l An log object containing sensor information.
 * @param oss An ostringstream object containing the text of the email.
 * @param message_type A std::string object that specifies the type of message
passed.
 * 
 */
void send_html_through_SMTP(const SensorLog &l, std::ostringstream &oss, std::string &message_type) {
    
    struct curl_slist *recipients = nullptr;
    struct curl_slist *headers = nullptr;
    struct curl_slist *slist = nullptr;
    curl_mime *mime;
    curl_mime *alt;
    curl_mimepart *part;

    CURL *curl = curl_easy_init();
    
    CURLcode result = CURLE_OK;

    if (curl) {
        // Set up username and password:
        set_up_stmp_connection(curl);

        // Mail TO:
        recipients = curl_slist_append(recipients, smtp_receiver_address.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        std::vector<std::string> header_list = create_html_header(message_type);
        // Create Header
        for (auto &i : header_list)
            headers = curl_slist_append(headers,i.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up the mime message:
        mime = curl_mime_init(curl);
        alt = curl_mime_init(curl);

        /* The inline part is an alternative proposing the html and the text
       versions of the e-mail. */
        alt = curl_mime_init(curl);

        /* HTML message. */
        part = curl_mime_addpart(alt);
        std::string inline_html = oss.str();
        curl_mime_data(part, inline_html.c_str(), CURL_ZERO_TERMINATED);
        curl_mime_type(part, "text/html");

        /* Text message. */
        part = curl_mime_addpart(alt);
        std::ostringstream text_section;
        text_section << "In case the inline html does not display correctly,"
		     << "the text is displayed below:" << std::endl
		     << l << std::endl;

        curl_mime_data(part, text_section.str().c_str(), CURL_ZERO_TERMINATED);

        /* Create the inline part. */
        part = curl_mime_addpart(mime);
        curl_mime_subparts(part, alt);
        curl_mime_type(part, "multipart/alternative");
        slist = curl_slist_append(nullptr, "Content-Disposition: inline");
        curl_mime_headers(part, slist, 1);


        /* Add the current source program as an attachment. */
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        // Enable debug info:
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        // Now send the message:
        result = curl_easy_perform(curl);

        // Now check for errors:
        if (result != CURLE_OK)
            std::cerr << "curl_easy_preform() failed : " << curl_easy_strerror(result)
                 << std::endl;

		curl_slist_free_all(recipients);
        curl_slist_free_all(headers);

        /* Free multipart message. */
        curl_mime_free(mime);

		// Always cleanup:
		curl_easy_cleanup(curl);
		//fclose(fs);

    }

}

/**
 * Sends the contents of a log object as a HTML email. This is done by
 * creating the header and sensor information and then sending the email.
 *
 * @param l An log object containing sensor information.
 * @param message_type A std::string object that specifies the type of message
to be passed.
 * 
 */
void send_log_as_HTML(const SensorLog &l, std::string &message_type){
    // Open the html template, append the log in it
    // and then pass it to the header.
    std::string smtp_endl = "\r\n";
    std::ifstream ifs{html_template_path};
    std::ostringstream new_file{};
    std::string check = "                <!--- INSERT MESSAGE :) -->";
    std::string temp;
    // Copy all data from html template to new file until the check is true.
    do{
        getline(ifs, temp);
        new_file << temp << smtp_endl;
    } while ((ifs.good() && temp != check));

    if (temp != check){
	std::string error = "I couldn't insert the log into " + html_template_path;
	error += "Make sure that the check comment is included in the file.\n";
        throw std::runtime_error(error);
    }
    
    // <!--- INSERT MESSAGE :) -->
    // Insert header
    new_file << "<h1> Pi Rain Alert: "
			 << message_type << " for "
             << create_date("%d %b %Y")
			 << "</h1>" << std::endl;
	
    new_file << "<p>" << smtp_endl;


    new_file << l << smtp_endl;
    new_file << "</p>" << smtp_endl;

    // Now continue the rest of the file:
    ifs.clear();
    while (ifs.good()){
        getline(ifs, temp);
        new_file << temp << smtp_endl;
    }
    // close file
    //new_file.close();
    // Now prepare to send
    send_html_through_SMTP(l, new_file, message_type);
}


/**
 * Sends the contents of a log object as a Text-only email.
 * @param l An log object containing sensor information.
 * @param message_type A std::string object that specifies the type of message
to be passed.
 */
void send_log_as_text(const SensorLog &l, std::string &message_type) {
    std::ostringstream new_file{};
    new_file << l;
    send_text_through_SMTP(new_file, message_type);

}
//------------------------------------------------------------------------------
// verify_user_name() : Very basic function to determine if a
// user_name is valid by using regex expression.
// Username should follow name@domain_name.domain.
//------------------------------------------------------------------------------

/**
 * Verifies whether a username is valid through the use of regular expressions.
 * @param test The Regex object to be used
 * @param user_name String containing the user_name to be checked.
 * @returns A boolean determining if the username was valid or not.
 */
bool verify_username(std::regex &test, const std::string &user_name) {
    std::string regex_str = R"(^[A-z][\w|\.]*@\w+\.+[\w | .]*[a-zA-Z]$)";
    // Use regex expressions to check for valid user_name.
    // ^[A-z][\w|\.]*@\w+\.+[\w | .]*[a-zA-Z]$
    test = std::regex{regex_str};
    return regex_match(user_name,test);

}



/**
 * Verifies whether a password is valid through the use of regular expressions.
 * The password must be at least six characters.
 * @param test The Regex object to be used
 * @param password String containing the password to be checked.
 * @returns A boolean determining if the password was valid or not.
 */

bool verify_password(std::regex &test, const std::string &password) {
    // Password should at least be 6 characters.
    std::string regex_str = "^[\\w | \\.]{6,}$";
    test = std::regex{regex_str};
    return regex_match(password, test);
}

/**
 * Verifies whether a password is valid through the use of regular expressions.
 * The user can specify the minimum amount of characters for the password.
 * @param test The Regex object to be used
 * @param size The minimum amount of characters for the password. The maximum
size is 127 characters.
 * 
 * @param password String containing the password to be checked.
 * @returns A boolean determining if the password was valid or not.
 */
[[maybe_unused]] bool verify_password(std::regex &test, int8_t &size, const std::string &password) {
    std::ostringstream os;
    os << R"("^[\\w | \\.]{)" << size << ",}$";
    test = std::regex{os.str()};
    return regex_match(password, test);
}

//------------------------------------------------------------------------------
// Creck_credentials : Quick check of booleans for SMTP reading
// This mainly exists in order to make get_smtp_credentials() shorter and
// to better describe error messages.
//------------------------------------------------------------------------------
/**
 * Determines if the SMTP credentials can be read or not.
 */
inline void check_credentials(bool check, std::string error) {
    if (!check)
        throw std::runtime_error(error);
}



/**
 * Read the SMTP credentials stored in smtp_info.txt. The file should be
 * formatted in the following way:
 * Line 1: sender email address
 * Line 2: sender email password
 * Line 3: recipient email address
 * @deprecated This function has been superseded by get_smtp_info_from_xml as
all settings have been merged into a single file. This should be used if the
Boost C++ Library cannot be installed on your system.
 *
 */
[[maybe_unused]] void get_smtp_credentials(){
    // Read the data from
    std::ifstream file{smtp_file_path};
    if (!file){
	    throw std::runtime_error("Cannot retrieve the SMTP credentials from " + smtp_file_path);
    }

    // Now check if the file is empty.
    if (file.peek() == std::ifstream::traits_type::eof()){
        throw std::runtime_error("smtp_info.txt is emtpy. Change that.");
    }

    // Retrieve the credentials and assign them
    std::string smtp_us, smtp_pass, reciv_us;
    getline(file, smtp_us);
    getline(file, smtp_pass);
    getline(file, reciv_us);

    auto file_status = file.rdstate();

    // The file should ONLY have the eof flag enabled after reading the file to the end.
    // If it doesn't meet those requirements, throw an error.
    if (file_status != std::ios_base::eofbit){
        std::string error = "Could not populate all variables from smtp_info.txt.";
        error += " Please make sure that smtp_info.txt is EXACTLY 3 lines.\n";
        throw std::runtime_error(error);
    }

    // Handle any issues regarding invalid email addresses/password.
    // This doesn't make sure if the email address even exists, so
    // its up to you to do so.
    std::regex test;
    bool check_info = verify_username(test, smtp_us);
    check_credentials(check_info, "The email address you're using "
                                  "to send email to is not valid.");

    check_info &= verify_password(test, smtp_pass);
    check_credentials(check_info, "The password is not valid for some reason.");

    check_info &= verify_username(test, reciv_us);
    check_credentials(check_info, "The email address you're using "
                                  "to receive email from the project is not valid.");

    smtp_username = smtp_us;
    smtp_password = smtp_pass;
    smtp_receiver_address = reciv_us;


}

/**
 * Retrieve the SMTP credentials from the project's settings file. If this cannot
 * be done, a runtime exception will be thrown.
 * @param smtp_info A map object containing the project settings
 */
void get_smtp_info_from_xml(const std::map<std::string, std::string> &smtp_info) {
	std::string smtp_us = smtp_info.at("sender_email");
	std::string smtp_pass = smtp_info.at("sender_password");
	std::string reciv_us = smtp_info.at("receiver_email");

	// Handle any issues regarding invalid email addresses/password.
	// This doesn't make sure if the email address even exists, so
	// its up to you to do so.
	std::regex test;
	bool check_info = verify_username(test, smtp_us);
	check_credentials(check_info, "The email address you're using "
								  "to send email to is not valid.");

	check_info &= verify_password(test, smtp_pass);
	check_credentials(check_info, "The password is not valid for some reason.");

	check_info &= verify_username(test, reciv_us);
	check_credentials(check_info, "The email address you're using "
								  "to receive email from the project is not valid.");

	// If everything goes well, copy the data.
	smtp_username = smtp_us;
	smtp_password = smtp_pass;
	smtp_receiver_address = reciv_us;
}
