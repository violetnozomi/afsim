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

#include "WsfEW_SimpleFT_Effect.hpp"

#include <cassert>
#include <cfloat>
#include <iostream>
#include <string>

#include "UtCast.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "UtSphericalEarth.hpp"
#include "UtStringUtil.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_SLB_Effect.hpp"
#include "WsfEW_SLC_Effect.hpp"
#include "WsfFalseTargetScreener.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

WsfEW_SimpleFT_Effect::WsfEW_SimpleFT_Effect()
   : WsfEW_PulseEffect()
   , mSimpleFT_DataMap()
   , mSensorDataMap()
   , mNumberFalseTargets(0)
   , mMaximumFT_Capacity(0)
   , mFloodedBlocked(false)
   , mForceAllow(false)
{
   mCoherencyMask = cEC_COHERENT_PULSE;

   // Set default data
   delete mEffectDataMap[nullptr];
   mEffectDataMap[nullptr] = NewEffectData();
}


WsfEW_SimpleFT_Effect::WsfEW_SimpleFT_Effect(const WsfEW_SimpleFT_Effect& aSrc)
   : WsfEW_PulseEffect(aSrc)
   , mSimpleFT_DataMap()
   , mSensorDataMap(aSrc.mSensorDataMap)
   , mNumberFalseTargets(aSrc.mNumberFalseTargets)
   , mMaximumFT_Capacity(aSrc.mMaximumFT_Capacity)
   , mFloodedBlocked(aSrc.mFloodedBlocked)
   , mForceAllow(aSrc.mForceAllow)
{
}

// virtual
WsfEW_Effect* WsfEW_SimpleFT_Effect::Clone() const
{
   return new WsfEW_SimpleFT_Effect(*this);
}

bool WsfEW_SimpleFT_Effect::Initialize(WsfSimulation& aSimulation)
{
   return WsfEW_PulseEffect::Initialize(aSimulation);
}

bool WsfEW_SimpleFT_Effect::ProcessInput(UtInput& aInput)
{
   return WsfEW_PulseEffect::ProcessInput(aInput);
}

// virtual
void WsfEW_SimpleFT_Effect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   aEW_Effects.mMask |= cEB_PULSE_EFFECT;
   WsfEW_PulseEffect::ApplyUnmitigatedEffect(aEW_Effects);

   if (mFloodedBlocked)
   {
      aEW_Effects.mMask |= cEB_FLOODED_BLOCKED;
   }

   if (mDebug)
   {
      auto out = ut::log::debug() << "Simple FT Data:";
      out.AddNote() << "Flooded Blocked: " << mFloodedBlocked;
      out.AddNote() << "Number Of FTs: " << mNumberFalseTargets;
      out.AddNote() << "Pulse Density: " << GetJammingPulseDensity();
      out.AddNote() << "Maximum FT Threshold: " << mMaximumFT_Capacity;
   }
}

// virtual
void WsfEW_SimpleFT_Effect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const
{
   // aEW_Effects.mMask |= cEB_PULSE_EFFECT;
   // if (mFloodedBlocked)
   //{
   //    aEW_Effects.mMask &= ~cEB_FLOODED_BLOCKED;
   //    aEW_Effects.mMask |= cEB_FLOODED_MAINTAINED;
   // }
   WsfEW_PulseEffect::ApplyMitigatedEffect(aEW_EffectPtr, aEW_Effects);
}

// virtual
void WsfEW_SimpleFT_Effect::UpdateEffects(double                                 aSimTime,
                                          const WsfEM_Interaction&               aTargetInteraction,
                                          const WsfEM_Interaction&               aJammerToTgtInteraction,
                                          const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                          WsfEW_Effects&                         aEW_Effects,
                                          WsfEW_Effect*                          aEW_EffectPtr /*= 0*/)
{
   WsfEM_Xmtr* jammerXmtrPtr = aJammerToTgtInteraction.GetTransmitter();
   WsfEM_Rcvr* radarRcvrPtr  = dynamic_cast<WsfEM_Rcvr*>(aTargetInteraction.GetReceiver());
   assert(radarRcvrPtr != nullptr);
   WsfEM_Xmtr* radarXmtrPtr = dynamic_cast<WsfEM_Xmtr*>(aTargetInteraction.GetTransmitter());
   assert(radarXmtrPtr != nullptr);

   WsfRadarSensor*            sensorPtr    = dynamic_cast<WsfRadarSensor*>(radarRcvrPtr->GetArticulatedPart());
   WsfRadarSensor::RadarMode* radarModePtr = dynamic_cast<WsfRadarSensor::RadarMode*>(radarRcvrPtr->GetMode());

   WsfEW_PulseEffect::UpdateEffects(aSimTime,
                                    aTargetInteraction,
                                    aJammerToTgtInteraction,
                                    aTgtToJammerInteraction,
                                    aEW_Effects,
                                    aEW_EffectPtr);

   SimpleFT_Data& radarData = dynamic_cast<SimpleFT_Data&>(GetEffectData(mEffectedSystemId));

   // If there is not a target or rcvr is not part of a radar sensor just return at this point
   if ((aTargetInteraction.GetTarget() == nullptr) || (sensorPtr == nullptr) || (radarModePtr == nullptr))
   {
      return;
   }

   WsfStringId sensorId(UtStringUtil::ToString((int)sensorPtr->GetUniqueId()) + ":" + radarModePtr->GetName());

   if (fabs(GetJammingPulseDensity() - 1.0) >= 0.001) // TODO debug, check to see if this is being executed properly
   {
      if ((aEW_Effects.mMask & WsfEW_Effect::cEB_FLOODED_BLOCKED) != 0u)
      {
         if (mDebug)
         {
            ut::log::debug() << "Skipping out of WSF_SIMPLE_FT_EFFECT update. Radar already flooded.";
         }
         return;
      }

      if (radarData.mUpdateOncePerFrame && (mSensorDataMap[sensorId].mLastUpdateTime >= 0.0))
      {
         if (((aSimTime - mSensorDataMap[sensorId].mLastUpdateTime) < radarModePtr->GetFrameTime()) &&
             (mSensorDataMap[sensorId].mNumberFalseTargets >= (unsigned int)radarData.mMaximumFT_Capacity))
         {
            if (mDebug)
            {
               ut::log::debug() << "Skipping out of WSF_SIMPLE_FT_EFFECT update. Radar capacity has already been met.";
            }
            mNumberFalseTargets = mSensorDataMap[sensorId].mNumberFalseTargets;
            mFloodedBlocked     = mSensorDataMap[sensorId].mFloodedBlocked;
            return;
         }
      }
   }

   mNumberFalseTargets = 0;
   mFloodedBlocked     = false;
   if (radarData.mCombineBeamCounts && (radarRcvrPtr->GetIndex() != 0))
   {
      mNumberFalseTargets = mSensorDataMap[sensorId].mNumberFalseTargets;
      mFloodedBlocked     = mSensorDataMap[sensorId].mFloodedBlocked;
   }

   // Set the maximum false target capacity.
   mMaximumFT_Capacity                    = radarData.mMaximumFT_Capacity;
   int                     rangeForceType = WsfFalseTargetScreener::cFORCE_NEITHER;
   double                  forceSectorArc(UtMath::cTWO_PI);
   double                  forceRange(DBL_MAX);
   int                     forceCountPerRangeSector(0);
   double                  forceJtoSDeltaThresh(DBL_MAX);
   bool                    reportsSNR(false);
   WsfFalseTargetScreener* screenerPtr(WsfFalseTargetScreener::Find(*radarModePtr));
   if (screenerPtr != nullptr)
   {
      if (!radarData.mExplicitFT_Capacity)
      {
         mMaximumFT_Capacity = screenerPtr->GetTrackCapacity();
      }
      rangeForceType           = screenerPtr->GetRangeForceType();
      forceSectorArc           = screenerPtr->GetForceSectorArc();
      forceRange               = screenerPtr->GetForceRange();
      forceCountPerRangeSector = screenerPtr->GetForceCountPerRangeSector();
      forceJtoSDeltaThresh     = screenerPtr->GetForceJtoSDeltaThreshold();
      reportsSNR               = screenerPtr->ReportsSNR();
   }

   if (aTargetInteraction.mFailedStatus != 0)
   {
      return;
   }

   // Get the jammer power and EW_Effects between the receiver and jammer transmitter
   WsfSensorResult jammerInteraction;
   double          jammerPower =
      WsfRF_Jammer::ComputeJammerPower(aSimTime, jammerXmtrPtr, aTargetInteraction, jammerInteraction, this);

   WsfEW_Result* ewJammerInteraction = WsfEW_Result::Find(jammerInteraction);
   jammerPower *= ewJammerInteraction->mEW_Effects.mEW_SignalEffect.mSignalPowerGain *
                  // ewJammerInteraction->mEW_Effects.mEW_CoherentJammingEffect.mBlanking *         // applied below
                  // ewJammerInteraction->mEW_Effects.mEW_CoherentJammingEffect.mCancellation *     // applied below
                  // ewJammerInteraction->mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain * // already applied
                  ewJammerInteraction->mEW_Effects.mEW_CoherentJammingEffect.mModulationGain *
                  ewJammerInteraction->mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain *
                  ewJammerInteraction->mEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression *
                  // ewJammerInteraction->mEW_Effects.mEW_CoherentJammingEffect.mRadiusFactor *     // applied below
                  // ewJammerInteraction->mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor *   // already applied
                  // ewJammerInteraction->mEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain *         // N/A
                  GetJammingPowerGain(mEffectedSystemId) * GetJtoSGain(mEffectedSystemId, cEC_COHERENT) *
                  GetModulationGain(mEffectedSystemId) * GetSignalPowerGain(mEffectedSystemId);

   // TODO - not sure if the repeater factor needs to be divided out or not, probably not.
   double normalizedJNR = radarRcvrPtr->ComputeSignalToNoise(jammerPower / jammerInteraction.mRcvrBeam.mGain,
                                                             jammerInteraction.mClutterPower,
                                                             0.0);

   // Check to see if the mainbeam is able to detect and return if not.
   double mainBeamJNR = normalizedJNR * radarRcvrPtr->GetPeakAntennaGain();
   if (mainBeamJNR < radarRcvrPtr->GetDetectionThreshold())
   {
      return;
   }

   // Calculate the minimum and maximum required antenna gain for detection
   double minAntennaGain(radarRcvrPtr->GetDetectionThreshold() / normalizedJNR);
   double maxAntennaGain(DBL_MAX);
   if (reportsSNR)
   {
      minAntennaGain =
         std::max(minAntennaGain, (aTargetInteraction.mRcvrBeam.mGain / forceJtoSDeltaThresh) / normalizedJNR);
      maxAntennaGain = std::min(maxAntennaGain, aTargetInteraction.mRcvrBeam.mGain * forceJtoSDeltaThresh / normalizedJNR);
   }

   // Account for EP technique effects that are applicable
   RcvrPtrDeltaGainVec rcvrDeltaGainVec;
   WsfEW_EA_EP*        epPtr = WsfEW_EP::GetElectronicProtect(*radarRcvrPtr);
   if (radarData.mApplyEP_Effects && (epPtr != nullptr))
   {
      SystemFunction                affectingSystemFunction = ConvertToEW_SystemFunction(jammerXmtrPtr->GetFunction());
      SystemFunction                affectedSystemFunction  = ConvertToEW_SystemFunction(radarRcvrPtr->GetFunction());
      WsfEW_Technique::EffectPtrVec mitigatingEffectList =
         epPtr->GetMitigatingEffects(this, affectingSystemFunction, affectedSystemFunction);
      for (WsfEW_Effect* mitigatingEffectPtr : mitigatingEffectList)
      {
         if (mitigatingEffectPtr != nullptr)
         {
            if (mitigatingEffectPtr == this)
            {
               continue;
            }

            // SLB
            WsfEW_SLB_Effect* slbEffect = dynamic_cast<WsfEW_SLB_Effect*>(mitigatingEffectPtr);
            if (slbEffect != nullptr)
            {
               WsfEM_Rcvr* auxRcvrPtr = slbEffect->GetAuxRcvrPtr();
               if (auxRcvrPtr != nullptr)
               {
                  rcvrDeltaGainVec.emplace_back(auxRcvrPtr, slbEffect->GetBlankingThreshold());
               }
               continue;
            }
            // SLC
            WsfEW_SLC_Effect* slcEffect = dynamic_cast<WsfEW_SLC_Effect*>(mitigatingEffectPtr);
            if (slcEffect != nullptr)
            {
               WsfEM_Rcvr* auxRcvrPtr = slcEffect->GetAuxRcvrPtr();
               if (auxRcvrPtr != nullptr)
               {
                  rcvrDeltaGainVec.emplace_back(auxRcvrPtr, slcEffect->GetCancelationLockRatio());
               }
               continue;
            }
         }
      }
   }

   // Find the degrees of antenna pattern at or above the minimum gain...must account for different slscan types.
   double detectionRatio(1.0);
   double minEl(0.0);
   double maxEl(0.0);
   double minAz(0.0);
   double maxAz(0.0);
   double azBW = radarRcvrPtr->GetAzimuthBeamwidth(0.0, 0.0);
   double elBW = radarRcvrPtr->GetElevationBeamwidth(0.0, 0.0);
   double absMinStepAngle(UtMath::cRAD_PER_DEG * 0.25);
   double absMaxStepAngle(UtMath::cRAD_PER_DEG * 1.0);

   double azStepSize(absMaxStepAngle);
   if ((radarRcvrPtr->GetAntenna()->GetScanMode() & WsfEM_Antenna::cSCAN_AZ) != 0)
   {
      radarRcvrPtr->GetAntenna()->GetAzimuthScanLimits(minAz, maxAz);
      azStepSize = std::max(absMinStepAngle, std::min(absMaxStepAngle, azBW * radarData.mDetectBW_Ratio));
   }

   double elStepSize(absMaxStepAngle);
   if ((radarRcvrPtr->GetAntenna()->GetScanMode() & WsfEM_Antenna::cSCAN_EL) != 0)
   {
      radarRcvrPtr->GetAntenna()->GetElevationScanLimits(minEl, maxEl);
      elStepSize = std::max(absMinStepAngle, std::min(absMaxStepAngle, elBW * radarData.mDetectBW_Ratio));
   }

   minAz += jammerInteraction.mRcvrBeam.mAz;
   maxAz += jammerInteraction.mRcvrBeam.mAz;
   minEl += jammerInteraction.mRcvrBeam.mEl;
   maxEl += jammerInteraction.mRcvrBeam.mEl;

   double pointsPerBW((azBW / azStepSize) + 1);
   bool   revAngles(false); // false is not scanning in elevation only
   double angle1Min(minEl);
   double angle1Max(maxEl);
   double angle1Step(elStepSize);
   double angle2Min(minAz);
   double angle2Max(maxAz);
   double angle2Step(azStepSize);
   if (radarRcvrPtr->GetAntenna()->GetScanMode() == WsfEM_Antenna::cSCAN_EL)
   {
      pointsPerBW = (elBW / elStepSize) + 1;
      angle1Min   = minAz;
      angle1Max   = maxAz;
      angle1Step  = azStepSize;
      angle2Min   = minEl;
      angle2Max   = maxEl;
      angle2Step  = elStepSize;
      revAngles   = true;
   }

   double    count(0);
   double    consecutiveCount(0);
   const int requiredConsecutiveCount(static_cast<int>(radarData.mDetectBW_Ratio * pointsPerBW + 1));
   int       angle1Count    = static_cast<int>(((angle1Max - angle1Min) / angle1Step) + 1.0);
   int       angle2Count    = static_cast<int>(((angle2Max - angle2Min) / angle2Step) + 1.0);
   int       totalScanCount = angle1Count * (angle2Count - (requiredConsecutiveCount - 1));

   // Setup sector counting
   double sectorCount(0);
   double sectorAzMin(0.0);
   double sectorAzMax(UtMath::cTWO_PI);
   if ((UtMath::cTWO_PI - forceSectorArc) > 0.001)
   {
      sectorAzMin = UtMath::NormalizeAngleMinusPi_Pi(aTargetInteraction.mRcvrToTgt.mAz - (forceSectorArc / 2));
      sectorAzMax = UtMath::NormalizeAngleMinusPi_Pi(aTargetInteraction.mRcvrToTgt.mAz + (forceSectorArc / 2));
   }

   double sectorRangeMin = std::max(0.0, aTargetInteraction.mRcvrToTgt.mRange - forceRange);
   double sectorRangeMax(DBL_MAX);
   if ((DBL_MAX - forceRange) > aTargetInteraction.mRcvrToTgt.mRange)
   {
      sectorRangeMax = aTargetInteraction.mRcvrToTgt.mRange + forceRange;
   }

   // int totalCounter(0);
   // int angle1Counter(0);

   double angle1(angle1Min);
   for (int inc1 = 1; inc1 <= angle1Count; ++inc1)
   {
      //++angle1Counter;
      // int angle2Counter(0);
      double angle2 = angle2Min;
      for (int inc2 = 1; inc2 <= angle2Count; ++inc2)
      {
         //++totalCounter;
         //++angle2Counter;
         double az = angle2;
         double el = angle1;
         if (revAngles)
         {
            az = angle1;
            el = angle2;
         }
         angle2 += angle2Step;

         // Normalize the angles
         UtMath::NormalizeAzElAnglesRad(az, el);

         double gain = radarRcvrPtr->GetAntennaGain(radarRcvrPtr->GetPolarization(),
                                                    jammerXmtrPtr->GetFrequency(),
                                                    az,
                                                    el,
                                                    aTargetInteraction.mRcvrBeam.mEBS_Az,
                                                    aTargetInteraction.mRcvrBeam.mEBS_El);

         for (auto& i : rcvrDeltaGainVec)
         {
            double auxGain = i.first->GetAntennaGain(radarRcvrPtr->GetPolarization(),
                                                     jammerXmtrPtr->GetFrequency(),
                                                     az,
                                                     el,
                                                     aTargetInteraction.mRcvrBeam.mEBS_Az,
                                                     aTargetInteraction.mRcvrBeam.mEBS_El);

            if ((auxGain - gain) >= i.second)
            {
               gain = -1.0;
            }
         }

         if ((gain >= minAntennaGain) && (gain <= maxAntennaGain))
         {
            ++consecutiveCount;
            if (consecutiveCount >= requiredConsecutiveCount)
            {
               ++count;

               if ((az >= sectorAzMin) && (az <= sectorAzMax))
               {
                  ++sectorCount;
               }
            }
         }
         else
         {
            consecutiveCount = 0;
         }
      }
      // angle2Counter = 0;
      angle1 += angle1Step;
      consecutiveCount = 0;
   }

   // detectionRatio = count / (totalCounter - ((requiredConsecutiveCount-1) * angle1Counter));
   detectionRatio = count / totalScanCount;

   // Calculate the maximum number of false targets per frame/scan
   double maxNumFTs = radarData.mMaximumNumberFTs;
   if (ExplicitJPD(mEffectedSystemId))
   {
      // double numPulsesPerPRI = radarXmtrPtr->GetPulseRepetitionInterval() / radarXmtrPtr->GetPulseWidth();
      // double numPRIperScan = aSensor->GetCurrentMode()->GetFrameTime() / radarXmtrPtr->GetPulseRepetitionInterval();
      // unsigned int numFTs =
      //    static_cast<unsigned int>(GetJammingPulseDensity() * numPulsesPerPRI * numPRIperScan);
      if (radarXmtrPtr->GetPulseWidth() > 0.0)
      {
         // Get the number of pulses being integrated,
         // This is similar to the WsfEW_RadarSensor::RadarBeam::ComputeIntegratedPulseCount(RadarMode& aMode)
         double                     numPulseIntegrated = 1; // Assuming continuous wave...
         WsfRadarSensor::RadarBeam* radarBeamPtr       = radarModePtr->mBeamList[radarRcvrPtr->GetIndex()];
         if ((radarBeamPtr != nullptr) && radarBeamPtr->UsingDetector())
         {
            numPulseIntegrated = (double)(radarBeamPtr->GetNumberOfPulsesIntegrated());
         }
         else if (radarXmtrPtr->GetPulseRepetitionFrequency() != 0.0)
         {
            // Pulsed radar...
            double timeOnTarget = 0.0;
            if (radarModePtr->GetDwellTime() == 0.0)
            {
               if (radarXmtrPtr->GetAntenna()->GetScanMode() == WsfEM_Antenna::cSCAN_EL)
               {
                  double minElev;
                  double maxElev;
                  radarXmtrPtr->GetAntenna()->GetElevationScanLimits(minElev, maxElev);
                  timeOnTarget =
                     radarModePtr->GetFrameTime() * radarXmtrPtr->GetElevationBeamwidth(0.0, 0.0) / (maxElev - minElev);
               }
               else
               {
                  double minAzimuth;
                  double maxAzimuth;
                  radarXmtrPtr->GetAntenna()->GetAzimuthScanLimits(minAzimuth, maxAzimuth);
                  timeOnTarget = radarModePtr->GetFrameTime() * radarXmtrPtr->GetAzimuthBeamwidth(0.0, 0.0) /
                                 (maxAzimuth - minAzimuth);
               }
            }
            else
            {
               // Tracker
               timeOnTarget = radarModePtr->GetDwellTime();
            }
            numPulseIntegrated = timeOnTarget * radarXmtrPtr->GetPulseRepetitionFrequency();
         }

         maxNumFTs = GetJammingPulseDensity() * sensorPtr->GetCurrentMode()->GetFrameTime() /
                     (numPulseIntegrated * radarXmtrPtr->GetPulseWidth());
      }
      else if (mDebug)
      {
         auto out = ut::log::debug() << "WsfEW_SimpleFT_Effect::UpdateFT: Pulsewidth not specified for sensor. Cannot "
                                        "set number of false-targets.";
         out.AddNote() << "Sensor: " << sensorPtr->GetName();
      }
   }

   // Account for radius effects (and/or agility of radar).
   double radiusRatio(1.0);
   double insideRadiusRatio  = GetRadiusFactor(mEffectedSystemId, WsfEW_RadiusEffect::cJRL_INSIDE);
   double outsideRadiusRatio = GetRadiusFactor(mEffectedSystemId, WsfEW_RadiusEffect::cJRL_OUTSIDE);
   mForceAllow               = false;

   if ((radarModePtr != nullptr) && (insideRadiusRatio > 0.0))
   {
      insideRadiusRatio *= radarModePtr->IsFrequencyAgile() ? 0.0 : 1.0;
   }

   if ((insideRadiusRatio < 1.0) || (outsideRadiusRatio < 1.0))
   {
      double maxRange(jammerInteraction.mRcvrToXmtr.mRange);
      if (radarXmtrPtr->GetPulseRepetitionInterval() > 0.0)
      {
         maxRange = UtMath::cLIGHT_SPEED * radarXmtrPtr->GetPulseRepetitionInterval() / 2;
      }
      else
      {
         double maxAlt      = std::max(jammerInteraction.mRcvrLoc.mAlt, 9144.0);
         double earthRadius = UtSphericalEarth::cEARTH_RADIUS * jammerInteraction.mEarthRadiusScale;
         maxRange           = sqrt((maxAlt * maxAlt) + (2.0 * earthRadius * maxAlt)); // horizon range
         maxRange           = std::min(maxRange, radarRcvrPtr->GetAntenna()->GetMaximumRange());
      }

      double percentMaxRange =
         std::min(1.0, jammerInteraction.mRcvrToXmtr.mRange / maxRange); // percent inside max range from jammer
      radiusRatio = (insideRadiusRatio * percentMaxRange) + (outsideRadiusRatio * (1 - percentMaxRange));

      double minFT_Range(0.0);
      double maxFT_Range = maxRange;
      if (insideRadiusRatio < 0.001)
      {
         minFT_Range = jammerInteraction.mRcvrToXmtr.mRange;
      }

      if (outsideRadiusRatio < 0.001)
      {
         maxFT_Range = jammerInteraction.mRcvrToXmtr.mRange;
      }

      // Calculate sector ratio
      if ((sectorRangeMin > maxFT_Range) || (sectorRangeMax < minFT_Range))
      {
         sectorCount = 0;
      }
      else
      {
         double rangeLo     = std::max(sectorRangeMin, minFT_Range);
         double rangeHi     = std::min(sectorRangeMax, maxFT_Range);
         double inbandRange = std::max(rangeHi - rangeLo, 0.0); // 'max' check only for safety
         sectorCount *= std::min((inbandRange / (maxFT_Range - minFT_Range)), 1.0);
      }

      if ((((rangeForceType & WsfFalseTargetScreener::cFORCE_INSIDE) != 0) &&
           (aTargetInteraction.mRcvrToTgt.mRange <= minFT_Range)) ||
          (((rangeForceType & WsfFalseTargetScreener::cFORCE_OUTSIDE) != 0) &&
           (aTargetInteraction.mRcvrToTgt.mRange >= maxFT_Range)))
      {
         mForceAllow = true;
      }
   }

   // Get inherent number of false targets present in the radar/receiver/processor based on the
   // maximum number of FTs, the detection ratio and the radiusRatio.
   mNumberFalseTargets += static_cast<int>(maxNumFTs * detectionRatio * radiusRatio);

   if ((!mForceAllow) && (forceCountPerRangeSector > 0) && (sectorCount <= forceCountPerRangeSector))
   {
      mForceAllow = true;
   }

   // Check and update flooded block flag.
   mFloodedBlocked = FloodedBlocked(mNumberFalseTargets, mMaximumFT_Capacity, radarData.mUseRandomDraw);

   if (radarData.mUpdateOncePerFrame || radarData.mCombineBeamCounts)
   {
      mSensorDataMap[sensorId].mLastUpdateTime     = aSimTime;
      mSensorDataMap[sensorId].mNumberFalseTargets = mNumberFalseTargets;
      mSensorDataMap[sensorId].mFloodedBlocked     = mFloodedBlocked;
   }

   if (mDebug)
   {
      auto out = ut::log::debug() << "Simple FT Effect Calculation Data:";
      out.AddNote() << "Normalized JNR: " << normalizedJNR;
      out.AddNote() << "Max Num FTs: " << maxNumFTs;
      out.AddNote() << "Pulse Density: " << GetJammingPulseDensity();
      out.AddNote() << "Detection Ratio: " << detectionRatio;
      out.AddNote() << "Radius Ratio: " << radiusRatio;
      out.AddNote() << "Number False Targets: " << mNumberFalseTargets;
      out.AddNote() << "Flooded Blocked: " << mFloodedBlocked;
      out.AddNote() << "Force Allowed: " << mForceAllow;
   }
}

// virtual
WsfEW_Effect::EffectData* WsfEW_SimpleFT_Effect::PropagateEffectDataPtr(WsfStringId aSystemTypeId)
{
   mSimpleFT_DataMap[aSystemTypeId] =
      dynamic_cast<SimpleFT_Data*>(WsfEW_PulseEffect::PropagateEffectDataPtr(aSystemTypeId));
   return mSimpleFT_DataMap[aSystemTypeId];
}

// virtual
void WsfEW_SimpleFT_Effect::UpdateJammerPulseDensity(WsfEM_Xmtr* aJammerXmtrPtr, WsfEM_Xmtr* aRadarXmtrPtr)
{
   assert(mEffectedSystemId != 0);

   if (!ExplicitJPD(mEffectedSystemId))
   {
      WsfRadarSensor::RadarMode* radarModePtr = dynamic_cast<WsfRadarSensor::RadarMode*>(aRadarXmtrPtr->GetMode());

      if ((radarModePtr != nullptr) && (radarModePtr->GetFrameTime() > 0.0))
      {
         double jammerPulseDensity(GetJammerPulseDensity(mEffectedSystemId));
         if (aRadarXmtrPtr->GetPulseWidth() > 0.0)
         {
            // Get the number of pulses being integrated,
            // This is similar to the WsfEW_RadarSensor::RadarBeam::ComputeIntegratedPulseCount(RadarMode& aMode)
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
            jammerPulseDensity = std::min(1.0,
                                          GetMaximumNumberFalseTargets(mEffectedSystemId) * aRadarXmtrPtr->GetPulseWidth() *
                                             numPulseIntegrated / radarModePtr->GetFrameTime());
         }
         else
         {
            auto out = ut::log::warning() << "WSF_SIMPLE_FT_EFFECT: pulsewidth not specified on radar.";
            out.AddNote() << "Radar: " << aRadarXmtrPtr->GetArticulatedPart()->GetName();
         }
         SetJammerPulseDensity(mEffectedSystemId, jammerPulseDensity, false);
      }
   }
}

// private
bool WsfEW_SimpleFT_Effect::FloodedBlocked(unsigned int aNumberFalseTargets, unsigned int aMaximumCapacity, bool aRandomDraw)
{
   if (mForceAllow)
   {
      return false;
   }

   if (aRandomDraw && (aNumberFalseTargets > 0) &&
       (GetRandom().Uniform<double>() > (aMaximumCapacity / aNumberFalseTargets)))
   {
      return true;
   }
   return (aNumberFalseTargets >= aMaximumCapacity);
}

unsigned int WsfEW_SimpleFT_Effect::GetMaximumNumberFalseTargets(WsfStringId aSystemTypeId) const
{
   auto edi = mSimpleFT_DataMap.find(aSystemTypeId);
   if (edi == mSimpleFT_DataMap.end())
   {
      edi = mSimpleFT_DataMap.find(nullptr);
   }
   return edi->second->mMaximumNumberFTs;
}

WsfEW_SimpleFT_Effect::EffectData* WsfEW_SimpleFT_Effect::NewEffectData()
{
   return new SimpleFT_Data();
}


// Embedded Effect data class
WsfEW_SimpleFT_Effect::SimpleFT_Data::SimpleFT_Data()
   : PulseData()
   , mMaximumNumberFTs(1000)
   , mMaximumFT_Capacity(1000)
   , mExplicitFT_Capacity(false)
   , mDetectBW_Ratio(1.0)
   , mApplyEP_Effects(true)
   , mUpdateOncePerFrame(false)
   , mCombineBeamCounts(true)
   , mUseRandomDraw(false)
{
   SetJammerPulseDensity(0.1, false);
}

// virtual
WsfEW_Types::EW_EffectData* WsfEW_SimpleFT_Effect::SimpleFT_Data::Clone() const
{
   return new SimpleFT_Data(*this);
}

// virtual
bool WsfEW_SimpleFT_Effect::SimpleFT_Data::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "maximum_false_target_capacity")
   {
      int ftCapacity;
      aInput.ReadValue(ftCapacity);
      aInput.ValueGreater(ftCapacity, 0);
      mMaximumFT_Capacity  = ut::safe_cast<unsigned int, int>(ftCapacity);
      mExplicitFT_Capacity = true;
   }
   else if (command == "number_of_false_targets")
   {
      int numFTs;
      aInput.ReadValue(numFTs);
      aInput.ValueGreater(numFTs, 0);
      mMaximumNumberFTs = ut::safe_cast<unsigned int, int>(numFTs);
      SetJammerPulseDensity(GetJammerPulseDensity(), false);
   }
   else if (command == "use_random_calculation_draw")
   {
      bool randomDraw;
      aInput.ReadValue(randomDraw);
      mUseRandomDraw = randomDraw;
   }
   else if (command == "apply_electronic_protect_effects")
   {
      aInput.ReadValue(mApplyEP_Effects);
   }
   else if (command == "update_once_per_frame")
   {
      aInput.ReadValue(mUpdateOncePerFrame);
   }
   else if (command == "percent_of_beamwidth_for_detection")
   {
      double detectBW_ratio;
      ;
      aInput.ReadValue(detectBW_ratio);
      aInput.ValueInClosedRange(detectBW_ratio, 0.0, 1.0);
      mDetectBW_Ratio = detectBW_ratio;
   }
   else if (command == "combine_multi_beam_counts")
   {
      aInput.ReadValue(mCombineBeamCounts);
   }
   else
   {
      myCommand = PulseData::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
bool WsfEW_SimpleFT_Effect::SimpleFT_Data::Initialize(WsfSimulation& aSimulation)
{
   return PulseData::Initialize(aSimulation);
}

// virtual
void WsfEW_SimpleFT_Effect::SimpleFT_Data::Reset()
{
   mMaximumNumberFTs    = 1000;
   mMaximumFT_Capacity  = 1000;
   mExplicitFT_Capacity = false;
   mDetectBW_Ratio      = 1.0;
   mApplyEP_Effects     = true;
   mUpdateOncePerFrame  = false;
   mCombineBeamCounts   = true;
   mUseRandomDraw       = false;
   PulseData::Reset();
   SetJammerPulseDensity(0.1, false);
}
