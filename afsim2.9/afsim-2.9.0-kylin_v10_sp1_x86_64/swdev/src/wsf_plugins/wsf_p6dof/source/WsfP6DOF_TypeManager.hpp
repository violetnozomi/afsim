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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFP6DOF_TYPEMANAGER_HPP
#define WSFP6DOF_TYPEMANAGER_HPP

#include "P6DofTypeManager.hpp"
#include "WsfScenarioExtension.hpp"

class UtInput;
class WsfSimulation;

class WsfP6DOF_TypeManager : public WsfScenarioExtension, public P6DofTypeManager
{
public:
   WsfP6DOF_TypeManager();
   ~WsfP6DOF_TypeManager() override;

   // The extension has been added to the scenario. GetScenario() is now callable.
   // This adds "WSF_P6DOF_MOVER" (WsfP6DOF_Mover), "WSF_P6DOF_GUIDANCE_COMPUTER"
   // (WsfP6DOF_GuidanceComputer), "WSF_P6DOF_FUEL" (WsfP6DOF_Fuel), and
   // "WSF_P6DOF_EXPLICIT_WEAPON" (WsfP6DOF_ExplicitWeapon). It also registers
   // ScriptTypes for WsfScriptP6DOF_MoverClass (for WsfP6DOF_Mover).
   void AddedToScenario() override;

   // This reads "p6dof_object_types" input blocks
   bool ProcessInput(UtInput& aInput) override;

   // This calls WsfSimulation::RegisterExtension() every time a new simulation is
   // created, giving it a new P6DOF object manager.
   void SimulationCreated(WsfSimulation& aSimulation) override;

   // This returns the platform type name for the specified P6DOF vehicle type. If no match is
   // found, an empty string will be returned.
   std::string GetP6DofVehiclePlatformTypeMatch(const std::string& aNameP6DofVehicleType) const;

private:
   // This adds the specified P6DOF vehicle type and platform type matching to the master list,
   // as long as the vehicle type name is unique.
   void AddP6DofVehiclePlatformTypeMatch(const std::string& aNameP6DofVehicleType, const std::string& aNamePlatformType);

   struct ObjectPlatformMatch
   {
      std::string nameP6DofVehicleType;
      std::string namePlatformType;
   };

   // This provides a list of P6DOF vehicle to Platform matches
   std::list<ObjectPlatformMatch> mObjectPlatformMatchList;
};

#endif
