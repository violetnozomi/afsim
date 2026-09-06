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

#include "GeoidUndulation.hpp"

#include <fstream>

#include "UtLog.hpp"

// ============================================================================
float GeoidUndulation::Get(double aLatitude, double aLongitude) const
{
   // Longitude values should be in 0-360 degrees format
   double longitude = aLongitude;
   if (longitude < 0.0)
   {
      longitude += 360.0;
   }

   mLatLookUp.Lookup(mLatValues, static_cast<float>(aLatitude));
   mLonLookUp.Lookup(mLonValues, static_cast<float>(longitude));
   return TblEvaluate(mHeightValuesM, mLatLookUp, mLonLookUp);
}

// ============================================================================
bool GeoidUndulation::ReadData(const std::string& aFilename, const std::string& aEGM_Type /* = "EGM96"*/)
{
   std::ifstream ifs(aFilename.c_str(), std::ios::binary);
   if (!ifs.is_open())
   {
      auto out = ut::log::error() << "Geoid Undulation file could not be opened for input.";
      out.AddNote() << "File: " << aFilename;
      return false;
   }

   if (aEGM_Type == "EGM96")
   {
      return ReadEGM96Data(ifs);
   }
   else if (aEGM_Type == "EGM2008")
   {
      return ReadEGM2008Data(ifs);
   }
   else
   {
      auto out = ut::log::error() << "Geoid Undulation EGM type was not recognized.";
      out.AddNote() << "Type: " << aEGM_Type;
      return false;
   }
}

// ============================================================================
bool GeoidUndulation::ReadEGM2008Data(std::ifstream& aStream)
{
   float  minLat, maxLat, minLon, maxLon, latSpacing, lonSpacing;
   size_t numLats, numLons;
   aStream >> maxLat;
   aStream >> minLon;
   aStream >> latSpacing;
   aStream >> lonSpacing;
   aStream >> numLats;
   aStream >> numLons;

   latSpacing /= 60.f;
   lonSpacing /= 60.f;

   minLat = maxLat - (latSpacing * (numLats - 1));
   maxLon = minLon + (lonSpacing * (numLons - 1));

   mLatValues.SetValues(minLat, maxLat, numLats);
   mLonValues.SetValues(minLon, maxLon, numLons);

   mHeightValuesM.Resize(numLats, numLons);

   for (size_t i = 0; i < numLats; ++i)
   {
      for (size_t j = 0; j < numLons; ++j)
      {
         float temp;
         if (aStream >> temp)
         {
            mHeightValuesM.Set(temp, numLats - 1 - i, j);
         }
      }
   }
   size_t dim1, dim2;
   mHeightValuesM.GetSize(dim1, dim2);

   return (mLatValues.GetSize() > 0) && (mLonValues.GetSize() > 0) && (dim1 == numLats) && (dim2 == numLons);
}

// ============================================================================
bool GeoidUndulation::ReadEGM96Data(std::ifstream& aStream)
{
   float minLat, maxLat, minLon, maxLon, latSpacing, lonSpacing;
   aStream >> minLat;
   aStream >> maxLat;
   aStream >> minLon;
   aStream >> maxLon;
   aStream >> latSpacing;
   aStream >> lonSpacing;

   size_t numLats = static_cast<size_t>((maxLat - minLat) / latSpacing + 1);
   size_t numLons = static_cast<size_t>((maxLon - minLon) / lonSpacing + 1);

   mLatValues.SetValues(minLat, maxLat, numLats);
   mLonValues.SetValues(minLon, maxLon, numLons);

   mHeightValuesM.Resize(numLats, numLons);

   for (size_t i = 0; i < numLats; ++i)
   {
      for (size_t j = 0; j < numLons; ++j)
      {
         float temp;
         if (aStream >> temp)
         {
            mHeightValuesM.Set(temp, numLats - 1 - i, j);
         }
      }
   }
   size_t dim1, dim2;
   mHeightValuesM.GetSize(dim1, dim2);

   return (mLatValues.GetSize() > 0) && (mLonValues.GetSize() > 0) && (dim1 == numLats) && (dim2 == numLons);
}
