// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFMTT_EVENTRESULTS_HPP
#define WSFMTT_EVENTRESULTS_HPP

#include "wsf_mtt_export.h"

class MTT_ActiveTrack;
class MTT_CandidateTrack;
class MTT_EmbryonicTrack;

#include "WsfEventResult.hpp"
class WsfTrackManager;

namespace wsf
{
namespace event
{

// ===================================================================================================
class WSF_MTT_EXPORT ActiveTrackInitiated : public Result
{
public:
   static constexpr const char* cNAME = "MTT_ACTIVE_TRACK_INITIATED";
   ActiveTrackInitiated(double              aSimTime,
                        WsfTrackManager*    aTrackManagerPtr,
                        MTT_ActiveTrack*    aActivePtr,
                        MTT_CandidateTrack* aCandidatePtr,
                        Settings            aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mTrackManagerPtr(aTrackManagerPtr)
      , mActivePtr(aActivePtr)
      , mCandidatePtr(aCandidatePtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   int    GetTrackID() const;
   int    GetCandidateTrackID() const;
   size_t GetPlatformIndex() const;

private:
   WsfTrackManager*    mTrackManagerPtr;
   MTT_ActiveTrack*    mActivePtr;
   MTT_CandidateTrack* mCandidatePtr;
};

// ===================================================================================================
class WSF_MTT_EXPORT ActiveTrackDropped : public Result
{
public:
   static constexpr const char* cNAME = "MTT_ACTIVE_TRACK_DROPPED";
   ActiveTrackDropped(double           aSimTime,
                      WsfTrackManager* aTrackManagerPtr,
                      MTT_ActiveTrack* aActivePtr,
                      Settings         aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mTrackManagerPtr(aTrackManagerPtr)
      , mActivePtr(aActivePtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   int    GetTrackID() const;
   size_t GetPlatformIndex() const;

private:
   WsfTrackManager* mTrackManagerPtr;
   MTT_ActiveTrack* mActivePtr;
};

// ===================================================================================================
class WSF_MTT_EXPORT CandidateTrackInitiated : public Result
{
public:
   static constexpr const char* cNAME = "MTT_CANDIDATE_TRACK_INITIATED";
   CandidateTrackInitiated(double              aSimTime,
                           WsfTrackManager*    aTrackManagerPtr,
                           MTT_CandidateTrack* aCandidatePtr,
                           MTT_EmbryonicTrack* aEmbryonicPtr,
                           Settings            aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mTrackManagerPtr(aTrackManagerPtr)
      , mCandidatePtr(aCandidatePtr)
      , mEmbryonicPtr(aEmbryonicPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   int    GetTrackID() const;
   int    GetEmbryonicTrackID() const;
   size_t GetPlatformIndex() const;

private:
   WsfTrackManager*    mTrackManagerPtr;
   MTT_CandidateTrack* mCandidatePtr;
   MTT_EmbryonicTrack* mEmbryonicPtr;
};

// ===================================================================================================
class WSF_MTT_EXPORT CandidateTrackDropped : public Result
{
public:
   static constexpr const char* cNAME = "MTT_CANDIDATE_TRACK_DROPPED";
   CandidateTrackDropped(double              aSimTime,
                         WsfTrackManager*    aTrackManagerPtr,
                         MTT_CandidateTrack* aCandidatePtr,
                         Settings            aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mTrackManagerPtr(aTrackManagerPtr)
      , mCandidatePtr(aCandidatePtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   int    GetTrackID() const;
   size_t GetPlatformIndex() const;

private:
   WsfTrackManager*    mTrackManagerPtr;
   MTT_CandidateTrack* mCandidatePtr;
};

// ===================================================================================================
class WSF_MTT_EXPORT EmbryonicTrackInitiated : public Result
{
public:
   static constexpr const char* cNAME = "MTT_EMBRYONIC_TRACK_INITIATED";
   EmbryonicTrackInitiated(double              aSimTime,
                           WsfTrackManager*    aTrackManagerPtr,
                           MTT_EmbryonicTrack* aEmbryonicPtr,
                           Settings            aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mTrackManagerPtr(aTrackManagerPtr)
      , mEmbryonicPtr(aEmbryonicPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   int    GetTrackID() const;
   size_t GetPlatformIndex() const;

private:
   WsfTrackManager*    mTrackManagerPtr;
   MTT_EmbryonicTrack* mEmbryonicPtr;
};

// ===================================================================================================
class WSF_MTT_EXPORT EmbryonicTrackDropped : public Result
{
public:
   static constexpr const char* cNAME = "MTT_EMBRYONIC_TRACK_INITIATED";
   EmbryonicTrackDropped(double              aSimTime,
                         WsfTrackManager*    aTrackManagerPtr,
                         MTT_EmbryonicTrack* aEmbryonicPtr,
                         Settings            aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mTrackManagerPtr(aTrackManagerPtr)
      , mEmbryonicPtr(aEmbryonicPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   int    GetTrackID() const;
   size_t GetPlatformIndex() const;

private:
   WsfTrackManager*    mTrackManagerPtr;
   MTT_EmbryonicTrack* mEmbryonicPtr;
};

} // namespace event
} // namespace wsf

#endif
