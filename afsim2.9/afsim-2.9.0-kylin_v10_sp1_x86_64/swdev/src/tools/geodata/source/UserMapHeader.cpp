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

#include "UserMapHeader.hpp"

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <string>

#include "UtLog.hpp"

//#define DEBUG_ON

// ----------------------------------------------------------------------------

// 'Open' a user map directory.  The specified name should be a directory
// built by UserMapTool.

UserMapHeader::UserMapHeader(const char* aFileName, int& aError)
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
   sscanf(readstring, "Northeast %lf %lf", &mNELat, &mNELon);
   myfile->getline(readstring, 1024, '\n');
   sscanf(readstring, "Southwest %lf %lf", &mSWLat, &mSWLon);
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

UserMapHeader::~UserMapHeader()
{
   mLOD_List.clear();
}

double UserMapHeader::GetLatDegPerPixel(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   LOD* lod = (LOD*)&(mLOD_List[aLOD]);

   return (lod->mDegreeSizeY / lod->mPixelSizeY);
}

double UserMapHeader::GetLonDegPerPixel(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   LOD* lod = (LOD*)&(mLOD_List[aLOD]);

   return (lod->mDegreeSizeX / lod->mPixelSizeX);
}

unsigned int UserMapHeader::GetHeight(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mPixelSizeY;
}

unsigned int UserMapHeader::GetWidth(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mPixelSizeX;
}

double UserMapHeader::GetHeightDeg(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mDegreeSizeY;
}

double UserMapHeader::GetWidthDeg(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mDegreeSizeX;
}

float UserMapHeader::GetLastUMax(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mLastUMax;
}

float UserMapHeader::GetLastVMax(unsigned int aLOD) const
{
   assert(aLOD < (unsigned int)mLODs);

   return mLOD_List[aLOD].mLastVMax;
}
