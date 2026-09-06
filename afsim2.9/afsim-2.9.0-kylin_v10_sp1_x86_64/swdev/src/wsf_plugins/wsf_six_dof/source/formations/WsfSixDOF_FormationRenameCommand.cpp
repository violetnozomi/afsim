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

#include "WsfSixDOF_FormationRenameCommand.hpp"

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_FormationUpdateStationKeeping.hpp"

wsf::six_dof::FormationRenameCommand::FormationRenameCommand()
   : FormationCommand()
   , mNewName{}
{
}

wsf::six_dof::FormationRenameCommand* wsf::six_dof::FormationRenameCommand::Clone() const
{
   return new FormationRenameCommand{*this};
}

double wsf::six_dof::FormationRenameCommand::Execute(double aSimTime)
{
   std::string oldName = GetFormation()->GetQualifiedName();
   if (GetFormation()->SetRelativeName(mNewName))
   {
      if (GetDebug())
      {
         auto out = ut::log::debug() << "Executing command. Formation renamed.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Command: " << Type();
         out.AddNote() << "Old Name: " << oldName;
         out.AddNote() << "New Name: " << mNewName;
      }
      FormationUpdateStationKeeping::Update(GetFormation(), aSimTime);
   }
   else
   {
      auto out = ut::log::warning() << "Failed to rename formation. A formation already exists with the new name.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Old Name: " << oldName;
      out.AddNote() << "New Name: " << mNewName;
      out.AddNote() << "This command will complete having accomplished nothing.";
   }
   return -1.0;
}

bool wsf::six_dof::FormationRenameCommand::AcceptCommand(Formation* aFormationPtr)
{
   bool retval{false};
   if (mNewName.empty())
   {
      auto out = ut::log::warning() << "New formation name cannot be empty.";
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Command will be ignored.";
   }
   else
   {
      retval = true;
   }
   return retval;
}

void wsf::six_dof::FormationRenameCommand::ComputeCommonTransformation(Formation* aFormationPtr) {}

std::unique_ptr<wsf::six_dof::FormationCommand>
wsf::six_dof::FormationRenameCommand::ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr)
{
   return nullptr;
}
