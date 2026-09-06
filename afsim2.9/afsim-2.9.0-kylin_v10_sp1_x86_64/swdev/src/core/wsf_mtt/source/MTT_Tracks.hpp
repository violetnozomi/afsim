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

#ifndef MTT_TRACKS_HPP
#define MTT_TRACKS_HPP

#include "MTT_ActiveTrack.hpp"
#include "MTT_CandidateTrack.hpp"
#include "MTT_EmbryonicTrack.hpp"
class SupBlock;

#include "UtGenericMappedList.hpp"

class MTT_Tracks // corresponds with block 383, "MTT Header"
{
public:
   virtual ~MTT_Tracks() = default;

   using MTT_EmbryonicTrackList = UtStdMappedList<MTT_EmbryonicTrack, int>; // A track list indexed by track id.
   using MTT_CandidateTrackList = UtStdMappedList<MTT_CandidateTrack, int>;
   using MTT_ActiveTrackList    = UtStdMappedList<MTT_ActiveTrack, int>;

   // Suppressor Offset List
   int                    mEmbryonicTrackCounter{0}; // 00
   MTT_EmbryonicTrackList mEmbryonicTracks;          // 01
   int                    mCandidateTrackCounter{0}; // 02
   MTT_CandidateTrackList mCandidateTracks;          // 03
   int                    mActiveTrackCounter{0};    // 04
   MTT_ActiveTrackList    mActiveTrackList;          // 05

   int mAssignmentIdCounter{0}; // 06    Necessary?

   void ConvertFrom(const SupBlock& aBlock);
};

#endif
