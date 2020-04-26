/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/05/2020 at 06:16 PM
 * 
 * log.cpp
 * 
 * -----------------------------------------------------------------------------
 */

#include "./Log.h"


//------------------------------------------------------------------------------
// Log Class Definitions
//
//
//------------------------------------------------------------------------------

Log::Log(map<string, bool> &sensor, map<string, double> &levl,
	 string comm, string time_stamp) {
    // Maps should be the same size as default. Error otherwise.
    if ((this->sensor_check.size() != sensor.size()) ||
        (this->level.size() != levl.size())){
        ostringstream os;
        os << "Passed sensor and level arguments have different sizes"
           << " from the default maps in Log. Aborting.";
        throw runtime_error(os.str());
    }
    this->sensor_check = sensor;
    this->level = levl;
    this->comment = std::move(comm);
    this->time_stamp = std::move(time_stamp);

}

// constructor
Log::Log(pqxx::row &row){ // Extract the contents of row and fill the log.
    raw_data = &row;
    this->extract_row();
}

// Copy Constructor
Log::Log(const Log &l){
    this->comment = l.comment;
    this->time_stamp = l.time_stamp;
    this->date = l.date;
    this->sensor_check = l.sensor_check;
    this->level = l.level;
    this->raw_data = l.raw_data;

}

// Copy assignment
Log& Log::operator=(const Log &l){
    // Only copy the raw data.
    comment = l.comment;
    time_stamp = l.time_stamp;
    date = l.date;
    sensor_check = l.sensor_check;
    level = l.level;
    raw_data = l.raw_data;

    return *this;
}

// Comparison Operator:
bool Log::operator==(const Log &b){
    return (comment == b.comment) && (time_stamp == b.time_stamp)
	&& (date == b.date) && (sensor_check == b.sensor_check)
	&& (level == b.level) && (raw_data == b.raw_data);

}
//------------------------------------------------------------------------------
// extract_row() -- extracts the appropriate fields from raw_data.
//------------------------------------------------------------------------------
bool Log::extract_row() {
    if (!raw_data){ // Prevent access of empty raw_data
	cerr << "Warning: Log does not point to any row.\n";
        return false;
    }

    // First, fill the date and timestamp.
    date = (raw_data->begin() + 1)->as(string{});
    auto it = raw_data->begin() + (log_offset - 1);
    time_stamp = it->as(string{});

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
        comment = it->as(string{});
    }

    return true;


}
//------------------------------------------------------------------------------
// extract_row_to_log - For any row
//------------------------------------------------------------------------------
void extract_row_to_log(Log &l, pqxx::row &row){
    if (l.row() == row){
        l.extract_row();
        return;
    }

    l.row() = row;
    l.extract_row();

}

//------------------------------------------------------------------------------
// operator<< - outputs all fields from log(except raw_data) to a ostream.
//------------------------------------------------------------------------------
ostream& operator<<(ostream &os, const Log &l){
    string html_break = "<br>";
    ostringstream o;
    o << "Date: " << l.date << html_break << endl;
    o << "Timestamp: " << l.time_stamp << html_break << endl;

    for (auto & i : l.sensor_check){
        o << i.first << ": ";
        o << ((i.second) ? "Online" : "Offline");
        o << html_break << endl;
    }

    // Battery level (In Percentage)
    double d = l.level.at("battery_level");
    o << "Battery Level: " << d << "%" << html_break << endl;
    d = l.level.at("rain_level");
    o << "Rain level: " << d << " in (" << inches_to_cm(d) << " cm)"
      << html_break << endl;
    o << "Comment: " << l.comment << html_break << endl;
    return os << o.str();

}

//------------------------------------------------------------------------------
// Database log functions
//
//
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// add_log(): Add a Log to a database.
//------------------------------------------------------------------------------
void add_log(pqxx::transaction_base &trans, const Log &l){
    ostringstream os;
    os << boolalpha;
    
    os << "INSERT INTO log(float_sensor_connected, rain_sensor_connected,"
       << " solar_panel_functional, battery_connected, battery_level, rain_level, log_comment)"
       << " VALUES (" ;

    for (const auto & i : l.sensor_check){
        os << trans.quote(i.second) << ", ";
    }


    // Use check to determine when the close the parenthesises.
    auto check = l.level.begin();
    advance(check, 1);

    for (auto i = l.level.begin(); i != l.level.end(); i++){
        os << trans.quote(i->second) << ", ";
    }
    os << trans.quote(l.comment) << ")";

    string query = os.str();
    //cout << query;
    trans.exec0(query);
    
    // Now update the database.
    trans.commit();
}


//------------------------------------------------------------------------------
// SMTP Log Functions
//
//
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// system_all_to_string(const char * cmd):
// This function takes the result of a system call and stores it into a
// string. Shamelessly taken from Stackoverflow.
//------------------------------------------------------------------------------
std::string system_call_to_string(const char* cmd) {
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    // Strip the newline.
    result.pop_back();
    return result;
}


//------------------------------------------------------------------------------
// create_date() : Do a system call to get the current date in
// month day year hh::mm:ss 
//------------------------------------------------------------------------------
std::string create_date(void){
    string date_str = system_call_to_string("date \"+%a, %d %b %Y %k:%M:%S -0500\"");

    if (date_str.back() == '\r')
        date_str += '\n';

    return date_str;
}

//------------------------------------------------------------------------------
// create_smtp_html_header():
// Populates a vector of std::string containing a smtp header.
// I can't use create_smtp_text_header to send a html email, so this method
// has to be used instead.
//------------------------------------------------------------------------------
vector<std::string> create_html_header(string &message_type) {

    ostringstream os;
    // find the newline:
    std::string date_str = create_date();
    vector<std::string> header_data;
    header_data.push_back(std::string{"Date: " + date_str});
    header_data.push_back(std::string{"To: " + smtp_receiver_address});
    header_data.push_back(std::string{"From: " + smtp_username});
    header_data.push_back(std::string{"Subject: Rain Alert Report: " + message_type});

    return header_data;
}

//------------------------------------------------------------------------------
// string create_stmp_text_header(void)
// Creates the appropriate smtp header in preparation of sending an email.
// STMP Headers are structured as so:
// Date: \r\n
// To: \r\n
// From: \r\n
// Message-ID:
// Subject:
// \r\n
//------------------------------------------------------------------------------

std::string create_smtp_text_header(string &message_type) {
    const std::string end_line = "\r\n";
    std::string date_str = create_date();

    ostringstream os;

    os << "Date: " << ""
       << date_str << end_line
       << "To: " << smtp_receiver_address << end_line
       << "From: " << smtp_username << end_line
       << "Subject: " << "Rain Alert Report: " << message_type <<  end_line
       << end_line;

    return os.str();
}


void set_up_stmp_connection(CURL *curl) {
    // Set up username and password:
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtp_username.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, smtp_password.c_str());

    // Set URL of mailserver.
    string url = "smtp://" + smtp_address + ":" + smtp_port;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Use TLS
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long) CURLUSESSL_ALL);

    // Don't have certificate, so disable some protection.
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    // Mail FROM:
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, smtp_username.c_str());



}
//------------------------------------------------------------------------------
// send_html_through_SMTP():
// Write the following to the file in text_file_path:
// * SMTP Header Specifying the appropriate information
// * Output of Log
// This is then sent to the recipient address in smtp_recipient_address.
//------------------------------------------------------------------------------

void send_text_through_SMTP(ostringstream &oss, string &message_type) {

    ofstream ofs{text_file_path, ios_base::trunc};
    ofs << create_smtp_text_header(message_type);
    ofs << oss.str();

    // Now close and reopen file as FILE
    ofs.close();
    FILE *fs = fopen(text_file_path.c_str(), "r");

    if (!fs){
        throw runtime_error("Could not open " + text_file_path);
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
            cerr << "curl_easy_preform() failed : " << curl_easy_strerror(result)
                 << endl;

        curl_slist_free_all(recipients);

        // Always cleanup:
        curl_easy_cleanup(curl);
        fclose(fs);

    }
}


//------------------------------------------------------------------------------
// send_html_through_SMTP():
// Send inline html email to the recipient address specified in
// smtp_receiver_address.
//------------------------------------------------------------------------------
void send_html_through_SMTP(const Log &l, ostringstream &oss, string &message_type) {
    
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

        vector<std::string> header_list = create_html_header(message_type);
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
        string inline_html = oss.str();
        curl_mime_data(part, inline_html.c_str(), CURL_ZERO_TERMINATED);
        curl_mime_type(part, "text/html");

        /* Text message. */
        part = curl_mime_addpart(alt);
        ostringstream text_section;
        text_section << "In case the inline html does not display correctly,"
		     << "the text is displayed below:" << endl
		     << l << endl;

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
            cerr << "curl_easy_preform() failed : " << curl_easy_strerror(result)
                 << endl;

	curl_slist_free_all(recipients);
        curl_slist_free_all(headers);

        // Always cleanup:
        curl_easy_cleanup(curl);
        //fclose(fs);

        /* Free multipart message. */
        curl_mime_free(mime);
    }
    


}

void send_log_as_HTML(const Log &l, string &message_type){
    // Open the html template, append the log in it
    // and then pass it to the header.
    string smtp_endl = "\r\n";
    ifstream ifs{html_template_path};
    //fstream new_file{html_file_path, ios_base::trunc | ios_base::out };
    ostringstream new_file{};
    string check = "                <!--- INSERT MESSAGE :) -->";
    string temp;
    // Copy all data from html template to new file until the check is true.
    do{
        getline(ifs, temp);
        new_file << temp << smtp_endl;
    } while ((ifs.good() && temp != check));

    if (temp != check){
	string error = "I couldn't insert the log into " + html_template_path;
	error += "Make sure that the check comment is included in the file.\n";
        throw runtime_error(error);
    }
    
    // <!--- INSERT MESSAGE :) -->
    // Insert header
    new_file << "<h1> Pi Rain Alert: " << message_type << " for "
             << system_call_to_string("date \"+%d %b %Y\"") << "</h1>" << endl;
    new_file << "<p>" << smtp_endl;

    // Make sure to

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
    //ifstream send_file{html_file_path, ios_base::in};
    send_html_through_SMTP(l, new_file, message_type);
}

void send_log_as_text(const Log &l, string &message_type) {
    ostringstream new_file{};
    new_file << l;
    send_text_through_SMTP(new_file, message_type);

}
//------------------------------------------------------------------------------
// verify_user_name() : Very basic function to determine if a
// user_name is valid by using regex expression.
// Username should follow name@domain_name.domain.
//------------------------------------------------------------------------------

bool verify_username(std::regex &test, const string &user_name) {
    string regex_str = R"(^[A-z][\w|\.]*@\w+\.+[\w | .]*[a-zA-Z]$)";
    // Use regex expressions to check for valid user_name.
    // ^[A-z][\w|\.]*@\w+\.+[\w | .]*[a-zA-Z]$
    test = std::regex{regex_str};
    return regex_match(user_name,test);

}


//------------------------------------------------------------------------------
// verify_password(): Very basic function to verify a password.
// The minimum size of the password is 6 characters, but the user can
// specify the minimum amount (up to a max of 128 characters)
//------------------------------------------------------------------------------

bool verify_password(std::regex &test, const string &password) {
    // Password should at least be 6 characters.
    string regex_str = "^[\\w | \\.]{6,}$";
    test = std::regex{regex_str};
    return regex_match(password, test);
}

[[maybe_unused]] bool verify_password(std::regex &test, int8_t &size, const string &password) {
    ostringstream os;
    os << R"("^[\\w | \\.]{)" << size << ",}$";
    test = std::regex{os.str()};
    return regex_match(password, test);
}
//------------------------------------------------------------------------------
// Creck_credentials : Quick check of booleans for SMTP reading
// This mainly exists in order to make get_smtp_credentials() shorter and
// to better describe error messages.
//------------------------------------------------------------------------------
inline void check_credentials(bool check, string error) {
    if (!check)
        throw runtime_error(error);
}

//------------------------------------------------------------------------------
// get_smtp_credentials(): Read the smtp credentials from log/smtp_info.txt
// Please place your info in that file. For security reasons, I won't provide
// mine. The file should have exactly three lines:
// Line 1: sender email address
// Line 2: sender email password
// Line 3: recipient email address
//------------------------------------------------------------------------------

void get_smtp_credentials(void){
    // Read the data from
    ifstream file{smtp_file_path};
    if (!file){
	    throw runtime_error("Cannot retrieve the SMTP credentials from " + smtp_file_path);
    }

    // Now check if the file is empty.
    if (file.peek() == std::ifstream::traits_type::eof()){
        throw runtime_error("smtp_info.txt is emtpy. Change that.");
    }

    // Retrieve the credentials and assign them
    string smtp_us, smtp_pass, reciv_us;
    getline(file, smtp_us);
    getline(file, smtp_pass);
    getline(file, reciv_us);

    auto file_status = file.rdstate();

    // The file should ONLY have the eof flag enabled after reading the file to the end.
    // If it doesn't meet those requirements, throw an error.
    if (file_status != ios_base::eofbit){
        string error = "Could not populate all variables from smtp_info.txt.";
        error += " Please make sure that smtp_info.txt is EXACTLY 3 lines.\n";
        throw runtime_error(error);
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
