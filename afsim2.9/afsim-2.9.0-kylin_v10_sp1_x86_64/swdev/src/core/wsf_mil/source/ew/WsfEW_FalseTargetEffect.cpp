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

#include "WsfEW_FalseTargetEffect.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "UtCast.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EffectManager.hpp"
#include "WsfEW_Types.hpp"
#include "WsfFalseTarget.hpp"
#include "WsfFalseTargetTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

WsfEW_FalseTargetEffect::WsfEW_FalseTargetEffect()
   : WsfEW_PulseEffect()
   , mFT_DataMap()
{
   mCoherencyMask      = cEC_COHERENT_PULSE;
   mBehavior           = cEB_FALSE_TARGET_EFFECT;
   mSystemFunctionMask = cES_SENSOR;
}

WsfEW_FalseTargetEffect::WsfEW_FalseTargetEffect(const WsfEW_FalseTargetEffect& aSrc)
   : WsfEW_PulseEffect(aSrc)
   , mFT_DataMap()
{
}

// virtual
WsfEW_Effect* WsfEW_FalseTargetEffect::Clone() const
{
   return new WsfEW_FalseTargetEffect(*this);
}

// virtual
bool WsfEW_FalseTargetEffect::Initialize(WsfSimulation& aSimulation)
{
   return WsfEW_PulseEffect::Initialize(aSimulation);
}

// virtual
bool WsfEW_FalseTargetEffect::ProcessInput(UtInput& aInput)
{
   return WsfEW_PulseEffect::ProcessInput(aInput);
}

bool WsfEW_FalseTargetEffect::InitializeFT(const WsfEM_Rcvr& aRadarRcvr, const WsfEM_Xmtr& aRadarXmtr)
{
   WsfSensorMode* sensorModePtr = dynamic_cast<WsfSensorMode*>(aRadarRcvr.GetMode());
   assert(sensorModePtr != nullptr);
   WsfSensor* sensorPtr = sensorModePtr->GetSensor();
   assert(sensorPtr != nullptr);
   WsfStringId effectedSystemId = sensorPtr->GetTypeId();
   assert(effectedSystemId != 0);

   WsfFalseTarget* ftPtr = GetFT_Ptr(effectedSystemId);
   if (ftPtr == nullptr)
   {
      return false;
   }

   if (!ftPtr->ExplicitAzimuthExtent())
   {
      ftPtr->SetAzimuthExtent(-UtMath::cPI, UtMath::cPI);
   }

   if ((!ftPtr->ExplicitBlipSpeeds()) && (GetMaxBlipSpeed(effectedSystemId) > 0.0))
   {
      ftPtr->SetBlipSpeeds(GetMinBlipSpeed(effectedSystemId), GetMaxBlipSpeed(effectedSystemId));
   }

   if (GetAutoGenFTs(effectedSystemId))
   {
      ftPtr->SetBearingReference(WsfFalseTarget::cJAMMER);
      ftPtr->SetBlipBehavior(WsfFalseTarget::cRANDOM_SCAN_TO_SCAN);
      ftPtr->SetRadarBehavior(WsfFalseTarget::cRANDOM_TO_MULTIPLE_RADARS);
      ftPtr->SetPlatformBehavior(WsfFalseTarget::cRANDOM_TO_MULTIPLE_PLATFORMS);
   }

   return true;
}

bool WsfEW_FalseTargetEffect::UpdateFT(const WsfEM_Rcvr& aRadarRcvr, const WsfEM_Xmtr& aRadarXmtr)
{
   WsfSensorMode* sensorModePtr = dynamic_cast<WsfSensorMode*>(aRadarRcvr.GetMode());
   assert(sensorModePtr != nullptr);
   WsfSensor* sensorPtr = sensorModePtr->GetSensor();
   assert(sensorPtr != nullptr);
   WsfStringId effectedSystemId = sensorPtr->GetTypeId();
   assert(effectedSystemId != 0);

   WsfFalseTarget* ftPtr = GetFT_Ptr(effectedSystemId);
   assert(ftPtr != nullptr);

   double radarLocLLA[3];
   aRadarRcvr.GetAntenna()->GetLocationLLA(radarLocLLA[0], radarLocLLA[1], radarLocLLA[2]);

   if (!ftPtr->ExplicitReferenceCentroid())
   {
      ftPtr->SetReferenceCentroid(radarLocLLA[0], radarLocLLA[1], 9144.0);
   }

   if ((!ftPtr->ExplicitScanInterval()) && (sensorModePtr->GetFrameTime() > 0.0))
   {
      ftPtr->SetScanInterval(sensorModePtr->GetFrameTime() * GetScanRateMultiplier(effectedSystemId));
   }
   else if (ftPtr->ExplicitScanInterval())
   {
      ftPtr->SetScanInterval(ftPtr->GetScanInterval() * GetScanRateMultiplier(effectedSystemId));
   }

   if (!ftPtr->ExplicitRangeExtent())
   {
      double maxRange(aRadarRcvr.GetAntenna()->GetMaximumRange());
      if (aRadarXmtr.GetPulseRepetitionInterval() > 0.0)
      {
         maxRange = std::min(maxRange, UtMath::cLIGHT_SPEED * aRadarXmtr.GetPulseRepetitionInterval() / 2);
      }
      else
      {
         double maxAlt       = std::max(radarLocLLA[2], 9144.0);
         double earthRadius  = UtSphericalEarth::cEARTH_RADIUS * aRadarXmtr.GetEarthRadiusMultiplier();
         double horizonRange = sqrt((maxAlt * maxAlt) + (2.0 * earthRadius * maxAlt)); // horizon range
         maxRange            = std::min(maxRange, horizonRange);
      }
      ftPtr->SetRangeExtent(0, maxRange);
   }

   if (ExplicitJPD(effectedSystemId))
   {
      // double numPulsesPerPRI = radarXmtrPtr->GetPulseRepetitionInterval() / radarXmtrPtr->GetPulseWidth();
      // double numPRIperScan = aSensor->GetCurrentMode()->GetFrameTime() / radarXmtrPtr->GetPulseRepetitionInterval();
      // unsigned int numFTs = static_cast<unsigned int>(GetJammingPulseDensity() * numPulsesPerPRI * numPRIperScan);
      if (aRadarXmtr.GetPulseWidth() > 0.0)
      {
         double maxNumFTs = sensorPtr->GetCurrentMode()->GetFrameTime() / aRadarXmtr.GetPulseWidth();

         // Get the number of pulses being integrated,
         // This is similar to the WsfRadarSensor::RadarBeam::ComputeIntegratedPulseCount(RadarMode& aMode)
         double                     numPulseIntegrated = 1; // Assuming continuous wave...
         WsfRadarSensor::RadarMode* radarModePtr       = dynamic_cast<WsfRadarSensor::RadarMode*>(sensorModePtr);
         if (radarModePtr != nullptr)
         {
            WsfRadarSensor::RadarBeam* radarBeamPtr = radarModePtr->mBeamList[aRadarRcvr.GetIndex()];
            if ((radarBeamPtr != nullptr) && radarBeamPtr->UsingDetector())
            {
               numPulseIntegrated = (double)(radarBeamPtr->GetNumberOfPulsesIntegrated());
            }
            else if (aRadarXmtr.GetPulseRepetitionFrequency() != 0.0)
            {
               // Pulsed radar...
               double timeOnTarget = 0.0;
               if (radarModePtr->GetDwellTime() == 0.0)
               {
                  if (aRadarXmtr.GetAntenna()->GetScanMode() == WsfEM_Antenna::cSCAN_EL)
                  {
                     double minEl;
                     double maxEl;
                     aRadarXmtr.GetAntenna()->GetElevationScanLimits(minEl, maxEl);
                     timeOnTarget =
                        radarModePtr->GetFrameTime() * aRadarXmtr.GetElevationBeamwidth(0.0, 0.0) / (maxEl - minEl);
                  }
                  else
                  {
                     double minAz;
                     double maxAz;
                     aRadarXmtr.GetAntenna()->GetAzimuthScanLimits(minAz, maxAz);
                     timeOnTarget =
                        radarModePtr->GetFrameTime() * aRadarXmtr.GetAzimuthBeamwidth(0.0, 0.0) / (maxAz - minAz);
                  }
               }
               else
               {
                  // Tracker
                  timeOnTarget = radarModePtr->GetDwellTime();
               }
               numPulseIntegrated = timeOnTarget * aRadarXmtr.GetPulseRepetitionFrequency();
            }
         }
         maxNumFTs /= numPulseIntegrated;

         unsigned int numFTs = static_cast<unsigned int>(GetJammerPulseDensity(effectedSystemId) * maxNumFTs);
         if (numFTs > 0)
         {
            ftPtr->SetInitialNumberOfFTs(numFTs);
         }
      }
      else if (mDebug)
      {
         auto out = ut::log::debug() << "WsfEW_FalseTargetEffect::UpdateFT: Pulsewidth not specified for sensor cannot "
                                        "set number of false-targets.";
         out.AddNote() << "Sensor: " << sensorPtr->GetName();
      }
   }

   if (ExplicitRangeConstraint(effectedSystemId))
   {
      ftPtr->SetRangeConstrained(GetRangeConstrained(effectedSystemId));
   }
   else if (!ftPtr->ExplicitRangeConstraint())
   {
      ftPtr->SetRangeConstrained(sensorModePtr->IsFrequencyAgile());
   }

   return true;
}

// virtual
void WsfEW_FalseTargetEffect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   aEW_Effects.mMask |= cEB_FALSE_TARGET_EFFECT;
   WsfEW_PulseEffect::ApplyUnmitigatedEffect(aEW_Effects);
}

// virtual
void WsfEW_FalseTargetEffect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const
{
   WsfEW_PulseEffect::ApplyMitigatedEffect(aEW_EffectPtr, aEW_Effects);
}

// virtual
void WsfEW_FalseTargetEffect::UpdateJammerPulseDensity(WsfEM_Xmtr* aJammerXmtrPtr, WsfEM_Xmtr* aRadarXmtrPtr)
{
   assert(mEffectedSystemId != 0);

   if (mCurrentTargetId == nullptr)
   {
      return;
   }

   if (!ExplicitJPD(mEffectedSystemId))
   {
      WsfRadarSensor::RadarMode* radarModePtr = dynamic_cast<WsfRadarSensor::RadarMode*>(aRadarXmtrPtr->GetMode());
      if ((radarModePtr != nullptr) && (radarModePtr->GetFrameTime() > 0.0))
      {
         double jammerPulseDensity(GetJammerPulseDensity(mEffectedSystemId));
         if (aRadarXmtrPtr->GetPulseWidth() > 0.0)
         {
            double                     numPulseIntegrated = 1; // Assuming continuous wave...
            WsfRadarSensor::RadarBeam* radarBeamPtr       = radarModePtr->mBeamList[aRadarXmtrPtr->GetIndex()];
            if ((radarBeamPtr != nullptr) && radarBeamPtr->UsingDetector())
            {
               numPulseIntegrated = (double)(radarBeamPtr->GetNumberOfPulsesIntegrated());
            }
            else if (aRadarXmtrPtr->GetPulseRepetitionFrequency() != 0.0)
            {
               // Pulsed radar...
               double timeOnTarget = 0.0;
               if (radarModePtr->GetDwellTime() == 0.0)
               {
                  if (aRadarXmtrPtr->GetAntenna()->GetScanMode() == WsfEM_Antenna::cSCAN_EL)
                  {
                     double minElev;
                     double maxElev;
                     aRadarXmtrPtr->GetAntenna()->GetElevationScanLimits(minElev, maxElev);
                     timeOnTarget = radarModePtr->GetFrameTime() * aRadarXmtrPtr->GetElevationBeamwidth(0.0, 0.0) /
                                    (maxElev - minElev);
                  }
                  else
                  {
                     double minAzimuth;
                     double maxAzimuth;
                     aRadarXmtrPtr->GetAntenna()->GetAzimuthScanLimits(minAzimuth, maxAzimuth);
                     timeOnTarget = radarModePtr->GetFrameTime() * aRadarXmtrPtr->GetAzimuthBeamwidth(0.0, 0.0) /
                                    (maxAzimuth - minAzimuth);
                  }
               }
               else
               {
                  // Tracker
                  timeOnTarget = radarModePtr->GetDwellTime();
               }
               numPulseIntegrated = timeOnTarget * aRadarXmtrPtr->GetPulseRepetitionFrequency();
            }
            jammerPulseDensity =
               std::min(1.0,
                        GetFT_Ptr(mEffectedSystemId)->GetInitialNumberOfFTs() * aRadarXmtrPtr->GetPulseWidth() *
                           numPulseIntegrated / radarModePtr->GetFrameTime());
         }
         else
         {
            auto out = ut::log::warning() << "WSF_FT_EFFECT: Pulsewidth not specified on radar.";
            out.AddNote() << "Radar: " << aRadarXmtrPtr->GetArticulatedPart()->GetName();
         }

         // Need to clone the effect data from the default data if data does not exist when updating the JammerPulseDensity.
         if (!EffectDataExists(mEffectedSystemId))
         {
            InsertNewEffectData(mEffectedSystemId, mEffectDataMap[nullptr]->Clone());
         }
         SetJammerPulseDensity(mEffectedSystemId, jammerPulseDensity, false);
      }
   }
}

// virtual
WsfEW_Effect::EffectData* WsfEW_FalseTargetEffect::PropagateEffectDataPtr(WsfStringId aSystemTypeId)
{
   mFT_DataMap[aSystemTypeId] = dynamic_cast<FT_Data*>(WsfEW_PulseEffect::PropagateEffectDataPtr(aSystemTypeId));
   return mFT_DataMap[aSystemTypeId];
}

// virtual
void WsfEW_FalseTargetEffect::SetJammerPulseDensity(WsfStringId aSystemTypeId,
                                                    double      aJammerPulseDensity,
                                                    bool        aIsExplicit /*= false*/)
{
   WsfEW_PulseEffect::SetJammerPulseDensity(aSystemTypeId, aJammerPulseDensity, aIsExplicit);
   if (aIsExplicit)
   {
      mFT_DataMap[aSystemTypeId]->mExplicitNumberOfFTs = false;
   }
}

WsfFalseTarget* WsfEW_FalseTargetEffect::GetFT_Ptr(WsfStringId aSystemTypeId)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, mEffectDataMap[nullptr]->Clone());
   }
   return mFT_DataMap.find(aSystemTypeId)->second->GetFT_Ptr();
}

double WsfEW_FalseTargetEffect::GetScanRateMultiplier(WsfStringId aSystemTypeId) const
{
   auto edi = mFT_DataMap.find(aSystemTypeId);
   if (edi == mFT_DataMap.end())
   {
      edi = mFT_DataMap.find(nullptr);
   }
   return edi->second->mScanRateMultiplier;
}

bool WsfEW_FalseTargetEffect::GetAutoGenFTs(WsfStringId aSystemTypeId) const
{
   auto edi = mFT_DataMap.find(aSystemTypeId);
   if (edi == mFT_DataMap.end())
   {
      edi = mFT_DataMap.find(nullptr);
   }
   return edi->second->mAutoGenFTs;
}

bool WsfEW_FalseTargetEffect::ExplicitRangeConstraint(WsfStringId aSystemTypeId) const
{
   auto edi = mFT_DataMap.find(aSystemTypeId);
   if (edi == mFT_DataMap.end())
   {
      edi = mFT_DataMap.find(nullptr);
   }
   return edi->second->mExplicitRangeConstraint;
}

bool WsfEW_FalseTargetEffect::GetRangeConstrained(WsfStringId aSystemTypeId) const
{
   auto edi = mFT_DataMap.find(aSystemTypeId);
   if (edi == mFT_DataMap.end())
   {
      edi = mFT_DataMap.find(nullptr);
   }
   return edi->second->mRangeConstrained;
}

double WsfEW_FalseTargetEffect::GetMinBlipSpeed(WsfStringId aSystemTypeId) const
{
   auto edi = mFT_DataMap.find(aSystemTypeId);
   if (edi == mFT_DataMap.end())
   {
      edi = mFT_DataMap.find(nullptr);
   }
   return edi->second->mBlipSpeeds[0];
}

double WsfEW_FalseTargetEffect::GetMaxBlipSpeed(WsfStringId aSystemTypeId) const
{
   auto edi = mFT_DataMap.find(aSystemTypeId);
   if (edi == mFT_DataMap.end())
   {
      edi = mFT_DataMap.find(nullptr);
   }
   return edi->second->mBlipSpeeds[1];
}

unsigned int WsfEW_FalseTargetEffect::GetNumberOfFTs(WsfStringId aSystemTypeId) const
{
   auto edi = mFT_DataMap.find(aSystemTypeId);
   if (edi == mFT_DataMap.end())
   {
      edi = mFT_DataMap.find(nullptr);
   }
   return edi->second->mNumberOfFTs;
}

bool WsfEW_FalseTargetEffect::ExplicitNumberOfFTs(WsfStringId aSystemTypeId) const
{
   auto edi = mFT_DataMap.find(aSystemTypeId);
   if (edi == mFT_DataMap.end())
   {
      edi = mFT_DataMap.find(nullptr);
   }
   return edi->second->mExplicitNumberOfFTs;
}


// Embedded Effect data class
WsfEW_FalseTargetEffect::FT_Data::FT_Data()
   : PulseData()
   , mAutoGenFTs(true)
   , mFT_NameId(nullptr)
   , mNumberOfFTs(1000)
   , mExplicitNumberOfFTs(false)
   , mRangeConstrained(false)
   , mExplicitRangeConstraint(false)
   , mScanRateMultiplier(1.0)
   , mFT_IsCopy(false)
   , mFT_Ptr(nullptr)
{
   UtVec2d::Set(mBlipSpeeds, 0.0);
   SetJammerPulseDensity(0.1, false);
}

WsfEW_FalseTargetEffect::FT_Data::FT_Data(const FT_Data& aSrc)
   : PulseData(aSrc)
   , mAutoGenFTs(aSrc.mAutoGenFTs)
   , mFT_NameId(aSrc.mFT_NameId)
   , mNumberOfFTs(aSrc.mNumberOfFTs)
   , mExplicitNumberOfFTs(aSrc.mExplicitNumberOfFTs)
   , mRangeConstrained(aSrc.mRangeConstrained)
   , mExplicitRangeConstraint(aSrc.mExplicitRangeConstraint)
   , mScanRateMultiplier(aSrc.mScanRateMultiplier)
   , mFT_IsCopy(aSrc.mFT_IsCopy)
   , mFT_Ptr(aSrc.mFT_Ptr) // reset below if it is a copy
{
   UtVec2d::Set(mBlipSpeeds, aSrc.mBlipSpeeds);

   if ((aSrc.mFT_Ptr != nullptr) && aSrc.mFT_IsCopy)
   {
      mFT_Ptr = nullptr;
      SetFT_Ptr(aSrc.mFT_Ptr->Clone(), true);
      mFT_Ptr->Reset();
   }
}

// virtual
WsfEW_Types::EW_EffectData* WsfEW_FalseTargetEffect::FT_Data::Clone() const
{
   return new FT_Data(*this);
}

WsfEW_FalseTargetEffect::FT_Data::~FT_Data()
{
   if (mFT_IsCopy)
   {
      delete mFT_Ptr;
      ;
   }
}

// virtual
bool WsfEW_FalseTargetEffect::FT_Data::ProcessInput(UtInput& aInput)
{
   WsfFalseTarget* ftPtr;

   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "false_target_technique") || (command == "false_target_name"))
   {
      if (mFT_NameId == nullptr)
      {
         std::string name;
         aInput.ReadValue(name);
         WsfScenario::FromInput(aInput).GetDeferredInput().MaybeRequires("false_target", name);
         WsfFalseTarget* ftTypePtr = WsfFalseTargetTypes::Get(WsfScenario::FromInput(aInput)).Find(name);
         if (ftTypePtr == nullptr)
         {
            throw UtInput::BadValue(aInput, "The false-target definition could not be found for " + name + ".");
         }
         mFT_NameId  = name;
         mAutoGenFTs = false;
      }
      else
      {
         throw UtInput::BadValue(aInput, "The false-target name has already been set.");
      }
   }
   else if (WsfFalseTargetTypes::Get(WsfScenario::FromInput(aInput)).LoadInstance(aInput, ftPtr))
   {
      if (ftPtr->GetRadarBehavior() == WsfFalseTarget::cCONSISTENT_TO_MULTIPLE_RADARS)
      {
         throw UtInput::BadValue(aInput,
                                 "Unable to define " + ftPtr->GetName() +
                                    " a consistent to multiple radar type within effect block,");
      }
      SetFT_Ptr(ftPtr, true);
      mAutoGenFTs = false;
   }
   else if (command == "range_constrained")
   {
      bool rangeConstrained;
      aInput.ReadValue(rangeConstrained);
      mRangeConstrained        = rangeConstrained;
      mExplicitRangeConstraint = true;
   }
   else if (command == "scan_rate_multiplier")
   {
      double srm;
      aInput.ReadValue(srm);
      aInput.ValueGreater(srm, 0.0);
      mScanRateMultiplier = srm;
   }
   else if ((command == "speeds") || (command == "blip_speeds") || (command == "track_speeds"))
   {
      double minSpeed;
      double maxSpeed;
      aInput.ReadValueOfType(minSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(minSpeed, 0.0);
      aInput.ReadValueOfType(maxSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(maxSpeed, 0.0);
      aInput.ValueGreaterOrEqual(maxSpeed, minSpeed);
      mBlipSpeeds[0] = minSpeed;
      mBlipSpeeds[1] = maxSpeed;
   }
   else if (command == "number_of_false_targets")
   {
      int numFTs;
      aInput.ReadValue(numFTs);
      aInput.ValueGreater(numFTs, 0);
      mNumberOfFTs         = ut::safe_cast<unsigned int, int>(numFTs);
      mExplicitNumberOfFTs = true;
      SetJammerPulseDensity(GetJammerPulseDensity(), false);
   }
   else if (!PulseData::ProcessInput(aInput))
   {
      myCommand = false;
   }
   return myCommand;
}

// virtual
bool WsfEW_FalseTargetEffect::FT_Data::Initialize(WsfSimulation& aSimulation)
{
   bool ok = true;

   ok &= PulseData::Initialize(aSimulation);

   if ((mFT_Ptr == nullptr) && (mFT_NameId == nullptr))
   {
      mAutoGenFTs = true;
      SetFT_Ptr(WsfFalseTargetTypes::Get(aSimulation.GetScenario()).Clone(std::string("WSF_FALSE_TARGET")), true);
      if (mFT_Ptr != nullptr)
      {
         std::ostringstream oss;
         oss << "FT." << GetEffectPtr()->GetEA_Ptr()->GetLinkedXmtrRcvr().GetPlatform()->GetName() << "."
             << GetEffectPtr()->GetEA_Ptr()->GetLinkedXmtrRcvr().GetArticulatedPart()->GetName();
         mFT_Ptr->SetName(oss.str());
      }
   }
   else if ((mFT_Ptr == nullptr) && (mFT_NameId != 0))
   {
      std::string     name      = mFT_NameId;
      WsfFalseTarget* ftTypePtr = WsfFalseTargetTypes::Get(aSimulation.GetScenario()).Find(name);
      if (ftTypePtr != nullptr) // Means that it was not an instance loaded within the effect block if it can be found
                                // in the type definitions.
      {
         if ((ftTypePtr->GetPlatformBehavior() == WsfFalseTarget::cCONSISTENT_TO_MULTIPLE_PLATFORMS) &&
             (ftTypePtr->GetRadarBehavior() == WsfFalseTarget::cCONSISTENT_TO_MULTIPLE_RADARS))
         {
            SetFT_Ptr(ftTypePtr, false);
         }
         else if ((ftTypePtr->GetPlatformBehavior() == WsfFalseTarget::cRANDOM_TO_MULTIPLE_PLATFORMS) &&
                  (ftTypePtr->GetRadarBehavior() == WsfFalseTarget::cCONSISTENT_TO_MULTIPLE_RADARS))
         {
            std::ostringstream   oss;
            WsfEW_EffectManager* effectManager(WsfEW_EffectManager::Find(*GetEffectPtr()->GetSimulation()));
            oss << name << "." << GetEffectPtr()->GetEA_Ptr()->GetLinkedXmtrRcvr().GetPlatform()->GetName();
            ftTypePtr = effectManager->FindFalseTargetType(oss.str());
            if (ftTypePtr == nullptr)
            {
               ftTypePtr = WsfFalseTargetTypes::Get(aSimulation.GetScenario()).Clone(name);
               ftTypePtr->SetName(oss.str());
               effectManager->AddFalseTargetType(ftTypePtr);
            }
            SetFT_Ptr(ftTypePtr, false);
         }
         else
         {
            SetFT_Ptr(ftTypePtr->Clone(), true);

            std::ostringstream oss;
            oss << "FT." << GetEffectPtr()->GetEA_Ptr()->GetLinkedXmtrRcvr().GetPlatform()->GetName() << "."
                << GetEffectPtr()->GetEA_Ptr()->GetLinkedXmtrRcvr().GetArticulatedPart()->GetName();
            mFT_Ptr->SetName(oss.str());
         }
      }
   }

   if (mFT_Ptr != nullptr)
   {
      if (mAutoGenFTs)
      {
         mFT_Ptr->SetDebug(GetEffectPtr()->DebugEnabled());
      }

      if (mExplicitNumberOfFTs)
      {
         mFT_Ptr->SetInitialNumberOfFTs(mNumberOfFTs, true);
      }

      if ((!mFT_Ptr->ExplicitNumberOfFTs()) && (!ExplicitJPD()))
      {
         SetJammerPulseDensity(0.1, true);
      }

      if (!mFT_Ptr->Initialize(GetEffectPtr()->GetSimulation()))
      {
         auto out = ut::log::error() << "Initializing false-target.";
         out.AddNote() << "False Target: " << mFT_NameId;
         ok = false;
      }
   }
   else
   {
      auto out = ut::log::error() << "Initializing false-target effect. False target not found.";
      out.AddNote() << "False Target: " << mFT_NameId;
      ok = false;
   }

   return ok;
}

// virtual
void WsfEW_FalseTargetEffect::FT_Data::Reset()
{
   mAutoGenFTs              = true;
   mNumberOfFTs             = 1000;
   mExplicitNumberOfFTs     = false;
   mRangeConstrained        = false;
   mExplicitRangeConstraint = false;
   mScanRateMultiplier      = 1.0;
   UtVec2d::Set(mBlipSpeeds, 0.0);
   if (mFT_Ptr != nullptr)
   {
      mFT_Ptr->Reset();
      mAutoGenFTs = false;
   }
   PulseData::Reset();
   SetJammerPulseDensity(0.1, false);
}

void WsfEW_FalseTargetEffect::FT_Data::SetFT_Ptr(WsfFalseTarget* aFT_Ptr, bool aFT_IsCopy)
{
   if (mFT_IsCopy && (mFT_Ptr != aFT_Ptr))
   {
      delete mFT_Ptr;
   }
   mFT_IsCopy = aFT_IsCopy;
   mFT_Ptr    = aFT_Ptr;
   mFT_NameId = aFT_Ptr->GetNameId();
}
