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

#include "WsfFormationCommandSequence.hpp"

#include "UtMemory.hpp"
#include "WsfFormation.hpp"
#include "WsfFormationManager.hpp"

WsfFormationCommandSequence::WsfFormationCommandSequence()
   : WsfFormationCommand{}
   , mCommands{}
   , mCurrentCommand{0}
{
}

WsfFormationCommandSequence::WsfFormationCommandSequence(const WsfFormationCommandSequence& aOther)
   : WsfFormationCommand(aOther)
   , mCommands{}
   , mCurrentCommand{aOther.mCurrentCommand}
{
   for (const auto& command : aOther.mCommands)
   {
      mCommands.push_back(ut::clone(command));
   }
}

//! Return the current command in the sequence.
WsfFormationCommand* WsfFormationCommandSequence::CurrentCommand() const
{
   WsfFormationCommand* retval{nullptr};
   if (mCurrentCommand < mCommands.size())
   {
      retval = mCommands[mCurrentCommand].get();
   }
   return retval;
}

//! Advance to the next command in the sequence, and return that commands.
WsfFormationCommand* WsfFormationCommandSequence::AdvanceCommand()
{
   ++mCurrentCommand;
   return CurrentCommand();
}

//! Add a command to the sequence.
//!
//! This will only add the command if this sequence has not been assigned to a formation.
void WsfFormationCommandSequence::AppendCommand(std::unique_ptr<WsfFormationCommand>&& aCommandPtr)
{
   if (aCommandPtr && !IsAssigned())
   {
      mCommands.push_back(std::move(aCommandPtr));
   }
}

//! Execute the sequence.
//!
//! This will march through all the commands that can be executed up to the given
//! @p aSimTime. The commands will be assigned to the formation to which this is
//! assigned, but these assignments will not launch a new event sequence. Instead, this
//! command will take over calling Update().
double WsfFormationCommandSequence::Execute(double aSimTime)
{
   if (mCommands.size() == 0)
   {
      return -1.0;
   }

   if (mCurrentCommand >= mCommands.size())
   {
      mCurrentCommand = 0;
   }

   WsfFormationCommand* currentPtr = CurrentCommand();
   double               retval     = -1.0;
   while (currentPtr && retval < aSimTime)
   {
      if (!currentPtr->IsAssigned())
      {
         mCommands[mCurrentCommand]->SetDebug(GetDebug());
         mCommands[mCurrentCommand]->Assign(GetFormation(), *GetFormation()->GetManager().GetSimulation(), aSimTime, false);
      }

      retval = currentPtr->Update(aSimTime);

      if (retval < aSimTime)
      {
         currentPtr = AdvanceCommand();
      }
   }

   return retval;
}

//! Determine acceptance of this sequence.
//!
//! This will accept those commands in the sequence that are accpted individually.
//! If there are commands in the sequence that are not accepted, those will not be
//! executed. This method is called after the sequence would be transformed for
//! child formations, and so this silent drop of the commands that are not accepted
//! will not be noticed.
bool WsfFormationCommandSequence::AcceptCommand(WsfFormation* aFormationPtr)
{
   std::vector<std::unique_ptr<WsfFormationCommand>> replacement{};
   for (auto& i : mCommands)
   {
      if (i->AcceptCommand(aFormationPtr))
      {
         replacement.push_back(std::move(i));
      }
   }
   if (replacement.size() != 0)
   {
      mCommands = std::move(replacement);
   }
   return mCommands.size() != 0;
}

//! Determine if this command sequence can propagate.
//!
//! A command sequence will propagate if at least one of the elements of the sequence will
//! propagate.
bool WsfFormationCommandSequence::CanPropagate() const
{
   bool retval{false};
   for (auto& i : mCommands)
   {
      retval = retval || i->CanPropagate();
   }
   return retval;
}

//! Compute common transformation data.
//!
//! This will compute the common transformation data for each member of the sequence.
void WsfFormationCommandSequence::ComputeCommonTransformation(WsfFormation* aFormationPtr)
{
   for (auto& i : mCommands)
   {
      i->ComputeCommonTransformation(aFormationPtr);
   }
}

//! Compute transformed sequence.
//!
//! This will compute the transformed sequence to forward to the given @p aChildPtr.
std::unique_ptr<WsfFormationCommand> WsfFormationCommandSequence::ComputeTransformation(WsfFormation* aParentPtr,
                                                                                        WsfFormation* aChildPtr)
{
   std::unique_ptr<WsfFormationCommand> retval{new WsfFormationCommandSequence{}};
   WsfFormationCommandSequence*         seqPtr = static_cast<WsfFormationCommandSequence*>(retval.get());

   for (auto& i : mCommands)
   {
      seqPtr->AppendCommand(i->ComputeTransformation(aParentPtr, aChildPtr));
   }

   return retval;
}
