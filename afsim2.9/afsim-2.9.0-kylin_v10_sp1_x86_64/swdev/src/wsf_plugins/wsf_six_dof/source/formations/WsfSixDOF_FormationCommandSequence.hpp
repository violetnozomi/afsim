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

#ifndef WSF_SIXDOF_FORMATIONCOMMANDSEQUENCE_HPP
#define WSF_SIXDOF_FORMATIONCOMMANDSEQUENCE_HPP

#include "wsf_six_dof_export.h"

#include <vector>

#include "WsfSixDOF_FormationCommand.hpp"

namespace wsf
{
namespace six_dof
{
//! A command that is a sequence of commands.
//!
//! Each command in the sequence is executed in turn, and each is allowed to have its
//! own constraint. Further, a command sequence can have a command sequence as a member
//! of the sequence. Canceling or dropping a command sequence will cancel or drop the
//! entire sequence, and only those members of the sequence that have executed will
//! have any effect on the simulation.
class WSF_SIX_DOF_EXPORT FormationCommandSequence : public FormationCommand
{
public:
   FormationCommandSequence();
   FormationCommandSequence(const FormationCommandSequence& aOther);
   ~FormationCommandSequence() override = default;

   FormationCommandSequence& operator=(const FormationCommandSequence& aOther) = delete;

   FormationCommandSequence* Clone() const override { return new FormationCommandSequence{*this}; }

   std::string Type() const override { return "SEQUENCE"; }

   bool IsInstantaneous() const override { return false; }
   bool IsSequence() const override { return true; }

   int               GetNumCommands() const { return static_cast<int>(mCommands.size()); }
   FormationCommand* CurrentCommand() const;
   FormationCommand* AdvanceCommand();
   void              AppendCommand(std::unique_ptr<FormationCommand>&& aCommandPtr);

   double                            Execute(double aSimTime) override;
   bool                              AcceptCommand(Formation* aFormationPtr) override;
   bool                              CanPropagate() const override;
   void                              ComputeCommonTransformation(Formation* aFormationPtr) override;
   std::unique_ptr<FormationCommand> ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr) override;

private:
   std::vector<std::unique_ptr<FormationCommand>> mCommands;
   size_t                                         mCurrentCommand;
};

} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONCOMMANDSEQUENCE_HPP
