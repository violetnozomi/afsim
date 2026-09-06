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

#include "WsfFormationDisbandCommand.hpp"

#include "UtLog.hpp"
#include "WsfFormation.hpp"
#include "WsfFormationManager.hpp"

namespace
{
constexpr double cEPSILON_SECONDS = 1.0e-10;
} // namespace

WsfFormationDisbandCommand::WsfFormationDisbandCommand()
   : WsfFormationCommand()
{
}

WsfFormationDisbandCommand* WsfFormationDisbandCommand::Clone() const
{
   return new WsfFormationDisbandCommand{*this};
}

double WsfFormationDisbandCommand::Execute(double aSimTime)
{
   if (GetDebug())
   {
      auto out = ut::log::debug() << "Executing command.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Formation: " << GetFormation()->GetQualifiedName();
   }
   WsfFormationManager& mgr = GetFormation()->GetManager();
   mgr.DisbandFormation(GetFormation()->GetQualifiedName());

   // Return a slightly advanced time here so that this command, which will
   // have become dropped after the call to WsfFormationManager::DisbandFormation,
   // will have its event execute once more to allow for this command to be deleted.
   return aSimTime + cEPSILON_SECONDS;
}

bool WsfFormationDisbandCommand::AcceptCommand(WsfFormation* aFormationPtr)
{
   bool retval{true};
   if (!aFormationPtr->IsRoot())
   {
      auto out = ut::log::warning() << "Command can only be given to a root formation.";
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Formation: " << aFormationPtr->GetQualifiedName();
      retval = false;
   }
   return retval;
}

void WsfFormationDisbandCommand::ComputeCommonTransformation(WsfFormation* aFormationPtr) {}

std::unique_ptr<WsfFormationCommand> WsfFormationDisbandCommand::ComputeTransformation(WsfFormation* aParentPtr,
                                                                                       WsfFormation* aChildPtr)
{
   return nullptr;
}
