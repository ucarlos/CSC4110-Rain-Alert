/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/01/2020 at 11:37 PM
 * 
 * Project.h
 * 
 * -----------------------------------------------------------------------------
 */

#ifndef CSC4110_PROJECT
#define CSC4110_PROJECT
// Standard Library Headers
#include <iostream>
using namespace std;

// Log Library
#include "./log/Log.h"

// Connection Library
#include "./sql/Connection.h"
// Project version number
#include "./include/Project_Version.h"

//------------------------------------------------------------------------------
// Menu Functions:
//------------------------------------------------------------------------------
void show_status(void);
void search_logs(void);
void test_sensors(void);
void database_options(void);
void email_options(void);


//------------------------------------------------------------------------------
// Testing Functions: For debugging purposes.
//------------------------------------------------------------------------------
void test_smtp(void);


// Any templates/ Classes?


#endif
