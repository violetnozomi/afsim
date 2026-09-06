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

// WsfAcousticSensor is a simple Acoustic sensor implementation.

// Documents referenced in this file:
//
// Ref 1. "The Correction of Measured Noise Spectra for the Effects of
//         Ground Reflection," Engineering Sciences Data Unit #94035.
// Ref 2. "Evaluation of the Attenuation of Sound by a Uniform Atmosphere,"
//        Engineering Sciences Data Unit #78002
// Ref 3. Garinther, Georges R., Kalb, Joel T., Hodge, David C.,
//        Price, G. Richard, "Technical Memorandum 3-85: Proposed Aural
//        Nondetectability Limits for Army Materiel," Mar 1985.
//        www.dtil.mil document AD-A156-704
// Ref 4. "Acoustics - Reference zero for the calibration of audiometric
//        equipment - Part 7: Reference threshold of hearing under
//        free-field and diffuse-field listening conditions," ISO 389-7:2005
// Ref 5. Wright, Charles P., "A Review of Outdoor Environmental Acoustic
//        Background Noise," Boeing Document #D6-38671, 18 May 1991.

#include "WsfAcousticSensor.hpp"

#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>

#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtMemory.hpp"
#include "UtReferenceTracked.hpp"
#include "WsfAcousticSignature.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfEM_Util.hpp"
#include "WsfEnvironment.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorComponent.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfStandardAcousticSignature.hpp"
#include "WsfTerrain.hpp"
#include "WsfUtil.hpp"

// initialize center frequency array
// static
const double WsfAcousticSensor::mCenterFrequency[24] = {50.0,   62.5,   80.0,   100.0,  125.0,  160.0,
                                                        200.0,  250.0,  315.0,  400.0,  500.0,  630.0,
                                                        800.0,  1000.0, 1250.0, 1600.0, 2000.0, 2500.0,
                                                        3150.0, 4000.0, 5000.0, 6300.0, 8000.0, 10000.0};

struct WsfAcousticSensor::SharedData : public UtReferenceCounted
{
   SharedData()
   {
      // define human hearing threshold: Ref 4
      const double aFreq[37] = {20.0,   25.0,   31.5,    40.0,    50.0,    63.0,    80.0,   100.0,  125.0,  160.0,
                                200.0,  250.0,  315.0,   400.0,   500.0,   630.0,   750.0,  800.0,  1000.0, 1250.0,
                                1500.0, 1600.0, 2000.0,  2500.0,  3000.0,  3150.0,  4000.0, 5000.0, 6000.0, 6300.0,
                                8000.0, 9000.0, 10000.0, 11200.0, 12500.0, 14000.0, 16000.0};

      std::vector<double> freqs;
      for (int i = 0; i < 37; ++i)
      {
         freqs.push_back(aFreq[i]);
      }

      const double level[37] = {78.5, 68.7, 59.5, 51.1, 44.0, 37.5, 31.5, 26.5, 22.1, 17.9, 14.4, 11.4, 8.4,
                                5.8,  3.8,  2.1,  1.2,  1.0,  0.8,  1.9,  1.0,  0.5,  -1.5, -3.1, -4.0, -4.0,
                                -3.8, -1.8, 1.4,  2.5,  6.8,  8.4,  9.8,  11.5, 14.4, 23.2, 43.7};

      std::vector<double> sigs;
      for (int i = 0; i < 37; ++i)
      {
         double aVal = pow(10.0, level[i] * 0.1);
         sigs.push_back(aVal);
      }

      // create & initialize sig table for human hearing threshold
      mHumanHearingThreshold.AddStateEntry("default", freqs, sigs);

      mHumanHearingThreshold.InitializeType();

      // create table with various states that will store background
      // noise data; each sensor mode contains a integer that points
      // to the appropriate state
      // data taken from Ref 5
      const double backgroundFreq[16] =
         {100.0, 125.0, 200.0, 250.0, 300.0, 400.0, 500.0, 600.0, 700.0, 800.0, 900.0, 1000.0, 2000.0, 3000.0, 4000.0, 5000.0};

      freqs.clear();

      for (int i = 0; i < 16; ++i)
      {
         freqs.push_back(backgroundFreq[i]);
      }

      // jungle daytime background noise (dB-20uPa)
      const double jungleDay[16] = {22.0, 21.5, 19.0, 17.5, 16.0, 13.5, 13.0, 13.0, 12.8, 12.5, 11.5, 11.0, 7.8, 8.0, 11.0, 16.0};

      sigs.clear();

      for (int i = 0; i < 16; ++i)
      {
         double val = pow(10.0, jungleDay[i] * 0.1);
         sigs.push_back(val);
      }

      WsfStringId jungleDayId = "jungle_day";
      mBackgroundNoise.AddStateEntry(jungleDayId, freqs, sigs);

      // jungle night background noise (dB-20uPa)
      const double jungleNight[16] = {17.0, 16.0, 12.0, 11.5, 11.0, 10.0, 9.5, 8.0, 7.0, 6.5, 6.5, 7.0, 19.5, 26.5, 28.0, 29.0};

      sigs.clear();
      for (int i = 0; i < 16; ++i)
      {
         double val = pow(10.0, jungleNight[i] * 0.1);
         sigs.push_back(val);
      }

      WsfStringId jungleNightId = "jungle_night";
      mBackgroundNoise.AddStateEntry(jungleNightId, freqs, sigs);

      // urban industrial (52 dBA)
      const double industrial[16] =
         {53.0, 52.5, 51.0, 50.0, 49.0, 47.0, 46.0, 44.5, 43.0, 42.0, 41.5, 40.5, 35.0, 31.0, 28.0, 26.0};

      sigs.clear();
      for (int i = 0; i < 16; ++i)
      {
         double val = pow(10.0, industrial[i] * 0.1);
         sigs.push_back(val);
      }
      WsfStringId industrialId = "industrial";
      mBackgroundNoise.AddStateEntry(industrialId, freqs, sigs);

      // rural (35 dBA)
      const double rural[16] = {42.0, 41.5, 33.0, 28.5, 27.0, 25.0, 23.5, 23.0, 23.0, 23.0, 22.5, 22.5, 22.5, 17.0, 13.5, 13.5};
      sigs.clear();
      for (int i = 0; i < 16; ++i)
      {
         double val = pow(10.0, rural[i] * 0.1);
         sigs.push_back(val);
      }
      WsfStringId ruralId = "rural";
      mBackgroundNoise.AddStateEntry(ruralId, freqs, sigs);

      // residential (48 dBA)
      const double residential[16] =
         {44.5, 45.0, 46.0, 45.0, 43.0, 40.5, 40.0, 40.0, 39.5, 39.0, 39.0, 38.5, 34.0, 25.0, 24.5, 23.5};
      sigs.clear();
      for (int i = 0; i < 16; ++i)
      {
         double val = pow(10.0, residential[i] * 0.1);
         sigs.push_back(val);
      }
      WsfStringId residentialId = "residential";
      mBackgroundNoise.AddStateEntry(residentialId, freqs, sigs);
      mBackgroundNoise.AddStateEntry("default", freqs, sigs);

      mBackgroundNoise.InitializeType();
   }
   WsfStandardAcousticSignature mHumanHearingThreshold;
   WsfStandardAcousticSignature mBackgroundNoise;
};

WsfAcousticSensor::WsfAcousticSensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mAcousticModeList()
   , mSharePtr(new SharedData)
{
   SetClass(cPASSIVE | cACOUSTIC); // This is a passive acoustic sensor
   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new AcousticMode(aScenario)));

   // Assign the default sensor scheduler and tracker
   SetScheduler(ut::make_unique<WsfDefaultSensorScheduler>());
   SetTracker(ut::make_unique<WsfDefaultSensorTracker>(aScenario));
}

WsfAcousticSensor::~WsfAcousticSensor()
{
   mSharePtr->Unref();
}

WsfAcousticSensor::WsfAcousticSensor(const WsfAcousticSensor& aSrc)
   : WsfSensor(aSrc)
   , mAcousticModeList()
   , mSharePtr(aSrc.mSharePtr)
{
   assert(mSharePtr);
   mSharePtr->AddRef();
}

// virtual
WsfSensor* WsfAcousticSensor::Clone() const
{
   return new WsfAcousticSensor(*this);
}

// virtual
bool WsfAcousticSensor::Initialize(double aSimTime)
{
   bool ok = WsfSensor::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mAcousticModeList);

   unsigned int i;

   // if threshold pointer is not defined, set it to human hearing
   for (i = 0; i < mAcousticModeList.size(); ++i)
   {
      if (mAcousticModeList[i]->mThresholdPtr == nullptr)
      {
         mAcousticModeList[i]->mThresholdPtr = &mSharePtr->mHumanHearingThreshold;
      }
   }

   return ok;
}

// virtual
void WsfAcousticSensor::Update(double aSimTime)
{
   // Bypass updates if not time for an update.  This avoids unnecessary device updates.
   // (A little slop is allowed to make sure event-driven chances occur as scheduled)
   if (mNextUpdateTime <= (aSimTime + 1.0E-5))
   {
      WsfSensor::Update(aSimTime);
      PerformScheduledDetections(aSimTime); // Perform any required detection attempts
   }
}

// virtual
size_t WsfAcousticSensor::GetEM_RcvrCount() const
{
   // Returns 0 prior to initialization
   return mAcousticModeList.empty() ? 0U : 1U;
}

// virtual
WsfEM_Rcvr& WsfAcousticSensor::GetEM_Rcvr(size_t aIndex) const
{
   return mAcousticModeList[mModeListPtr->GetCurrentMode()]->mRcvr;
}

// ****************************************************************************
// Definition for the nested class that implements the mode of the sensor.

WsfAcousticSensor::AcousticMode::AcousticMode(WsfScenario& aScenario)
   : WsfSensorMode()
   , mAntenna()
   , mRcvr(WsfEM_Rcvr::cRF_SENSOR, &mAntenna)
   , mDetectionThreshold(0.0)
   , mVerbose(false)
   , mAtmosphere(aScenario.GetAtmosphere())
   , mSensorType(cACOUSTIC_HUMAN)
   , mEffectiveFilterBandwidth()
   , mThresholdPtr(nullptr)
   , cDEFAULT_ID("default")
   , mBackgroundNoiseStateId("default")
   , mTerrainPtr(nullptr)
{
   SetCapabilities(cALL ^ cPULSEWIDTH ^ cFREQUENCY ^ cPULSE_REPITITION_INTERVAL);
}

WsfAcousticSensor::AcousticMode::AcousticMode(const AcousticMode& aSrc)
   : WsfSensorMode(aSrc)
   , mAntenna(aSrc.mAntenna)
   , mRcvr(aSrc.mRcvr, &mAntenna)
   , mDetectionThreshold(aSrc.mDetectionThreshold)
   , mVerbose(aSrc.mVerbose)
   , mAtmosphere(aSrc.mAtmosphere)
   , mSensorType(aSrc.mSensorType)
   , mEffectiveFilterBandwidth(aSrc.mEffectiveFilterBandwidth)
   , mThresholdPtr(aSrc.mThresholdPtr)
   , cDEFAULT_ID("default")
   , mBackgroundNoiseStateId(aSrc.mBackgroundNoiseStateId)
   , mTerrainPtr(nullptr)
{
}

// virtual
WsfMode* WsfAcousticSensor::AcousticMode::Clone() const
{
   return new AcousticMode(*this);
}

// virtual
WsfAcousticSensor::AcousticMode::~AcousticMode()
{
   delete mTerrainPtr;
}

// virtual
bool WsfAcousticSensor::AcousticMode::Initialize(double aSimTime)
{
   bool ok     = WsfSensorMode::Initialize(aSimTime);
   mTerrainPtr = new wsf::Terrain(mSensorPtr->GetPlatform()->GetTerrain());
   ok &= mAntenna.Initialize(mSensorPtr);
   mMaximumRange = mAntenna.GetMaximumRange();

   // set receiver frequency to bogus number so it can initialize
   mRcvr.SetFrequency(1.0);
   ok &= mRcvr.Initialize(*mSensorPtr->GetSimulation());

   // Set the debug flags
   mRcvr.SetDebugEnabled(GetSensor()->DebugEnabled());

   if (mSensorType == cACOUSTIC_HUMAN) // currently the only allowed type
   {
      // set effective filter bandwidth
      mEffectiveFilterBandwidth.clear();
      mEffectiveFilterBandwidth.push_back(133.0);
      mEffectiveFilterBandwidth.push_back(115.9);
      mEffectiveFilterBandwidth.push_back(105.7);
      mEffectiveFilterBandwidth.push_back(100.8);
      mEffectiveFilterBandwidth.push_back(98.7);
      mEffectiveFilterBandwidth.push_back(98.7);
      mEffectiveFilterBandwidth.push_back(98.7);
      mEffectiveFilterBandwidth.push_back(102.2);
      mEffectiveFilterBandwidth.push_back(117.3);
      mEffectiveFilterBandwidth.push_back(122.5);
      mEffectiveFilterBandwidth.push_back(140.0);
      mEffectiveFilterBandwidth.push_back(161.0);
      mEffectiveFilterBandwidth.push_back(192.5);
      mEffectiveFilterBandwidth.push_back(231.0);
      mEffectiveFilterBandwidth.push_back(290.9);
      mEffectiveFilterBandwidth.push_back(366.1);
      mEffectiveFilterBandwidth.push_back(461.0);
      mEffectiveFilterBandwidth.push_back(580.3);
      mEffectiveFilterBandwidth.push_back(730.5);
      mEffectiveFilterBandwidth.push_back(919.8);
      mEffectiveFilterBandwidth.push_back(1157.9);
      mEffectiveFilterBandwidth.push_back(1457.5);
      mEffectiveFilterBandwidth.push_back(1835.2);
      mEffectiveFilterBandwidth.push_back(2310.1);
   }
   // only allow human hearing for now
   // else if (mSensorType = cACOUSTIC_NARROW)
   //{
   //   TODO
   //}
   // else if (mSensorType = cACOUSTIC_WIDE)
   //{
   //   TODO
   //}

   return ok;
}

// virtual
bool WsfAcousticSensor::AcousticMode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "detection_threshold")
   {
      aInput.ReadValue(mDetectionThreshold);
      mRcvr.SetDetectionThreshold(mDetectionThreshold);
   }
   else if (command == "acoustic_type")
   {
      std::string aType;
      aInput.ReadValue(aType);
      if (aType != "human")
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "verbose")
   {
      mVerbose = true;
   }
   else if (command == "background_noise")
   {
      std::string type;
      aInput.ReadValue(type);
      if ((type == "jungle_day") || (type == "jungle_night") || (type == "industrial") || (type == "rural") ||
          (type == "residential"))
      {
         mBackgroundNoiseStateId = WsfStringId(type);
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (mAntenna.ProcessInput(aInput) || mRcvr.ProcessInput(aInput) || mRcvr.ProcessInputBlock(aInput))
   {
   }
   else if (mAtmosphere.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfSensorMode::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfAcousticSensor::AcousticMode::AttemptToDetect(double           aSimTime,
                                                      WsfPlatform*     aTargetPtr,
                                                      Settings&        aSettings,
                                                      WsfSensorResult& aResult)
{
   bool detected = false;
   aResult.Reset(aSettings);
   aResult.SetCategory(GetSensor()->GetZoneAttenuationModifier());
   GetSensor()->UpdatePosition(aSimTime); // Ensure my position is current
   aTargetPtr->Update(aSimTime);          // Ensure the target position is current
   auto out = ut::log::debug();
   if (GetSensor()->DebugEnabled())
   {
      out << "Attempting to detect target.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Target: " << aTargetPtr->GetName();
   }

   // Determine if concealed (like in a building).
   if (aResult.mFailedStatus == 0)
   {
      aResult.mCheckedStatus |= WsfSensorResult::cCONCEALMENT;
      if (aTargetPtr->GetConcealmentFactor() > 0.99F)
      {
         // We can't detect if it's in a building (or something like that)
         aResult.mFailedStatus |= WsfSensorResult::cCONCEALMENT;
         // Must have object pointers so event_output and debug output show locations.
         aResult.BeginGenericInteraction(nullptr, aTargetPtr, &mRcvr);
      }
   }

   if ((aResult.mFailedStatus == 0) && (aResult.BeginOneWayInteraction(&mRcvr, aTargetPtr) == 0))
   {
      // Set the position of the antenna.
      aResult.SetReceiverBeamPosition();

      // propagation factor
      double propagation   = pow(4.0 * UtMath::cPI * aResult.mRcvrToTgt.mRange, 2.0);
      double propagationDB = 10.0 * log10(propagation);

      // Account for structural masking...
      if (aResult.mMaskingFactor > 0.0)
      {
         // The masking factor is in the range [0..1]. When converting to dB it will be zero or negative,
         // and therefore must be SUBTRACTED to the propagation value, which is positive dB (but later
         // is subtracted...)
         propagationDB -= UtMath::LinearToDB(aResult.mMaskingFactor);
      }
      else
      {
         propagationDB = 300.0;
      }

      // Doppler coefficient
      double dopplerEffect = ComputeDopplerTerm(aResult);
      if (dopplerEffect < 0)
      {
         // perceiving the sound is impossible in this case
         // either the target is moving faster than the sound towards us
         // or we are moving faster than the sound away from the target
         return false;
      }

      // Loop over the 1/3-octave band frequencies to compute S/N for each
      // frequency
      for (int band = 0; band < GetCenterFreqSize(); ++band)
      {
         // compute auditory filter band level
         double filteredSource   = ApplyFilterWeighting(band, aTargetPtr, 1, dopplerEffect);
         double filteredSourceDB = 10.0 * log10(filteredSource);

         // compute atmospheric attenuation for frequency
         double attenuationDB = AtmosphericAttenuation(aResult, GetCenterFreq(band)) * aResult.mRcvrToTgt.mRange * 0.01;

         // determine ground effect
         double groundEffectDB = GroundEffectAttenuation(&aResult, GetCenterFreq(band), band);
         if (aResult.CategoryIsSet())
         {
            double aZoneAttenuation = (std::max(0.0, 1.0 - aResult.mZoneAttenuationValue));
            attenuationDB *= aZoneAttenuation;
         }

         // determine noise pressure @ receiver
         double receivedPressureDB = filteredSourceDB + groundEffectDB - attenuationDB - propagationDB;

         // compute filtered background noise
         double filteredBackground   = ApplyFilterWeighting(band);
         double filteredBackgroundDB = 10.0 * log10(filteredBackground);

         // adjust background for Pd and false alarm rate
         filteredBackgroundDB += 10.0 * log10(2.32 / 0.4 / sqrt(mEffectiveFilterBandwidth[band]));
         // get sensing threshold
         double threshold   = mThresholdPtr->GetNoisePressure(cDEFAULT_ID, GetCenterFreq(band));
         double thresholdDB = 10.0 * log10(threshold);

         if ((receivedPressureDB > filteredBackgroundDB) && (receivedPressureDB > thresholdDB))
         {
            detected = true;
         }

         double ston = receivedPressureDB - filteredBackgroundDB;
         if (ston > aResult.mSignalToNoise)
         {
            aResult.mSignalToNoise = ston;

            // determine the probability of detection
            aResult.mPd = ComputeProbabilityOfDetection(receivedPressureDB,
                                                        std::max(filteredBackgroundDB, thresholdDB),
                                                        mDetectionThreshold);
         }

         if (GetSensor()->DebugEnabled())
         {
            out.AddNote() << "1/3 Octave Band Freq: " << GetCenterFreq(band);
            out.AddNote() << "Filtered Source: " << filteredSourceDB;
            out.AddNote() << "Attenuation: " << attenuationDB;
            out.AddNote() << "Propagation: " << propagationDB;
            out.AddNote() << "Ground Effect: " << groundEffectDB;
            out.AddNote() << "Received Sound: " << receivedPressureDB;
            out.AddNote() << "Filtered Background: " << filteredBackgroundDB;
            out.AddNote() << "Threshold: " << thresholdDB;
         }
      }
   }

   // see if the resulting Pd results in detection
   if (aResult.mFailedStatus == 0)
   {
      aResult.mCheckedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
      if (detected)
      {
         // Check terrain masking only if all other checks succeed
         aResult.MaskedByTerrain();
      }
      else
      {
         aResult.mFailedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
      }
   }

   // Compute component effects.
   WsfSensorComponent::PostAttemptToDetect(*GetSensor(), aSimTime, aTargetPtr, aResult);

   // Determine the impact (if any) that an external script should have on detection status
   detected = GetSensor()->ScriptAllowDetection(aSimTime, aTargetPtr, aResult);

   if (GetSensor()->DebugEnabled())
   {
      auto note = out.AddNote() << "Sensor Debug Info.";
      aResult.Print(note);
   }
   GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, aResult);
   return detected;
}


//! Computes the filtered weights to account for human hearing
//! @param aIndex The 1/3 octave band frequency index
//! @param aTargetPtr A pointer to the target platform
//! @param aFlag = 1 if target signature, 0 if background
//! @param aDoppler Doppler shift frequency adjustment
//! @returns The 1/3 octave frequency weighting factor (Ref 3)
double WsfAcousticSensor::AcousticMode::ApplyFilterWeighting(int          aIndex,
                                                             WsfPlatform* aTargetPtr,
                                                             const int    aFlag,
                                                             const double aDoppler)
{
   const double Weight[5][24] = {{0.0, 0.0, 0.3048, 0.1521, 0.07568, 0.03776, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                  0.0, 0.0, 0.0,    0.0,    0.0,     0.0,     0.0, 0.0, 0.0, 0.0, 0.0, 0.0},

                                 {0.0,    0.5333, 0.4355,  0.3565, 0.2917, 0.2388, 0.1950, 0.1596,
                                  0.1306, 0.1069, 0.08750, 0.0,    0.0,    0.0,    0.0,    0.0,
                                  0.0,    0.0,    0.0,     0.0,    0.0,    0.0,    0.0,    0.0},

                                 {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                  1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},

                                 {1.0,    0.6683,  0.5176,  0.3999, 0.3090, 0.2388, 0.1845, 0.1429,
                                  0.1104, 0.08311, 0.06592, 0.0,    0.0,    0.0,    0.0,    0.0,
                                  0.0,    0.0,     0.0,     0.0,    0.0,    0.0,    0.0,    0.0},

                                 {0.5, 0.45, 0.3846, 0.1321, 0.04539, 0.01560, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                  0.0, 0.0,  0.0,    0.0,    0.0,     0.0,     0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};

   int lowerOffset = std::min(std::max((aIndex - 2), 0), 2);
   int upperOffset = std::min(((GetCenterFreqSize() - 1) - aIndex), 2);

   double level = 0.0;
   for (int i = -lowerOffset; i < (upperOffset + 1); ++i)
   {
      double pressure = 0.0;
      if (aFlag == 1)
      {
         double freq = GetCenterFreq(aIndex + i) * aDoppler;
         pressure    = WsfAcousticSignature::GetValue(aTargetPtr, freq);
      }
      else
      {
         pressure = dynamic_cast<WsfAcousticSensor*>(GetSensor())
                       ->mSharePtr->mBackgroundNoise.GetNoisePressure(mBackgroundNoiseStateId, GetCenterFreq(aIndex + i));
      }

      level += pressure * Weight[i + 2][aIndex];
   }

   return level;
}

// virtual
void WsfAcousticSensor::AcousticMode::Deselect(double aSimTime)
{
   mRcvr.Deactivate();
}


// virtual
void WsfAcousticSensor::AcousticMode::Select(double aSimTime)
{
   mRcvr.Activate();
}

//! Compute the atmospheric attenuation for the current interaction.
//! and frequency
//! @param aResult The current interaction data.
//! @param aFreq The current center band frequency
//! @returns The atmospheric attenuation in dB / 100m (Ref 2)
double WsfAcousticSensor::AcousticMode::AtmosphericAttenuation(const WsfSensorResult& aResult, double aFreq)
{
   // determine atmospheric values that will be needed
   // use the mid-point altitude to determine values
   double alt        = 0.5 * (aResult.mRcvrLoc.mAlt + aResult.mTgtLoc.mAlt);
   double temp       = mAtmosphere.Temperature(alt);
   double relHum     = mAtmosphere.RelativeHumidity(alt);
   double press      = mAtmosphere.Pressure(alt);
   double pressSL    = mAtmosphere.Pressure(0.0);
   double pressRatio = press / pressSL;

   // calculate terms that are independent of frequency
   double aH  = relHum / pressRatio * pow(10.0, (20.318 - 2939.0 / temp - 4.922 * log10(temp)));
   double fro = pressRatio * (24.0 + 44100.0 * aH * (0.05 + aH) / (0.391 + aH));
   double frn =
      (9.0 + 350.0 * aH * exp(-6.142 * (pow((293.0 / temp), (1.0 / 3.0)) - 1.0))) * pressRatio * sqrt(293.0 / temp);
   double muo_a = 1.881e4 * pow(temp, -2.5) * exp(-2239.1 / temp) * pow((1.0 - exp(-2239.1 / temp)), -2.0);
   double mun_a = 1.571e5 * pow(temp, -2.5) * exp(-3352.0 / temp) * pow((1.0 - exp(-3352.0 / temp)), -2.0);
   double mc_f2 = 2.152e-12 * sqrt(temp) / pressRatio;

   // calculate terms that are frequency dependent
   double moPrime = 2.0 * muo_a * aFreq / (aFreq / fro + fro / aFreq);
   double mnPrime = 2.0 * mun_a * aFreq / (aFreq / frn + frn / aFreq);
   double mc      = mc_f2 * pow(aFreq, 2.0);

   double attenuation = 434.3 * (moPrime + mnPrime + mc);

   return attenuation;
}


//! Compute the probability of detection.
//!
//! @param aSignal [input] The filtered target noise
//! @param aNoise  [input] The filtered background noise
//! @param aThreshold [input] The signal-to-noise ratio required to generate a Pd = 0.5.
//!
//! @returns The probability of detection in the range [0, 1]
double WsfAcousticSensor::AcousticMode::ComputeProbabilityOfDetection(double aSignal, double aNoise, double aThreshold)
{
   if (aSignal <= 0.0)
   {
      return 0.0;
   }

   double signalToNoise = aSignal - aNoise;
   double beta          = signalToNoise - aThreshold;

#if 1
   // The following refers to MDC report B1368 (15 Jan 1989).
   //
   // Equation 5.3-12 defines Pd = Q(-beta) + Q(beta + 2*alpha)
   // where Q is the Gaussian Q function.
   //
   // The second term is very small compared to the first so:
   //
   // Pd = Q(-beta) = 1 - Q(beta)
   //
   // THe 'Handbook of Mathematical Functions' (Abramowitz and Stegun) say:
   //
   // P(x) + Q(x) = 1         (equation 26.2.5)
   // P(x) = 1 - Q(x)
   //
   // Pd = P(x)
   //
   // The following is the approximation for P(x) as defined in 26.2.16.

   static const double cCONST = 0.39894228; // 1.0 / sqrt(2*pi)
   double              pd     = 0.5;
   double              x      = beta;
   double              z      = cCONST * exp(-0.5 * x * x);
   if (x > 1.0E-5)
   {
      double t = 1.0 / (1.0 + 0.33267 * x);
      pd       = 1.0 - z * (0.4361836 * t - 0.1201676 * t * t + .9372980 * t * t * t);
   }
   else if (x < -1.0E-5)
   {
      double t = 1.0 / (1.0 - 0.33267 * x);
      pd       = z * (0.4361836 * t - 0.1201676 * t * t + .9372980 * t * t * t);
   }

#else

   // The following is a pretty darn close approximation to the above.

   double              pd        = 0.0;
   static const double cBASE     = 10.0;
   static const double cEXPONENT = -0.5;

   // This pretty closely approximates the function
   if (beta >= 0.0)
   {
      // pd = 0.5 + 0.5 * (1.0 - pow(10.0, -0.5 * beta));
      pd = 1.0 - 0.5 * pow(cBASE, cEXPONENT * beta);
   }
   else
   {
      // pd = 0.5 - 0.5 * (1.0 - pow(10.0,  0.5 * beta));
      pd = 0.5 * pow(cBASE, cEXPONENT * (-beta));
   }
#endif

   return pd;
}


//! Computes the sound attenuation due to ground effect
//! @param aResultPtr Pointer to the current interaction data.
//! @param aFreq  The current 1/3 octave band center frequency
//! @param aIndex The current 1/3 octave band center frequency index
//!
//! @returns The ground effect attenuation [0, unknown] (Ref 1)
double WsfAcousticSensor::AcousticMode::GroundEffectAttenuation(WsfSensorResult* aResultPtr,
                                                                const double     aFreq,
                                                                const int        aIndex)
{
   double returnValue = 1.0;

   // compute angle of incidence of reflected path and slant range
   // from target to reflection point
   double incidenceAngle       = 0.0;
   double reflectionSlantRange = 0.0;
   double reflectionLoc[3];
   ComputeIncidenceAngle(aResultPtr, incidenceAngle, reflectionSlantRange, &reflectionLoc[0]);

   // The algorithms document in Reference 1 are valid for incidence
   // angles less than 5 degrees.  For larger incidence angle, do not
   // account for any ground effect
   if (incidenceAngle > (5.0 * UtMath::cRAD_PER_DEG))
   {
      returnValue = 1.0;
   }
   else
   {
      double rPrime = aResultPtr->mTgtToRcvr.mRange / reflectionSlantRange;

      // compute ground range from target to receiver
      double groundRange = UtMeasurementUtil::GroundRange(aResultPtr->mTgtLoc.mLocWCS, aResultPtr->mRcvrLoc.mLocWCS);

      // use 1/2 the altitude difference to compute the speed of sound
      double atmosAlt = 0.5 * (aResultPtr->mTgtLoc.mAlt + aResultPtr->mRcvrLoc.mAlt);
      double sonicVel = mAtmosphere.SonicVelocity(atmosAlt);

      // compute turbulence effects: aCoherenceFunction
      const double TURBULENCE_SCALE_PARAMETER = 1.1; // Ref 1
      double       betaDriver                 = sonicVel * groundRange / aFreq;
      double       beta                       = 0.0;
      if (sqrt(betaDriver) > TURBULENCE_SCALE_PARAMETER)
      {
         beta = 0.5;
      }
      else
      {
         beta = 1.0;
      }

      // set fluctuating index of refraction per Ref 1
      // if perfectly still = 0;
      // if nominally still = 1e-8
      // if moderately still = 1e-7
      // if turbulent = 1e-6
      // for now, assume turbulent atmosphere
      const double REFRACTION_INDEX = 1.0e-6;
      double       aP =
         REFRACTION_INDEX * pow((aFreq / sonicVel), 2.0) * groundRange * TURBULENCE_SCALE_PARAMETER * sqrt(UtMath::cPI);
      double coherenceFunction = exp(-0.2 * beta * aP);

      // compute ground impedance Z (complex value)
      // effective flow resistivity & inverse effect depth of surface layer
      // set per Ref 1 with the following assumptions
      //    Urban landcover uses values for tarmac/concrete
      //    Barren landcover uses values or sand
      //    Wetland landcovers use values for wet compacted soil
      //    All other landcovers use values for grass
      double flowResistivity = 0.0; // (N-s/m^4)
      double inverseDepth    = 0.0; // (1/m)

      WsfEnvironment::LandCover aCover = GetSensor()->GetScenario().GetEnvironment().GetLandCover();
      if (aCover == WsfEnvironment::cBARREN)
      {
         flowResistivity = 80.0e3; // Ref 1 lists 60 - 100 kN-s/m^4
         inverseDepth    = 0.0;
      }
      else if ((aCover == WsfEnvironment::cWETLAND_FORESTED) || (aCover == WsfEnvironment::cWETLAND_NONFORESTED))
      {
         flowResistivity = 4000.0e3;
         inverseDepth    = 0.0;
      }
      else if (aCover == WsfEnvironment::cURBAN)
      {
         flowResistivity = 4500.0e3;
         inverseDepth    = 0.0;
      }
      else
      {
         flowResistivity = 40.0e3; // Ref 1 lists 30 - 50 kN-s/m^4
         inverseDepth    = 32.5;   // Ref 1 lists 20 - 45 1/m
      }
      double rho          = mAtmosphere.Density(reflectionLoc[2]);
      double zReal        = sqrt(flowResistivity / UtAtmosphere::cGAMMA / UtMath::cPI / rho / aFreq);
      double sonicVelRefl = mAtmosphere.SonicVelocity(reflectionLoc[2]);
      double zImag        = zReal + sonicVelRefl * inverseDepth * 0.2 / UtAtmosphere::cGAMMA / UtMath::cPI / aFreq;
      std::complex<double> aZ(zReal, zImag);

      // compute sound pressure reflection coefficient
      std::complex<double> reflectionCoefficient = (sin(incidenceAngle) - (1.0 / aZ)) / (sin(incidenceAngle) + (1.0 / aZ));

      // compute "numerical distance"
      double lambdaRefl = sonicVelRefl / aFreq;
      double imag       = 0.5 * (2.0 * UtMath::cPI / lambdaRefl * reflectionSlantRange);
      imag *= pow(std::abs(std::complex<double>(sin(incidenceAngle)) + std::complex<double>(1.0) / aZ), 2.0);
      std::complex<double> numericalDistance(0.0, imag);

      // compute zeta and eta: need frequency span of 1/3 octave band
      double deltaFreq = 0.0;
      if (aIndex == 0)
      {
         deltaFreq = 13.0;
      }
      else if (aIndex == (GetCenterFreqSize() - 1))
      {
         deltaFreq = 4.8e3;
      }
      else
      {
         deltaFreq = GetCenterFreq(aIndex + 1) - GetCenterFreq(aIndex - 1);
      }
      double zeta = UtMath::cPI * deltaFreq / aFreq;
      double eta  = 2 * UtMath::cPI * sqrt(1 + pow((deltaFreq * 0.5 / aFreq), 2.0));

      // compute boundary loss factor
      std::complex<double> boundaryLoss(0.0, 0.0);
      if (std::abs(numericalDistance) < 10.0)
      {
         boundaryLoss +=
            std::complex<double>(1.0) +
            sqrt(std::complex<double>(UtMath::cPI) * numericalDistance) * exp(-numericalDistance) -
            std::complex<double>(2.0) * numericalDistance *
               (1.0 + numericalDistance / 3.0 + pow(numericalDistance, 2.0) / 10.0 + pow(numericalDistance, 3.0) / 43.0) *
               exp(-numericalDistance);
      }
      else
      {
         boundaryLoss += -(0.5 / numericalDistance + 3.0 / pow((2.0 * numericalDistance), 2.0) +
                           3.0 * 5.0 / pow((2.0 * numericalDistance), 3.0));
      }

      // compute relative image source strength
      std::complex<double> aQ = (0.0);
      aQ += reflectionCoefficient + boundaryLoss * (std::complex<double>(1.0) - reflectionCoefficient);

      // compute return value
      double slantRange = aResultPtr->mTgtToRcvr.mRange;
      double phaseAngle = aResultPtr->mTgtToRcvr.mAz;
      returnValue       = 1.0 + pow((rPrime * std::abs(aQ)), 2.0) +
                    2 * rPrime * coherenceFunction * std::abs(aQ) *
                       (lambdaRefl / zeta / (slantRange - reflectionSlantRange)) *
                       sin(zeta * (reflectionSlantRange - slantRange) / lambdaRefl) *
                       cos(eta * (reflectionSlantRange - slantRange) / lambdaRefl + phaseAngle);
   }

   return returnValue;
}


//! Computes the incidence angle of the vector from the ground
//! reflection point to the target
//! @param aResultPtr Pointer to the current interaction data.
//! @param aAngle [output] The incidence angle
//! @param aRange [output] slant range from target to reflection point
//! @param aLoc [output] WCS location of reflection point
//! @returns The incidence angle (Ref 1)
void WsfAcousticSensor::AcousticMode::ComputeIncidenceAngle(WsfSensorResult* aResultPtr,
                                                            double&          aAngle,
                                                            double&          aRange,
                                                            double*          aLoc)
{
   // get pointer to target
   WsfEM_Rcvr* aRcvrPtr = aResultPtr->GetReceiver();

   // compute ground range from target to receiver
   double groundRange = UtMeasurementUtil::GroundRange(aResultPtr->mTgtLoc.mLocWCS, aResultPtr->mRcvrLoc.mLocWCS);

   // determine location of acoustic wave reflection
   double targetLocNED[3];
   aRcvrPtr->GetPlatform()->ConvertWCSToNED(aResultPtr->mTgtLoc.mLocWCS, targetLocNED);
   double bearing              = atan2(targetLocNED[1], targetLocNED[0]);
   double reflectionLat        = 0.0;
   double reflectionLon        = 0.0;
   double reflectionDistanceNM = groundRange / UtMath::cM_PER_NM;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(aResultPtr->mTgtLoc.mLat,
                                                    aResultPtr->mTgtLoc.mLon,
                                                    bearing,
                                                    reflectionDistanceNM,
                                                    reflectionLat,
                                                    reflectionLon);
   float reflectionAlt = 0.0;
   if (mTerrainPtr->IsEnabled())
   {
      mTerrainPtr->GetElevInterp(reflectionLat, reflectionLon, reflectionAlt);
   }
   aLoc[0] = reflectionLat;
   aLoc[1] = reflectionLon;
   aLoc[2] = double(reflectionAlt);

   // use ComputeNEDTransform procedure to determine WCS to NED transform
   double WCSToNEDTransform[3][3];
   double reflectionWCS[3];
   UtEllipsoidalEarth::ComputeNEDTransform(reflectionLat, reflectionLon, reflectionAlt, WCSToNEDTransform, reflectionWCS);

   // get NED position of target relative to reflection point
   double tgtLocNED[3];
   UtMat3d::Transform(tgtLocNED, WCSToNEDTransform, reflectionWCS);

   // compute incidence angle
   double xy = sqrt((tgtLocNED[0] * tgtLocNED[0]) + (tgtLocNED[1] * tgtLocNED[1]));

   aAngle = UtMath::Sign(-tgtLocNED[2]) * UtMath::cPI_OVER_2;
   if (xy != 0.0)
   {
      aAngle = -atan2(tgtLocNED[2], xy);
   }

   // compute slant range from target to reflection point
   double tgtToReflectionVecWCS[3];
   UtVec3d::Subtract(tgtToReflectionVecWCS, reflectionWCS, aResultPtr->mTgtLoc.mLocWCS);

   aRange = UtVec3d::Normalize(tgtToReflectionVecWCS);
}


//! Computes the divisor for freq_flight = freq_static / dopplerTerm
//! @param aResult Reference to the current interaction data.
//! @returns the dopplerTerm
double WsfAcousticSensor::AcousticMode::ComputeDopplerTerm(const WsfSensorResult& aResult)
{
   // use 1/2 the altitude difference to compute the speed of sound
   double atmosAlt = 0.5 * (aResult.mTgtLoc.mAlt + aResult.mRcvrLoc.mAlt);
   double sonicVel = mAtmosphere.SonicVelocity(atmosAlt); // speed of sound at this altitude

   double aVelWCS[3];
   double aLocWCS[3];
   aResult.GetReceiver()->GetPlatform()->GetVelocityWCS(aVelWCS); // sensor platform's velocity in WCS
   aResult.GetReceiver()->GetPlatform()->GetLocationWCS(aLocWCS); // sensor platform's location in WCS

   double bVelWCS[3];
   double bLocWCS[3];
   aResult.GetTarget()->GetVelocityWCS(bVelWCS); // target's velocity in WCS
   aResult.GetTarget()->GetLocationWCS(bLocWCS); // target's location in WCS

   // compute relative locations
   double relLocWCS_AtoB[3];
   double relLocWCS_BtoA[3];
   UtVec3d::Subtract(relLocWCS_AtoB, bLocWCS, aLocWCS); // vector from sensor to target
   UtVec3d::Subtract(relLocWCS_BtoA, aLocWCS, bLocWCS); // vector from target to sensor
   // compute relative velocities
   double relSpeedAtoB = 0.0;
   double relSpeedBtoA = 0.0;

   // doesn't work if target & sensor are co-located
   if (UtVec3d::Normalize(relLocWCS_AtoB) > 0 && UtVec3d::Normalize(relLocWCS_BtoA) > 0)
   {
      relSpeedAtoB = UtVec3d::DotProduct(relLocWCS_AtoB, aVelWCS);
      relSpeedBtoA = UtVec3d::DotProduct(relLocWCS_BtoA, bVelWCS);
   }

   // check if the target is faster than the sound coming towards us
   // check if we are faster than sound going away from the target
   if ((-relSpeedAtoB >= sonicVel) || (relSpeedBtoA >= sonicVel))
   {
      return -1;
   }

   // common Doppler equation has Vsource as negative if moving away towards listener
   // our Vsource is a positive closing speed, so we subtract it (relSpeedBtoA) in the denominator
   double dopplerCoefficient = (sonicVel + relSpeedAtoB) / (sonicVel - relSpeedBtoA);
   return dopplerCoefficient;
}
