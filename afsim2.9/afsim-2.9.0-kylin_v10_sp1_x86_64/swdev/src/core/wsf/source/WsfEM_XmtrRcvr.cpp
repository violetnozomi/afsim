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

#include "WsfEM_XmtrRcvr.hpp"

#include <algorithm>
#include <cassert>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Attenuation.hpp"
#include "WsfEM_AttenuationTypes.hpp"
#include "WsfEM_Propagation.hpp"
#include "WsfEM_PropagationTypes.hpp"
#include "WsfEM_Util.hpp"
#include "WsfMode.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace
{
double cUNDEFINED = 1.0E+30;
}

// =================================================================================================
WsfEM_XmtrRcvr::WsfEM_XmtrRcvr(WsfEM_Antenna* aGeometryPtr /* = 0 */)
   : mAttenuationType()
   , mPropagationType()
   , mAttenuationPtr(nullptr)
   , mPropagationPtr(nullptr)
   , mModePtr(nullptr)
   , mAntennaPtr(aGeometryPtr)
   , mAssociatePtr(nullptr)
   , mBeamTilt(cUNDEFINED)
   , mEarthRadiusMultiplier(1.0)
   , mFrequency(0.0)
   , mBandwidth(0.0)
   , mInternalLoss(1.0)
   , mPolarization(WsfEM_Types::cPOL_DEFAULT)
   , mIndex(0)
   , mDebugEnabled(false)
   , mCheckMasking(true)
   , mExplicitBandwidth(false)
   , mAttenuationInputEnabled(true)
   , mPropagationInputEnabled(true)
{
   // By default, horizon and terrain checking are enabled for transmitters and receivers
   mTerrainMaskingMode = WsfEM_Interaction::cRCVR_HORIZON_MASKING | WsfEM_Interaction::cRCVR_TERRAIN_MASKING |
                         WsfEM_Interaction::cXMTR_HORIZON_MASKING | WsfEM_Interaction::cXMTR_TERRAIN_MASKING;
}

// =================================================================================================
WsfEM_XmtrRcvr::WsfEM_XmtrRcvr(const WsfEM_XmtrRcvr& aSrc, WsfEM_Antenna* aGeometryPtr)
   : WsfAuxDataEnabled(aSrc)
   , mAttenuationType(aSrc.mAttenuationType)
   , mPropagationType(aSrc.mPropagationType)
   , mAttenuationPtr(nullptr)
   , mPropagationPtr(nullptr)
   , mModePtr(nullptr)
   , mAntennaPtr(aGeometryPtr)
   , mPatternTables(aSrc.mPatternTables)
   , mAssociatePtr(nullptr)
   , mBeamTilt(aSrc.mBeamTilt)
   , mEarthRadiusMultiplier(aSrc.mEarthRadiusMultiplier)
   , mFrequency(aSrc.mFrequency)
   , mBandwidth(aSrc.mBandwidth)
   , mInternalLoss(aSrc.mInternalLoss)
   , mPolarization(aSrc.mPolarization)
   , mIndex(aSrc.mIndex)
   , mUsedExplicitPolarization(aSrc.mUsedExplicitPolarization)
   , mDebugEnabled(aSrc.mDebugEnabled)
   , mTerrainMaskingMode(aSrc.mTerrainMaskingMode)
   , mCheckMasking(aSrc.mCheckMasking)
   , mExplicitBandwidth(aSrc.mExplicitBandwidth)
   , mAttenuationInputEnabled(aSrc.mAttenuationInputEnabled)
   , mPropagationInputEnabled(aSrc.mPropagationInputEnabled)
{
   for (auto& table : mPatternTables)
   {
      for (auto& pattern : table)
      {
         pattern.first = pattern.first->Clone();
      }
   }
}

// =================================================================================================
// virtual
WsfEM_XmtrRcvr::~WsfEM_XmtrRcvr()
{
   delete mAttenuationPtr;
   delete mPropagationPtr;
   for (const auto& table : mPatternTables)
   {
      for (const auto& pattern : table)
      {
         delete pattern.first;
      }
   }
}

// =================================================================================================
//! Indicate that two systems (typically a transmitter/receiver pair) are 'associated'.
//! Certain parameters that haven't been defined in one system are automatically copied
//! from the associated system during initialization.
// static
void WsfEM_XmtrRcvr::AssociateSystems(WsfEM_XmtrRcvr* aSystem1Ptr, WsfEM_XmtrRcvr* aSystem2Ptr)
{
   aSystem1Ptr->mAssociatePtr = aSystem2Ptr;
   aSystem2Ptr->mAssociatePtr = aSystem1Ptr;
}

// =================================================================================================
//! Convenience method to get the pointer to the associated platform.
//! @note This is valid only after initialization.
WsfPlatform* WsfEM_XmtrRcvr::GetPlatform() const
{
   return (mAntennaPtr ? mAntennaPtr->GetPlatform() : nullptr);
}

// =================================================================================================
//! Convenience method to get the pointer to the associated articulated/platform part.
//! @note This is valid only after initialization.
WsfArticulatedPart* WsfEM_XmtrRcvr::GetArticulatedPart() const
{
   return (mAntennaPtr ? mAntennaPtr->GetArticulatedPart() : nullptr);
}

// =================================================================================================
bool WsfEM_XmtrRcvr::Initialize(WsfSimulation& aSimulation)
{
   bool ok = true;
   if (mAntennaPtr == nullptr)
   {
      ut::log::error() << "A transmitter/receiver must have an associated WsfEM_Antenna.";
      ok = false;
   }
   else if (mAntennaPtr->GetArticulatedPart() == nullptr)
   {
      ut::log::error() << "A WsfEM_Antenna must have an associated articulated part.";
      ok = false;
   }

   // Check to see if the unique id is valid and if not set it
   // Note: This check is necessary as this class may be initialized multiple times
   //       for the same object to re-verify this object.
   if (GetUniqueId() == 0)
   {
      AssignUniqueId(&aSimulation); // In WsfUniqueId
   }

   // Set any unassigned values from the associated system.
   if (mAssociatePtr != nullptr)
   {
      // If I have not defined ANY antenna patterns, copy them from my associated system.

      bool copyTables = true;
      for (const auto& table : mPatternTables)
      {
         if (!table.empty())
         {
            copyTables = false;
            break;
         }
      }
      if (copyTables)
      {
         mPatternTables = mAssociatePtr->mPatternTables;
         for (auto& table : mPatternTables)
         {
            for (auto& pattern : table)
            {
               pattern.first = pattern.first->Clone();
            }
         }
         mUsedExplicitPolarization = mAssociatePtr->mUsedExplicitPolarization;
      }

      if ((mBeamTilt == cUNDEFINED) && (mAssociatePtr->mBeamTilt != cUNDEFINED))
      {
         mBeamTilt = mAssociatePtr->mBeamTilt;
      }
   }

   if (mBeamTilt == cUNDEFINED)
   {
      mBeamTilt = 0.0;
   }

   // Ensure that each non-default polarization, that did not have a pattern explicitly
   // declared, uses a default pattern for all polarizations and all frequencies
   // Start with the HORIZONTAL Polarization and loop through all of the others
   for (unsigned int i = 1; i < WsfEM_Types::cPOL_COUNT; ++i)
   {
      if (mPatternTables[i].empty())
      {
         mPatternTables[i] = mPatternTables[WsfEM_Types::cPOL_DEFAULT];
         for (auto& pattern : mPatternTables[i])
         {
            pattern.first = pattern.first->Clone();
         }
      }
   }
   for (const auto& table : mPatternTables)
   {
      for (const auto& pattern : table)
      {
         pattern.first->Initialize(&aSimulation);
      }
   }

   // Load and initialize the attenuation model object.

   if (!mAttenuationType.IsNull())
   {
      mAttenuationPtr = WsfEM_AttenuationTypes::Get(aSimulation.GetScenario()).Clone(mAttenuationType);
      if (mAttenuationPtr == nullptr)
      {
         auto out = ut::log::error() << "Undefined attenuation_model.";
         out.AddNote() << "Model: " << mAttenuationType;
         ok = false;
      }
      else if (mAttenuationPtr->IsNullModel())
      {
         // The referenced model is a 'null' (no-effect) model - delete it to save processing time.
         delete mAttenuationPtr;
         mAttenuationPtr = nullptr;
      }
      else if (!mAttenuationPtr->Initialize(this))
      {
         ut::log::error() << "Failed to initialize attenuation_model.";
         ok = false;
      }
   }

   // Load and initialize the propagation model object.

   if (!mPropagationType.IsNull())
   {
      mPropagationPtr = WsfEM_PropagationTypes::Get(aSimulation.GetScenario()).Clone(mPropagationType);
      if (mPropagationPtr == nullptr)
      {
         auto out = ut::log::error() << "Undefined propagation_model.";
         out.AddNote() << "Model: " << mPropagationType;
         ok = false;
      }
      else if (mPropagationPtr->IsNullModel())
      {
         // The referenced model is a 'null' (no-effect) model - delete it to save processing time.
         delete mPropagationPtr;
         mPropagationPtr = nullptr;
      }
      else if (!mPropagationPtr->Initialize(this))
      {
         ut::log::error() << "Failed to initialize propagation_model.";
         ok = false;
      }
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfEM_XmtrRcvr::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   // We load a temp value to find what category the command is, then
   // we can verify it's not a replicated item for this part.
   WsfStringId temp;

   // Allow embedded antenna input if an antenna is attached.
   if (command == "antenna_pattern")
   {
      std::string patternType;
      aInput.ReadValue(patternType);
      WsfAntennaPattern* antennaPatternPtr = WsfScenario::FromInput(aInput).CloneTypeT<WsfAntennaPattern>(patternType);
      if (antennaPatternPtr == nullptr)
      {
         throw UtInput::BadValue(aInput);
      }

      ClearAntennaPatterns();

      // add antenna to list with valid frequency default of 0.0
      AddAntennaPattern(antennaPatternPtr, WsfEM_Types::cPOL_DEFAULT, 0.0);
   }
   else if (command == "antenna_pattern_table")
   {
      UtInputBlock block(aInput, "end_antenna_pattern_table");
      ClearAntennaPatterns();
      WsfEM_Types::Polarization polarization = WsfEM_Types::cPOL_DEFAULT;

      while (block.ReadCommand(command))
      {
         if (command == "frequency")
         {
            double frequency = -1;
            block.GetInput().ReadValueOfType(frequency, UtInput::cFREQUENCY);
            if (frequency < 0.0)
            {
               throw UtInput::BadValue(aInput);
            }
            std::string patternType;
            block.GetInput().ReadValue(patternType);
            WsfAntennaPattern* antennaPatternPtr =
               WsfScenario::FromInput(aInput).CloneTypeT<WsfAntennaPattern>(patternType);
            if (antennaPatternPtr == nullptr)
            {
               throw UtInput::BadValue(aInput);
            }

            if (!AddAntennaPattern(antennaPatternPtr, polarization, frequency))
            {
               throw UtInput::BadValue(aInput, "Antenna pattern previously defined for frequency.");
            }
         }
         else if (command == "polarization")
         {
            polarization = WsfEM_Types::cPOL_DEFAULT;

            // If a polarization is not specified, the default is equivalent to 'polarization default'
            std::string polarizationStr;
            aInput.ReadValue(polarizationStr);
            if (!WsfEM_Util::StringToEnum(polarization, polarizationStr))
            {
               throw UtInput::BadValue(aInput, "Invalid polarization: " + polarizationStr);
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      if (mPatternTables[WsfEM_Types::cPOL_DEFAULT].empty())
      {
         throw UtInput::BadValue(aInput, "A polarization of 'default' must be included in an antenna_pattern_table.");
      }
   }
   else if (command == "beam_tilt")
   {
      double beamTilt;
      aInput.ReadValueOfType(beamTilt, UtInput::cANGLE);
      aInput.ValueInClosedRange(beamTilt, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      SetBeamTilt(beamTilt);
   }
   else if ((mAntennaPtr) && mAntennaPtr->ProcessInput(aInput))
   {
   }
   else if (command == "bandwidth")
   {
      double bandwidth;
      aInput.ReadValueOfType(bandwidth, UtInput::cFREQUENCY);
      aInput.ValueGreaterOrEqual(bandwidth, 0.0);
      SetBandwidth(bandwidth);
   }
   else if (command == "earth_radius_multiplier")
   {
      double earthRadiusMultiplier;
      aInput.ReadValue(earthRadiusMultiplier);
      aInput.ValueGreater(earthRadiusMultiplier, 0.0);
      SetEarthRadiusMultiplier(earthRadiusMultiplier);
   }
   else if (command == "effective_earth_radius")
   {
      double effectiveEarthRadius;
      aInput.ReadValueOfType(effectiveEarthRadius, UtInput::cLENGTH);
      aInput.ValueGreater(effectiveEarthRadius, 0.0);
      SetEarthRadiusMultiplier(effectiveEarthRadius / UtSphericalEarth::cEARTH_RADIUS);
   }
   else if (command == "frequency")
   {
      double frequency;
      aInput.ReadValueOfType(frequency, UtInput::cFREQUENCY);
      aInput.ValueGreater(frequency, 0.0);
      SetFrequency(frequency);
   }
   else if (command == "wavelength")
   {
      double wavelength;
      aInput.ReadValueOfType(wavelength, UtInput::cLENGTH);
      aInput.ValueGreater(wavelength, 0.0);
      SetFrequency(UtMath::cLIGHT_SPEED / wavelength);
   }
   else if (command == "internal_loss")
   {
      double internalLoss;
      aInput.ReadValueOfType(internalLoss, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(internalLoss, 1.0);
      SetInternalLoss(internalLoss);
   }
   else if (command == "polarization")
   {
      WsfEM_Types::Polarization polarization;
      std::string               polarizationStr;
      aInput.ReadValue(polarizationStr);
      if (!WsfEM_Util::StringToEnum(polarization, polarizationStr))
      {
         throw UtInput::BadValue(aInput, "Invalid polarization: " + polarizationStr);
      }
      SetPolarization(polarization);
   }
   else if (mAttenuationInputEnabled &&
            WsfEM_AttenuationTypes::Get(WsfScenario::FromInput(aInput)).LoadReference(aInput, temp))
   {
      mAttenuationType = temp;
   }
   else if (mPropagationInputEnabled &&
            WsfEM_PropagationTypes::Get(WsfScenario::FromInput(aInput)).LoadReference(aInput, temp))
   {
      mPropagationType = temp;
   }
   else if (command == "check_terrain_masking")
   {
      aInput.ReadValue(mCheckMasking);
   }
   else if (command == "terrain_masking_mode")
   {
      std::string terrainMaskingMode;
      aInput.ReadValue(terrainMaskingMode);
      SetMaskingMode(terrainMaskingMode);
   }
   else if (WsfAuxDataEnabled::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
//! Update the state of the object to reflect its position as of the supplied time.
//! @param aSimTime The current simulation time.
// virtual
void WsfEM_XmtrRcvr::UpdatePosition(double aSimTime)
{
   assert(mAntennaPtr != nullptr);
   mAntennaPtr->UpdatePosition(aSimTime);
}

// =================================================================================================
//! Set the pointer to a new attenuation model.
//! This provides a means to define a new model at run-time. This may be called only after Initialize.
//! @param aAttenuationPtr Pointer to the attenuation model, which may be null. The model must be initialized.
//! Any existing model is deleted and this class assumes of the new model.
void WsfEM_XmtrRcvr::SetAttenuationModel(WsfEM_Attenuation* aAttenuationPtr)
{
   delete mAttenuationPtr;
   mAttenuationPtr = aAttenuationPtr;
}

// =================================================================================================
//! Set the pointer to the propagation model.
//! This provides a means to define a new model at run-time. This may be called only after Initialize.
//! @param aPropagationPtr Pointer to the propagation model, which may be null. The model must be initialized.
//! Any existing model is deleted and this class assumes of the new model.
void WsfEM_XmtrRcvr::SetPropagationModel(WsfEM_Propagation* aPropagationPtr)
{
   delete mPropagationPtr;
   mPropagationPtr = aPropagationPtr;
}

// =================================================================================================
//! Return the antenna pattern for the supplied polarization and frequency.
//! @param  aPolarization The polarization of the signal.
//! @param  aFrequency   The frequency of the signal.
//! @return Pointer to the antenna gain pattern or nullptr if there is no associated pattern.
WsfAntennaPattern* WsfEM_XmtrRcvr::GetAntennaPattern(WsfEM_Types::Polarization aPolarization, double aFrequency) const
{
   WsfAntennaPattern* antennaPatternPtr = nullptr;

   const auto& table = mPatternTables[aPolarization];
   if (!table.empty())
   {
      // antenna patterns sorted by increasing frequency
      auto patternIter = std::upper_bound(std::begin(table),
                                          std::end(table),
                                          aFrequency,
                                          [](const double aFrequency, const AntennaPatternList::value_type& aPattern)
                                          { return (aFrequency < aPattern.second); });

      // Frequency in vector is lower bound of antenna pattern
      // frequency validity range.
      if (patternIter != table.begin())
      {
         std::advance(patternIter, -1);
      }
      antennaPatternPtr = patternIter->first;
   }
   return antennaPatternPtr;
}

// =================================================================================================
//! Add the antenna pattern to the list with the associated lower frequency bound.
//! @param aAntennaPatternPtr Pointer to the antenna pattern.
//! @param aFrequency The lower frequency bound of the antenna pattern.
//! @param aPolarization The polarization of the antenna pattern. Defaults to DEFAULT
//! @returns True if the insert was successful, False if a pattern associated with the frequency already exists.
bool WsfEM_XmtrRcvr::AddAntennaPattern(WsfAntennaPattern*        aAntennaPatternPtr,
                                       WsfEM_Types::Polarization aPolarization,
                                       double                    aFrequency)
{
   auto& table       = mPatternTables[aPolarization];
   auto  patternIter = std::lower_bound(std::begin(table),
                                       std::end(table),
                                       aFrequency,
                                       [](const AntennaPatternList::value_type& aPattern, const double aFrequency)
                                       { return (aPattern.second < aFrequency); });

   if ((patternIter == std::end(table)) || (patternIter->second != aFrequency))
   {
      table.emplace(patternIter, aAntennaPatternPtr, aFrequency);
      if (aPolarization != WsfEM_Types::cPOL_DEFAULT)
      {
         mUsedExplicitPolarization[aPolarization] = true;
      }
      return true;
   }

   return false;
}

// =================================================================================================
//! Clear current list of antenna patterns and add the antenna pattern to
//! the list with the associated polarization and lower frequency bound.
//!
//! @param aAntennaPatternPtr Pointer to the antenna pattern.
//! @param aFrequency The lower frequency bound of the antenna pattern.
//! @param aPolarization The polarization of the antenna pattern. Defaults to DEFAULT
//! @returns True if the insert was successful, False if a pattern associated with the frequency already exists.
bool WsfEM_XmtrRcvr::SetAntennaPattern(WsfAntennaPattern*        aAntennaPatternPtr,
                                       WsfEM_Types::Polarization aPolarization,
                                       double                    aFrequency)
{
   ClearAntennaPatterns();
   return AddAntennaPattern(aAntennaPatternPtr, aPolarization, aFrequency);
}

// =================================================================================================
//! Return the list of antenna patterns for a specified polarization.
//! @param aPolarization The polarization for whose antenna patterns are being requested.
//! @returns The pointer to the vector of the associated antenna pattern list.
//! @note The return value is zero for non-default polarization if no pattern was explicitly loaded.
//! @note This method was created to allow External Services access to the antenna patterns. It should not be considered
//! part of the public interface as it is exposing data that was once 'private'. Use at your own risk.
const WsfEM_XmtrRcvr::AntennaPatternList* WsfEM_XmtrRcvr::GetAntennaPatternList(WsfEM_Types::Polarization aPolarization) const
{
   // Always return list for default polarization.
   // For other polarizations, return list only if explicitly loaded.
   if (aPolarization == WsfEM_Types::cPOL_DEFAULT || mUsedExplicitPolarization[aPolarization])
   {
      return &mPatternTables[aPolarization];
   }
   return nullptr;
}

// =================================================================================================
//! Get the antenna gain at the specified azimuth and elevation.
//! @param aPolarization The polarization of the signal being transmitted or received.
//! @param aFrequency    The frequency of the signal being transmitted or received (Hertz)
//! @param aTargetAz     The azimuth of the target with respect to the beam boresight (radians)
//! @param aTargetEl     The elevation of the target with respect to the beam boresight (radians)
//! @param aEBS_Az       The azimuth of the beam boresight with respect to the antenna face (radians).
//! (see note below).
//! @param aEBS_El       The elevation of the beam boresight with respect to the antenna face (radians).
//! (see note below).
//! @returns The antenna gain as an absolute value (not dBi).
//! @note The arguments 'aEBS_Az' and 'aEBS_El' are only used if electronic beam steering is used.
// virtual
double WsfEM_XmtrRcvr::GetAntennaGain(WsfEM_Types::Polarization aPolarization,
                                      double                    aFrequency,
                                      double                    aTargetAz,
                                      double                    aTargetEl,
                                      double                    aEBS_Az,
                                      double                    aEBS_El)
{
   double antennaGain = 1.0;

   WsfAntennaPattern* antennaPatternPtr = GetAntennaPattern(aPolarization, aFrequency);
   if (antennaPatternPtr != nullptr)
   {
      antennaGain = antennaPatternPtr->GetGain(aFrequency, aTargetAz, aTargetEl, aEBS_Az, aEBS_El);
      if (mAntennaPtr->GetEBS_Mode() != WsfEM_Antenna::cEBS_NONE)
      {
         // Apply any adjustment to account for ESA beam steering losses.
         antennaGain *= mAntennaPtr->ComputeBeamSteeringLoss(aEBS_Az, aEBS_El);

         // Don't let it fall below the minimum gain allowed.
         antennaGain = std::max(antennaGain, antennaPatternPtr->GetMinimumGain());
      }
   }
   return antennaGain;
}

// =================================================================================================
//! Get the azimuth beamwidth of the first antenna pattern found.
//! @param aEBS_Azimuth The Electronic Beamwidth Steering in terms of Azimuth (radians)
//! @param aEBS_Elevation The Electronic Beamwidth Steering in terms of Elevation (radians)
//! @return Azimuth beamwidth (radians)
// virtual
double WsfEM_XmtrRcvr::GetAzimuthBeamwidth(double aEBS_Azimuth, double aEBS_Elevation) const
{
   double             azBeamwidth       = 1.0 * UtMath::cRAD_PER_DEG;
   WsfAntennaPattern* antennaPatternPtr = GetAntennaPattern(WsfEM_Types::cPOL_DEFAULT, mFrequency);
   if (antennaPatternPtr != nullptr)
   {
      azBeamwidth = antennaPatternPtr->GetAzimuthBeamwidth(mFrequency, aEBS_Azimuth, aEBS_Elevation);
   }
   else
   {
      if (aEBS_Azimuth != 0.0)
      {
         double EBS_Effect = std::cos(aEBS_Azimuth);
         if (EBS_Effect > 0.0)
         {
            azBeamwidth /= EBS_Effect;
         }
      }
   }
   return azBeamwidth;
}

// =================================================================================================
//! Get the elevation beamwidth of the first antenna pattern found.
//! @param aEBS_Azimuth The Electronic Beamwidth Steering in terms of Azimuth (radians)
//! @param aEBS_Elevation The Electronic Beamwidth Steering in terms of Elevation (radians)
//! @return Elevation beamwidth (radians)
// virtual
double WsfEM_XmtrRcvr::GetElevationBeamwidth(double aEBS_Azimuth, double aEBS_Elevation) const
{
   double             elBeamwidth       = 1.0 * UtMath::cRAD_PER_DEG;
   WsfAntennaPattern* antennaPatternPtr = GetAntennaPattern(WsfEM_Types::cPOL_DEFAULT, mFrequency);
   if (antennaPatternPtr != nullptr)
   {
      elBeamwidth = antennaPatternPtr->GetElevationBeamwidth(mFrequency, aEBS_Azimuth, aEBS_Elevation);
   }
   else
   {
      if (aEBS_Elevation != 0.0)
      {
         double EBS_Effect = std::cos(aEBS_Elevation);
         if (EBS_Effect > 0.0)
         {
            elBeamwidth /= EBS_Effect;
         }
      }
   }
   return elBeamwidth;
}

// =================================================================================================
//! Return the peak antenna gain of the first antenna pattern found.
//! @return The peak antenna gain (absolute ratio, NOT dB)
// virtual
double WsfEM_XmtrRcvr::GetPeakAntennaGain() const
{
   double             antennaGain       = 1.0;
   WsfAntennaPattern* antennaPatternPtr = GetAntennaPattern(WsfEM_Types::cPOL_DEFAULT, mFrequency);
   if (antennaPatternPtr != nullptr)
   {
      antennaGain = antennaPatternPtr->GetPeakGain(mFrequency);
   }
   return antennaGain;
}

// =================================================================================================
//! Return the minimum antenna gain of the first antenna pattern found.
//! @return The minimum antenna gain (absolute ratio, NOT dB)
// virtual
double WsfEM_XmtrRcvr::GetMinimumAntennaGain() const
{
   double             antennaGain       = 1.0;
   WsfAntennaPattern* antennaPatternPtr = GetAntennaPattern(WsfEM_Types::cPOL_DEFAULT, mFrequency);
   if (antennaPatternPtr != nullptr)
   {
      antennaGain = antennaPatternPtr->GetMinimumGain();
   }
   return antennaGain;
}

// =================================================================================================
// virtual
void WsfEM_XmtrRcvr::SetAntennaBeamCount(unsigned int aBeamCount)
{
   for (const auto& table : mPatternTables)
   {
      for (const auto& pattern : table)
      {
         pattern.first->SetAntennaBeamCount(aBeamCount);
      }
   }
}

// =================================================================================================
//! Clear the array of tables for each polarization
// private
void WsfEM_XmtrRcvr::ClearAntennaPatterns()
{
   for (auto& table : mPatternTables)
   {
      for (const auto& pattern : table)
      {
         delete pattern.first;
      }
      table.clear();
   }
   mUsedExplicitPolarization = {false};
}

// =================================================================================================
bool WsfEM_XmtrRcvr::IsHorizonMaskingEnabled() const
{
   return ((mTerrainMaskingMode & WsfEM_Interaction::cRCVR_HORIZON_MASKING) ||
           (mTerrainMaskingMode & WsfEM_Interaction::cXMTR_HORIZON_MASKING));
}

// =================================================================================================
bool WsfEM_XmtrRcvr::IsTerrainMaskingEnabled() const
{
   return ((mTerrainMaskingMode & WsfEM_Interaction::cRCVR_TERRAIN_MASKING) ||
           (mTerrainMaskingMode & WsfEM_Interaction::cXMTR_TERRAIN_MASKING));
}

// =================================================================================================
std::string WsfEM_XmtrRcvr::MaskingMode() const
{
   if (mTerrainMaskingMode == (WsfEM_Interaction::cRCVR_HORIZON_MASKING | WsfEM_Interaction::cRCVR_TERRAIN_MASKING |
                               WsfEM_Interaction::cXMTR_HORIZON_MASKING | WsfEM_Interaction::cXMTR_TERRAIN_MASKING))
   {
      return "terrain_and_horizon";
   }
   if (mTerrainMaskingMode == (WsfEM_Interaction::cRCVR_TERRAIN_MASKING | WsfEM_Interaction::cXMTR_TERRAIN_MASKING))
   {
      return "terrain_only";
   }
   if (mTerrainMaskingMode == (WsfEM_Interaction::cRCVR_HORIZON_MASKING | WsfEM_Interaction::cXMTR_HORIZON_MASKING))
   {
      return "horizon_only";
   }
   return std::string{};
}

// =================================================================================================
void WsfEM_XmtrRcvr::SetMaskingMode(const std::string& aTerrainMaskingMode)
{
   if ((aTerrainMaskingMode == "terrain_and_horizon") || aTerrainMaskingMode.empty())
   {
      mTerrainMaskingMode = WsfEM_Interaction::cRCVR_HORIZON_MASKING | WsfEM_Interaction::cRCVR_TERRAIN_MASKING |
                            WsfEM_Interaction::cXMTR_HORIZON_MASKING | WsfEM_Interaction::cXMTR_TERRAIN_MASKING;
   }
   else if (aTerrainMaskingMode == "terrain_only")
   {
      mTerrainMaskingMode = WsfEM_Interaction::cRCVR_TERRAIN_MASKING | WsfEM_Interaction::cXMTR_TERRAIN_MASKING;
   }
   else if (aTerrainMaskingMode == "horizon_only")
   {
      mTerrainMaskingMode = WsfEM_Interaction::cRCVR_HORIZON_MASKING | WsfEM_Interaction::cXMTR_HORIZON_MASKING;
   }
}

// =================================================================================================
WsfSimulation* WsfEM_XmtrRcvr::GetSimulation() const
{
   WsfPlatform* platformPtr = GetPlatform();
   return platformPtr ? platformPtr->GetSimulation() : nullptr;
}

// =================================================================================================
WsfEM_Manager* WsfEM_XmtrRcvr::GetEM_Manager() const
{
   WsfSimulation* simPtr = GetSimulation();
   return simPtr ? &simPtr->GetEM_Manager() : nullptr;
}

// =================================================================================================
//! Get the wavelength of the transmitter.
//! @returns wavelength (in meters)
double WsfEM_XmtrRcvr::GetWavelength() const
{
   return UtMath::cLIGHT_SPEED / std::max(mFrequency, 1.0E-30);
}

// =================================================================================================
//! Set the polarization of the receive antenna.
//! @param aPolarization The polarization.
// virtual
void WsfEM_XmtrRcvr::SetPolarization(WsfEM_Types::Polarization aPolarization) {}

// =================================================================================================
//! Set the operating frequency.
//! @param aFrequency The operating frequency (Hz)
// virtual
void WsfEM_XmtrRcvr::SetFrequency(double aFrequency) {}

// =================================================================================================
//! Set the operating bandwidth.
//! @param aBandwidth The receiver operating bandwidth (Hz).
// virtual
void WsfEM_XmtrRcvr::SetBandwidth(double aBandwidth) {}
