// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UtoPathParser_hpp
#define UtoPathParser_hpp

//----------------------------------------------------------------
// The UtoPathParser provides a set of static helper methods that
// are convienent for parsing file paths.
//----------------------------------------------------------------

#include <string>

class UtoPathParser
{
public:
   // Returns the full path up to, but not including the /filename.
   // Additonally, if a path is passed to GetPath one level will
   //  be stripped off.
   // Returns an empty string if no path exists.
   // aFile is the full path with or without the filename.
   static std::string GetPath(const std::string& aFile);

   // Returns the filename without the path and file extension.
   // Returns an empty string if no filename exists.
   // aFile is the full path with filename.
   static std::string GetName(const std::string& aFile);

   // Returns the file's extension.
   // Returns an empty string if no extension exists.
   // aFile is the full path with filename.
   static std::string GetExtension(const std::string& aFile);

   // Returns the filename with extension.
   // Returns an empty string if no filename and extension exist.
   // aFile is the full path with filename.
   static std::string GetNameAndExtension(const std::string& aFile);

   // Return true if the file exists, false otherwise.
   static bool FileExists(const std::string& aFile);

private:
   UtoPathParser(); // Prohibit use.
};

#endif
