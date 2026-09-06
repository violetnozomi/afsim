// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef MTT_TRACK_HPP
#define MTT_TRACK_HPP

#include <fstream>

#include "WsfStringId.hpp"
class InteractionKey; // Hopefully don't have to further define this
#include "SupBlock.hpp"

class MTT_Track
{
public:
   // Base class for track "data"
   class Data
   {
   public:
      virtual ~Data()             = default;
      virtual Data* Clone() const = 0;
      virtual void  ConvertFrom(const SupBlock& aBlock) {}
      virtual void  Log(std::fstream& aFS) {}
   };

public:
   MTT_Track()
      : mUpdateTime(0.0)
      , mTrackId(0)
      , mDataPtr(nullptr)
      , mAssignmentId(0)
      , mOriginatorId(nullptr)
      , mSensorId(nullptr)
      , mVerticalChannelActive(false)
      , mVerticalChannelUpdateTime(0.0)
   {
   }

   MTT_Track(const MTT_Track& aSrc)
      : mUpdateTime(aSrc.mUpdateTime)
      , mTrackId(aSrc.mTrackId)
      , mDataPtr(aSrc.mDataPtr->Clone())
      , mAssignmentId(aSrc.mAssignmentId)
      , mOriginatorId(aSrc.mOriginatorId)
      , mSensorId(aSrc.mSensorId)
      , mVerticalChannelActive(aSrc.mVerticalChannelActive)
      , mVerticalChannelUpdateTime(aSrc.mVerticalChannelActive)
   {
   }

   MTT_Track& operator=(const MTT_Track&) = delete;

   virtual ~MTT_Track() { delete mDataPtr; }

   int GetId() const { return mTrackId; }
   // Suppressor offset ID
   double mUpdateTime;   // 01
   int    mTrackId;      // 02
   Data*  mDataPtr;      // 03
   int    mAssignmentId; // 04
   // InteractionKey* mCommanderPtr;               // 05 Commander that sent this assignment
   WsfStringId mOriginatorId;              // (wsf only)
   WsfStringId mSensorId;                  // (wsf only)
   bool        mVerticalChannelActive;     // 06
   double      mVerticalChannelUpdateTime; // 07

   void ConvertFrom(const SupBlock& aBlock)
   {
      mUpdateTime                = aBlock.mFloatBlock[1];
      mTrackId                   = aBlock.mIntBlock[2];
      mAssignmentId              = aBlock.mIntBlock[4];
      mVerticalChannelActive     = (aBlock.mIntBlock[6] != 0);
      mVerticalChannelUpdateTime = (aBlock.mFloatBlock[7]);
   }

   virtual void Log(std::fstream& aFS)
   {
      aFS << mUpdateTime << std::endl;
      aFS << mTrackId << std::endl;
      aFS << mAssignmentId << std::endl;
      aFS << mVerticalChannelActive << std::endl;
      aFS << mVerticalChannelUpdateTime << std::endl;
      mDataPtr->Log(aFS);
   }
};

#endif
