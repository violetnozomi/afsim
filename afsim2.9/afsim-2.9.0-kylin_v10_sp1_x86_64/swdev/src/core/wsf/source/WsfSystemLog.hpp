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

#ifndef WSFSYSTEMLOG_HPP
#define WSFSYSTEMLOG_HPP

#include "wsf_export.h"

#include <iosfwd>
#include <mutex>
#include <string>

class UtInput;

//! A class for maintaining a log of system-related information.
//! This is typically used to maintain an audit trail of files that were used
//! in the simulation.

class WSF_EXPORT WsfSystemLog
{
public:
   WsfSystemLog();
   ~WsfSystemLog();

   static void DisableDefaultLogFile();

   bool LoggingActive();
   bool ProcessInput(UtInput& aInput);
   bool OpenLogFile(const std::string& aFileName);
   void WriteLogEntry(const std::string& aLogEntry);
   void WriteOutputLogEntry(const std::string& aOutputFileType, const std::string& aOutputFilePath);

private:
   void          CloseLogFile();
   WsfSystemLog& operator=(const WsfSystemLog&) = delete;
   WsfSystemLog(WsfSystemLog& aSrc);

   std::mutex mMutex;

   std::ofstream* mDefaultLogFilePtr;
   std::ofstream* mLogFilePtr;
};

#endif
