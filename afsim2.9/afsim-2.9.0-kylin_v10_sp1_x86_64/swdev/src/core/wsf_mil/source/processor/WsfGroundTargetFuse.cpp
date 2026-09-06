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

#include "WsfGroundTargetFuse.hpp"

#include "UtInput.hpp"

WsfGroundTargetFuse::WsfGroundTargetFuse(WsfScenario& aScenario)
   : WsfWeaponFuse(aScenario)
{
   SetAirToGroundMode(true); // Also forces detonation when descending below 0.0 AGL
   SetUseCurrentTarget(true);
   SetGrossProximityRange(500.0);
}

WsfProcessor* WsfGroundTargetFuse::Clone() const
{
   return new WsfGroundTargetFuse(*this);
}

//! Primary purpose for this virtual method override is to prevent
//! use of internally incompatible commands.
// virtual
bool WsfGroundTargetFuse::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   // Filter out 'forbidden' commands:
   if ((command == "detonate_above_height_AGL") || (command == "detonate_above_height_agl") ||
       (command == "detonate_above_height_MSL") || (command == "detonate_above_height_msl") ||
       (command == "excluded_category"))
   {
      std::string scream = "The " + command + " option is not valid for a WsfGroundTargetFuse object.";
      throw UtInput::BadValue(aInput, scream);
   }
   else
   {
      myCommand = WsfWeaponFuse::ProcessInput(aInput);
   }
   return myCommand;
}
