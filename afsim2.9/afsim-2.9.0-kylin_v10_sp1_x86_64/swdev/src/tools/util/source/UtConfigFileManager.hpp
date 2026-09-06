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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

// UtConfigFileManager.h - interface for the UtConfigFileManager class
//  This class handles reading and writing to a configuration file.
/////////////////////////////////////////////////////////////////////////

#ifndef UTCONFIGFILEMANAGER_HPP
#define UTCONFIGFILEMANAGER_HPP

#include "ut_export.h"

#include <string>

class UT_EXPORT UtConfigFileManager
{
public:
   ////////////////////////////////////////////////////////////////////////////////////
   // This constructor opens a file of the given name in the homedirectory for config reading
   ////////////////////////////////////////////////////////////////////////////////////
   UtConfigFileManager(const char* homedirectory, const char* filename = "cme_cfg");

   //////////////////////////////////////////////////////////////////////////////////////////////
   // The destructor closes the file if it is open
   //////////////////////////////////////////////////////////////////////////////////////////////
   ~UtConfigFileManager() = default;

   //////////////////////////////////////////////////////////////////////////////////
   // This method puts the char string value of the requested category/key in value from the config
   // file
   //////////////////////////////////////////////////////////////////////////////////
   const char* GetConfigValue(const char* category, const char* key);

   ////////////////////////////////////////////////////////////////////////////////
   // This method sets the given category/key combination to value in the configuration file
   ////////////////////////////////////////////////////////////////////////////////
   void SetConfigValue(const char* category, const char* key, const char* value);

   /////////////////////////////////////////////////////////////////////////////////////////////
   // This method compares the category/key's value from the configuration file with the given
   // value.  It returns true is they are the same, false otherwise
   /////////////////////////////////////////////////////////////////////////////////////////////
   bool CompareConfigValue(const char* category, const char* key, const char* value);

private:
   std::string mHomeDirectory;
   std::string mFilename;
   std::string mWholeFilename;
   std::string mLastValue;
};

#endif
