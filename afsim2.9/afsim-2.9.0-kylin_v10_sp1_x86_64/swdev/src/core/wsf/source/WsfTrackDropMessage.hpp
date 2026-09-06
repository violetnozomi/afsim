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

#ifndef WSFTRACKDROPMESSAGE_HPP
#define WSFTRACKDROPMESSAGE_HPP

#include "wsf_export.h"

#include <memory>

class UtScriptClass;
class UtScriptTypes;
#include "WsfMessage.hpp"
#include "WsfTrackId.hpp"

/**
   A specialization of WsfMessage that represents the message generated to
   indicate that a track has been dropped.

   WsfSensor in an example of a class that generates this type of message.
*/
class WSF_EXPORT WsfTrackDropMessage : public WsfMessage
{
public:
   WsfTrackDropMessage(); // For XIO (de)serialization.

   WsfTrackDropMessage(WsfPlatform* aPlatformPtr, double aTime, const WsfTrackId& aTrackId, size_t aPlatformIndex);

   ~WsfTrackDropMessage() override = default;

   //! Clone this object by creating an identical copy and returning a pointer to it.
   //!   @returns A pointer to the cloned object.
   WsfMessage* Clone() const override { return new WsfTrackDropMessage(*this); }

   const char*                           GetScriptClassName() const override;
   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static WsfStringId GetTypeId();

   //! Get the time at which the track was dropped.
   //!   @returns The simulation time at which the track was dropped.
   double GetTime() const { return mTime; }

   //! Set the time at which the track was dropped.
   void SetTime(double aSimTime) { mTime = aSimTime; }

   //! Get the ID of the track being dropped.
   //!   @returns The ID of the dropped track.
   const WsfTrackId& GetTrackId() const { return mTrackId; }

   //! Get the truth platform index corresponding to the dropped track.
   //!   @returns The platform index corresponding to the dropped track.
   size_t GetTargetIndex() const { return mTargetIndex; }

   //! Set the truth platform index corresponding to the dropped track.
   void SetTargetIndex(size_t aTargetIndex) { mTargetIndex = aTargetIndex; }

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfMessage::Serialize(aBuff);
      aBuff& mTime& mTrackId& mTargetIndex;
   }

private:
   double     mTime;
   WsfTrackId mTrackId;
   size_t     mTargetIndex;
};

#endif
