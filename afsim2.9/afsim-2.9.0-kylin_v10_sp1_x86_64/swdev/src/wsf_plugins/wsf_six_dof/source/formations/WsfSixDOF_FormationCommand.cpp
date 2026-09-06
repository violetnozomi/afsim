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

#include "WsfSixDOF_FormationCommand.hpp"

#include <stdexcept>

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_Formation.hpp"

wsf::six_dof::FormationCommand::FormationCommand()
   : mFormationPtr{nullptr}
   , mConstraintPtr{nullptr}
   , mStatus{Status::cUNASSIGNED}
   , mDebug{false}
{
}

wsf::six_dof::FormationCommand::FormationCommand(const wsf::six_dof::FormationCommand& aOther)
   : mFormationPtr{nullptr}
   , mConstraintPtr{nullptr}
   , mStatus{Status::cUNASSIGNED}
   , mDebug{aOther.mDebug}
{
   if (aOther.mConstraintPtr)
   {
      mConstraintPtr.reset(aOther.mConstraintPtr->Clone());
   }
}

//! Process the command.
//!
//! This template method will handle the work of transforming this command to
//! the sub-formations of @p aFormationPtr if this is a propagating command.
//! Further, this will also decide if @p aFormationPtr will accept the
//! command or not, returning the result of that determination. Note that
//! this can return false even if the command is propagated in transformed
//! form to the sub-formations of @p aFormationPtr.
//!
//! \param aFormationPtr - the formation under consideration
//! \param aSimTime      - the current simulation time.
//! \returns             - true if the command is accepted, false otherwise.
bool wsf::six_dof::FormationCommand::Process(wsf::six_dof::Formation* aFormationPtr, double aSimTime)
{
   if (GetDebug())
   {
      auto out = ut::log::debug() << "Processing command on formation.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Formation: " << aFormationPtr->GetQualifiedName();
   }
   if (CanPropagate() && !aFormationPtr->IsLeaf())
   {
      ComputeCommonTransformation(aFormationPtr);

      auto func = [aFormationPtr, aSimTime, this](Formation* aSubPtr)
      {
         if (aSubPtr->IsAttached())
         {
            if (GetDebug())
            {
               auto out = ut::log::debug() << "Computing transformed command for sub-formation from parent.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Command: " << Type();
               out.AddNote() << "Parent: " << aFormationPtr->GetQualifiedName();
               out.AddNote() << "Child: " << aSubPtr->GetQualifiedName();
            }
            auto transformedCommandPtr = ComputeTransformation(aFormationPtr, aSubPtr);
            if (transformedCommandPtr)
            {
               if (GetDebug())
               {
                  auto out = ut::log::debug() << "Executing transformed command.";
                  out.AddNote() << "T = " << aSimTime;
                  out.AddNote() << "Command: " << transformedCommandPtr->Type();
                  out.AddNote() << "Formation: " << aSubPtr->GetQualifiedName();
               }
               aSubPtr->ExecuteCommand(std::move(transformedCommandPtr), aSimTime + 1.0e-8);
            }
         }
      };

      aFormationPtr->InvokeOnSubFormations(func);
   }
   if (GetDebug())
   {
      auto out = ut::log::debug() << "Considering acceptance of command on formation.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Formation: " << aFormationPtr->GetQualifiedName();
   }
   return AcceptCommand(aFormationPtr);
}

//! Update the command.
//!
//! This is the central driver of the command's execution. This handles status changes
//! and handles calling into the Execute() method on the specific command in question.
//! This will return the next time that the associated command should be executed. This
//! return value will either be after @p aSimTime, indicating that the command is not
//! yet finished executing, or it will be before @p aSimTime, indicating that the command
//! has completed.
//!
//! @param aSimTime - the time at which the command is being evaluated.
//! @returns        - the next time at which the command should be evaluated.
double wsf::six_dof::FormationCommand::Update(double aSimTime)
{
   if (!IsAssigned())
   {
      throw std::runtime_error("Unassigned command is being updated.");
   }

   if (GetDebug())
   {
      auto out = ut::log::debug() << "Updating command assigned to formation.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Formation: " << mFormationPtr->GetQualifiedName();
   }

   double retval = -1.0;
   if (IsDropped())
   {
      if (GetDebug())
      {
         ut::log::debug() << "Command was dropped.";
      }
   }
   else if (IsCanceling())
   {
      if (GetDebug())
      {
         ut::log::debug() << "Command was canceled.";
      }
      mStatus = Status::cCANCELED;
   }
   else
   {
      if (IsPending())
      {
         if (GetDebug())
         {
            ut::log::debug() << "Command is pending.";
         }

         if (!mConstraintPtr || mConstraintPtr->IsSatisfied(aSimTime))
         {
            if (GetDebug())
            {
               ut::log::debug() << "Constraint is satisfied. Status changed to executing.";
            }
            mStatus = Status::cEXECUTING;
         }
         else
         {
            retval = mConstraintPtr->GetNextConstraintEvalTime(aSimTime);
         }
      }

      if (IsExecuting())
      {
         if (GetDebug())
         {
            ut::log::debug() << "Command is executing.";
         }

         retval = Execute(aSimTime);
         if (retval < aSimTime)
         {
            if (GetDebug())
            {
               ut::log::debug() << "Execution complete. Status changed to completed.";
            }
            mStatus = Status::cCOMPLETED;
         }
      }
   }

   return retval;
}

//! Assign this command to the given formation.
//!
//! This will assign this command to the given formation. This will change
//! the status of this command from unassigned to pending. If @p aLaunch is true,
//! this will also schedule the event to handle the execution of the command.
//! @p aLaunch should almost always be true.
//!
//! \param aFormationPtr - the formation to which this command will be assigned.
//! \param aSim          - the simulation in which the command is executing.
//! \param aSimTime      - the sim time of the assignment (not required to be the same as the current simulation time).
//! \param aLaunch       - true if the assignment should also launch the event to execute the command.
void wsf::six_dof::FormationCommand::Assign(Formation* aFormationPtr, WsfSimulation& aSim, double aSimTime, bool aLaunch)
{
   if (!aFormationPtr)
   {
      throw std::domain_error("Cannot assign command to null formation");
   }
   else if (mFormationPtr)
   {
      throw std::runtime_error("Unable to assign already assigned command");
   }
   else
   {
      if (GetDebug())
      {
         auto out = ut::log::debug() << "Assigning command to formation.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Command: " << Type();
         out.AddNote() << "Formation: " << aFormationPtr->GetQualifiedName();
      }
      mFormationPtr = aFormationPtr;
      mStatus       = Status::cPENDING;
      if (mConstraintPtr)
      {
         mConstraintPtr->SetAssignmentTime(aSimTime);
      }
      if (aLaunch)
      {
         aSim.AddEvent(ut::make_unique<Event>(this, aSim, aSimTime));
      }
   }
}

//! Cancel this command.
//!
//! This will cause the command to stop executing, and (eventually) change its status
//! to canceled.
void wsf::six_dof::FormationCommand::Cancel()
{
   if (IsPending() || IsExecuting())
   {
      if (GetDebug())
      {
         auto out = ut::log::debug() << "Canceling command.";
         out.AddNote() << "Command: " << Type();
         out.AddNote() << "Formation: " << mFormationPtr->GetQualifiedName();
      }
      mStatus = Status::cCANCELING;
   }
   else if (IsCompleted())
   {
      if (GetDebug())
      {
         auto out = ut::log::debug() << "Canceling command is pointless. Status unchanged.";
         out.AddNote() << "Command: " << Type();
         out.AddNote() << "Formation: " << mFormationPtr->GetQualifiedName();
      }
   }
   else if (mStatus == Status::cUNASSIGNED)
   {
      if (GetDebug())
      {
         auto out = ut::log::debug() << "Canceling command that is unassigned.";
         out.AddNote() << "Command: " << Type();
      }
      mStatus = Status::cCANCELED;
   }
}

//! Drop this command.
//!
//! This will drop the command. This means that the formation will no longer own the command,
//! the command is no longer assigned to the formation, and the memory of this command will
//! be reclaimed during the next execution of the associated event.
void wsf::six_dof::FormationCommand::Drop(WsfSimulation& aSim)
{
   if (IsRunning())
   {
      mStatus = Status::cDROPPED;
   }
   else
   {
      aSim.AddEvent(ut::make_unique<Event>(this, aSim, aSim.GetSimTime()));
      mStatus = Status::cDROPPED;
   }
}

//! Set an absolute time constraint.
void wsf::six_dof::FormationCommand::Constraint::SetAbsoluteTimeConstraint(double aAbsoluteTime)
{
   mType           = Type::cABSOLUTE;
   mConstraintTime = aAbsoluteTime;
}

//! Set a relative time constraint.
void wsf::six_dof::FormationCommand::Constraint::SetRelativeTimeConstraint(double aRelativeTime)
{
   mType           = Type::cRELATIVE;
   mConstraintTime = aRelativeTime;
}

//! Returns if the constraint is satisfied.
//!
//! @param aSimTime - the current time of evaluation of the constraint.
//! @returns        - true if the constraint is satisfied; false otherwise.
bool wsf::six_dof::FormationCommand::Constraint::IsSatisfied(double aSimTime) const
{
   bool retval{false};
   if (mType == Type::cRELATIVE)
   {
      retval = aSimTime >= GetRelativeTimeConstraint();
   }
   else if (mType == Type::cABSOLUTE)
   {
      retval = aSimTime >= GetAbsoluteTimeConstraint();
   }
   return retval;
}

//! Return the next time at which the constraint should be evaluated.
//!
//! This allows for constraints to control when or how often the constraint will be
//! next evaluated.
double wsf::six_dof::FormationCommand::Constraint::GetNextConstraintEvalTime(double aSimTime) const
{
   double retval{-1.0};
   if (mType == Type::cRELATIVE)
   {
      retval = GetRelativeTimeConstraint();
   }
   else if (mType == Type::cABSOLUTE)
   {
      retval = GetAbsoluteTimeConstraint();
   }
   return retval;
}

//! Set the constraint for this command.
void wsf::six_dof::FormationCommand::SetConstraint(std::unique_ptr<FormationCommand::Constraint>&& aConstraintPtr)
{
   mConstraintPtr = std::move(aConstraintPtr);
}

wsf::six_dof::FormationCommand::Event::Event(FormationCommand* aCommandPtr, WsfSimulation& aSim, double aSimTime)
   : WsfEvent(aSimTime)
   , mCommandPtr{aCommandPtr}
   , mSim(aSim)
{
   if (!aCommandPtr)
   {
      throw std::runtime_error("Cannot create formation command event with no associated command");
   }
}

//! Execute event for command processing.
//!
//! The events associated with commands have the job of calling FormationCommand::Update()
//! and then either resetting the event's time so it can be rescheduled, or ending the chain
//! of commands. If the command is dropped by its formation, the Event takes responsibility
//! for freeing the memory associated with the command.
WsfEvent::EventDisposition wsf::six_dof::FormationCommand::Event::Execute()
{
   WsfEvent::EventDisposition retval{WsfEvent::cDELETE};

   double nextEventTime = mCommandPtr->Update(GetTime());

   if (mCommandPtr->IsDropped())
   {
      // When the command is dropped by its formation, this event has ownership of the command.
      delete mCommandPtr;
   }
   else if (mCommandPtr->IsPending() || mCommandPtr->IsExecuting())
   {
      SetTime(nextEventTime);
      retval = WsfEvent::cRESCHEDULE;
   }

   return retval;
}
