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
#ifndef UTORENDERBINMANAGER_H
#define UTORENDERBINMANAGER_H

#include "utilosg_export.h"

#include <list>
#include <vector>

class UTILOSG_EXPORT UtoRenderBinUser
{
public:
   UtoRenderBinUser(int aSortId);
   virtual ~UtoRenderBinUser();

   void CutToFrontRenderBin();

   void         AssignRenderBinId();
   unsigned int GetRenderBinId() { return mRenderBinId; }
   void         SetRenderBinId(unsigned int aId) { mRenderBinId = aId; }
   virtual void SetRenderBin(int aBin) = 0;
   int          GetSortId() { return mRenderBinSortId; }

private:
   unsigned int mRenderBinId;
   int          mRenderBinSortId;
   bool         mRenderBinAssigned; // indicates that the user is known by the manager
};

// this class is used by things that need screen space sorting
// users will request to be added with GetId(), which will assign an
// id and renderbin.  Users can request to be put to the front using
// PushToFront
class UTILOSG_EXPORT UtoRenderBinManager
{
public:
   UtoRenderBinManager();
   virtual ~UtoRenderBinManager();

   static UtoRenderBinManager* Instance();
   void                        PushToFront(UtoRenderBinUser* aUser);
   void                        Add(UtoRenderBinUser* aUser);
   void                        Remove(UtoRenderBinUser* aUser);
   void                        AssignId(UtoRenderBinUser* aUser);

   std::vector<std::pair<unsigned int, int>>::const_iterator
   GetForemostElement(const std::vector<std::pair<unsigned int, int>>& aElementList) const;

   bool SetViewAltitude(float aAltitude);
   void SetCloudAltitude(float aAltitude);
   int  GetTerrainLayerBin() const;
   int  GetCloudLayerBin() const;
   int  GetModelLayerBin(float aAltitude) const;

private:
   int        mMaxId;
   int        mNextId;
   static int msFirstBin;

   int   mTerrainLayer;
   int   mBeforeCloudLayer;
   int   mCloudLayer;
   float mCloudAltitude;
   int   mBeyondCloudLayer;
   bool  mAboveClouds;

   typedef std::list<UtoRenderBinUser*> BinList;

   BinList                     mBinMap;
   static UtoRenderBinManager* msPointer;
};

#endif // UTORENDERBINMANAGER_H
