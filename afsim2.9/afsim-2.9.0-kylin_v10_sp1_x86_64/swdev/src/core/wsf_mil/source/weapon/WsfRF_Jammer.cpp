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

#include "WsfRF_Jammer.hpp"

#include <algorithm>
#include <climits>
#include <iostream>

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtVec2.hpp"
#include "UtVec3.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_Result.hpp"
#include "WsfEW_Types.hpp" // Defines a namespace of the same name.
#include "WsfFalseTarget.hpp"
#include "WsfFalseTargetScreener.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackProcessor.hpp"
#include "WsfUtil.hpp"
#include "WsfWeaponObserver.hpp"

WsfRF_Jammer::WsfRF_Jammer(const WsfScenario& aScenario)
   : WsfWeapon(aScenario)
   , mJammerGroup()
   , mJammerGroupNameId(nullptr)
   , mGroupPowerDistribution(cCONSTANT_POWER)
   , mJammerModeList()
   , mJammerInitialized(false)
   , mTargetDataMap()
   , mMutex()
{
   // Create the mode list with the jammer-specific mode template.
   SetModeList(new WsfModeList(new JammerMode(aScenario)));

   // Indicate that the jammer defaults to 'off'.
   WsfPlatformPart::SetInitiallyTurnedOn(false);
}

WsfRF_Jammer::WsfRF_Jammer(const WsfRF_Jammer& aSrc)
   : WsfWeapon(aSrc)
   , mJammerGroup(aSrc.mJammerGroup)
   , mJammerGroupNameId(aSrc.mJammerGroupNameId)
   , mGroupPowerDistribution(aSrc.mGroupPowerDistribution)
   , mJammerModeList()
   , mJammerInitialized(aSrc.mJammerInitialized)
   , mTargetDataMap()
   , mMutex()
{
}

// virtual
WsfWeapon* WsfRF_Jammer::Clone() const
{
   return new WsfRF_Jammer(*this);
}

// virtual
bool WsfRF_Jammer::Initialize(double aSimTime)
{
   bool ok = WsfWeapon::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mJammerModeList);

   // Determine other group members if any
   InitializeJammerGroup();

   // Set the initialized flag; always do this last, but before the StartJamming Commands below.
   mJammerInitialized = ok;

   // start jamming if system is initially turned on
   if (InitiallyTurnedOn() && mJammerInitialized)
   {
      TurnOn(aSimTime);
   }

   return ok;
}

// virtual
bool WsfRF_Jammer::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "jammer_group")
   {
      std::string jammerGroupName;
      aInput.ReadValue(jammerGroupName);
      mJammerGroupNameId = jammerGroupName;
      AddCategory(mJammerGroupNameId);
   }
   else if (command == "group_power_distribution")
   {
      std::string powerDistributionStr;
      aInput.ReadValue(powerDistributionStr);
      if (powerDistributionStr == "average")
      {
         mGroupPowerDistribution = cAVERAGE_POWER;
      }
      else if (powerDistributionStr == "constant")
      {
         mGroupPowerDistribution = cCONSTANT_POWER;
      }
   }
   // For disallowed commands, we want to "capture" them here, rather than let a
   // base class handle them. To avoid breaking backward compatibility in the middle
   // of a major release, a warning is issued rather than treating them as errors.
   else if ((command == "quantity") || (command == "maximum_quantity") || (command == "reload_increment") ||
            (command == "reload_inventory") || (command == "reload_threshold"))
   {
      ut::log::warning() << "The " << command << " command is incompatible with WSF_RF_JAMMER (ignored).";
      double dummy;
      aInput.ReadValue(dummy);
   }
   else if (command == "reload_time")
   {
      ut::log::warning() << "The reload_time command is incompatible with WSF_RF_JAMMER (ignored).";
      WsfRandomVariable dummy;
      dummy.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "inhibit_while_reloading")
   {
      ut::log::warning() << "The inhibit_while_reloading command is incompatible with WSF_RF_JAMMER (ignored).";
      aInput.ReadBool();
   }
   else
   {
      myCommand = WsfWeapon::ProcessInput(aInput);
   }

   return myCommand;
}

// virtual
const int* WsfRF_Jammer::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_RF_JAMMER,
                               cWSF_COMPONENT_WEAPON,
                               cWSF_COMPONENT_ARTICULATED_PART,
                               cWSF_COMPONENT_PLATFORM_PART,
                               cWSF_COMPONENT_NULL};
   return roles;
}

// virtual
void* WsfRF_Jammer::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_RF_JAMMER)
   {
      return this;
   }
   return WsfWeapon::QueryInterface(aRole);
}

// virtual
bool WsfRF_Jammer::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool        messageProcessed = false;
   WsfStringId messageType      = aMessage.GetType();
   if (messageType == WsfTrackMessage::GetTypeId())
   {
      messageProcessed               = true;
      const WsfTrackMessage& message = dynamic_cast<const WsfTrackMessage&>(aMessage);
      if (message.GetTrack()->GetUpdateCount() > 0)
      {
         SensorTrackUpdated(aSimTime, message.GetTrack());
      }
      else
      {
         SensorTrackInitiated(aSimTime, message.GetTrack());
      }
   }
   else if (messageType == WsfTrackDropMessage::GetTypeId())
   {
      messageProcessed                   = true;
      const WsfTrackDropMessage& message = dynamic_cast<const WsfTrackDropMessage&>(aMessage);
      SensorTrackDropped(aSimTime, message.GetTrackId(), message.GetTargetIndex());
   }
   return messageProcessed;
}

// virtual
void WsfRF_Jammer::TurnOff(double aSimTime)
{
   if (mJammerInitialized && (GetEM_XmtrCount(mModeListPtr->GetCurrentMode()) > 0))
   {
      StopJamming(aSimTime, 0.0, 0.0, 0, WsfTrackId());
   }
   WsfWeapon::TurnOff(aSimTime);
}

// virtual
void WsfRF_Jammer::TurnOn(double aSimTime)
{
   WsfWeapon::TurnOn(aSimTime);

   // Start a jamming assignment if system is initialized AND no spots have been started AND not using a repeater.
   // Note: The GetEM_XmtrCount checks protects against the calls from StartJamming(...) and
   //       for a system that has already been initially started and turned off.
   if ((GetEM_XmtrCount(mModeListPtr->GetCurrentMode()) == 0) && mJammerInitialized)
   {
      // Only select the initial mode or first mode entered to start jamming with.
      JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];

      if (!modePtr->mUseRepeater)
      {
         // If the user input file turns on debug for this weapon the StartJamming method
         // will return a statement as to whether jamming was started or not.
         WsfTrackId trackId(nullptr, 0);
         StartJamming(aSimTime, modePtr->mXmtr->GetFrequency(), modePtr->mXmtr->GetBandwidth(), 0, nullptr, trackId, 0);
      }
   }
}

// virtual
size_t WsfRF_Jammer::GetEM_XmtrCount() const
{
   size_t xmtrCount = 0;
   // Need this check because DIS platform constructor is trying to get xmtr count before initialization
   if (mJammerInitialized)
   {
      JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
      xmtrCount           = modePtr->mActiveXmtrSpotPtrs.size();
   }
   return xmtrCount;
}

// virtual
WsfEM_Xmtr& WsfRF_Jammer::GetEM_Xmtr(size_t aIndex)
{
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
   return *(modePtr->mActiveXmtrSpotPtrs[aIndex]);
}

// virtual
size_t WsfRF_Jammer::GetEM_XmtrCount(size_t aModeIndex) const
{
   size_t xmtrCount = 0;
   if ((aModeIndex < mJammerModeList.size()) && mJammerInitialized) // Need this check because DIS platform constructor
                                                                    // is trying to get xmtr count before initialization

   {
      JammerMode* modePtr = mJammerModeList[aModeIndex];
      xmtrCount           = modePtr->mActiveXmtrSpotPtrs.size();
   }
   return xmtrCount;
}

// virtual
WsfEM_Xmtr& WsfRF_Jammer::GetEM_Xmtr(size_t aModeIndex, size_t aIndex)
{
   if (aModeIndex < mJammerModeList.size())
   {
      JammerMode* modePtr = mJammerModeList[aModeIndex];
      if (aIndex < modePtr->mActiveXmtrSpotPtrs.size())
      {
         return *(modePtr->mActiveXmtrSpotPtrs[aIndex]);
      }
   }
   return WsfWeapon::GetEM_Xmtr(aModeIndex, aIndex); // return dummy xmtr
}

// virtual
WsfEM_Xmtr& WsfRF_Jammer::GetEM_Xmtr(size_t aModeIndex, size_t aBeamIndex, size_t aSpotIndex)
{
   if (aModeIndex < mJammerModeList.size())
   {
      JammerMode* modePtr = mJammerModeList[aModeIndex];
      if (aBeamIndex < modePtr->mActiveBeamPtrs.size())
      {
         JammerBeam* beamPtr = modePtr->mActiveBeamPtrs[aBeamIndex];
         if (aSpotIndex < beamPtr->mActiveSpotPtrs.size())
         {
            return *(beamPtr->mActiveSpotPtrs[aSpotIndex]);
         }
      }
   }
   return WsfWeapon::GetEM_Xmtr(aModeIndex, aBeamIndex, aSpotIndex); // return dummy xmtr
}

// virtual
WsfEM_Xmtr& WsfRF_Jammer::GetMasterXmtr()
{
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
   return *(*modePtr).mXmtr;
}

WsfEM_Xmtr&
WsfRF_Jammer::GetJammerXmtr(double aSimTime, double aFrequency, double aBandwidth, size_t aBeamNumber, WsfTrackId aTrackId)
{
   // Get a pointer to the current mode
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];

   BeamSpotIndexMap beamSpotIndexMap;
   if (modePtr->GetActiveBeamSpotIndexes(aFrequency, aBandwidth, aBeamNumber, aTrackId, beamSpotIndexMap))
   {
      // Grab the first one in the list that matches and ignore all others
      auto iterBeamSpotMap = beamSpotIndexMap.begin();
      // SpotIndexVec spotIndexVec = iterBeamSpotMap->second;

      // Get the beamPtr and xmtrSpot from index
      JammerBeam* beamPtr = modePtr->mActiveBeamPtrs[iterBeamSpotMap->first];
      return *(beamPtr->mActiveSpotPtrs[iterBeamSpotMap->second[0]]);
   }
   return WsfWeapon::GetEM_Xmtr(0); // return dummy xmtr
}

double WsfRF_Jammer::GetMinimumFrequency() const
{
   // Get a pointer to the current mode
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
   return (modePtr->mXmtr->GetFrequency() - 0.5 * modePtr->mXmtr->GetBandwidth());
}

double WsfRF_Jammer::GetMaximumFrequency() const
{
   // Get a pointer to the current mode
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
   return (modePtr->mXmtr->GetFrequency() + 0.5 * modePtr->mXmtr->GetBandwidth());
}

// virtual
unsigned int WsfRF_Jammer::GetActiveRequestCount() const
{
   unsigned int requestCount = 0;
   // Need this check because DIS platform constructor is trying to get xmtr count before initialization
   if (mJammerInitialized)
   {
      // Does not take into account jammer groups
      JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
      requestCount        = static_cast<unsigned int>(modePtr->mActiveXmtrSpotPtrs.size());
   }
   return requestCount;
}

// virtual
unsigned int WsfRF_Jammer::GetMaximumRequestCount() const
{
   size_t maxRequestCount = 0;
   // Need this check because DIS platform constructor is trying to get xmtr count before initialization
   if (mJammerInitialized)
   {
      // Does not take into account jammer groups
      JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
      maxRequestCount     = modePtr->mMaximumNumSpots;
   }
   return static_cast<unsigned int>(maxRequestCount);
}

// virtual
size_t WsfRF_Jammer::GetActiveBeamCount() const
{
   size_t beamCount = 0;
   // Need this check because DIS platform constructor is trying to get xmtr count before initialization
   if (mJammerInitialized)
   {
      // Does not take into account jammer groups
      JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
      beamCount           = modePtr->GetActiveBeamCount();
   }
   return beamCount;
}

// virtual
size_t WsfRF_Jammer::GetMaximumBeamCount() const
{
   size_t beamCount = 0;
   // Need this check because DIS platform constructor is trying to get xmtr count before initialization
   if (mJammerInitialized)
   {
      // Does not take into account jammer groups
      JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
      beamCount           = (modePtr->GetActiveBeamCount() + modePtr->GetAvailableBeamCount());
   }
   return beamCount;
}

// virtual
size_t WsfRF_Jammer::GetActiveSpotCount(size_t aBeamNumber) const
{
   size_t spotCount = 0;
   // Need this check because DIS platform constructor is trying to get xmtr count before initialization
   if (mJammerInitialized)
   {
      // Does not take into account jammer groups
      JammerMode* modePtr      = mJammerModeList[mModeListPtr->GetCurrentMode()];
      bool        beamIsActive = false;
      JammerBeam* beamPtr      = modePtr->GetJammerBeam(aBeamNumber, beamIsActive);
      if ((beamPtr != nullptr) && beamIsActive)
      {
         spotCount = beamPtr->GetActiveSpotCount();
      }
   }
   return spotCount;
}

// virtual
size_t WsfRF_Jammer::GetMaximumSpotCount(size_t aBeamNumber) const
{
   size_t spotCount = 0;
   // Need this check because DIS platform constructor is trying to get xmtr count before initialization
   if (mJammerInitialized)
   {
      // Does not take into account jammer groups
      JammerMode* modePtr      = mJammerModeList[mModeListPtr->GetCurrentMode()];
      bool        beamIsActive = false;
      JammerBeam* beamPtr      = modePtr->GetJammerBeam(aBeamNumber, beamIsActive);
      if (beamPtr != nullptr)
      {
         spotCount = (beamPtr->GetActiveSpotCount() + beamPtr->GetAvailableSpotCount());
      }
   }
   return spotCount;
}


// virtual
double WsfRF_Jammer::GetQuantityRemaining() const
{
   return (GetMaximumRequestCount() - GetActiveRequestCount());
}

// virtual
bool WsfRF_Jammer::StartJammingTrack(double aSimTime, WsfStringId aTechniqueId, const WsfTrack* aTrackPtr)
{
   // Set the default return value
   bool startedJamming(false);

   WsfTrack::Signal signal;
   bool             signalsSuccessful(true);
   for (unsigned int signalIndex = 0; signalIndex < aTrackPtr->GetSignalCount(); ++signalIndex)
   {
      // Check if there is spots remaining and not a barrage jammer for continuing on.
      if ((GetQuantityRemaining() <= 0.0) && (GetMaximumRequestCount() > 1))
      {
         break;
      }

      aTrackPtr->GetSignalEntry(signalIndex, signal);
      double bandwidth = signal.mUpperFrequency - signal.mLowerFrequency;
      double frequency = signal.mLowerFrequency + (bandwidth / 2);
      if (signal.mPulseWidth > 0.0)
      {
         bandwidth = 1.0 / signal.mPulseWidth;
      }

      if ((GetMinimumFrequency() <= frequency) && (GetMaximumFrequency() >= frequency))
      {
         signalsSuccessful &=
            StartJamming(aSimTime, frequency, bandwidth, 0, aTechniqueId, aTrackPtr->GetTrackId(), aTrackPtr->GetTargetIndex());
      }
      startedJamming |= signalsSuccessful;
   }

   // Get a pointer to the current mode
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
   if (startedJamming && modePtr->mUseRepeater)
   {
      modePtr->mRepeater.RegisterTrack(aTrackPtr);
   }

   return (startedJamming && signalsSuccessful);
}

// virtual
bool WsfRF_Jammer::StartJamming(double      aSimTime,
                                double      aFrequency,
                                double      aBandwidth,
                                size_t      aBeamNumber,
                                WsfStringId aTechniqueId,
                                WsfTrackId  aTrackId,
                                size_t      aTargetIndex)
{
   // Set the default return value
   bool startedJamming(false);

   // Get a pointer to the current mode
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];

   bool beamIsActive(false);
   modePtr->mBeamAllocationChanged = false;
   JammerBeam* beamPtr             = modePtr->GetNextAssignableBeam(aBeamNumber, beamIsActive);
   if (beamPtr != nullptr)
   {
      // Get a xmtr for the spot on the selected beam
      unsigned int numSpotsAvailable = static_cast<unsigned int>(beamPtr->mAvailableSpotPtrs.size());
      JammerXmtr*  xmtrPtr(nullptr);
      if (numSpotsAvailable > 0)
      {
         xmtrPtr = beamPtr->mAvailableSpotPtrs[numSpotsAvailable - 1];
         beamPtr->mAvailableSpotPtrs.pop_back();
      }
      else if (modePtr->mMaximumNumSpots == 1 && numSpotsAvailable == 0)
      {
         xmtrPtr = beamPtr->mActiveSpotPtrs[0];
         if (xmtrPtr != nullptr)
         {
            xmtrPtr->Deactivate();
         }
         beamPtr->mAvailableSpotPtrs.clear();
         beamPtr->mActiveSpotPtrs.clear();
         beamPtr->mSpotTrackIds.clear();
         modePtr->mActiveXmtrSpotPtrs.clear();
      }

      if (xmtrPtr != nullptr)
      {
         WsfEW_EA_EP* ea_ep_Ptr = WsfEW_EA::GetElectronicAttack(*(modePtr->mXmtr));
         if (ea_ep_Ptr != nullptr)
         {
            WsfEW_EA_EP* eaPtr = dynamic_cast<WsfEW_EA*>(ea_ep_Ptr->Clone());
            WsfEW_EA::SetElectronicAttack(*xmtrPtr, eaPtr);
         }

         // Check to see if the frequency boundary conditions are okay.
         if (((GetMinimumFrequency() <= aFrequency) && (GetMaximumFrequency() >= aFrequency)) ||
             (modePtr->mMaximumNumSpots == 1))
         {
            // Adjust the frequency and bandwidth based on the input if necessary.
            double frequency = aFrequency;
            double bandwidth = aBandwidth;
            modePtr->AdjustFrequencyBandwidth(frequency, bandwidth);

            // Set the adjusted frequency and bandwidth.
            xmtrPtr->SetFrequency(frequency);
            xmtrPtr->SetBandwidth(bandwidth);

            // Initialize and save the spot,  first time to set startedJamming to 'true'- Do NOT use &=
            startedJamming =
               xmtrPtr->Initialize(*GetSimulation()); // will reset the EA techniques in use to default_on ones

            // Set the technique based on the input must do this after the Initializing the transmitter above
            if (!aTechniqueId.IsNull())
            {
               WsfEW_EA_EP* eaPtr = WsfEW_EA::GetElectronicAttack(*xmtrPtr);
               if (eaPtr != nullptr)
               {
                  startedJamming &= eaPtr->SelectTechnique(aTechniqueId);
               }
            }
         }
      }

      auto out = ut::log::debug();
      if (startedJamming)
      {
         // Add to master xmtr list for this mode
         modePtr->mActiveXmtrSpotPtrs.push_back(xmtrPtr);

         // Set the targeted location on the transmitter
         beamPtr->mActiveSpotPtrs.push_back(xmtrPtr);
         beamPtr->mSpotTrackIds.push_back(aTrackId);

         SetTargetIndex(xmtrPtr->GetUniqueId(), aTargetIndex);
         SetTargetTrackId(xmtrPtr->GetUniqueId(), aTrackId);

         // Check if the beams is active, if NOT, then move from available to active
         if (!beamIsActive)
         {
            modePtr->mActiveBeamPtrs[(beamPtr->GetBeamNumber() - 1)] = beamPtr;
            modePtr->mAvailableBeamPtrs.erase((beamPtr->GetBeamNumber() - 1));
            modePtr->mBeamAllocationChanged = true;
         }

         // Turn on the jammer if not on
         if (!IsTurnedOn())
         {
            TurnOn(aSimTime); // calls modePtr->Select(aSimTime)
         }
         else
         {
            modePtr->Select(aSimTime);
         }

         if (modePtr->mUseRepeater)
         {
            modePtr->mRepeater.RegisterTrackId(aTrackId, aTargetIndex);
         }

         // Adjust the power and antenna beam counts.
         // NOTE: needs to follow the beam map and spot vector routines above
         modePtr->AdjustPowerDistribution(mGroupPowerDistribution, GetActiveGroupCount());

         WsfObserver::JammingRequestInitiated(
            GetSimulation())(aSimTime, this, xmtrPtr->GetFrequency(), xmtrPtr->GetBandwidth(), aTechniqueId, aTargetIndex);
         if (DebugEnabled())
         {
            out << "Started jamming on track.";
         }
      }
      else
      {
         // put the xmtr back into the available list if it is valid.
         if (xmtrPtr != nullptr)
         {
            beamPtr->mAvailableSpotPtrs.push_back(xmtrPtr);
         }

         if (DebugEnabled())
         {
            out << "Failed to start jamming on track.";
         }
      }

      if (DebugEnabled())
      {
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Jammer: " << GetName();
         if (GetPlatform()->GetName().compare(0, 5, "<dis>") == 0)
         {
            out.AddNote() << "Platform Type: " << GetPlatform()->GetType();
            out.AddNote() << "Jammer Type: " << GetType();
         }
         out.AddNote() << "Owning Platform Id: " << aTrackId.GetOwningPlatformId();
         out.AddNote() << "Local Track Number: " << aTrackId.GetLocalTrackNumber();
         out.AddNote() << "Target Index: " << aTargetIndex;
         if (startedJamming)
         {
            out.AddNote() << "Frequency: " << xmtrPtr->GetFrequency() << " Hz";
            out.AddNote() << "Bandwidth: " << xmtrPtr->GetBandwidth() << " Hz";
         }
         else
         {
            out.AddNote() << "Frequency: " << aFrequency << " Hz";
            out.AddNote() << "Bandwidth: " << aBandwidth << " Hz";
         }
         if (beamPtr)
         {
            if (beamPtr->mBeamNumber > 0)
            {
               out.AddNote() << "Beam Number: " << beamPtr->mBeamNumber;
            }
         }
         if (!aTechniqueId.IsNull())
         {
            out.AddNote() << "Technique: " << aTechniqueId;
         }
      }
   }

   return startedJamming;
}

// virtual
bool WsfRF_Jammer::StopJamming(double aSimTime, double aFrequency, double aBandwidth, size_t aBeamNumber, WsfTrackId aTrackId)
{
   // Set the default return value
   bool stoppedJamming(false);

   // Get a pointer to the current mode
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];

   modePtr->mBeamAllocationChanged = false;

   // need the beamPtr and xmtrSpot and index into mActiveSpotPtrs, which is the same as the index into the
   // mSpotTrackIds lists.
   BeamSpotIndexMap beamSpotIndexMap;
   if (modePtr->GetActiveBeamSpotIndexes(aFrequency, aBandwidth, aBeamNumber, aTrackId, beamSpotIndexMap))
   {
      auto iterBeamSpotMap = beamSpotIndexMap.begin();
      for (; iterBeamSpotMap != beamSpotIndexMap.end(); ++iterBeamSpotMap)
      {
         unsigned int beamIndex = iterBeamSpotMap->first;

         // need the beamPtr and xmtrSpot from index
         JammerBeam* beamPtr = modePtr->mActiveBeamPtrs[beamIndex];

         SpotIndexVec* spotIndexVec = &(iterBeamSpotMap->second);
         while ((!spotIndexVec->empty()) && (beamPtr != nullptr))
         {
            // Grab the largest index in the list and start from there so that we can go in
            // descending order and don't mess up the order in the mActiveSpots, and mSpotTrackIds.
            unsigned int spotIndex = *max_element(spotIndexVec->begin(), spotIndexVec->end());
            spotIndexVec->erase(max_element(spotIndexVec->begin(), spotIndexVec->end())); // remove this element

            JammerXmtr* xmtrPtr = beamPtr->mActiveSpotPtrs[spotIndex];

            // Deactivate the xmtr
            xmtrPtr->Deactivate();

            // Add back into the available list
            beamPtr->mAvailableSpotPtrs.push_back(xmtrPtr);

            // Remove from active list
            beamPtr->mActiveSpotPtrs.erase((beamPtr->mActiveSpotPtrs.begin() + spotIndex));

            // Remove from active xmtr list
            auto iterXmtr = find(modePtr->mActiveXmtrSpotPtrs.begin(), modePtr->mActiveXmtrSpotPtrs.end(), xmtrPtr);
            if (iterXmtr != modePtr->mActiveXmtrSpotPtrs.end())
            {
               modePtr->mActiveXmtrSpotPtrs.erase(iterXmtr);
            }

            // Store and clear some data
            size_t     targetIndex = GetTargetIndex(xmtrPtr->GetUniqueId());
            WsfTrackId trackId     = beamPtr->mSpotTrackIds.at(spotIndex);
            beamPtr->mSpotTrackIds.erase((beamPtr->mSpotTrackIds.begin() + spotIndex));
            mTargetDataMap.erase(xmtrPtr->GetUniqueId());

            // the below is for future implementation
            // if (modePtr->mUseRepeater)
            //{
            //   if ((modePtr->mRepeater.GetOperatingMode() == WsfRF_Repeater::cOM_MANUAL) &&
            //   modePtr->mRepeater.IsSignalFollowing())
            //   {
            //      modePtr->mRepeater.RemoveTrackId(aTrackId, targetIndex);
            //   }
            //}

            // Notify and erase target entry
            WsfObserver::JammingRequestCanceled(
               GetSimulation())(aSimTime, this, xmtrPtr->GetFrequency(), xmtrPtr->GetBandwidth(), targetIndex);

            // Check if the spot list for the beam is empty, if so, then move from active to available
            if (beamPtr->mActiveSpotPtrs.empty())
            {
               modePtr->mAvailableBeamPtrs[(beamPtr->GetBeamNumber() - 1)] = beamPtr;
               modePtr->mActiveBeamPtrs.erase((beamPtr->GetBeamNumber() - 1));
               modePtr->mBeamAllocationChanged = true;
            }

            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Stopped jamming on track.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Jammer: " << GetName();
               if (GetPlatform()->GetName().compare(0, 5, "<dis>") == 0)
               {
                  out.AddNote() << "Platform Type: " << GetPlatform()->GetType();
                  out.AddNote() << "Jammer Type: " << GetType();
               }
               out.AddNote() << "Owning Platform Id: " << trackId.GetOwningPlatformId();
               out.AddNote() << "Local Track Number: " << trackId.GetLocalTrackNumber();
               out.AddNote() << "Target Index: " << targetIndex;
               out.AddNote() << "Frequency: " << xmtrPtr->GetFrequency() << " Hz";
               out.AddNote() << "Bandwidth: " << xmtrPtr->GetBandwidth() << " Hz";
               if (beamPtr->mBeamNumber > 0)
               {
                  out.AddNote() << "Beam Number: " << beamPtr->mBeamNumber;
               }
            }

            stoppedJamming = true;
         }
      }

      // Adjust the power and antenna beam counts.
      // NOTE: needs to follow the beam map and spot vector routines above
      modePtr->AdjustPowerDistribution(mGroupPowerDistribution, GetActiveGroupCount());
   }

   // Check the spot list if empty and not using repeater then turn off jammer
   // This will work for barrage jammer also
   if (modePtr->mActiveXmtrSpotPtrs.empty() && (!modePtr->mUseRepeater))
   {
      if (IsTurnedOn())
      {
         TurnOff(aSimTime); // calls modePtr->Deselect(aSimTime)
      }
      else
      {
         modePtr->Deselect(aSimTime);
      }
   }

   if (!stoppedJamming)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Failed to stop jamming.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Jammer: " << GetName();
         if (GetPlatform()->GetName().compare(0, 5, "<dis>") == 0)
         {
            out.AddNote() << "Platform Type: " << GetPlatform()->GetType();
            out.AddNote() << "Jammer Type: " << GetType();
         }
         if (aTrackId.GetLocalTrackNumber() != 0 || aTrackId.GetOwningPlatformId() != 0)
         {
            out.AddNote() << "Owning Platform Id: " << aTrackId.GetOwningPlatformId();
            out.AddNote() << "Local Track Number: " << aTrackId.GetLocalTrackNumber();
         }
         out.AddNote() << "Frequency: " << aFrequency << " Hz";
         out.AddNote() << "Bandwidth: " << aBandwidth << " Hz";
         if (aBeamNumber > 0)
         {
            out.AddNote() << "Beam Number: " << aBeamNumber;
         }
      }
   }

   return stoppedJamming;
}

// virtual
bool WsfRF_Jammer::UpdateJamming(double      aSimTime,
                                 size_t      aXmtrIndex,
                                 double      aFrequency,
                                 double      aBandwidth,
                                 WsfStringId aTechniqueId,
                                 WsfTrackId  aTrackId,
                                 size_t      aTargetIndex)
{
   // Set the default return value
   bool updatedJamming(false);

   // Get a pointer to the current mode
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
   JammerXmtr* xmtrPtr = dynamic_cast<JammerXmtr*>(&GetEM_Xmtr(aXmtrIndex));
   if ((modePtr == nullptr) || (xmtrPtr == nullptr))
   {
      return updatedJamming;
   }

   bool        beamIsActive(false);
   JammerBeam* beamPtr = modePtr->GetJammerBeam(xmtrPtr->GetIndex(), beamIsActive);
   if (beamPtr == nullptr)
   {
      return updatedJamming;
   }

   auto svi = find(beamPtr->mActiveSpotPtrs.begin(), beamPtr->mActiveSpotPtrs.end(), xmtrPtr);
   if (svi == beamPtr->mActiveSpotPtrs.end())
   {
      return updatedJamming;
   }

   bool freqBW_Update =
      (fabs(xmtrPtr->GetFrequency() - aFrequency) > 0.001) || (fabs(xmtrPtr->GetBandwidth() - aBandwidth) > 0.001);
   WsfEW_EA_EP* eaPtr     = WsfEW_EA::GetElectronicAttack(*xmtrPtr);
   bool         EA_Update = (!aTechniqueId.IsNull()) && (eaPtr != nullptr) && (!eaPtr->TechniqueIsInUse(aTechniqueId));
   bool         targetOrTrackUpdate = (GetTargetIndex(xmtrPtr->GetUniqueId()) != aTargetIndex) ||
                              (GetTargetTrackId(xmtrPtr->GetUniqueId()) != aTrackId);

   bool updatedRequired = freqBW_Update || EA_Update || targetOrTrackUpdate;

   if (updatedRequired)
   {
      // Set to 'true' then only use as an error catch for updates.
      updatedJamming = true;

      if (freqBW_Update)
      {
         // Check to see if the frequency boundary conditions are okay.
         if (((GetMinimumFrequency() <= aFrequency) && (GetMaximumFrequency() >= aFrequency)) ||
             (modePtr->mMaximumNumSpots == 1))
         {
            // Adjust the frequency and bandwidth based on the input if necessary.
            double frequency = aFrequency;
            double bandwidth = aBandwidth;
            modePtr->AdjustFrequencyBandwidth(frequency, bandwidth);

            // Set the adjusted frequency and bandwidth.
            xmtrPtr->SetFrequency(frequency);
            xmtrPtr->SetBandwidth(bandwidth);
         }
      }

      if (freqBW_Update || EA_Update)
      {
         // Initialize the spot
         updatedJamming &= xmtrPtr->Initialize(*GetSimulation()); // will reset the EA techniques in use to default_on ones

         // Set the technique based on the input must do this after the Initializing the transmitter above
         if (!aTechniqueId.IsNull())
         {
            if (eaPtr != nullptr)
            {
               updatedJamming &= eaPtr->SelectTechnique(aTechniqueId);
            }
         }
      }

      if (targetOrTrackUpdate)
      {
         // Erase spot and track id from active list and trackId list and re-add.
         size_t pos = svi - beamPtr->mActiveSpotPtrs.begin();
         beamPtr->mActiveSpotPtrs.erase(svi);
         beamPtr->mSpotTrackIds.erase((beamPtr->mSpotTrackIds.begin() + pos));

         // Set the targeted location on the transmitter
         beamPtr->mActiveSpotPtrs.push_back(xmtrPtr);
         beamPtr->mSpotTrackIds.push_back(aTrackId);

         SetTargetIndex(xmtrPtr->GetUniqueId(), aTargetIndex);
         SetTargetTrackId(xmtrPtr->GetUniqueId(), aTrackId);

         if (modePtr->mUseRepeater)
         {
            modePtr->mRepeater.RegisterTrackId(aTrackId, aTargetIndex);
         }
      }

      auto out = ut::log::debug();
      if (updatedJamming)
      {
         WsfObserver::JammingRequestUpdated(
            GetSimulation())(aSimTime, this, xmtrPtr->GetFrequency(), xmtrPtr->GetBandwidth(), aTechniqueId, aTargetIndex);

         if (DebugEnabled())
         {
            out << "Updated jamming.";
         }
      }
      else
      {
         if (DebugEnabled())
         {
            out << "Updated jamming failed.";
         }
      }

      if (DebugEnabled())
      {
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Jammer: " << GetName();
         if (GetPlatform()->GetName().compare(0, 5, "<dis>") == 0)
         {
            out.AddNote() << "Platform Type: " << GetPlatform()->GetType();
            out.AddNote() << "Jammer Type: " << GetType();
         }
         out.AddNote() << "Owning Platform Id: " << aTrackId.GetOwningPlatformId();
         out.AddNote() << "Local Track Number: " << aTrackId.GetLocalTrackNumber();
         out.AddNote() << "Target Index: " << aTargetIndex;
         if (updatedJamming)
         {
            out.AddNote() << "Frequency: " << xmtrPtr->GetFrequency() << " Hz";
            out.AddNote() << "Bandwidth: " << xmtrPtr->GetBandwidth() << " Hz";
         }
         else
         {
            out.AddNote() << "Frequency: " << aFrequency << " Hz";
            out.AddNote() << "Bandwidth: " << aBandwidth << " Hz";
         }
         if (beamPtr->mBeamNumber > 0)
         {
            out.AddNote() << "Beam Number: " << beamPtr->mBeamNumber;
         }
         if (!aTechniqueId.IsNull())
         {
            out.AddNote() << "Technique: " << aTechniqueId;
         }
      }
   }

   return updatedJamming;
}

// virtual
void WsfRF_Jammer::SetTargetIndex(int aId, size_t aTargetIndex)
{
   mTargetDataMap[aId].mTargetIndex = aTargetIndex;
}

// virtual
size_t WsfRF_Jammer::GetTargetIndex(int aId) const
{
   size_t targetIndex = 0;
   auto   tdmi        = mTargetDataMap.find(aId);
   if (tdmi != mTargetDataMap.end())
   {
      targetIndex = tdmi->second.mTargetIndex;
   }
   return targetIndex;
}

void WsfRF_Jammer::SetTargetTrackId(int aId, const WsfTrackId& aTargetTrackId)
{
   mTargetDataMap[aId].mTargetTrackId = aTargetTrackId;

   // Set the target index also for sending out over DIS if the track is valid.
   if (!aTargetTrackId.IsNull())
   {
      auto* targetTrackPtr = FindTargetTrack(aTargetTrackId);
      if (targetTrackPtr != nullptr)
      {
         SetTargetIndex(aId, targetTrackPtr->GetTargetIndex());
      }
   }
}

// virtual
WsfTrackId WsfRF_Jammer::GetTargetTrackId(int aId) const
{
   WsfTrackId trackId(nullptr, 0);
   auto       tdmi = mTargetDataMap.find(aId);
   if (tdmi != mTargetDataMap.end())
   {
      trackId = tdmi->second.mTargetTrackId;
   }
   return trackId;
}

void WsfRF_Jammer::SetTargetedLocationWCS(int aId, const double aTargetedLocationWCS[3])
{
   UtVec3d::Set(mTargetDataMap[aId].mTargetedLocationWCS, aTargetedLocationWCS);
   mTargetDataMap[aId].mTargetedLocationValid = true;
}

void WsfRF_Jammer::GetTargetedLocationWCS(int aId, double aTargetedLocationWCS[3]) const
{
   auto tdmi = mTargetDataMap.find(aId);
   if (tdmi != mTargetDataMap.end())
   {
      UtVec3d::Set(aTargetedLocationWCS, tdmi->second.mTargetedLocationWCS);
   }
}

// virtual
bool WsfRF_Jammer::TargetedLocationValid(int aId) const
{
   bool locValid = false;
   auto tdmi     = mTargetDataMap.find(aId);
   if (tdmi != mTargetDataMap.end())
   {
      locValid = tdmi->second.mTargetedLocationValid;
   }
   return locValid;
}

bool WsfRF_Jammer::SelectTarget(double aSimTime, WsfEM_Xmtr* aXmtr)
{
   double targetlocation[3];

   // Check the track first the target index.
   // The track has priority over the target index as it is also filled in for a track centric jammer assignment.
   int        xmtrId  = aXmtr->GetUniqueId();
   WsfTrackId trackId = GetTargetTrackId(xmtrId);
   if (!trackId.IsNull())
   {
      auto* targetTrackPtr = FindTargetTrack(trackId);
      if (targetTrackPtr != nullptr)
      {
         if (targetTrackPtr->LocationValid())
         {
            targetTrackPtr->GetExtrapolatedLocationWCS(aSimTime, targetlocation);
            SetTargetedLocationWCS(xmtrId, targetlocation);
         }
      }
   }
   else if (GetTargetIndex(xmtrId) > 0)
   {
      // Get target platform pointer
      WsfPlatform* tempPlatformPtr = GetSimulation()->GetPlatformByIndex(GetTargetIndex(xmtrId));
      if (tempPlatformPtr != nullptr)
      {
         tempPlatformPtr->GetLocationWCS(targetlocation);
         SetTargetedLocationWCS(xmtrId, targetlocation);
      }
   }

   if (TargetedLocationValid(xmtrId))
   {
      double targetLocationWCS[3];
      GetTargetedLocationWCS(xmtrId, targetLocationWCS);
      SetTransientCuedLocationWCS(targetLocationWCS);
      return true;
   }
   return false;
}

bool WsfRF_Jammer::ClearTarget(WsfEM_Xmtr* aXmtr)
{
   if (static_cast<WsfRF_Jammer*>(aXmtr->GetArticulatedPart())->TargetedLocationValid(aXmtr->GetUniqueId()))
   {
      aXmtr->GetArticulatedPart()->ClearTransientCue();
      return true;
   }
   return false;
}

WsfLocalTrack* WsfRF_Jammer::FindTargetTrack(const WsfTrackId& aTargetTrackId) const
{
   // Get owning platform pointer
   WsfPlatform*   platformPtr    = GetSimulation()->GetPlatformByName(aTargetTrackId.GetOwningPlatformId());
   WsfLocalTrack* targetTrackPtr = platformPtr->GetTrackManager().FindTrack(aTargetTrackId);
   if (targetTrackPtr == nullptr)
   {
      targetTrackPtr = platformPtr->GetTrackManager().FindCorrelatedTrack(aTargetTrackId);
      if (targetTrackPtr == nullptr)
      {
         for (WsfComponentList::RoleIterator<WsfProcessor> iter(*platformPtr); !iter.AtEnd(); ++iter)
         {
            auto* tpPtr = dynamic_cast<WsfTrackProcessor*>(*iter);
            if (tpPtr != nullptr)
            {
               targetTrackPtr = tpPtr->GetTrackManager().FindTrack(aTargetTrackId);
               if (targetTrackPtr != nullptr)
               {
                  break;
               }
            }
         }
      }
   }
   return targetTrackPtr;
}

// static
bool WsfRF_Jammer::UpdateRepeaterResult(double                   aSimTime,
                                        WsfEM_Xmtr*              aXmtr,
                                        WsfSensorResult&         aResult,
                                        WsfEM_Interaction&       aJammerInteraction,
                                        const WsfEM_Interaction& aTargetInteraction)
{
   WsfEW_Result::FindOrCreate(aJammerInteraction);
   bool updated = false;
   aResult.Reset();
   JammerMode* jammerModePtr = dynamic_cast<JammerMode*>(aXmtr->GetMode());
   if (jammerModePtr != nullptr)
   {
      if (jammerModePtr->mUseRepeater)
      {
         WsfEW_Result& ewJammerResult = WsfEW_Result::FindOrCreate(aJammerInteraction);
         jammerModePtr->mRepeater.CheckDetectAndUpdate(aSimTime, aXmtr, aResult, aTargetInteraction);
         ewJammerResult.mEW_Effects.mCurrentRepeaterData = WsfEW_Result::Find(aResult)->mEW_Effects.mCurrentRepeaterData;
         ewJammerResult.mEW_Effects.mMask                = WsfEW_Result::Find(aResult)->mEW_Effects.mMask;
         updated                                         = true;
      }
   }
   return updated;
}

// virtual
bool WsfRF_Jammer::SelectEA_Technique(double      aSimTime,
                                      double      aFrequency,
                                      double      aBandwidth,
                                      size_t      aBeamNumber,
                                      WsfStringId aTechniqueId,
                                      WsfTrackId  aTrackId)
{
   // Set the default return value
   bool techniqueSelected(false);

   // Get a pointer to the current mode
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];

   // while an entry is found that matches the search criteria select that technique
   // if available
   BeamSpotIndexMap beamSpotIndexMap;
   if (modePtr->GetActiveBeamSpotIndexes(aFrequency, aBandwidth, aBeamNumber, aTrackId, beamSpotIndexMap))
   {
      for (auto& beamSpotIndex : beamSpotIndexMap)
      {
         unsigned int beamIndex = beamSpotIndex.first;

         SpotIndexVec spotIndexVec = beamSpotIndex.second;
         for (unsigned int spotIndex : spotIndexVec)
         {
            // need the beamPtr and xmtrSpot from index
            JammerBeam* beamPtr = modePtr->mActiveBeamPtrs[beamIndex];
            JammerXmtr* xmtrPtr = beamPtr->mActiveSpotPtrs[spotIndex];

            bool         selectedTechnique(false);
            WsfEW_EA_EP* eaPtr = WsfEW_EA::GetElectronicAttack(*xmtrPtr);
            if (eaPtr != nullptr)
            {
               selectedTechnique = eaPtr->SelectTechnique(aTechniqueId);
            }

            if (DebugEnabled())
            {
               auto out = ut::log::debug();
               if (selectedTechnique)
               {
                  out << "Selected EA technique.";
               }
               else
               {
                  out << "Failed selecting EA technique.";
               }

               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Jammer: " << GetName();
               out.AddNote() << "Technique: " << aTechniqueId;
               out.AddNote() << "Owning Platform Id: " << aTrackId.GetOwningPlatformId();
               out.AddNote() << "Local Track Number: " << aTrackId.GetLocalTrackNumber();
               if (selectedTechnique)
               {
                  out.AddNote() << "Frequency: " << xmtrPtr->GetFrequency() << " Hz";
                  out.AddNote() << "Bandwidth: " << xmtrPtr->GetBandwidth() << " Hz";
               }
               else
               {
                  out.AddNote() << "Frequency: " << aFrequency << " Hz";
                  out.AddNote() << "Bandwidth: " << aBandwidth << " Hz";
               }

               if (beamPtr->mBeamNumber > 0)
               {
                  out.AddNote() << "Beam Number:" << beamPtr->mBeamNumber;
               }
            }

            techniqueSelected |= selectedTechnique;
         }
      }
   }
   return techniqueSelected;
}

// virtual
bool WsfRF_Jammer::DeselectEA_Technique(double      aSimTime,
                                        double      aFrequency,
                                        double      aBandwidth,
                                        size_t      aBeamNumber,
                                        WsfStringId aTechniqueId,
                                        WsfTrackId  aTrackId)
{
   // Set the default return value
   bool techniqueDeselected(false);

   // Get a pointer to the current mode
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];

   // while an entry is found that matches the search criteria select that technique
   // if available
   BeamSpotIndexMap beamSpotIndexMap;
   if (modePtr->GetActiveBeamSpotIndexes(aFrequency, aBandwidth, aBeamNumber, aTrackId, beamSpotIndexMap))
   {
      for (auto& beamSpotIndex : beamSpotIndexMap)
      {
         unsigned int beamIndex = beamSpotIndex.first;

         SpotIndexVec spotIndexVec = beamSpotIndex.second;
         for (unsigned int spotIndex : spotIndexVec)
         {
            // need the beamPtr and xmtrSpot from index
            JammerBeam* beamPtr = modePtr->mActiveBeamPtrs[beamIndex];
            JammerXmtr* xmtrPtr = beamPtr->mActiveSpotPtrs[spotIndex];

            bool         deselectedTechnique(true);
            WsfEW_EA_EP* eaPtr = WsfEW_EA::GetElectronicAttack(*xmtrPtr);
            if (eaPtr != nullptr)
            {
               deselectedTechnique = eaPtr->DeselectTechnique(aTechniqueId);
            }

            auto out = ut::log::debug();
            if (deselectedTechnique && DebugEnabled())
            {
               out << "Deselected EA technique.";
            }
            else if (DebugEnabled())
            {
               out << "Failed deselecting EA technique.";
            }

            if (DebugEnabled())
            {
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Jammer: " << GetName();
               out.AddNote() << "Technique: " << aTechniqueId;
               out.AddNote() << "Owning Platform Id: " << aTrackId.GetOwningPlatformId();
               out.AddNote() << "Local Track Number: " << aTrackId.GetLocalTrackNumber();
               if (deselectedTechnique)
               {
                  out.AddNote() << "Frequency: " << xmtrPtr->GetFrequency() << " Hz";
                  out.AddNote() << "Bandwidth: " << xmtrPtr->GetBandwidth() << " Hz";
               }
               else
               {
                  out.AddNote() << "Frequency: " << aFrequency << " Hz";
                  out.AddNote() << "Bandwidth: " << aBandwidth << " Hz";
               }

               if (beamPtr->mBeamNumber > 0)
               {
                  out.AddNote() << "Beam Number:" << beamPtr->mBeamNumber;
               }
            }

            techniqueDeselected |= deselectedTechnique;
         }
      }
   }
   return techniqueDeselected;
}

// virtual
bool WsfRF_Jammer::SetEA_DeltaGainTechnique(double      aSimTime,
                                            WsfStringId aTechniqueId,
                                            bool        aOnOff,
                                            WsfStringId aEffectId,
                                            double      aJammingDeltaGain_dB,
                                            WsfStringId aSystemTypeId,
                                            WsfStringId aSystemFunctionId)
{
   // Set the default return value
   bool deltaGainTechniqueSet(false);

   JammerMode*  modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
   JammerXmtr*  xmtrPtr = (*modePtr).mXmtr.get();
   WsfEW_EA_EP* eaPtr   = WsfEW_EA::GetElectronicAttack(*xmtrPtr);
   if (eaPtr != nullptr)
   {
      deltaGainTechniqueSet = eaPtr->SetDeltaGainTechnique(aSimTime,
                                                           aTechniqueId,
                                                           aOnOff,
                                                           eaPtr->DebugEnabled(),
                                                           aEffectId,
                                                           UtMath::DB_ToLinear(aJammingDeltaGain_dB),
                                                           aSystemTypeId,
                                                           aSystemFunctionId);
   }

   auto out = ut::log::debug();
   if (deltaGainTechniqueSet && DebugEnabled())
   {
      out << "Set EA jammer power delta gain technique.";
   }
   else if (DebugEnabled())
   {
      out << "Failed to set EA jammer power delta gain technique.";
   }

   if (DebugEnabled())
   {
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Jammer: " << GetName();
      out.AddNote() << "Technique: " << aTechniqueId;
      out.AddNote() << "Effect: " << aEffectId;
      out.AddNote() << "Delta Gain: " << aJammingDeltaGain_dB << " dB";
      out.AddNote() << "System Type: " << aSystemTypeId;
      out.AddNote() << "Function Type: " << aSystemFunctionId;
   }

   return deltaGainTechniqueSet;
}

// virtual
bool WsfRF_Jammer::SetEA_DeltaGainTechnique(double      aSimTime,
                                            WsfStringId aTechniqueId,
                                            bool        aOnOff,
                                            WsfStringId aEffectId,
                                            double      aJammingDeltaGain_dB,
                                            WsfStringId aSystemTypeId,
                                            WsfStringId aSystemFunctionId,
                                            double      aFrequency,
                                            double      aBandwidth,
                                            size_t      aBeamNumber,
                                            WsfTrackId  aTrackId)
{
   // Set the default return value
   bool deltaGainTechniqueSet(false);

   // Get a pointer to the current mode
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];

   // while an entry is found that matches the search criteria select that technique if available
   BeamSpotIndexMap beamSpotIndexMap;
   if (modePtr->GetActiveBeamSpotIndexes(aFrequency, aBandwidth, aBeamNumber, aTrackId, beamSpotIndexMap))
   {
      bool setDeltaGainTechnique(true);
      for (auto& beamSpotIndex : beamSpotIndexMap)
      {
         unsigned int beamIndex = beamSpotIndex.first;

         SpotIndexVec spotIndexVec = beamSpotIndex.second;
         for (unsigned int spotIndex : spotIndexVec)
         {
            // need the beamPtr and xmtrSpot from index
            JammerBeam*  beamPtr = modePtr->mActiveBeamPtrs[beamIndex];
            JammerXmtr*  xmtrPtr = beamPtr->mActiveSpotPtrs[spotIndex];
            WsfEW_EA_EP* eaPtr   = WsfEW_EA::GetElectronicAttack(*xmtrPtr);
            if (eaPtr != nullptr)
            {
               setDeltaGainTechnique &= eaPtr->SetDeltaGainTechnique(aSimTime,
                                                                     aTechniqueId,
                                                                     aOnOff,
                                                                     eaPtr->DebugEnabled(),
                                                                     aEffectId,
                                                                     UtMath::DB_ToLinear(aJammingDeltaGain_dB),
                                                                     aSystemTypeId,
                                                                     aSystemFunctionId);
            }

            if (DebugEnabled())
            {
               auto out = ut::log::debug();
               if (setDeltaGainTechnique)
               {
                  out << "Set EA jammer power delta gain technique.";
               }
               else
               {
                  out << "Failed to set EA jammer power delta gain technique.";
               }

               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Jammer: " << GetName();
               out.AddNote() << "Technique: " << aTechniqueId;
               out.AddNote() << "Effect: " << aEffectId;
               out.AddNote() << "Delta Gain: " << aJammingDeltaGain_dB << " dB ";
               out.AddNote() << "System Type: " << aSystemTypeId;
               out.AddNote() << "Function Type: " << aSystemFunctionId;
               if (setDeltaGainTechnique)
               {
                  size_t     targetIndex = GetTargetIndex(xmtrPtr->GetUniqueId());
                  WsfTrackId trackId     = beamPtr->mSpotTrackIds.at(spotIndex);
                  out.AddNote() << "Owning Platform Id: " << aTrackId.GetOwningPlatformId();
                  out.AddNote() << "Local Track Number: " << aTrackId.GetLocalTrackNumber();
                  out.AddNote() << "Target Index: " << targetIndex;
                  out.AddNote() << "Frequency: " << xmtrPtr->GetFrequency() << " Hz";
                  out.AddNote() << "Bandwidth: " << xmtrPtr->GetBandwidth() << " Hz";
               }
               else if (aTrackId.GetLocalTrackNumber() != 0 || aTrackId.GetOwningPlatformId() != 0)
               {
                  out.AddNote() << "Owning Platform Id: " << aTrackId.GetOwningPlatformId();
                  out.AddNote() << "Local Track Number: " << aTrackId.GetLocalTrackNumber();
               }
               if (beamPtr->mBeamNumber > 0)
               {
                  out.AddNote() << "Beam Number: " << beamPtr->mBeamNumber;
               }
            }

            if (setDeltaGainTechnique && aOnOff)
            {
               setDeltaGainTechnique &=
                  SelectEA_Technique(aSimTime, aFrequency, aBandwidth, aBeamNumber, aTechniqueId, aTrackId);
            }

            deltaGainTechniqueSet |= setDeltaGainTechnique;
         }
      }
   }
   return deltaGainTechniqueSet;
}

// virtual
bool WsfRF_Jammer::WithinFieldOfView(double aSimTime, WsfPlatform* aPlatformPtr)
{
   bool withinFieldOfView(false);

   // Update the location of the platform
   aPlatformPtr->Update(aSimTime);

   WsfEM_Antenna* antennaPtr = GetMasterXmtr().GetAntenna();
   if (antennaPtr != nullptr)
   {
      double trueThisToTgtAz(0.0);
      double trueThisToTgtEl(0.0);
      double apparentThisToTgtLocWCS[3] = {0.0};
      double apparentThisToTgtAz(0.0);
      double apparentThisToTgtEl(0.0);
      double apparentTgtToThisLocWCS[3] = {0.0};

      if (antennaPtr->WithinFieldOfView(aPlatformPtr,
                                        1.0,
                                        trueThisToTgtAz,
                                        trueThisToTgtEl,
                                        apparentThisToTgtLocWCS,
                                        apparentThisToTgtAz,
                                        apparentThisToTgtEl,
                                        apparentTgtToThisLocWCS))
      {
         withinFieldOfView = true;
      }
   }
   return withinFieldOfView;
}

// virtual
bool WsfRF_Jammer::WithinFieldOfView(double aSimTime, WsfTrack* aTrackPtr)
{
   bool withinFieldOfView(false);

   WsfEM_Antenna* antennaPtr = GetMasterXmtr().GetAntenna();
   if (antennaPtr != nullptr)
   {
      // WithinFieldOfView output
      double trueThisToTgtAz(0.0);
      double trueThisToTgtEl(0.0);
      double apparentThisToTgtLocWCS[3] = {0.0};
      double apparentThisToTgtAz(0.0);
      double apparentThisToTgtEl(0.0);
      double apparentTgtToThisLocWCS[3] = {0.0};

      // WithinFieldOfView input
      double tgtLocWCS[3] = {0.0};
      aTrackPtr->GetExtrapolatedLocationWCS(aSimTime, tgtLocWCS);

      if (antennaPtr->WithinFieldOfView(tgtLocWCS,
                                        1.0,
                                        trueThisToTgtAz,
                                        trueThisToTgtEl,
                                        apparentThisToTgtLocWCS,
                                        apparentThisToTgtAz,
                                        apparentThisToTgtEl,
                                        apparentTgtToThisLocWCS))
      {
         withinFieldOfView = true;
      }
   }
   return withinFieldOfView;
}

// static
double WsfRF_Jammer::ComputeJammerPower(double                   aSimTime,
                                        WsfEM_Xmtr*              aJammerXmtrPtr,
                                        const WsfEM_Interaction& aTargetInteraction,
                                        WsfEM_Interaction&       aJammerInteraction,
                                        WsfEW_Effect*            aProcessingEW_EffectPtr /*= 0*/)
{
   WsfEW_Result& ewResult = WsfEW_Result::FindOrCreate(aJammerInteraction);
   ComputeJammerEffect(aSimTime, aJammerXmtrPtr, aTargetInteraction, aJammerInteraction, true, true, aProcessingEW_EffectPtr);

   if ((ewResult.mNoiseJammerPower > 0.0) || (ewResult.mPulseJammerPower > 0.0))
   {
      return ((ewResult.mNoiseJammerPower * ewResult.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain *
               ewResult.mEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor) +
              (ewResult.mPulseJammerPower * ewResult.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *
               ewResult.mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor));
   }
   return (ewResult.mCoherentJammerPower * ewResult.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *
           ewResult.mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor);
}

// static
double WsfRF_Jammer::ComputeJammerPower(double                   aSimTime,
                                        WsfEM_Xmtr*              aJammerXmtrPtr,
                                        const WsfEM_Interaction& aTargetInteraction,
                                        WsfEW_Effect*            aProcessingEW_EffectPtr /*= 0*/)
{
   WsfEM_Interaction jammerInteraction;
   WsfEW_Result&     ewResult = WsfEW_Result::FindOrCreate(jammerInteraction);
   ComputeJammerEffect(aSimTime, aJammerXmtrPtr, aTargetInteraction, jammerInteraction, false, true, aProcessingEW_EffectPtr);

   if ((ewResult.mNoiseJammerPower > 0.0) || (ewResult.mPulseJammerPower > 0.0))
   {
      return ((ewResult.mNoiseJammerPower * ewResult.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain *
               ewResult.mEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor) +
              (ewResult.mPulseJammerPower * ewResult.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *
               ewResult.mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor));
   }
   return (ewResult.mCoherentJammerPower * ewResult.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *
           ewResult.mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor);
}

// static
void WsfRF_Jammer::ComputeJammerPowers(double                   aSimTime,
                                       WsfEM_Xmtr*              aJammerXmtrPtr,
                                       const WsfEM_Interaction& aTargetInteraction,
                                       WsfEM_Interaction&       aJammerInteraction,
                                       double                   aJammerPowers[],
                                       WsfEW_Effect*            aProcessingEW_EffectPtr /*= 0*/)
{
   UtVec3d::Set(aJammerPowers, 0.0);

   ComputeJammerEffect(aSimTime, aJammerXmtrPtr, aTargetInteraction, aJammerInteraction, true, true, aProcessingEW_EffectPtr);

   WsfEW_Result& ewResult = WsfEW_Result::FindOrCreate(aJammerInteraction);

   aJammerPowers[0] = ewResult.mNoiseJammerPower * ewResult.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain *
                      ewResult.mEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor;
   aJammerPowers[1] = ewResult.mPulseJammerPower * ewResult.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *
                      ewResult.mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor;
   aJammerPowers[2] = ewResult.mCoherentJammerPower * ewResult.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *
                      ewResult.mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor;
}

// static
void WsfRF_Jammer::ComputeJammerPowers(double                   aSimTime,
                                       WsfEM_Xmtr*              aJammerXmtrPtr,
                                       const WsfEM_Interaction& aTargetInteraction,
                                       double                   aJammerPowers[],
                                       WsfEW_Effect*            aProcessingEW_EffectPtr /*= 0*/)
{
   UtVec3d::Set(aJammerPowers, 0.0);

   WsfEM_Interaction jammerInteraction;
   WsfEW_Result&     ewResult = WsfEW_Result::FindOrCreate(jammerInteraction);
   ComputeJammerEffect(aSimTime, aJammerXmtrPtr, aTargetInteraction, jammerInteraction, false, true, aProcessingEW_EffectPtr);

   aJammerPowers[0] = ewResult.mNoiseJammerPower * ewResult.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain *
                      ewResult.mEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor;
   aJammerPowers[1] = ewResult.mPulseJammerPower * ewResult.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *
                      ewResult.mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor;
   aJammerPowers[2] = ewResult.mCoherentJammerPower * ewResult.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *
                      ewResult.mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor;
}

// static
void WsfRF_Jammer::ComputeSingleJammerEffect(double             aSimTime,
                                             WsfEM_Xmtr*        aJammerXmtrPtr,
                                             WsfEM_Interaction& aTargetInteraction,
                                             WsfEM_Interaction& aJammerInteraction,
                                             bool               aApplyEW_Effects)
{
   WsfEW_Result* tgtInteractionPtr = WsfEW_Result::Find(aTargetInteraction);
   if (tgtInteractionPtr == nullptr)
   {
      return;
   }
   WsfEW_Result& tgtInteraction = *tgtInteractionPtr;

   // Undo the effects so that the ratio of power the current jammer adds to the total can be calculated
   // in aTargetInteraction.
   if (aApplyEW_Effects && (tgtInteraction.mEW_Effects.mMask > 0))
   {
      double factor;
      if (/*(tgtInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_JAMMER_POWER_EFFECT) &&*/
          (tgtInteraction.mNoiseJammerPower > 0.0))

      {
         factor = tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mBlanking *
                  tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mCancelation *
                  tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mModulationGain *
                  tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain *
                  tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mJtoXGain *
                  // tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mPulseSuppression *
                  tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRadiusFactor *
                  tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor;
         // tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRPJ_Gain;
         if (factor > 0.0)
         {
            tgtInteraction.mNoiseJammerPower /= factor;
         }
         else
         {
            tgtInteraction.mNoiseJammerPower = 0.0;
         }
      }

      if (/*((tgtInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_PULSE_EFFECT) ||
           (tgtInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_JAMMER_POWER_EFFECT)) &&*/
          (tgtInteraction.mPulseJammerPower > 0.0))
      {
         factor = tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mBlanking *
                  tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mCancelation *
                  tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mModulationGain *
                  tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *
                  tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mJtoXGain *
                  tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mPulseSuppression *
                  tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRadiusFactor *
                  tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor *
                  tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain;
         if (factor > 0.0)
         {
            tgtInteraction.mPulseJammerPower /= factor;
         }
         else
         {
            tgtInteraction.mPulseJammerPower = 0.0;
         }
      }

      if (/*((aTargetInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_FALSE_TARGET_EFFECT) ||
           (aTargetInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_PULSE_EFFECT) ||
           (aTargetInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_JAMMER_POWER_EFFECT)) &&*/
          (tgtInteraction.mCoherentJammerPower > 0.0))
      {
         factor = tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mBlanking *
                  tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mCancelation *
                  tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mModulationGain *
                  tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *
                  tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain *
                  tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression *
                  tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRadiusFactor *
                  tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor *
                  tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain;
         if (factor > 0.0)
         {
            tgtInteraction.mCoherentJammerPower /= factor;
         }
         else
         {
            tgtInteraction.mCoherentJammerPower = 0.0;
         }
      }
   }

   ComputeJammerEffect(aSimTime, aJammerXmtrPtr, aTargetInteraction, aJammerInteraction, true, true);

   WsfEW_Result& ewJmrResult = WsfEW_Result::FindOrCreate(aJammerInteraction);

   tgtInteraction.mNoiseJammerPower += ewJmrResult.mNoiseJammerPower;
   tgtInteraction.mPulseJammerPower += ewJmrResult.mPulseJammerPower;
   tgtInteraction.mCoherentJammerPower += ewJmrResult.mCoherentJammerPower;
   tgtInteraction.mEW_Effects.mMask |= ewJmrResult.mEW_Effects.mMask;
   tgtInteraction.mEW_Effects.mEA_CoherencyMask |= ewJmrResult.mEW_Effects.mEA_CoherencyMask;

   // Compute the fraction of power this jammer to the total jammer power present
   double noiseFraction = 1.0;
   double pulseFraction = 1.0;
   double cohFraction   = 1.0;
   if (tgtInteraction.mNoiseJammerPower > 0.0)
   {
      noiseFraction = 0.0;
      if (ewJmrResult.mNoiseJammerPower > 0.0)
      {
         noiseFraction = ewJmrResult.mNoiseJammerPower / tgtInteraction.mNoiseJammerPower;
      }
   }
   if (tgtInteraction.mPulseJammerPower > 0.0)
   {
      pulseFraction = 0.0;
      if (ewJmrResult.mPulseJammerPower > 0.0)
      {
         pulseFraction = ewJmrResult.mPulseJammerPower / tgtInteraction.mPulseJammerPower;
      }
   }
   if (tgtInteraction.mCoherentJammerPower > 0.0)
   {
      cohFraction = 0.0;
      if (ewJmrResult.mCoherentJammerPower > 0.0)
      {
         cohFraction = ewJmrResult.mCoherentJammerPower / tgtInteraction.mCoherentJammerPower;
      }
   }

   aTargetInteraction.mInterferencePower = tgtInteraction.mNoiseJammerPower + tgtInteraction.mPulseJammerPower;

   tgtInteraction.mEW_Effects.mEW_Error.mAzimuth =
      std::max(tgtInteraction.mEW_Effects.mEW_Error.mAzimuth, ewJmrResult.mEW_Effects.mEW_Error.mAzimuth);
   tgtInteraction.mEW_Effects.mEW_Error.mElevation =
      std::max(tgtInteraction.mEW_Effects.mEW_Error.mElevation, ewJmrResult.mEW_Effects.mEW_Error.mElevation);
   tgtInteraction.mEW_Effects.mEW_Error.mRange =
      std::max(tgtInteraction.mEW_Effects.mEW_Error.mRange, ewJmrResult.mEW_Effects.mEW_Error.mRange);
   tgtInteraction.mEW_Effects.mEW_Error.mVelocity =
      std::max(tgtInteraction.mEW_Effects.mEW_Error.mVelocity, ewJmrResult.mEW_Effects.mEW_Error.mVelocity);

   if ((tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain > 1.0) &&
       (ewJmrResult.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain > 1.0))
   {
      tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain =
         std::max(tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain,
                  ewJmrResult.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain);
   }
   else if ((tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain < 1.0) &&
            (ewJmrResult.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain < 1.0))
   {
      tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain =
         std::min(tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain,
                  ewJmrResult.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain);
   }
   else
   {
      tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain *= ewJmrResult.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain;
   }

   if ((tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain > 1.0) &&
       (ewJmrResult.mEW_Effects.mEW_SignalEffect.mSignalPowerGain > 1.0))
   {
      tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain =
         std::max(tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain,
                  ewJmrResult.mEW_Effects.mEW_SignalEffect.mSignalPowerGain);
   }
   else if ((tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain < 1.0) &&
            (ewJmrResult.mEW_Effects.mEW_SignalEffect.mSignalPowerGain < 1.0))
   {
      tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain =
         std::min(tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain,
                  ewJmrResult.mEW_Effects.mEW_SignalEffect.mSignalPowerGain);
   }
   else
   {
      tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain *=
         ewJmrResult.mEW_Effects.mEW_SignalEffect.mSignalPowerGain;
   }

   // The assumption here is that there could be some input power in the aTargetInteraction, since there is only a
   // single jammer adding in the (1 - xxxFraction) should do the trick to normalize the EW_Effects ratios for the
   // aTargetInteracion.EW_Efects.
   tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mBlanking =
      tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mBlanking * (1 - noiseFraction) +
      ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mBlanking * noiseFraction;
   tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mCancelation =
      tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mCancelation * (1 - noiseFraction) +
      ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mCancelation * noiseFraction;
   tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mModulationGain =
      tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mModulationGain * (1 - noiseFraction) +
      ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mModulationGain * noiseFraction;
   tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain =
      tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain * (1 - noiseFraction) +
      ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain * noiseFraction;
   tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mJtoXGain =
      tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mJtoXGain * (1 - noiseFraction) +
      ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mJtoXGain * noiseFraction;
   // tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mPulseSuppression =
   //    tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mPulseSuppression * (1 - noiseFraction) +
   //    ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mPulseSuppression * noiseFraction;
   tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRadiusFactor =
      tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRadiusFactor * (1 - noiseFraction) +
      ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mRadiusFactor * noiseFraction;
   tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor =
      tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor * (1 - noiseFraction) +
      ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor * noiseFraction;
   // tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRPJ_Gain =
   //    tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRPJ_Gain * (1 - noiseFraction) +
   //    aJammerInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRPJ_Gain * noiseFraction;

   tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mBlanking =
      tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mBlanking * (1 - pulseFraction) +
      ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mBlanking * pulseFraction;
   tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mCancelation =
      tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mCancelation * (1 - pulseFraction) +
      ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mCancelation * pulseFraction;
   tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mModulationGain =
      tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mModulationGain * (1 - pulseFraction) +
      ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mModulationGain * pulseFraction;
   tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain =
      tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain * (1 - pulseFraction) +
      ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain * pulseFraction;
   tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mJtoXGain =
      tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mJtoXGain * (1 - pulseFraction) +
      ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mJtoXGain * pulseFraction;
   tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mPulseSuppression =
      tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mPulseSuppression * (1 - pulseFraction) +
      ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mPulseSuppression * pulseFraction;
   tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRadiusFactor =
      tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRadiusFactor * (1 - pulseFraction) +
      ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mRadiusFactor * pulseFraction;
   tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor =
      tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor * (1 - pulseFraction) +
      ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor * pulseFraction;
   tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain =
      tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain * (1 - pulseFraction) +
      ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain * pulseFraction;

   tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mBlanking =
      tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mBlanking * (1 - cohFraction) +
      ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mBlanking * cohFraction;
   tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mCancelation =
      tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mCancelation * (1 - cohFraction) +
      ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mCancelation * cohFraction;
   tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mModulationGain =
      tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mModulationGain * (1 - cohFraction) +
      ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mModulationGain * cohFraction;
   tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain =
      tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain * (1 - cohFraction) +
      ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain * cohFraction;
   tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain =
      tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain * (1 - cohFraction) +
      ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain * cohFraction;
   tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression =
      tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression * (1 - cohFraction) +
      ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression * cohFraction;
   tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRadiusFactor =
      tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRadiusFactor * (1 - cohFraction) +
      ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mRadiusFactor * cohFraction;
   tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor =
      tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor * (1 - cohFraction) +
      ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor * cohFraction;
   tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain =
      tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain * (1 - cohFraction) +
      ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain * cohFraction;

   if (aApplyEW_Effects)
   {
      if (ewJmrResult.mEW_Effects.mMask > 0)
      {
         if (/*(ewJmrResult.mEW_Effects.mMask & WsfEW_Effect::cEB_JAMMER_POWER_EFFECT) &&*/
             (ewJmrResult.mNoiseJammerPower > 0.0))
         {
            ewJmrResult.mNoiseJammerPower *= ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mBlanking *
                                             ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mCancelation *
                                             ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mModulationGain *
                                             ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain *
                                             ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mJtoXGain *
                                             // ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mPulseSuppression *
                                             ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mRadiusFactor *
                                             ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor;
            // ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mRPJ_Gain;
         }

         if (/*((ewJmrResult.mEW_Effects.mMask & WsfEW_Effect::cEB_PULSE_EFFECT) ||
            (ewJmrResult.mEW_Effects.mMask & WsfEW_Effect::cEB_JAMMER_POWER_EFFECT)) &&*/
             (ewJmrResult.mPulseJammerPower > 0.0))
         {
            ewJmrResult.mPulseJammerPower *= ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mBlanking *
                                             ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mCancelation *
                                             ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mModulationGain *
                                             ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *
                                             ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mJtoXGain *
                                             ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mPulseSuppression *
                                             ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mRadiusFactor *
                                             ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor *
                                             ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain;
         }

         if (/*((ewJmrResult.mEW_Effects.mMask & WsfEW_Effect::cEB_FALSE_TARGET_EFFECT) ||
            (ewJmrResult.mEW_Effects.mMask & WsfEW_Effect::cEB_PULSE_EFFECT) ||
            (ewJmrResult.mEW_Effects.mMask & WsfEW_Effect::cEB_JAMMER_POWER_EFFECT)) &&*/
             (ewJmrResult.mCoherentJammerPower > 0.0))
         {
            ewJmrResult.mCoherentJammerPower *= ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mBlanking *
                                                ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mCancelation *
                                                ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mModulationGain *
                                                ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *
                                                ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain *
                                                ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression *
                                                ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mRadiusFactor *
                                                ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor *
                                                ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain;
         }

         if (/*(ewJmrResult.mEW_Effects.mMask & WsfEW_Effect::cEB_SIGNAL_POWER_EFFECT) &&*/
             (aJammerInteraction.GetReceiver() != nullptr))
         {
            aJammerInteraction.GetReceiver()->SetNoiseMultiplier(ewJmrResult.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain);
            aJammerInteraction.mRcvdPower *= ewJmrResult.mEW_Effects.mEW_SignalEffect.mSignalPowerGain;
         }
      }

      if (tgtInteraction.mEW_Effects.mMask > 0)
      {
         // Determine if an EW technique is having an effect on received jammer power
         if (/*(tgtInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_JAMMER_POWER_EFFECT) &&*/
             (tgtInteraction.mNoiseJammerPower > 0.0))
         {
            tgtInteraction.mNoiseJammerPower *= tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mBlanking *
                                                tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mCancelation *
                                                tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mModulationGain *
                                                tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain *
                                                tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mJtoXGain *
                                                // tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mPulseSuppression *
                                                tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRadiusFactor *
                                                tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor;
            // tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRPJ_Gain;
         }

         // Determine if an EW technique is having an effect on received pulse jammer power
         if (/*((tgtInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_PULSE_EFFECT) ||
            (tgtInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_JAMMER_POWER_EFFECT)) &&*/
             (tgtInteraction.mPulseJammerPower > 0.0))
         {
            tgtInteraction.mPulseJammerPower *= tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mBlanking *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mCancelation *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mModulationGain *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mJtoXGain *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mPulseSuppression *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRadiusFactor *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain;
         }

         // Determine if an EW technique is having an effect on received coherent jammer power
         if (/*((tgtInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_FALSE_TARGET_EFFECT) ||
            (tgtInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_PULSE_EFFECT) ||
            (tgtInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_JAMMER_POWER_EFFECT)) &&*/
             (tgtInteraction.mCoherentJammerPower > 0.0))
         {
            tgtInteraction.mCoherentJammerPower *=
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mBlanking *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mCancelation *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mModulationGain *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRadiusFactor *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain;
         }

         aTargetInteraction.mInterferencePower = tgtInteraction.mNoiseJammerPower + tgtInteraction.mPulseJammerPower;

         if (/*(tgtInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_SIGNAL_POWER_EFFECT) &&*/
             (aTargetInteraction.GetReceiver() != nullptr))
         {
            aTargetInteraction.GetReceiver()->SetNoiseMultiplier(tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain);
            aTargetInteraction.mRcvdPower *= tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain;
         }
      }
   }
}

// static
void WsfRF_Jammer::ComputeTotalJammerEffects(double aSimTime, WsfEM_Interaction& aTargetInteraction, bool aApplyEW_Effects)
{
   WsfEW_Result* tgtInteractionPtr = WsfEW_Result::Find(aTargetInteraction);
   if (tgtInteractionPtr == nullptr)
   {
      return;
   }
   WsfEW_Result& tgtInteraction = *tgtInteractionPtr;

   tgtInteraction.mNoiseJammerPower      = 0.0;
   tgtInteraction.mPulseJammerPower      = 0.0;
   aTargetInteraction.mInterferencePower = 0.0;
   tgtInteraction.mCoherentJammerPower   = 0.0;
   tgtInteraction.mEW_Effects.Reset();

   std::vector<double>        noisePower;
   std::vector<double>        pulsePower;
   std::vector<double>        cohPower;
   std::vector<WsfEW_Effects> ew_Effects;

   WsfEM_Rcvr* rcvrPtr = aTargetInteraction.GetReceiver();
   size_t      interactorCount(rcvrPtr->GetInterferenceInteractorCount());
   if (interactorCount > 0)
   {
      WsfEM_Interaction jammerInteraction;
      WsfEW_Result&     ewJmrResult = WsfEW_Result::FindOrCreate(jammerInteraction);
      for (size_t xmtrIndex = 0; xmtrIndex < interactorCount; ++xmtrIndex)
      {
         WsfEM_Xmtr* jammerXmtrPtr = rcvrPtr->GetInterferenceInteractorEntry(xmtrIndex);

         ComputeJammerEffect(aSimTime, jammerXmtrPtr, aTargetInteraction, jammerInteraction, true, false);

         tgtInteraction.mNoiseJammerPower += ewJmrResult.mNoiseJammerPower;
         tgtInteraction.mPulseJammerPower += ewJmrResult.mPulseJammerPower;
         tgtInteraction.mCoherentJammerPower += ewJmrResult.mCoherentJammerPower;
         tgtInteraction.mEW_Effects.mMask |= ewJmrResult.mEW_Effects.mMask;
         tgtInteraction.mEW_Effects.mEA_CoherencyMask |= ewJmrResult.mEW_Effects.mEA_CoherencyMask;

         noisePower.push_back(ewJmrResult.mNoiseJammerPower);
         pulsePower.push_back(ewJmrResult.mPulseJammerPower);
         cohPower.push_back(ewJmrResult.mCoherentJammerPower);
         ew_Effects.push_back(ewJmrResult.mEW_Effects);
      }

      auto npi = noisePower.begin();
      auto ppi = pulsePower.begin();
      auto cpi = cohPower.begin();
      auto ewi = ew_Effects.begin();
      if (tgtInteraction.mNoiseJammerPower > 0.0)
      {
         tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.Zeroize();
      }

      if (tgtInteraction.mPulseJammerPower > 0.0)
      {
         tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.Zeroize();
      }

      aTargetInteraction.mInterferencePower = tgtInteraction.mNoiseJammerPower + tgtInteraction.mPulseJammerPower;

      if (tgtInteraction.mCoherentJammerPower > 0.0)
      {
         tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.Zeroize();
      }

      for (; npi != noisePower.end(); ++npi, ++cpi, ++ppi, ++ewi)
      {
         // Compute the fraction of power to the total jammer power present
         if ((tgtInteraction.mNoiseJammerPower > 0.0) && ((*npi) > 0.0))
         {
            double noiseFraction = (*npi) / tgtInteraction.mNoiseJammerPower;

            tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mBlanking +=
               ((*ewi).mEW_NoiseJammingEffect.mBlanking * noiseFraction);
            tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mCancelation +=
               ((*ewi).mEW_NoiseJammingEffect.mCancelation * noiseFraction);
            tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mModulationGain +=
               ((*ewi).mEW_NoiseJammingEffect.mModulationGain * noiseFraction);
            tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain +=
               ((*ewi).mEW_NoiseJammingEffect.mJammingPowerGain * noiseFraction);
            tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mJtoXGain +=
               ((*ewi).mEW_NoiseJammingEffect.mJtoXGain * noiseFraction);
            // tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mPulseSuppression +=
            //    ((*ewi).mEW_NoiseJammingEffect.mPulseSuppression * noiseFraction);
            tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mPulseSuppression = 1.0;
            tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRadiusFactor +=
               ((*ewi).mEW_NoiseJammingEffect.mRadiusFactor * noiseFraction);
            tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor +=
               ((*ewi).mEW_NoiseJammingEffect.mRepeaterFactor * noiseFraction);
            // tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRPJ_Gain +=
            //    ((*ewi).mEW_NoiseJammingEffect.mRPJ_Gain * noiseFraction);
            tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRPJ_Gain = 1.0;
         }
         if ((tgtInteraction.mPulseJammerPower > 0.0) && ((*ppi) > 0.0))
         {
            double pulseFraction = (*ppi) / tgtInteraction.mPulseJammerPower;

            tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mBlanking +=
               ((*ewi).mEW_PulseJammingEffect.mBlanking * pulseFraction);
            tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mCancelation +=
               ((*ewi).mEW_PulseJammingEffect.mCancelation * pulseFraction);
            tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mModulationGain +=
               ((*ewi).mEW_PulseJammingEffect.mModulationGain * pulseFraction);
            tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain +=
               ((*ewi).mEW_PulseJammingEffect.mJammingPowerGain * pulseFraction);
            tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mJtoXGain +=
               ((*ewi).mEW_PulseJammingEffect.mJtoXGain * pulseFraction);
            tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mPulseSuppression +=
               ((*ewi).mEW_PulseJammingEffect.mPulseSuppression * pulseFraction);
            tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRadiusFactor +=
               ((*ewi).mEW_PulseJammingEffect.mRadiusFactor * pulseFraction);
            tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor +=
               ((*ewi).mEW_PulseJammingEffect.mRepeaterFactor * pulseFraction);
            tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain +=
               ((*ewi).mEW_PulseJammingEffect.mRPJ_Gain * pulseFraction);
         }
         if ((tgtInteraction.mCoherentJammerPower > 0.0) && ((*cpi) > 0.0))
         {
            double cohFraction = (*cpi) / tgtInteraction.mCoherentJammerPower;

            tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mBlanking +=
               ((*ewi).mEW_CoherentJammingEffect.mBlanking * cohFraction);
            tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mCancelation +=
               ((*ewi).mEW_CoherentJammingEffect.mCancelation * cohFraction);
            tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mModulationGain +=
               ((*ewi).mEW_CoherentJammingEffect.mModulationGain * cohFraction);
            tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain +=
               ((*ewi).mEW_CoherentJammingEffect.mJammingPowerGain * cohFraction);
            tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain +=
               ((*ewi).mEW_CoherentJammingEffect.mJtoXGain * cohFraction);
            tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression +=
               ((*ewi).mEW_CoherentJammingEffect.mPulseSuppression * cohFraction);
            tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRadiusFactor +=
               ((*ewi).mEW_CoherentJammingEffect.mRadiusFactor * cohFraction);
            tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor +=
               ((*ewi).mEW_CoherentJammingEffect.mRepeaterFactor * cohFraction);
            tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain +=
               ((*ewi).mEW_CoherentJammingEffect.mRPJ_Gain * cohFraction);
         }

         tgtInteraction.mEW_Effects.mEW_Error.mAzimuth =
            std::max(tgtInteraction.mEW_Effects.mEW_Error.mAzimuth, ewi->mEW_Error.mAzimuth);
         tgtInteraction.mEW_Effects.mEW_Error.mElevation =
            std::max(tgtInteraction.mEW_Effects.mEW_Error.mElevation, ewi->mEW_Error.mElevation);
         tgtInteraction.mEW_Effects.mEW_Error.mRange =
            std::max(tgtInteraction.mEW_Effects.mEW_Error.mRange, ewi->mEW_Error.mRange);
         tgtInteraction.mEW_Effects.mEW_Error.mVelocity =
            std::max(tgtInteraction.mEW_Effects.mEW_Error.mVelocity, ewi->mEW_Error.mVelocity);

         if ((tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain > 1.0) &&
             ((*ewi).mEW_SignalEffect.mRcvrNoiseGain > 1.0))
         {
            tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain =
               std::max(tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain, (*ewi).mEW_SignalEffect.mRcvrNoiseGain);
         }
         else if ((tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain < 1.0) &&
                  ((*ewi).mEW_SignalEffect.mRcvrNoiseGain < 1.0))
         {
            tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain =
               std::min(tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain, (*ewi).mEW_SignalEffect.mRcvrNoiseGain);
         }
         else
         {
            tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain *= (*ewi).mEW_SignalEffect.mRcvrNoiseGain;
         }

         if ((tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain > 1.0) &&
             ((*ewi).mEW_SignalEffect.mSignalPowerGain > 1.0))
         {
            tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain =
               std::max(tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain,
                        (*ewi).mEW_SignalEffect.mSignalPowerGain);
         }
         else if ((tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain < 1.0) &&
                  ((*ewi).mEW_SignalEffect.mSignalPowerGain < 1.0))
         {
            tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain =
               std::min(tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain,
                        (*ewi).mEW_SignalEffect.mSignalPowerGain);
         }
         else
         {
            tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain *= (*ewi).mEW_SignalEffect.mSignalPowerGain;
         }
      }

      if (aApplyEW_Effects)
      {
         // Determine if an EW technique is having an effect on received jammer power
         if (/*(tgtInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_JAMMER_POWER_EFFECT) &&*/
             (tgtInteraction.mNoiseJammerPower > 0.0))
         {
            tgtInteraction.mNoiseJammerPower *= tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mBlanking *
                                                tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mCancelation *
                                                tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mModulationGain *
                                                tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain *
                                                tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mJtoXGain *
                                                // tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mPulseSuppression *
                                                tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRadiusFactor *
                                                tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor;
            // tgtInteraction.mEW_Effects.mEW_NoiseJammingEffect.mRPJ_Gain;
         }

         // Determine if an EW technique is having an effect on received pulse jammer power
         if (/*(tgtInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_PULSE_EFFECT) &&*/
             (tgtInteraction.mPulseJammerPower > 0.0))
         {
            tgtInteraction.mPulseJammerPower *= tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mBlanking *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mCancelation *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mModulationGain *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mJtoXGain *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mPulseSuppression *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRadiusFactor *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor *
                                                tgtInteraction.mEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain;
         }

         aTargetInteraction.mInterferencePower = tgtInteraction.mNoiseJammerPower + tgtInteraction.mPulseJammerPower;

         // Determine if an EW technique is having an effect on received coherent jammer power
         if (/*(tgtInteraction.mEW_Effects.mMask & WsfEW_Effect::cEB_FALSE_TARGET_EFFECT) &&*/
             (tgtInteraction.mCoherentJammerPower > 0.0))
         {
            tgtInteraction.mCoherentJammerPower *=
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mBlanking *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mCancelation *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mModulationGain *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRadiusFactor *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor *
               tgtInteraction.mEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain;
         }

         if (/*(ewJmrResult.mEW_Effects.mMask & WsfEW_Effect::cEB_SIGNAL_POWER_EFFECT) &&*/
             (aTargetInteraction.GetReceiver() != nullptr))
         {
            aTargetInteraction.GetReceiver()->SetNoiseMultiplier(tgtInteraction.mEW_Effects.mEW_SignalEffect.mRcvrNoiseGain);
            aTargetInteraction.mRcvdPower *= tgtInteraction.mEW_Effects.mEW_SignalEffect.mSignalPowerGain;
         }
      }
   }
}

void WsfRF_Jammer::ComputeJammerEffect(double                   aSimTime,
                                       WsfEM_Xmtr*              aXmtrPtr,
                                       const WsfEM_Interaction& aTargetInteraction,
                                       WsfEM_Interaction&       aJammerToTgtInteraction,
                                       bool                     aCalculateEW_Effects,
                                       bool                     aIntermediateCalculation,
                                       WsfEW_Effect*            aProcessingEW_EffectPtr /*= 0*/)
{
   // Reset the Jammer Interaction
   aJammerToTgtInteraction.Reset();

   // Get the victim receiver
   WsfEM_Rcvr* rcvrPtr = aTargetInteraction.GetReceiver();

   // Ensure that the transmitter and receiver are not on the same platform and
   // make sure that the transmitter is indeed a jammer.
   //
   // In theory a radar could jam another radar, but we don't allow that.
   if ((aXmtrPtr->GetFunction() != WsfEM_Xmtr::cXF_INTERFERER) || (aXmtrPtr->GetPlatform() == rcvrPtr->GetPlatform()))
   {
      return;
   }

   // Determine if the frequency bandwidth of the receiver falls with the jammer bandwidth
   if (!rcvrPtr->CanInteractWith(aXmtrPtr))
   {
      return;
   }

   // Perform a quick culling check to determine if the receiver is even close to being with the declared transmitter range.
   if (!WsfUtil::PotentiallyWithinRange(aSimTime,
                                        aXmtrPtr->GetPlatform(),
                                        rcvrPtr->GetPlatform(),
                                        aXmtrPtr->GetAntenna()->GetMaximumRange()))
   {
      return;
   }

   WsfRF_Jammer* jammerPtr = static_cast<WsfRF_Jammer*>(aXmtrPtr->GetArticulatedPart());

   std::lock_guard<std::recursive_mutex> lock(jammerPtr->GetMutex());

   jammerPtr->SelectTarget(aSimTime, aXmtrPtr); // set the transient cued location if a target is specified.
   aXmtrPtr->UpdatePosition(aSimTime);          // Ensure that the jammer position is current

   WsfSensorResult repeaterResult;
   WsfEW_Result::FindOrCreate(repeaterResult);
   UpdateRepeaterResult(aSimTime, dynamic_cast<JammerXmtr*>(aXmtrPtr), repeaterResult, aJammerToTgtInteraction, aTargetInteraction);

   if ((aJammerToTgtInteraction.BeginOneWayInteraction(aXmtrPtr, rcvrPtr, true, false) == 0) &&
       (repeaterResult.mFailedStatus == 0))
   {
      WsfEM_Rcvr*                     esmRcvrPtr = repeaterResult.GetReceiver();
      ut::optional<WsfEM_Interaction> tgtToJammerInteraction;

      if (esmRcvrPtr != nullptr)
      {
         tgtToJammerInteraction.emplace();
         tgtToJammerInteraction->BeginOneWayInteraction(aTargetInteraction.GetTransmitter(), esmRcvrPtr, true, false);
         tgtToJammerInteraction->SetTransmitterBeamPosition(aTargetInteraction.mXmtrBeam);
         tgtToJammerInteraction->SetReceiverBeamPosition();
         tgtToJammerInteraction->ComputeRF_OneWayPower();
      }


      // Copy the receiver beam position from the incoming interaction
      aJammerToTgtInteraction.SetReceiverBeamPosition(aTargetInteraction.mRcvrBeam);

      aJammerToTgtInteraction.SetTransmitterBeamPosition();

      // Compute the power from the jammer as seen by the receiver.
      double jammerPower = aJammerToTgtInteraction.ComputeRF_OneWayPower();

      WsfEW_Result& ewJmrResult = WsfEW_Result::FindOrCreate(aJammerToTgtInteraction);

      ewJmrResult.mEW_Effects.Process(aSimTime,
                                      aTargetInteraction,
                                      aJammerToTgtInteraction,
                                      tgtToJammerInteraction,
                                      !aCalculateEW_Effects);

      // Categorize the jamming power according to the effects
      if (((ewJmrResult.mEW_Effects.mEA_CoherencyMask & (WsfEW_Effect::cEC_NONE | WsfEW_Effect::cEC_NONCOHERENT)) != 0U) ||
          (ewJmrResult.mEW_Effects.mEA_CoherencyMask == 0))
      {
         ewJmrResult.mNoiseJammerPower =
            jammerPower * ((ewJmrResult.mEW_Effects.mEW_NoiseJammingEffect.mProtectTarget != 0) ? 1.0 : 0.0);
      }
      if ((ewJmrResult.mEW_Effects.mEA_CoherencyMask & WsfEW_Effect::cEC_NONCOHERENT_PULSE) != 0U)
      {
         ewJmrResult.mPulseJammerPower =
            jammerPower * ((ewJmrResult.mEW_Effects.mEW_PulseJammingEffect.mProtectTarget != 0) ? 1.0 : 0.0);
      }
      if ((ewJmrResult.mEW_Effects.mEA_CoherencyMask & (WsfEW_Effect::cEC_COHERENT | WsfEW_Effect::cEC_COHERENT_PULSE)) != 0U)
      {
         ewJmrResult.mCoherentJammerPower =
            jammerPower * ComputeReceiverGains(rcvrPtr) *
            ((ewJmrResult.mEW_Effects.mEW_CoherentJammingEffect.mProtectTarget != 0) ? 1.0 : 0.0);
      }

      // Check terrain masking only if some effect is present.
      if ((ewJmrResult.mNoiseJammerPower > 0.0) || (ewJmrResult.mPulseJammerPower > 0.0) ||
          (ewJmrResult.mCoherentJammerPower > 0.0) || (ewJmrResult.mEW_Effects.mMask != 0))
      {
         if (aJammerToTgtInteraction.MaskedByTerrain())
         {
            ewJmrResult.mNoiseJammerPower    = 0.0;
            ewJmrResult.mPulseJammerPower    = 0.0;
            ewJmrResult.mCoherentJammerPower = 0.0;
            ewJmrResult.mEW_Effects.Reset();
         }
      }

      aJammerToTgtInteraction.mInterferencePower = ewJmrResult.mNoiseJammerPower + ewJmrResult.mPulseJammerPower;

      if (!aIntermediateCalculation)
      {
         // Send out the results of the interaction
         WsfObserver::JammingAttempt(
            aXmtrPtr->GetPlatform()->GetSimulation())(aSimTime, aXmtrPtr, rcvrPtr, aJammerToTgtInteraction);
      }
   }

   if (!aIntermediateCalculation)
   {
      aXmtrPtr->NotifyListeners(aSimTime, aJammerToTgtInteraction);
   }
   jammerPtr->ClearTarget(aXmtrPtr); // clears the transient cued location if a target is specified.

   if ((rcvrPtr->DebugEnabled() || aXmtrPtr->DebugEnabled()) && (!aIntermediateCalculation))
   {
      auto out = ut::log::debug() << "Jamming Interaction:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << aXmtrPtr->GetPlatform()->GetName();
      out.AddNote() << "Part: " << aXmtrPtr->GetArticulatedPart()->GetName();
      auto note = out.AddNote() << "Attempting to Jam: " << rcvrPtr->GetPlatform()->GetName() << '.'
                                << rcvrPtr->GetArticulatedPart()->GetName();
      if (rcvrPtr->GetMode() != nullptr)
      {
         note.AddNote() << "Mode: " << rcvrPtr->GetMode()->GetName();
      }

      aJammerToTgtInteraction.Print(out);
   }
}

// static
double WsfRF_Jammer::ComputeReceiverGains(WsfEM_Rcvr* aRcvrPtr)
{
   // Adjust coherent jammer power with radar gains if necessary, does not apply signal processor gains
   double                     receiverGains = 1.0;
   WsfRadarSensor::RadarMode* radarModePtr  = dynamic_cast<WsfRadarSensor::RadarMode*>(aRcvrPtr->GetMode());
   if (radarModePtr != nullptr)
   {
      WsfRadarSensor::RadarBeam* radarBeamPtr = radarModePtr->mBeamList[aRcvrPtr->GetIndex()];
      if (radarBeamPtr != nullptr)
      {
         // Account for the gain due to pulse compression, integration and
         // other general post-reception adjustments (for compatibility with SUPPRESSOR OPERATING-LOSSES)..
         receiverGains = radarBeamPtr->mXmtrPtr->GetPulseCompressionRatio() * radarBeamPtr->GetIntegrationGain() *
                         radarBeamPtr->GetAdjustmentFactor();
      }
   }
   return receiverGains;
}

// private
void WsfRF_Jammer::InitializeJammerGroup()
{
   // Check to see if this jammer is in a power group
   if (mJammerGroupNameId != 0)
   {
      // Loop through all the weapon on the same platform
      for (WsfComponentList::RoleIterator<WsfWeapon> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
      {
         // See if this weapon is a jammer
         WsfWeapon* weaponPtr = *iter;
         if (weaponPtr != this)
         {
            WsfRF_Jammer* jammerPtr = dynamic_cast<WsfRF_Jammer*>(weaponPtr);
            if (jammerPtr != nullptr)
            {
               if (jammerPtr->mJammerGroupNameId == mJammerGroupNameId)
               {
                  mJammerGroup.push_back(weaponPtr);
               }
            }
         }
      }
   }
}

unsigned int WsfRF_Jammer::GetActiveGroupCount() const
{
   unsigned int count = 0;
   if (GetEM_XmtrCount() > 0)
   {
      ++count;
   }
   auto groupIter = mJammerGroup.begin();
   for (; groupIter != mJammerGroup.end(); ++groupIter)
   {
      if ((*groupIter)->GetEM_XmtrCount() > 0)
      {
         ++count;
      }
   }
   return count;
}

void WsfRF_Jammer::SensorTrackDropped(double aSimTime, WsfTrackId aTrackId, size_t aTargetIndex)
{
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
   if (modePtr->mUseRepeater)
   {
      modePtr->mRepeater.SensorTrackDropped(aSimTime, aTrackId, aTargetIndex);
   }
}

void WsfRF_Jammer::SensorTrackInitiated(double aSimTime, const WsfTrack* aTrackPtr)
{
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
   if (modePtr->mUseRepeater)
   {
      modePtr->mRepeater.SensorTrackInitiated(aSimTime, aTrackPtr);
   }
}

void WsfRF_Jammer::SensorTrackUpdated(double aSimTime, const WsfTrack* aTrackPtr)
{
   JammerMode* modePtr = mJammerModeList[mModeListPtr->GetCurrentMode()];
   if (modePtr->mUseRepeater)
   {
      modePtr->mRepeater.SensorTrackUpdated(aSimTime, aTrackPtr);
   }
}

WsfRF_Jammer::JammerXmtr::JammerXmtr(WsfEM_Antenna* aAntennaPtr)
   : WsfEM_Xmtr(WsfEM_Xmtr::cXF_INTERFERER, aAntennaPtr)
   , mIgnoredCategories()
   , mIgnoredSides()
   , mIgnoredDomains(0)
   , mIgnoreSameSide(false)
{
   UtVec2d::Set(mFrequencyBand, 0.0);
}

WsfRF_Jammer::JammerXmtr::JammerXmtr(const JammerXmtr& aSrc, WsfEM_Antenna* aAntennaPtr)
   : WsfEM_Xmtr(aSrc, aAntennaPtr)
   , mIgnoredCategories(aSrc.mIgnoredCategories)
   , mIgnoredSides(aSrc.mIgnoredSides)
   , mIgnoredDomains(aSrc.mIgnoredDomains)
   , mIgnoreSameSide(aSrc.mIgnoreSameSide)
{
   UtVec2d::Set(mFrequencyBand, aSrc.mFrequencyBand);
}

// virtual
bool WsfRF_Jammer::JammerXmtr::Initialize(WsfSimulation& aSimulation)
{
   if (mIgnoreSameSide &&
       (std::find(mIgnoredSides.begin(), mIgnoredSides.end(), GetPlatform()->GetSideId()) == mIgnoredSides.end()))
   {
      mIgnoredSides.push_back(GetPlatform()->GetSideId());
   }

   return WsfEM_Xmtr::Initialize(aSimulation);
}

// virtual
bool WsfRF_Jammer::JammerXmtr::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "ignore")
   {
      std::string category;
      aInput.ReadValue(category);
      mIgnoredCategories.JoinCategory(WsfStringId(category));
   }
   else if (command == "ignore_side")
   {
      std::string side;
      aInput.ReadValue(side);
      mIgnoredSides.emplace_back(side);
   }
   else if (command == "ignore_domain")
   {
      std::string domain;
      aInput.ReadValue(domain);
      WsfSpatialDomain domainEnum;
      if (!WsfTypes::StringToEnum(domain, domainEnum))
      {
         throw UtInput::BadValue(aInput);
      }
      mIgnoredDomains |= (1 << domainEnum);
   }
   else if (command == "ignore_same_side")
   {
      mIgnoreSameSide = true;
   }
   else if (command == "frequency_band")
   {
      double lowerFrequency;
      double upperFrequency;
      aInput.ReadValueOfType(lowerFrequency, UtInput::cFREQUENCY);
      aInput.ReadValueOfType(upperFrequency, UtInput::cFREQUENCY);
      aInput.ValueGreaterOrEqual(lowerFrequency, 0.0);
      aInput.ValueGreaterOrEqual(upperFrequency, lowerFrequency);
      UtVec2d::Set(mFrequencyBand, lowerFrequency, upperFrequency);
      SetFrequency(0.0); // force the use of this definition when initialized
   }
   else if ((command == "alternate_frequency") || (command == "frequency_list") || (command == "frequency_channels"))
   {
      throw UtInput::BadValue(aInput, "not currently allowed for WSF_RF_JAMMER transmitters.");
   }
   else
   {
      myCommand = WsfEM_Xmtr::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
bool WsfRF_Jammer::JammerXmtr::AllowInteractionWith(WsfEM_Rcvr* aRcvrPtr)
{
   bool         allowDetectionChances = true;
   WsfPlatform* platformPtr           = aRcvrPtr->GetPlatform();
   if (platformPtr->IsExternalShadow())
   {
      allowDetectionChances = false;
   }
   else if ((mIgnoredDomains & (1 << platformPtr->GetSpatialDomain())) != 0)
   {
      allowDetectionChances = false;
   }
   else if (std::find(mIgnoredSides.begin(), mIgnoredSides.end(), platformPtr->GetSideId()) != mIgnoredSides.end())
   {
      allowDetectionChances = false;
   }
   else if (mIgnoredCategories.Intersects(platformPtr->GetCategories()))
   {
      allowDetectionChances = false;
   }
   return allowDetectionChances;
}

// public
void WsfRF_Jammer::JammerXmtr::GetFrequencyBand(double aFrequencyBand[2]) const
{
   UtVec2d::Set(aFrequencyBand, mFrequencyBand);
}

WsfRF_Jammer::JammerMode::JammerMode(const WsfScenario& aScenario)
   : WsfWeaponMode(aScenario)
   , mAntenna(new WsfEM_Antenna)
   , mXmtr(new JammerXmtr(mAntenna.get()))
   , mBeam(new JammerBeam(0))
   , mActiveBeamPtrs()
   , mAvailableBeamPtrs()
   , mBeamAllocationChanged(false)
   , mMaxNumBeams(1)
   , mBeamPowerDistribution(cAVERAGE_POWER)
   , mMaximumNumSpots(1)
   , mActiveXmtrSpotPtrs()
   , mRepeater()
   , mUseRepeater(false)
{
}

WsfRF_Jammer::JammerMode::JammerMode(const JammerMode& aSrc)
   : WsfWeaponMode(aSrc)
   , mAntenna(new WsfEM_Antenna(*aSrc.mAntenna))
   , mXmtr(new JammerXmtr(*aSrc.mXmtr, mAntenna.get()))
   , mBeam(new JammerBeam(*aSrc.mBeam))
   , mActiveBeamPtrs()
   , mAvailableBeamPtrs()
   , mBeamAllocationChanged(aSrc.mBeamAllocationChanged)
   , mMaxNumBeams(aSrc.mMaxNumBeams)
   , mBeamPowerDistribution(aSrc.mBeamPowerDistribution)
   , mMaximumNumSpots(aSrc.mMaximumNumSpots)
   , mActiveXmtrSpotPtrs()
   , mRepeater(aSrc.mRepeater)
   , mUseRepeater(aSrc.mUseRepeater)
{
}

// virtual
WsfRF_Jammer::JammerMode::~JammerMode()
{
   // Clean up any beam information
   auto beamIter = mActiveBeamPtrs.begin();
   for (; beamIter != mActiveBeamPtrs.end(); ++beamIter)
   {
      delete ((*beamIter).second);
   }

   beamIter = mAvailableBeamPtrs.begin();
   for (; beamIter != mAvailableBeamPtrs.end(); ++beamIter)
   {
      delete ((*beamIter).second);
   }
}

// virtual
WsfMode* WsfRF_Jammer::JammerMode::Clone() const
{
   return new JammerMode(*this);
}

// virtual
bool WsfRF_Jammer::JammerMode::Initialize(double aSimTime, WsfWeapon* aWeaponPtr)
{
   // Set the default return value
   bool ok = true;

   // Initialize the base class
   WsfWeaponMode::Initialize(aSimTime, aWeaponPtr);

   // Pass the debug enabled flag to the master transmitter, iff debug level is 'level two'
   // Note: By setting a 'level one' debug the jamming interactions printouts will be suppressed in certain cases.
   if ((aWeaponPtr->GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG) != 0U)
   {
      mXmtr->SetDebugEnabled(aWeaponPtr->DebugEnabled());
   }

   // Define the mode that owns the master transmitter (this will get copied to the beam transmitters in their
   // Initialize method.
   mXmtr->SetMode(this);

   // Initialize the antenna
   ok &= mAntenna->Initialize(aWeaponPtr);

   // Initialize the repeater
   ok &= mRepeater.Initialize(aSimTime, aWeaponPtr);

   // Check and set the frequency on the master transmitter and initialize it
   // Note: This frequency set is done in the mode so the JammerXmtr::Initialize will put
   //       out a warning message for a invalid frequency when used outside initialization.
   if (mXmtr->GetFrequency() == 0.0)
   {
      // If the receiver frequency was not specified, use the limits of all bands.
      // We need something to prevent an error while initializing the receiver.
      double frequencyBand[2];
      mXmtr->GetFrequencyBand(frequencyBand);
      double minFreq   = frequencyBand[0];
      double maxFreq   = frequencyBand[1];
      double frequency = 0.5 * (minFreq + maxFreq);
      mXmtr->SetFrequency(frequency);

      // set receiver bandwidth to frequency band range
      double bandwidth = maxFreq - minFreq;
      mXmtr->SetBandwidth(bandwidth);
   }
   ok &= mXmtr->Initialize(*aWeaponPtr->GetSimulation());

   // Initialize the master beam
   mBeam->SetBeamNumber(0);
   ok &= mBeam->Initialize(*aWeaponPtr->GetSimulation(), *mXmtr);

   // multi-beam jammer; initialize available beams
   for (unsigned int i = 0; i < mMaxNumBeams; ++i)
   {
      // Copy the data from the master; new-ed memory is free-ed in the mode destructor
      JammerBeam* beamPtr = new JammerBeam(*mBeam, (i + 1));

      // Initialize and save the spot
      ok &= beamPtr->Initialize(*aWeaponPtr->GetSimulation(), *mXmtr);
      mAvailableBeamPtrs[i] = beamPtr;
   }

   return ok;
}

// virtual
bool WsfRF_Jammer::JammerMode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (mAntenna->ProcessInput(aInput) || mXmtr->ProcessInput(aInput) || mXmtr->ProcessInputBlock(aInput))
   {
   }
   else if (mBeam->ProcessInput(aInput))
   {
      mMaximumNumSpots = mMaxNumBeams * mBeam->mMaxNumSpotsPerBeam;
   }
   else if (command == "maximum_number_of_beams")
   {
      aInput.ReadValue(mMaxNumBeams);
      aInput.ValueGreaterOrEqual(mMaxNumBeams, 1U);

      mMaximumNumSpots = mMaxNumBeams * mBeam->mMaxNumSpotsPerBeam;
   }
   else if (command == "maximum_number_of_spots")
   {
      aInput.ReadValue(mMaximumNumSpots);
      aInput.ValueGreaterOrEqual(mMaximumNumSpots, 1U);

      // if there is only 1 beam and maximum_spots_per_beam was entered
      // then set the beam max spots to mMaxSpots for the jammer
      if (mMaxNumBeams == 1)
      {
         mBeam->mMaxNumSpotsPerBeam = mMaximumNumSpots;
      }
   }
   else if (command == "beam_power_distribution")
   {
      std::string powerDistributionStr;
      aInput.ReadValue(powerDistributionStr);
      if (powerDistributionStr == "average")
      {
         mBeamPowerDistribution = cAVERAGE_POWER;
      }
      else if (powerDistributionStr == "constant")
      {
         mBeamPowerDistribution = cCONSTANT_POWER;
      }
   }
   else if (mRepeater.ProcessInput(aInput) || mRepeater.ProcessInputBlock(aInput))
   {
      mUseRepeater = true;
   }
   else
   {
      myCommand = WsfWeaponMode::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
void WsfRF_Jammer::JammerMode::Deselect(double aSimTime)
{
   // Deactivate all beams
   for (auto& activeBeamPtr : mActiveBeamPtrs)
   {
      activeBeamPtr.second->Deselect(aSimTime);
   }
   mRepeater.Reset(aSimTime);
}

// virtual
void WsfRF_Jammer::JammerMode::Select(double aSimTime)
{
   // Activate all the beams
   for (auto& activeBeamPtr : mActiveBeamPtrs)
   {
      activeBeamPtr.second->Select(aSimTime);
   }
}

void WsfRF_Jammer::JammerMode::AdjustPowerDistribution(PowerDistribution aGroupPowerDistribution,
                                                       unsigned int      aActiveGroupCount)
{
   // Calculate the average power from the total
   double dutyCycle(mXmtr->GetDutyCycle());

   if (aGroupPowerDistribution == cAVERAGE_POWER && aActiveGroupCount > 0)
   {
      dutyCycle /= aActiveGroupCount;
   }
   if (mBeamPowerDistribution == cAVERAGE_POWER && !mActiveBeamPtrs.empty())
   {
      dutyCycle /= mActiveBeamPtrs.size();
   }

   // Change values in all active spots
   auto beamIter = mActiveBeamPtrs.begin();
   for (; beamIter != mActiveBeamPtrs.end(); ++beamIter)
   {
      unsigned int spotCount(1);
      JammerBeam*  beamPtr = (*beamIter).second;
      assert(beamPtr != nullptr);
      if (beamPtr->mSpotPowerDistribution == cAVERAGE_POWER)
      {
         spotCount = static_cast<unsigned int>(beamPtr->mActiveSpotPtrs.size());
      }
      for (auto& activeSpotPtr : beamPtr->mActiveSpotPtrs)
      {
         activeSpotPtr->SetDutyCycle(dutyCycle / spotCount);
         if (mBeamAllocationChanged)
         {
            activeSpotPtr->SetAntennaBeamCount(static_cast<unsigned int>(mActiveBeamPtrs.size()));
         }
      }
   }
}

size_t WsfRF_Jammer::JammerMode::GetActiveBeamCount() const
{
   return static_cast<unsigned int>(mActiveBeamPtrs.size());
}

size_t WsfRF_Jammer::JammerMode::GetAvailableBeamCount() const
{
   return static_cast<unsigned int>(mAvailableBeamPtrs.size());
}

WsfRF_Jammer::JammerBeam* WsfRF_Jammer::JammerMode::GetJammerBeam(size_t aBeamNumber, bool& aIsActive) const
{
   JammerBeam* beamPtr(nullptr);
   auto        beamIter = mAvailableBeamPtrs.find(aBeamNumber - 1);
   if (beamIter != mAvailableBeamPtrs.end())
   {
      beamPtr   = (*beamIter).second;
      aIsActive = false;
   }
   else
   {
      beamIter = mActiveBeamPtrs.find(aBeamNumber - 1);
      if (beamIter != mActiveBeamPtrs.end())
      {
         beamPtr   = (*beamIter).second;
         aIsActive = true;
      }
   }
   return beamPtr;
}

bool WsfRF_Jammer::JammerMode::GetActiveBeamSpotIndexes(double            aFrequency,
                                                        double            aBandwidth,
                                                        size_t            aBeamNumber,
                                                        WsfTrackId        aTrackId,
                                                        BeamSpotIndexMap& aBeamSpotIndexMap)
{
   // Define the return variable
   bool found = false;

   // Get the assigned frequency and bandwidth that was assigned after adjustment
   double frequency = aFrequency;
   double bandwidth = aBandwidth;
   AdjustFrequencyBandwidth(frequency, bandwidth);

   SpotIndexVec spotIndexVec;

   aBeamSpotIndexMap.clear();

   WsfTrackId rawTrackId;
   if (mUseRepeater)
   {
      rawTrackId = mRepeater.MergedTrackId(aTrackId);
   }

   // Go through all the active beams and get the active beam indexes with a matching spot
   auto beamIter = mActiveBeamPtrs.begin();
   for (; beamIter != mActiveBeamPtrs.end(); ++beamIter)
   {
      bool        foundSpots(false);
      JammerBeam* beamPtr = beamIter->second;
      if (beamPtr != nullptr)
      {
         if (aBeamNumber == 0 || (beamPtr->GetBeamNumber() == aBeamNumber))
         {
            // See if we can find a spot index on this beam for the search criteria
            foundSpots = beamPtr->GetActiveSpotIndex(frequency, bandwidth, aTrackId, rawTrackId, spotIndexVec);
         }
      }

      if (foundSpots)
      {
         aBeamSpotIndexMap[(beamPtr->GetBeamNumber() - 1)] = spotIndexVec;
      }
      found |= foundSpots;
   }
   return found;
}

WsfRF_Jammer::JammerBeam* WsfRF_Jammer::JammerMode::GetNextAssignableBeam(size_t aBeamNumber, bool& aIsActive) const
{
   JammerBeam* beamPtr(nullptr);
   aIsActive = false;
   // Check for (maximum spot assignments or if a barrage jammer) and a valid beam number range
   if (((GetActiveXmtrSpotCount() < GetMaximumXmtrSpotCount()) || (mMaximumNumSpots == 1)) && (aBeamNumber <= mMaxNumBeams))
   {
      if (aBeamNumber == 0) // this input means to auto select and add beams and spots
      {
         // Get an available beam ...
         unsigned int numActiveSpots(UINT_MAX); //
         for (unsigned int i = 0; i < mMaxNumBeams; ++i)
         {
            JammerBeam* tempBeamPtr = GetJammerBeam(i + 1, aIsActive);
            if (tempBeamPtr == nullptr)
            {
               continue;
            }
            // if beam is not active, get it and break out
            if (!aIsActive)
            {
               beamPtr = tempBeamPtr;
               break;
            }
            // beam is active
            else if (((tempBeamPtr->mActiveSpotPtrs.size() < numActiveSpots) && (!tempBeamPtr->mAvailableSpotPtrs.empty())) ||
                     (mMaximumNumSpots == 1))
            {
               beamPtr        = tempBeamPtr;
               numActiveSpots = static_cast<unsigned int>(beamPtr->mActiveSpotPtrs.size());
            }
         }
      }
      else // a good beam number has been entered
      {
         JammerBeam* tempBeamPtr = GetJammerBeam(aBeamNumber, aIsActive);
         if ((!tempBeamPtr->mAvailableSpotPtrs.empty()) || (mMaximumNumSpots == 1))
         {
            beamPtr = tempBeamPtr;
         }
      }
   }
   return beamPtr;
}

bool WsfRF_Jammer::JammerMode::AdjustFrequencyBandwidth(double& aFrequency, double& aBandwidth)
{
   bool adjusted = false;

   // Difference tolerance
   double DIFF_TOLERANCE(0.01);

   double masterFreqLo = mXmtr->GetFrequency() - 0.5 * mXmtr->GetBandwidth();
   double masterFreqHi = masterFreqLo + mXmtr->GetBandwidth();

   // Allow for wild-carding of frequency and bandwidth for some inputs
   if ((fabs(aFrequency) > DIFF_TOLERANCE) && (fabs(aBandwidth) > DIFF_TOLERANCE))
   {
      if (((masterFreqLo <= aFrequency) && (masterFreqHi >= aFrequency)) && (mMaximumNumSpots != 1))
      {
         double freqLo = aFrequency - 0.5 * aBandwidth;
         double freqHi = freqLo + aBandwidth;

         freqLo = (masterFreqLo > freqLo) ? masterFreqLo : freqLo;
         freqHi = (masterFreqHi < freqHi) ? masterFreqHi : freqHi;

         aBandwidth = freqHi - freqLo;
         aFrequency = freqLo + 0.5 * aBandwidth;

         adjusted = true;
      }
   }
   return adjusted;
}

WsfRF_Jammer::JammerBeam::JammerBeam(unsigned int aBeamNumber)
   : mBeamNumber(aBeamNumber)
   , mActiveSpotPtrs()
   , mAvailableSpotPtrs()
   , mSpotTrackIds()
   , mMaxNumSpotsPerBeam(1)
   , mSpotPowerDistribution(cAVERAGE_POWER)
{
}

WsfRF_Jammer::JammerBeam::JammerBeam(const JammerBeam& aSrc, unsigned int aBeamNumber)
   : mBeamNumber(aBeamNumber)
   , mActiveSpotPtrs()
   , mSpotTrackIds()
   , mMaxNumSpotsPerBeam(aSrc.mMaxNumSpotsPerBeam)
   , mSpotPowerDistribution(aSrc.mSpotPowerDistribution)
{
}

// virtual
WsfRF_Jammer::JammerBeam::~JammerBeam()
{
   // Clean up any spot information
   for (auto& activeSpotPtr : mActiveSpotPtrs)
   {
      delete activeSpotPtr;
   }

   // Clean up any spot information
   for (auto& availableSpotPtr : mAvailableSpotPtrs)
   {
      delete availableSpotPtr;
   }
}

// virtual
bool WsfRF_Jammer::JammerBeam::Initialize(WsfSimulation& aSimulation, JammerXmtr& aXmtr)
{
   // Spot jammer; initialize available xmtrs
   for (unsigned int i = 0; i < mMaxNumSpotsPerBeam; ++i)
   {
      // Copy the data from the master
      JammerXmtr* xmtrPtr = new JammerXmtr(aXmtr, aXmtr.GetAntenna());

      // Identify the xmtr with the mode and beam index.
      xmtrPtr->SetMode(aXmtr.GetMode()); // Propagate the owning mode from the master
      xmtrPtr->SetIndex(mBeamNumber);

      // Initialize and save the spot
      xmtrPtr->Initialize(aSimulation);
      mAvailableSpotPtrs.push_back(xmtrPtr);
   }

   return true;
}

// virtual
bool WsfRF_Jammer::JammerBeam::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "maximum_spots_per_beam")
   {
      aInput.ReadValue(mMaxNumSpotsPerBeam);
      aInput.ValueGreaterOrEqual(mMaxNumSpotsPerBeam, 1U);
   }
   // for backwards compatibility keep power_distribution keyword on spot_power_distribution
   else if (command == "spot_power_distribution" || command == "power_distribution")
   {
      std::string powerDistributionStr;
      aInput.ReadValue(powerDistributionStr);
      if (powerDistributionStr == "average")
      {
         mSpotPowerDistribution = cAVERAGE_POWER;
      }
      else if (powerDistributionStr == "constant")
      {
         mSpotPowerDistribution = cCONSTANT_POWER;
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// virtual
void WsfRF_Jammer::JammerBeam::Deselect(double aSimTime)
{
   // Deactivate all the transmitters
   for (auto& activeSpotPtr : mActiveSpotPtrs)
   {
      activeSpotPtr->Deactivate();
   }
}

// virtual
void WsfRF_Jammer::JammerBeam::Select(double aSimTime)
{
   // Activate all the transmitters
   for (auto& activeSpotPtr : mActiveSpotPtrs)
   {
      activeSpotPtr->Activate();
   }
}

unsigned int WsfRF_Jammer::JammerBeam::GetActiveSpotCount() const
{
   return static_cast<unsigned int>(mActiveSpotPtrs.size());
}

unsigned int WsfRF_Jammer::JammerBeam::GetAvailableSpotCount() const
{
   return static_cast<unsigned int>(mAvailableSpotPtrs.size());
}

bool WsfRF_Jammer::JammerBeam::GetActiveSpotIndex(double        aFrequency,
                                                  double        aBandwidth,
                                                  WsfTrackId    aTrackId,
                                                  WsfTrackId    aRawTrackId,
                                                  SpotIndexVec& aSpotIndexVec) const
{
   // Define the return variable
   bool found = false;

   // Difference tolerance
   double DIFF_TOLERANCE(0.01);

   aSpotIndexVec.clear();

   // loop over all active spots and return the first one found that meets the search criteria supplied
   for (unsigned int spotIndex = 0; spotIndex < mActiveSpotPtrs.size(); ++spotIndex)
   {
      JammerXmtr* xmtrSpotPtr = mActiveSpotPtrs[spotIndex];
      // Find the supplied frequency/bandwidth pair with associated track
      if ((fabs(xmtrSpotPtr->GetFrequency() - aFrequency) < DIFF_TOLERANCE) &&
          (fabs(xmtrSpotPtr->GetBandwidth() - aBandwidth) < DIFF_TOLERANCE) &&
          ((((aTrackId.GetOwningPlatformId() == mSpotTrackIds[spotIndex].GetOwningPlatformId()) &&
             (aTrackId.GetOwningPlatformId() != 0)) &&
            ((aTrackId.GetLocalTrackNumber() == mSpotTrackIds[spotIndex].GetLocalTrackNumber()) &&
             (aTrackId.GetLocalTrackNumber() != 0))) ||
           (((aRawTrackId.GetOwningPlatformId() == mSpotTrackIds[spotIndex].GetOwningPlatformId()) &&
             (aRawTrackId.GetOwningPlatformId() != 0)) &&
            ((aRawTrackId.GetLocalTrackNumber() == mSpotTrackIds[spotIndex].GetLocalTrackNumber()) &&
             (aRawTrackId.GetLocalTrackNumber() != 0)))))
      {
         aSpotIndexVec.push_back(spotIndex);
      }
      // Find the supplied frequency/bandwidth pair
      else if ((fabs(xmtrSpotPtr->GetFrequency() - aFrequency) < DIFF_TOLERANCE) &&
               (fabs(xmtrSpotPtr->GetBandwidth() - aBandwidth) < DIFF_TOLERANCE) &&
               (aTrackId.GetOwningPlatformId() == nullptr) && (aTrackId.GetLocalTrackNumber() == 0))
      {
         aSpotIndexVec.push_back(spotIndex);
      }
      // Find the supplied track
      else if (/*(fabs(aFrequency) < DIFF_TOLERANCE) &&  // aFrequency == 0.0
               (fabs(aBandwidth) < DIFF_TOLERANCE) &&  // aBandwidth == 0.0  comment out for task manager cancels of
               agile assignments */
               ((((aTrackId.GetOwningPlatformId() == mSpotTrackIds[spotIndex].GetOwningPlatformId()) &&
                  (aTrackId.GetOwningPlatformId() != 0)) &&
                 ((aTrackId.GetLocalTrackNumber() == mSpotTrackIds[spotIndex].GetLocalTrackNumber()) &&
                  (aTrackId.GetLocalTrackNumber() != 0))) ||
                (((aRawTrackId.GetOwningPlatformId() == mSpotTrackIds[spotIndex].GetOwningPlatformId()) &&
                  (aRawTrackId.GetOwningPlatformId() != 0)) &&
                 ((aRawTrackId.GetLocalTrackNumber() == mSpotTrackIds[spotIndex].GetLocalTrackNumber()) &&
                  (aRawTrackId.GetLocalTrackNumber() != 0)))))
      {
         aSpotIndexVec.push_back(spotIndex);
      }
      // Get all
      else if ((fabs(aFrequency) < DIFF_TOLERANCE) && // aFrequency == 0.0
               (fabs(aBandwidth) < DIFF_TOLERANCE) && // aBandwidth == 0.0
               ((aTrackId.GetOwningPlatformId() == nullptr) && (aTrackId.GetLocalTrackNumber() == 0)))
      {
         aSpotIndexVec.push_back(spotIndex);
      }
   }

   if (!aSpotIndexVec.empty())
   {
      found = true;
   }
   return found;
}
