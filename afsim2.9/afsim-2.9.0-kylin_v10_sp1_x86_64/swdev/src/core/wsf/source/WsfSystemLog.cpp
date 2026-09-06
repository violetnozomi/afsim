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

#include "WsfSystemLog.hpp"

#include <cctype>
#include <ctime>
#include <fstream>
#include <string>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtPath.hpp"
#include "WsfVersion.hpp"

namespace
{
bool sCreateDefaultLog = true;
}

// =================================================================================================
WsfSystemLog::WsfSystemLog()
   : mMutex()
   , mDefaultLogFilePtr(nullptr)
   , mLogFilePtr(nullptr)
{
   if (sCreateDefaultLog)
   {
      std::string    defaultLogName = UtPath::GetExePath().GetFileName(false) + ".log";
      std::ofstream* defaultFilePtr = new std::ofstream(defaultLogName.c_str());
      if (defaultFilePtr->is_open())
      {
         mDefaultLogFilePtr = defaultFilePtr;
      }
      else
      {
         delete defaultFilePtr;
      }
   }
}

// =================================================================================================
WsfSystemLog::~WsfSystemLog()
{
   CloseLogFile();
   delete mDefaultLogFilePtr;
}

// =================================================================================================
//! Returns 'true' if logging is active (i.e.: The log file is open).
//! This can be used by callers to avoid (the potentially expensive) formatting a log file message
//! when no logging is being performed.
bool WsfSystemLog::LoggingActive()
{
   return mLogFilePtr || mDefaultLogFilePtr;
}

// =================================================================================================
bool WsfSystemLog::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "log")
   {
      std::string logData;
      aInput.ReadLine(logData, false);
      WriteLogEntry(logData);
   }
   else if (command == "log_file")
   {
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName = aInput.SubstitutePathVariables(fileName);
      if (OpenLogFile(fileName))
      {
         // Write a log entry indicating the WSF version
         WriteLogEntry(std::string("version wsf ").append(wsf::version::GetString()));
         // Write a log entry to indicate which file this came from.
         WriteLogEntry("file " + aInput.GetCurrentFileName());
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
bool WsfSystemLog::OpenLogFile(const std::string& aFileName)
{
   CloseLogFile();

   mLogFilePtr = new std::ofstream(aFileName.c_str());
   if (!mLogFilePtr->fail() && mLogFilePtr->is_open())
   {
      // success
   }
   else
   {
      { // RAII block
         auto out = ut::log::error() << "Unable to open system log file.";
         out.AddNote() << "File: " << aFileName;
      }
      delete mLogFilePtr;
      mLogFilePtr = nullptr;
   }
   return mLogFilePtr != nullptr;
}

// =================================================================================================
void WsfSystemLog::CloseLogFile()
{
   if (mLogFilePtr != nullptr)
   {
      delete mLogFilePtr;
      mLogFilePtr = nullptr;
   }
}

// =================================================================================================
void WsfSystemLog::WriteLogEntry(const std::string& aLogEntry)
{
   if (LoggingActive())
   {
      std::lock_guard<std::mutex> lock(mMutex);
      for (std::string::size_type i = 0; i < aLogEntry.size(); ++i)
      {
         if (!isspace(aLogEntry[i]))
         {
            time_t utcTime;
            time(&utcTime);
            struct tm* localTimePtr = localtime(&utcTime);
            char       timeStr[32];
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTimePtr);
            if (mLogFilePtr != nullptr)
            {
               (*mLogFilePtr) << timeStr << ' ' << aLogEntry.substr(i) << std::endl;
            }
            if (mDefaultLogFilePtr != nullptr)
            {
               (*mDefaultLogFilePtr) << timeStr << ' ' << aLogEntry.substr(i) << std::endl;
            }
            return;
         }
      }
   }
}

// =================================================================================================
void WsfSystemLog::WriteOutputLogEntry(const std::string& aOutputFileType, const std::string& aOutputFilePath)
{
   WriteLogEntry(aOutputFileType + " output file: " + aOutputFilePath);
}

// =================================================================================================
//! Does not create a log file by default.  Must be called prior to WsfApplication creation
void WsfSystemLog::DisableDefaultLogFile()
{
   sCreateDefaultLog = false;
}
