// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfP6DOF_ExplicitWeapon.hpp"

#include "P6DofVehicle.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"

// =================================================================================================

WsfP6DOF_ExplicitWeapon::WsfP6DOF_ExplicitWeapon(WsfScenario& aScenario)
   : WsfExplicitWeapon(aScenario)
{
   // Set launched platform to dummy type so initialization does not fail.
   // This is unused by WSF_P6DOF_EXPLICIT_WEAPON.
   SetLaunchedPlatformType("WSF_PLATFORM");
}

WsfWeapon* WsfP6DOF_ExplicitWeapon::Clone() const
{
   return new WsfP6DOF_ExplicitWeapon(*this);
}

bool WsfP6DOF_ExplicitWeapon::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   // Note:  For some commands, we want to "capture" them here, rather than let
   // WsfExplicitWeapon handle them. We just want to "warn" about these, rather
   // than treating them as errors. In particular, the "reloading" related
   // commands may be supported by P6DOF in the future, so these may become
   // valid at that time of introduction.
   if ((command == "quantity") || (command == "maximum_quantity"))
   {
      auto   out = ut::log::warning() << "WSF_P6DOF_EXPLICIT_WEAPON does not utilize quantity or maximum_quantity.";
      double dummy;
      aInput.ReadValue(dummy);
   }
   else if ((command == "reload_increment") || (command == "reload_inventory") || (command == "reload_time"))
   {
      auto   out = ut::log::warning() << "WSF_P6DOF_EXPLICIT_WEAPON does not utilize reload commands.";
      double dummy;
      aInput.ReadValue(dummy);
   }
   else if (command == "inhibit_while_reloading")
   {
      auto out = ut::log::warning() << "WSF_P6DOF_EXPLICIT_WEAPON does not utilize reloading commands.";
      aInput.ReadBool(); // Be sure t read the value, even though we don't use it
   }
   else if (command == "launch_delta_v")
   {
      std::string units;
      aInput.ReadValue(mLaunchDeltaV_PCS[0]);
      aInput.ReadValue(mLaunchDeltaV_PCS[1]);
      aInput.ReadValue(mLaunchDeltaV_PCS[2]);
      aInput.ReadValue(units);
      double multiplier = aInput.ConvertValue(1.0, units, UtInput::cSPEED);
      UtVec3d::Multiply(mLaunchDeltaV_PCS, multiplier);
      auto out = ut::log::warning() << "WSF_P6DOF_EXPLICIT_WEAPON does not utilize launch_delta_v.";
   }
   else if (command == "ignore_launch_platform_velocity")
   {
      auto out = ut::log::warning() << "WSF_P6DOF_EXPLICIT_WEAPON does not utilize ignore_launch_platform_velocity.";
      aInput.ReadBool();
   }
   else if (command == "launched_platform_type")
   {
      auto        out = ut::log::warning() << "WSF_P6DOF_EXPLICIT_WEAPON does not utilize launched_platform_type.";
      std::string dummy;
      aInput.ReadValue(dummy); // Be sure to read the value, even though we don't use it
   }
   else if (command == "add_subobject")
   {
      std::string subobjectName;
      aInput.ReadValue(subobjectName);
      mSubobjectList.push_back(subobjectName);

      // Increment the quantity of weapons remaining
      double qty = static_cast<double>(mSubobjectList.size());
      SetQuantityRemaining(qty);
   }
   else
   {
      myCommand = WsfExplicitWeapon::ProcessInput(aInput);
   }

   return myCommand;
}

WsfWeapon::FireResult WsfP6DOF_ExplicitWeapon::Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings)
{
   WsfWeapon::FireResult result;

   // Get the name of the subobject weapon from the weapon list
   std::string subobjectWeaponName;
   if (!mSubobjectList.empty())
   {
      subobjectWeaponName = mSubobjectList.front();
      mSubobjectList.pop_front();
   }
   else
   {
      // There are no remaining weapons is list, so return
      return result;
   }

   // Verify that the launcher is a P6DOF mover
   WsfPlatform*    launchingPlatform  = GetPlatform();
   WsfMover*       launchingMover     = launchingPlatform->GetMover();
   WsfP6DOF_Mover* launcherP6dofMover = nullptr;
   P6DofVehicle*   wpnSubobject       = nullptr;
   if (launchingMover->IsA_TypeOf("WSF_P6DOF_MOVER"))
   {
      launcherP6dofMover = dynamic_cast<WsfP6DOF_Mover*>(launchingMover);

      P6DofVehicle* launcherP6DofVehicle = launcherP6dofMover->GetP6DofVehicle();

      // Loop through the subobjects on the vehicle, looking for a match
      auto& subobjectList = launcherP6DofVehicle->GetSubobjectList();
      for (const auto& obj : subobjectList)
      {
         if (obj->GetBaseName() == subobjectWeaponName)
         {
            wpnSubobject = obj;
         }
      }
   }
   else
   {
      // The parent platform is not a WSF_P6DOF_MOVER, so we cannot proceed
      return result;
   }

   // This will be the newly fired/jettisoned weapon platform
   WsfPlatform* wpnPlatformPtr = nullptr;

   // If we have a valid launcher p6dof mover and a valid weapon subobject, we can proceed
   if ((launcherP6dofMover != nullptr) && (wpnSubobject != nullptr))
   {
      // Directly jettison the weapon subobject and receive the weapon platform
      wpnPlatformPtr = launcherP6dofMover->DirectJettisonSubobjectPlatform(wpnSubobject, aSimTime);

      if (wpnPlatformPtr == nullptr)
      {
         auto out = ut::log::warning() << "No weapon platform available.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Subobject: " << subobjectWeaponName;
         return result;
      }

      return FireP(aSimTime, aTarget, aSettings, wpnPlatformPtr);
   }
   else
   {
      auto out = ut::log::warning() << "Lack either a valid launcher p6dof mover or a valid weapon subobject.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Subobject: " << subobjectWeaponName;
      return result;
   }
}
