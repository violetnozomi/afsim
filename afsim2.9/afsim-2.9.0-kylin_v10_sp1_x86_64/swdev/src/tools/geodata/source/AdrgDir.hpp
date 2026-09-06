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

#ifndef ADRGDIR_HPP
#define ADRGDIR_HPP

#include "geodata_export.h"

// This class represents a directory that contains ARC Digitized Raster
// Graphics (ADRG) data.  The directory is expected to be in the standard
// NIMA structure.
//
// Refer to MIL-A-89007, 22 Feb 1990, for the ADRG specification.

#include <string>

#include "ArcDir.hpp"
class AdrgFile;

class GEODATA_EXPORT AdrgDir : public ArcDir
{
public:
   // 'Open' a ADRG directory.  The specified name should be a directory
   // that represents either an ADRG CD-ROM or an EXACT COPY of a CD-ROM.

   AdrgDir(const char* aDirName, int& aError);

   ~AdrgDir() override;

private:
   void ReadTransmittalHeaderFile(AdrgFile& aThFile);
   void ReadDistributionRectangle(AdrgFile& aThFile, const char* aDrDirName);

   std::string mDirName;
   bool        mLowerCase;

   // These are defined but not implemented to prevent use.
   AdrgDir(const AdrgDir& aSrc);
   AdrgDir& operator=(const AdrgDir& aRhs) = delete;
};

#endif
