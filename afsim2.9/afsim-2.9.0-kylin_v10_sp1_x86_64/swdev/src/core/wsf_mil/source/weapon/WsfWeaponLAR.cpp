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

#include "WsfWeaponLAR.hpp"

#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfATG_WeaponLAR.hpp"

WsfWeaponLAR::WsfWeaponLAR()
   : WsfObject()
   , mSimulationPtr()
   , mDebugEnabled(false)
{
   // Should be overridden by derived types...
   SetType(WsfStringId("WSF_WEAPON_LAR"));
}

// virtual
bool WsfWeaponLAR::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "debug")
   {
      mDebugEnabled = true;
   }
   else
   {
      myCommand = WsfObject::ProcessInput(aInput);
      ;
   }
   return myCommand;
}

// virtual
bool WsfWeaponLAR::Initialize(WsfSimulation& aSimulation)
{
   mSimulationPtr = &aSimulation;
   return true;
}

// static
bool WsfWeaponLAR::LoadInstance(UtInput& aInput, WsfWeaponLAR*& aLARPtr)
{
   bool success = false;

   if (aInput.GetCommand() == "launch_acceptable_region")
   {
      std::string instanceName;
      aInput.ReadValue(instanceName);

      std::string baseType;
      aInput.ReadValue(baseType);

      if (baseType == WsfATG_WeaponLAR::GetTypeString())
      {
         std::unique_ptr<WsfATG_WeaponLAR> larPtr(new WsfATG_WeaponLAR());
         if (larPtr != nullptr)
         {
            larPtr->SetName(instanceName);
            UtInputBlock inputBlock(aInput, "end_launch_acceptable_region");
            dynamic_cast<WsfATG_WeaponLAR*>(larPtr.get())->ProcessInputBlock(inputBlock);
            WsfWeaponLAR* tempPtr = larPtr.release();
            aLARPtr               = dynamic_cast<WsfWeaponLAR*>(tempPtr);
            success               = true;
         }
      }
      // else if  //... other new LAR types are created, must add them in here...
      else if (baseType == WsfWeaponLAR::GetTypeString())
      {
         throw UtInput::BadValue(aInput, "Cannot create an instance of the WsfWeaponLAR base class.");
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown platform type: " + baseType);
      }
   }
   return success;
}
