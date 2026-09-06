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

#ifndef WSFTRACKMESSAGE_HPP
#define WSFTRACKMESSAGE_HPP

#include "wsf_export.h"

#include <memory>

class UtScriptClass;
class UtScriptTypes;
#include "WsfMessage.hpp"
#include "WsfStringId.hpp"
class WsfTrack;

//! A specialization of WsfMessage that represents a track (WsfTrack)
//! to be transmitted over a communications network.
class WSF_EXPORT WsfTrackMessage : public WsfMessage
{
public:
   WsfTrackMessage(); // For XIO (de)serialization.
   WsfTrackMessage(WsfPlatform* aPlatformPtr);
   WsfTrackMessage(WsfPlatform* aPlatformPtr, const WsfTrack& aTrack);
   WsfTrackMessage(const WsfTrackMessage&);
   WsfTrackMessage& operator=(const WsfTrackMessage&) = delete;
   ~WsfTrackMessage() override;

   //! Clone this object by creating an identical copy and returning a pointer to it.
   //! @returns A pointer to the cloned object.
   WsfMessage* Clone() const override { return new WsfTrackMessage(*this); }

   const char* GetScriptClassName() const override;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static WsfStringId GetTypeId();

   //! Return the pointer to the track
   //! @returns a pointer to the track contained in the message
   //! @note This pointer is valid only during the processing of the message.
   //! If the information is to be saved then the track must be cloned.
   WsfTrack* GetTrack() const { return mTrackPtr; }

   void SetTrack(const WsfTrack& aTrack);

   //! @name Helpers for WsfTrackProcessor.
   //! These routines exists to help WsfTrackProcessor detect circular track updates.
   //@{
   WsfStringId GetSenderId() const { return mSenderId; }
   WsfStringId GetReplyId() const { return mReplyId; }
   void        SetSenderId(WsfStringId aSenderId) { mSenderId = aSenderId; }
   void        SetReplyId(WsfStringId aReplyId) { mReplyId = aReplyId; }
   //@}

   bool CanBeReplacedBy(const WsfMessage& aMessage) const override;

   //! @return The string ID of the name of the sensor that generated the track contained in the message
   WsfStringId GetComponentNameId() const override;
   //! @return The string ID of the type of the sensor that generated the track contained in the message
   WsfStringId GetComponentTypeId() const override;
   //! @return The string ID of the mode of the sensor that generated the track contained in the message
   WsfStringId GetComponentModeId() const override;

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfMessage::Serialize(aBuff);
      aBuff& mTrackPtr& mSenderId& mReplyId;
   }

private:
   WsfTrack* mTrackPtr;

   //! The string ID of the name of the platform that is sending the track report.
   WsfStringId mSenderId;

   //! The value of mSenderId to be sent in any track update that is a result of this report.
   WsfStringId mReplyId;
};

#endif
