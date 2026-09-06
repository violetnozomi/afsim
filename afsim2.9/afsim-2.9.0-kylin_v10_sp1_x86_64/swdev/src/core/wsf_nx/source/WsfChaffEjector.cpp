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
#include "WsfChaffEjector.hpp"

#include <memory>
#include <typeinfo>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"
#include "WsfChaffWeapon.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfWeapon.hpp"

namespace
{
//! Component factory to process chaff ejector input on a chaff weapon
class ChaffEjectorComponentFactory : public WsfComponentFactory<WsfWeapon>
{
public:
   bool ProcessInput(UtInput& aInput, WsfWeapon& aWeapon) override
   {
      bool myCommand(false);
      if (aInput.GetCommand() == "ejector")
      {
         if (!aWeapon.IsA_TypeOf("WSF_CHAFF_WEAPON"))
         {
            throw UtInput::BadValue(aInput, "Chaff ejectors can only be added to chaff weapons!");
         }
         myCommand = true;
         std::string name;
         aInput.ReadValue(name);
         auto         ejectorPtr = ut::make_unique<WsfChaffEjector>(name);
         UtInputBlock block(aInput);
         block.ProcessInput(ejectorPtr.get());

         aWeapon.GetComponents().DeleteComponent<WsfChaffEjector>(ejectorPtr->GetComponentName());
         if (!aWeapon.GetComponents().AddComponent(ejectorPtr.get()))
         {
            throw UtInput::BadValue(aInput, "Duplicate chaff ejector: " + ejectorPtr->GetName());
         }
         ejectorPtr.release();
      }
      return myCommand;
   }
};
} // namespace

WsfChaffEjector::WsfChaffEjector()
   : WsfSimpleWeaponComponent(cCOMPONENT_ROLE<WsfChaffEjector>())
   , UtEntityPart()
   , mQuantityRemaining(10)
   , mParcelType()
   , mEjectionVelocity(15.0)
   , mEjectionElevation(0.0)
   , mEjectionAzimuth(0.0)
{
}

WsfChaffEjector::WsfChaffEjector(const std::string& aName)
   : WsfSimpleWeaponComponent(cCOMPONENT_ROLE<WsfChaffEjector>(), aName)
   , UtEntityPart()
   , mQuantityRemaining(10)
   , mParcelType()
   , mEjectionVelocity(15.0)
   , mEjectionElevation(0.0)
   , mEjectionAzimuth(0.0)
{
}

WsfChaffEjector::WsfChaffEjector(const WsfChaffEjector& aSrc)
   : WsfSimpleWeaponComponent(aSrc)
   , UtEntityPart(aSrc)
   , mQuantityRemaining(aSrc.mQuantityRemaining)
   , mParcelType(aSrc.mParcelType)
   , mEjectionVelocity(aSrc.mEjectionVelocity)
   , mEjectionElevation(aSrc.mEjectionElevation)
   , mEjectionAzimuth(aSrc.mEjectionAzimuth)
{
}

WsfChaffEjector::~WsfChaffEjector()
{
   // Detach the observer from the platform.
   if (GetPlatform())
   {
      GetPlatform()->DetachObserver(this);
   }
}

void WsfChaffEjector::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<ChaffEjectorComponentFactory>());
}

WsfComponent* WsfChaffEjector::CloneComponent() const
{
   return new WsfChaffEjector(*this);
}

void* WsfChaffEjector::QueryInterface(int aRole)
{
   return (aRole == cCOMPONENT_ROLE<WsfChaffEjector>()) ? this : nullptr;
}

bool WsfChaffEjector::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "quantity")
   {
      aInput.ReadValue(mQuantityRemaining);
      aInput.ValueGreater(mQuantityRemaining, 0U);
   }
   else if (command == "parcel_type")
   {
      aInput.ReadValue(mParcelType);
   }
   else if (command == "location")
   {
      std::string units;
      double      location[3];
      aInput.ReadValue(location[0]);
      aInput.ReadValue(location[1]);
      aInput.ReadValue(location[2]);
      aInput.ReadValue(units);
      double multiplier = aInput.ConvertValue(1.0, units, UtInput::cLENGTH);
      UtVec3d::Multiply(location, multiplier);
      UtEntityPart::SetLocation(location);
   }
   else if (command == "ejection_velocity")
   {
      aInput.ReadValueOfType(mEjectionVelocity, UtInput::cSPEED);
      aInput.ValueGreater(mEjectionVelocity, 0.0);
   }
   else if (command == "ejection_elevation")
   {
      aInput.ReadValueOfType(mEjectionElevation, UtInput::cANGLE);
      aInput.ValueInClosedRange(mEjectionElevation, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }
   else if (command == "ejection_azimuth")
   {
      aInput.ReadValueOfType(mEjectionAzimuth, UtInput::cANGLE);
      aInput.ValueInClosedRange(mEjectionAzimuth, -UtMath::cPI, UtMath::cPI);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

bool WsfChaffEjector::Initialize(double aSimTime)
{
   bool ok = true;

   // Verify the parcel type is defined
   if (!GetWeapon()->GetScenario().FindType("chaff_parcel", mParcelType))
   {
      auto logger = ut::log::error() << "Chaff parcel type is not defined for ejector.";
      logger.AddNote() << "Platform: " << GetPlatform()->GetName();
      logger.AddNote() << "Weapon: " << GetWeapon()->GetName();
      logger.AddNote() << "Parcel Type: " << mParcelType;
      logger.AddNote() << "Ejector: " << GetName();

      ok = false;
   }

   // Set orientation of Part Coordinate System in UtEntityPart
   UtEntityPart::SetOrientation(mEjectionAzimuth, mEjectionElevation, 0.0);

   // Define the entity to which the ejector part is attached
   UtEntityPart::SetOwningEntity(GetPlatform());
   GetPlatform()->AttachObserver(this);

   return ok;
}

void WsfChaffEjector::SetLaunchState(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (GetOwningEntity()) // Ejector must be attached to a platform for kinematic calculations
   {
      // Ensure the position of the platform is current. If the platform location is not current then WsfPlatform
      // will invoke our OnPlatformUpdated method via the WsfSinglePlatformObserver interface.
      GetPlatform()->Update(aSimTime);

      // LOCATION
      double weaponLocWCS[3];
      GetLocationWCS(weaponLocWCS);
      aPlatformPtr->SetLocationWCS(weaponLocWCS);

      // VELOCITY
      // Translate chaff ejection velocity from ECS to WCS and add to launcher velocity to compute initial velocity
      double launcherVelWCS[3];
      GetPlatform()->GetVelocityWCS(launcherVelWCS);

      UtVec3dX ejectVelECS(mEjectionAzimuth, mEjectionElevation, mEjectionVelocity, true);
      double   ejectVelWCS[3];
      GetPlatform()->ConvertECSVectorToWCS(ejectVelWCS, ejectVelECS.GetData());

      double weaponVelWCS[3];
      UtVec3d::Add(weaponVelWCS, launcherVelWCS, ejectVelWCS);
      aPlatformPtr->SetVelocityWCS(weaponVelWCS);

      // ORIENTATION
      // Chaff parcels are modeled as spheres so orientation doesn't matter for kinematics, but in order to compute
      // presented area of cloud for parcel RCS later, we need relative locations of each parcel along the longitudinal
      // axis of the cloud, which is defined by the initial velocity vector of first parcel. So we orient the parcel
      // with its initial velocity vector.
      double psiWCS   = atan2(weaponVelWCS[1], weaponVelWCS[0]);
      double xy       = sqrt(weaponVelWCS[1] * weaponVelWCS[1] + weaponVelWCS[0] * weaponVelWCS[0]);
      double thetaWCS = -atan2(weaponVelWCS[2], xy);
      aPlatformPtr->SetOrientationWCS(psiWCS, thetaWCS, 0.0);

      // ACCELERATION
      double weaponAclWCS[3] = {0.0, 0.0, 0.0};
      aPlatformPtr->SetAccelerationWCS(weaponAclWCS);
   }
}

// Ownership of cloned chaff parcel is passed to the caller
std::unique_ptr<WsfChaffParcel> WsfChaffEjector::EjectParcel(double aSimTime)
{
   if (mQuantityRemaining > 0)
   {
      std::unique_ptr<WsfChaffParcel> parcelPtr(
         dynamic_cast<WsfChaffParcel*>(GetWeapon()->GetScenario().CloneType("chaff_parcel", mParcelType)));

      if (GetWeapon()->DebugEnabled())
      {
         auto logger = ut::log::debug() << "Dispensing chaff parcel.";
         logger.AddNote() << "Parcel Type: " << mParcelType;
         logger.AddNote() << "Ejector: " << GetName();
      }
      // Decrement quantity remaining
      --mQuantityRemaining;

      return parcelPtr;
   }

   return nullptr;
}
