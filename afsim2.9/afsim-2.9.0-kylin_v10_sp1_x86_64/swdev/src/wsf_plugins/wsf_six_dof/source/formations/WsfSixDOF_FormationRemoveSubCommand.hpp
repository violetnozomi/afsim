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

#ifndef WSF_SIXDOF_FORMATIONREMOVESUBCOMMAND_HPP
#define WSF_SIXDOF_FORMATIONREMOVESUBCOMMAND_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_FormationCommand.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT FormationRemoveSubCommand : public FormationCommand
{
public:
   FormationRemoveSubCommand();
   FormationRemoveSubCommand(const FormationRemoveSubCommand& aOther) = default;
   ~FormationRemoveSubCommand() override                              = default;

   FormationRemoveSubCommand* Clone() const override { return new FormationRemoveSubCommand{*this}; }

   const char* GetScriptClassName() const override { return "WsfSixDOF_FormationRemoveSubCommand"; }

   std::string Type() const override { return "REMOVESUBFORMATION"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   //! Return the relative name of the sub-formation to remove.
   std::string GetSubformationToRemove() const { return mSubToRemove; }

   //! Set the relative name of the sub-formation to remove.
   void SetSubformationToRemove(const std::string& aName) { mSubToRemove = aName; }

   double                            Execute(double aSimTime) override;
   bool                              AcceptCommand(Formation* aFormationPtr) override;
   bool                              CanPropagate() const override { return false; }
   void                              ComputeCommonTransformation(Formation* aFormationPtr) override;
   std::unique_ptr<FormationCommand> ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr) override;

private:
   Formation* ValidateSubformation(Formation* aFormationPtr) const;

   std::string mSubToRemove;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONREMOVESUBCOMMAND_HPP
