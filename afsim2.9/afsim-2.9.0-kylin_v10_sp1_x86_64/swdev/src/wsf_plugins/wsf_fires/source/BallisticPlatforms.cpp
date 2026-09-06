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

#include "BallisticPlatforms.hpp"

#include <sstream>
//#include "BallisticPath.hpp"
#include "FiresMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfWeaponEngagement.hpp"
#include "dis/WsfDisFire.hpp"

namespace Fires
{

// static
WsfPlatform* BallisticPlatforms::CreatePlatformP(const std::string& aTypeName, WsfSimulation* aSimPtr)
{
   // Clone the platform; based on the type
   // WsfPlatform* prototypePtr = WsfPlatform::FindType(aTypeName);
   WsfPlatform* prototypePtr = dynamic_cast<WsfPlatform*>(aSimPtr->GetScenario().FindType("platform_type", aTypeName));
   WsfPlatform* weaponPtr    = nullptr;
   if (prototypePtr != nullptr)
   {
      weaponPtr = prototypePtr->Clone();
   }
   return weaponPtr;
}

// static
//! Given an engagement object, create a properly initialized clone of the given type.
//!@param aTypeName   The ballistic type to be cloned and initialized.
//!@param aEngagement The engagement object providing the initialization data for the cloned ballistic type.
//!@param aSimPtr     Pointer to the simulation.
WsfPlatform* BallisticPlatforms::CreatePlatform(const std::string&         aTypeName,
                                                const WsfWeaponEngagement& aEngagement,
                                                WsfSimulation*             aSimPtr)
{
   double targetLocWCS[3];
   aEngagement.GetTargetLocationAtLaunchWCS(targetLocWCS);
   WsfPlatform* firingPlatformPtr = aEngagement.GetFiringPlatform();
   WsfPlatform* weaponPlatformPtr = nullptr;
   if (firingPlatformPtr != nullptr)
   {
      double firingLocationWCS[3];
      firingPlatformPtr->GetLocationWCS(firingLocationWCS);
      weaponPlatformPtr = CreatePlatformP(aTypeName, aSimPtr);

      if (weaponPlatformPtr != nullptr)
      {
         weaponPlatformPtr->SetLocationWCS(firingLocationWCS);
         double targetLocNED[3];
         weaponPlatformPtr->ConvertWCSToNED(targetLocWCS, targetLocNED);
         double range          = UtVec3d::Magnitude(targetLocNED);
         double initialHeading = atan2(targetLocNED[1], targetLocNED[0]);
         InitializePlatform(weaponPlatformPtr, firingLocationWCS, initialHeading, range);
      }
   }
   return weaponPlatformPtr;
}

// static private
bool BallisticPlatforms::InitializePlatform(WsfPlatform* aWeaponPtr,
                                            const double aLocationWCS[3],
                                            double       aInitialBearing,
                                            double       aRange)
{
   // This is called only when it is known that aWeaponPtr is non-zero, but this suppresses cppcheck warning.
   if (aWeaponPtr == nullptr)
   {
      return false;
   }

   // Need to set initial position and velocity based on the fire pdu loc and vel.
   FiresMover* moverPtr = dynamic_cast<FiresMover*>(aWeaponPtr->GetMover());
   if (moverPtr == nullptr)
   {
      auto logError = ut::log::error() << "Weapon Type needs to have a fires mover!";
      logError.AddNote() << "Weapon Type: " << aWeaponPtr->GetType();
      throw UtException("Weapon type " + aWeaponPtr->GetType() + " needs to have a fires mover!");
   }
   aWeaponPtr->SetLocationWCS(aLocationWCS);
   moverPtr->SetImpactRange(aRange);
   moverPtr->SetInitialBearing(aInitialBearing);
   double lat, lon, alt;
   aWeaponPtr->GetLocationLLA(lat, lon, alt);
   auto logInfo = ut::log::info() << "BallisticPlatforms::InitializePlatform: Weapon Location: ";
   logInfo.AddNote() << "LLA: " << lat << ", " << lon << ", " << alt;
   return true;
}

// static
//! Create a properly initialized clone of the given ballistic type.
//!@param aTypeName The ballistic type to be cloned.
//!@param aFire The fire pdu information that provides the initialization data.
//!@param aSimPtr The pointer to the simulation.
WsfPlatform* BallisticPlatforms::CreatePlatform(const std::string& aTypeName, const DisFire& aFire, WsfSimulation* aSimPtr)
{
   WsfPlatform* weaponPtr = CreatePlatformP(aTypeName, aSimPtr);

   if (weaponPtr != nullptr)
   {
      // Need to set initial position and velocity based on the fire pdu loc and vel.
      double locX, locY, locZ;
      aFire.GetLocation(locX, locY, locZ);
      double locWCS[3] = {locX, locY, locZ};
      float  velX, velY, velZ;
      aFire.GetVelocity(velX, velY, velZ);
      double velWCS[3] = {velX, velY, velZ};
      double range     = aFire.GetRange();
      weaponPtr->SetLocationWCS(locWCS);
      weaponPtr->SetVelocityWCS(velWCS);
      double velNED[3];
      weaponPtr->GetVelocityNED(velNED);
      UtVec3d::Normalize(velNED);

      if (range < 1.0)
      {
         ut::log::warning() << "Range from fire PDU is too small.";
         range = 10000.0;
      }

      // find the n-e component and use to compute level ground range intercept
      double ne[3]          = {range * velNED[0], range * velNED[1], 0.0};
      double initialBearing = atan2(ne[1], ne[0]);

      InitializePlatform(weaponPtr, locWCS, initialBearing, range);

      // Set the marking to the fire event identifier's application and number
      std::ostringstream oss;
      oss << aFire.GetEvent().GetSite() << ":";
      oss << aFire.GetEvent().GetApplication() << ":";
      oss << aFire.GetEvent().GetNumber() << std::ends;
      std::string idStr = oss.str();
      weaponPtr->SetMarking(idStr);
      oss.clear();
      oss.str("");

      // Set the name based on the type and fire id
      oss << aTypeName << "(";
      oss << aFire.GetEvent().GetSite() << ":";
      oss << aFire.GetEvent().GetApplication() << ":";
      oss << aFire.GetEvent().GetNumber() << ")" << std::ends;
      std::string name = oss.str();
      weaponPtr->SetName(name);
   }

   return weaponPtr;
}

} // namespace Fires
