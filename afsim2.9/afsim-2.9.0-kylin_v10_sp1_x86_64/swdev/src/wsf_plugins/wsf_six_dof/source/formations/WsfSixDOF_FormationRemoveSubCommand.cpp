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

#include "WsfSixDOF_FormationRemoveSubCommand.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_FormationManager.hpp"
#include "WsfSixDOF_FormationUpdateStationKeeping.hpp"

wsf::six_dof::FormationRemoveSubCommand::FormationRemoveSubCommand()
   : FormationCommand()
   , mSubToRemove{}
{
}

double wsf::six_dof::FormationRemoveSubCommand::Execute(double aSimTime)
{
   double     retval{-1.0};
   Formation* formPtr         = GetFormation();
   Formation* formToRemovePtr = ValidateSubformation(formPtr);
   if (formToRemovePtr != nullptr)
   {
      Formation*  parentOfRemovedPtr = formToRemovePtr->GetParentFormation();
      std::string nameToRemove       = formToRemovePtr->GetRelativeName();
      if (parentOfRemovedPtr->RemoveChildFormation(nameToRemove))
      {
         if (GetDebug())
         {
            auto out = ut::log::debug() << "Executing command. Removing child formation from parent.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Command: " << Type();
            out.AddNote() << "Parent: " << formPtr->GetQualifiedName();
            out.AddNote() << "Child: " << mSubToRemove;
         }

         // After a successful remove, the subformation should not be top level with a name
         // equal to its previous relative name. So this should not fail.
         Formation* newRootPtr = formPtr->GetManager().GetFormation(nameToRemove);
         if (newRootPtr)
         {
            FormationUpdateStationKeeping::Update(newRootPtr, aSimTime);
         }
         else
         {
            throw std::runtime_error("The removed subformation did not exist as a top level formation after removal.");
         }
      }
      else
      {
         auto out = ut::log::warning() << "Failed to remove child formation.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Command: " << Type();
         out.AddNote() << "Parent: " << formPtr->GetQualifiedName();
         out.AddNote() << "Child: " << mSubToRemove;
         out.AddNote() << "This command will complete having accomplished nothing.";
      }
   }
   else
   {
      {
         // RAII block
         auto out = ut::log::warning() << "Child formation does not exist.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Command: " << Type();
         out.AddNote() << "Parent: " << formPtr->GetQualifiedName();
         out.AddNote() << "Child: " << mSubToRemove;
         out.AddNote() << "Command will be canceled.";
      }
      Cancel();
      retval = aSimTime + 1.0e-8;
   }
   return retval;
}

bool wsf::six_dof::FormationRemoveSubCommand::AcceptCommand(Formation* aFormationPtr)
{
   bool ok = ValidateSubformation(aFormationPtr) != nullptr;
   if (!ok)
   {
      auto out = ut::log::warning() << "Child formation does not exist.";
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Parent: " << aFormationPtr->GetQualifiedName();
      out.AddNote() << "Child: " << mSubToRemove;
      out.AddNote() << "Command will be ignored.";
   }
   return ok;
}

void wsf::six_dof::FormationRemoveSubCommand::ComputeCommonTransformation(Formation* aFormationPtr) {}

std::unique_ptr<wsf::six_dof::FormationCommand>
wsf::six_dof::FormationRemoveSubCommand::ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr)
{
   return nullptr;
}

wsf::six_dof::Formation* wsf::six_dof::FormationRemoveSubCommand::ValidateSubformation(Formation* aFormationPtr) const
{
   std::string fullName = aFormationPtr->GetQualifiedName() + "." + mSubToRemove;
   return aFormationPtr->GetManager().GetFormation(fullName);
}
