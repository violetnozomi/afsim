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
#include "TrackList.hpp"

#include "WsfTrack.hpp"

namespace WsfL16
{

TrackList::TrackList() {}

TrackList::~TrackList()
{
   for (TrackMap::iterator i = mTracks.begin(); i != mTracks.end(); ++i)
   {
      delete i->second;
   }
}

void TrackList::Add(TrackId aTrackId, WsfTrack* aTrack)
{
   mTracks[aTrackId] = aTrack;
}

WsfTrack* TrackList::Find(TrackId aTrackId)
{
   WsfTrack*          trackPtr = nullptr;
   TrackMap::iterator i        = mTracks.find(aTrackId);
   if (i != mTracks.end())
   {
      trackPtr = i->second;
   }
   return trackPtr;
}

TrackId::TrackId(unsigned aSourceTrackNumber, unsigned aTrackNumber)
   : mSourceTrackNumber(aSourceTrackNumber)
   , mTrackNumber(aTrackNumber)
{
}

bool TrackId::operator<(const TrackId& aRhs) const
{
   if (mSourceTrackNumber < aRhs.mSourceTrackNumber)
   {
      return true;
   }
   else if (mSourceTrackNumber > aRhs.mSourceTrackNumber)
   {
      return false;
   }
   return (mTrackNumber < aRhs.mTrackNumber);
}

} // namespace WsfL16
