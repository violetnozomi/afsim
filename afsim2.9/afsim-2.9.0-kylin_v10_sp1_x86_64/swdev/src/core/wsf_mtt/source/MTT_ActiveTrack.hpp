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

#ifndef MTT_ACTIVE_TRACK_HPP
#define MTT_ACTIVE_TRACK_HPP

#include "wsf_mtt_export.h"

#include <fstream>
#include <string>

#include "MTT_Track.hpp"
#include "SupBlock.hpp"
#include "UtMatrix.hpp"
class WsfTrack;
class WsfLocalTrack;
class WsfMTT_ReferencePoint;
//! Corresponds with Suppressor block 387.
class WSF_MTT_EXPORT MTT_ActiveTrack : public MTT_Track
{
public:
   class Data : public MTT_Track::Data
   {
   public:
      Data()
         : mHorizontalInfoMatrix(6, 6)
         , mFilterStates(6)
         , mVerticalInfoMatrix(2, 2)
         , mVerticalState(2)
         , mStraightFlightModeProbability(0.0)
         , mTurningFlightModeProbability(0.0)
         , mPreviousFilterStates(6)
         , mPreviousHorizontalMatrix(6, 6)
      {
      }

      MTT_Track::Data* Clone() const override { return new Data(*this); }

      ~Data() override {}
      // Suppressor Offset (1 offset)
      UtMatrixd mHorizontalInfoMatrix; // 1-36
      UtMatrixd mFilterStates;         // 37-42
      UtMatrixd mVerticalInfoMatrix;   // 43-46
      UtMatrixd mVerticalState;        // 47-48

      double    mStraightFlightModeProbability; // 49
      double    mTurningFlightModeProbability;  // 50
      UtMatrixd mPreviousFilterStates;          // 51-56
      UtMatrixd mPreviousHorizontalMatrix;      // 57-92

      void ConvertFrom(const SupBlock& aBlock) override
      {
         mHorizontalInfoMatrix.CopyFrom(aBlock.mDoubleBlock);
         mFilterStates.CopyFrom(aBlock.mDoubleBlock + 36);
         mVerticalInfoMatrix.CopyFrom(aBlock.mDoubleBlock + 42);
         mVerticalState.CopyFrom(aBlock.mDoubleBlock + 46);
         mStraightFlightModeProbability = aBlock.mDoubleBlock[48];
         mTurningFlightModeProbability  = aBlock.mDoubleBlock[49];
         mPreviousFilterStates.CopyFrom(aBlock.mDoubleBlock + 50);
         mPreviousHorizontalMatrix.CopyFrom(aBlock.mDoubleBlock + 56);
      }
      void Log(std::fstream& aFS) override
      {
         aFS << mHorizontalInfoMatrix << std::endl;
         aFS << mFilterStates << std::endl;
         aFS << mVerticalInfoMatrix << std::endl;
         aFS << mVerticalState << std::endl;
         aFS << mStraightFlightModeProbability << std::endl;
         aFS << mTurningFlightModeProbability << std::endl;
         aFS << mPreviousFilterStates << std::endl;
         aFS << mPreviousHorizontalMatrix << std::endl;
      }
   };

public:
   MTT_ActiveTrack();

   MTT_ActiveTrack(const MTT_ActiveTrack& aSrc);

   ~MTT_ActiveTrack() override = default;

   void SetData(const Data& aData);

   MTT_ActiveTrack::Data& GetData() const;

   void CopyTo(double aSimTime, const WsfMTT_ReferencePoint& aReferencePoint, WsfLocalTrack& aTrack) const;

   void CopyAuxDataTo(WsfLocalTrack& aTrack) const;

   void CopyFrom(double aSimTime, const WsfTrack& aTrack);
};

#endif
