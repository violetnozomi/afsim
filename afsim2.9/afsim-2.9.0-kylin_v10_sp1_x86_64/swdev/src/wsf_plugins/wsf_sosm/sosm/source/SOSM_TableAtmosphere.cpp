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

#include "SOSM_TableAtmosphere.hpp"

#include <iostream>

#include "SOSM_Interaction.hpp"
#include "SOSM_Manager.hpp"
#include "SOSM_Selector.hpp"
#include "TblLookup.hpp"
#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"

// =================================================================================================
SOSM_TableAtmosphere::SOSM_TableAtmosphere(SOSM_Manager* aManagerPtr)
   : SOSM_Atmosphere()
   , mSharedDataPtr(new SharedData(aManagerPtr))
{
}

// =================================================================================================
// virtual
SOSM_Atmosphere* SOSM_TableAtmosphere::Clone() const
{
   return new SOSM_TableAtmosphere(*this);
}

// =================================================================================================
// virtual
void SOSM_TableAtmosphere::InputComplete()
{
   // Make sure required tables were provided.

   const SOSM_SpectralTable2D& backgroundRadiance      = mSharedDataPtr->mBackgroundRadiance;
   const SOSM_SpectralTable3D& foregroundRadiance      = mSharedDataPtr->mForegroundRadiance;
   const SOSM_SpectralTable3D& foregroundTransmittance = mSharedDataPtr->mForegroundTransmittance;
   if ((backgroundRadiance.Count() == 0) || (foregroundTransmittance.Count() == 0))
   {
      throw UtException("Required tables were not specified");
   }

   // Check for consistency in the spectral definitions.
   if (!foregroundTransmittance.Equals(backgroundRadiance))
   {
      throw UtException("Inconsistent spectral limits for " + GetTypeName());
   }
   if ((foregroundRadiance.Count() != 0) && (!foregroundTransmittance.Equals(foregroundRadiance)))
   {
      throw UtException("Inconsistent spectral limits for " + GetTypeName());
   }
   DefineSpectralLimits(foregroundTransmittance);

   // Check for consistency in the independent variable specifications (all tables must have the same breakpoints).

   CheckIV("altitude", foregroundTransmittance.IV1(), backgroundRadiance.IV1());
   CheckIV("elevation", foregroundTransmittance.IV2(), backgroundRadiance.IV2());
   if (foregroundRadiance.Count() != 0)
   {
      CheckIV("altitude", foregroundTransmittance.IV1(), foregroundRadiance.IV1());
      CheckIV("elevation", foregroundTransmittance.IV2(), foregroundRadiance.IV2());
      CheckIV("range", foregroundTransmittance.IV3(), foregroundRadiance.IV3());
   }
}

// =================================================================================================
// virtual
bool SOSM_TableAtmosphere::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "background_radiance")
   {
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName                    = aInput.LocateFile(fileName);
      SOSM_SpectralTable2D& table = mSharedDataPtr->mBackgroundRadiance;
      table.SetDV_Info(SOSM_TableVar("radiance", 0.0F, 1.0E+37F, 1.0F));
      table.SetIV1_Info(SOSM_TableVar("altitude", 0.0F, 1.0E+37F, 1.0F));
      table.SetIV2_Info(SOSM_TableVar("elevation", -90.0F, 90.0F, static_cast<float>(UtMath::cRAD_PER_DEG)));
      table.Load(fileName, GetCachePrefix());
      UpdateModificationTime(table.GetModificationTime());
   }
   else if (command == "foreground_radiance")
   {
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName                    = aInput.LocateFile(fileName);
      SOSM_SpectralTable3D& table = mSharedDataPtr->mForegroundRadiance;
      table.SetDV_Info(SOSM_TableVar("radiance", 0.0F, 1.0E+37F, 1.0F));
      table.SetIV1_Info(SOSM_TableVar("altitude", 0.0F, 1.0E+37F, 1.0F));
      table.SetIV2_Info(SOSM_TableVar("elevation", -90.0F, 90.0F, static_cast<float>(UtMath::cRAD_PER_DEG)));
      table.SetIV3_Info(SOSM_TableVar("range", 0.0F, 1.0E+37F, 1.0F));
      table.Load(fileName, GetCachePrefix());
      UpdateModificationTime(table.GetModificationTime());
   }
   else if (command == "foreground_transmittance")
   {
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName                    = aInput.LocateFile(fileName);
      SOSM_SpectralTable3D& table = mSharedDataPtr->mForegroundTransmittance;
      table.SetDV_Info(SOSM_TableVar("transmittance", 0.0F, 1.0F, 1.0F));
      table.SetIV1_Info(SOSM_TableVar("altitude", 0.0F, 1.0E+37F, 1.0F));
      table.SetIV2_Info(SOSM_TableVar("elevation", -90.0F, 90.0F, static_cast<float>(UtMath::cRAD_PER_DEG)));
      table.SetIV3_Info(SOSM_TableVar("range", 0.0F, 1.0E+37F, 1.0F));
      table.Load(fileName, GetCachePrefix());
      UpdateModificationTime(table.GetModificationTime());
   }
   else
   {
      myCommand = SOSM_Atmosphere::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
void SOSM_TableAtmosphere::ComputeAtmosphereData(const SOSM_Selector& aSelector,
                                                 SOSM_Interaction&    aInteraction,
                                                 std::vector<float>&  aBackgroundRadiance,
                                                 std::vector<float>&  aForegroundRadiance,
                                                 std::vector<float>&  aForegroundTransmittance)
{
   aSelector.InitializeOutput(aBackgroundRadiance, 0.0F);
   aSelector.InitializeOutput(aForegroundTransmittance, 0.0F);
   aSelector.InitializeOutput(aForegroundRadiance, 0.0F);

   // All tables have the same breakpoints, so the independent variable lookups only need be done once.

   TblLookupLU<float> altLookup;
   TblLookupLU<float> elLookup;
   TblLookupLU<float> rangeLookup;
   altLookup.Lookup(mSharedDataPtr->mForegroundTransmittance.IV1(), aInteraction.GetSensorAltitude());
   elLookup.Lookup(mSharedDataPtr->mForegroundTransmittance.IV2(), aInteraction.GetAbsoluteTargetElevation());
   rangeLookup.Lookup(mSharedDataPtr->mForegroundTransmittance.IV3(), aInteraction.GetSlantRange());

   mSharedDataPtr->mBackgroundRadiance.Interpolate(aBackgroundRadiance, aSelector, altLookup, elLookup);
   mSharedDataPtr->mForegroundTransmittance.Interpolate(aForegroundTransmittance, aSelector, altLookup, elLookup, rangeLookup);

   // The foreground radiance is optional
   if (mSharedDataPtr->mForegroundRadiance.Count() != 0)
   {
      mSharedDataPtr->mForegroundRadiance.Interpolate(aForegroundRadiance, aSelector, altLookup, elLookup, rangeLookup);
   }
   else
   {
      int oi = aSelector.OutputStartIndex();
      for (int i = 0; i < aSelector.Count(); ++i)
      {
         aForegroundRadiance[oi] = 0.0F;
         ++oi;
      }
   }
}

// =================================================================================================
//! Get the sample points to be used for fast detection mode table generation.
//! This version simply returns the breakpoints for the tables that define the atmosphere.
// virtual
void SOSM_TableAtmosphere::GetSamplePoints(std::vector<float>& aAltValues,
                                           std::vector<float>& aElValues,
                                           std::vector<float>& aRangeValues)
{
   aAltValues   = mSharedDataPtr->mForegroundTransmittance.IV1().GetValues();
   aElValues    = mSharedDataPtr->mForegroundTransmittance.IV2().GetValues();
   aRangeValues = mSharedDataPtr->mForegroundTransmittance.IV3().GetValues();
}

// =================================================================================================
// private
void SOSM_TableAtmosphere::CheckIV(const std::string&       aVarName,
                                   const TblIndVarU<float>& aIV1,
                                   const TblIndVarU<float>& aIV2) const
{
   if (aIV1.GetSize() != aIV2.GetSize())
   {
      throw UtException("Inconsistent " + aVarName + " dimensions for " + GetTypeName());
   }

   for (size_t i = 0; i < aIV1.GetSize(); ++i)
   {
      if (aIV1.Get(i) != aIV2.Get(i))
      {
         throw UtException("Inconsistent " + aVarName + " values for " + GetTypeName());
      }
   }
}

// =================================================================================================
SOSM_TableAtmosphere::SharedData::SharedData(SOSM_Manager* aManagerPtr)
   : mBackgroundRadiance(aManagerPtr)
   , mForegroundRadiance(aManagerPtr)
   , mForegroundTransmittance(aManagerPtr)
{
}
