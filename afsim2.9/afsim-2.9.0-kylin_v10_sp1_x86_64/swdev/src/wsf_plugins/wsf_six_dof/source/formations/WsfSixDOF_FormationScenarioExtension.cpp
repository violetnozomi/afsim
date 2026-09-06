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

#include "WsfSixDOF_FormationScenarioExtension.hpp"

#include "wsf_six_dof_export.h"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlugin.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_FormationSection.hpp"
#include "WsfSixDOF_FormationSimulationExtension.hpp"
#include "WsfSixDOF_FormationUnit.hpp"

wsf::six_dof::FormationScenarioExtension::FormationScenarioExtension()
   : WsfScenarioExtension()
   , mManagerPtr{nullptr}
{
}

wsf::six_dof::FormationScenarioExtension::~FormationScenarioExtension() {}

void wsf::six_dof::FormationScenarioExtension::AddedToScenario()
{
   WsfScenario& scenario = GetScenario();

   mManagerPtr.reset(new FormationManager{scenario, nullptr});

   bool ok{true};
   ok &= mManagerPtr->AddType("six_dof_unit", ut::make_unique<FormationUnit>(*mManagerPtr, "unit"));
   ok &= mManagerPtr->AddType("six_dof_section", ut::make_unique<FormationSection>(*mManagerPtr, "section"));
   ok &= mManagerPtr->AddType("six_dof_formation", ut::make_unique<Formation>(*mManagerPtr, "formation"));
   if (!ok)
   {
      throw std::runtime_error("ERROR: wsf_six_dof_formation: unable to register basic formation types.");
   }
}

bool wsf::six_dof::FormationScenarioExtension::ProcessInput(UtInput& aInput)
{
   bool        myCommand{false};
   std::string command = aInput.GetCommand();

   if (command == "six_dof_formation")
   {
      UtInputBlock inputBlock(aInput);

      std::string formName{};
      if (!inputBlock.ReadCommand())
      {
         throw UtInput::BadValue(aInput, "formations must be given a relative name.");
      }
      formName = inputBlock.GetCommand();
      // NOTE: Right now the only top level type that can be created is a formation, so we
      // reuse 'command' here. In the future, there may be a specific type in the input.
      Formation* inputForm = mManagerPtr->CreateFormation(command, formName);
      if (!inputForm)
      {
         throw UtInput::BadValue(aInput, "Error creating formation.");
      }

      while (inputBlock.ReadCommand())
      {
         if (!inputForm->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      if (!inputForm->GetOffset().IsZero())
      {
         auto out = ut::log::warning() << "Top level formation was given non-zero offset. This offset will be ignored.";
         out.AddNote() << "Formation: " << inputForm->GetQualifiedName();
         out.AddNote() << "Location: " << aInput.GetLocation();
         inputForm->SetOffset(FormationOffset{});
      }

      myCommand = true;
   }

   return myCommand;
}

bool wsf::six_dof::FormationScenarioExtension::Complete2()
{
   bool retval = mManagerPtr->ValidateInitialFormations();
   if (!retval)
   {
      throw std::runtime_error("Unable to validate input formations.");
   }

   mManagerPtr->SetInitialMemberKinematics();

   return retval;
}

void wsf::six_dof::FormationScenarioExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension("wsf_six_dof_formation", ut::make_unique<FormationSimulationExtension>());
}
