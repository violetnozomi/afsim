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

#include "WsfHighEnergyLaser.hpp"

#include <string>

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfDefaultHEL.hpp"
#include "WsfDirectedEnergyWeapon.hpp"
#include "WsfException.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfIntersectMesh.hpp"
#include "WsfIntersectProcessor.hpp" // DEPRECATED
#include "WsfLocalTrack.hpp"
#include "WsfMil.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfHighEnergyLaserTypes& WsfHighEnergyLaserTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetHighEnergyLaserTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfHighEnergyLaserTypes& WsfHighEnergyLaserTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetHighEnergyLaserTypes();
}

// =================================================================================================
WsfHighEnergyLaserTypes::WsfHighEnergyLaserTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfHighEnergyLaser>(aScenario, "high_energy_laser")
{
   Add("default", ut::make_unique<WsfDefaultHEL>(aScenario));
}

//! (Factory Method) Create a new instance of a strategy with the given name.
//! @param aName The name of the desired strategy.
//! @return A new instance of the desired strategy, or 0 if the strategy is not registered.
WsfHighEnergyLaser* WsfHighEnergyLaserTypes::Create(const std::string& aName) const
{
   WsfHighEnergyLaser* strategyPtr    = Find(aName);
   WsfHighEnergyLaser* newStrategyPtr = nullptr;
   if (strategyPtr != nullptr)
   {
      newStrategyPtr = static_cast<WsfHighEnergyLaser*>(strategyPtr->Clone());
   }
   else
   {
      throw UtException("WsfHighEnergyLaser: Strategy " + aName + " does not exist!");
   }
   return newStrategyPtr;
}

WsfHighEnergyLaser::WsfHighEnergyLaser(WsfScenario& aScenario)
   : Beam()
   , mWeaponPtr(nullptr)
   , mSysInit(false)
   , mCalculateIncidence(false)
   , mIncidenceAngle(0.0)
   , mTargetIndex(0)
   , mTargetOffsetName()
   , mTargetOccluded(false)
   , mTargetRange(1000.0)
   , mIsLasing(false)
   , mDebug(false)
   , mLastUpdateTime(0.0)
   , mDamageRadius(0.0)
   , mDefaultDamageRadius(0.0)
   , mJitterExpectedValue(0.0)
   , mBeamQuality(1.0)
   , mTurbulenceModel(aScenario)
{
}

WsfHighEnergyLaser::WsfHighEnergyLaser(const WsfHighEnergyLaser& aSrc)
   : Beam(aSrc)
   , mWeaponPtr(aSrc.mWeaponPtr)
   , mSysInit(false)
   , mCalculateIncidence(aSrc.mCalculateIncidence)
   , mIncidenceAngle(aSrc.mIncidenceAngle)
   , mTargetIndex(aSrc.mTargetIndex)
   , mTargetOffsetName(aSrc.mTargetOffsetName)
   , mTargetOccluded(aSrc.mTargetOccluded)
   , mTargetRange(aSrc.mTargetRange)
   , mIsLasing(false)
   , mDebug(aSrc.mDebug)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mDamageRadius(aSrc.mDamageRadius)
   , mDefaultDamageRadius(aSrc.mDefaultDamageRadius)
   , mJitterExpectedValue(aSrc.mJitterExpectedValue)
   , mBeamQuality(aSrc.mBeamQuality)
   , mTurbulenceModel(aSrc.mTurbulenceModel)
{
}

// virtual
bool WsfHighEnergyLaser::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = true;
   if (command == "calculate_incidence")
   {
      aInput.ReadValue(mCalculateIncidence);
   }
   else if (command == "jitter")
   {
      double jitter;
      aInput.ReadValueOfType(jitter, UtInput::cANGLE);
      SetJitter(jitter);
   }
   else if ((command == "atmospheric_structure") || (command == "CN2_form"))
   {
      std::string model;
      aInput.ReadCommand(model);
      mTurbulenceModel.SetCN2Form(model);
   }
   else if (command == "damage_radius") // NO_DOC | DEPRECATED
   {
      aInput.ReadValueOfType(mDefaultDamageRadius, UtInput::cLENGTH);
   }
   else if (command == "aperture_diameter")
   {
      double apertureDiameter;
      aInput.ReadValueOfType(apertureDiameter, UtInput::cLENGTH);
      SetApertureDiameter(apertureDiameter);
   }
   else if (command == "wavelength")
   {
      double wavelength;
      aInput.ReadValueOfType(wavelength, UtInput::cLENGTH);
      SetWavelength(wavelength);
   }
   else if (command == "laser_type")
   {
      std::string type;
      aInput.ReadCommand(type);
      if (type == "carbon_dioxide")
      {
         SetWavelength(1000.0E-9);
      }
      else if (type == "nd_yag")
      {
         SetWavelength(1064.0E-9);
      }
      else if (type == "coil")
      {
         SetWavelength(1315.0E-9);
      }
      else if (type == "deuterium_fluoride")
      {
         SetWavelength(3800.0E-9);
      }
      else
      {
         ut::log::error() << "Value for laser_type not recognized.";
         throw(UtInput::BadValue(aInput));
      }
   }
   else if (command == "power")
   {
      double power;
      aInput.ReadValueOfType(power, UtInput::cPOWER);
      SetInitialPower(power);
   }
   else if (command == "beam_quality")
   {
      double beamQuality;
      aInput.ReadValue(beamQuality);
      SetBeamQuality(beamQuality);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// virtual
bool WsfHighEnergyLaser::Initialize(WsfDirectedEnergyWeapon* aWeaponPtr)
{
   // todo make sure necessary inputs provided.
   mWeaponPtr = aWeaponPtr;
   mDebug     = aWeaponPtr->DebugEnabled();
   return (mWeaponPtr != nullptr);
}

// private
void WsfHighEnergyLaser::BeginLasing(double aSimTime, size_t aTargetIndex,
                                     const std::string& aTargetOffsetName) // = ""
{
   // Set up the beam interaction.
   if (mTargetIndex != 0)
   {
      throw UtException("WsfHighEnergyLaser: 'BeginLasing' called without previous call to 'EndLasing'.");
   }

   // Save the target offset for later calculation.
   mTargetOffsetName = aTargetOffsetName;
   mTargetIndex      = aTargetIndex;

   mIsLasing = true;
   Beam::Initialize();
   mLastUpdateTime = aSimTime - 1.0e-6;

   // Perform an initial update, so that the instantaneous beam quantites are valid
   // (esp for a directed energy fire pdu in DIS).
   UpdateLasing(aSimTime);
}

//! Assuming a laser engagement is underway, update the target geometry and
//! Integrate the fluence on target.
void WsfHighEnergyLaser::UpdateLasing(double aSimTime) // same track as before
{
   if ((aSimTime > mLastUpdateTime) && mIsLasing)
   {
      if (UpdateEngagementGeometry(aSimTime, mTargetIndex, mTargetOffsetName))
      {
         Propagate(aSimTime);
      }
      mLastUpdateTime = aSimTime;
   }
}

void WsfHighEnergyLaser::EndLasing(double aSimTime)
{
   mTargetIndex = 0;
   mIsLasing    = false;
}

// protected
//! Default target geometry update calculation.
//! This should be extended, possibly as a template method, to account for the new geometry meshes.
// protected
bool WsfHighEnergyLaser::UpdateEngagementGeometry(double             aSimTime,
                                                  size_t             aTargetIndex,
                                                  const std::string& aTargetOffset,
                                                  bool               aAddJitter)
{
   WsfPlatform* targetPtr   = GetSimulation()->GetPlatformByIndex(aTargetIndex);
   bool         targetValid = (targetPtr) != nullptr;
   if (targetValid)
   {
      // Update target geometry
      // TODO this needs to be adjusted for terrain
      targetPtr->Update(aSimTime);
      WsfIntersectMesh* meshPtr =
         static_cast<WsfIntersectMesh*>(targetPtr->FindComponentByRole(cWSF_COMPONENT_INTERSECT_MESH));
      if (meshPtr != nullptr)
      {
         if (meshPtr->TargetOffsetExists(aTargetOffset))
         {
            mTargetOffsetName = aTargetOffset;
         }
         else if (aTargetOffset.empty())
         {
            // If no target offset was provided, pick the first one in the offset list:
            mTargetOffsetName = meshPtr->GetTargetOffsetName(1);
         }

         double surfaceDotProduct = 1.0;
         int    materialCode      = 0;
         double jitter            = 0.0;

         if (aAddJitter)
         {
            jitter = GetSimulation()->GetRandom().Normal(0.0, mJitterExpectedValue);
         }

         bool meshIntersectionSuccess =
            meshPtr->Intersect(GetWeapon()->GetPlatform(), mTargetOffsetName, jitter, surfaceDotProduct, materialCode, mTargetRange);

         if (!meshPtr->TargetOffsetExists(mTargetOffsetName))
         {
            // Only issue the error once.
            static std::set<size_t> sOnceSet; // TODO-AWK Ugly!!
            if (sOnceSet.find(targetPtr->GetIndex()) == sOnceSet.end())
            {
               sOnceSet.insert(targetPtr->GetIndex());
               auto out = ut::log::error() << "WsfHighEnergyLaser::UpdateEngagementGeometry: Intersect test for target "
                                              "is invalid. Check the offset name.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Target: " << mTargetOffsetName;
            }
         }
         mIncidenceAngle = acos(std::min(std::max(surfaceDotProduct, -1.0), 1.0));
         if ((mIncidenceAngle > UtMath::cPI_OVER_2) || (mIncidenceAngle < -UtMath::cPI_OVER_2))
         {
            mIncidenceAngle = UtMath::cPI_OVER_2; // This will produce negligible power on target.
         }

         // Todo check with Jonathon about case where material code is nonzero and dot product is negative
         mTargetOccluded = meshIntersectionSuccess && ((materialCode == 0) || (surfaceDotProduct < 0.0));
      }
      // DEPRECATED BEGIN
      //-----------------------------------------------------------------------------------------------------------
      else if (targetPtr->GetAuxDataConst().AttributeExists("INTERSECT_PROCESSOR_NAME"))
      {
         std::string            ipName = targetPtr->GetAuxDataConst().GetString("INTERSECT_PROCESSOR_NAME");
         WsfIntersectProcessor* ipPtr = static_cast<WsfIntersectProcessor*>(targetPtr->GetComponent<WsfProcessor>(ipName));
         // TODO_CBA the code assumed the intersect processor existed...

         if (ipPtr->TargetOffsetExists(aTargetOffset))
         {
            mTargetOffsetName = aTargetOffset;
         }
         else if (aTargetOffset.empty())
         {
            // If no target offset was provided, pick the first one in the offset list:
            mTargetOffsetName = ipPtr->GetTargetOffsetName(1);
         }

         double surfaceDotProduct = 1.0;
         int    materialCode      = 0;
         double jitter            = 0.0;

         if (aAddJitter)
         {
            static ut::Random randomJitter; // TODO-AWK this may be a bi issue with repeatability
            jitter = randomJitter.Normal(0.0, mJitterExpectedValue);
         }

         bool meshIntersectionSuccess =
            ipPtr->Intersect(GetWeapon()->GetPlatform(), mTargetOffsetName, jitter, surfaceDotProduct, materialCode, mTargetRange);

         if (!ipPtr->TargetOffsetExists(mTargetOffsetName))
         {
            // Only issue the error once.
            static std::set<size_t> sOnceSet; // TODO-AWK Ugly!!
            if (sOnceSet.find(targetPtr->GetIndex()) == sOnceSet.end())
            {
               sOnceSet.insert(targetPtr->GetIndex());
               auto out = ut::log::error() << "WsfHighEnergyLaser::UpdateEngagementGeometry: Intersect test for target "
                                              "is invalid. Check the offset name.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Target: " << mTargetOffsetName;
            }
         }
         mIncidenceAngle = acos(std::min(std::max(surfaceDotProduct, -1.0), 1.0));
         if ((mIncidenceAngle > UtMath::cPI_OVER_2) || (mIncidenceAngle < -UtMath::cPI_OVER_2))
         {
            mIncidenceAngle = UtMath::cPI_OVER_2; // This will produce negligible power on target.
         }

         // Todo check with Jonathon about case where material code is nonzero and dot product is negative
         mTargetOccluded = meshIntersectionSuccess && ((materialCode == 0) || (surfaceDotProduct < 0.0));
      }
      // DEPRECATED END
      //-----------------------------------------------------------------------------------------------------------
      else if (mCalculateIncidence) // and no associated mesh
      {
         // Check aspect (target to shooter)
         double targetVecWCS[3];
         targetPtr->GetRelativeLocationWCS(mWeaponPtr->GetPlatform(), targetVecWCS);
         double az;
         double el;
         targetPtr->ComputeAspect(targetVecWCS, az, el);

         // Now find the incidence angle, here wrt the waterline of the target.
         // Relate the az and el angles to the incidence angle.
         mIncidenceAngle = acos(std::min(std::max(cos(el) * cos(az), -1.0), 1.0));
         if ((mIncidenceAngle > UtMath::cPI_OVER_2) || (mIncidenceAngle < -UtMath::cPI_OVER_2))
         {
            mIncidenceAngle = UtMath::cPI_OVER_2; // This will produce negligible power on target.
         }

         // Don't calculate target occlusion for this case (try to have a mesh!!).
      }

      // Set the incidence angle in the weapon
      GetWeapon()->GetBeam()->SetIncidenceAngle(mIncidenceAngle);

      // Use existing centroid geometry to compute range
      double targetLoc[3];
      targetPtr->GetLocationWCS(targetLoc);

      double myLoc[3];
      mWeaponPtr->GetLocationWCS(myLoc);

      double targetVec[3];
      UtVec3d::Subtract(targetVec, targetLoc, myLoc);
      mTargetRange = UtVec3d::Magnitude(targetVec);
   }

   return targetValid;
}

WsfSimulation* WsfHighEnergyLaser::GetSimulation() const
{
   return mWeaponPtr != nullptr ? mWeaponPtr->GetSimulation() : nullptr;
}
