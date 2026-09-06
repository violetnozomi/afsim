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

#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfP6DOF_TypeManager.hpp"
#include "WsfPlugin.hpp"
#include "WsfScenario.hpp"
#include "WsfScriptP6DOF_Mover.hpp"
#include "WsfScriptP6DOF_MoverTestObject.hpp"
#include "formations/WsfFormationScenarioExtension.hpp"
#include "formations/WsfScriptFormation.hpp"
#include "formations/WsfScriptFormationAddSubCommand.hpp"
#include "formations/WsfScriptFormationAttachCommand.hpp"
#include "formations/WsfScriptFormationChangeLeadSubCommand.hpp"
#include "formations/WsfScriptFormationChangeOffsetCommand.hpp"
#include "formations/WsfScriptFormationCommand.hpp"
#include "formations/WsfScriptFormationCommandSequence.hpp"
#include "formations/WsfScriptFormationDetachCommand.hpp"
#include "formations/WsfScriptFormationDisbandCommand.hpp"
#include "formations/WsfScriptFormationManager.hpp"
#include "formations/WsfScriptFormationManeuverLimitsCommand.hpp"
#include "formations/WsfScriptFormationOffset.hpp"
#include "formations/WsfScriptFormationRemoveSubCommand.hpp"
#include "formations/WsfScriptFormationRenameCommand.hpp"
#include "formations/WsfScriptFormationTurnToHeadingCommand.hpp"
#include "maneuvers/WsfScriptFlightPathAngleManeuver.hpp"
#include "maneuvers/WsfScriptHoldCurrentSpeedManeuver.hpp"
#include "maneuvers/WsfScriptManeuver.hpp"
#include "maneuvers/WsfScriptManeuverConstraint.hpp"
#include "maneuvers/WsfScriptManeuverSequence.hpp"
#include "maneuvers/WsfScriptPitchAngleManeuver.hpp"
#include "maneuvers/WsfScriptPitchDeltaManeuver.hpp"
#include "maneuvers/WsfScriptPitchGLoadManeuver.hpp"
#include "maneuvers/WsfScriptPitchRateManeuver.hpp"
#include "maneuvers/WsfScriptPursueManeuver.hpp"
#include "maneuvers/WsfScriptRelativeManeuver.hpp"
#include "maneuvers/WsfScriptRollAngleManeuver.hpp"
#include "maneuvers/WsfScriptRollDeltaManeuver.hpp"
#include "maneuvers/WsfScriptRollRateManeuver.hpp"
#include "maneuvers/WsfScriptScriptedManeuver.hpp"
#include "maneuvers/WsfScriptSpeedKCAS_Maneuver.hpp"
#include "maneuvers/WsfScriptSpeedKTAS_Maneuver.hpp"
#include "maneuvers/WsfScriptSpeedMachManeuver.hpp"
#include "maneuvers/WsfScriptTurnToHeadingManeuver.hpp"

namespace
{
class ApplicationExtension : public WsfApplicationExtension
{
   void AddedToApplication(WsfApplication& aApplication) override
   {
      UtScriptTypes* typesPtr = aApplication.GetScriptTypes();

      // Use one of the following
      typesPtr->Register(ut::make_unique<WsfScriptP6DOF_MoverClass>("WsfP6DOF_Mover", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptP6DOF_MoverTestObject>("WsfP6DOF_MoverTestObject", typesPtr));

      // Maneuver script types
      typesPtr->Register(ut::make_unique<WsfScriptManeuver>("WsfManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptManeuverConstraint>("WsfManeuverConstraint", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptManeuverSequence>("WsfManeuverSequence", typesPtr));

      // Elemental maneuvers
      typesPtr->Register(ut::make_unique<WsfScriptFlightPathAngleManeuver>("WsfFlightPathAngleManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptHoldCurrentSpeedManeuver>("WsfHoldCurrentSpeedManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptPitchAngleManeuver>("WsfPitchAngleManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptPitchDeltaManeuver>("WsfPitchDeltaManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptPitchGLoadManeuver>("WsfPitchGLoadManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptPitchRateManeuver>("WsfPitchRateManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptRollAngleManeuver>("WsfRollAngleManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptRollDeltaManeuver>("WsfRollDeltaManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptRollRateManeuver>("WsfRollRateManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptScriptedManeuver>("WsfScriptedManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptSpeedKCAS_Maneuver>("WsfSpeedKCAS_Maneuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptSpeedKTAS_Maneuver>("WsfSpeedKTAS_Maneuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptSpeedMachManeuver>("WsfSpeedMachManeuver", typesPtr));

      // Other maneuvers
      typesPtr->Register(ut::make_unique<WsfScriptTurnToHeadingManeuver>("WsfTurnToHeadingManeuver", typesPtr));

      // Relative Maneuvers
      typesPtr->Register(ut::make_unique<WsfScriptRelativeManeuver>("WsfRelativeManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptPursueManeuver>("WsfPursueManeuver", typesPtr));

      // Formation script types
      typesPtr->Register(ut::make_unique<WsfScriptFormation>("WsfFormation", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptFormationCommand>("WsfFormationCommand", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptFormationCommandConstraint>("WsfFormationCommandConstraint", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptFormationCommandSequence>("WsfFormationCommandSequence", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptFormationManager>("WsfFormationManager", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptFormationOffset>("WsfFormationOffset", typesPtr));

      // Formation command script types
      typesPtr->Register(ut::make_unique<WsfScriptFormationAddSubCommand>("WsfFormationAddSubCommand", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptFormationAttachCommand>("WsfFormationAttachCommand", typesPtr));
      typesPtr->Register(
         ut::make_unique<WsfScriptFormationChangeLeadSubCommand>("WsfFormationChangeLeadSubCommand", typesPtr));
      typesPtr->Register(
         ut::make_unique<WsfScriptFormationChangeOffsetCommand>("WsfFormationChangeOffsetCommand", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptFormationDetachCommand>("WsfFormationDetachCommand", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptFormationDisbandCommand>("WsfFormationDisbandCommand", typesPtr));
      typesPtr->Register(
         ut::make_unique<WsfScriptFormationManeuverLimitsCommand>("WsfFormationManeuverLimitsCommand", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptFormationRemoveSubCommand>("WsfFormationRemoveSubCommand", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptFormationRenameCommand>("WsfFormationRenameCommand", typesPtr));
      typesPtr->Register(
         ut::make_unique<WsfScriptFormationTurnToHeadingCommand>("WsfFormationTurnToHeadingCommand", typesPtr));
   }

   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<WsfP6DOF_TypeManager>());
      aScenario.RegisterExtension("wsf_formation", ut::make_unique<WsfFormationScenarioExtension>());
   }
};
} // namespace

void WSF_P6DOF_EXPORT Register_wsf_p6dof(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_p6dof"))
   {
      aApplication.RegisterFeature("p6dof", "wsf_p6dof");
      WSF_REGISTER_EXTENSION(aApplication, wsf_mil); // This extension REQUIRES the "wsf_mil" extension
      aApplication.RegisterExtension("wsf_p6dof", ut::make_unique<ApplicationExtension>());
      aApplication.ExtensionDepends("wsf_p6dof", "wsf_mil", true);
      aApplication.ExtensionDepends("wsf_p6dof", "event_pipe", false);
   }
}

extern "C"
{
   //! This method is called when the plugin is loaded to ensure that the plugin and the executable loading it were
   //! built with the same version of the plugin API.
   UT_PLUGIN_EXPORT void WsfPluginVersion(UtPluginVersion& aVersion)
   {
      aVersion =
         UtPluginVersion(WSF_PLUGIN_API_MAJOR_VERSION, WSF_PLUGIN_API_MINOR_VERSION, WSF_PLUGIN_API_COMPILER_STRING);
   }

   //! This method is called when the plugin is loaded. It must have exactly this signature (name and parameter) to
   //! succeed. As we only have an application reference at load time, one usually registers an application extension,
   //! within which the ScenarioCreated method can be overridden to gain access to a scenario.  If one also needs access
   //! to the simulation, one should also instantiate and register a simulation extension by overriding the
   //! SimulationCreated method in the scenario extension.
   UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication) { Register_wsf_p6dof(aApplication); }
}
