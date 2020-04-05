cmake_minimum_required(VERSION 3.15)
project(CSC4110_Project VERSION 0.12)


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
            " If on Linux, please install libpqxx through your package manager"
            " (apt, yum, etc). If on macOS, use homebrew to install \"libpqxx\"."
            "You must also have postgresql installed on your system.")
endif()

# Set C and C++ Standards
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_C_STANDARD 11)

# Add SMTP Library
add_library(SMTP ./smtp/SMTPMail.cpp)
#add_library(Sensors ./sensors/Float_Sensor.cc)
#Add Subdirectories



#add_executable(CSC4110_Project main.cpp)

add_executable(CSC4110_Project main.cpp ./sensors/Float_Sensor.cc log/log.cpp)

target_link_libraries(CSC4110_Project PUBLIC SMTP m pqxx pq)
target_include_directories(CSC4110_Project PUBLIC
        "${PROJECT_BINARY_DIR}")
