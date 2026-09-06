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

#ifndef WSFASSOCIATIONMESSAGE_HPP
#define WSFASSOCIATIONMESSAGE_HPP

#include "wsf_export.h"

#include <memory>
#include <string>

class UtScriptClass;
class UtScriptTypes;
#include "WsfMessage.hpp"
class WsfPlatform;
class WsfSensor;
#include "WsfStringId.hpp"
#include "WsfTrackId.hpp"

//! A 'association message' associates a track produced by a sensor with a tracking request.
//!
//! A request can be made to a sensor attempt to establish a track on an object using a cue that is
//! itself a track (WsfSensor::StartTracking). When the sensor acquires the target(s), it will
//! generate tracks. This message is issued by the sensor to indicate that a given sensor track
//! is associated with a particular request.
//!
//! In the typical case, the 'subject track ID' is the ID of the track that was provided to
//! WsfSensor::StartTracking and the 'associated track ID' is the ID of the track that is being
//! produced by the sensor in response to the request.
//!
//! \note The sensor will ensure that this message preceeds any message that pertains to the
//! associated track. In this way, a recipient will be able to correctly associate the sensor track
//! messages with the tracking request.
class WSF_EXPORT WsfAssociationMessage : public WsfMessage
{
public:
   WsfAssociationMessage(); // For XIO (de)serialization.
   WsfAssociationMessage(WsfPlatform* aPlatformPtr);
   WsfAssociationMessage(WsfSensor* aSensorPtr);
   WsfMessage*                           Clone() const override;
   const char*                           GetScriptClassName() const override;
   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static WsfStringId GetTypeId();

   //! @name Producing sensor methods.
   //! These methods can be used to retrieve information about the producing sensor.
   //@{
   WsfPlatform* GetSensorPlatform() const;

   //! Return the index of the platform on which the producing sensor is located.
   size_t GetSensorPlatformIndex() const { return mSensorPlatformIndex; }

   //! Set the index of the platform on which the producing sensor is located.
   void SetSensorPlatformIndex(size_t aPlatformIndex) { mSensorPlatformIndex = aPlatformIndex; }

   //! Return the name of the producing sensor.
   WsfStringId GetSensorName() const { return mSensorName; }

   //! Set the name of the producing sensor.
   void SetSensorName(WsfStringId aSensorName) { mSensorName = aSensorName; }
   //@}

   //! @name Subject Track ID methods.
   //!
   //! The 'subject ID' is the ID of the track to which the associated track is being associated.
   //@{
   //! Get the subject track ID.
   WsfTrackId GetSubjectId() const { return mSubjectId; }

   //! Set the subject track ID.
   void SetSubjectId(const WsfTrackId& aSubjectId) { mSubjectId = aSubjectId; }
   //@}

   //! @name Associated Track ID methods.
   //!
   //! The 'associated ID' is the track ID that is being associated with the subject track.
   //@{
   //! Get the associated track ID.
   WsfTrackId GetAssociatedId() const { return mAssociatedId; }

   //! Set the associated track ID.
   void SetAssociatedId(const WsfTrackId& aAssociatedId) { mAssociatedId = aAssociatedId; }
   //@}

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfMessage::Serialize(aBuff);
      aBuff& mSensorPlatformIndex& mSensorName& mSubjectId& mAssociatedId;
   }

protected:
   // Using the default copy constructor.
   // WsfAssociationMessage(const WsfAssociationMessage& aSrc);

   //! The index of the platform on which the producing sensor is located.
   size_t mSensorPlatformIndex;

   //! The name of the producing sensor.
   WsfStringId mSensorName;

   //! The ID of the track to which the associated track applies.
   WsfTrackId mSubjectId;

   //! The ID of the track that is associated with the subject track ID.
   WsfTrackId mAssociatedId;
};

#endif
