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

#include "WsfSixDOF_ExplicitWeapon.hpp"

#include "WsfPlatform.hpp"
#include "WsfSixDOF_Mover.hpp"

// =================================================================================================

wsf::six_dof::ExplicitWeapon::ExplicitWeapon(WsfScenario& aScenario)
   : WsfExplicitWeapon(aScenario)
{
   // Set launched platform to dummy type so initialization does not fail.
   // This is unused by WSF_SIX_DOF_EXPLICIT_WEAPON.
   SetLaunchedPlatformType("WSF_PLATFORM");
}

WsfWeapon* wsf::six_dof::ExplicitWeapon::Clone() const
{
   return new ExplicitWeapon(*this);
}

bool wsf::six_dof::ExplicitWeapon::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   // Note:  For some commands, we want to "capture" them here, rather than let
   // WsfExplicitWeapon handle them. We just want to "warn" about these, rather
   // than treating them as errors. In particular, the "reloading" related
   // commands may be supported by SixDOF in the future, so these may become
   // valid at that time of introduction.
   if ((command == "quantity") || (command == "maximum_quantity"))
   {
      ut::log::warning() << "WSF_SIX_DOF_EXPLICIT_WEAPON does not utilize quantity or maximum_quantity.";
      double dummy;
      aInput.ReadValue(dummy);
   }
   else if ((command == "reload_increment") || (command == "reload_inventory") || (command == "reload_time"))
   {
      ut::log::warning() << "WSF_SIX_DOF_EXPLICIT_WEAPON does not utilize reload commands.";
      double dummy;
      aInput.ReadValue(dummy);
   }
   else if (command == "inhibit_while_reloading")
   {
      ut::log::warning() << "WSF_SIX_DOF_EXPLICIT_WEAPON does not utilize reloading commands.";
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
      ut::log::warning() << "WSF_SIX_DOF_EXPLICIT_WEAPON does not utilize launch_delta_v.";
   }
   else if (command == "ignore_launch_platform_velocity")
   {
      ut::log::warning() << "WSF_SIX_DOF_EXPLICIT_WEAPON does not utilize ignore_launch_platform_velocity.";
      aInput.ReadBool();
   }
   else if (command == "launched_platform_type")
   {
      ut::log::warning() << "WSF_SIX_DOF_EXPLICIT_WEAPON does not utilize launched_platform_type.";
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

WsfWeapon::FireResult wsf::six_dof::ExplicitWeapon::Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings)
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

   // Verify that the launcher is a SixDOF mover
   WsfPlatform* launchingPlatform = GetPlatform();
   WsfMover*    launchingMover    = launchingPlatform->GetMover();
   Mover*       launcherMover     = dynamic_cast<Mover*>(launchingMover);
   Mover*       wpnSubobject      = nullptr;

   if (launcherMover != nullptr)
   {
      wpnSubobject = launcherMover->GetSubobjectByBaseName(subobjectWeaponName);
   }
   else
   {
      // The parent platform is not a WSF_SIX_DOF_MOVER, so we cannot proceed
      return result;
   }

   // This will be the newly fired/jettisoned weapon platform
   WsfPlatform* wpnPlatformPtr = nullptr;

   // If we have a valid launcher SixDOF mover and a valid weapon subobject, we can proceed
   if (wpnSubobject != nullptr)
   {
      // Directly jettison the weapon subobject and receive the weapon platform
      wpnPlatformPtr = launcherMover->DirectJettisonSubobjectPlatform(wpnSubobject, aSimTime);

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
      auto out = ut::log::warning() << "Lack either a valid launcher SixDOF mover or a valid weapon subobject.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Subobject: " << subobjectWeaponName;
      return result;
   }
}
