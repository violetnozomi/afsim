// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfFormationScenarioExtension.hpp"

#include "wsf_p6dof_export.h"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfFormation.hpp"
#include "WsfFormationSection.hpp"
#include "WsfFormationSimulationExtension.hpp"
#include "WsfFormationUnit.hpp"
#include "WsfPlugin.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

WsfFormationScenarioExtension::WsfFormationScenarioExtension()
   : WsfScenarioExtension()
   , mManagerPtr{nullptr}
{
}

WsfFormationScenarioExtension::~WsfFormationScenarioExtension() {}

void WsfFormationScenarioExtension::AddedToScenario()
{
   WsfScenario& scenario = GetScenario();

   mManagerPtr.reset(new WsfFormationManager{scenario, nullptr});

   bool ok{true};
   ok &= mManagerPtr->AddType("unit", ut::make_unique<WsfFormationUnit>(*mManagerPtr, "unit"));
   ok &= mManagerPtr->AddType("section", ut::make_unique<WsfFormationSection>(*mManagerPtr, "section"));
   ok &= mManagerPtr->AddType("formation", ut::make_unique<WsfFormation>(*mManagerPtr, "formation"));
   if (!ok)
   {
      throw std::runtime_error("ERROR: wsf_formation: unable to register basic formation types.");
   }
}

bool WsfFormationScenarioExtension::ProcessInput(UtInput& aInput)
{
   bool        myCommand{false};
   std::string command = aInput.GetCommand();

   if (command == "formation")
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
      WsfFormation* inputForm = mManagerPtr->CreateFormation(command, formName);
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
         inputForm->SetOffset(WsfFormationOffset{});
      }

      myCommand = true;
   }

   return myCommand;
}

bool WsfFormationScenarioExtension::Complete2()
{
   bool retval = mManagerPtr->ValidateInitialFormations();
   if (!retval)
   {
      throw std::runtime_error("Unable to validate input formations.");
   }

   mManagerPtr->SetInitialMemberKinematics();

   return retval;
}

void WsfFormationScenarioExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension("wsf_formation", ut::make_unique<WsfFormationSimulationExtension>());
}
