// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFXIO_TRACKREQUEST_HPP
#define WSFXIO_TRACKREQUEST_HPP

#include "wsf_export.h"

#include "GenUniqueId.hpp"
#include "WsfStringId.hpp"
class WsfXIO_LocalTrackPkt;
class WsfXIO_RawTrackPkt;
#include "xio/WsfXIO_Request.hpp"
class WsfXIO_TrackDropPkt;

//! Represents a request for track information.
//! May be subclassed to handle received tracks
class WSF_EXPORT WsfXIO_TrackRequest : public WsfXIO_Request
{
public:
   //! Construct a WsfXIO_TrackRequest
   //! aRemotePlatformIndex is the index of the platform in the simulation that is receiving the request
   WsfXIO_TrackRequest(int aRemotePlatformIndex, WsfXIO_Connection* aConnectionPtr, bool aIsReliable = true);

   //! Sets the sensor name for the track request.  Must be used
   //! prior to submitting the request.  Default is empty.
   void SetSensorName(WsfStringId aSensorName) { mSensorNameId = aSensorName; }

   WsfStringId GetSensorName() const { return mSensorNameId; }

   //! Specifies that tracks already in the track manager will be sent
   //! upon accepted request
   void EnableSendInitialTracks() { mSendInitialTracks = true; }

   //! Specifies what kind of track data is required to be sent
   //! @param aRequiredData A combination of flags in WsfXIO_RequestTracksPkt::TrackData
   void SetRequiredData(int aRequiredData) { mRequiredData = aRequiredData; }

   //! Handle() is called when a messages sent as a response to this request
   //! are received.
   virtual void Handle(const WsfXIO_RawTrackPkt& aPkt) {}
   virtual void Handle(const WsfXIO_LocalTrackPkt& aPkt) {}
   virtual void Handle(const WsfXIO_TrackDropPkt& aPkt) {}

   //! Returns the index of platform whose tracks this object is requesting
   int GetPlatformIndex() const { return mRemotePlatformIndex; }

   void SetUpdateInterval(double aUpdateInterval, bool aRealTime);
   void HandleResponse(WsfXIO_ResponsePkt& aPkt) override;

   //! Returns 'true' if this request will be serviced with undirected responses.  Used internally.
   bool IsGlobalRequest() const { return mSensorNameId == nullptr && mUpdateInterval == 0; }

protected:
   void Initialized() override;

   int         mRemotePlatformIndex;
   int         mRequiredData;
   bool        mSendInitialTracks;
   WsfStringId mSensorNameId;
   double      mUpdateInterval;
   bool        mRealTime;
};


//! Manages all track requests.  Routes incoming track messages to the correct WsfXIO_TrackRequest.
class WSF_EXPORT WsfXIO_TrackRequestManager : protected WsfXIO_RequestManager
{
public:
   typedef std::pair<GenUniqueId, int> TargetId;

   WsfXIO_TrackRequestManager(WsfXIO_Interface* aInterfacePtr);

   void AddRequest(WsfXIO_TrackRequest* aRequestPtr);
   void RemoveRequest(WsfXIO_TrackRequest* aRequestPtr);
   void RemoveRequest(int aRequestId);
   void RemoveRequest(GenUniqueId& aAppId, int aPlatformIndex);

protected:
   WsfXIO_TrackRequest* FindRequest(GenUniqueId& aAppId, int aPlatformIndex);

   template<typename T>
   void ForwardPacket(T& aPkt)
   {
      WsfXIO_TrackRequest* requestPtr = FindRequest(aPkt.mApplicationId, aPkt.mPlatformIndex);
      if (requestPtr != nullptr)
      {
         requestPtr->Handle(aPkt);
      }
   }

   void RequestCancelled(WsfXIO_Request* aRequestPtr) override;

   void HandleRawTrack(WsfXIO_RawTrackPkt& aPkt);
   void HandleLocalTrack(WsfXIO_LocalTrackPkt& aPkt);
   void HandleDropTrack(WsfXIO_TrackDropPkt& aPkt);

   typedef std::map<TargetId, WsfXIO_TrackRequest*> RequestMap;
   RequestMap                                       mRequests;
};

#endif
