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

#include "UtoPathParser.hpp"

#include <fstream>

#include "UtLog.hpp"

std::string UtoPathParser::GetPath(const std::string& aFile)
{
   // Strip off the name and extension and return the path.
   size_t index = aFile.find_last_of("/\\");

   if (index != std::string::npos)
   {
      // If there is a filename, strip if off.
      if (index + 1 < aFile.length())
      {
         return aFile.substr(0, index);
      }
      // If there is no filename, strip off one directory level.
      else
      {
         index = aFile.find_last_of("/\\", index - 1);

         if (index != std::string::npos)
         {
            return aFile.substr(0, index);
         }
      }
   }

   return "./";
}

std::string UtoPathParser::GetName(const std::string& aFile)
{
   std::string filename = GetNameAndExtension(aFile);

   // Strip off the extension and return the name.
   size_t index = filename.find_last_of(".");

   if (index != std::string::npos)
   {
      return filename.substr(0, index);
   }

   return filename;
}

std::string UtoPathParser::GetExtension(const std::string& aFile)
{
   std::string filename = GetNameAndExtension(aFile);

   // Strip off the name and return the extension.
   size_t index = filename.find_first_of(".");

   if (index != std::string::npos)
   {
      return filename.substr(index);
   }

   return "";
}

std::string UtoPathParser::GetNameAndExtension(const std::string& aFile)
{
   // Strip off the path and return the name and extension.
   size_t index = aFile.find_last_of("/\\");

   if (index != std::string::npos)
   {
      return aFile.substr(index + 1);
   }

   return aFile;
}

bool UtoPathParser::FileExists(const std::string& aFile)
{
   std::ifstream fin(aFile.c_str(), std::ios::in);

   if (fin)
   {
      fin.close();
      return true;
   }

   return false;
}

//#define TEST_UT_PATH_PARSER

#ifdef TEST_UT_PATH_PARSER
int main(int argc, char* argv[])
{
   int index = 0;

   if (argc > 1)
   {
      index = 1;
   }

   string path = UtoPathParser::GetPath(argv[index]);
   auto   out  = ut::log::info() << "TEST_UT_PATH_PARSER:";
   while (path != "/" && path != "")
   {
      out.AddNote() << "Path: " << path;
      path = UtoPathParser::GetPath(path);
   }

   out.AddNote() << "File: " << UtoPathParser::GetNameAndExtension(argv[index]);

   out.AddNote() << "Name: " << UtoPathParser::GetName(argv[index]);

   out.AddNote() << "Extension: " << UtoPathParser::GetExtension(argv[index]);

   return 0;
}
#endif
