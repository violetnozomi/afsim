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

// ----------------------------------------------------------------------------
//
// This class represents a text description that was built by the UserMapTool
//
// ----------------------------------------------------------------------------

#include "UserMapLodHeader.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

#include "UtLog.hpp"

//#define DEBUG_ON

// ----------------------------------------------------------------------------

// 'Open' a user map directory.  The specified name should be a directory
// built by UserMapTool.

UserMapLodHeader::UserMapLodHeader(const char* aFileName, int& aError)
{
   int  i;
   char readstring[1024];
   char dummy[1024];

   mLOD_List.clear();
   aError = 1;

   std::fstream* myfile = new std::fstream(aFileName, std::ios::in);

   if (myfile == nullptr)
   {
      auto out = ut::log::error() << "UserMapHeader could not open header file.";
      out.AddNote() << "File: " << aFileName;
      return;
   }

   myfile->getline(readstring, 1024, '\n');
   sscanf(readstring, "Original_Image %s", dummy); // Original_Image name
   myfile->getline(readstring, 1024, '\n');
   sscanf(readstring, "Compression %d", &mCompression);
   myfile->getline(readstring, 1024, '\n');
   sscanf(readstring, "LODs %d", &mLODs);
   for (i = 0; i < mLODs; ++i)
   {
      // create lod
      LOD lod;
      myfile->getline(readstring, 1024, '\n'); // LOD number, ignore, we can assume they come in sequential order by design
      myfile->getline(readstring, 1024, '\n');
      sscanf(readstring, "\tNortheast %lf %lf", &(lod.mNELat), &(lod.mNELon));
      myfile->getline(readstring, 1024, '\n');
      sscanf(readstring, "\tSouthwest %lf %lf", &(lod.mSWLat), &(lod.mSWLon));
      myfile->getline(readstring, 1024, '\n');
      sscanf(readstring, "\tTile_Pixel_Size %d %d", &(lod.mPixelSizeX), &(lod.mPixelSizeY));
      myfile->getline(readstring, 1024, '\n');
      sscanf(readstring, "\tTile_Degree_Size %lf %lf", &(lod.mDegreeSizeY), &(lod.mDegreeSizeX));
      myfile->getline(readstring, 1024, '\n');
      sscanf(readstring, "\tClip_Values %f %f", &(lod.mLastUMax), &(lod.mLastVMax));
      mLOD_List.push_back(lod);
   }
   assert(mLOD_List.size() == (unsigned int)mLODs);

   aError = 0;
}

UserMapLodHeader::~UserMapLodHeader()
{
   mLOD_List.clear();
}

double UserMapLodHeader::GetLatDegPerPixel(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   LOD* lod = (LOD*)&(mLOD_List[aLOD]);

   return (lod->mDegreeSizeY / lod->mPixelSizeY);
}

double UserMapLodHeader::GetLonDegPerPixel(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   LOD* lod = (LOD*)&(mLOD_List[aLOD]);

   return (lod->mDegreeSizeX / lod->mPixelSizeX);
}

unsigned int UserMapLodHeader::GetHeight(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mPixelSizeY;
}

unsigned int UserMapLodHeader::GetWidth(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mPixelSizeX;
}

double UserMapLodHeader::GetHeightDeg(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mDegreeSizeY;
}

double UserMapLodHeader::GetWidthDeg(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mDegreeSizeX;
}

float UserMapLodHeader::GetLastUMax(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mLastUMax;
}

float UserMapLodHeader::GetLastVMax(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mLastVMax;
}

double UserMapLodHeader::GetSouthWestLat(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mSWLat;
}

double UserMapLodHeader::GetSouthWestLon(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mSWLon;
}

double UserMapLodHeader::GetNorthEastLat(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mNELat;
}

double UserMapLodHeader::GetNorthEastLon(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mNELon;
}
