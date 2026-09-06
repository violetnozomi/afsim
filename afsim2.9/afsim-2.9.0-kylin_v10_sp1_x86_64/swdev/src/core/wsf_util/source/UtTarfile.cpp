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

#include "UtTarfile.hpp"

#include <cassert>
#include <fstream>

#ifndef _WIN32
#include <sys/stat.h>
#endif

namespace
{
const char utTarHdr[11] = "\0\0\0\0UtTar\0";
void       CopyStream(const unsigned int aBytesToCopy, std::istream& aInputStream, std::ostream& aOutputStream)
{
   unsigned int       bytesLeft = aBytesToCopy;
   const unsigned int cBUF_SIZE = 1024;
   char               buf[cBUF_SIZE];
   while (bytesLeft > 0)
   {
      unsigned int bytesToRead = std::min(cBUF_SIZE, bytesLeft);
      int          bytesRead;
      if (!aInputStream.read(buf, bytesToRead))
      {
         // if we fail to read file, pad with zeroes
         memset(buf, 0, bytesToRead);
         bytesRead = bytesToRead;
      }
      else
      {
         bytesRead = (int)aInputStream.gcount();
      }
      aOutputStream.write(buf, bytesRead);
      bytesLeft -= bytesRead;
   }
}

} // namespace
bool UtTarfileI::Unpack(std::istream& aInputTar, const UtPath& aOutputDirectory)
{
   aOutputDirectory.Mkdir(false);
   char hdrBuf[10];
   aInputTar.read(hdrBuf, 10);
   if (!aInputTar)
   {
      return false;
   }
   if (memcmp(hdrBuf, utTarHdr, 10) != 0)
   {
      return false;
   }
   for (;;)
   {
      char c = aInputTar.get();
      if (aInputTar)
      {
         if (c == '0') // make directory
         {
            std::string path;
            if (!ReadDirectory(path, aInputTar))
            {
               return false;
            }
            (aOutputDirectory + path).Mkdir(false);
         }
         else if (c == '1') // make file
         {
            std::string  path;
            unsigned int permissionBits(0);
            unsigned int fileSize(0);
            if (!ReadFileHdr(path, permissionBits, fileSize, aInputTar))
            {
               return false;
            }
            std::ofstream out;
            std::string   filePathStr = (aOutputDirectory + path).GetSystemPath();
            out.open(filePathStr.c_str(), std::ios_base::binary);
            CopyStream(fileSize, aInputTar, out);
            out.close();
#ifndef _WIN32
            if (permissionBits & 4)
            {
               chmod(filePathStr.c_str(), S_IXUSR);
            }
#endif
         }
         else
         {
            // expected 0 or 1
            return false;
         }
      }
      else
      {
         break;
      }
   }
   return true;
}

void UtTarfileO::AddFile(const std::string& aDestinationPath, const UtPath& aFilePath)
{
   UtPath::StatData data;
   aFilePath.Stat(data);
   AddFile(aDestinationPath, aFilePath, data);
}

void UtTarfileO::AddFile(const std::string& aFilePath, const UtPath& aPath, const UtPath::StatData& aStatData)
{
   if (mFilterFunction)
   {
      if (!mFilterFunction(aPath, aStatData))
      {
         return;
      }
   }
   std::ifstream fs;
   fs.open(aPath.GetSystemPath().c_str(), std::ios_base::binary);
   WriteFile(aFilePath, aStatData.mPermissionBits, (unsigned int)aStatData.mFileSizeBytes, fs, *mOutStream);
}

void UtTarfileO::AddDirectory(const std::string& aPrefix, const UtPath& aPath)
{
   UtPath::StatData data;
   aPath.Stat(data);
   std::string dirName = aPrefix;
   if (dirName.empty())
   {
      dirName = '.';
   }
   char lastChar = dirName[dirName.size() - 1];
   if (lastChar != '/' && lastChar != '\\')
   {
      dirName += '/';
   }
   AddDirectory(dirName, aPath, aPath, data);
}

void UtTarfileO::AddDirectory(const std::string&      aPrefix,
                              const UtPath&           aBasePath,
                              const UtPath&           aPath,
                              const UtPath::StatData& aStatData)
{
   if (mFilterFunction)
   {
      if (!mFilterFunction(aPath, aStatData))
      {
         return;
      }
   }
   WriteDirectory(aPrefix + aBasePath.GetRelativePath(aPath).GetNormalizedPath(), *mOutStream);
   std::vector<std::string> files, dirs;
   aPath.ListDir(files, dirs);

   for (size_t i = 0; i < dirs.size(); ++i)
   {
      UtPath           filePath = aPath + dirs[i];
      UtPath::StatData statData;
      filePath.Stat(statData);
      if (statData.mStatType == UtPath::cDIRECTORY)
      {
         AddDirectory(aPrefix, aBasePath, filePath, statData);
      }
   }
   for (size_t i = 0; i < files.size(); ++i)
   {
      UtPath           filePath = aPath + files[i];
      UtPath::StatData statData;
      filePath.Stat(statData);
      if (statData.mStatType == UtPath::cFILE)
      {
         std::string tarFilePath = aPrefix + "/" + aBasePath.GetRelativePath(filePath).GetNormalizedPath();
         AddFile(tarFilePath, filePath, statData);
      }
   }
}

void UtTarfileO::SetFilter(const FilterFunction& aFilter)
{
   mFilterFunction = aFilter;
}

bool UtTarfileI::ReadDirectory(std::string& aPath, std::istream& aInputTar)
{
   char path[1024];
   path[1023] = 0;
   if (aInputTar.getline(path, 1023, '\0'))
   {
      aPath = path;
      return true;
   }
   return false;
}

void UtTarfileO::WriteDirectory(const std::string& aPath, std::ostream& aOutputTar)
{
   aOutputTar << '0';
   aOutputTar << aPath;
   aOutputTar << '\0';
}

bool UtTarfileI::ReadFileHdr(std::string&  aFilePath,
                             unsigned int& aPermissionBits,
                             unsigned int& aFileSize,
                             std::istream& aInputTar)
{
   char perm = aInputTar.get();
   if (perm <= '0' || perm > '7')
   {
      return false;
   }
   aPermissionBits = (int)(perm - '0');
   if (!aInputTar.read((char*)&aFileSize, 4))
   {
      return false;
   }
   char path[1024];
   if (!aInputTar.getline(path, 1024, '\0'))
   {
      return false;
   }
   aFilePath = path;
   return true;
}

void UtTarfileO::WriteFile(const std::string& aFilePath,
                           const unsigned int aPermissionBits,
                           const unsigned int aFileSize,
                           std::istream&      aFileData,
                           std::ostream&      aOutputTar)
{
   aOutputTar.put('1');
   assert(/*aPermissionBits >= 0 && */ aPermissionBits <= 7); // TODO was this meant to be just a greater than 0 check??
   aOutputTar.put('0' + aPermissionBits);
   aOutputTar.write((const char*)&aFileSize, 4);
   aOutputTar.write(aFilePath.c_str(), aFilePath.size());
   aOutputTar.put('\0');

   CopyStream(aFileSize, aFileData, aOutputTar);
}

UtTarfileO::UtTarfileO(std::ostream& aOutputStream)
   : mOutStream(&aOutputStream)
{
   mOutStream->write(utTarHdr, 10);
}
