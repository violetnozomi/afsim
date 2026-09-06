// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "BathymetryXYZ_Tile.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>

// Utilities
#include "UtCast.hpp"
#include "UtLog.hpp"
#include "UtStringUtil.hpp"

// =================================================================================================
Geodata::BathymetryXYZ_Tile::BathymetryXYZ_Tile(size_t aIndex)
   : GeoElevationTile()
   , mBathymetryIndex(aIndex)
{
   SetDataType(cFLOAT);
   SetDataOrderType(cROW_MAJOR);
}

// =================================================================================================
bool Geodata::BathymetryXYZ_Tile::CheckTile(TileData& aData)
{
   std::ifstream ifs;
   ifs.open(aData.mFilename, std::ios_base::in);
   if (ifs.is_open())
   {
      // Check the first lines
      std::string line;
      getline(ifs, line);
      switch (CheckHeader(line))
      {
      case (1):
      {
         // File has a header; read the next line
         getline(ifs, line);
      }
      break;
      case (2):
      {
         ifs.close();
         return false;
      }
      break;
      default:
         break;
      }

      float lat1(-999.);
      float lon1(-999.);
      if (!line.empty())
      {
         float depth;
         ProcessLine(line, lat1, lon1, depth);
      }
      else
      {
         ifs.close();
         return false;
      }

      // Check for the last line
      ifs.seekg(-1, std::ios_base::end);
      while (true)
      {
         char ch;
         ifs.get(ch);
         if ((int)ifs.tellg() <= 1)
         {
            // First line is the last line
            ifs.seekg(0);
            getline(ifs, line);
            break;
         }
         else if (ch == '\n')
         {
            getline(ifs, line);
            break;
         }
         else
         {
            ifs.seekg(-2, std::ios_base::cur);
         }
      }

      float lat2(999.);
      float lon2(999.);
      if (!line.empty())
      {
         float depth;
         ProcessLine(line, lat2, lon2, depth);
      }

      // Check for ascending latitudes
      if (lat2 >= lat1)
      {
         aData.mAscendingLatitudes = true;
      }

      // Check for ascending longitudes
      if (lon2 < lon1)
      {
         auto out = ut::log::error() << "Longitudes must be in ascending order array sizes in bathymetry file.";
         out.AddNote() << "File: " << aData.mFilename;
         ifs.close();
         return false;
      }

      // Set the min/ max values
      aData.mLatRange.first  = std::min(lat1, lat2);
      aData.mLatRange.second = std::max(lat1, lat2);
      aData.mLonRange.first  = std::min(lon1, lon2);
      aData.mLonRange.second = std::max(lon1, lon2);

      ifs.close();
   }
   return true;
}

// =================================================================================================
int Geodata::BathymetryXYZ_Tile::LoadTile(const TileData& aData)
{
   std::ifstream ifs(aData.mFilename, std::ios_base::in);
   if (!ifs.is_open())
   {
      auto out = ut::log::error() << "Cannot open bathymetry file.";
      out.AddNote() << "File: " << aData.mFilename;
      return 1;
   }

   size_t             lineNum = 0;
   std::string        line;
   std::vector<float> lats, lons, depths;
   float              lat, lon, depth;
   while (getline(ifs, line))
   {
      // Check to see if the file has a header line
      bool headerLine(false);
      if (lineNum == 0)
      {
         switch (CheckHeader(line))
         {
         case (1):
         {
            // File has a header
            headerLine = true;
         }
         break;
         case (2):
         {
            auto out = ut::log::error() << "Unexpected header information in bathymetry file.";
            out.AddNote() << "File: " << aData.mFilename;
            return 2;
         }
         break;
         default:
            headerLine = false;
            break;
         }
      }

      // If not a header line then parse
      // Longitude, Latitude, Depth (decimal degrees, decimal degrees, meters)
      if (!headerLine)
      {
         ProcessLine(line, lat, lon, depth);

         // Save the data
         lats.emplace_back(lat);
         lons.emplace_back(lon);
         depths.emplace_back(depth);
      }

      ++lineNum;
   }
   ifs.close();

   // Check for ascending longitude
   if ((lons.size() > 1) && (lons[1] < lons[0]))
   {
      auto out = ut::log::error() << "Longitudes must be in ascending order array sizes in bathymetry file.";
      out.AddNote() << "File: " << aData.mFilename;
      return 2;
   }

   // Remove duplicate elements from lat and lon
   std::sort(lats.begin(), lats.end());
   auto last = std::unique(lats.begin(), lats.end());
   lats.erase(last, lats.end());
   std::sort(lons.begin(), lons.end());
   last = std::unique(lons.begin(), lons.end());
   lons.erase(last, lons.end());

   // Data is expected to be a grid with a constant cell size
   if (depths.size() != lats.size() * lons.size())
   {
      auto out = ut::log::error() << "Unexpected array sizes in bathymetry file.";
      out.AddNote() << "File: " << aData.mFilename;
      return 2;
   }

   mLatPoints = ut::safe_cast<unsigned int, size_t>(lats.size());
   mLonPoints = ut::safe_cast<unsigned int, size_t>(lons.size());

   // Latitude intervals
   if (mLatPoints > 0)
   {
      mLatInterval      = (aData.mLatRange.second - aData.mLatRange.first) / mLatPoints;
      mHalfLatInterval  = 0.5 * mLatInterval;
      mRecipLatInterval = 1.0 / mLatInterval;
   }
   else
   {
      auto out = ut::log::error() << "One or more latitudes required in bathymetry file.";
      out.AddNote() << "File: " << aData.mFilename;
      return 2;
   }

   // Longitude intervals
   if (mLonPoints > 0)
   {
      mLonInterval      = (aData.mLonRange.second - aData.mLonRange.first) / mLonPoints;
      mHalfLonInterval  = 0.5 * mLonInterval;
      mRecipLonInterval = 1.0 / mLonInterval;
   }
   else
   {
      auto out = ut::log::error() << "One or more longitudes required in bathymetry file.";
      out.AddNote() << "File: " << aData.mFilename;
      return 2;
   }

   // Set tile extents
   SetSWLat(aData.mLatRange.first + mLatInterval);
   SetSWLon(aData.mLonRange.first + mLonInterval);
   SetNELat(aData.mLatRange.second + mLatInterval);
   SetNELon(aData.mLonRange.second + mLonInterval);

   // Allocate the space for the actual elevation data
   unsigned totalPoints = mLonPoints * mLatPoints;
   CreateData(totalPoints);

   // May need to transpose data
   if (aData.mAscendingLatitudes)
   {
      memcpy(mData.mAsFloat, &depths[0], depths.size() * sizeof(float));
   }
   else
   {
      // Data is from NW to SE; ned to re-order from SW to NE
      std::vector<float> newDepths;
      newDepths.resize(totalPoints);
      for (unsigned int latIdx = 0; latIdx < mLatPoints; ++latIdx)
      {
         for (unsigned int lonIdx = 0; lonIdx < mLonPoints; ++lonIdx)
         {
            unsigned int newIdx = mLonPoints * (mLatPoints - 1 - latIdx) + lonIdx;
            unsigned int oldIdx = latIdx * mLonPoints + lonIdx;
            newDepths[newIdx]   = depths[oldIdx];
         }
      }
      memcpy(mData.mAsFloat, &newDepths[0], newDepths.size() * sizeof(float));
   }

   return 0;
}

// =================================================================================================
int Geodata::BathymetryXYZ_Tile::CheckHeader(const std::string& aHeader)
{
   // Copy the string
   std::string header(aHeader);

   // Convert to lower case
   UtStringUtil::ToLower(header);

   // Check for several header key words

   // x y z
   size_t x = header.find("x");
   size_t y = header.find("y");
   if (x != std::string::npos)
   {
      if ((y != std::string::npos) && (x < y))
      {
         return 1;
      }
      return 2;
   }

   // longitude latitude
   // ===       ===
   size_t lon = header.find("lon");
   size_t lat = header.find("lat");
   if (lon != std::string::npos)
   {
      if ((lat != std::string::npos) && (lon < lat))
      {
         return 1;
      }
      return 2;
   }

   // east north
   size_t east  = header.find("east");
   size_t north = header.find("north");
   if (east != std::string::npos)
   {
      if ((north != std::string::npos) && (east < north))
      {
         return 1;
      }
      return 2;
   }

   return 0;
}

// =================================================================================================
void Geodata::BathymetryXYZ_Tile::ProcessLine(std::string& aLine, float& aLatitude, float& aLongitude, float& aDepth)
{
   // Check for commas, semicolons
   // Don't need to do anything for tabs or spaces
   if (aLine.find(",") != std::string::npos)
   {
      // comma separated
      UtStringUtil::ReplaceAll(aLine, ",", " ");
   }
   else if (aLine.find(";") != std::string::npos)
   {
      // semicolon separated
      UtStringUtil::ReplaceAll(aLine, ";", " ");
   }

   // Read in the line
   std::istringstream iss(aLine);
   iss >> aLongitude >> aLatitude >> aDepth;
}

// =================================================================================================
// =================================================================================================
Geodata::BathymetryXYZ_Tile* Geodata::BathymetryXYZ_TileCache::UseTile(size_t aIndex)
{
   BathymetryXYZ_Tile* tilePtr   = nullptr;
   auto                cacheIter = mCache.find(aIndex);
   if (cacheIter != mCache.end())
   {
      tilePtr               = (*cacheIter).second.get();
      unsigned int useCount = tilePtr->IncrementUseCount();
#ifdef DEBUG_ON
      { // RAII block
         auto out = ut::log::debug() << "Release tile:";
         out.AddNote() << "From: " << tilePtr->GetSWLat() << ", " << tilePtr->GetSWLon();
         out.AddNote() << "To: " << tilePtr->GetNELat() << ", " << tilePtr->GetNELon();
         out.AddNote() << "Use Count: " << useCount;
      }
#endif
      if (useCount == 1)
      {
         // When the use count goes from 0 to 1 it means the tile is going
         // from inactive to active.  Therefore we must remove it from the
         // inactive list...
         mInactive.remove(tilePtr->GetBathymetryIndex());
      }
   }
   return tilePtr;
}

// =================================================================================================
void Geodata::BathymetryXYZ_TileCache::ReleaseTile(BathymetryXYZ_Tile* aTilePtr)
{
   unsigned int useCount = aTilePtr->DecrementUseCount();
#ifdef DEBUG_ON
   { // RAII block
      auto out = ut::log::debug() << "Release tile:";
      out.AddNote() << "From: " << aTilePtr->GetSWLat() << ", " << aTilePtr->GetSWLon();
      out.AddNote() << "To: " << aTilePtr->GetNELat() << ", " << aTilePtr->GetNELon();
      out.AddNote() << "Use Count: " << useCount;
   }
#endif
   if (useCount == 0)
   {
      mInactive.push_front(aTilePtr->GetBathymetryIndex()); // Add to inactive list...
   }
}

// =================================================================================================
void Geodata::BathymetryXYZ_TileCache::PurgeInactive(double aMaxDataSize)
{
   if (mSizeInBytes > aMaxDataSize)
   {
      auto li = mInactive.end();
      while ((mSizeInBytes > aMaxDataSize) && (!mInactive.empty()))
      {
         --li;
         size_t tileIndex = *li;
#ifdef DEBUG_ON
         { // RAII block
            auto out = ut::log::debug() << "Purge tile:";
            out.AddNote() << "From: " << tilePtr->GetSWLat() << ", " << tilePtr->GetSWLon();
            out.AddNote() << "To: " << tilePtr->GetNELat() << ", " << tilePtr->GetNELon();
         }
#endif
         auto cacheIter = mCache.find(tileIndex);
         if (cacheIter != mCache.end())
         {
            mSizeInBytes -= cacheIter->second->GetDataSize();
            mCache.erase(cacheIter);
         }
      }
      mInactive.erase(li, mInactive.end());
   }
}

// =================================================================================================
void Geodata::BathymetryXYZ_TileCache::PurgeInactive(unsigned int aMaxDataSize)
{
   if (mInactive.size() > aMaxDataSize)
   {
      auto li = mInactive.end();
      while (mInactive.size() > aMaxDataSize)
      {
         --li;
         size_t index = *li;
#ifdef DEBUG_ON
         { // RAII block
            auto out = ut::log::debug() << "Purge tile:";
            out.AddNote() << "From: " << tilePtr->GetSWLat() << ", " << tilePtr->GetSWLon();
            out.AddNote() << "To: " << tilePtr->GetNELat() << ", " << tilePtr->GetNELon();
         }
#endif
         auto cacheIter = mCache.find(index);
         if (cacheIter != mCache.end())
         {
            mSizeInBytes -= cacheIter->second->GetDataSize();
            mCache.erase(cacheIter);
         }
      }
      mInactive.erase(li, mInactive.end());
   }
}

// =================================================================================================
void Geodata::BathymetryXYZ_TileCache::AddTile(std::unique_ptr<BathymetryXYZ_Tile> aTilePtr)
{
#ifdef DEBUG_ON
   auto out = ut::log::debug() << "Add tile:";
   out.AddNote() << "From: " << aTilePtr->GetSWLat() << ", " << aTilePtr->GetSWLon();
   out.AddNote() << "To: " << aTilePtr->GetNELat() << ", " << aTilePtr->GetNELon();
   out.AddNote() << "Use Count: " << useCount;
#endif
   mSizeInBytes += aTilePtr->GetDataSize();
   auto index = aTilePtr->GetBathymetryIndex();
   mCache.emplace(index, std::move(aTilePtr));
}

// =================================================================================================
void Geodata::BathymetryXYZ_TileCache::Reset()
{
   mSizeInBytes = 0;
   mCache.clear();
   mInactive.clear();
}
