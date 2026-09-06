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

#ifndef WSFSENSORPROCESSOR_HPP
#define WSFSENSORPROCESSOR_HPP

#include "wsf_export.h"

#include <mutex>
#include <vector>

class UtScriptTypes;
#include "UtScriptExtension.hpp"

class WsfPlatform;
#include "WsfProcessor.hpp"
class WsfScenario;
class WsfSensor;
#include "WsfStringId.hpp"
class WsfTrack;

//! A base class sensor processor that connects to single
//! or multiple sensors to allow for sensor base functionality
//! and sensor management for inheriting classes.
//!
//! @note The inheriting class may want to also inherit from the
//! WsfSingleSensorObserver to attach directly to certain sensor events.

class WSF_EXPORT WsfSensorProcessor : public WsfProcessor
{
public:
   WsfSensorProcessor(WsfScenario& aScenario);
   ~WsfSensorProcessor() override = default;

   //! @name Base Class overrides
   //@{
   WsfProcessor* Clone() const override = 0;
   bool          Initialize(double aSimTime) override;
   bool          ProcessInput(UtInput& aInput) override;
   void          PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) override;
   void          PlatformDeleted(WsfPlatform* aPlatformPtr) override;
   //@}

   virtual bool AddSensor(WsfStringId aPlatformNameId, WsfStringId aSensorNameId);
   virtual bool RemoveSensor(WsfStringId aPlatformNameId, WsfStringId aSensorNameId);

   static void RegisterScriptMethods(UtScriptTypes& aScriptTypes);

protected:
   WsfSensorProcessor(const WsfSensorProcessor& aSrc);
   WsfSensorProcessor& operator=(const WsfSensorProcessor& aRhs) = delete;

   //! Clear any data or lists maintained by inheriting classes
   virtual bool SensorAdded(WsfSensor* aSensorPtr)   = 0;
   virtual bool SensorRemoved(WsfSensor* aSensorPtr) = 0;

   //! @name Tracking Methods
   //@{
   virtual void TrackUpdated(double aSimTime, WsfSensor* aSensorPtr, WsfTrack* aTrackPtr);

   virtual void TrackDropped(double aSimTime, const WsfTrack& aTrack);


   void SendTrackUpdateMessage(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);
   //@}

   std::recursive_mutex mIncomingMutex;

private:
   //! Input Platform-Sensor list
   std::vector<std::pair<WsfStringId, WsfStringId>> mSensors;

   //! The logical length (in bits) of the track messages that originate from this processor.
   int mMessageLength;
   //! The message priority for messages that originate from this processor.
   int mMessagePriority;
};

class WsfSensorProcessorScriptExtensions : public UtScriptExtension
{
public:
   ~WsfSensorProcessorScriptExtensions() override = default;
   bool AddExtClassMethods(const std::string& aClassName, const std::string& aBaseName, UtScriptTypes* aScriptTypesPtr) override;
};

#endif
