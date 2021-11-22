/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 01/07/2021 at 02:54 PM
 *
 * Project_Base.h
 * In order to prevent libraries from being included multiple times, I'd like
 * to just make a header simply handles the most basic libraries that
 * are to be used by the project.
 * -----------------------------------------------------------------------------
 */
#pragma once
#ifndef PROJECT_BASE_H
#define PROJECT_BASE_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <regex>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include "DebugLog.h"

// Common Macros
//#define MAX_SECONDS_IN_DAY (86400)
const int32_t MAX_SECONDS_IN_DAY = 86400;
#endif
