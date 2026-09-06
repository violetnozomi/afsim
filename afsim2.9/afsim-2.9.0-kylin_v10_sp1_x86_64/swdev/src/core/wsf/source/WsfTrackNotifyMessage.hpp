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

#ifndef WSFTRACKNOTIFYMESSAGE_HPP
#define WSFTRACKNOTIFYMESSAGE_HPP

#include "wsf_export.h"

#include <memory>

class UtScriptClass;
class UtScriptTypes;
#include "WsfMessage.hpp"
#include "WsfTrackId.hpp"

/**
   A specialization of WsfMessage that is used by the WsfTrackManager to
   represent state changes of a track in the master track list (see WsfTrackManager
   for supported state changes).
   Once received, the receiver can check his master track list to take
   appropriate action.
*/
class WSF_EXPORT WsfTrackNotifyMessage : public WsfMessage
{
public:
   WsfTrackNotifyMessage(WsfPlatform*      aPlatformPtr,
                         const WsfTrackId& aTrackId,
                         WsfStringId       aTrackProcessorNameId,
                         int               aChangedReason);

   ~WsfTrackNotifyMessage() override;

   WsfMessage* Clone() const override { return new WsfTrackNotifyMessage(*this); }

   const char* GetScriptClassName() const override;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   //! Get the ID of the track whose state has changed.
   //!   @returns The ID of the dropped track.
   const WsfTrackId& GetTrackId() const { return mTrackId; }

   //! Get the name of the track processor that owns the changed track.
   //! @note There can be multiple track processors on a single platform.
   WsfStringId GetTrackProcessorNameId() const { return mTrackProcessorNameId; }

   //! Get the reason for sending this message.
   //! @returns The changed track status.
   int GetChangedReason() const { return mChangedReason; }

   static WsfStringId GetTypeId();

   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mTrackId& mChangedReason;
   }

private:
   WsfTrackId  mTrackId;
   WsfStringId mTrackProcessorNameId;
   int         mChangedReason;
};

#endif
