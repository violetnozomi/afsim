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

#ifndef MTT_CANDIDATE_TRACK_HPP
#define MTT_CANDIDATE_TRACK_HPP

#include "MTT_EmbryonicTrack.hpp"
#include "MTT_Track.hpp"
#include "UtMatrix.hpp"

//! Corresponds with Suppressor block 386.
class MTT_CandidateTrack : public MTT_Track
{
public:
   // Candidate track data should be same as embryonic track data
   //      typedef MTT_EmbryonicTrack::Data Data;

   class Data : public MTT_Track::Data
   {
   public:
      Data()
         : mErrorInfoMatrix(4, 4)
         , mHorizontalState(4)
         , mVerticalErrorInfoMatrix(2, 2)
         , mVerticalState(2)
      {
      }

      MTT_Track::Data* Clone() const override { return new Data(*this); }

      ~Data() override {}

      void CopyFromEmbryonicData(const MTT_EmbryonicTrack::Data& aData)
      {
         mErrorInfoMatrix         = aData.mErrorInfoMatrix;
         mHorizontalState         = aData.mHorizontalState;
         mVerticalErrorInfoMatrix = aData.mVerticalErrorInfoMatrix;
         mVerticalState           = aData.mVerticalState;
      }

      void ConvertFrom(const SupBlock& aBlock) override
      {
         mErrorInfoMatrix.CopyFrom(aBlock.mDoubleBlock + 0);          // 0-31   (4x4 = 16 doubles)
         mHorizontalState.CopyFrom(aBlock.mDoubleBlock + 16);         // 32-39  (4x1 = 4 doubles)
         mVerticalErrorInfoMatrix.CopyFrom(aBlock.mDoubleBlock + 20); // 40-47  (2x2 = 4 doubles)
         mVerticalState.CopyFrom(aBlock.mDoubleBlock + 24);           // 48-51  (2x1 = 2 doubles)
      }

      void Log(std::fstream& aFS) override
      {
         aFS << mErrorInfoMatrix << std::endl;
         aFS << mHorizontalState << std::endl;
         aFS << mVerticalErrorInfoMatrix << std::endl;
         aFS << mVerticalState << std::endl;
      }
      // Suppressor offset (386 block)
      UtMatrixd mErrorInfoMatrix;         // 0-31   (4x4 = 16 doubles)
      UtMatrixd mHorizontalState;         // 32-39  (4x1 = 4 doubles)
      UtMatrixd mVerticalErrorInfoMatrix; // 40-47  (2x2 = 4 doubles)
      UtMatrixd mVerticalState;           // 48-51  (2x1 = 2 doubles)
   };

public:
   MTT_CandidateTrack() { mDataPtr = new MTT_CandidateTrack::Data(); }

   MTT_CandidateTrack(const MTT_CandidateTrack& aSrc)
      : MTT_Track(aSrc)
   {
   }

   ~MTT_CandidateTrack() override {}

   void SetData(const Data& aData)
   {
      if (mDataPtr != nullptr)
      {
         delete mDataPtr;
      }
      mDataPtr = new Data(aData);
   }

   MTT_CandidateTrack::Data& GetData() const { return *(static_cast<MTT_CandidateTrack::Data*>(mDataPtr)); }
};

#endif
