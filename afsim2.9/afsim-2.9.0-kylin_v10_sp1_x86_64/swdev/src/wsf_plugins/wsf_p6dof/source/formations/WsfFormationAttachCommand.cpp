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

#include "WsfFormationAttachCommand.hpp"

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfFormation.hpp"
#include "WsfFormationUpdateStationKeeping.hpp"
#include "WsfP6DOF_Mover.hpp"

WsfFormationAttachCommand::WsfFormationAttachCommand()
   : WsfFormationCommand()
{
}

WsfFormationAttachCommand* WsfFormationAttachCommand::Clone() const
{
   return new WsfFormationAttachCommand{*this};
}

double WsfFormationAttachCommand::Execute(double aSimTime)
{
   WsfFormation* formationPtr = GetFormation();
   if (formationPtr)
   {
      if (GetDebug())
      {
         auto out = ut::log::debug() << "Executing command.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Command: " << Type();
         out.AddNote() << "Formation: " << formationPtr->GetQualifiedName();
      }
      formationPtr->SetAttached(true);

      if (formationPtr->IsLeaf())
      {
         WsfFormationUpdateStationKeeping::Update(formationPtr, aSimTime);
      }

      auto func = [aSimTime](WsfFormation* aFormationPtr)
      {
         auto cmd = ut::make_unique<WsfFormationAttachCommand>();
         aFormationPtr->ExecuteCommand(std::move(cmd), aSimTime);
      };
      formationPtr->InvokeOnSubFormations(func);
   }
   return -1.0;
}

bool WsfFormationAttachCommand::AcceptCommand(WsfFormation* aFormationPtr)
{
   return true;
}

void WsfFormationAttachCommand::ComputeCommonTransformation(WsfFormation* aFormationPtr) {}

std::unique_ptr<WsfFormationCommand> WsfFormationAttachCommand::ComputeTransformation(WsfFormation* aParentPtr,
                                                                                      WsfFormation* aChildPtr)
{
   return nullptr;
}
