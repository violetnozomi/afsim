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

#ifndef USERMAPHEADER_HPP
#define USERMAPHEADER_HPP

#include "geodata_export.h"

// This class represents a text header that contains user map data made by
// the UserMapTool.

#include <vector>

class GEODATA_EXPORT UserMapHeader
{
public:
   UserMapHeader(const char* aFileName, int& aError);

   ~UserMapHeader();

   inline bool         GetCompressed() const { return (mCompression != 0); }
   inline double       GetSouthWestLat() const { return mSWLat; }
   inline double       GetSouthWestLon() const { return mSWLon; }
   inline double       GetNorthEastLat() const { return mNELat; }
   inline double       GetNorthEastLon() const { return mNELon; }
   inline unsigned int GetLODCount() const { return mLODs; }
   double              GetLatDegPerPixel(unsigned int aLOD) const;
   double              GetLonDegPerPixel(unsigned int aLOD) const;
   unsigned int        GetHeight(unsigned int aLOD) const;
   unsigned int        GetWidth(unsigned int aLOD) const;
   double              GetHeightDeg(unsigned int aLOD) const;
   double              GetWidthDeg(unsigned int aLOD) const;
   float               GetLastUMax(unsigned int aLOD) const;
   float               GetLastVMax(unsigned int aLOD) const;

private:
   struct LOD
   {
      unsigned int mPixelSizeX;
      unsigned int mPixelSizeY;
      double       mDegreeSizeX;
      double       mDegreeSizeY;
      float        mLastUMax;
      float        mLastVMax;
   };

   std::vector<LOD> mLOD_List;
   double           mNELat;
   double           mNELon;
   double           mSWLat;
   double           mSWLon;
   int              mCompression;
   int              mLODs;
};

#endif
