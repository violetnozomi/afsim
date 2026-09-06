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

// UtConfigFileManager.h - implementation for the UtConfigFileManager class
//  Config File assumptions, categories are unindented.
//  Keys are indented by a single tab '/t'
//  Only '=' and spaces (' ') may seperate a key from its value
//  All methods are case sensative.
/////////////////////////////////////////////////////////////////////////

#include "UtConfigFileManager.hpp"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

#include "UtLog.hpp"

////////////////////////////////////////////////////////////////////////////////////
// This constructor opens a file of the given name in the homedirectory for config reading
////////////////////////////////////////////////////////////////////////////////////
UtConfigFileManager::UtConfigFileManager(const char* homedirectory, const char* filename)
   : mHomeDirectory(homedirectory)
{
#if defined(_WIN32)
   mFilename = filename;
   mFilename += ".ini";
#else
   mFilename = ".";
   mFilename += filename;
#endif // defined( _WIN32 )

   mWholeFilename = homedirectory;
   mWholeFilename += "/" + mFilename;
}

//////////////////////////////////////////////////////////////////////////////////
// This method puts the char string value of the requested category/key in value
// from the config file
//////////////////////////////////////////////////////////////////////////////////
const char* UtConfigFileManager::GetConfigValue(const char* category, const char* key)
{
   char         line[1024];
   int          underheading = 0;
   int          place;
   unsigned int i;

   std::ifstream configFileIn(mWholeFilename.c_str(), std::ios::in);

   if (!configFileIn)
   {
      return nullptr;
   }

   configFileIn.seekg(0, std::ios_base::beg);


   while ((!(configFileIn.eof())) && (configFileIn.getline(line, 1024)))
   {
      if (underheading != 0)
      {
         if (line[0] == '[')
         {
            underheading = 0;
         }
         if (0 == strncmp(&(line[1]), key, strlen(key)))
         {
            place = static_cast<int>(strlen(key)) + 1;
            while ((line[place] == ' ') || (line[place] == '='))
            {
               ++place;
               if (place > (int)strlen(line))
               {
                  configFileIn.close();
                  return nullptr; // config file is badly formated
               }
            }
            mLastValue.assign(&(line[place]));
            for (i = 0; i < mLastValue.length(); ++i)
            {
               if (mLastValue[i] == '\n')
               {
                  mLastValue.replace(i, 1, "\0");
               }
            }
            configFileIn.close();
            return mLastValue.c_str();
         }
      }
      else
      {
         if (0 == strncmp(&(line[1]), category, strlen(category)))
         {
            underheading = 1;
         }
      }
   }
   configFileIn.close();
   return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// This method sets the given category/key combination to value in the configuration file
////////////////////////////////////////////////////////////////////////////////
void UtConfigFileManager::SetConfigValue(const char* category, const char* key, const char* value)
{
   std::string tempfilename;
   std::string strcategory = "[";
   strcategory += category;
   strcategory += "]";
   std::string strkey = "\t";
   strkey += key;
   strkey += " = ";

   std::ifstream configFileIn(mWholeFilename.c_str(), std::ios::in);

   if (configFileIn.is_open() && configFileIn.good()) // if file exists
   {
      configFileIn.seekg(0, std::ios_base::beg);
      tempfilename = mHomeDirectory;
      tempfilename += "/temporaryFile.deleteme";
      std::ofstream output(tempfilename.c_str(), std::ios::out);
      if ((!output.is_open()) || output.fail())
      {
         auto out = ut::log::error() << "Could not write to temporary file.";
         out.AddNote() << "File: " << tempfilename;
         return;
      }

      char line[1024];
      int  inHeader = 0;
      int  written  = 0;
      while ((!(configFileIn.eof())) && (configFileIn.getline(line, 1024)))
      {
         int donotwriteline = 0;
         if (inHeader == 1)
         {
            if (line[0] == '[')
            {
               if (written == 0)
               {
                  std::string temp = "\t";
                  temp += key;
                  temp += " = ";
                  temp += value;
                  temp += "\n";

                  output << temp;
                  written = 1;
               }
               inHeader = 0;
            }
            else if (0 == strncmp(line, strkey.c_str(), strkey.length()))
            {
               std::string temp = "\t";
               temp += key;
               temp += " = ";
               temp += value;
               temp += "\n";

               output << temp;
               donotwriteline = 1;
               written        = 1;
            }
         }
         else // not in header
         {
            if (0 == strncmp(line, strcategory.c_str(), strcategory.length()))
            {
               inHeader = 1;
            }
         }

         if (donotwriteline == 0)
         {
            output << line << std::endl;
         }
         else
         {
            donotwriteline = 0;
         }
      }

      if (written == 0)
      {
         if (inHeader == 0)
         {
            strcpy(line, "\0");
            sprintf(line, "[%s]\n", category);
            output << line;
         }
         strcpy(line, "\0");
         sprintf(line, "\t%s = %s\n", key, value);
         output << line;
         // written = 1;
      }
      // read and write stuff

      configFileIn.close();
      output.close();

      remove(mWholeFilename.c_str());
      rename(tempfilename.c_str(), mWholeFilename.c_str()); // copy temporary file to filename's file
      remove(tempfilename.c_str());                         // destroy temporary file
   }
   else
   {
      std::ofstream output(mWholeFilename, std::ios::out);
      if ((!output.is_open()) || output.fail())
      {
         auto out = ut::log::error() << "Could not write to file.";
         out.AddNote() << "File: " << mWholeFilename;
         return;
      }
      output << "--------------------------------------------------------------\n";
      output << "--This file was created using the UtConfigFileManager class.--\n";
      output << "--Hand editing is not recommended, and may cause programs    --\n";
      output << "--using this file to malfunction.  If that cannot convince  --\n";
      output << "--you not to edit this file by hand, here are the rules:    --\n";
      output << "--                                                          --\n";
      output << "--All text is case sensitive.                               --\n";
      output << "--Categories are unindented and bracketed.                  --\n";
      output << "--Keys are indented by a single tab.                        --\n";
      output << "--Keys and values are only separated by = and spaces.       --\n";
      output << "--------------------------------------------------------------\n";
      output << "\n\n";
      output << "[" << category << "]" << std::endl << "\t" << key << " = " << value << std::endl;
      output.close();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////
// This method compares the category/key's value from the configuration file with the given
// value.  It returns true is they are the same, false otherwise
/////////////////////////////////////////////////////////////////////////////////////////////
bool UtConfigFileManager::CompareConfigValue(const char* category, const char* key, const char* value)
{
   const char* temp = GetConfigValue(category, key);
   if (temp == nullptr)
   {
      return false;
   }

   std::string infile = temp;
   if (infile == value)
   {
      return true;
   }
   return false;
}
