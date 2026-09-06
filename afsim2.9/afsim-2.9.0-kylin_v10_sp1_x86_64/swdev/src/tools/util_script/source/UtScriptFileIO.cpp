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

#include "UtScriptFileIO.hpp"

#include <fstream>
#include <streambuf>
#include <string>

#include "UtFileStream.hpp"
#include "UtLog.hpp"
#include "UtPath.hpp"
#include "UtScriptMethodDefine.hpp"
namespace
{
bool OpenHelper(UtFileStream* aFileStreamPtr, const std::string& aFileName, const std::string& aMode = std::string{})
{
   // Clear the failbit if it was previously set
   if (aFileStreamPtr->stream())
   {
      aFileStreamPtr->stream()->clear();
   }

   try
   {
      if (aMode.empty() || aMode == "in")
      {
         aFileStreamPtr->open(aFileName.c_str(), std::ios::in);
      }
      else if (aMode == "out")
      {
         aFileStreamPtr->open(aFileName.c_str(), std::ios::out);
      }
      else if (aMode == "app" || aMode == "append")
      {
         aFileStreamPtr->open(aFileName.c_str(), std::ios::app | std::ios::out);
      }
      else
      {
         aFileStreamPtr->open(aFileName.c_str(), std::ios::in);
      }
   }
   catch (std::fstream::failure&)
   {
      auto out = ut::log::error() << "File does not exist (or, cannot open).";
      out.AddNote() << "File: " << aFileName;
   }

   return aFileStreamPtr->stream() && aFileStreamPtr->stream()->is_open();
}
} // namespace

UtScriptFileIO::UtScriptFileIO(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("FileIO", aScriptTypesPtr)
{
   UT_SCRIPT_WRAP_CLASS(UtFileStream);

   mConstructible = true;

   UT_SCRIPT_METHOD(bool, Open, (const std::string& aFile)) { return OpenHelper(&self, aFile); };

   UT_SCRIPT_METHOD(bool, Open, (const std::string& aFile, const std::string& aMode))
   {
      return OpenHelper(&self, aFile, aMode);
   };

   UT_SCRIPT_METHOD(bool, Open, (const UtPath& aPath)) { return OpenHelper(&self, aPath.GetSystemPath()); };

   UT_SCRIPT_METHOD(bool, Open, (const UtPath& aPath, const std::string& aMode))
   {
      return OpenHelper(&self, aPath.GetSystemPath(), aMode);
   };
   UT_SCRIPT_WRAP_MEMBER("Close", close);

   UT_SCRIPT_METHOD(bool, Eof, ()) { return !self.stream() || self.stream()->eof(); };

   UT_SCRIPT_METHOD(std::string, Mode, ())
   {
      auto        mode     = self.GetOpenMode();
      std::string modeName = "none";

      if (mode == std::ios_base::in)
      {
         modeName = "in";
      }
      else if (mode == std::ios_base::out)
      {
         modeName = "out";
      }
      else if (mode == (std::ios_base::app | std::ios_base::out))
      {
         modeName = "append";
      }

      return modeName;
   };

   UT_SCRIPT_METHOD(UtPath, Path, ()) { return UtPath{self.GetFilePath()}; };

   UT_SCRIPT_METHOD(std::string, ReadAll, ())
   {
      if (self.stream() &&                                          // stream is open
          self.GetOpenMode() & (std::ios_base::app | std::ios::in)) // can read from it.
      {
         std::istreambuf_iterator<char> eos;
         return std::string(std::istreambuf_iterator<char>(*self.stream()), eos);
      }
      return "";
   };

   // static -- Read entire file into string
   UT_SCRIPT_STATIC_METHOD(std::string, ReadAll, (const std::string& aFileName))
   {
      std::ifstream infile(aFileName);
      if (infile)
      {
         std::istreambuf_iterator<char> eos;
         return std::string(std::istreambuf_iterator<char>(infile), eos);
      }
      return "";
   };

   UT_SCRIPT_METHOD(std::string, Readln, ())
   {
      std::string str = "";
      if (self.stream() &&                            // stream is open
          (self.GetOpenMode() & (std::ios_base::in))) // can read from it.))
      {
         std::getline(*self.stream(), str);
      }
      return str;
   };

   UT_SCRIPT_METHOD(std::string, ReadToken, ())
   {
      std::string str = "";
      if (self.stream() &&                            // stream is open
          (self.GetOpenMode() & (std::ios_base::in))) // can read from it.)
      {
         (*self.stream()) >> str;
      }
      return str;
   };

   UT_SCRIPT_METHOD(void, Write, (const std::string& output))
   {
      if (self.stream() &&                                             // stream is open
          (self.GetOpenMode() & (std::ios_base::app | std::ios::out))) // can write to it.
      {
         *self.stream() << output;
      }
   };

   UT_SCRIPT_METHOD(void, Writeln, (const std::string& output))
   {
      if (self.stream() &&                                             // stream is open
          (self.GetOpenMode() & (std::ios_base::app | std::ios::out))) // can write to it.
      {
         *self.stream() << output << std::endl;
      }
   };
}

void* UtScriptFileIO::Create(const UtScriptContext& aInstance)
{
   return new UtFileStream();
}

void UtScriptFileIO::Destroy(void* aObject)
{
   delete static_cast<UtFileStream*>(aObject);
}
