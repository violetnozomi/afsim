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

//! A specialization of WsfSensor that provides the baseline passive RF sensor.
//! This is useful for modeling RWR, ELINT and COMINT sensing functions.

#include "WsfESM_Sensor.hpp"

#include "UtMemory.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfWeapon.hpp"

// =================================================================================================
WsfESM_Sensor::WsfESM_Sensor(WsfScenario& aScenario)
   : WsfPassiveSensor(aScenario)
{
   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new ESM_Mode()));
}

// =================================================================================================
// virtual
WsfSensor* WsfESM_Sensor::Clone() const
{
   return new WsfESM_Sensor(*this);
}

// =================================================================================================
// Nested class WsfESM_Sensor::ESM_Mode.
// =================================================================================================

// =================================================================================================
// virtual
WsfMode* WsfESM_Sensor::ESM_Mode::Clone() const
{
   return new ESM_Mode(*this);
}

// =================================================================================================
// virtual
void WsfESM_Sensor::ESM_Mode::ComputeInterferenceEffect(double aSimTime, WsfEM_Xmtr* aXmtrPtr, WsfEM_Rcvr* aRcvrPtr)
{
   // Jamming interactions are driven by sensing chances (e.g.: WsfRadarSensor calls WsfRF_Jammer to compute
   // the jamming effects when a radar sensing chances is performed). If there are no radars, the jamming code
   // will never get called, so the listener for jamming interactions will never get performed and the jammer
   // will never be seen. Another issue occurs where a jammer is jamming a radar in which the sensing chances
   // are occurring at a rate slower than the frame_time of the ESM. In that case the jammer would get seen in
   // one ESM frame, but not the next.
   //
   // To circumvent this problem we will always force a jamming interaction to occur against us. This will force
   // the listener to be called which will then check to see if the jammer transmitter can be detected.

   aXmtrPtr->UpdatePosition(aSimTime);
   WsfEM_Interaction targetInteraction;
   if (targetInteraction.BeginOneWayInteraction(aXmtrPtr, aRcvrPtr, false, true) == 0)
   {
      targetInteraction.SetReceiverBeamPosition();
      targetInteraction.SetTransmitterBeamPosition();
      WsfEM_Interaction jammerInteraction(targetInteraction);
      WsfRF_Jammer::ComputeSingleJammerEffect(aSimTime, aXmtrPtr, targetInteraction, jammerInteraction, false);
   }
}

// =================================================================================================
//! Returns true if the mode should utilize the 'listener' detection method for a jamming beam
bool WsfESM_Sensor::ESM_Mode::UseListenerForInterferer(WsfEM_Xmtr* aXmtrPtr)
{
   auto* weaponPtr = dynamic_cast<WsfWeapon*>(aXmtrPtr->GetArticulatedPart());
   auto* modePtr   = dynamic_cast<WsfWeapon::WsfWeaponMode*>(aXmtrPtr->GetMode());
   if ((weaponPtr != nullptr) && (modePtr != nullptr))
   {
      if (weaponPtr->GetSlewMode() != WsfArticulatedPart::cSLEW_FIXED)
      {
         return true;
      }
   }

   return false;
}

// Script Accessor Class
class WSF_MIL_EXPORT WsfScriptESM_SensorClass : public WsfScriptPassiveSensorClass
{
public:
   WsfScriptESM_SensorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
};


std::unique_ptr<UtScriptClass> WsfESM_Sensor::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptESM_SensorClass>(aClassName, aScriptTypesPtr);
}

WsfScriptESM_SensorClass::WsfScriptESM_SensorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptPassiveSensorClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfESM_Sensor");
}
