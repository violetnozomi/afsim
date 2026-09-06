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

#include "WsfFormationChangeLeadSubCommand.hpp"

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfFormation.hpp"
#include "WsfFormationUpdateStationKeeping.hpp"

WsfFormationChangeLeadSubCommand::WsfFormationChangeLeadSubCommand()
   : WsfFormationCommand()
   , mNewLeaderName{}
{
}

double WsfFormationChangeLeadSubCommand::Execute(double aSimTime)
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
      WsfFormationUpdateStationKeeping::Update(GetFormation(), aSimTime);
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

bool WsfFormationChangeLeadSubCommand::AcceptCommand(WsfFormation* aFormationPtr)
{
   return true;
}

void WsfFormationChangeLeadSubCommand::ComputeCommonTransformation(WsfFormation* aFormationPtr) {}

std::unique_ptr<WsfFormationCommand> WsfFormationChangeLeadSubCommand::ComputeTransformation(WsfFormation* aParentPtr,
                                                                                             WsfFormation* aChildPtr)
{
   return nullptr;
}
