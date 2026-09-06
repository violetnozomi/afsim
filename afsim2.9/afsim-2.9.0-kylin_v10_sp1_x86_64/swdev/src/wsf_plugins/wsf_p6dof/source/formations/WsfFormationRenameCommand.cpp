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

#include "WsfFormationRenameCommand.hpp"

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfFormation.hpp"
#include "WsfFormationUpdateStationKeeping.hpp"

WsfFormationRenameCommand::WsfFormationRenameCommand()
   : WsfFormationCommand()
   , mNewName{}
{
}

WsfFormationRenameCommand* WsfFormationRenameCommand::Clone() const
{
   return new WsfFormationRenameCommand{*this};
}

double WsfFormationRenameCommand::Execute(double aSimTime)
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
      WsfFormationUpdateStationKeeping::Update(GetFormation(), aSimTime);
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

bool WsfFormationRenameCommand::AcceptCommand(WsfFormation* aFormationPtr)
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

void WsfFormationRenameCommand::ComputeCommonTransformation(WsfFormation* aFormationPtr) {}

std::unique_ptr<WsfFormationCommand> WsfFormationRenameCommand::ComputeTransformation(WsfFormation* aParentPtr,
                                                                                      WsfFormation* aChildPtr)
{
   return nullptr;
}
