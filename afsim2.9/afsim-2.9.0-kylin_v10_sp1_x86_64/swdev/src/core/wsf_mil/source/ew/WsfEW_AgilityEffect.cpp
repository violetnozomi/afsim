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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfEW_AgilityEffect.hpp"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <string>
#include <vector>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfComm.hpp"
#include "WsfCommComponentHW.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_CommComponent.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfEW_SensorComponent.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

WsfEW_AgilityEffect::WsfEW_AgilityEffect()
   : WsfEW_Effect()
   , mAgilityMap()
   , mAgilityTypeMask(0)
   , mFrequencyChangeScheduled(false)
   , mModeChangeScheduled(false)
{
   mCoherencyMask = cEC_NONCOHERENT;
   mBehavior      = cEB_AGILITY_EFFECT;
}

// virtual
WsfEW_Effect* WsfEW_AgilityEffect::Clone() const
{
   return new WsfEW_AgilityEffect(*this);
}

bool WsfEW_AgilityEffect::Initialize(WsfSimulation& aSimulation)
{
   return WsfEW_Effect::Initialize(aSimulation);
}

bool WsfEW_AgilityEffect::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command(aInput.GetCommand());
   if (command == "agility_type")
   {
      std::string agilityType;
      aInput.ReadValue(agilityType);
      UtInputBlock inputBlock(aInput, "end_agility_type");
      do
      {
         if (agilityType == "frequency_changing")
         {
            mAgilityTypeMask |= cAT_FREQUENCY;
            AgilityDataValues adv;
            AgilityDataVec    advec;
            advec.push_back(adv);
            mAgilityMap[cAT_FREQUENCY] = advec;
         }
         else if (agilityType == "mode_changing")
         {
            mAgilityTypeMask |= cAT_MODE;
            if (command == "mode_name")
            {
               std::string modeName;
               aInput.ReadValue(modeName);
               if (mAgilityMap[cAT_MODE].empty())
               {
                  AgilityDataValues adv;
                  adv.mId = modeName;
                  AgilityDataVec advec;
                  advec.push_back(adv);
                  mAgilityMap[cAT_MODE] = advec;
               }
               else
               {
                  AgilityDataValues adv;
                  adv.mId              = modeName;
                  AgilityDataVec advec = mAgilityMap[cAT_MODE];
                  advec.push_back(adv);
                  mAgilityMap[cAT_MODE] = advec;
               }
            }
         }
         else
         {
            throw UtInput::BadValue(aInput, " 'agility_type' not recognized.");
         }
      } while (inputBlock.ReadCommand(command));
   }
   else
   {
      myCommand = WsfEW_Effect::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
void WsfEW_AgilityEffect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   aEW_Effects.mMask |= cEB_UNDEFINED;
   // WsfEW_Effect::ApplyUnmitigatedEffect(aEW_Effects);

   if (mDebug)
   {
      auto out = ut::log::debug() << "Platform being jammed when attempting to detect target. Agility effect applied.";
      out.AddNote() << "T = " << GetSimulation()->GetSimTime();
      out.AddNote() << "Platform: " << mCurrentRcvrPlatformId;
      out.AddNote() << "Jamming Platform: " << mCurrentXmtrPlatformId;
      out.AddNote() << "Target: " << mCurrentTargetId;
   }
}

// virtual
void WsfEW_AgilityEffect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const
{
   if (mFrequencyChangeScheduled || mModeChangeScheduled)
   {
      aEW_Effects.mMask |= cEB_AGILITY_EFFECT;
   }
   // WsfEW_Effect::ApplyMitigatedEffect(aEW_EffectPtr, aEW_Effects);
}

// virtual
void WsfEW_AgilityEffect::UpdateEffects(double                                 aSimTime,
                                        const WsfEM_Interaction&               aTargetInteraction,
                                        const WsfEM_Interaction&               aJammerToTgtInteraction,
                                        const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                        WsfEW_Effects&                         aEW_Effects,
                                        WsfEW_Effect*                          aEW_EffectPtr /*= 0*/)
{
   WsfEM_Rcvr* rcvrPtr = aTargetInteraction.GetReceiver();
   assert(rcvrPtr != nullptr);
   WsfEM_Xmtr* xmtrPtr = aTargetInteraction.GetTransmitter();
   assert(xmtrPtr != nullptr);

   WsfEW_Effect::UpdateEffects(aSimTime,
                               aTargetInteraction,
                               aJammerToTgtInteraction,
                               aTgtToJammerInteraction,
                               aEW_Effects,
                               aEW_EffectPtr);

   mModeChangeScheduled      = false;
   mFrequencyChangeScheduled = false;

   if (mDebug)
   {
      auto out = ut::log::debug() << "AGILITY_EFFECTS:UpdateEffects:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mCurrentXmtrPlatformId;
      out.AddNote() << "Agiligy Effect: " << GetName();
      out.AddNote() << "Jamming: " << mCurrentRcvrPlatformId;
      out.AddNote() << "Target: " << mCurrentTargetId;
   }

   if (aTargetInteraction.GetTarget() != nullptr)
   {
      WsfSensorMode*         sensorModePtr(nullptr);
      WsfEW_SensorComponent* componentPtr(nullptr);
      if ((rcvrPtr->GetFunction() == WsfEM_Rcvr::cRF_SENSOR) || (rcvrPtr->GetFunction() == WsfEM_Rcvr::cRF_PASSIVE_SENSOR))
      {
         sensorModePtr = dynamic_cast<WsfSensorMode*>(rcvrPtr->GetMode());
         if (sensorModePtr != nullptr)
         {
            WsfSensor* sensorPtr = sensorModePtr->GetSensor();
            componentPtr         = WsfEW_SensorComponent::Find(*sensorPtr);
         }
      }

      if (componentPtr != nullptr && sensorModePtr)
      {
         // If we are not perceiving jamming then just return.
         if (!componentPtr->JammingPerceived(*sensorModePtr))
         {
            return;
         }

         WsfSensor* sensorPtr      = sensorModePtr->GetSensor();
         double     firstPerceived = componentPtr->GetJammingFirstPerceived(*sensorModePtr);
         double     lastPerceived  = componentPtr->GetJammingLastPerceived(*sensorModePtr);

         if ((mAgilityTypeMask & cAT_MODE) != 0)
         {
            bool canAllowModeChange =
               ((mAgilityTypeMask & cAT_FREQUENCY) == 0) ||
               (sensorPtr->GetModeSelectDelay() - (aSimTime - firstPerceived) <= sensorModePtr->GetAltFreqSelectDelay());

            if (canAllowModeChange)
            {
               if (!sensorPtr->IsModeChangeScheduled())
               {
                  AgilityDataValues temp_adv;
                  temp_adv.mId        = sensorPtr->GetCurrentModeName();
                  AgilityDataVec adv  = mAgilityMap[cAT_MODE];
                  auto           advi = std::find(adv.begin(), adv.end(), temp_adv);
                  if (advi != adv.end())
                  {
                     ++advi;
                     if (advi == adv.end())
                     {
                        advi = adv.begin();
                     }
                  }
                  else if (!adv.empty())
                  {
                     advi = adv.begin();
                  }

                  if (advi != adv.end())
                  {
                     if ((sensorPtr->GetCurrentModeName() != advi->mId))
                     {
                        sensorPtr->ScheduleModeChange(firstPerceived, true, advi->mId);
                        mModeChangeScheduled = sensorPtr->IsModeChangeScheduled();

                        if (mDebug && mModeChangeScheduled)
                        {
                           auto out = ut::log::debug() << "Change mode scheduled for beam.";
                           out.AddNote() << "T = " << firstPerceived + sensorPtr->GetModeSelectDelay() << " sec";
                           out.AddNote() << "Beam: " << rcvrPtr->GetIndex() + 1;
                           out.AddNote() << "Previous Mode: " << sensorPtr->GetCurrentModeName();
                           out.AddNote() << "New Mode: " << advi->mId;
                           out.AddNote() << "PerceivedJammingTime: " << firstPerceived << " sec";
                        }
                     }
                  }
               }
               else
               {
                  if (mDebug)
                  {
                     auto out = ut::log::debug() << "Change mode previously scheduled for beam.";
                     out.AddNote() << "T = " << firstPerceived + sensorPtr->GetModeSelectDelay();
                     out.AddNote() << "Beam: " << rcvrPtr->GetIndex() + 1;
                     out.AddNote() << "Previous Mode: " << sensorPtr->GetCurrentModeName();
                     out.AddNote() << "Perceived Jamming Time: " << firstPerceived << " sec";
                  }
                  mModeChangeScheduled = sensorPtr->IsModeChangeScheduled();
               }
            }
         }

         if ((mAgilityTypeMask & cAT_FREQUENCY) != 0)
         {
            if ((!sensorModePtr->IsAltFreqChangeScheduled())/* &&
                (!sensorPtr->IsModeChangeScheduled())*/)
            {
               sensorModePtr->ScheduleAltFreqChange(lastPerceived);
               mFrequencyChangeScheduled = sensorModePtr->IsAltFreqChangeScheduled();

               if (mDebug && mFrequencyChangeScheduled)
               {
                  auto out = ut::log::debug() << "Alternate frequency change scheduled for beam.";
                  out.AddNote() << "T = "
                                << std::max(lastPerceived, sensorModePtr->GetLastAltFreqSelectTime()) +
                                      sensorModePtr->GetAltFreqSelectDelay()
                                << " sec";
                  out.AddNote() << "Beam: " << rcvrPtr->GetIndex() + 1;
                  out.AddNote() << "Frequency Id: " << xmtrPtr->GetCurrentAlternateFrequencyId();
                  out.AddNote() << "Perceived Jamming Time: " << firstPerceived << " sec";
               }
            }
            else if (mDebug)
            {
               auto out = ut::log::debug() << "Alternate frequency change scheduled for beam.";
               out.AddNote() << "T = "
                             << std::max(lastPerceived, sensorModePtr->GetLastAltFreqSelectTime()) +
                                   sensorModePtr->GetAltFreqSelectDelay()
                             << " sec";
               out.AddNote() << "Beam: " << rcvrPtr->GetIndex() + 1;
               out.AddNote() << "Frequency Id: " << xmtrPtr->GetCurrentAlternateFrequencyId();
               out.AddNote() << "Perceived Jamming Time: " << firstPerceived << " sec";
            }
            mFrequencyChangeScheduled = sensorModePtr->IsAltFreqChangeScheduled();
         }
      }
      else if (mDebug)
      {
         auto out = ut::log::debug() << "Unrecognized system type.";
         out.AddNote() << "Type: " << rcvrPtr->GetArticulatedPart()->GetNameId();
         out.AddNote() << "Only 'WSF_RADAR_SENSOR' system type is currently recognized.";
      }
   }
   else if ((rcvrPtr != nullptr) && (rcvrPtr->GetFunction() == WsfEM_Rcvr::cRF_COMM))
   {
      WsfEW_CommComponent* componentPtr(nullptr);
      auto                 commPtr = dynamic_cast<wsf::comm::Comm*>(rcvrPtr->GetArticulatedPart());
      if (commPtr != nullptr)
      {
         componentPtr = WsfEW_CommComponent::Find(*commPtr);
      }
      if (componentPtr != nullptr)
      {
         if (!componentPtr->JammingPerceived())
         {
            return;
         }

         wsf::comm::ComponentHW* commHW_Ptr = wsf::comm::ComponentHW::Find(*commPtr);
         if (commHW_Ptr != nullptr)
         {
            WsfEM_Xmtr& commXmtr       = commHW_Ptr->GetEM_Xmtr(0);
            double      firstPerceived = componentPtr->GetJammingFirstPerceived();
            double      lastPerceived  = componentPtr->GetJammingLastPerceived();

            if ((mAgilityTypeMask & cAT_MODE) != 0)
            {
               if (mDebug)
               {
                  ut::log::debug() << "Agility type 'mode_changing' is not currently valid for 'comm' systems.";
               }
            }

            if ((mAgilityTypeMask & cAT_FREQUENCY) != 0)
            {
               if (!commHW_Ptr->IsAltFreqChangeScheduled())
               {
                  commHW_Ptr->ScheduleAltFreqChange(lastPerceived);
                  mFrequencyChangeScheduled = commHW_Ptr->IsAltFreqChangeScheduled();

                  if (mDebug && mFrequencyChangeScheduled)
                  {
                     auto out = ut::log::debug() << "Alternate frequency change scheduled to change from frequency.";
                     out.AddNote() << "T = "
                                   << std::max(lastPerceived, commHW_Ptr->GetLastAltFreqSelectTime()) +
                                         commHW_Ptr->GetAltFreqSelectDelay()
                                   << " sec";
                     out.AddNote() << "Frequency Id: " << commXmtr.GetCurrentAlternateFrequencyId();
                     out.AddNote() << "Perceived Jamming Time: " << firstPerceived << " sec";
                  }
               }
               else if (mDebug)
               {
                  auto out = ut::log::debug()
                             << "Alternate frequency change previously scheduled to change from frequency id.";
                  out.AddNote() << "T = "
                                << std::max(lastPerceived, commHW_Ptr->GetLastAltFreqSelectTime()) +
                                      commHW_Ptr->GetAltFreqSelectDelay()
                                << " sec";
                  out.AddNote() << "Frequency Id: " << commXmtr.GetCurrentAlternateFrequencyId();
                  out.AddNote() << "Perceived Jamming Time: " << firstPerceived << " sec";
               }
               mFrequencyChangeScheduled = commHW_Ptr->IsAltFreqChangeScheduled();
            }
         }
         else if (mDebug)
         {
            auto out = ut::log::debug() << "Unrecognized communication hardware component type.";
            out.AddNote() << "Part: " << rcvrPtr->GetArticulatedPart()->GetNameId();
         }
      }
      else if (mDebug)
      {
         auto out = ut::log::debug() << "Unrecognized system type.";
         out.AddNote() << "Type: " << rcvrPtr->GetArticulatedPart()->GetNameId();
         out.AddNote() << "Only 'comm' systems is currently recognized.";
      }
   }
}

bool WsfEW_AgilityEffect::AgilityDataValues::operator==(const AgilityDataValues& aRhs) const
{
   bool areEqual(false);
   if ((mId == aRhs.mId) && (mValue == aRhs.mValue))
   {
      areEqual = true;
   }
   return areEqual;
}

bool WsfEW_AgilityEffect::AgilityDataValues::operator!=(const AgilityDataValues& aRhs) const
{
   bool areNotEqual(false);
   if ((mId != aRhs.mId) || (mValue != aRhs.mValue))
   {
      areNotEqual = true;
   }
   return areNotEqual;
}
