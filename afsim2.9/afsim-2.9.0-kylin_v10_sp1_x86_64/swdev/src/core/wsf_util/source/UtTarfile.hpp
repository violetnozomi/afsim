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

#ifndef UTTARFILE_HPP
#define UTTARFILE_HPP

#include <functional>
#include <iosfwd>

#include "UtPath.hpp"
#include "WsfUtExport.hpp"

//! Supports read/write operations similar to tar files
//! but this is not actually compatible with tar files.
class WSF_UT_EXPORT UtTarfileO
{
public:
   typedef std::function<bool(const UtPath&, const UtPath::StatData&)> FilterFunction;

   UtTarfileO(std::ostream& aOutputStream);

   // Assign a filter function used to pick which directories and files are processed
   // on calls to AddDirectory / AddFile
   void SetFilter(const FilterFunction& aFilter);

   void AddDirectory(const std::string& aPrefix, const UtPath& aPath);

   void AddFile(const std::string& aDestinationPath, const UtPath& aFilePath);

private:
   static void WriteDirectory(const std::string& aPath, std::ostream& aOutputTar);

   static void WriteFile(const std::string& aFilePath,
                         const unsigned int aPermissionBits,
                         const unsigned int aFileSize,
                         std::istream&      aFileData,
                         std::ostream&      aOutputTar);

   void AddDirectory(const std::string& aPrefix, const UtPath& aBasePath, const UtPath& aPath, const UtPath::StatData& aStatData);

   void AddFile(const std::string& aFilePath, const UtPath& aPath, const UtPath::StatData& aStatData);

   std::ostream*  mOutStream;
   FilterFunction mFilterFunction;
};

class WSF_UT_EXPORT UtTarfileI
{
public:
   // Unpack a tar file into a output directory
   static bool Unpack(std::istream& aInputTar, const UtPath& aOutputDirectory);

private:
   static bool ReadDirectory(std::string& aPath, std::istream& aInputTar);


   static bool
   ReadFileHdr(std::string& aFilePath, unsigned int& aPermissionBits, unsigned int& aFileSize, std::istream& aInputTar);
};
#endif
