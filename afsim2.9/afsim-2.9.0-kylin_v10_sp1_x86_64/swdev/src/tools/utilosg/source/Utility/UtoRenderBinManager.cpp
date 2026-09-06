// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtoRenderBinManager.hpp"

int UtoRenderBinManager::msFirstBin = 15; // we reserve 15+ for screen space sorting (as in comment bubbles)
UtoRenderBinManager* UtoRenderBinManager::msPointer = nullptr;

// Here is my attempt to reconstruct how we use renderbins:
// -2  Ephemeris (stuff that is really far away)
// 0   Terrain
// 1   Models-beyond the cloud layer (dynamically swaps when the camera crosses the cloud layer
// 2   cloud-layer
// 3   Models before the cloud layer (dynamically swaps when the camera crosses the cloud layer
// 14  Sensor Volumes
// 15+ Screen sorted elements (currently only comment bubbles)

// static
UtoRenderBinManager* UtoRenderBinManager::Instance()
{
   if (msPointer == nullptr)
   {
      msPointer = new UtoRenderBinManager();
   }
   return msPointer;
}

UtoRenderBinManager::UtoRenderBinManager()
   : mTerrainLayer(0)
   , mBeforeCloudLayer(3)
   , mCloudLayer(2)
   , mCloudAltitude(7000.0)
   , mBeyondCloudLayer(1)
   , mAboveClouds(true)
{
   mNextId = 0;
   if (msPointer == nullptr)
   {
      msPointer = this;
   }
}

// virtual
UtoRenderBinManager::~UtoRenderBinManager()
{
   mNextId = 0;
   mBinMap.clear();
}

void UtoRenderBinManager::AssignId(UtoRenderBinUser* aUser)
{
   aUser->SetRenderBinId(mNextId);
   ++mNextId;
}

void UtoRenderBinManager::PushToFront(UtoRenderBinUser* aUser)
{
   int bin = msFirstBin + (int)mBinMap.size() + 1;
   aUser->SetRenderBin(bin);
   mBinMap.push_front(aUser);
   --bin;

   std::list<UtoRenderBinUser*>::iterator it = mBinMap.begin();
   ++it;
   for (; it != mBinMap.end(); ++it)
   {
      if ((*it)->GetRenderBinId() == aUser->GetRenderBinId())
      {
         mBinMap.erase(it);
         break;
      }
      else
      {
         (*it)->SetRenderBin(bin);
         --bin;
      }
   }
}

void UtoRenderBinManager::Add(UtoRenderBinUser* aUser)
{
   aUser->SetRenderBinId(mNextId);
   ++mNextId;

   PushToFront(aUser);
}

void UtoRenderBinManager::Remove(UtoRenderBinUser* aUser)
{
   int bin = msFirstBin + (int)mBinMap.size() - 1;

   std::list<UtoRenderBinUser*>::iterator it;
   std::list<UtoRenderBinUser*>::iterator heldIt = mBinMap.end();

   for (it = mBinMap.begin(); it != mBinMap.end(); ++it)
   {
      if ((*it)->GetRenderBinId() == aUser->GetRenderBinId())
      {
         heldIt = it;
      }
      else
      {
         (*it)->SetRenderBin(bin);
         --bin;
      }
   }
   if (heldIt != mBinMap.end())
   {
      mBinMap.erase(heldIt);
   }
}

std::vector<std::pair<unsigned int, int>>::const_iterator
UtoRenderBinManager::GetForemostElement(const std::vector<std::pair<unsigned int, int>>& aElementList) const
{
   bool                                                      first     = true;
   int                                                       heldPlace = 0;
   std::vector<std::pair<unsigned int, int>>::const_iterator it;
   std::vector<std::pair<unsigned int, int>>::const_iterator heldIt = aElementList.end();
   std::list<UtoRenderBinUser*>::const_iterator              rbuIt;
   for (it = aElementList.begin(); it != aElementList.end(); ++it)
   {
      int id    = it->second;
      int place = 0;
      for (rbuIt = mBinMap.begin(); rbuIt != mBinMap.end(); ++rbuIt)
      {
         if (id == (*rbuIt)->GetSortId())
         {
            if (first)
            {
               heldIt    = it;
               heldPlace = place;
               first     = false;
            }
            else if (place < heldPlace)
            {
               heldIt    = it;
               heldPlace = place;
            }
         }
         ++place;
      }
   }
   return heldIt;
}

// return value indicate we have crossed the cloud layer
bool UtoRenderBinManager::SetViewAltitude(float aAltitude)
{
   if (((mAboveClouds) && (aAltitude < mCloudAltitude)) || ((!mAboveClouds) && (aAltitude > mCloudAltitude)))
   {
      mAboveClouds = (aAltitude > mCloudAltitude);
      return true;
   }
   return false;
}

void UtoRenderBinManager::SetCloudAltitude(float aAltitude)
{
   mCloudAltitude = aAltitude;
}

int UtoRenderBinManager::GetTerrainLayerBin() const
{
   return mTerrainLayer;
}

int UtoRenderBinManager::GetCloudLayerBin() const
{
   return mCloudLayer;
}

int UtoRenderBinManager::GetModelLayerBin(float aAltitude) const
{
   if (mAboveClouds)
   {
      if (aAltitude > mCloudAltitude)
      {
         return mBeforeCloudLayer;
      }
      else
      {
         return mBeyondCloudLayer;
      }
   }
   else
   {
      if (aAltitude > mCloudAltitude)
      {
         return mBeyondCloudLayer;
      }
      else
      {
         return mBeforeCloudLayer;
      }
   }
}

UtoRenderBinUser::UtoRenderBinUser(int aSortId)
{
   mRenderBinSortId   = aSortId;
   mRenderBinAssigned = false;
}

UtoRenderBinUser::~UtoRenderBinUser()
{
   if (mRenderBinAssigned)
   {
      UtoRenderBinManager::Instance()->Remove(this);
   }
}

void UtoRenderBinUser::CutToFrontRenderBin()
{
   if (mRenderBinAssigned)
   {
      UtoRenderBinManager::Instance()->PushToFront(this);
   }
}

void UtoRenderBinUser::AssignRenderBinId()
{
   UtoRenderBinManager::Instance()->AssignId(this);
   mRenderBinAssigned = true;
   CutToFrontRenderBin();
}
