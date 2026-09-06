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

#ifndef WSFFORMATIONCOMMANDSEQUENCE_HPP
#define WSFFORMATIONCOMMANDSEQUENCE_HPP

#include "wsf_p6dof_export.h"

#include <vector>

#include "WsfFormationCommand.hpp"

//! A command that is a sequence of commands.
//!
//! Each command in the sequence is executed in turn, and each is allowed to have its
//! own constraint. Further, a command sequence can have a command sequence as a member
//! of the sequence. Cancelling or dropping a command sequence will cancel or drop the
//! entire sequence, and only those members of the sequence that have executed will
//! have any effect on the simulation.
class WSF_P6DOF_EXPORT WsfFormationCommandSequence : public WsfFormationCommand
{
public:
   WsfFormationCommandSequence();
   WsfFormationCommandSequence(const WsfFormationCommandSequence& aOther);
   ~WsfFormationCommandSequence() override = default;

   WsfFormationCommandSequence& operator=(const WsfFormationCommandSequence& aOther) = delete;

   WsfFormationCommandSequence* Clone() const override { return new WsfFormationCommandSequence{*this}; }

   std::string Type() const override { return "SEQUENCE"; }

   bool IsInstantaneous() const override { return false; }
   bool IsSequence() const override { return true; }

   int                  GetNumCommands() const { return static_cast<int>(mCommands.size()); }
   WsfFormationCommand* CurrentCommand() const;
   WsfFormationCommand* AdvanceCommand();
   void                 AppendCommand(std::unique_ptr<WsfFormationCommand>&& aCommandPtr);

   double                               Execute(double aSimTime) override;
   bool                                 AcceptCommand(WsfFormation* aFormationPtr) override;
   bool                                 CanPropagate() const override;
   void                                 ComputeCommonTransformation(WsfFormation* aFormationPtr) override;
   std::unique_ptr<WsfFormationCommand> ComputeTransformation(WsfFormation* aParentPtr, WsfFormation* aChildPtr) override;

private:
   std::vector<std::unique_ptr<WsfFormationCommand>> mCommands;
   size_t                                            mCurrentCommand;
};

#endif // WSFFORMATIONCOMMANDSEQUENCE_HPP
