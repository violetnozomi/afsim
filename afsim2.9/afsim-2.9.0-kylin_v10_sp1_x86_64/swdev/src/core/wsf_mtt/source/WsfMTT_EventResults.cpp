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

#include "WsfMTT_EventResults.hpp"

#include <ostream>

#include "MTT_ActiveTrack.hpp"
#include "MTT_CandidateTrack.hpp"
#include "MTT_EmbryonicTrack.hpp"
#include "WsfEventUtils.hpp"
#include "WsfPlatform.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"

namespace wsf
{
namespace event
{

void ActiveTrackInitiated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MTT_ACTIVE_TRACK_INITIATED ";
   aStream << mTrackManagerPtr->GetPlatform()->GetName();
   aStream << " Active_Track: " << mActivePtr->GetId();
   if (mCandidatePtr != nullptr)
   {
      aStream << " Candidate_Track: " << mCandidatePtr->GetId();
   }
   aStream << '\n';
}

void ActiveTrackInitiated::PrintCSV(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << ',' << "MTT_ACTIVE_TRACK_INITIATED";
   aStream << ',' << mTrackManagerPtr->GetPlatform()->GetName();
   aStream << ',' << mActivePtr->GetId();
   if (mCandidatePtr != nullptr)
   {
      aStream << ',' << mCandidatePtr->GetId();
   }
   aStream << '\n';
}

int ActiveTrackInitiated::GetTrackID() const
{
   return mActivePtr != nullptr ? mActivePtr->GetId() : 0;
}

int ActiveTrackInitiated::GetCandidateTrackID() const
{
   return mCandidatePtr != nullptr ? mCandidatePtr->GetId() : 0;
}

size_t ActiveTrackInitiated::GetPlatformIndex() const
{
   return mTrackManagerPtr != nullptr ? mTrackManagerPtr->GetPlatform()->GetIndex() : 0;
}

void ActiveTrackDropped::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MTT_ACTIVE_TRACK_DROPPED " << mTrackManagerPtr->GetPlatform()->GetName();
   aStream << " Active_Track: " << mActivePtr->GetId() << '\n';
}

void ActiveTrackDropped::PrintCSV(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << ',' << "MTT_ACTIVE_TRACK_DROPPED";
   aStream << ',' << mTrackManagerPtr->GetPlatform()->GetName();
   aStream << ',' << mActivePtr->GetId();
   aStream << '\n';
}

int ActiveTrackDropped::GetTrackID() const
{
   return mActivePtr != nullptr ? mActivePtr->GetId() : 0;
}

size_t ActiveTrackDropped::GetPlatformIndex() const
{
   return mTrackManagerPtr != nullptr ? mTrackManagerPtr->GetPlatform()->GetIndex() : 0;
}

void CandidateTrackInitiated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MTT_CANDIDATE_TRACK_INITIATED " << mTrackManagerPtr->GetPlatform()->GetName();
   aStream << " Candidate_Track: " << mCandidatePtr->GetId();
   if (mEmbryonicPtr != nullptr)
   {
      aStream << " Embryonic_Track: " << mEmbryonicPtr->GetId();
   }
   aStream << '\n';
}

void CandidateTrackInitiated::PrintCSV(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << ',' << "MTT_CANDIDATE_TRACK_INITIATED";
   aStream << ',' << mTrackManagerPtr->GetPlatform()->GetName();
   aStream << ',' << mCandidatePtr->GetId();
   if (mEmbryonicPtr != nullptr)
   {
      aStream << ',' << mEmbryonicPtr->GetId();
   }
   aStream << '\n';
}

int CandidateTrackInitiated::GetTrackID() const
{
   return mCandidatePtr != nullptr ? mCandidatePtr->GetId() : 0;
}

int CandidateTrackInitiated::GetEmbryonicTrackID() const
{
   return mEmbryonicPtr != nullptr ? mEmbryonicPtr->GetId() : 0;
}

size_t CandidateTrackInitiated::GetPlatformIndex() const
{
   return mTrackManagerPtr != nullptr ? mTrackManagerPtr->GetPlatform()->GetIndex() : 0;
}

void CandidateTrackDropped::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MTT_CANDIDATE_TRACK_DROPPED " << mTrackManagerPtr->GetPlatform()->GetName();
   aStream << " Candidate_Track: " << mCandidatePtr->GetId() << '\n';
}

void CandidateTrackDropped::PrintCSV(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << ',' << "MTT_CANDIDATE_TRACK_DROPPED";
   aStream << ',' << mTrackManagerPtr->GetPlatform()->GetName();
   aStream << ',' << mCandidatePtr->GetId();
   aStream << '\n';
}

int CandidateTrackDropped::GetTrackID() const
{
   return mCandidatePtr != nullptr ? mCandidatePtr->GetId() : 0;
}

size_t CandidateTrackDropped::GetPlatformIndex() const
{
   return mTrackManagerPtr != nullptr ? mTrackManagerPtr->GetPlatform()->GetIndex() : 0;
}

void EmbryonicTrackInitiated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MTT_EMBRYONIC_TRACK_INITIATED " << mTrackManagerPtr->GetPlatform()->GetName();
   aStream << " Embryonic_Track: " << mEmbryonicPtr->GetId() << '\n';
}

void EmbryonicTrackInitiated::PrintCSV(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << ',' << "MTT_EMBRYONIC_TRACK_INITIATED";
   aStream << ',' << mTrackManagerPtr->GetPlatform()->GetName();
   aStream << ',' << mEmbryonicPtr->GetId();
   aStream << '\n';
}

int EmbryonicTrackInitiated::GetTrackID() const
{
   return mEmbryonicPtr != nullptr ? mEmbryonicPtr->GetId() : 0;
}

size_t EmbryonicTrackInitiated::GetPlatformIndex() const
{
   return mTrackManagerPtr != nullptr ? mTrackManagerPtr->GetPlatform()->GetIndex() : 0;
}

void EmbryonicTrackDropped::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MTT_EMBRYONIC_TRACK_DROPPED " << mTrackManagerPtr->GetPlatform()->GetName();
   aStream << " Embryonic_Track: " << mEmbryonicPtr->GetId() << '\n';
}

void EmbryonicTrackDropped::PrintCSV(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << ',' << "MTT_EMBRYONIC_TRACK_DROPPED";
   aStream << ',' << mTrackManagerPtr->GetPlatform()->GetName();
   aStream << ',' << mEmbryonicPtr->GetId();
   aStream << '\n';
}

int EmbryonicTrackDropped::GetTrackID() const
{
   return mEmbryonicPtr != nullptr ? mEmbryonicPtr->GetId() : 0;
}

size_t EmbryonicTrackDropped::GetPlatformIndex() const
{
   return mTrackManagerPtr != nullptr ? mTrackManagerPtr->GetPlatform()->GetIndex() : 0;
}

} // namespace event
} // namespace wsf
