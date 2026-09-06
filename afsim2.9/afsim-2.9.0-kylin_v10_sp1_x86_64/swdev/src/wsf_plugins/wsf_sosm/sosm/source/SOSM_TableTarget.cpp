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

#include "SOSM_TableTarget.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>

#include "SOSM_Interaction.hpp"
#include "SOSM_Manager.hpp"
#include "SOSM_Selector.hpp"
#include "SOSM_Utility.hpp"
#include "TblLookup.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"

// The persistent list of radiant intensity tables.
SOSM_TableTarget::IntensityTableMap SOSM_TableTarget::sIntensityTables;

// =================================================================================================
SOSM_TableTarget::SOSM_TableTarget(SOSM_Manager* aManagerPtr)
   : SOSM_Target(aManagerPtr)
{
}

// =================================================================================================
SOSM_TableTarget::SOSM_TableTarget(const SOSM_TableTarget& aSrc)
   : SOSM_Target(aSrc)
{
}

// =================================================================================================
// virtual
SOSM_Target* SOSM_TableTarget::Clone() const
{
   return new SOSM_TableTarget(*this);
}

// =================================================================================================
// virtual
void SOSM_TableTarget::InputComplete()
{
   SOSM_Target::InputComplete(); // Let the base class do its thing first

   // Use the spectral limits of the first state as the limits...
   TT_State* statePtr = static_cast<TT_State*>(mSharedDataPtr->mStates[0]);
   DefineSpectralLimits(*statePtr);
}

// =================================================================================================
// virtual
void SOSM_TableTarget::ComputeRadiantIntensity(const SOSM_SpectralObject& aRequestor,
                                               SOSM_Interaction&          aInteraction,
                                               std::vector<float>&        aBodyIntensity,
                                               float&                     aBodyArea,
                                               std::vector<float>&        aPlumeIntensity,
                                               float&                     aPlumeArea)
{
   // Initialize the result vector. All elements must be initialized to zero because each of the
   // signature components will add to the value...

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

   // Loop through all the components, adding the contribution from each to the total signature

   SOSM_Selector      selector;
   std::vector<float> intensity(aRequestor.Count());
   float              totalBodyArea  = 0.0F;
   float              totalPlumeArea = 0.0F;
   const TT_State*    statePtr       = static_cast<const TT_State*>(StateEntry(aInteraction.mTargetStateIndex));
   for (std::list<Band>::const_iterator bli = statePtr->mBandList.begin(); bli != statePtr->mBandList.end(); ++bli)
   {
      const Band& band = *bli;
      if (selector.Intersect(band, aRequestor) == SOSM_Selector::cINTERSECTS)
      {
         // ----------------------------------------------------------------------------------------
         // Body
         // ----------------------------------------------------------------------------------------

         const Component& body = band.mBody;
         if (body.mIntensityTablePtr &&
             (selector.Intersect(*(body.mIntensityTablePtr), aRequestor) == SOSM_Selector::cINTERSECTS))
         {
            // Perform the independent variable lookups.
            TblLookupLU<float> azLookupBody;
            TblLookupLU<float> elLookupBody;
            body.mIntensityTablePtr->PolarLookup(azLookupBody, elLookupBody, azimuth, elevation);

            // Get the body the projected area (m^2).
            float bodyArea = body.mAreaTablePtr->Interpolate(azLookupBody, elLookupBody);
            totalBodyArea += bodyArea;

            // Get the body radiant intensity (W/sr/um).
            body.mIntensityTablePtr->Interpolate(intensity, selector, azLookupBody, elLookupBody);

            // Increment the output result.
            int oi = selector.OutputStartIndex();
            for (int i = 0; i < selector.Count(); ++i)
            {
               aBodyIntensity[oi] += intensity[oi];
               ++oi;
            }
         }

         // ----------------------------------------------------------------------------------------
         // Plume
         // ----------------------------------------------------------------------------------------

         const Component& plume = band.mPlume;
         if ((plume.mIntensityTablePtr) &&
             (selector.Intersect(*(plume.mIntensityTablePtr), aRequestor) == SOSM_Selector::cINTERSECTS))
         {
            // Perform the independent variable lookups.
            TblLookupLU<float> azLookupPlume;
            TblLookupLU<float> elLookupPlume;
            plume.mIntensityTablePtr->PolarLookup(azLookupPlume, elLookupPlume, azimuth, elevation);

            // Get the plume projected area (m^2).
            float plumeArea = plume.mAreaTablePtr->Interpolate(azLookupPlume, elLookupPlume);
            totalPlumeArea += plumeArea;

            // Get the plume radiant intensity (W/sr/um).
            plume.mIntensityTablePtr->Interpolate(intensity, selector, azLookupPlume, elLookupPlume);

            // Increment the output result.
            int oi = selector.OutputStartIndex();
            for (int i = 0; i < selector.Count(); ++i)
            {
               aPlumeIntensity[oi] += intensity[oi];
               ++oi;
            }
         }
      }
   }

   aBodyArea  = totalBodyArea * 1.0E+4F; // m^2 -> cm^2
   aPlumeArea = totalPlumeArea * 1.0E+4F;
}

// =================================================================================================
// Get the sample points of the currently selected state (fast detection mode table generation).
// This version simply returns the union of the sample points from the component tables.
// virtual
void SOSM_TableTarget::GetSamplePoints(SOSM_Interaction&   aInteraction,
                                       std::vector<float>& aAzValues,
                                       std::vector<float>& aElValues)
{
   std::vector<float> azValues;
   std::vector<float> elValues;

   const TT_State* statePtr = static_cast<const TT_State*>(StateEntry(aInteraction.mTargetStateIndex));
   for (std::list<Band>::const_iterator bli = statePtr->mBandList.begin(); bli != statePtr->mBandList.end(); ++bli)
   {
      const Band& band = *bli;

      if (band.mBody.mIntensityTablePtr)
      {
         const std::vector<float>& azValues2 = band.mBody.mIntensityTablePtr->IV1().GetValues();
         const std::vector<float>& elValues2 = band.mBody.mIntensityTablePtr->IV2().GetValues();
         std::vector<float>        mergedAzValues;
         std::vector<float>        mergedElValues;
         std::set_union(azValues.begin(),
                        azValues.end(),
                        azValues2.begin(),
                        azValues2.end(),
                        std::back_insert_iterator<std::vector<float>>(mergedAzValues));
         std::set_union(elValues.begin(),
                        elValues.end(),
                        elValues2.begin(),
                        elValues2.end(),
                        std::back_insert_iterator<std::vector<float>>(mergedElValues));
         azValues = mergedAzValues;
         elValues = mergedElValues;
      }

      if (band.mPlume.mIntensityTablePtr)
      {
         const std::vector<float>& azValues2 = band.mPlume.mIntensityTablePtr->IV1().GetValues();
         const std::vector<float>& elValues2 = band.mPlume.mIntensityTablePtr->IV2().GetValues();
         std::vector<float>        mergedAzValues;
         std::vector<float>        mergedElValues;
         std::set_union(azValues.begin(),
                        azValues.end(),
                        azValues2.begin(),
                        azValues2.end(),
                        std::back_insert_iterator<std::vector<float>>(mergedAzValues));
         std::set_union(elValues.begin(),
                        elValues.end(),
                        elValues2.begin(),
                        elValues2.end(),
                        std::back_insert_iterator<std::vector<float>>(mergedElValues));
         azValues = mergedAzValues;
         elValues = mergedElValues;
      }
   }

   aAzValues = azValues;
   aElValues = elValues;
}

// =================================================================================================
// Get the sample target state (altitude, speed, throttle) (for fast detection mode table generation).
// virtual
void SOSM_TableTarget::GetSampleTargetState(SOSM_Interaction& aInteraction, float& aAltitude, float& aSpeed, float& aThrottle)
{
   // The samples conditions are not important to this model. They are provided to the table generator
   // which simply pushed them back in to the interaction object - where they aren't used!
   aAltitude = 0.0F;
   aSpeed    = 0.0F;
   aThrottle = 0.0F;
}

// =================================================================================================
// Called by the base class to create an instance of State specific to our model.
// protected virtual
SOSM_Target::StateBase* SOSM_TableTarget::CreateState(const std::string& aStateName) const
{
   return new TT_State(aStateName);
}

// =================================================================================================
// Nested class 'TT_State'.
// =================================================================================================

// =================================================================================================
SOSM_TableTarget::TT_State::TT_State(const std::string& aStateName)
   : StateBase(aStateName)
   , mBandList()
   , mExplicitBandUsed(false)
   , mImplicitBandUsed(false)
{
   // Must have one band for implicit band input.
   mBandList.push_back(Band("default"));
}

// =================================================================================================
// This is called by the base class when all of the input for the state has been read.
// virtual
void SOSM_TableTarget::TT_State::InputComplete(SOSM_Target& aTarget)
{
   // If explicit bands were not provided, make sure the implicit input is valid.
   // (Even if an implicit band wasn't used, we still check - because it will catch the error).
   if (!mExplicitBandUsed)
   {
      mBandList.front().InputComplete(mStateName);
   }

   // Determine the composite spectral limits of all bands. This is just the union of all
   // the bands, including gaps between.

   SOSM_SpectralObject stateLimits;
   for (std::list<Band>::iterator bli = mBandList.begin(); bli != mBandList.end(); ++bli)
   {
      Band&                       band   = *bli;
      SOSM_SpectralObject::Status status = stateLimits.Union(stateLimits, band);
      if (status != SOSM_SpectralObject::cOK)
      {
         throw UtException("State " + mStateName + ", band " + band.mBandName + ": " +
                           SOSM_SpectralObject::StatusString(status));
      }
   }

   // Make sure data has been provided.
   if (stateLimits.Count() == 0)
   {
      throw UtException("State " + mStateName + ": no data has been defined");
   }
   DefineSpectralLimits(stateLimits);
}

// =================================================================================================
// virtual
bool SOSM_TableTarget::TT_State::ProcessInput(UtInput& aInput, SOSM_Target& aTarget)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   // An explicit 'band' command can be used only if commands have not been applied to the implicit band.
   // Commands for the implicit 'band' cannot be used if an explicit band has been defined.
   if (command == "band")
   {
      if (mImplicitBandUsed)
      {
         throw UtInput::BadValue(aInput, "'band' cannot be used in this context");
      }

      // If first explicit band, clear the band list to eliminate the default implicit band.
      if (!mExplicitBandUsed)
      {
         mBandList.clear();
         mExplicitBandUsed = true;
      }
      UtInputBlock inputBlock(aInput);

      // Read the band name and make sure it doesn't already exist
      std::string bandName;
      aInput.ReadValue(bandName);
      for (std::list<Band>::const_iterator bli = mBandList.begin(); bli != mBandList.end(); ++bli)
      {
         if (bandName == bli->mBandName)
         {
            throw UtInput::BadValue(aInput, "duplicate 'band' name: " + bandName);
         }
      }

      // Create the new band and read its input.
      mBandList.push_back(Band(bandName));
      while (inputBlock.ReadCommand())
      {
         if (!mBandList.back().ProcessInput(aInput, aTarget))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      mBandList.back().InputComplete(mStateName);
   }
   else if (mBandList.front().ProcessInput(aInput, aTarget))
   {
      if (mExplicitBandUsed)
      {
         throw UtInput::BadValue(aInput, "Implicit 'band' commands cannot be used if an explicit 'band' has been defined");
      }
      mImplicitBandUsed = true;
   }
   else
   {
      myCommand = StateBase::ProcessInput(aInput, aTarget);
   }
   return myCommand;
}

// =================================================================================================
// Nested class 'Band'.
// =================================================================================================

// =================================================================================================
SOSM_TableTarget::Band::Band(const std::string& aBandName)
   : SOSM_SpectralObject()
   , mBandName(aBandName)
{
}

// =================================================================================================
void SOSM_TableTarget::Band::InputComplete(const std::string& aStateName)
{
   std::string prefix("State " + aStateName + ", band " + mBandName + ": ");

   // Make sure the area and intensity definitions are consistent within a component.

   // The body component must be provided.
   if (!mBody.mAreaTablePtr || !mBody.mIntensityTablePtr)
   {
      throw UtException(prefix + "body_area and body_intensity must be provided");
   }

   mBody.CheckConsistency(prefix, "body");
   mPlume.CheckConsistency(prefix, "plume");

   // Determine the spectral limits of the state. This is just the union of the
   // spectral limits of all the components.

   SOSM_SpectralObject bandLimits;
   if (mBody.mIntensityTablePtr)
   {
      SOSM_SpectralObject::Status status = bandLimits.Union(bandLimits, *(mBody.mIntensityTablePtr));
      if (status != SOSM_SpectralObject::cOK)
      {
         throw UtException(prefix + SOSM_SpectralObject::StatusString(status));
      }
   }
   if (mPlume.mIntensityTablePtr)
   {
      SOSM_SpectralObject::Status status = bandLimits.Union(bandLimits, *(mPlume.mIntensityTablePtr));
      if (status != SOSM_SpectralObject::cOK)
      {
         throw UtException(prefix + SOSM_SpectralObject::StatusString(status));
      }
   }

   if (bandLimits.Count() == 0)
   {
      throw UtException(prefix + "No components have been defined");
   }
   DefineSpectralLimits(bandLimits);
}

// =================================================================================================
bool SOSM_TableTarget::Band::ProcessInput(UtInput& aInput, SOSM_Target& aTarget)
{
   std::string cachePrefix(aTarget.GetCachePrefix());
   std::string fileName;
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "body_and_plume")
   {
      aInput.ReadValueQuoted(fileName);
      mBody.LoadAreaTable(aTarget.GetManager(), aInput.LocateFile(fileName + ".bda"), cachePrefix);
      mBody.LoadIntensityTable(aTarget.GetManager(), aInput.LocateFile(fileName + ".bdi"), cachePrefix);
      mPlume.LoadAreaTable(aTarget.GetManager(), aInput.LocateFile(fileName + ".pla"), cachePrefix);
      mPlume.LoadIntensityTable(aTarget.GetManager(), aInput.LocateFile(fileName + ".pli"), cachePrefix);
   }
   else if (command == "body")
   {
      aInput.ReadValueQuoted(fileName);
      mBody.LoadAreaTable(aTarget.GetManager(), aInput.LocateFile(fileName + ".bda"), cachePrefix);
      mBody.LoadIntensityTable(aTarget.GetManager(), aInput.LocateFile(fileName + ".bdi"), cachePrefix);
   }
   else if (command == "body_area")
   {
      aInput.ReadValueQuoted(fileName);
      mBody.LoadAreaTable(aTarget.GetManager(), aInput.LocateFile(fileName), cachePrefix);
   }
   else if (command == "body_intensity")
   {
      aInput.ReadValueQuoted(fileName);
      mBody.LoadIntensityTable(aTarget.GetManager(), aInput.LocateFile(fileName), cachePrefix);
   }
   else if (command == "plume")
   {
      aInput.ReadValueQuoted(fileName);
      mPlume.LoadAreaTable(aTarget.GetManager(), aInput.LocateFile(fileName + ".pla"), cachePrefix);
      mPlume.LoadIntensityTable(aTarget.GetManager(), aInput.LocateFile(fileName + ".pli"), cachePrefix);
   }
   else if (command == "plume_area")
   {
      aInput.ReadValueQuoted(fileName);
      mPlume.LoadAreaTable(aTarget.GetManager(), aInput.LocateFile(fileName), cachePrefix);
   }
   else if (command == "plume_intensity")
   {
      aInput.ReadValueQuoted(fileName);
      mPlume.LoadIntensityTable(aTarget.GetManager(), aInput.LocateFile(fileName), cachePrefix);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
// Nested class 'Component'.
// =================================================================================================

// =================================================================================================
void SOSM_TableTarget::Component::CheckConsistency(const std::string& aMessagePrefix, const std::string& aComponentType) const
{
   std::string prefix(aMessagePrefix);

   if (!mAreaTablePtr && !mIntensityTablePtr)
   {
      return;
   }
   if (!mAreaTablePtr && mIntensityTablePtr)
   {
      throw UtException(prefix + aComponentType + "_area not provided");
   }
   if (mAreaTablePtr && !mIntensityTablePtr)
   {
      throw UtException(prefix + aComponentType + "_intensity not provided");
   }

   // Ensure independent variables are the same size with the same values.

   prefix = aMessagePrefix + aComponentType + "_area and " + aComponentType + "_intensity: ";

   if ((mAreaTablePtr->IV1().GetSize() != mIntensityTablePtr->IV1().GetSize()) ||
       (mAreaTablePtr->IV2().GetSize() != mIntensityTablePtr->IV2().GetSize()))
   {
      throw UtException(prefix + "Inconsistent azimuth/elevation values");
   }

   size_t i;
   for (i = 0; i < mAreaTablePtr->IV1().GetSize(); ++i)
   {
      if (mAreaTablePtr->IV1().Get(i) != mIntensityTablePtr->IV1().Get(i))
      {
         throw UtException(prefix + "Inconsistent azimuth values");
      }
   }
   for (i = 0; i < mAreaTablePtr->IV2().GetSize(); ++i)
   {
      if (mAreaTablePtr->IV2().Get(i) != mIntensityTablePtr->IV2().Get(i))
      {
         throw UtException(prefix + "Inconsistent elevation values");
      }
   }
}

// =================================================================================================
void SOSM_TableTarget::Component::LoadAreaTable(SOSM_Manager*      aManagerPtr,
                                                const std::string& aFileName,
                                                const std::string& aCachePrefix)
{
   SOSM_TableVar dvInfo("area", 0.0F, 1.0E+37F, 1.0F); // m^2
   mAreaTablePtr = SOSM_ScalarTable2D::LoadSharedPolarTable(aManagerPtr, aFileName, aCachePrefix, dvInfo);
}

// =================================================================================================
void SOSM_TableTarget::Component::LoadIntensityTable(SOSM_Manager*      aManagerPtr,
                                                     const std::string& aFileName,
                                                     const std::string& aCachePrefix)
{
   IntensityTableMap::iterator rti = sIntensityTables.find(aFileName);
   if (rti != sIntensityTables.end())
   {
      mIntensityTablePtr = rti->second;
   }
   else
   {
      std::unique_ptr<SOSM_SpectralTable2D> tempTablePtr(new SOSM_SpectralTable2D(aManagerPtr));
      float                                 radPerDeg = static_cast<float>(UtMath::cRAD_PER_DEG);
      tempTablePtr->SetDV_Info(SOSM_TableVar("intensity", 0.0F, 1.0E+37F, 1.0F));
      tempTablePtr->SetIV1_Info(SOSM_TableVar("azimuth", -180.0F, 180.0F, radPerDeg));
      tempTablePtr->SetIV2_Info(SOSM_TableVar("elevation", -90.0F, 90.0F, radPerDeg));
      tempTablePtr->Load(aFileName, aCachePrefix);
      mIntensityTablePtr          = IntensityTablePtr(tempTablePtr.release());
      sIntensityTables[aFileName] = mIntensityTablePtr;
   }
}
