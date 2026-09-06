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

#include "WsfDirectedEnergyWeapon.hpp"

#include <cmath>

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"

WsfDirectedEnergyWeapon::WsfDirectedEnergyWeapon(const WsfScenario& aScenario)
   : WsfImplicitWeapon(aScenario)
   , mBeamPtr(nullptr)
   , mIsFiring(false)
{
}

WsfDirectedEnergyWeapon::WsfDirectedEnergyWeapon(const WsfDirectedEnergyWeapon& aSrc)
   : WsfImplicitWeapon(aSrc)
   , mBeamPtr(nullptr)
   , mIsFiring(false)
{
   if (aSrc.mBeamPtr != nullptr)
   {
      mBeamPtr = aSrc.mBeamPtr->Clone();
   }
}

// virtual
WsfDirectedEnergyWeapon::~WsfDirectedEnergyWeapon()
{
   delete mBeamPtr;
}

// ================================================================================================
const int* WsfDirectedEnergyWeapon::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_DIRECTED_ENERGY_WEAPON,
                               cWSF_COMPONENT_WEAPON,
                               cWSF_COMPONENT_ARTICULATED_PART,
                               cWSF_COMPONENT_PLATFORM_PART,
                               cWSF_COMPONENT_NULL};
   return roles;
}

// ================================================================================================
void* WsfDirectedEnergyWeapon::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_DIRECTED_ENERGY_WEAPON)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_WEAPON)
   {
      return dynamic_cast<WsfWeapon*>(this);
   }
   if (aRole == cWSF_COMPONENT_ARTICULATED_PART)
   {
      return dynamic_cast<WsfArticulatedPart*>(this);
   }
   if (aRole == cWSF_COMPONENT_PLATFORM_PART)
   {
      return dynamic_cast<WsfPlatformPart*>(this);
   }
   return nullptr;
}

bool WsfDirectedEnergyWeapon::BeginEngagement(double aSimTime, EngagementData* aEngagementDataPtr)
{
   bool success = WsfImplicitWeapon::BeginEngagement(aSimTime, aEngagementDataPtr);

   // Set the initial weapon velocity to be the speed of light.
   if (success)
   {
      assert(aEngagementDataPtr->mTargetTrackPtr != nullptr);
      double weaponVelocityWCS[3];
      double myLocWCS[3];
      GetPlatform()->GetLocationWCS(myLocWCS);
      double targetLocWCS[3];
      aEngagementDataPtr->mTargetTrackPtr->GetLocationWCS(targetLocWCS);
      UtVec3d::Subtract(weaponVelocityWCS, targetLocWCS, myLocWCS);
      UtVec3d::Normalize(weaponVelocityWCS);
      UtVec3d::Multiply(weaponVelocityWCS, UtMath::cLIGHT_SPEED);
      aEngagementDataPtr->mEngagementPtr->SetInitialWeaponVelocityWCS(weaponVelocityWCS);
   }

   return success;
}

//! Set the beam structure.  The instance assumes ownership of the passed object.
void WsfDirectedEnergyWeapon::SetBeam(WsfDirectedEnergyWeapon::Beam* aBeamPtr)
{
   if (mBeamPtr != nullptr)
   {
      delete mBeamPtr;
   }
   mBeamPtr = aBeamPtr;
}

WsfDirectedEnergyWeapon::Beam::Beam()
   : WsfObject()
   , mAveragePower(0.0)
   , mInitialPower(0.0)
   , mEnergyInSpot(0.0)
   , mPeakFluence(0.0)
   , mEdgeFluence(0.0)
   , mPeakIrradiance(0.0)
   , mAverageIrradiance(0.0)
   , mEdgeIrradiance(0.0)
   , mPulseRepetitionFrequency(0.0)
   , mPulseWidth(0.0)
   , mEdgeRadius(0.0)
   , mSpotSize(0.0)
   , mIncidenceAngle(0.0)
   , mCosIncidenceAngle(1.0)
   , mWavelength(0.0)
   , mWavelength_nm()
   , mApertureDiameter(0.0)
{
   for (int i = 0; i < 3; ++i)
   {
      mOffsetLocationECS[i] = 0.0;
   }
}

WsfDirectedEnergyWeapon::Beam::Beam(const Beam& aSrc)
   : WsfObject(aSrc)
   , mAveragePower(aSrc.mAveragePower)
   , mInitialPower(aSrc.mInitialPower)
   , mEnergyInSpot(aSrc.mEnergyInSpot)
   , mPeakFluence(aSrc.mPeakFluence)
   , mEdgeFluence(aSrc.mEdgeFluence)
   , mPeakIrradiance(aSrc.mPeakIrradiance)
   , mAverageIrradiance(aSrc.mAverageIrradiance)
   , mEdgeIrradiance(aSrc.mEdgeIrradiance)
   , mPulseRepetitionFrequency(aSrc.mPulseRepetitionFrequency)
   , mPulseWidth(aSrc.mPulseWidth)
   , mEdgeRadius(aSrc.mEdgeRadius)
   , mSpotSize(aSrc.mSpotSize)
   , mIncidenceAngle(aSrc.mIncidenceAngle)
   , mCosIncidenceAngle(aSrc.mCosIncidenceAngle)
   , mWavelength(aSrc.mWavelength)
   , mWavelength_nm(aSrc.mWavelength_nm)
   , mApertureDiameter(aSrc.mApertureDiameter)
{
   for (int i = 0; i < 3; ++i)
   {
      mOffsetLocationECS[i] = aSrc.mOffsetLocationECS[i];
   }
}

// virtual
bool WsfDirectedEnergyWeapon::Beam::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = true;
   if (command == "power")
   {
      double power;
      aInput.ReadValueOfType(power, UtInput::cPOWER);
      SetInitialPower(power);
   }
   else if (command == "aperture_diameter")
   {
      double diameter;
      aInput.ReadValueOfType(diameter, UtInput::cLENGTH);
      SetApertureDiameter(diameter);
   }
   else if (command == "wavelength")
   {
      double wavelength;
      aInput.ReadValueOfType(wavelength, UtInput::cLENGTH);
      SetWavelength(wavelength);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

//! Initialize the beam structure elements to initial values.
void WsfDirectedEnergyWeapon::Beam::Initialize()
{
   mAveragePower      = 0.0;
   mEnergyInSpot      = 0.0;
   mPeakFluence       = 0.0;
   mEdgeFluence       = 0.0;
   mPeakIrradiance    = 0.0;
   mEdgeIrradiance    = 0.0;
   mAverageIrradiance = 0.0;
   mEdgeRadius        = 0.0;
   mIncidenceAngle    = 0.0;
   mCosIncidenceAngle = 1.0;
}

//! Get the beam's offset location in target entity coordinates.
void WsfDirectedEnergyWeapon::Beam::GetOffsetLocationECS(double aLocationECS[3]) const
{
   UtVec3d::Set(aLocationECS, mOffsetLocationECS);
}

//! Set the beam's offset location in target entity coordinates.
void WsfDirectedEnergyWeapon::Beam::SetOffsetLocationECS(const double aLocationECS[3])
{
   UtVec3d::Set(mOffsetLocationECS, aLocationECS);
}

//! Return the "semi-major axis" of the beam on the target,
//! defined to be the radius of the spot divided by the cosine of the incidence angle.
double WsfDirectedEnergyWeapon::Beam::GetSemiMajorAxis() const
{
   double semiMajorAxis = mSpotSize;
   if (mCosIncidenceAngle > 0.0)
   {
      semiMajorAxis /= mCosIncidenceAngle;
   }
   return semiMajorAxis;
}

//! Return the energy density in the beam, which is defined to be
//! the integrated energy divided by the spot area.
double WsfDirectedEnergyWeapon::Beam::GetEnergyDensity() const
{
   double energyDensity = 0.0;
   if (mEdgeRadius > 0.0)
   {
      energyDensity = mEnergyInSpot * mCosIncidenceAngle / (UtMath::cPI * mEdgeRadius * mEdgeRadius);
   }
   return energyDensity;
}

//! Set the incidence angle of the beam on target, with respect to the normal to the target's surface at the spot center.
void WsfDirectedEnergyWeapon::Beam::SetIncidenceAngle(double aIncidenceAngle)
{
   mIncidenceAngle    = aIncidenceAngle;
   mCosIncidenceAngle = cos(mIncidenceAngle);
}

void WsfDirectedEnergyWeapon::Beam::SetWavelength(double aWavelength)
{
   mWavelength    = aWavelength;
   mWavelength_nm = ((unsigned)(aWavelength * 1.0e9));
}

// virtual
WsfWeapon::FireResult WsfDirectedEnergyWeapon::Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings)
{
   FireResult result;
   bool success = (aTarget.mTrackPtr != nullptr) || (GetPlatform()->GetTrackManager().GetCurrentTarget() != nullptr) ||
                  !aTarget.mTargetName.empty();
   // Note: The directed energy fire must be sent before the standard fire pdu.
   // Therefore we must defer the base class call until the end.
   // Try to ensure the base class call will succeed.
   if (success)
   {
      // Inform observers that we have begun this enagagement
      WsfObserver::DirectedEnergyWeaponFired(GetSimulation())(aSimTime, this, GetEngagement());
      result = WsfImplicitWeapon::Fire(aSimTime, aTarget, aSettings);
   }
   mIsFiring = result.mSuccess;
   return result;
}


//! Return whether the weapon is currently firing.
//! The default implementation returns true if the weapon
//! currently has an engagement.
bool WsfDirectedEnergyWeapon::IsFiring() const
{
   return mIsFiring;
}

// override
void WsfDirectedEnergyWeapon::EngagementComplete(double aSimTime, WsfWeaponEngagement* aEngagementPtr)
{
   WsfPlatform* targetPlatformPtr = aEngagementPtr->GetTargetPlatform();

   if (targetPlatformPtr != nullptr)
   {
      // Provide target location and velocity as impact point
      double targetLocWCS[3];
      targetPlatformPtr->GetLocationWCS(targetLocWCS);
      aEngagementPtr->SetTargetLocationWCS(targetPlatformPtr, targetLocWCS);
      // calculate target vector
      WsfPlatform* weaponPlatformPtr = aEngagementPtr->GetFiringPlatform();
      if (weaponPlatformPtr != nullptr)
      {
         double weaponPlatformLocWCS[3];
         weaponPlatformPtr->GetLocationWCS(weaponPlatformLocWCS);
         double targetVecWCS[3];
         UtVec3d::Subtract(targetVecWCS, targetLocWCS, weaponPlatformLocWCS);
         UtVec3d::Normalize(targetVecWCS);
         double weaponVelWCS[3];
         UtVec3d::Multiply(weaponVelWCS, targetVecWCS, UtMath::cLIGHT_SPEED);
         aEngagementPtr->SetWeaponVelocityWCS(weaponVelWCS);
      }
   }

   WsfWeapon::EngagementComplete(aSimTime, aEngagementPtr);
}

void WsfDirectedEnergyWeapon::CeaseFire(double aSimTime)
{
   mIsFiring = false;
   WsfImplicitWeapon::CeaseFire(aSimTime);
}
