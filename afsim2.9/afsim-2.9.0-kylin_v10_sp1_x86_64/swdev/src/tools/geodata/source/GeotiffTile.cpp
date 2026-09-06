// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "GeotiffTile.hpp"

#include "UtException.hpp"
#include "UtTiff.hpp"

namespace
{
double ComponentSize(GeoLatLonGridTile::DataType aType)
{
   switch (aType)
   {
   case (GeoLatLonGridTile::cUNSIGNED_CHAR):
      return 1.0;
   case (GeoLatLonGridTile::cFLOAT):
      return sizeof(float);
   case (GeoLatLonGridTile::cDOUBLE):
      return sizeof(double);
   default:
      return 1.0;
   }
}
} // namespace

Geodata::GeotiffTile::GeotiffTile(size_t aIndex)
   : mGeotiffIndex(aIndex)
{
   SetDataType(cSHORT_INT);
   SetDataOrderType(cCOLUMN_MAJOR);
}

int Geodata::GeotiffTile::LoadTile(const TileData& aData)
{
   UtTiff::ComponentFormat format;
   int result = UtTiff::ReadElevationData(aData.mFilename, 0, format, mData.mAsVoid, mLatPoints, mLonPoints);
   if (result != 0)
   {
      return result;
   }
   mLatInterval = (aData.mLatRange.second - aData.mLatRange.first) / mLatPoints;
   mLonInterval = (aData.mLonRange.second - aData.mLonRange.first) / mLonPoints;
   switch (format)
   {
   case (UtTiff::cComponentFormatByte):
      mDataType = cUNSIGNED_CHAR;
      break;
   case (UtTiff::cComponentFormatDouble):
      mDataType = cDOUBLE;
      break;
   case (UtTiff::cComponentFormatFloat):
      mDataType = cFLOAT;
      break;
   case UtTiff::cComponentFormatInt16:
      mDataType = cSHORT_INT;
      break;
   default:
      return 2;
   }
   mSizeInBytes      = aData.mWidth * aData.mHeight * ComponentSize(mDataType);
   mHalfLatInterval  = 0.5 * mLatInterval;
   mHalfLonInterval  = 0.5 * mLonInterval;
   mRecipLatInterval = 1.0 / mLatInterval;
   mRecipLonInterval = 1.0 / mLonInterval;
   SetDataOrderType(cROW_MAJOR);
   SetSWLat(aData.mLatRange.first + mLatInterval);
   SetSWLon(aData.mLonRange.first);
   SetNELat(aData.mLatRange.second + mLonInterval);
   SetNELon(aData.mLonRange.second);

   return 0;
}

Geodata::GeotiffTile* Geodata::GeotiffTileCache::UseTile(size_t aIndex)
{
   GeotiffTile* tilePtr   = nullptr;
   auto         cacheIter = mCache.find(aIndex);
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
         mInactive.remove(tilePtr->GetGeotiffIndex());
      }
   }
   return tilePtr;
}

void Geodata::GeotiffTileCache::ReleaseTile(GeotiffTile* aTilePtr)
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
      mInactive.push_front(aTilePtr->GetGeotiffIndex()); // Add to inactive list...
   }
}

void Geodata::GeotiffTileCache::PurgeInactive(double aMaxDataSize)
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

void Geodata::GeotiffTileCache::PurgeInactive(unsigned int aMaxDataSize)
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

void Geodata::GeotiffTileCache::AddTile(std::unique_ptr<GeotiffTile> aTilePtr)
{
#ifdef DEBUG_ON
   auto out = ut::log::debug() << "Add tile:";
   out.AddNote() << "From: " << aTilePtr->GetSWLat() << ", " << aTilePtr->GetSWLon();
   out.AddNote() << "To: " << aTilePtr->GetNELat() << ", " << aTilePtr->GetNELon();
   out.AddNote() << "Use Count: " << useCount;
#endif
   mSizeInBytes += aTilePtr->GetDataSize();
   auto index = aTilePtr->GetGeotiffIndex();
   mCache.emplace(index, std::move(aTilePtr));
}

void Geodata::GeotiffTileCache::Reset()
{
   mSizeInBytes = 0;
   mCache.clear();
   mInactive.clear();
}
