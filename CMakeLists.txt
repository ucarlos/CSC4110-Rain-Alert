cmake_minimum_required(VERSION 3.15)
project(CSC4110_Project VERSION 0.60)


# Add version number to program
configure_file(./include/Project_Version.h.in ./include/Project_Version.h)

# Set module path:
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-build-debug/modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
# First, We need to know what must be installed first.
# Check for libpqcxx (Using version 7)

find_library(PSQL_CXX pqxx)
if(NOT PSQL_CXX)
    message(FATAL_ERROR "\"libpqxx\" could not be found on your system."
            " If on Linux, please install libpqxx through your package manager."
            "You must also have postgresql installed on your system.")
endif()

find_library(CURL_CHECK curl)
if (NOT CURL_CHECK)
    message(FATAL_ERROR "\"libcurl\" could not be found on your system."
            "Please install it using your package manager or compile from source.")
endif()

find_file(BOOST_CHECK boost)
if (NOT BOOST_CHECK)
    message(FATAL_ERROR "\"libboost\" could not be found on your system."
            "Please install it using your package manager or compile from source."
            "This is required for XML parsing.")
endif()

find_library(NEWT_CHECK newt)
if (NOT NEWT_CHECK)
    set(NEWT_GUI FALSE)
else()
    set(NEWT_GUI TRUE)
endif()


# Set C and C++ Standards
set(CMAKE_CXX_STANDARD 17)

# Add SMTP Library
#add_library(SMTP ./smtp/SMTPMail.cpp ./smtp/smtp.c)
add_library(Connection sql/PSQL_Connection.cc)
add_library(Log log/Log.cc)


# Set as false if you can get output from the sensors.
# Otherwise use a random-number generator to provide values for both
# the float sensor and the rain guage.
set(SENSORS_OUTPUT_IS_RNG TRUE)

if (SENSORS_OUTPUT_IS_RNG)
    add_library(Sensors ./sensors/Alternative_Sensor.cc)
else()
    add_library(Sensors ./sensors/Float_Sensor.cc ./sensors/Check_Sensors.cc ./sensors/Rain_Gauge.cc)
endif()

add_library(Configuration XML_Configuration.cc)
#Add Subdirectories

#Test with coverage:
set(NEWT_GUI FALSE)
if (NEWT_GUI AND NEWT_CHECK)
    message("Using the Newt UI for the Project.")
    add_executable(CSC4110_Project Main.cc Newt_Menu.cc "Debug Functions.cc"
            ./Sensor_Date.cc ./Sensor_Tracking.cc  log/Log.cc sql/PSQL_Connection.cc)

    target_link_libraries(CSC4110_Project PUBLIC curl m pqxx pq ssl crypto
            pthread Log Connection ncurses newt Sensors Configuration)
else()
    message("Using the Default Basic Menu UI for the Project.")
    add_executable(CSC4110_Project Main.cc Basic_Menu.cc ./Sensor_Tracking.cc "Debug Functions.cc"
            ./Sensor_Date.cc  log/Log.cc sql/PSQL_Connection.cc)

    target_link_libraries(CSC4110_Project PUBLIC curl m pqxx pq
            ssl crypto pthread Log Connection Sensors Configuration)
endif()

target_include_directories(CSC4110_Project PUBLIC
        "${PROJECT_BINARY_DIR}")