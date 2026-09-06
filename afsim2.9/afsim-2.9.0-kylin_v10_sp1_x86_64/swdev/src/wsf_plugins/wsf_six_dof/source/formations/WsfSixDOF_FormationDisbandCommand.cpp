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

#include "WsfSixDOF_FormationDisbandCommand.hpp"

#include "UtLog.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_FormationManager.hpp"

namespace
{
constexpr double cEPSILON_SECONDS = 1.0e-10;
} // namespace

wsf::six_dof::FormationDisbandCommand::FormationDisbandCommand()
   : FormationCommand()
{
}

wsf::six_dof::FormationDisbandCommand* wsf::six_dof::FormationDisbandCommand::Clone() const
{
   return new FormationDisbandCommand{*this};
}

double wsf::six_dof::FormationDisbandCommand::Execute(double aSimTime)
{
   if (GetDebug())
   {
      auto out = ut::log::debug() << "Executing command.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Formation: " << GetFormation()->GetQualifiedName();
   }
   wsf::six_dof::FormationManager& mgr = GetFormation()->GetManager();
   mgr.DisbandFormation(GetFormation()->GetQualifiedName());

   // Return a slightly advanced time here so that this command, which will
   // have become dropped after the call to WsfSixDOF_FormationManager::DisbandFormation,
   // will have its event execute once more to allow for this command to be deleted.
   return aSimTime + cEPSILON_SECONDS;
}

bool wsf::six_dof::FormationDisbandCommand::AcceptCommand(Formation* aFormationPtr)
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

void wsf::six_dof::FormationDisbandCommand::ComputeCommonTransformation(Formation* aFormationPtr) {}

std::unique_ptr<wsf::six_dof::FormationCommand>
wsf::six_dof::FormationDisbandCommand::ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr)
{
   return nullptr;
}
