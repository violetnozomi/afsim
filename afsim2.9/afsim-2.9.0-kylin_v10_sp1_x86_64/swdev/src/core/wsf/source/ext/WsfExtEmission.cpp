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

#include "ext/WsfExtEmission.hpp"

#include <iostream>
#include <sstream>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_XmtrRcvr.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorTypes.hpp"
#include "WsfSimulation.hpp"

//! Process input commands.
bool WsfExtEmission::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "debug_emission_pdu")
   {
      unsigned int debugLevel(0);
      aInput.ReadValue(debugLevel);
      aInput.ValueGreaterOrEqual(debugLevel, 0U);
      sDebugMask = debugLevel;
   }
   else if (command == "beam_function")
   {
      ProcessBeamFunction(aInput);
   }
   else if (command == "beam_type")
   {
      ProcessBeamType(aInput);
   }
   else if (command == "emitter_function")
   {
      ProcessEmitterFunction(aInput);
   }
   else if (command == "emitter_type")
   {
      ProcessEmitterType(aInput);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

void WsfExtEmission::ProcessBeamFunction(UtInput& aInput)
{
   std::string partType;
   aInput.ReadValue(partType);
   WsfStringId partTypeId = partType;

   std::string modeName;
   aInput.ReadValue(modeName);
   WsfStringId modeNameId;
   if (modeName != "*")
   {
      modeNameId = modeName;
   }

   std::string tempBeamStr;
   aInput.ReadValue(tempBeamStr);
   int beamNumber = 0;
   if (tempBeamStr != "*")
   {
      aInput.PushBack(tempBeamStr);
      aInput.ReadValue(beamNumber);
      aInput.ValueInClosedRange(beamNumber, 1, 255);
   }

   int tempBeamFunction;
   aInput.ReadValue(tempBeamFunction);
   aInput.ValueInClosedRange(tempBeamFunction, 0, 255);
   uint8_t beamFunction = static_cast<uint8_t>(tempBeamFunction);

   BeamKey beamKey(partTypeId, modeNameId, beamNumber);
   sBeamKeyToBeamFunction[beamKey] = beamFunction;
}

void WsfExtEmission::ProcessBeamType(UtInput& aInput)
{
   std::string partType;
   aInput.ReadValue(partType);
   WsfStringId partTypeId = partType;

   std::string modeName;
   aInput.ReadValue(modeName);
   WsfStringId modeNameId;
   if (modeName != "*")
   {
      modeNameId = modeName;
   }

   std::string tempBeamStr;
   aInput.ReadValue(tempBeamStr);
   int beamNumber = 0;
   if (tempBeamStr != "*")
   {
      aInput.PushBack(tempBeamStr);
      aInput.ReadValue(beamNumber);
      aInput.ValueInClosedRange(beamNumber, 1, 255);
   }

   int tempBeamType;
   aInput.ReadValue(tempBeamType);
   aInput.ValueInClosedRange(tempBeamType, 0, 65535);
   uint16_t beamType = static_cast<uint16_t>(tempBeamType);

   BeamKey beamKey(partTypeId, modeNameId, beamNumber);
   sBeamKeyToBeamType[beamKey] = beamType;
}

void WsfExtEmission::ProcessEmitterFunction(UtInput& aInput)
{
   std::string partType;
   aInput.ReadValue(partType);
   int emitterFunction;
   aInput.ReadValue(emitterFunction);
   aInput.ValueInClosedRange(emitterFunction, 0, 255);

   WsfStringId partTypeId                 = partType;
   uint8_t     emitterFunctionDIS         = static_cast<uint8_t>(emitterFunction);
   sPartTypeToEmitterFunction[partTypeId] = emitterFunctionDIS;
}

void WsfExtEmission::ProcessEmitterType(UtInput& aInput)
{
   std::string partType;
   aInput.ReadValue(partType);
   int emitterType;
   aInput.ReadValue(emitterType);
   aInput.ValueInClosedRange(emitterType, 0, 65535);

   WsfStringId partTypeId                 = partType;
   uint16_t    emitterTypeDIS             = static_cast<uint16_t>(emitterType);
   sPartTypeToEmitterType[partTypeId]     = emitterTypeDIS;
   sEmitterTypeToPartType[emitterTypeDIS] = partTypeId;
}

//! Return the DIS beam function that corresponds to given BeamKey.
uint8_t WsfExtEmission::GetBeamFunction(WsfStringId aPartTypeId, WsfStringId aModeNameId, int aBeamNumber)
{
   BeamKey beamKey(aPartTypeId, aModeNameId, aBeamNumber);

   std::map<BeamKey, uint8_t>::const_iterator iter = sBeamKeyToBeamFunction.find(beamKey);
   if (iter == sBeamKeyToBeamFunction.end())
   {
      // wild card only the beam
      beamKey.mBeamNumber = 0;
      iter                = sBeamKeyToBeamFunction.find(beamKey);
      if (iter == sBeamKeyToBeamFunction.end())
      {
         // wild card only the mode
         beamKey.mMode       = nullptr;
         beamKey.mBeamNumber = aBeamNumber;
         iter                = sBeamKeyToBeamFunction.find(beamKey);
         if (iter == sBeamKeyToBeamFunction.end())
         {
            // wild card the mode and beam
            beamKey.mMode       = nullptr;
            beamKey.mBeamNumber = 0;
            iter                = sBeamKeyToBeamFunction.find(beamKey);
         }
      }
   }

   uint8_t beamFunction = 0;
   if (iter != sBeamKeyToBeamFunction.end())
   {
      beamFunction = iter->second;
   }
   return beamFunction;
}

//! Return the DIS beam index that corresponds to given <sensor type, Mode, beam number> key.
uint16_t WsfExtEmission::GetBeamType(WsfStringId aPartTypeId, WsfStringId aModeNameId, int aBeamNumber)
{
   uint16_t                                    beamParamIndex = 0;
   BeamKey                                     beamKey(aPartTypeId, aModeNameId, aBeamNumber);
   std::map<BeamKey, uint16_t>::const_iterator bpiIter;

   bpiIter = sBeamKeyToBeamType.find(beamKey);
   if (bpiIter == sBeamKeyToBeamType.end())
   {
      // wild card only the beam
      beamKey.mBeamNumber = 0;
      bpiIter             = sBeamKeyToBeamType.find(beamKey);
      if (bpiIter == sBeamKeyToBeamType.end())
      {
         // wild card only the mode
         beamKey.mMode       = nullptr;
         beamKey.mBeamNumber = aBeamNumber;
         bpiIter             = sBeamKeyToBeamType.find(beamKey);
         if (bpiIter == sBeamKeyToBeamType.end())
         {
            // wild card the mode and beam
            beamKey.mMode       = nullptr;
            beamKey.mBeamNumber = 0;
            bpiIter             = sBeamKeyToBeamType.find(beamKey);
         }
      }
   }

   if (bpiIter != sBeamKeyToBeamType.end())
   {
      beamParamIndex = bpiIter->second;
   }
   return beamParamIndex;
}

//! Return the DIS emitter function that corresponds to given WSF part (comm/sensor) type ID.
uint8_t WsfExtEmission::GetEmitterFunction(WsfStringId aPartTypeId)
{
   uint8_t emitterFunction = 0;
   auto    iter            = sPartTypeToEmitterFunction.find(aPartTypeId);
   if (iter != sPartTypeToEmitterFunction.end())
   {
      emitterFunction = iter->second;
   }
   return emitterFunction;
}

//! Return the DIS emitter name that corresponds to given WSF part (comm/sensor) type ID.
uint16_t WsfExtEmission::GetEmitterType(WsfStringId aPartTypeId)
{
   uint16_t emitterType = 0;
   auto     iter        = sPartTypeToEmitterType.find(aPartTypeId);
   if (iter != sPartTypeToEmitterType.end())
   {
      emitterType = iter->second;
   }
   return emitterType;
}

//! Return the WSF part (comm/sensor) type ID that corresponds to given DIS Emitter type
WsfStringId WsfExtEmission::GetPartTypeId(uint16_t aEmitterType)
{
   WsfStringId partTypeId;
   auto        iter = sEmitterTypeToPartType.find(aEmitterType);
   if (iter != sEmitterTypeToPartType.end())
   {
      partTypeId = iter->second;
   }
   return partTypeId;
}

//! Update the beam field-of-view limits from the user limits.
//! Use the field-of-view limits from the antenna definition (which could be the default
//! 'unlimited' values) if either of the following is true:
//!
//! -) The user explicitly specified the field-of-view limits in the antenna definition.
//! -) No limits from the scan or slew limits could be used.
//!
//! The incoming values are not modified if they have valid values AND the user did not
//! explicitly specify [azimuth/elevation]_field_of_view in the antenna definition.
// private static
void WsfExtEmission::UpdateBeamLimits(WsfEM_XmtrRcvr& aXmtrRcvr, double& aMinAz, double& aMaxAz, double& aMinEl, double& aMaxEl)
{
   WsfEM_Antenna*          antennaPtr = aXmtrRcvr.GetAntenna();
   WsfEM_Antenna::ScanMode scanMode   = antennaPtr->GetScanMode();

   WsfArticulatedPart*          partPtr  = antennaPtr->GetArticulatedPart();
   WsfArticulatedPart::SlewMode slewMode = partPtr->GetSlewMode();

   // If a limit has not been imposed, use the [azimuth|elevation]_scan_limits or
   // [azimuth|elevation]_slew_limits if specified.

   if ((aMinAz == 0.0) && (aMaxAz == 0.0))
   {
      if ((scanMode == WsfEM_Antenna::cSCAN_AZ) || (scanMode == WsfEM_Antenna::cSCAN_AZ_EL))
      {
         antennaPtr->GetAzimuthScanLimits(aMinAz, aMaxAz);
      }
      else if ((slewMode == WsfArticulatedPart::cSLEW_AZ) || (slewMode == WsfArticulatedPart::cSLEW_AZ_EL))
      {
         aMinAz = partPtr->GetMinAzSlew();
         aMaxAz = partPtr->GetMaxAzSlew();
      }
   }

   if ((aMinEl == 0.0) && (aMaxEl == 0.0))
   {
      if ((scanMode == WsfEM_Antenna::cSCAN_EL) || (scanMode == WsfEM_Antenna::cSCAN_AZ_EL))
      {
         antennaPtr->GetElevationScanLimits(aMinEl, aMaxEl);
      }
      else if ((slewMode == WsfArticulatedPart::cSLEW_EL) || (slewMode == WsfArticulatedPart::cSLEW_AZ_EL))
      {
         aMinEl = partPtr->GetMinElSlew();
         aMaxEl = partPtr->GetMaxElSlew();
      }
   }

   double minAzFOV;
   double maxAzFOV;
   antennaPtr->GetAzimuthFieldOfView(minAzFOV, maxAzFOV);
   if ((minAzFOV != -UtMath::cPI) || (maxAzFOV != UtMath::cPI))
   {
      // User explicitly specified azimuth_field_of_view
      aMinAz = minAzFOV;
      aMaxAz = maxAzFOV;
   }
   else if ((aMinAz == 0.0) && (aMaxAz == 0.0))
   {
      // scan_mode or slew_mode did not include azimuth
      aMinAz = minAzFOV;
      aMaxAz = maxAzFOV;
   }

   double minElFOV;
   double maxElFOV;
   antennaPtr->GetElevationFieldOfView(minElFOV, maxElFOV);
   if ((minElFOV != -UtMath::cPI_OVER_2) || (maxElFOV != UtMath::cPI_OVER_2))
   {
      // User explicitly specified elevation_field_of_view
      aMinEl = minElFOV;
      aMaxEl = maxElFOV;
   }
   else if ((aMinEl == 0.0) && (aMaxEl == 0.0))
   {
      // scan_mode or slew_mode did not include elevation
      aMinEl = minElFOV;
      aMaxEl = maxElFOV;
   }
}

//! Gets a sensor from the given platform.
//!
//! @param aPlatformPtr         [input] Pointer to the sensor's platform.
//! @param aSensorTypeId        [input] Sensor type ID.
//! @param aEmitterNameDIS      [input] DIS emitter system - emitter name
//! @param aEmitterIdNumberDIS  [input] DIS emitter system - emitter ID number
//!
//! @return A pointer to the sensor just found.
WsfSensor* WsfExtEmission::GetSensor(WsfPlatform* aPlatformPtr,
                                     WsfStringId  aSensorTypeId,
                                     unsigned int aEmitterNameDIS,
                                     unsigned int aEmitterIdNumberDIS)
{
   // Search through emitters already on-board
   WsfSensor* sensorPtr = nullptr;
   for (WsfComponentList::RoleIterator<WsfSensor> iter(*aPlatformPtr); !iter.AtEnd(); ++iter)
   {
      // Check for the type match
      bool sensorTypeMatches(false);
      if (aSensorTypeId > nullptr)
      {
         sensorTypeMatches = iter->IsA_TypeOf(aSensorTypeId);
      }

      // Check for emitter name match

      int  auxEmitterName;
      bool emitterNameMatches =
         WsfUtil::GetAuxValue(*iter, "dis_emitter_name", auxEmitterName) && (auxEmitterName == (int)aEmitterNameDIS);

      if (sensorTypeMatches || emitterNameMatches)
      {
         // Now check the emitter ID
         if ((iter->GetAuxDataConst()).AttributeExists("dis_emitter_id"))
         {
            if ((iter->GetAuxDataConst()).GetInt("dis_emitter_id") == (int)aEmitterIdNumberDIS)
            {
               // Found!
               sensorPtr = *iter;
               break;
            }
         }
         else
         {
            // Create the mapping to the DIS emitter ID number
            (iter->GetAuxData()).Assign("dis_emitter_id", (int)aEmitterIdNumberDIS);
            // Found!
            sensorPtr = *iter;
            break;
         }
      }
   }
   return sensorPtr;
}

//! Gets a sensor from the given platform.
//!
//! @param aSensorTypeId        [input] Sensor type ID.
//! @param aEmitterNameDIS      [input] DIS emitter system - emitter name
//! @param aEmitterIdNumberDIS  [input] DIS emitter system - emitter ID number
//!
//! @return A pointer to the sensor just found.
WsfSensor* WsfExtEmission::GetSensor(WsfStringId aSensorTypeId, unsigned int aEmitterNameDIS, unsigned int aEmitterIdNumberDIS)
{
   // Set the default return value
   WsfSensor* sensorPtr = WsfSensorTypes::Get(GetScenario()).Clone(aSensorTypeId);
   if (sensorPtr != nullptr)
   {
      WsfAttributeContainer& auxData = sensorPtr->GetAuxData();
      auxData.Assign("dis_emitter_name", (int)aEmitterNameDIS);
      // Create the mapping to the DIS emitter ID number
      auxData.Assign("dis_emitter_id", (int)aEmitterIdNumberDIS);
   }

   return sensorPtr;
}

// ============================================================================
//! Adds a sensor to the given platform.
//!
//! @param aSimTime             [input] Current simulation time.
//! @param aPlatformPtr         [input] Pointer to the sensor's platform.
//! @param aSensorTypeId        [input] Sensor type ID.
//! @param aEmitterNameDIS      [input] DIS emitter system - emitter name
//! @param aEmitterIdNumberDIS  [input] DIS emitter system - emitter ID number
//!
//! @return A pointer to the sensor just added.
WsfSensor* WsfExtEmission::AddSensor(double       aSimTime,
                                     WsfPlatform* aPlatformPtr,
                                     WsfStringId  aSensorTypeId,
                                     unsigned int aEmitterNameDIS,
                                     unsigned int aEmitterIdNumberDIS)
{
   // Set default return value
   WsfSensor* newSensorPtr = nullptr;

   // Clone the platform; based on the type
   WsfPlatform* srcPlatformPtr =
      (WsfPlatform*)aPlatformPtr->GetSimulation()->GetScenario().FindType("platform_type", aPlatformPtr->GetType());
   // WsfPlatform* srcPlatformPtr = WsfPlatform::FindType(aPlatformPtr->GetType());

   // Get the sensor from the source platform
   WsfSensor* srcSensorPtr =
      WsfExtEmission::GetSensor(srcPlatformPtr, aSensorTypeId, aEmitterNameDIS, aEmitterIdNumberDIS);
   if (srcSensorPtr == nullptr)
   {
      // Sensor is not on the cloned platform; use emitter_type info and clone sensor
      srcSensorPtr = WsfExtEmission::GetSensor(aSensorTypeId, aEmitterNameDIS, aEmitterIdNumberDIS);
   }

   if (srcSensorPtr != nullptr)
   {
      // Clone the sensor
      newSensorPtr = srcSensorPtr->Clone();
      if (newSensorPtr != nullptr)
      {
         // Emitter is a sensor; set the name and add
         std::ostringstream oss;
         oss << aEmitterNameDIS << '_' << aEmitterIdNumberDIS;
         newSensorPtr->SetName(oss.str());

         newSensorPtr->SetExternallyControlled(aSimTime, true);

         if (aPlatformPtr->AddComponent(newSensorPtr))
         {
            if (!newSensorPtr->Initialize(aSimTime))
            {
               { // RAII block
                  auto out = ut::log::error() << "Initialization failed for external sensor.";
                  out.AddNote() << "Sensor: " << aPlatformPtr->GetName() << "." << newSensorPtr->GetName();
               }

               aPlatformPtr->DeleteComponent(newSensorPtr->GetNameId(), cWSF_COMPONENT_SENSOR);
               newSensorPtr = nullptr;
            }
         }
         else
         {
            // Adding the sensor to the platform failed
            delete newSensorPtr;
            newSensorPtr = nullptr;
         }
      }
   }

   return newSensorPtr;
}

WsfExtEmission::WsfExtEmission(WsfScenario& aScenario)
   : mScenarioPtr(&aScenario)
{
}

////////////
WsfExtEmission::BeamKey::BeamKey(WsfStringId aId, WsfStringId aMode, int aBeamNumber)
   : mId(aId)
   , mMode(aMode)
   , mBeamNumber(aBeamNumber)
{
}

bool WsfExtEmission::BeamKey::operator<(const BeamKey& aRhs) const
{
   bool isLessThan(false);

   if (mId < aRhs.mId)
   {
      isLessThan = true;
   }
   else if (mId == aRhs.mId)
   {
      if (mMode < aRhs.mMode)
      {
         isLessThan = true;
      }
      else if (mMode == aRhs.mMode)
      {
         if (mBeamNumber < aRhs.mBeamNumber)
         {
            isLessThan = true;
         }
      }
   }

   return isLessThan;
}

bool WsfExtEmission::BeamKey::operator==(const BeamKey& aRhs) const
{
   return ((mId == aRhs.mId) && (mMode == aRhs.mMode) && (mBeamNumber == aRhs.mBeamNumber));
}

bool WsfExtEmission::BeamKey::operator!=(const BeamKey& aRhs) const
{
   return ((mId != aRhs.mId) || (mMode != aRhs.mMode) || (mBeamNumber != aRhs.mBeamNumber));
}
