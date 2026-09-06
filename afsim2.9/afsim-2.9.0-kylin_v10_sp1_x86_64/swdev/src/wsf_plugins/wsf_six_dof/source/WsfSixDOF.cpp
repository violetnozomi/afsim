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

#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlugin.hpp"
#include "WsfScenario.hpp"
#include "WsfScriptPointMassSixDOF_Mover.hpp"
#include "WsfScriptPointMassSixDOF_MoverTestObject.hpp"
#include "WsfScriptRigidBodySixDOF_Mover.hpp"
#include "WsfScriptRigidBodySixDOF_MoverTestObject.hpp"
#include "WsfScriptSixDOF_Mover.hpp"
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_TypeManager.hpp"
#include "formations/WsfScriptSixDOF_Formation.hpp"
#include "formations/WsfScriptSixDOF_FormationAddSubCommand.hpp"
#include "formations/WsfScriptSixDOF_FormationAttachCommand.hpp"
#include "formations/WsfScriptSixDOF_FormationChangeLeadSubCommand.hpp"
#include "formations/WsfScriptSixDOF_FormationChangeOffsetCommand.hpp"
#include "formations/WsfScriptSixDOF_FormationCommand.hpp"
#include "formations/WsfScriptSixDOF_FormationCommandSequence.hpp"
#include "formations/WsfScriptSixDOF_FormationDetachCommand.hpp"
#include "formations/WsfScriptSixDOF_FormationDisbandCommand.hpp"
#include "formations/WsfScriptSixDOF_FormationManager.hpp"
#include "formations/WsfScriptSixDOF_FormationManeuverLimitsCommand.hpp"
#include "formations/WsfScriptSixDOF_FormationOffset.hpp"
#include "formations/WsfScriptSixDOF_FormationRemoveSubCommand.hpp"
#include "formations/WsfScriptSixDOF_FormationRenameCommand.hpp"
#include "formations/WsfScriptSixDOF_FormationTurnToHeadingCommand.hpp"
#include "formations/WsfSixDOF_FormationScenarioExtension.hpp"
#include "maneuvers/WsfScriptSixDOF_FlightPathAngleManeuver.hpp"
#include "maneuvers/WsfScriptSixDOF_HoldCurrentSpeedManeuver.hpp"
#include "maneuvers/WsfScriptSixDOF_Maneuver.hpp"
#include "maneuvers/WsfScriptSixDOF_ManeuverConstraint.hpp"
#include "maneuvers/WsfScriptSixDOF_ManeuverSequence.hpp"
#include "maneuvers/WsfScriptSixDOF_PitchAngleManeuver.hpp"
#include "maneuvers/WsfScriptSixDOF_PitchDeltaManeuver.hpp"
#include "maneuvers/WsfScriptSixDOF_PitchGLoadManeuver.hpp"
#include "maneuvers/WsfScriptSixDOF_PitchRateManeuver.hpp"
#include "maneuvers/WsfScriptSixDOF_PursueManeuver.hpp"
#include "maneuvers/WsfScriptSixDOF_RelativeManeuver.hpp"
#include "maneuvers/WsfScriptSixDOF_RollAngleManeuver.hpp"
#include "maneuvers/WsfScriptSixDOF_RollDeltaManeuver.hpp"
#include "maneuvers/WsfScriptSixDOF_RollRateManeuver.hpp"
#include "maneuvers/WsfScriptSixDOF_ScriptedManeuver.hpp"
#include "maneuvers/WsfScriptSixDOF_SpeedKCAS_Maneuver.hpp"
#include "maneuvers/WsfScriptSixDOF_SpeedKTAS_Maneuver.hpp"
#include "maneuvers/WsfScriptSixDOF_SpeedMachManeuver.hpp"
#include "maneuvers/WsfScriptSixDOF_TurnToHeadingManeuver.hpp"

namespace wsf
{
namespace six_dof
{
class ApplicationExtension : public WsfApplicationExtension
{
   void AddedToApplication(WsfApplication& aApplication) override
   {
      UtScriptTypes* typesPtr = aApplication.GetScriptTypes();

      // Don't create these -- they are the base types for the classes below
      typesPtr->Register(ut::make_unique<script::Mover>("WsfSixDOF_Mover", typesPtr));
      typesPtr->Register(ut::make_unique<script::MoverTestObject>("WsfSixDOF_MoverTestObject", typesPtr));

      // Use one of the following
      typesPtr->Register(ut::make_unique<script::RigidBodyMover>("WsfRigidBodySixDOF_Mover", typesPtr));
      typesPtr->Register(ut::make_unique<script::RigidBodyMoverTestObject>("WsfRigidBodySixDOF_MoverTestObject", typesPtr));
      typesPtr->Register(ut::make_unique<script::PointMassMover>("WsfPointMassSixDOF_Mover", typesPtr));
      typesPtr->Register(ut::make_unique<script::PointMassMoverTestObject>("WsfPointMassSixDOF_MoverTestObject", typesPtr));

      // Maneuver script types
      typesPtr->Register(ut::make_unique<script::Maneuver>("WsfSixDOF_Maneuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::ManeuverConstraint>("WsfSixDOF_ManeuverConstraint", typesPtr));
      typesPtr->Register(ut::make_unique<script::ManeuverSequence>("WsfSixDOF_ManeuverSequence", typesPtr));

      // Elemental maneuvers
      typesPtr->Register(ut::make_unique<script::FlightPathAngleManeuver>("WsfSixDOF_FlightPathAngleManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::HoldCurrentSpeedManeuver>("WsfSixDOF_HoldCurrentSpeedManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::PitchAngleManeuver>("WsfSixDOF_PitchAngleManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::PitchDeltaManeuver>("WsfSixDOF_PitchDeltaManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::PitchGLoadManeuver>("WsfSixDOF_PitchGLoadManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::PitchRateManeuver>("WsfSixDOF_PitchRateManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::RollAngleManeuver>("WsfSixDOF_RollAngleManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::RollDeltaManeuver>("WsfSixDOF_RollDeltaManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::RollRateManeuver>("WsfSixDOF_RollRateManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::ScriptedManeuver>("WsfSixDOF_ScriptedManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::SpeedKCAS_Maneuver>("WsfSixDOF_SpeedKCAS_Maneuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::SpeedKTAS_Maneuver>("WsfSixDOF_SpeedKTAS_Maneuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::SpeedMachManeuver>("WsfSixDOF_SpeedMachManeuver", typesPtr));

      // Other maneuvers
      typesPtr->Register(ut::make_unique<script::TurnToHeadingManeuver>("WsfSixDOF_TurnToHeadingManeuver", typesPtr));

      // Relative Maneuvers
      typesPtr->Register(ut::make_unique<script::RelativeManeuver>("WsfSixDOF_RelativeManeuver", typesPtr));
      typesPtr->Register(ut::make_unique<script::PursueManeuver>("WsfSixDOF_PursueManeuver", typesPtr));

      // Formation script types
      typesPtr->Register(ut::make_unique<script::Formation>("WsfSixDOF_Formation", typesPtr));
      typesPtr->Register(ut::make_unique<script::FormationCommand>("WsfSixDOF_FormationCommand", typesPtr));
      typesPtr->Register(
         ut::make_unique<script::FormationCommandConstraint>("WsfSixDOF_FormationCommandConstraint", typesPtr));
      typesPtr->Register(ut::make_unique<script::FormationCommandSequence>("WsfSixDOF_FormationCommandSequence", typesPtr));
      typesPtr->Register(ut::make_unique<script::FormationManager>("WsfSixDOF_FormationManager", typesPtr));
      typesPtr->Register(ut::make_unique<script::FormationOffset>("WsfSixDOF_FormationOffset", typesPtr));

      // Formation command script types
      typesPtr->Register(ut::make_unique<script::FormationAddSubCommand>("WsfSixDOF_FormationAddSubCommand", typesPtr));
      typesPtr->Register(ut::make_unique<script::FormationAttachCommand>("WsfSixDOF_FormationAttachCommand", typesPtr));
      typesPtr->Register(
         ut::make_unique<script::FormationChangeLeadSubCommand>("WsfSixDOF_FormationChangeLeadSubCommand", typesPtr));
      typesPtr->Register(
         ut::make_unique<script::FormationChangeOffsetCommand>("WsfSixDOF_FormationChangeOffsetCommand", typesPtr));
      typesPtr->Register(ut::make_unique<script::FormationDetachCommand>("WsfSixDOF_FormationDetachCommand", typesPtr));
      typesPtr->Register(ut::make_unique<script::FormationDisbandCommand>("WsfSixDOF_FormationDisbandCommand", typesPtr));
      typesPtr->Register(
         ut::make_unique<script::FormationManeuverLimitsCommand>("WsfSixDOF_FormationManeuverLimitsCommand", typesPtr));
      typesPtr->Register(
         ut::make_unique<script::FormationRemoveSubCommand>("WsfSixDOF_FormationRemoveSubCommand", typesPtr));
      typesPtr->Register(ut::make_unique<script::FormationRenameCommand>("WsfSixDOF_FormationRenameCommand", typesPtr));
      typesPtr->Register(
         ut::make_unique<script::FormationTurnToHeadingCommand>("WsfSixDOF_FormationTurnToHeadingCommand", typesPtr));
   }

   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension("wsf_six_dof_formation", ut::make_unique<FormationScenarioExtension>());
      aScenario.RegisterExtension("wsf_six_dof_environment", ut::make_unique<Environment>());
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<TypeManager>());
   }
};
} // namespace six_dof
} // namespace wsf

void WSF_SIX_DOF_EXPORT Register_wsf_six_dof(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_six_dof"))
   {
      WSF_REGISTER_EXTENSION(aApplication, wsf_mil); // This extension REQUIRES the "wsf_mil" extension
      aApplication.RegisterExtension("wsf_six_dof", ut::make_unique<wsf::six_dof::ApplicationExtension>());
      aApplication.ExtensionDepends("wsf_six_dof", "wsf_mil", true);
      aApplication.ExtensionDepends("wsf_six_dof", "event_pipe", false);
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
   UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication) { Register_wsf_six_dof(aApplication); }
}
