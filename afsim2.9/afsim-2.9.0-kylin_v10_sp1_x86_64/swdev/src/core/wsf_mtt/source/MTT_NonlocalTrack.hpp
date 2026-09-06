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

#ifndef MTT_NONLOCAL_TRACK_HPP
#define MTT_NONLOCAL_TRACK_HPP

#include "SupBlock.hpp"
#include "UtMatrix.hpp"

//! This is a representation of a non-local (or, combined, or "active" track);
//! corresponding with a Suppressor 363 block.
class MTT_NonlocalTrack
{
public:
   class Filter
   {
   public:
      // Aggrregated Filter data (367 block).
      Filter()
         : mUpdateTime(0.0)
         , mFilteredState(6)
         , mInformationMatrix(6, 6)
         , mLastVerticalUpdateTime(0.0)
         , mX_Acceleration(0.0)
         , mY_Acceleration(0.0)
         , mVerticalInformationMatrix(2, 2)
         , mPreviousUpdateTime(0.0)
         , mPreviousFilteredState(6)
         , mPreviousInformationMatrix(6, 6)
      {
      }
      ~Filter() {}

      double    mUpdateTime;    //+1, 0
      UtMatrixd mFilteredState; //+1, 1-6)
      // 1: x position
      // 2: y position
      // 3: x velocity
      // 4: y velocity
      // 5: z position
      // 6: z velocity
      UtMatrixd mInformationMatrix;         //(+1, 7-42)
      double    mLastVerticalUpdateTime;    //(+1, 43)
      double    mX_Acceleration;            //(+1, 44)
      double    mY_Acceleration;            //(+1, 45)
      UtMatrixd mVerticalInformationMatrix; //(+1, 46-49)
      double    mPreviousUpdateTime;        //(+1, 50)
      UtMatrixd mPreviousFilteredState;     //(+1, 51-56)
      UtMatrixd mPreviousInformationMatrix; //(+1, 57-92)
   };

public:
   MTT_NonlocalTrack()
      : mFilter()
      , mHasVerticalData(false)
      , mUpdateFlag(0)
   {
   }

   virtual ~MTT_NonlocalTrack() {}

   Filter   mFilter;
   bool     mHasVerticalData; //+4
   unsigned mUpdateFlag;      //+8  : 1 == 1d (height finder)
   //    : 2 == 2d
   //    : 3 == 3d

   void ConvertFrom(const SupBlock& aBlock)
   {
      mHasVerticalData = (aBlock.mIntBlock[4] != 0);
      mUpdateFlag      = aBlock.mIntBlock[8];
   }
};

typedef MTT_NonlocalTrack MTT_CombinedTrack; // Because a Suppressor data block is a terrible thing to waste.

#endif
