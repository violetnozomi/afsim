// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvTrackDb.hpp"


#define DISABLE_SRC_TRACKS 0

const rv::TrackDb::SourceTracks* rv::TrackDb::FindOwnerTracks(unsigned int aOwnerIndex) const
{
   std::map<unsigned int, SourceTracks>::const_iterator iter = mSourceTracks.find(aOwnerIndex);
   if (iter != mSourceTracks.end())
   {
      return &iter->second;
   }
   return nullptr;
}

void rv::TrackDb::RemoveMessage(rv::MsgLocalTrackUpdate* aMsg, bool aAtBegin)
{
#if !DISABLE_SRC_TRACKS
   mSourceTracks[aMsg->ownerIndex()].RemoveMessage(aMsg, aAtBegin);
#endif
}

void rv::TrackDb::RemoveMessage(MsgSensorTrackUpdate* aMsg, bool aAtBegin)
{
#if !DISABLE_SRC_TRACKS
   mSourceTracks[aMsg->ownerIndex()].RemoveMessage(aMsg, aAtBegin);
#endif
}

void rv::TrackDb::RemoveMessage(rv::MsgLocalTrackDrop* aMsg, bool aAtBegin)
{
   // ignore
}

void rv::TrackDb::RemoveMessage(rv::MsgSensorTrackDrop* aMsg, bool aAtBegin)
{
   // ignore
}

void rv::TrackDb::RemoveMessage(rv::MsgLocalTrackCreated* aMsg, bool aAtBegin)
{
#if !DISABLE_SRC_TRACKS
   mSourceTracks[aMsg->ownerIndex()].RemoveMessage(aMsg, aAtBegin);
#endif
}

void rv::TrackDb::RemoveMessage(rv::MsgSensorTrackCreated* aMsg, bool aAtBegin)
{
#if !DISABLE_SRC_TRACKS
   mSourceTracks[aMsg->ownerIndex()].RemoveMessage(aMsg, aAtBegin);
#endif
}

void rv::TrackDb::AddMessage(rv::MsgLocalTrackUpdate* aMsg, bool aAtBegin)
{
#if !DISABLE_SRC_TRACKS
   mSourceTracks[aMsg->ownerIndex()].AddMessage(aMsg, aAtBegin);
#endif
}

void rv::TrackDb::AddMessage(MsgSensorTrackUpdate* aMsg, bool aAtBegin)
{
#if !DISABLE_SRC_TRACKS
   mSourceTracks[aMsg->ownerIndex()].AddMessage(aMsg, aAtBegin);
#endif
}

void rv::TrackDb::AddMessage(rv::MsgLocalTrackDrop* aMsg, bool aAtBegin)
{
#if !DISABLE_SRC_TRACKS
   mSourceTracks[aMsg->ownerIndex()].AddMessage(aMsg, aAtBegin);
#endif
}

void rv::TrackDb::AddMessage(rv::MsgSensorTrackDrop* aMsg, bool aAtBegin)
{
#if !DISABLE_SRC_TRACKS
   mSourceTracks[aMsg->ownerIndex()].AddMessage(aMsg, aAtBegin);
#endif
}

void rv::TrackDb::AddMessage(rv::MsgLocalTrackCreated* aMsg, bool aAtBegin)
{
#if !DISABLE_SRC_TRACKS
   mSourceTracks[aMsg->ownerIndex()].AddMessage(aMsg, aAtBegin);
#endif
}

void rv::TrackDb::AddMessage(rv::MsgSensorTrackCreated* aMsg, bool aAtBegin)
{
#if !DISABLE_SRC_TRACKS
   mSourceTracks[aMsg->ownerIndex()].AddMessage(aMsg, aAtBegin);
#endif
}

void rv::TrackDb::SourceTracks::RemoveMessage(rv::MsgLocalTrackUpdate* aMsg, bool aAtBegin)
{
   rv::Track_Id    id = aMsg->track().trackId();
   LocalTrackList& tl = mLocalTracks[id];
   tl.mUpdates.pop(aMsg, aAtBegin);
   if (tl.mUpdates.empty() && !tl.mMsgCreate)
   {
      mLocalTracks.erase(id);
   }
   ++mUpdateCount;
}

void rv::TrackDb::SourceTracks::RemoveMessage(rv::MsgLocalTrackDrop* aMsg, bool aAtBegin)
{
   // ignore
}


void rv::TrackDb::SourceTracks::AddMessage(rv::MsgLocalTrackCreated* aMsg, bool aAtBegin)
{
   LocalTrackList& tracks = mLocalTracks[aMsg->trackId()];
   tracks.mMsgCreate      = aMsg;
   tracks.mMsgCreateAlive = true;
   ++mUpdateCount;
}

void rv::TrackDb::SourceTracks::RemoveMessage(rv::MsgLocalTrackCreated* aMsg, bool aAtBegin)
{
   rv::Track_Id    id = aMsg->trackId();
   LocalTrackList& tl = mLocalTracks[id];
   tl.mMsgCreate      = nullptr;
   tl.mMsgCreateAlive = false;
   if (tl.mUpdates.empty())
   {
      mLocalTracks.erase(id);
   }
   ++mUpdateCount;
}

void rv::TrackDb::SourceTracks::AddMessage(rv::MsgLocalTrackUpdate* aMsg, bool aAtBegin)
{
   LocalTrackList& tl = mLocalTracks[aMsg->track().trackId()];
   tl.mUpdates.push(aMsg, aAtBegin);
   ++mUpdateCount;
}

void rv::TrackDb::SourceTracks::AddMessage(rv::MsgLocalTrackDrop* aMsg, bool aAtBegin)
{
   LocalTrackList& tl = mLocalTracks[aMsg->trackId()];
   tl.mDropTime       = aMsg->simTime();
   ++mUpdateCount;
}


////
void rv::TrackDb::SourceTracks::RemoveMessage(rv::MsgSensorTrackUpdate* aMsg, bool aAtBegin)
{
   rv::Track_Id     id = aMsg->track().trackId();
   SensorTrackList& tl = mSensorTracks[id];
   tl.mUpdates.pop(aMsg, aAtBegin);
   if (tl.mUpdates.empty() && !tl.mMsgCreate)
   {
      mSensorTracks.erase(id);
   }
   ++mUpdateCount;
}

void rv::TrackDb::SourceTracks::RemoveMessage(rv::MsgSensorTrackDrop* aMsg, bool aAtBegin)
{
   // ignore
}


void rv::TrackDb::SourceTracks::AddMessage(rv::MsgSensorTrackCreated* aMsg, bool aAtBegin)
{
   SensorTrackList& tracks = mSensorTracks[aMsg->trackId()];
   tracks.mMsgCreate       = aMsg;
   tracks.mMsgCreateAlive  = true;
   ++mUpdateCount;
}

void rv::TrackDb::SourceTracks::RemoveMessage(rv::MsgSensorTrackCreated* aMsg, bool aAtBegin)
{
   rv::Track_Id     id = aMsg->trackId();
   SensorTrackList& tl = mSensorTracks[id];
   tl.mMsgCreate       = nullptr;
   tl.mMsgCreateAlive  = false;
   if (tl.mUpdates.empty())
   {
      mSensorTracks.erase(id);
   }
   ++mUpdateCount;
}

void rv::TrackDb::SourceTracks::AddMessage(rv::MsgSensorTrackUpdate* aMsg, bool aAtBegin)
{
   SensorTrackList& tl = mSensorTracks[aMsg->track().trackId()];
   tl.mUpdates.push(aMsg, aAtBegin);
   ++mUpdateCount;
}

void rv::TrackDb::SourceTracks::AddMessage(rv::MsgSensorTrackDrop* aMsg, bool aAtBegin)
{
   SensorTrackList& tl = mSensorTracks[aMsg->trackId()];
   tl.mDropTime        = aMsg->simTime();
   ++mUpdateCount;
}
