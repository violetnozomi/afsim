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

#include "WsfAirTargetFuse.hpp"

#include "UtInput.hpp"

WsfAirTargetFuse::WsfAirTargetFuse(WsfScenario& aScenario)
   : WsfWeaponFuse(aScenario)
{
   SetUseCurrentTarget(true);
   SetAGL_Criteria(cDETONATE_BELOW, 0.0);
   SetGrossUpdateInterval(0.5); // Needs to be a little faster for air-air, and surface to air.
   SetGrossProximityRange(1500.00);
}

WsfProcessor* WsfAirTargetFuse::Clone() const
{
   return new WsfAirTargetFuse(*this);
}

//! Primary purpose for this virtual method override is to prevent
//! use of internally incompatible commands.
// virtual
bool WsfAirTargetFuse::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   // Filter out 'forbidden' commands:
   if ((command == "do_not_use_current_target") || (command == "continue_on_loss_of_target") ||
       (command == "air_to_ground_mode") || (command == "exclusive_target_name") || (command == "excluded_category"))
   {
      std::string note = "The " + command + " option is not valid for a WsfAirTargetFuse object.";
      throw UtInput::BadValue(aInput, note);
   }
   else
   {
      myCommand = WsfWeaponFuse::ProcessInput(aInput);
   }
   return myCommand;
}
