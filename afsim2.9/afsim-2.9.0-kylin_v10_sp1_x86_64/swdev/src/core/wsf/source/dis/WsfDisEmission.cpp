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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "dis/WsfDisEmission.hpp"

#include <algorithm>
#include <climits>

#include "DisIO.hpp"
#include "UtInput.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfDisObserver.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorTypes.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"
#include "ext/WsfExtEmission.hpp"
#include "ext/WsfExtInterface.hpp"

WsfDisEmission::OverrideTypeAndFunctionCallback WsfDisEmission::OverrideTypeAndFunction;

// ============================================================================
WsfDisEmission::WsfDisEmission(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisEmission(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// ============================================================================
//! Process the DIS Emission PDU.
// virtual
int WsfDisEmission::Process()
{
   // find the parent platform.
   DisEntityId     platformId     = GetEmittingEntityId();
   WsfDisPlatform* disPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, platformId);
   if (disPlatformPtr != nullptr)
   {
      if (disPlatformPtr->GetPlatform()->IsExternallyControlled())
      {
         if (WsfDisUtil::GetExtInterface(mInterfacePtr)->GetEmission()->GetDebugMask() &
             (WsfExtEmission::cLEVEL_ONE_DEBUG | WsfExtEmission::cLEVEL_TWO_DEBUG))
         {
            double simTime = WsfDisUtil::GetTimeFromPdu(mInterfacePtr, *this);
            auto   out     = ut::log::debug() << "WsfDisEmission: Processed.";
            out.AddNote() << "T = " << simTime;
            out.AddNote() << "Name: " << disPlatformPtr->GetPlatform()->GetName();
            out.AddNote() << "Type: " << disPlatformPtr->GetPlatform()->GetType();
         }

         UpdatePlatform(disPlatformPtr->GetPlatform());
         WsfPlatform* shadowPlatformPtr = disPlatformPtr->GetShadowPlatform();
         if (shadowPlatformPtr != nullptr)
         {
            UpdateShadowPlatform(shadowPlatformPtr);
         }
      }
   }

   // Notify any subscribers that an Emission PDU had been received.
   WsfObserver::DisEmissionReceived (&WsfDisUtil::GetSimulation(mInterfacePtr))(mInterfacePtr, *this);

   return 1;
}

// ============================================================================
//! Update the shadow platform with emission PDU data.
void WsfDisEmission::UpdateShadowPlatform(WsfPlatform* aShadowPlatformPtr)
{
   // loop through number of systems
   auto sysIter(GetSystemBegin());
   while (sysIter != GetSystemEnd())
   {
      DisSystem* sysPtr = *sysIter;

      // Have we defined the dis emitter type as having a corresponding WSF sensor type?
      WsfStringId typeId = GetExtEmission()->GetPartTypeId(sysPtr->GetName());
      if (typeId != 0)
      {
         // Search through emitters until one with a matching id is found.
         WsfSensor* sensorPtr = nullptr;
         for (WsfComponentList::RoleIterator<WsfSensor> iter(*aShadowPlatformPtr); !iter.AtEnd(); ++iter)
         {
            if (iter->GetTypeId() == typeId)
            {
               sensorPtr = *iter;
               break;
            }
         }

         if (sensorPtr == nullptr) // Create the new emitter
         {
            sensorPtr = WsfSensorTypes::Get(mInterfacePtr->GetScenario()).Clone(typeId);
            if (sensorPtr != nullptr)
            {
               aShadowPlatformPtr->AddComponent(sensorPtr);
               sensorPtr->Initialize(mInterfacePtr->GetSimTime());
               if (sensorPtr->IsTurnedOn())
               {
                  // schedule the turn on event
                  sensorPtr->TurnOff(mInterfacePtr->GetSimTime());
                  WsfDisUtil::GetSimulation(mInterfacePtr).TurnPartOn(mInterfacePtr->GetSimTime(), sensorPtr);
               }
            }
         }
         else // (sensorPtr != 0)
         {
            // Update the sensor with pdu info. here.
            auto   beamIter(sysPtr->GetBeamBegin());
            double maxPower = 0.0; // in dbmW

            while ((beamIter != sysPtr->GetBeamEnd()) && (maxPower <= 0.0))
            {
               DisBeam* beamPtr = *beamIter;
               if (beamPtr->GetEffectiveRadiatedPower() > maxPower)
               {
                  maxPower = beamPtr->GetEffectiveRadiatedPower();
               }
               ++beamIter;
            }
            if (maxPower > 0.0)
            {
               if (!sensorPtr->IsTurnedOn())
               {
                  WsfDisUtil::GetSimulation(mInterfacePtr).TurnPartOn(mInterfacePtr->GetSimTime(), sensorPtr);
               }
            }
            else if (sensorPtr->IsTurnedOn())
            {
               WsfDisUtil::GetSimulation(mInterfacePtr).TurnPartOff(mInterfacePtr->GetSimTime(), sensorPtr);
            }
         }
      }
      ++sysIter;
   }
}

// ============================================================================
//! Updates the platform using the DIS emission PDU data.
//!
//! @param aPlatformPtr   [input] Pointer to the platform to be updated.
void WsfDisEmission::UpdatePlatform(WsfPlatform* aPlatformPtr)
{
   // loop through number of systems
   auto sysIter(GetSystemBegin());
   while (sysIter != GetSystemEnd())
   {
      DisSystem* sysPtr = *sysIter;

      mInterfacePtr->UpdatePlatformSystem(aPlatformPtr, this, sysPtr);

      ++sysIter;
   } // while loop
}


// ============================================================================
//! Notify listeners (e.g. Passive sensors) of activity.
//! @param aBeamPtr [input] Pointer to the DIS beam data.
//! @param aXmtrPtr [input] Pointer to the transmitter object that corresponds to the beam.
void WsfDisEmission::NotifySensorListeners(DisBeam* aBeamPtr, WsfEM_Xmtr* aXmtrPtr)
{
   // For now it is assumed the transmitter has a linked receiver. If we ever want to do non-linked then
   // we'll have to do something else.
   if (aXmtrPtr->GetLinkedReceiver() == nullptr)
   {
      return;
   }

   // For each target in the track/jam list, create an interaction object that would reflect the sensor
   // (xmtr) looking directly at the target and notify the listeners and interferers. This simulates a local
   // detection chance that would perform the same operation.
   //
   // Note that this causes 'detection attempts' to be performed when PDU's are received. This may be
   // faster or slower than the actual revisit time specified by the sensor.

   std::vector<DisTrackJam> tjList(aBeamPtr->GetTargetVector());
   auto                     tjIter(tjList.begin());
   while (tjIter != tjList.end())
   {
      WsfDisPlatform* disPlatformPtr = mInterfacePtr->FindDisPlatform(tjIter->GetEntityId());
      if ((disPlatformPtr != nullptr) && (disPlatformPtr->GetPlatform() != nullptr))
      {
         WsfPlatform* platformPtr = disPlatformPtr->GetPlatform();
         double       simTime     = WsfDisUtil::GetSimTime(mInterfacePtr);
         platformPtr->Update(simTime); // Ensure target position is current
         double targetLocWCS[3];
         platformPtr->GetLocationWCS(targetLocWCS);
         aXmtrPtr->GetArticulatedPart()->SetTransientCuedLocationWCS(targetLocWCS);
         aXmtrPtr->UpdatePosition(simTime); // Ensure the transmitter position/orientation is current

         // Create the interaction object and set the transmitter/receiver beam positions
         WsfEM_Interaction targetInteraction;
         targetInteraction.BeginTwoWayInteraction(aXmtrPtr, platformPtr, aXmtrPtr->GetLinkedReceiver());
         targetInteraction.SetTransmitterBeamPosition();
         targetInteraction.SetReceiverBeamPosition();

         // Notify the listeners about the interaction
         aXmtrPtr->NotifyListeners(simTime, targetInteraction);
      }
      ++tjIter;
   }
}

// ============================================================================
//! Updates the sensor from the DIS emitter system data.
//!
//! @param aSystemPtr         [input] Pointer to the DIS emission PDU system.
//! @param aSensorPtr         [input] Pointer to the sensor to update.
void WsfDisEmission::UpdateSensor(DisSystem* aSystemPtr, WsfSensor* aSensorPtr)
{
   // Update location
   UpdateLocation(aSystemPtr, aSensorPtr);

   size_t beamCount    = aSystemPtr->GetNumberOfBeams();
   size_t emitterCount = aSensorPtr->GetEM_XmtrCount();
   bool canMultiSelect = false; // may be a bit redundant (i.e. check modeStartCount.empty()), but provides a quick bool
   WsfSensorModeList* modeListPtr = aSensorPtr->GetModeList();
   std::vector<int>   modeStartCount;
   if ((modeListPtr != nullptr) && modeListPtr->CanMultiSelect())
   {
      // Toggle the multi-select flag
      canMultiSelect = true;
      emitterCount   = aSensorPtr->GetMaximumRequestCount();

      std::vector<WsfSensorMode*> modes;
      modeListPtr->GetDerivedModeList(modes);
      modeStartCount.push_back(1); // Performance, first mode is beam 1, obviously
      size_t modeCnt = modeListPtr->GetModeCount();
      for (size_t mi = 1; mi < modeCnt; ++mi)
      {
         int reqCnt = modes.at(mi - 1)->GetMaximumRequestCount();
         modeStartCount.push_back(reqCnt + modeStartCount.at(mi - 1));
      }
   }

   if (beamCount == 0)
   {
      // Turn system off
      WsfDisUtil::GetSimulation(mInterfacePtr).TurnPartOff(mInterfacePtr->GetSimTime(), aSensorPtr);
   }
   else if (emitterCount == 0)
   {
      // Passive interferer... (we don't do anything for these yet...)
   }
   else
   {
      // Update the xmtr from the beam data
      //
      // NOTE: The number of beams may be greater than the number of transmitters. In particular, this will occur when one
      //       WSF is playing against another WSF over DIS. The WSF DIS interface may send out a 'pseudo-beam' to
      //       reflect the number of items being detected (but not yet in track). This also lets us handle partial updates.

      emitterCount = std::min(emitterCount, beamCount);

      bool                systemOn = false;
      std::vector<size_t> modeSelections;
      auto                beamIter(aSystemPtr->GetBeamBegin());
      for (unsigned int emitterIndex = 0; emitterIndex < emitterCount; ++emitterIndex)
      {
         DisBeam* beamPtr = beamIter[emitterIndex];
         DisUint8 beamNum = beamPtr->GetNumber();
         if (beamNum == 255)
         {
            continue;
         }

         WsfEM_Xmtr* xmtrPtr = &aSensorPtr->GetEM_Xmtr(emitterIndex);
         if (canMultiSelect)
         {
            size_t modeIndex = 0;
            size_t beamIndex = 0;
            size_t mscnt     = modeStartCount.size();
            for (size_t mi = 0; mi < mscnt; ++mi)
            {
               if ((beamNum >= modeStartCount.at(mi)) && (mi < aSensorPtr->GetModeCount()))
               {
                  modeIndex = mi;
                  beamIndex = beamNum - modeStartCount.at(mi);
               }
            }
            beamIndex = std::min(beamIndex, std::max(static_cast<size_t>(0), aSensorPtr->GetEM_XmtrCount(modeIndex) - 1));
            xmtrPtr   = &aSensorPtr->GetEM_Xmtr(modeIndex, beamIndex);
         }
         bool beamOn = false;
         UpdateXmtr(beamPtr, xmtrPtr, beamOn);
         systemOn |= beamOn;
         if (canMultiSelect && beamOn)
         {
            xmtrPtr->GetMode()->Select(mInterfacePtr->GetSimTime());
            modeSelections.push_back(xmtrPtr->GetMode()->GetModeIndex());
         }
         UpdateSysBeamMapping(aSystemPtr->GetNumber(), beamPtr->GetNumber(), xmtrPtr->GetUniqueId());
         if (mInterfacePtr->UseTrackJamForTrackingRequests() && xmtrPtr->HasListeners() &&
             (beamPtr->GetNumberOfTargets() > 0))
         {
            NotifySensorListeners(beamPtr, xmtrPtr);
         }
      }

      // Turn the system on or off based on the power level
      if (systemOn)
      {
         WsfDisUtil::GetSimulation(mInterfacePtr).TurnPartOn(mInterfacePtr->GetSimTime(), aSensorPtr);

         // If there are any mode selections made we need to clean up any modes that were previously
         // selected but are now not selected
         if (!modeSelections.empty())
         {
            for (size_t smi = 0; smi < modeListPtr->GetModeCount(); ++smi)
            {
               if (find(modeSelections.begin(), modeSelections.end(), smi) == modeSelections.end())
               {
                  aSensorPtr->DeselectMode(mInterfacePtr->GetSimTime(), modeListPtr->GetModeNameId(smi));
               }
            }
         }
      }
      else
      {
         WsfDisUtil::GetSimulation(mInterfacePtr).TurnPartOff(mInterfacePtr->GetSimTime(), aSensorPtr);
      }
   }
}

// ============================================================================
//! Updates the EM transmitter from the DIS emitter beam data.
//!
//! @param aBeamPtr  [input]   Pointer to the DIS emission PDU beam.
//! @param aXmtrPtr  [input]   Pointer to the transmitter to update.
//! @param aBeamOn   [updated] Set to 'true' if the effective radiated power is > 0.0, otherwise not changed.
void WsfDisEmission::UpdateXmtr(DisBeam* aBeamPtr, WsfEM_Xmtr* aXmtrPtr, bool& aBeamOn)
{
   if (GetExtEmission()->GetDebugMask() & WsfExtEmission::cLEVEL_TWO_DEBUG)
   {
      auto out = ut::log::debug() << "WsfDisEmission: Updating transmitter.";
      out.AddNote() << "Beam ID: " << static_cast<int>(aBeamPtr->GetNumber());

      if (aBeamPtr->GetParameterIndex() > 0)
      {
         out.AddNote() << "Beam Index: " << static_cast<int>(aBeamPtr->GetParameterIndex());
      }

      if (aXmtrPtr->GetMode() != nullptr)
      {
         out.AddNote() << "Mode: " << aXmtrPtr->GetMode()->GetName();
      }
   }

   // Frequency (Hertz)
   aXmtrPtr->SetFrequency(aBeamPtr->GetFrequency());

   // Frequency range (Hertz)
   aXmtrPtr->SetBandwidth(aBeamPtr->GetFrequencyRange());

   // Convert the DIS Effective radiated power (dBmW) to WSF raw transmitter power (watts).
   // Note DIS power the output from the antenna, WSF is output from the transmitter amplifier.
   // This should be the inverse of what is in WsfDisPlatform::UpdateBeam.

   double                    power_dBmW    = aBeamPtr->GetEffectiveRadiatedPower();
   double                    power_W       = 0.001 * pow(10.0, power_dBmW / 10.0);
   WsfEM_Types::Polarization aPolarization = aXmtrPtr->GetPolarization();
   power_W /= aXmtrPtr->GetAntennaGain(aPolarization, aBeamPtr->GetFrequency(), 0.0, 0.0, 0.0, 0.0);
   power_W *= aXmtrPtr->GetInternalLoss();
   aXmtrPtr->SetPower(power_W);

   // System on check (if power_dBmW is 0 then it is off, NOTE: the power_W will be 1 milliWatt)
   if (!aBeamOn && (power_dBmW > 0.0))
   {
      aBeamOn = true;
   }

   // Pulse repetition frequency (Hertz)
   aXmtrPtr->SetPulseRepetitionFrequency(aBeamPtr->GetPulseRepetitionFrequency());

   // Pulse width (microseconds DIS -> seconds WSF)
   aXmtrPtr->SetPulseWidth(aBeamPtr->GetPulseWidth() / 1.0E6);

   if (GetExtEmission()->GetDebugMask() & WsfExtEmission::cLEVEL_TWO_DEBUG)
   {
      auto out = ut::log::debug() << "WsfDisEmission: Transmitter state.";
      out.AddNote() << "Freq: " << aXmtrPtr->GetFrequency() << " hz";
      out.AddNote() << "BW: " << aXmtrPtr->GetBandwidth() << " hz";
      out.AddNote() << "Power: " << power_W << " watts";
      out.AddNote() << "Pulse Repeats: " << aXmtrPtr->GetPulseRepetitionFrequency() << " hz";
      out.AddNote() << "Pule Width: " << aXmtrPtr->GetPulseWidth() << " s";
   }
   mInterfacePtr->UpdateXmtrFromBeam(this, aBeamPtr, aXmtrPtr, aBeamOn);
}

// ============================================================================
//! Updates the articulated part location from the DIS emitter location data.
//!
//! @param aSystemPtr           [input] Pointer to the DIS emission PDU system.
//! @param aArticulatedPartPtr  [input] Pointer to the part to update.
void WsfDisEmission::UpdateLocation(DisSystem* aSystemPtr, WsfArticulatedPart* aArticulatedPartPtr)
{
   // Get location from the system
   DisFloat32 xyzDIS[3];
   aSystemPtr->GetLocation(xyzDIS[0], xyzDIS[1], xyzDIS[2]);

   // Set location of the entity part
   double xyz[3];
   UtVec3d::Set(xyz, static_cast<double>(xyzDIS[0]), static_cast<double>(xyzDIS[1]), static_cast<double>(xyzDIS[2]));
   aArticulatedPartPtr->SetLocation(xyz);
}

// ============================================================================
//!
//! @param aSysUniqueId        [input] Unique ID of the DIS system.
//! @param aBeamUniqueId       [input] Unique ID of the DIS beam.
//! @param aXmtrUniqueId       [input] Unique ID of the xmtr.
void WsfDisEmission::UpdateSysBeamMapping(unsigned int aSysUniqueId, unsigned int aBeamUniqueId, unsigned int aXmtrUniqueId)
{
   // Update the system id - beam id to the xmtr unique id mapping in the DIS platform
   WsfDisPlatform* disPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, GetEmittingEntityId());
   if (disPlatformPtr != nullptr)
   {
      WsfDisPlatform::SB_Key sbKey(aSysUniqueId, aBeamUniqueId);
      disPlatformPtr->SetSysBeamIdToPartId(sbKey, aXmtrUniqueId);
   }
}

WsfExtEmission* WsfDisEmission::GetExtEmission() const
{
   return WsfDisUtil::GetExtInterface(mInterfacePtr)->GetEmission();
}
