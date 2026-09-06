// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "GenTime.hpp"

#include <string.h>
#include <time.h>

GenTime::GenTime() {}

GenTime::~GenTime() {}

// Static method
std::string GenTime::GetWallClockTime()
{
   time_t theTime;
   char   timeBuff[26];
   char   tempyTime[8];

   // Get the time value
   time(&theTime);

#ifdef _WIN32

   // Convert the time value to a string
   ctime_s(timeBuff, 26, &theTime);

#else

   // Convert the time value to a string
   ctime_r(&theTime, timeBuff);

#endif

   // Save the time
   memcpy(tempyTime, &timeBuff[11], 8);

   // Move the year by the date
   memcpy(&timeBuff[11], &timeBuff[20], 4);

   // Put the time back, but at the end
   memcpy(&timeBuff[17], tempyTime, 8);

   // Put a comma and a space after the year
   timeBuff[15] = ',';
   timeBuff[16] = ' ';

   // Want to be thread safe, so return by value
   return std::string(timeBuff);
}
