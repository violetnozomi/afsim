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
#include "UtPath.hpp"

#include <cerrno>
#include <cstring>

#include "UtLog.hpp"

#ifdef _WIN32
#include <windows.h>

#include <direct.h>
#else

#include <climits>
#include <cstdlib>

#include <dirent.h>
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>

namespace
{

bool isSlash(char c)
{
   return c == '/' || c == '\\';
}

} // namespace

UtPath::UtPath(const std::string& aPath)
   : mPathString(aPath)
{
   MakeNormalizedPath(mPathString);
}

//! Rewrites the path with forward slashes, and
//! removes unneeded parts of the path
void UtPath::MakeNormalizedPath(std::string& aPath)
{
   std::string   prefix;
   SubstringList dirStack;
   ParsePathP(aPath, prefix, dirStack);
   CleanDirectoryStack(dirStack);
   aPath = BuildPath(prefix, dirStack);
}

//! Removes unneeded parts of the path
//! For instance:  /adir/../adir2    ==>   /adir2
//!                /adir/./adir2     ==>   /adir/adir2
//!                .                 ==>   .
void UtPath::CleanDirectoryStack(SubstringList& aDirStack)
{
   // Special case: Only reduce '.' when another component exists
   if (aDirStack.size() == 1 && aDirStack[0] == ".")
   {
      return;
   }

   for (size_t i = 0; i < aDirStack.size(); ++i)
   {
      if (aDirStack[i] == ".")
      {
         aDirStack.erase(aDirStack.begin() + i);
         --i;
      }
   }
   for (size_t i = 0; i + 1 < aDirStack.size(); ++i)
   {
      if (aDirStack[i + 1] == ".." && aDirStack[i] != ".." && aDirStack[i] != ".")
      {
         aDirStack.erase(aDirStack.begin() + i, aDirStack.begin() + i + 2);
         i = 0;
      }
   }
}

// static
//! Returns the path to the current working directory
UtPath UtPath::WorkingDirectory()
{
   const int cMAX_PATH_LENGTH(1024);
   char      buffer[cMAX_PATH_LENGTH];
#ifdef _WIN32
   auto result = _getcwd(buffer, cMAX_PATH_LENGTH);
#else
   auto   result        = getcwd(buffer, cMAX_PATH_LENGTH);
#endif
   if (!result)
   {
      auto out = ut::log::error() << "getcwd()";
      out.AddNote() << "Errno: " << std::strerror(errno);
   }
   return UtPath(buffer);
}

// static
//! Returns the executable path
UtPath UtPath::GetExePath()
{
   const int cMAX_PATH_LENGTH(1024);
   char      buffer[cMAX_PATH_LENGTH];
#ifdef _WIN32
   GetModuleFileNameA(nullptr, buffer, cMAX_PATH_LENGTH);
#else
   size_t pathNameSize  = readlink("/proc/self/exe", buffer, cMAX_PATH_LENGTH);
   buffer[pathNameSize] = '\0';
#endif
   return UtPath(buffer);
}

//! If aPath is a relative path, it is appended to this.
//! If aPath is an absolute path, it's copy is returned.
//! This behavior is similar to the change directory command, 'cd'
//! Returns aPath applied to this path.
UtPath UtPath::operator+(const UtPath& aPath) const
{
   SubstringList dirStack;
   std::string   prefix;
   ParsePath(prefix, dirStack);

   SubstringList dirStack2;
   std::string   prefix2;
   ParsePathP(aPath.GetNormalizedPath(), prefix2, dirStack2);

   UtPath path;
   if (prefix2.empty())
   {
      dirStack.insert(dirStack.end(), dirStack2.begin(), dirStack2.end());
      CleanDirectoryStack(dirStack);
      path = UtPath(BuildPath(prefix, dirStack));
   }
   else
   {
      path = aPath;
   }
   return path;
}
UtPath& UtPath::operator+=(const UtPath& aPath)
{
   *this = (*this) + aPath;
   return *this;
}

UtPath& UtPath::Up()
{
   SubstringList dirStack;
   std::string   prefix;
   ParsePath(prefix, dirStack);
   const char* dotDot = "..";
   if (!dirStack.empty())
   {
      if (dirStack.back() == "..")
      {
         dirStack.emplace_back(dotDot, dotDot + 2);
      }
      else
      {
         dirStack.pop_back();
      }
   }
   else
   {
      dirStack.emplace_back(dotDot, dotDot + 2);
   }

   mPathString = BuildPath(prefix, dirStack);
   return *this;
}

//! Change back slashes into forward slashes
void UtPath::NormalizeSlashes(std::string& aPath)
{
   size_t length = aPath.length();
   for (size_t i = 0; i < length; ++i)
   {
      if (aPath[i] == '\\')
      {
         aPath[i] = '/';
      }
   }
}

// static
//  Splits the path up into 2 parts.
//! @param aPrefix This is set to the prefix of the path.  This is null for
//!  relative paths.  For full paths, this could be "/", "\\", or "C:\".
//! @param aDirectoryStack  A vector of strings representing the directories
//!    in the path.
void UtPath::ParsePathP(const std::string& aPathString, std::string& aPrefix, SubstringList& aDirectoryStack)
{
   const char* p = aPathString.c_str();
   if (isSlash(*p))
   {
// double-slash is only significant on windows
#ifdef _WIN32
      if (isSlash(p[1]))
      {
         aPrefix = "//";
         p += 2;
      }
      else
#endif
      {
         aPrefix = '/';
         ++p;
      }
   }
   else if (*p != 0 && p[1] == ':' && p[2] != 0)
   {
      aPrefix.assign(p, p + 3);
      if (aPrefix[2] == '\\')
      {
         aPrefix[2] = '/';
      }
      p += 3;
   }

   const char* pos = p;
   while (*p != 0)
   {
      char c  = *p;
      char cn = p[1];
      if (!isSlash(c))
      {
         ++p;
      }
      if (cn == 0 || isSlash(c))
      {
         size_t length = p - pos;
         if (length > 0)
         {
            if (length == 1 && *pos == '.')
            {
               // do nothing.  Don't add current dir to the stack.
            }
            else if (length == 2 && *pos == '.' && c == '.')
            {
               if (!aDirectoryStack.empty() && aDirectoryStack.back() != "..")
               {
                  aDirectoryStack.pop_back();
               }
               else
               {
                  aDirectoryStack.emplace_back(pos, p);
               }
            }
            else
            {
               aDirectoryStack.emplace_back(pos, p);
            }
         }
         pos = p = p + 1;
         if (cn == 0)
            break;
      }
   }
   // Special case: '.' components are removed, but if no other components are present,
   //                the '.' is still significant so add it back in.
   if (aPrefix.empty() && aDirectoryStack.empty() && !aPathString.empty())
   {
      aDirectoryStack.emplace_back(".");
   }
}

void UtPath::ParsePath(std::string& aPrefix, SubstringList& aDirectoryStack) const
{
   UtPath::ParsePathP(mPathString, aPrefix, aDirectoryStack);
}

// static
//! Builds a path string from the prefix, and directory stack
std::string UtPath::BuildPath(const std::string& aPrefix, SubstringList& aDirectoryStack)
{
   std::string path = aPrefix;
   for (unsigned int i = 0; i < aDirectoryStack.size(); ++i)
   {
      if (i != 0)
      {
         path += '/';
      }
      path.append(aDirectoryStack[i].begin, aDirectoryStack[i].end);
   }
   return path;
}

#ifdef _WIN32
//! Returns the path in a compatible version for system calls
std::string UtPath::GetSystemPath() const
{
   std::string path(mPathString);
   size_t      length = path.length();
   for (size_t i = 0; i < length; ++i)
   {
      if (path[i] == '/')
      {
         path[i] = '\\';
      }
   }
   return path;
}

//! Changes the current working directory to the current path
bool UtPath::SetWorkingDirectory() const
{
   std::string sysPath = GetSystemPath();
   return (0 == _chdir(sysPath.c_str()));
}
namespace
{
bool OS_Mkdir(const char* aPath)
{
   return 0 != CreateDirectory(aPath, nullptr);
}
} // namespace

#else
//! Returns the path in a compatible version for system calls
std::string UtPath::GetSystemPath() const
{
   return mPathString;
}
//! Changes the current working directory to the current path
bool UtPath::SetWorkingDirectory() const
{
   return (0 == chdir(mPathString.c_str()));
}
namespace
{
bool OS_Mkdir(const char* aPath)
{
   return 0 == mkdir(aPath, S_IRWXG | S_IRWXU);
}
} // namespace
#endif

std::string UtPath::GetFileName(bool aWithExtension /*=true*/) const
{
   std::string filename;

   size_t i = mPathString.find_last_of('/');
   filename = (i == std::string::npos) ? mPathString : mPathString.substr(i + 1);
   if (!aWithExtension)
   {
      // Strip off the extension and return the name.
      size_t index = filename.find_last_of(".");

      if (index != std::string::npos)
      {
         return filename.substr(0, index);
      }
   }
   return filename;
}

std::string UtPath::GetFileExtension() const
{
   std::string filename = GetFileName(true);

   // Strip off the name and return the extension.
   size_t index = filename.find_first_of(".");

   if (index != std::string::npos)
   {
      return filename.substr(index);
   }
   return "";
}

//! uses stat() to determine if the path is valid, and if it points
//! to a file or a directory
UtPath::StatType UtPath::Stat() const
{
   StatType    fileType(cSTAT_ERROR);
   std::string sysPath = GetSystemPath();
   struct stat buff;
   if (0 == ::stat(sysPath.c_str(), &buff))
   {
      if ((buff.st_mode & S_IFDIR) != 0)
      {
         fileType = cDIRECTORY;
      }
      else
      {
         fileType = cFILE;
      }
   }
   return fileType;
}

//! uses stat() to determine if the path is valid, and if it points
//! to a file or a directory
bool UtPath::Stat(StatData& aData) const
{
   aData.mStatType     = cSTAT_ERROR;
   std::string sysPath = GetSystemPath();
   struct stat buff;
   if (0 == ::stat(sysPath.c_str(), &buff))
   {
      if ((buff.st_mode & S_IFDIR) != 0)
      {
         aData.mStatType = cDIRECTORY;
      }
      else
      {
         aData.mStatType = cFILE;
      }
      aData.mModifiedDate   = buff.st_mtime;
      aData.mFileSizeBytes  = (size_t)buff.st_size;
      aData.mPermissionBits = 0;
#ifdef _WIN32
      if ((buff.st_mode & S_IREAD) != 0)
         aData.mPermissionBits |= cPB_OWNER_READ;
      if ((buff.st_mode & S_IWRITE) != 0)
         aData.mPermissionBits |= cPB_OWNER_WRITE;
      if ((buff.st_mode & S_IEXEC) != 0)
         aData.mPermissionBits |= cPB_OWNER_EXECUTE;
#else
      if (buff.st_mode & S_IRUSR)
         aData.mPermissionBits |= cPB_OWNER_READ;
      if (buff.st_mode & S_IWUSR)
         aData.mPermissionBits |= cPB_OWNER_WRITE;
      if (buff.st_mode & S_IXUSR)
         aData.mPermissionBits |= cPB_OWNER_EXECUTE;
#endif
   }
   return aData.mStatType != cSTAT_ERROR;
}

//! Create a single directory, or all parent directories as well
bool UtPath::Mkdir(bool aCreateAll /*= true*/) const
{
   if (aCreateAll)
   {
      // Create all directories that do not exist in the path.
      std::vector<std::string> directories;
      UtPath                   thisPath = *this;
      StatType                 fs       = thisPath.Stat();

      while (fs == cSTAT_ERROR)
      {
         directories.push_back(thisPath.GetSystemPath());
         thisPath.Up();
         fs = thisPath.Stat();
      }

      bool ok = true;
      for (int i = int(directories.size()) - 1; i >= 0 && ok; --i)
      {
         ok = ok && OS_Mkdir(directories[i].c_str());
      }
      return ok;
   }
   else
   {
      // only create one directory
      return OS_Mkdir(GetSystemPath().c_str());
   }
}

//! Remove an empty directory
//! Returns 'true' if successful
bool UtPath::RemoveDir() const
{
   return 0 == ::rmdir(GetSystemPath().c_str());
}

namespace
{
bool RmTree(const UtPath& aPath)
{
   std::vector<std::string> files;
   std::vector<std::string> dirs;
   aPath.ListDir(files, dirs);
   for (const auto& file : files)
   {
      UtPath filePath = aPath + file;
      filePath.RemoveFile();
   }
   for (const auto& dir : dirs)
   {
      UtPath filePath = aPath + dir;
      RmTree(filePath);
   }
   return aPath.RemoveDir();
}
} // namespace

//! Remove a directory and all contained files
//! Returns 'true' if successful
bool UtPath::RemoveTree() const
{
   if (IsDirectory())
   {
      return RmTree(*this);
   }
   return false;
}

bool UtPath::RemoveFile() const
{
   return 0 == ::remove(GetSystemPath().c_str());
}

//! Rename a file or directory.
//! @param aNewName The new name of the file or directory.
//! @returns 'true' if successful
bool UtPath::Rename(const std::string& aNewName) const
{
   UtPath newPath(*this);
   newPath.Up();
   newPath += aNewName;
   return 0 == ::rename(GetSystemPath().c_str(), newPath.GetSystemPath().c_str());
}

UtPath UtPath::GetRelativePath(const UtPath& aReferencedDirectory) const
{
   std::string   localPrefix;
   std::string   refPrefix;
   SubstringList localDirList;
   SubstringList refDirList;

   UtPath fullLocalPath = WorkingDirectory() + *this;
   UtPath fullRefPath   = WorkingDirectory() + aReferencedDirectory;
   ParsePathP(fullLocalPath.mPathString, localPrefix, localDirList);
   ParsePathP(fullRefPath.mPathString, refPrefix, refDirList);

   if (localPrefix == refPrefix)
   {
      size_t i = 0;
      for (; i < localDirList.size(); ++i)
      {
         if (refDirList.size() <= i)
         {
            break;
         }
         if (refDirList[i] != localDirList[i])
         {
            break;
         }
      }
      SubstringList dirList;
      dirList.insert(dirList.end(), localDirList.size() - i, "..");
      dirList.insert(dirList.end(), refDirList.begin() + i, refDirList.end());
      return UtPath(BuildPath("", dirList));
   }
   return aReferencedDirectory;
}

// Return 'true' if this path is an ancestor of aOtherPath
// 'c:\dir1'.IsAncestorOf('c:\dir1\subdir') == true
// 'c:\dir1'.IsAncestorOf('c:\dir1\subdir\file.txt') == true
// 'c:\dir1'.IsAncestorOf('c:\dir1') == false
bool UtPath::IsAncestorOf(const UtPath& aOtherPath) const
{
   std::string   localPrefix;
   std::string   refPrefix;
   SubstringList localDirList;
   SubstringList refDirList;

   UtPath fullLocalPath = WorkingDirectory() + *this;
   UtPath fullRefPath   = WorkingDirectory() + aOtherPath;
   ParsePathP(fullLocalPath.mPathString, localPrefix, localDirList);
   ParsePathP(fullRefPath.mPathString, refPrefix, refDirList);

   if (localPrefix != refPrefix)
      return false;
   if (localDirList.size() >= refDirList.size())
      return false;
   refDirList.resize(localDirList.size());
   return localDirList == refDirList;
}

// If current path is relative, convert it to absolute path. Assume path is relative to referenced path, if provided,
// otherwise current working directory.
// If current path is absolute, it will be unchanged.
void UtPath::MakeFullPath(const UtPath& aReferencePath /* = UtPath() */)
{
   SubstringList dirStack;
   std::string   prefix;
   ParsePath(prefix, dirStack);
   // If prefix is not empty, it is an absolute path already.
   if (prefix.empty())
   {
      UtPath newPath = (aReferencePath.IsNull() ? WorkingDirectory() : aReferencePath) + *this;
      std::swap(mPathString, newPath.mPathString);
   }
}

//! Returns the full path without sym links
UtPath UtPath::GetRealPath() const
{
   UtPath fullPath(*this);
   fullPath.MakeFullPath();
#ifndef _WIN32
   char        realPath[PATH_MAX + 1];
   std::string sysPath = fullPath.GetSystemPath();
   char*       result  = realpath(sysPath.c_str(), realPath);
   if (result)
   {
      return UtPath(result);
   }
#endif
   // No implementation on windows yet, just return full path
   return fullPath;
}

void UtPath::Tokenize(std::string& aPrefix, std::vector<std::string>& aTokens) const
{
   SubstringList substrings;
   ParsePathP(mPathString, aPrefix, substrings);
   for (const Substring& ss : substrings)
   {
      aTokens.emplace_back(ss.begin, ss.end);
   }
}

//! Gets the list of files and directories contained in the directory pointed to by this UtPath.
//! Does not include '.' or '..'
//! Returns 'true' if this is a directory.
bool UtPath::ListDir(std::vector<std::string>& aFiles, std::vector<std::string>& aDirectories) const
{
   std::string path = GetSystemPath();
#ifndef _WIN32
   DIR* dirPtr = opendir(path.c_str());
   if (!dirPtr)
   {
      auto out = ut::log::error() << "opendir()";
      out.AddNote() << "Errno: " << std::strerror(errno);
      return false;
   }
   for (struct dirent* entryPtr = readdir(dirPtr); entryPtr != nullptr; entryPtr = readdir(dirPtr))
   {
      const char* fileName = entryPtr->d_name;
      if (entryPtr->d_type == DT_REG)
      {
         aFiles.push_back(fileName);
      }
      else if (entryPtr->d_type == DT_DIR)
      {
         // check for and ignore '.' and '..'
         bool isDotDir = (fileName[0] == '.' && (fileName[1] == '\0' || (fileName[1] == '.' && fileName[2] == '\0')));
         if (!isDotDir)
         {
            aDirectories.push_back(fileName);
         }
      }
   }
   closedir(dirPtr);
#else
   path += "\\*";
   WIN32_FIND_DATA wfd;
   HANDLE hf;
   hf = FindFirstFile(path.c_str(), &wfd);
   do
   {
      if (hf == INVALID_HANDLE_VALUE)
      {
         return false;
      }
      const char* fileName = wfd.cFileName;
      if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0u)
      {
         bool isDotDir = (fileName[0] == '.' && (fileName[1] == '\0' || (fileName[1] == '.' && fileName[2] == '\0')));
         if (!isDotDir)
         {
            aDirectories.emplace_back(wfd.cFileName);
         }
      }
      else
      {
         aFiles.emplace_back(wfd.cFileName);
      }
   } while (FindNextFile(hf, &wfd));

   FindClose(hf);
#endif
   return true;
}
