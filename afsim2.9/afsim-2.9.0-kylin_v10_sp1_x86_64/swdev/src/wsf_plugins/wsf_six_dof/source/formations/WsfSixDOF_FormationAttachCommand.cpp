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

#include "WsfSixDOF_FormationAttachCommand.hpp"

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_FormationUpdateStationKeeping.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::FormationAttachCommand::FormationAttachCommand()
   : FormationCommand()
{
}

wsf::six_dof::FormationAttachCommand* wsf::six_dof::FormationAttachCommand::Clone() const
{
   return new FormationAttachCommand{*this};
}

double wsf::six_dof::FormationAttachCommand::Execute(double aSimTime)
{
   Formation* formationPtr = GetFormation();
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
         FormationUpdateStationKeeping::Update(formationPtr, aSimTime);
      }

      auto func = [aSimTime](Formation* aFormationPtr)
      {
         auto cmd = ut::make_unique<FormationAttachCommand>();
         aFormationPtr->ExecuteCommand(std::move(cmd), aSimTime);
      };
      formationPtr->InvokeOnSubFormations(func);
   }
   return -1.0;
}

bool wsf::six_dof::FormationAttachCommand::AcceptCommand(Formation* aFormationPtr)
{
   return true;
}

void wsf::six_dof::FormationAttachCommand::ComputeCommonTransformation(Formation* aFormationPtr) {}

std::unique_ptr<wsf::six_dof::FormationCommand>
wsf::six_dof::FormationAttachCommand::ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr)
{
   return nullptr;
}
