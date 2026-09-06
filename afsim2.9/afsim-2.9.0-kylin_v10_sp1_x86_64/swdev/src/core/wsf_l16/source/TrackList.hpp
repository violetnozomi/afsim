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
#ifndef L16_TRACKLIST_HPP
#define L16_TRACKLIST_HPP

#include <map>

class WsfTrack;

namespace WsfL16
{
//! Represent a track id as a source track number and a track number
//! (similar to WsfTrackId in WSF).  Mapping to the source track number
//! is performed because one can expect sporadic episodes of two STNs
//! reporting the same track number at the same time.
class TrackId
{
public:
   TrackId(unsigned aSourceTrackNumber, unsigned aTrackNumber);

   unsigned mSourceTrackNumber;
   unsigned mTrackNumber;

   bool operator<(const TrackId& aRhs) const;
};

//! Represent a track list as a map.
//! Provide methods to Add and Find tracks in the map.
class TrackList
{
public:
   typedef std::map<TrackId, WsfTrack*> TrackMap;
   TrackList();
   ~TrackList();

   void Add(TrackId aTrackId, WsfTrack* aTrack);

   WsfTrack* Find(TrackId aTrackId);

   TrackMap::const_iterator Begin() const { return mTracks.begin(); }
   TrackMap::const_iterator End() const { return mTracks.end(); }

protected:
   TrackMap mTracks;
};
} // namespace WsfL16

#endif
