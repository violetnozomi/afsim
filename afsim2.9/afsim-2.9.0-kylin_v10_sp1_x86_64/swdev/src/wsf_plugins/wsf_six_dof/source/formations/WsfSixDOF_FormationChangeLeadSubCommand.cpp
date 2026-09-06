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

#include "WsfSixDOF_FormationChangeLeadSubCommand.hpp"

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_FormationUpdateStationKeeping.hpp"

wsf::six_dof::FormationChangeLeadSubCommand::FormationChangeLeadSubCommand()
   : FormationCommand()
   , mNewLeaderName{}
{
}

double wsf::six_dof::FormationChangeLeadSubCommand::Execute(double aSimTime)
{
   if (GetFormation()->SetLead(mNewLeaderName))
   {
      if (GetDebug())
      {
         auto out = ut::log::debug() << "Executing command. Setting child formation as lead.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Command: " << Type();
         out.AddNote() << "Parent: " << GetFormation()->GetQualifiedName();
         out.AddNote() << "Child: " << mNewLeaderName;
      }
      FormationUpdateStationKeeping::Update(GetFormation(), aSimTime);
   }
   else
   {
      auto out = ut::log::warning() << "Failed to set child as lead formation.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Parent: " << GetFormation()->GetQualifiedName();
      out.AddNote() << "Child: " << mNewLeaderName;
      out.AddNote() << "This command will complete having accomplished nothing.";
   }
   return -1.0;
}

bool wsf::six_dof::FormationChangeLeadSubCommand::AcceptCommand(Formation* aFormationPtr)
{
   return true;
}

void wsf::six_dof::FormationChangeLeadSubCommand::ComputeCommonTransformation(Formation* aFormationPtr) {}

std::unique_ptr<wsf::six_dof::FormationCommand>
wsf::six_dof::FormationChangeLeadSubCommand::ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr)
{
   return nullptr;
}
