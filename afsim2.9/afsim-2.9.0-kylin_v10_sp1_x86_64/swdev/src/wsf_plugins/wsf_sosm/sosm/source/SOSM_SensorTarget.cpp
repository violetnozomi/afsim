// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SOSM_SensorTarget.hpp"

#include <cmath>
#include <iostream>
#include <memory>

#include "SOSM_Atmosphere.hpp"
#include "SOSM_BinaryIO.hpp"
#include "SOSM_Interaction.hpp"
#include "SOSM_Manager.hpp"
#include "SOSM_ScalarTable2D.hpp"
#include "SOSM_ScalarTable3D.hpp"
#include "SOSM_Sensor.hpp"
#include "SOSM_SimpleInteraction.hpp"
#include "SOSM_Target.hpp"
#include "SOSM_Utility.hpp"
#include "TblLookup.hpp"
#include "UtMath.hpp"

SOSM_SensorTarget::SnrAtmTableMap SOSM_SensorTarget::sSnrAtmTables;
SOSM_SensorTarget::SnrTgtTableMap SOSM_SensorTarget::sSnrTgtTables;

// =================================================================================================
SOSM_SensorTarget::SOSM_SensorTarget(SOSM_Manager* aManagerPtr)
   : mManagerPtr(aManagerPtr)
   , mSensorPtr(nullptr)
   , mTargetPtr(nullptr)
   , mUsingTables(false)
   , mSnrAtmPtr(nullptr)
   , mSnrTgtPtr(nullptr)
{
}

// =================================================================================================
// virtual
SOSM_SensorTarget::~SOSM_SensorTarget()
{
   delete mSensorPtr;
   delete mTargetPtr;
}

// =================================================================================================
bool SOSM_SensorTarget::Initialize(const std::string& aSensorType, const std::string& aTargetType)
{
   // Reset the object just in case the caller is reusing it.
   delete mSensorPtr;
   mSensorPtr = nullptr;
   delete mTargetPtr;
   mTargetPtr   = nullptr;
   mUsingTables = false;

   // Allocate and initialize new sensor and target instances.

   bool ok    = true;
   mSensorPtr = mManagerPtr->AllocateSensor(aSensorType);
   mTargetPtr = mManagerPtr->AllocateTarget(aTargetType);
   if (mSensorPtr == nullptr)
   {
      ok = false;
      ut::log::error() << "Unable to create a sensor instance of type " << aSensorType;
   }
   if (mTargetPtr == nullptr)
   {
      ok = false;
      ut::log::error() << "Unable to create a target instance of type " << aTargetType;
   }

   // If the sensor and target have been successfully created and initialized,
   // create/load the 'fast detection mode' tables if requested.

   if (ok && mManagerPtr->UseFastDetectionMode())
   {
      try
      {
         LoadSensorTargetTables();
      }
      catch (std::exception& e)
      {
         auto logError = ut::log::error() << "Exception occurred during fast_detection_mode initialization!";
         logError.AddNote() << "Error Message: " << e.what();
         logError.AddNote() << "fast_detection_mode will not be use for sensor type: " << aSensorType;
         logError.AddNote() << "Target Type: " << aTargetType;
      }
   }
   return ok;
}

// =================================================================================================
//! Compute the irradiance on the sensor when looking at the target.
//! @param aInteraction [input] The interaction object representing the sensor and target state.
//! @returns The irradiance on the sensor (W/cm^2)
float SOSM_SensorTarget::ComputeTargetIrradiance(SOSM_Interaction& aInteraction) const
{
   if (!mUsingTables)
   {
      // Compute the target irradiance using the actual sensor and target models.
      return static_cast<float>(mSensorPtr->ComputeTargetIrradiance(mTargetPtr, aInteraction));
   }

   // Compute the target irradiance using the fast detection mode.

   // Select the target state. If it has changed since the last time, reset the table lookup objects.

   size_t tgtStateIndex = mTargetPtr->SelectState(aInteraction);

   float tgtToSnrAz;
   float tgtToSnrEl;
   aInteraction.GetTargetToSensorAspect(tgtToSnrAz, tgtToSnrEl);
   tgtToSnrAz = static_cast<float>(UtMath::NormalizeAngleMinusPi_Pi(tgtToSnrAz));

   TblLookupLU<float> tgtAzLookup;
   TblLookupLU<float> tgtElLookup;
   mSnrTgtPtr->mRadiantIntensityPtrs[tgtStateIndex]->PolarLookup(tgtAzLookup, tgtElLookup, tgtToSnrAz, tgtToSnrEl);
   float targetIntensity_xR = mSnrTgtPtr->mRadiantIntensityPtrs[tgtStateIndex]->Interpolate(tgtAzLookup, tgtElLookup);
   float targetArea         = mSnrTgtPtr->mProjectedAreaPtrs[tgtStateIndex]->Interpolate(tgtAzLookup, tgtElLookup);

   float snrAlt     = aInteraction.GetSensorAltitude();
   float absTgtEl   = aInteraction.GetAbsoluteTargetElevation();
   float slantRange = std::max(aInteraction.GetSlantRange(), 1.0F);

   TblLookupLU<float> atmAltLookup;
   TblLookupLU<float> atmElLookup;
   TblLookupLU<float> atmRangeLookup;

   atmAltLookup.Lookup(mSnrAtmPtr->mForegroundRadiancePtr->IV1(), snrAlt);
   atmElLookup.Lookup(mSnrAtmPtr->mForegroundRadiancePtr->IV2(), absTgtEl);
   atmRangeLookup.Lookup(mSnrAtmPtr->mForegroundRadiancePtr->IV3(), slantRange);

   float backgroundRadiance_xR = mSnrAtmPtr->mBackgroundRadiancePtr->Interpolate(atmAltLookup, atmElLookup);
   float foregroundRadiance_xR =
      mSnrAtmPtr->mForegroundRadiancePtr->Interpolate(atmAltLookup, atmElLookup, atmRangeLookup);
   float transmittance_xR = mSnrAtmPtr->mTransmittancePtr->Interpolate(atmAltLookup, atmElLookup, atmRangeLookup);

   // Apply any user-supplied scale factors to radiance and transmittance.
   backgroundRadiance_xR *= aInteraction.mBackgroundScaleFactor; // Default 1.0
   foregroundRadiance_xR *= aInteraction.mForegroundScaleFactor; // Default 1.0
   transmittance_xR *= aInteraction.mTransmittanceScaleFactor;   // Default 1.0
   if (aInteraction.mTransmittanceScaleFactor < 0.01F)
   {
      backgroundRadiance_xR = foregroundRadiance_xR;
   }

   // Compute the target contrast irradiance.
   //
   // Note that the division by the average response in the first term accounts for the fact that
   // 'targetRadiantIntensity_xR' and 'transmittance_xR' have been multiplied by the response. Thus, when
   // multiplied together, the result is TWO multiplications of the response. The division eliminates one
   // of them (approximately).

   float averageResponse   = mSensorPtr->GetAverageResponse();
   float contrastIntensity = ((targetIntensity_xR * transmittance_xR) / averageResponse) +
                             (foregroundRadiance_xR - backgroundRadiance_xR) * targetArea;

   // In some cases the values have been multiplied by the sensor response before saving to the binary files.
   // In order to get output similar to the 'slow' mode we must divide it back out. This is an approximation,
   // but it is plenty good for this.

   aInteraction.mBodyArea           = targetArea;
   aInteraction.mBodyIntensity      = targetIntensity_xR;
   aInteraction.mPlumeArea          = 0.0F; // Not separately maintained
   aInteraction.mPlumeIntensity     = 0.0F; // Not separately maintained
   aInteraction.mTransmittance      = transmittance_xR;
   aInteraction.mBackgroundRadiance = backgroundRadiance_xR;
   aInteraction.mForegroundRadiance = foregroundRadiance_xR;
   aInteraction.mContrastIntensity  = contrastIntensity;

   double slantRangeCM = slantRange * 1.0E+2; // m -> cm
   // fabs() allows for positive or negative contrast
   float targetIrradiance = static_cast<float>(fabs(contrastIntensity) / (slantRangeCM * slantRangeCM));

   if (mManagerPtr->DebugLevel() > 0)
   {
      auto logDebug = ut::log::debug();
      aInteraction.Print(logDebug, mManagerPtr->GetUtAtmosphere(), mSensorPtr, mTargetPtr);
      logDebug.AddNote() << "Target Irradiance: " << targetIrradiance << " W/cm^2";
   }
   return targetIrradiance;
}

// =================================================================================================
//! Compute the probability of detection for a specified target irradiance.
//! @param aTargetIrradiance [input] The irradiance on the sensor while looking at the target of interest. (W/cm^2)
//! @param aInteraction      [input] The interaction object representing the sensor and target state.
//! @returns The probability of detection in the range [0..1].
float SOSM_SensorTarget::ComputeProbabilityOfDetection(float aTargetIrradiance, SOSM_Interaction& aInteraction) const
{
   return mSensorPtr->ComputeProbabilityOfDetection(aTargetIrradiance, aInteraction);
}

// =================================================================================================
SOSM_SensorTarget::SnrAtmPair* SOSM_SensorTarget::CreateOrLoadSensorAtmosphereTables()
{
   std::string snrType(mSensorPtr->GetTypeName());
   std::string atmType(mSensorPtr->GetAtmosphere()->GetTypeName());
   std::string snrCachePrefix(mSensorPtr->GetCachePrefix());
   std::string atmCachePrefix(mSensorPtr->GetAtmosphere()->GetCachePrefix());
   std::string rootFileName;
   std::string cacheFileName;

   // ----------------------------------------------------------------------------------------------
   // First attempt to load tables from the cache.
   //
   // The cache files are considered current if they exist and are newer that the newest of the
   // sensor definition file or the associated atmosphere definition file.
   // ----------------------------------------------------------------------------------------------

   if (!mManagerPtr->IgnoreCacheFiles())
   {
      double sourceModificationTime =
         std::max(mSensorPtr->GetModificationTime(), mSensorPtr->GetAtmosphere()->GetModificationTime());

      bool cacheIsCurrent = true;
      rootFileName        = atmCachePrefix + ".ibr";
      cacheFileName       = mManagerPtr->CacheFileName(rootFileName, snrCachePrefix);
      if (SOSM_Utility::FileModificationTime(cacheFileName) <= sourceModificationTime)
      {
         cacheIsCurrent = false;
      }
      rootFileName  = atmCachePrefix + ".ifr";
      cacheFileName = mManagerPtr->CacheFileName(rootFileName, snrCachePrefix);
      if (SOSM_Utility::FileModificationTime(cacheFileName) <= sourceModificationTime)
      {
         cacheIsCurrent = false;
      }
      rootFileName  = atmCachePrefix + ".ift";
      cacheFileName = mManagerPtr->CacheFileName(rootFileName, snrCachePrefix);
      if (SOSM_Utility::FileModificationTime(cacheFileName) <= sourceModificationTime)
      {
         cacheIsCurrent = false;
      }

      if (cacheIsCurrent)
      {
         try
         {
            std::unique_ptr<SnrAtmPair> pairPtr(new SnrAtmPair(mManagerPtr));
            rootFileName  = atmCachePrefix + ".ibr";
            cacheFileName = mManagerPtr->CacheFileName(rootFileName, snrCachePrefix);
            pairPtr->mBackgroundRadiancePtr->LoadBinaryFile(cacheFileName);
            rootFileName  = atmCachePrefix + ".ifr";
            cacheFileName = mManagerPtr->CacheFileName(rootFileName, snrCachePrefix);
            pairPtr->mForegroundRadiancePtr->LoadBinaryFile(cacheFileName);
            rootFileName  = atmCachePrefix + ".ift";
            cacheFileName = mManagerPtr->CacheFileName(rootFileName, snrCachePrefix);
            pairPtr->mTransmittancePtr->LoadBinaryFile(cacheFileName);
            return pairPtr.release();
         }
         catch (std::exception& e)
         {
            auto logError = ut::log::error() << "Trying to read cache data for Sensor!";
            logError.AddNote() << "Sensor: " << snrType;
            logError.AddNote() << "Atmosphere: " << atmType;
            logError.AddNote() << "Message Error: " << e.what();
         }
      }
   }

   // ----------------------------------------------------------------------------------------------
   // The cached files do not exist, are out-of-date or have been requested to be ignored.
   // ----------------------------------------------------------------------------------------------
   if (mManagerPtr->ShowStatus())
   {
      auto logInfo = ut::log::info() << "Creating fast detection mode data for Sensor.";
      logInfo.AddNote() << "Sensor Type: " << snrType;
      logInfo.AddNote() << "Atmosphere Type: " << atmType;
   }
   std::unique_ptr<SnrAtmPair> pairPtr(new SnrAtmPair(mManagerPtr));

   // Get the altitude/elevation/range values for table generation
   std::vector<float> altValues;
   std::vector<float> elValues;
   std::vector<float> rangeValues;
   mSensorPtr->GetAtmosphere()->GetSamplePoints(altValues, elValues, rangeValues);

   SOSM_ScalarTable2D* bgrTablePtr = pairPtr->mBackgroundRadiancePtr;
   bgrTablePtr->IV1().SetValues(altValues);
   bgrTablePtr->IV2().SetValues(elValues);
   bgrTablePtr->DV().resize(altValues.size() * elValues.size());

   SOSM_ScalarTable3D* fgrTablePtr = pairPtr->mForegroundRadiancePtr;
   fgrTablePtr->IV1().SetValues(altValues);
   fgrTablePtr->IV2().SetValues(elValues);
   fgrTablePtr->IV3().SetValues(rangeValues);
   fgrTablePtr->DV().resize(altValues.size() * elValues.size() * rangeValues.size());

   SOSM_ScalarTable3D* fgtTablePtr = pairPtr->mTransmittancePtr;
   fgtTablePtr->IV1().SetValues(altValues);
   fgtTablePtr->IV2().SetValues(elValues);
   fgtTablePtr->IV3().SetValues(rangeValues);
   fgtTablePtr->DV().resize(altValues.size() * elValues.size() * rangeValues.size());

   SOSM_SimpleInteraction interaction;

   size_t aeIndex  = 0;
   size_t aerIndex = 0;
   for (size_t altIndex = 0; altIndex < altValues.size(); ++altIndex)
   {
      interaction.SetSensorAltitude(altValues[altIndex]);
      for (size_t elIndex = 0; elIndex < elValues.size(); ++elIndex)
      {
         interaction.SetAbsoluteTargetElevation(elValues[elIndex]);
         for (size_t rangeIndex = 0; rangeIndex < rangeValues.size(); ++rangeIndex)
         {
            interaction.SetSlantRange(rangeValues[rangeIndex]);
            float backgroundRadiance_xR;
            float foregroundRadiance_xR;
            float transmittance_xR;
            mSensorPtr->ComputeIntegratedAtmosphericData(interaction,
                                                         backgroundRadiance_xR,
                                                         foregroundRadiance_xR,
                                                         transmittance_xR);
            fgrTablePtr->DV()[aerIndex] = foregroundRadiance_xR;
            fgtTablePtr->DV()[aerIndex] = transmittance_xR;
            ++aerIndex;
            if (rangeIndex == 0)
            {
               bgrTablePtr->DV()[aeIndex] = backgroundRadiance_xR;
               ++aeIndex;
            }
         }
      }
   }

   // ----------------------------------------------------------------------------------------------
   // If requested, write the tables to the cache directory.
   // ----------------------------------------------------------------------------------------------

   if (mManagerPtr->WriteCacheFiles())
   {
      auto logInfo = ut::log::info() << "Writing cached fast detection mode data for Sensor.";
      logInfo.AddNote() << "Sensor: " << snrType;
      logInfo.AddNote() << "Atmosphere: " << atmType;
      rootFileName  = atmCachePrefix + ".ibr";
      cacheFileName = mManagerPtr->CacheFileName(rootFileName, snrCachePrefix);
      pairPtr->mBackgroundRadiancePtr->SaveBinaryFile(cacheFileName);
      rootFileName  = atmCachePrefix + ".ifr";
      cacheFileName = mManagerPtr->CacheFileName(rootFileName, snrCachePrefix);
      pairPtr->mForegroundRadiancePtr->SaveBinaryFile(cacheFileName);
      rootFileName  = atmCachePrefix + ".ift";
      cacheFileName = mManagerPtr->CacheFileName(rootFileName, snrCachePrefix);
      pairPtr->mTransmittancePtr->SaveBinaryFile(cacheFileName);
   }
   return pairPtr.release();
}

// =================================================================================================
SOSM_SensorTarget::SnrTgtPair* SOSM_SensorTarget::CreateOrLoadSensorTargetTables()
{
   std::string snrType(mSensorPtr->GetTypeName());
   std::string tgtType(mTargetPtr->GetTypeName());
   std::string snrCachePrefix(mSensorPtr->GetCachePrefix());
   ;
   std::string tgtCachePrefix(mTargetPtr->GetCachePrefix());
   std::string snrTgtCachePrefix(snrCachePrefix + '/' + tgtCachePrefix);

   std::string rootFileName;
   std::string cacheFileName;
   size_t      stateIndex;
   size_t      stateCount = mTargetPtr->StateCount();

   // ----------------------------------------------------------------------------------------------
   // First attempt to load tables from the cache.
   //
   // The cache files are considered current if they exist and are newer that the newest of the
   // sensor definition file or the target definition file.
   // ----------------------------------------------------------------------------------------------

   if (!mManagerPtr->IgnoreCacheFiles())
   {
      double sourceModificationTime = std::max(mSensorPtr->GetModificationTime(), mTargetPtr->GetModificationTime());

      bool cacheIsCurrent = true;
      for (stateIndex = 0; stateIndex < mTargetPtr->StateCount(); ++stateIndex)
      {
         std::string stateName = mTargetPtr->StateEntry(stateIndex)->mStateName;
         rootFileName          = stateName + ".ita";
         cacheFileName         = mManagerPtr->CacheFileName(rootFileName, snrTgtCachePrefix);
         if (SOSM_Utility::FileModificationTime(cacheFileName) <= sourceModificationTime)
         {
            cacheIsCurrent = false;
         }
         rootFileName  = stateName + ".iti";
         cacheFileName = mManagerPtr->CacheFileName(rootFileName, snrTgtCachePrefix);
         if (SOSM_Utility::FileModificationTime(cacheFileName) <= sourceModificationTime)
         {
            cacheIsCurrent = false;
         }
      }

      if (cacheIsCurrent)
      {
         try
         {
            std::unique_ptr<SnrTgtPair> pairPtr(new SnrTgtPair(mTargetPtr));
            for (stateIndex = 0; stateIndex < stateCount; ++stateIndex)
            {
               std::string stateName = mTargetPtr->StateEntry(stateIndex)->mStateName;
               rootFileName          = stateName + ".ita";
               cacheFileName         = mManagerPtr->CacheFileName(rootFileName, snrTgtCachePrefix);
               pairPtr->mProjectedAreaPtrs[stateIndex]->LoadBinaryFile(cacheFileName);
               rootFileName  = stateName + ".iti";
               cacheFileName = mManagerPtr->CacheFileName(rootFileName, snrTgtCachePrefix);
               pairPtr->mRadiantIntensityPtrs[stateIndex]->LoadBinaryFile(cacheFileName);
            }
            return pairPtr.release();
         }
         catch (std::exception& e)
         {
            auto logError = ut::log::error() << "Trying to read cache data for Sensor!";
            logError.AddNote() << "Sensor: " << snrType;
            logError.AddNote() << "Target Type: " << tgtType;
            logError.AddNote() << "Error Message: " << e.what();
         }
      }
   }

   // ----------------------------------------------------------------------------------------------
   // The cached files do not exist, are out-of-date or have been requested to be ignored.
   // ----------------------------------------------------------------------------------------------
   if (mManagerPtr->ShowStatus())
   {
      auto logInfo = ut::log::info() << "Creating fast detection mode data for Sensor.";
      logInfo.AddNote() << "Sensor Type: " << snrType;
      logInfo.AddNote() << "Target: " << tgtType;
   }
   std::unique_ptr<SnrTgtPair> pairPtr(new SnrTgtPair(mTargetPtr));

   SOSM_SimpleInteraction interaction;

   for (stateIndex = 0; stateIndex < stateCount; ++stateIndex)
   {
      interaction.mTargetStateIndex = stateIndex; // Select the target state

      // Populate the interaction object with the sample target altitude, speed and throttle for this state.
      float altitude;
      float speed;
      float throttle;
      mTargetPtr->GetSampleTargetState(interaction, altitude, speed, throttle);
      interaction.SetTargetAltitude(altitude);
      interaction.SetTargetSpeed(speed);
      interaction.SetTargetThrottle(throttle);

      // Get the azimuth and elevation sample points for this state.
      std::vector<float> azValues;
      std::vector<float> elValues;
      mTargetPtr->GetSamplePoints(interaction, azValues, elValues);

      SOSM_ScalarTable2D* intensityTablePtr = pairPtr->mRadiantIntensityPtrs[stateIndex];
      intensityTablePtr->IV1().SetValues(azValues);
      intensityTablePtr->IV2().SetValues(elValues);
      intensityTablePtr->DV().resize(azValues.size() * elValues.size());

      SOSM_ScalarTable2D* areaTablePtr = pairPtr->mProjectedAreaPtrs[stateIndex];
      areaTablePtr->IV1().SetValues(azValues);
      areaTablePtr->IV2().SetValues(elValues);
      areaTablePtr->DV().resize(azValues.size() * elValues.size());

      size_t aeIndex = 0;
      for (size_t azIndex = 0; azIndex < azValues.size(); ++azIndex)
      {
         for (size_t elIndex = 0; elIndex < elValues.size(); ++elIndex)
         {
            interaction.SetTargetToSensorAspect(azValues[azIndex], elValues[elIndex]);
            float radiantIntensity_xR;
            float projectedArea;
            mSensorPtr->ComputeIntegratedTargetData(mTargetPtr, interaction, radiantIntensity_xR, projectedArea);
            intensityTablePtr->DV()[aeIndex] = radiantIntensity_xR;
            areaTablePtr->DV()[aeIndex]      = projectedArea;
            ++aeIndex;
         }
      }
   }

   // ----------------------------------------------------------------------------------------------
   // If requested, write the tables to the cache directory.
   // ----------------------------------------------------------------------------------------------

   if (mManagerPtr->WriteCacheFiles())
   {
      for (stateIndex = 0; stateIndex < stateCount; ++stateIndex)
      {
         std::string stateName = mTargetPtr->StateEntry(stateIndex)->mStateName;
         auto        logInfo   = ut::log::info() << "Writing cached fast detection mode data for Sensor.";
         logInfo.AddNote() << "Sensor: " << snrType;
         logInfo.AddNote() << "Target: " << tgtType;
         logInfo.AddNote() << "State: " << stateName;
         rootFileName  = stateName + ".ita";
         cacheFileName = mManagerPtr->CacheFileName(rootFileName, snrTgtCachePrefix);
         pairPtr->mProjectedAreaPtrs[stateIndex]->SaveBinaryFile(cacheFileName);
         rootFileName  = stateName + ".iti";
         cacheFileName = mManagerPtr->CacheFileName(rootFileName, snrTgtCachePrefix);
         pairPtr->mRadiantIntensityPtrs[stateIndex]->SaveBinaryFile(cacheFileName);
      }
   }
   return pairPtr.release();
}

// =================================================================================================
//! Load precomputed sensor-target specific tables if permitted.
void SOSM_SensorTarget::LoadSensorTargetTables()
{
   mUsingTables = false;

   // Determine if the necessary tables exist (either computed or loaded from cache).

   std::string snrType(mSensorPtr->GetTypeName());
   std::string atmType(mSensorPtr->GetAtmosphere()->GetTypeName());
   std::string tgtType(mTargetPtr->GetTypeName());

   // First check the sensor-atmosphere tables.

   std::string                    key = snrType + '_' + atmType;
   SnrAtmTableMap::const_iterator sai = sSnrAtmTables.find(key);
   if (sai == sSnrAtmTables.end())
   {
      mSnrAtmPtr = CreateOrLoadSensorAtmosphereTables();
      if (mSnrAtmPtr != nullptr)
      {
         sSnrAtmTables[key] = mSnrAtmPtr;
      }
   }
   else
   {
      mSnrAtmPtr = sai->second;
   }

   // Next check the sensor-target tables.

   key                                = snrType + '_' + tgtType;
   SnrTgtTableMap::const_iterator sti = sSnrTgtTables.find(key);
   if (sti == sSnrTgtTables.end())
   {
      mSnrTgtPtr = CreateOrLoadSensorTargetTables();
      if (mSnrTgtPtr != nullptr)
      {
         sSnrTgtTables[key] = mSnrTgtPtr;
      }
   }
   else
   {
      mSnrTgtPtr = sti->second;
   }

   mUsingTables = ((mSnrAtmPtr != nullptr) && (mSnrTgtPtr != nullptr));
}

// =================================================================================================
SOSM_SensorTarget::SnrAtmPair::SnrAtmPair(SOSM_Manager* aManagerPtr)
{
   mBackgroundRadiancePtr = new SOSM_ScalarTable2D(aManagerPtr);
   mForegroundRadiancePtr = new SOSM_ScalarTable3D(aManagerPtr);
   mTransmittancePtr      = new SOSM_ScalarTable3D(aManagerPtr);
}

// =================================================================================================
SOSM_SensorTarget::SnrAtmPair::~SnrAtmPair()
{
   delete mBackgroundRadiancePtr;
   delete mForegroundRadiancePtr;
   delete mTransmittancePtr;
}

// =================================================================================================
SOSM_SensorTarget::SnrTgtPair::SnrTgtPair(SOSM_Target* aTargetPtr)
{
   size_t stateCount = aTargetPtr->StateCount();
   mProjectedAreaPtrs.resize(stateCount, nullptr);
   mRadiantIntensityPtrs.resize(stateCount, nullptr);
   for (size_t stateIndex = 0; stateIndex < stateCount; ++stateIndex)
   {
      mProjectedAreaPtrs[stateIndex]    = new SOSM_ScalarTable2D(aTargetPtr->GetManager());
      mRadiantIntensityPtrs[stateIndex] = new SOSM_ScalarTable2D(aTargetPtr->GetManager());
   }
}

// =================================================================================================
SOSM_SensorTarget::SnrTgtPair::~SnrTgtPair()
{
   for (size_t i = 0; i < mProjectedAreaPtrs.size(); ++i)
   {
      delete mProjectedAreaPtrs[i];
      delete mRadiantIntensityPtrs[i];
   }
}
