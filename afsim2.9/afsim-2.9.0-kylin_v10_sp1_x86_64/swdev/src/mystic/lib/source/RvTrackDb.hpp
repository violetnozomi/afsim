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
#ifndef RVWSFRESULTTRACKDB_HPP
#define RVWSFRESULTTRACKDB_HPP
#include "RvExport.hpp"
#include "RvResultDb.hpp"

namespace rv
{
class RV_EXPORT TrackDb
{
public:
   void AddMessage(MsgLocalTrackCreated* aMsg, bool aAtBegin);
   void RemoveMessage(MsgLocalTrackCreated* aMsg, bool aAtBegin);
   void AddMessage(MsgLocalTrackDrop* aMsg, bool aAtBegin);
   void RemoveMessage(MsgLocalTrackDrop* aMsg, bool aAtBegin);
   void AddMessage(MsgLocalTrackUpdate* aMsg, bool aAtBegin);
   void RemoveMessage(MsgLocalTrackUpdate* aMsg, bool aAtBegin);

   void AddMessage(MsgSensorTrackCreated* aMsg, bool aAtBegin);
   void RemoveMessage(MsgSensorTrackCreated* aMsg, bool aAtBegin);
   void AddMessage(MsgSensorTrackDrop* aMsg, bool aAtBegin);
   void RemoveMessage(MsgSensorTrackDrop* aMsg, bool aAtBegin);
   void AddMessage(MsgSensorTrackUpdate* aMsg, bool aAtBegin);
   void RemoveMessage(MsgSensorTrackUpdate* aMsg, bool aAtBegin);

   // this represents all the information we have about a given track
   template<typename MessageUpdateType, typename MessageCreatedType>
   struct TrackList
   {
      Track_Id GetTrackId() const;

      MessageArray<MessageUpdateType> mUpdates;
      MessageCreatedType*             mMsgCreate      = nullptr;
      bool                            mMsgCreateAlive = true;
      float                           mDropTime       = std::numeric_limits<float>::max();
   };

   using LocalTrackList  = TrackList<MsgLocalTrackUpdate, MsgLocalTrackCreated>;
   using SensorTrackList = TrackList<MsgSensorTrackUpdate, MsgSensorTrackCreated>;

   struct CompareTrackId
   {
      bool operator()(const Track_Id& aLhs, const Track_Id& aRhs) const
      {
         if (aLhs.localTrackNumber() < aRhs.localTrackNumber())
            return true;
         if (aLhs.localTrackNumber() > aRhs.localTrackNumber())
            return false;
         return aLhs.owner() < aRhs.owner();
      }
   };

   // this is like a given platforms track map
   struct SourceTracks
   {
      SourceTracks()
         : mUpdateCount(0)
      {
      }
      void AddMessage(MsgLocalTrackCreated* aMsg, bool aAtBegin);
      void RemoveMessage(MsgLocalTrackCreated* aMsg, bool aAtBegin);
      void AddMessage(MsgLocalTrackDrop* aMsg, bool aAtBegin);
      void RemoveMessage(MsgLocalTrackDrop* aMsg, bool aAtBegin);
      void AddMessage(MsgLocalTrackUpdate* aMsg, bool aAtBegin);
      void RemoveMessage(MsgLocalTrackUpdate* aMsg, bool aAtBegin);

      void AddMessage(MsgSensorTrackCreated* aMsg, bool aAtBegin);
      void RemoveMessage(MsgSensorTrackCreated* aMsg, bool aAtBegin);
      void AddMessage(MsgSensorTrackDrop* aMsg, bool aAtBegin);
      void RemoveMessage(MsgSensorTrackDrop* aMsg, bool aAtBegin);
      void AddMessage(MsgSensorTrackUpdate* aMsg, bool aAtBegin);
      void RemoveMessage(MsgSensorTrackUpdate* aMsg, bool aAtBegin);

      using LocalTrackMap  = std::map<Track_Id, LocalTrackList, CompareTrackId>;
      using SensorTrackMap = std::map<Track_Id, SensorTrackList, CompareTrackId>;
      LocalTrackMap  mLocalTracks;
      SensorTrackMap mSensorTracks;
      unsigned int   mUpdateCount;
   };

   // this is the interface to get a given platform's track map
   const SourceTracks* FindOwnerTracks(unsigned int aOwnerIndex) const;

private:
   std::map<unsigned int, SourceTracks> mSourceTracks;
};

template<typename MessageUpdateType, typename MessageCreatedType>
Track_Id TrackDb::TrackList<MessageUpdateType, MessageCreatedType>::GetTrackId() const
{
   if (mMsgCreate != nullptr)
   {
      return mMsgCreate->trackId();
   }
   return rv::Track_Id();
}

} // namespace rv
#endif
