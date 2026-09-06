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

#include "WsfFormationAddSubCommand.hpp"

#include "UtLog.hpp"
#include "WsfFormation.hpp"
#include "WsfFormationManager.hpp"
#include "WsfFormationUpdateStationKeeping.hpp"

WsfFormationAddSubCommand::WsfFormationAddSubCommand()
   : WsfFormationCommand{}
   , mParentRelativeName{}
   , mFormName{}
   , mOffset{}
{
}

WsfFormationAddSubCommand* WsfFormationAddSubCommand::Clone() const
{
   return new WsfFormationAddSubCommand{*this};
}

double WsfFormationAddSubCommand::Execute(double aSimTime)
{
   double        retval{-1.0};
   WsfFormation* formPtr   = GetFormation();
   WsfFormation* parentPtr = GetProposedParent(formPtr);
   if (VerifyProposedChild(formPtr) && parentPtr != nullptr)
   {
      WsfFormation* formToAdd = formPtr->GetManager().GetFormation(mFormName);
      formToAdd->SetOffset(mOffset);
      bool ok = parentPtr->AddChildFormation(formToAdd);

      if (ok)
      {
         formToAdd->SetAttached(false);
         WsfFormationUpdateStationKeeping::Update(formToAdd, aSimTime);
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
      { // RAII block
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

bool WsfFormationAddSubCommand::AcceptCommand(WsfFormation* aFormationPtr)
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

void WsfFormationAddSubCommand::ComputeCommonTransformation(WsfFormation* aFormationPtr) {}

std::unique_ptr<WsfFormationCommand> WsfFormationAddSubCommand::ComputeTransformation(WsfFormation* aParentPtr,
                                                                                      WsfFormation* aChildPtr)
{
   return nullptr;
}

bool WsfFormationAddSubCommand::VerifyProposedChild(WsfFormation* aFormationPtr) const
{
   bool                 retval{false};
   WsfFormationManager& mgr  = aFormationPtr->GetManager();
   WsfFormation*        form = mgr.GetFormation(mFormName);
   if (form && form->IsRoot())
   {
      retval = true;
   }
   return retval;
}

WsfFormation* WsfFormationAddSubCommand::GetProposedParent(WsfFormation* aFormationPtr) const
{
   WsfFormation* retval{nullptr};
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
