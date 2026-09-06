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

#include "LandUseTile.hpp"

#include <fstream>

#include "UtBitmapImage.hpp"
#include "UtInputDocument.hpp"
#include "UtInputFile.hpp"

LandUseTile::LandUseTile(LandUseTile::Format aFormat)
   : GeoLatLonGridTile()
   , mFormat(aFormat)
   , mBitmapPtr(nullptr)
{
   SetDataType(cUNSIGNED_CHAR);
   SetDataOrderType(cROW_MAJOR);
}

LandUseTile::~LandUseTile()
{
   if (mBitmapPtr != nullptr)
   {
      // Need to make sure we clear the pointer to data before
      // the base class tries to delete it (it is still owned by
      // the bitmap object.
      mData.mAsUnsignedChar = nullptr;
      delete mBitmapPtr;
   }
}

void LandUseTile::InitializeFromHeader()
{
   SetSWLon(mHeader.GetSWLon());
   SetNELon(mHeader.GetNELon());
   SetSWLat(mHeader.GetSWLat());
   SetNELat(mHeader.GetNELat());

   mLatPoints = mHeader.mNRows;
   mLonPoints = mHeader.mNCols;

   mLatInterval      = mHeader.mCellSize;
   mHalfLatInterval  = 0.5 * mLatInterval;
   mRecipLatInterval = 1.0 / mLatInterval;

   mLonInterval      = mHeader.mCellSize;
   mHalfLonInterval  = 0.5 * mLonInterval;
   mRecipLonInterval = 1.0 / mLonInterval;
}

int LandUseTile::LoadTile(const char* aFileName)
{
   int status = -1;
   if (mFormat == cARC_ASCII)
   {
      std::string   headerFileName = std::string(aFileName) + ".asc";
      std::ifstream ifs(headerFileName.c_str(), std::ios_base::in);
      status = FloatGridTile::LoadHeader(ifs, mHeader, 6); // read only the first 6 lines of the file...
      if (status == 0)
      {
         InitializeFromHeader();

         // Allocate the space for the actual elevation data
         unsigned totalPoints = mLonPoints * mLatPoints;
         CreateData(totalPoints);

         // load the rest of the data...
         for (unsigned i = 0; i < mLatPoints; ++i)
         {
            for (unsigned j = 0; j < mLonPoints; ++j)
            {
               short temp;
               ifs >> temp;
               mData.mAsUnsignedChar[(j + 1) * mLatPoints - i - 1] = (unsigned char)temp;
            }
         }
      }
   }
   else // bmp format
   {
      std::string headerFileName = std::string(aFileName) + ".ERS";
      status                     = LoadERS_Header(headerFileName);
      if (status == 0)
      {
         InitializeFromHeader();
         std::string bmpFileName = std::string(aFileName) + ".bmp";
         assert(mBitmapPtr == nullptr);
         mBitmapPtr = new UtBitmapImage();
         bool ok    = mBitmapPtr->Load(bmpFileName);
         if (ok)
         {
            status = 0;

            // Assign data, but don't take ownersip of it.
            mData.mAsUnsignedChar = mBitmapPtr->GetImage().mBytePtr;
         }
      }
   }
   return status;
}

//! Construct a dummy Dem cell for the specified location. The location
//! is specified in the WGS-84 reference frame using decimal degrees
//! with negative values for west and south.
//! @note This is basically the same as the DTED version; might want to revisit this implementation.
void LandUseTile::MakeDummyTile(double aLat, double aLon, double aLatInterval, double aLonInterval)
{
   // Determine the integral location of the southwest corner that
   // contains the specified location.

   mDummyTile = true;

   int lat = static_cast<int>(aLat);
   if (lat > aLat)
   {
      --lat;
   }
   int lon = static_cast<int>(aLon);
   if (lon > aLon)
   {
      --lon;
   }

   SetSWLat(lat);
   SetSWLon(lon);
   SetNELat(lat + 1.0);
   SetNELon(lon + 1.0);
   mLatInterval      = 1.0;
   mHalfLatInterval  = 0.5 * mLatInterval;
   mRecipLatInterval = 1.0 / mLatInterval;
   mLonInterval      = 1.0;
   mHalfLonInterval  = 0.5 * mLonInterval;
   mRecipLonInterval = 1.0 / mLonInterval;
   mLatPoints        = 2;
   mLonPoints        = 2;

   // Create dummy elevation data.

   CreateData(4);
   for (unsigned int i = 0; i < 4; ++i)
   {
      mData.mAsUnsignedChar[i] = 0;
   }
}

int LandUseTile::LoadERS_Header(const std::string& aFileName)
{
   int status = -1;

   std::unique_ptr<UtInputFile> filePtr = nullptr;
   try
   {
      filePtr = ut::make_unique<UtInputFile>(aFileName);
   }
   catch (UtInputDocument::OpenError&)
   {
      return status; // failed to open file
   }

   std::string command;
   unsigned    numberCommands = 0;
   UtInput     input;
   input.PushInput(std::move(filePtr));

   while (true)
   {
      try
      {
         input.ReadCommand(command);
         if (command == "Xdimension")
         {
            input.ReadCommand(command); // equals sign
            input.ReadValue(mHeader.mCellSize);
            ++numberCommands;
         }
         //         else if (command == "Ydimension")
         //         {
         //         }
         else if (command == "Eastings")
         {
            input.ReadCommand(command);
            input.ReadValue(mHeader.mXLLCorner);
            ++numberCommands;
         }
         else if (command == "Northings")
         {
            input.ReadCommand(command);
            input.ReadValue(mHeader.mYLLCorner);
            // value is for northmost corner
            mHeader.mYLLCorner -= 1.0;
            ++numberCommands;
         }
         else if (command == "NrOfLines")
         {
            input.ReadCommand(command);
            input.ReadValue(mHeader.mNRows);
            ++numberCommands;
         }
         else if (command == "NrOfCellsPerLine")
         {
            input.ReadCommand(command);
            input.ReadValue(mHeader.mNCols);
            ++numberCommands;
         }
      }
      catch (UtInput::EndOfData&)
      {
         break;
      }
   }

   if (numberCommands == 5)
   {
      status = 0;
   }

   return status;
}
