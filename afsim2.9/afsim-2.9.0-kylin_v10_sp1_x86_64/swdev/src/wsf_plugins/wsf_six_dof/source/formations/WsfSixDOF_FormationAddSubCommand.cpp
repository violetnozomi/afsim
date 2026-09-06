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

#include "WsfSixDOF_FormationAddSubCommand.hpp"

#include "UtLog.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_FormationManager.hpp"
#include "WsfSixDOF_FormationUpdateStationKeeping.hpp"

wsf::six_dof::FormationAddSubCommand::FormationAddSubCommand()
   : FormationCommand{}
   , mParentRelativeName{}
   , mFormName{}
   , mOffset{}
{
}

wsf::six_dof::FormationAddSubCommand* wsf::six_dof::FormationAddSubCommand::Clone() const
{
   return new FormationAddSubCommand{*this};
}

double wsf::six_dof::FormationAddSubCommand::Execute(double aSimTime)
{
   double     retval{-1.0};
   Formation* formPtr   = GetFormation();
   Formation* parentPtr = GetProposedParent(formPtr);
   if (VerifyProposedChild(formPtr) && parentPtr != nullptr)
   {
      Formation* formToAdd = formPtr->GetManager().GetFormation(mFormName);
      formToAdd->SetOffset(mOffset);
      bool ok = parentPtr->AddChildFormation(formToAdd);

      if (ok)
      {
         formToAdd->SetAttached(false);
         FormationUpdateStationKeeping::Update(formToAdd, aSimTime);
         if (GetDebug())
         {
            auto out = ut::log::debug() << "Executing command. Adding child formation to parent.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Command: " << Type();
            out.AddNote() << "Parent: " << formPtr->GetQualifiedName();
            out.AddNote() << "Child: " << mFormName;
         }
      }
      else
      {
         auto out = ut::log::warning() << "Failed to add child formation.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Command: " << Type();
         out.AddNote() << "Parent: " << formPtr->GetQualifiedName();
         out.AddNote() << "Child: " << mFormName;
         out.AddNote() << "This command will complete having accomplished nothing.";
      }
   }
   else
   {
      {
         // RAII block
         auto out = ut::log::warning() << "Formation does not exist or is not top level.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Command: " << Type();
         out.AddNote() << "Formation: " << mFormName;
         out.AddNote() << "Command will be canceled.";
      }
      Cancel();
      retval = aSimTime + 1.0e-8;
   }
   return retval;
}

bool wsf::six_dof::FormationAddSubCommand::AcceptCommand(Formation* aFormationPtr)
{
   bool okChild = VerifyProposedChild(aFormationPtr);
   if (!okChild)
   {
      auto out = ut::log::warning() << "Formation does not exist or is not top level.";
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Formation: " << mFormName;
      out.AddNote() << "Command will be ignored.";
   }
   bool okParent = GetProposedParent(aFormationPtr) != nullptr;
   if (!okParent)
   {
      auto out = ut::log::warning() << "Parent formation does not have a child with the relative name.";
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Parent: " << aFormationPtr->GetQualifiedName();
      out.AddNote() << "Child: " << mParentRelativeName;
      out.AddNote() << "Command will be ignored.";
   }
   return okChild && okParent;
}

void wsf::six_dof::FormationAddSubCommand::ComputeCommonTransformation(Formation* aFormationPtr) {}

std::unique_ptr<wsf::six_dof::FormationCommand>
wsf::six_dof::FormationAddSubCommand::ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr)
{
   return nullptr;
}

bool wsf::six_dof::FormationAddSubCommand::VerifyProposedChild(Formation* aFormationPtr) const
{
   bool              retval{false};
   FormationManager& mgr  = aFormationPtr->GetManager();
   Formation*        form = mgr.GetFormation(mFormName);
   if (form && form->IsRoot())
   {
      retval = true;
   }
   return retval;
}

wsf::six_dof::Formation* wsf::six_dof::FormationAddSubCommand::GetProposedParent(Formation* aFormationPtr) const
{
   Formation* retval{nullptr};
   if (!mParentRelativeName.empty())
   {
      std::string fullName = aFormationPtr->GetQualifiedName() + "." + mParentRelativeName;
      retval               = aFormationPtr->GetManager().GetFormation(fullName);
   }
   else
   {
      return aFormationPtr;
   }
   return retval;
}
