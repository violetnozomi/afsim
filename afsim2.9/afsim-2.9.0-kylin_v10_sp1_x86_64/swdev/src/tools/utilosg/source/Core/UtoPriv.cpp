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

/////////////////////////////////////////////////////////////////////////////
// UtoPriv.cpp : implementation file

#include <algorithm>
#include <cassert>
#include <fstream>
#include <sstream>

#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
// forcing value to bool 'true' or 'false', do you really want to add !!?
// bool b = !!function();
#pragma warning(disable : 4800)
#include <shlwapi.h>
#else
#include <unistd.h>
#endif

#include <exception>
#include <stdexcept>

#include "UtoEntity.hpp"
#include "UtoPathParser.hpp"
#include "UtoPriv.hpp"
#include "UtoShape.hpp"

const osg::Vec3 YawAxis   = osg::Vec3(0, 0, 1);
const osg::Vec3 PitchAxis = osg::Vec3(-1, 0, 0);
const osg::Vec3 RollAxis  = osg::Vec3(0, -1, 0);

std::string UtoFindFirstPath(const std::string& file_name, const std::string& search_path)
{
   if (file_name.empty())
      throw std::logic_error("Invalid Parameter");

   // first check if <file_name> is valid alone.
   if (!UtoPathIsRelative(file_name) && UtoPathParser::FileExists(file_name))
   {
      return file_name;
   }

   // check each path in <search_path>.
   std::string            path, temp, abs_path;
   std::string::size_type pos = 0;
   std::string::size_type len = search_path.find_first_of(UtoSearchDelimiter);

   while (pos != std::string::npos)
   {
      path.assign(search_path, pos, len - pos);

      StripLeadingTrailing(path);

      // append <file_name> to <path>, but only if <base> isn't empty
      if (UtoPathIsRelative(file_name))
      {
         if (path.empty())
            temp = file_name;
         else if (path == "\\") // avoid \\relative\file and searching the network
            temp = path + file_name;
         else
            temp = path + "/" + file_name;
      }
      else
      {
         temp = file_name;
      }

      // canonicalize the path string, removing all '.' and '..'
      UtoPathCanonicalize(abs_path, temp);

      if (UtoPathFileExists(abs_path))
      {
         return abs_path;
      }

      if (len != std::string::npos)
      {
         pos = len + 1;
         len = search_path.find_first_of(UtoSearchDelimiter, len + 1);
      }
      else
      {
         pos = len;
      }
   }

   // DEBUG(("%s file not found in search path, %s\n", file_name.c_str(),
   //    search_path.c_str()));

   return "";
}

void StripLeadingTrailing(std::string& str, const std::string& tok)
{
   const size_t len     = str.size();
   size_t       leading = 0;
   while (leading < len && tok.find(str[leading]) != tok.npos)
      ++leading;
   if (leading >= len)
   {
      // all whitespace or empty, return an empty string
      if (!str.empty())
         str.clear();
      return;
   }

   // If len is zero, trailing will be a big number, but that is already
   // taken care of by the previous check.
   size_t trailing = len - 1;
   while (tok.find(str[trailing]) != tok.npos)
      --trailing;

   std::string t = str.substr(leading, trailing - leading + 1);
   str.swap(t);
}

std::string UtoAbsolutePathOf(const std::string& path, const std::string& base)
{
   // not big on throwing exceptions to the runtime because no one is going to handle this
   if (path.empty() || base.empty())
      throw std::logic_error("Invalid Parameter");

   // expand the path string
   std::string file_name = UtoExpandEnvString(path);

   if (UtoPathIsRelative(file_name))
   {
      std::string temp;

      // append <path> (now file_name) to <base>, but only if <base> isn't empty
      if (base.empty())
         temp = file_name;
      else if (base == "\\") // avoid \\relative\file and searching the network
         temp = base + file_name;
      else
         temp = base + '/' + file_name;
      file_name = temp;
   }

   // if not a valid file or directory then return empty string
   if (!UtoPathFileExists(file_name))
   {
      return "";
   }
   // if we have a file name path, then strip the filename off
   else if (!UtoPathIsDirectory(file_name))
   {
      file_name = UtoPathParser::GetPath(file_name);
   }

   assert(UtoPathIsDirectory(file_name));

   return file_name;
}

std::string UtoExpandEnvString(const std::string& rString)
{
   // local(s)
   std::string sString = "";

   // windows specific
#ifdef _WIN32

   // buffer to hold the expanded string 10k of data
   // don't make this static since this function should only be called during
   // init
   char sBuffer[10240];

   // expand the environment variables
   ExpandEnvironmentStrings(rString.c_str(), sBuffer, sizeof(sBuffer) / sizeof(char) - 1);
   // set the string to the return string
   sString = sBuffer;

   // validate that there are no '%' in the string
   if (sString.find('%') != std::string::npos)
      sString = "";

#else

   // TODO - Figure out how to replicate the Windows ExpandEnvironmentStrings method.
   // For now just return the original string.
   sString = rString;

#endif

   return sString;
}

bool UtoPathCanonicalize(std::string& dst, const std::string& path)
{
   bool result;

   // windows specific
#ifdef _WIN32
   // buffer to hold the updated string
   char sPathCan[MAX_PATH];
   // canonicalize the string
   result = PathCanonicalize(sPathCan, path.c_str());
   // update the local string
   dst = sPathCan;
#else
   // TODO - Figure out how to replicate the Windows PathCanonicalize method.
   // For now just return the original string.
   dst    = path;
   result = true;
#endif

   return result;
}

bool UtoPathIsRelative(const std::string& path)
{
#ifdef _WIN32
   return PathIsRelative(path.c_str()) ? true : false;
#else
   return path.empty() || path[0] != '/';
#endif
}

bool UtoPathFileExists(const std::string& path)
{
   struct stat buf;
   return stat(path.c_str(), &buf) == 0;
}

bool UtoPathIsDirectory(const std::string& path)
{
   struct stat buf;
   if (stat(path.c_str(), &buf) == -1)
      return false;
   return buf.st_mode & S_IFDIR;
}
