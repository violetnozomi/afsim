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

#include "SOSM_SimpleTarget.hpp"

#include <cmath>
#include <iostream>

#include "SOSM_BlackBody.hpp"
#include "SOSM_Interaction.hpp"
#include "SOSM_Manager.hpp"
#include "SOSM_Selector.hpp"
#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"

// =================================================================================================
SOSM_SimpleTarget::SOSM_SimpleTarget(SOSM_Manager* aManagerPtr)
   : SOSM_Target(aManagerPtr)
{
}

// =================================================================================================
// virtual
SOSM_Target* SOSM_SimpleTarget::Clone() const
{
   return new SOSM_SimpleTarget(*this);
}

// =================================================================================================
// Implementation of base class function (documented in SOSM_Target.hpp)
// virtual
void SOSM_SimpleTarget::ComputeRadiantIntensity(const SOSM_SpectralObject& aRequestor,
                                                SOSM_Interaction&          aInteraction,
                                                std::vector<float>&        aBodyIntensity,
                                                float&                     aBodyArea,
                                                std::vector<float>&        aPlumeIntensity,
                                                float&                     aPlumeArea)
{
   const ST_State* statePtr = static_cast<const ST_State*>(StateEntry(aInteraction.mTargetStateIndex));

   if (static_cast<int>(aBodyIntensity.size()) < aRequestor.Count())
   {
      aBodyIntensity.resize(aRequestor.Count());
   }
   if (static_cast<int>(aPlumeIntensity.size()) < aRequestor.Count())
   {
      aPlumeIntensity.resize(aRequestor.Count());
   }

   for (int i = 0; i < aRequestor.Count(); ++i)
   {
      aBodyIntensity[i]  = 0.0F;
      aPlumeIntensity[i] = 0.0F;
   }

   float azimuth;
   float elevation;
   aInteraction.GetTargetToSensorAspect(azimuth, elevation);
   azimuth = static_cast<float>(UtMath::NormalizeAngleMinusPi_Pi(azimuth));

   // For each component (cold or hot), compute the contributions from each and add to the sum.

   std::vector<float> radiantExitance(aRequestor.Count());

   // ----------------------------------------------------------------------------------------------
   // Compute the cold part contributions. If a constant radiant exitance was provided then it will
   // be used. Otherwise a blackbody model will be used where the temperature is determined using
   // an aerodynamic heading model.

   float hotPartArea  = statePtr->mConstantHotPartArea;
   float coldPartArea = statePtr->mConstantColdPartArea;
   if (statePtr->mColdPartAreaTablePtr)
   {
      TblLookupLU<float> azLookupCold;
      TblLookupLU<float> elLookupCold;
      statePtr->mColdPartAreaTablePtr->PolarLookup(azLookupCold, elLookupCold, azimuth, elevation);
      coldPartArea = statePtr->mColdPartAreaTablePtr->Interpolate(azLookupCold, elLookupCold);
      if (statePtr->mHotPartAreaFractionTablePtr)
      {
         TblLookupLU<float> azLookupHot;
         TblLookupLU<float> elLookupHot;
         statePtr->mHotPartAreaFractionTablePtr->PolarLookup(azLookupHot, elLookupHot, azimuth, elevation);
         float hotPartAreaFraction = statePtr->mHotPartAreaFractionTablePtr->Interpolate(azLookupHot, elLookupHot);
         hotPartArea               = coldPartArea * hotPartAreaFraction;
         coldPartArea -= hotPartArea;
      }
   }
   float bodyArea = coldPartArea;

   // Radiant exitance  M                                W/cm^2
   // Radiance          L = M / pi;                      W/cm^2/sr
   // Radiant Intensity I = L * area = M * (area / pi);  W/sr

   if (statePtr->mColdPartRadiantIntensity <= 0.0F)
   {
      float bodyTemperature = statePtr->mColdPartTemperature;
      if (bodyTemperature <= 0.0F)
      {
         bodyTemperature = BodyTemperature(aInteraction);
      }

      // Determine the spectral radiant exitance (M, W/cm^2/um),
      // convert spectral radiant intensity (W/sr/um) and add to total.
      SOSM_BlackBody::ComputeRadiantExitance(bodyTemperature, radiantExitance, aRequestor); // W/cm^2/um
      float conversionFactor = static_cast<float>((coldPartArea * 1.0E+4F) / UtMath::cPI);  // m^2 -> cm^2
      for (int i = 0; i < aRequestor.Count(); ++i)
      {
         aBodyIntensity[i] = radiantExitance[i] * conversionFactor; // W/sr/um
      }
   }
   else
   {
      float radiantIntensity = statePtr->mColdPartRadiantIntensity; // W/sr
      float intensityFactor  = 1.0F / (aRequestor.UpperWavelength() - aRequestor.LowerWavelength());
      for (int i = 0; i < aRequestor.Count(); ++i)
      {
         aBodyIntensity[i] = radiantIntensity * intensityFactor; // W/sr/um
      }
   }

   // ==============================================================================================
   // Add in the hot part contributions (if specified). If a temperature was specified then use the
   // blackbody model. If a constant radiant exitance was provided the it will be used.

   if ((statePtr->mHotPartTemperature > 0.0F) || (statePtr->mHotPartRadiantIntensity > 0.0F))
   {
      if (statePtr->mHotPartAreaTablePtr)
      {
         TblLookupLU<float> azLookupHot;
         TblLookupLU<float> elLookupHot;
         statePtr->mHotPartAreaTablePtr->PolarLookup(azLookupHot, elLookupHot, azimuth, elevation);
         hotPartArea = statePtr->mHotPartAreaTablePtr->Interpolate(azLookupHot, elLookupHot);
      }
      bodyArea += hotPartArea;

      if (statePtr->mHotPartTemperature > 0.0F)
      {
         // Determine the spectral radiant exitance (M, W/cm^2/um),
         // convert spectral radiant intensity (W/sr/um) and add to total.
         SOSM_BlackBody::ComputeRadiantExitance(statePtr->mHotPartTemperature, radiantExitance, aRequestor); // W/cm^2/um
         float conversionFactor = static_cast<float>((hotPartArea * 1.0E+4F) / UtMath::cPI); // m^2 -> cm^2
         for (int i = 0; i < aRequestor.Count(); ++i)
         {
            aBodyIntensity[i] += (radiantExitance[i] * conversionFactor); // W/sr/um
         }
      }
      else
      {
         float radiantIntensity = statePtr->mHotPartRadiantIntensity; // W/sr
         float intensityFactor  = 1.0F / (aRequestor.UpperWavelength() - aRequestor.LowerWavelength());
         for (int i = 0; i < aRequestor.Count(); ++i)
         {
            aBodyIntensity[i] += radiantIntensity * intensityFactor; // W/sr/um
         }
      }
   }

   // ==============================================================================================
   // Add in the plume contributions (if specified). If a temperature was specified then use the
   // blackbody model. If a constant radiant exitance was provided the it will be used.

   float plumeArea = 0.0F;
   if ((statePtr->mPlumeTemperature > 0.0F) || (statePtr->mPlumeRadiantIntensity > 0.0F))
   {
      plumeArea = statePtr->mConstantPlumeArea;
      if (statePtr->mPlumeAreaTablePtr)
      {
         TblLookupLU<float> azLookupPlume;
         TblLookupLU<float> elLookupPlume;
         statePtr->mPlumeAreaTablePtr->PolarLookup(azLookupPlume, elLookupPlume, azimuth, elevation);
         plumeArea = statePtr->mPlumeAreaTablePtr->Interpolate(azLookupPlume, elLookupPlume);
      }

      if (statePtr->mPlumeTemperature > 0.0F)
      {
         // Determine the spectral radiant exitance (M, W/cm^2/um),
         // convert spectral radiant intensity (W/sr/um) and add to total.
         SOSM_BlackBody::ComputeRadiantExitance(statePtr->mPlumeTemperature, radiantExitance, aRequestor); // W/cm^2/sr
         float conversionFactor = static_cast<float>((plumeArea * 1.0E+4F) / UtMath::cPI); // m^2 -> cm^2
         for (int i = 0; i < aRequestor.Count(); ++i)
         {
            aPlumeIntensity[i] = (radiantExitance[i] * conversionFactor); // W/sr/um
         }
      }
      else
      {
         float radiantIntensity = statePtr->mPlumeRadiantIntensity; // W/sr
         float intensityFactor  = 1.0F / (aRequestor.UpperWavelength() - aRequestor.LowerWavelength());
         for (int i = 0; i < aRequestor.Count(); ++i)
         {
            aPlumeIntensity[i] = radiantIntensity * intensityFactor; // W/sr/um
         }
      }
   }

   aBodyArea  = bodyArea * 1.0E+4F; // m^2 -> cm^2
   aPlumeArea = plumeArea * 1.0E+4F;
}

// =================================================================================================
//! Return the sample points to be used for 'fast detection mode' table generation.
// virtual
void SOSM_SimpleTarget::GetSamplePoints(SOSM_Interaction&   aInteraction,
                                        std::vector<float>& aAzValues,
                                        std::vector<float>& aElValues)
{
   const float azValues[] = {0.0F,   5.0F,   10.0F,  15.0F,  20.0F,  25.0F,  30.0F,  35.0F,  40.0F,  45.0F,
                             50.0F,  55.0F,  60.0F,  65.0F,  70.0F,  75.0F,  80.0F,  85.0F,  90.0F,  95.0F,
                             100.0F, 105.0F, 110.0F, 115.0F, 120.0F, 125.0F, 130.0F, 135.0F, 140.0F, 145.0F,
                             150.0F, 155.0F, 160.0F, 165.0F, 170.0F, 175.0F, 180.0F};
   const float elValues[] = {-90.0F, -85.0F, -80.0F, -75.0F, -70.0F, -65.0F, -60.0F, -55.0F, -50.0F, -45.0F,
                             -40.0F, -35.0F, -30.0F, -25.0F, -20.0F, -15.0F, -10.0F, -5.0F,  0.0F,   5.0F,
                             10.0F,  15.0F,  20.0F,  25.0F,  30.0F,  35.0F,  40.0F,  45.0F,  50.0F,  55.0F,
                             60.0F,  65.0F,  70.0F,  75.0F,  80.0F,  85.0F,  90.0F};

   size_t azCount = sizeof(azValues) / sizeof(float);
   aAzValues.resize(azCount);
   for (size_t azIndex = 0; azIndex < azCount; ++azIndex)
   {
      aAzValues[azIndex] = azValues[azIndex] * static_cast<float>(UtMath::cRAD_PER_DEG);
   }

   size_t elCount = sizeof(elValues) / sizeof(float);
   aElValues.resize(elCount);
   for (size_t elIndex = 0; elIndex < elCount; ++elIndex)
   {
      aElValues[elIndex] = elValues[elIndex] * static_cast<float>(UtMath::cRAD_PER_DEG);
   }
}

// =================================================================================================
//! Get the sample target state (altitude, speed, throttle) (for fast detection mode table generation).
// virtual
void SOSM_SimpleTarget::GetSampleTargetState(SOSM_Interaction& aInteraction, float& aAltitude, float& aSpeed, float& aThrottle)
{
   const ST_State* statePtr = static_cast<const ST_State*>(StateEntry(aInteraction.mTargetStateIndex));
   if ((statePtr->mSampleAltitude < 0.0F) || (statePtr->mSampleSpeed < 0.0F))
   {
      std::string msg = "Target type: " + GetTypeName() + ", state: " + statePtr->mStateName +
                        ": sample_altitude and sample_speed must be provided";
      throw UtException(msg);
   }

   aAltitude = statePtr->mSampleAltitude;
   aSpeed    = statePtr->mSampleSpeed;
   if (statePtr->mSampleSpeedIsMach)
   {
      mMutex.lock(); // UtAtmosphere is not thread-safe
      aSpeed = static_cast<float>(aSpeed * mAtmosphere.SonicVelocity(aAltitude));
      mMutex.unlock();
   }
   aThrottle = statePtr->mSampleThrottle;
   if (aThrottle < 0.0F)
   {
      aThrottle = 0.5F * (statePtr->mMaxThrottle + statePtr->mMinThrottle);
   }
}

// =================================================================================================
//! Compute the body temperature due to aerodynamic heating.
//!
//! @param aInteraction The interaction object from which to get the target state.
//! @returns The body temperature (deg-K).
float SOSM_SimpleTarget::BodyTemperature(SOSM_Interaction& aInteraction)
{
   float altitude = aInteraction.GetTargetAltitude();
   float speed    = aInteraction.GetTargetSpeed();

   mMutex.lock(); // UtAtmosphere is not thread-safe
   float ambientTemperature = static_cast<float>(mAtmosphere.Temperature(altitude));
   float sonicVelocity      = static_cast<float>(mAtmosphere.SonicVelocity(altitude));
   mMutex.unlock();

   // Compute the scaling factor to convert ambient to stagnation temperature.
   // Reference 1, equation 3-8.

   float           mach              = speed / sonicVelocity;
   const ST_State* statePtr          = static_cast<const ST_State*>(StateEntry(aInteraction.mTargetStateIndex));
   float stagnationTemperatureFactor = 1.0F + (statePtr->mRecoveryFactor * 0.5F * (statePtr->mGamma - 1.0F) * mach * mach);

   return ambientTemperature * stagnationTemperatureFactor;
}

// =================================================================================================
//! Called by the base class to create an instance of State specific to our model.
// protected virtual
SOSM_Target::StateBase* SOSM_SimpleTarget::CreateState(const std::string& aStateName) const
{
   return new ST_State(aStateName);
}

// =================================================================================================
SOSM_SimpleTarget::ST_State::ST_State(const std::string& aStateName)
   : StateBase(aStateName)
   , mColdPartAreaTablePtr()
   , mHotPartAreaTablePtr()
   , mHotPartAreaFractionTablePtr()
   , mPlumeAreaTablePtr()
   , mConstantColdPartArea(0.0F)
   , mRecoveryFactor(0.85F)
   , mGamma(1.4F)
   , mColdPartTemperature(0.0F)
   , mColdPartRadiantIntensity(0.0F)
   , mConstantHotPartArea(0.0F)
   , mHotPartTemperature(0.0F)
   , mHotPartRadiantIntensity(0.0F)
   , mConstantPlumeArea(0.0F)
   , mPlumeTemperature(0.0F)
   , mPlumeRadiantIntensity(0.0F)
   , mSampleAltitude(-1.0F)
   , mSampleSpeed(-1.0F)
   , mSampleThrottle(-1.0F)
   , mSampleSpeedIsMach(false)
{
}

// =================================================================================================
// virtual
void SOSM_SimpleTarget::ST_State::InputComplete(SOSM_Target& aTarget)
{
   // Make sure a cold part area of some sort exists and there is no conflicting definition.
   if ((mConstantColdPartArea <= 0.0F) && !mColdPartAreaTablePtr)
   {
      throw UtException("cold_part_area must be specified");
   }
   else if ((mConstantColdPartArea > 0.0) && (mColdPartAreaTablePtr))
   {
      throw UtException("cold_part_area table and constant cannot both be specified");
   }

   if (mHotPartTemperature > 0.0)
   {
      // Make sure a hot part area of some sort exists and there is no conflicting definition.
      if (!mHotPartAreaFractionTablePtr)
      {
         if ((mConstantHotPartArea <= 0.0F) && !mHotPartAreaTablePtr)
         {
            throw UtException("hot_part_area must be specified");
         }
         else if ((mConstantHotPartArea > 0.0) && (mHotPartAreaTablePtr))
         {
            throw UtException("hot_part_area table and constant cannot both be specified");
         }
      }
      else if (!mColdPartAreaTablePtr)
      {
         throw UtException("cold_part_area table must be specified if hot_part_area_fraction is specified");
      }
   }
   else
   {
      if ((mConstantHotPartArea > 0.0F) || (mHotPartAreaTablePtr))
      {
         throw UtException("hot_part_area must be omitted if hot_part_temperature omitted");
      }
      // Don't check hot_part_area_fraction as it could be used for a pure cold case.
   }
}

// =================================================================================================
// virtual
bool SOSM_SimpleTarget::ST_State::ProcessInput(UtInput& aInput, SOSM_Target& aTarget)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "cold_part_area")
   {
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      if (fileName == "constant")
      {
         aInput.ReadValueOfType(mConstantColdPartArea, UtInput::cAREA);
         aInput.ValueGreater(mConstantColdPartArea, 0.0F);
      }
      else
      {
         fileName = aInput.LocateFile(fileName);
         SOSM_TableVar dvInfo("area", 0.0F, 1.0E+37F, 1.0F);
         mColdPartAreaTablePtr =
            SOSM_ScalarTable2D::LoadSharedPolarTable(aTarget.GetManager(), fileName, aTarget.GetCachePrefix(), dvInfo);
      }
   }
   else if (command == "recovery_factor")
   {
      aInput.ReadValue(mRecoveryFactor);
      aInput.ValueGreater(mRecoveryFactor, 0.0F);
      aInput.ValueLessOrEqual(mRecoveryFactor, 1.0F);
   }
   else if (command == "gamma")
   {
      aInput.ReadValue(mGamma);
      aInput.ValueGreater(mGamma, 0.0F);
   }
   else if (command == "cold_part_temperature")
   {
      aInput.ReadValueOfType(mColdPartTemperature, UtInput::cTEMPERATURE);
      aInput.ValueGreaterOrEqual(mColdPartTemperature, 0.0F); // A value of zero reverts to blackbody form
      if (mColdPartTemperature > 0.0F)
      {
         mColdPartRadiantIntensity = 0.0F;
      }
   }
   else if (command == "cold_part_radiant_intensity")
   {
      ReadRadiantIntensity(aInput, mColdPartRadiantIntensity);
      if (mColdPartRadiantIntensity > 0.0F)
      {
         mColdPartTemperature = 0.0F;
      }
   }
   else if (command == "hot_part_area")
   {
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      if (fileName == "constant")
      {
         aInput.ReadValueOfType(mConstantHotPartArea, UtInput::cAREA);
         aInput.ValueGreater(mConstantHotPartArea, 0.0F);
      }
      else
      {
         fileName = aInput.LocateFile(fileName);
         SOSM_TableVar dvInfo("area", 0.0F, 1.0E+37F, 1.0F);
         mHotPartAreaTablePtr =
            SOSM_ScalarTable2D::LoadSharedPolarTable(aTarget.GetManager(), fileName, aTarget.GetCachePrefix(), dvInfo);
      }
   }
   else if (command == "hot_part_area_fraction")
   {
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName = aInput.LocateFile(fileName);
      SOSM_TableVar dvInfo("area_fraction", 0.0F, 1.0F, 1.0F);
      mHotPartAreaFractionTablePtr =
         SOSM_ScalarTable2D::LoadSharedPolarTable(aTarget.GetManager(), fileName, aTarget.GetCachePrefix(), dvInfo);
   }
   else if (command == "hot_part_temperature")
   {
      aInput.ReadValueOfType(mHotPartTemperature, UtInput::cTEMPERATURE);
      aInput.ValueGreater(mHotPartTemperature, 0.0F);
      if (mHotPartTemperature > 0.0F)
      {
         mHotPartRadiantIntensity = 0.0F;
      }
   }
   else if (command == "hot_part_radiant_intensity")
   {
      ReadRadiantIntensity(aInput, mHotPartRadiantIntensity);
      if (mHotPartRadiantIntensity > 0.0F)
      {
         mHotPartTemperature = 0.0F;
      }
   }
   else if (command == "plume_area")
   {
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      if (fileName == "constant")
      {
         aInput.ReadValueOfType(mConstantPlumeArea, UtInput::cAREA);
         aInput.ValueGreater(mConstantPlumeArea, 0.0F);
      }
      else
      {
         fileName = aInput.LocateFile(fileName);
         SOSM_TableVar dvInfo("area", 0.0F, 1.0E+37F, 1.0F);
         mPlumeAreaTablePtr =
            SOSM_ScalarTable2D::LoadSharedPolarTable(aTarget.GetManager(), fileName, aTarget.GetCachePrefix(), dvInfo);
      }
   }
   else if (command == "plume_temperature")
   {
      aInput.ReadValueOfType(mPlumeTemperature, UtInput::cTEMPERATURE);
      aInput.ValueGreater(mPlumeTemperature, 0.0F);
      if (mPlumeTemperature > 0.0F)
      {
         mPlumeRadiantIntensity = 0.0F;
      }
   }
   else if (command == "plume_radiant_intensity")
   {
      ReadRadiantIntensity(aInput, mPlumeRadiantIntensity);
      if (mPlumeRadiantIntensity > 0.0F)
      {
         mPlumeTemperature = 0.0F;
      }
   }
   else if (command == "sample_altitude")
   {
      aInput.ReadValueOfType(mSampleAltitude, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mSampleAltitude, 0.0F);
   }
   else if (command == "sample_speed")
   {
      aInput.ReadValueOfType(mSampleSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mSampleSpeed, 0.0F);
      mSampleSpeedIsMach = false;
   }
   else if (command == "sample_mach")
   {
      aInput.ReadValue(mSampleSpeed);
      aInput.ValueGreaterOrEqual(mSampleSpeed, 0.0F);
      mSampleSpeedIsMach = true;
   }
   else if (command == "sample_throttle")
   {
      aInput.ReadValue(mSampleThrottle);
      aInput.ValueInClosedRange(mSampleThrottle, 0.0F, 1.0F);
   }
   else
   {
      myCommand = StateBase::ProcessInput(aInput, aTarget);
   }
   return myCommand;
}

// =================================================================================================
void SOSM_SimpleTarget::ST_State::ReadRadiantIntensity(UtInput& aInput, float& aValue)
{
   std::string units;
   aInput.ReadValue(aValue);
   aInput.ReadValue(units);
   aInput.ValueGreaterOrEqual(aValue, 0.0F); // A value of zero reverts to blackbody form

   // Parse the units.
   std::string::size_type slashPos = units.find('/');
   if ((slashPos != std::string::npos) && (slashPos != 0) && ((slashPos + 1) != units.size()))
   {
      std::string powerUnits(units.substr(0, slashPos));
      std::string angleUnits(units.substr(slashPos + 1));
      double      powerFactor = aInput.ConvertValue(1.0, powerUnits, UtInput::cPOWER);
      double      angleFactor = aInput.ConvertValue(1.0, angleUnits, UtInput::cSOLID_ANGLE);
      double      multiplier  = powerFactor / angleFactor;
      aValue                  = static_cast<float>(aValue * multiplier);
   }
   else
   {
      throw UtInput::BadValue(aInput, "Unknown intensity units: " + units);
   }
}
