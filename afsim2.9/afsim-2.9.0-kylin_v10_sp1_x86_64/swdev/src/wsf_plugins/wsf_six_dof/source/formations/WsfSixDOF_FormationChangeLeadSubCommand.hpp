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

#ifndef WSF_SIXDOF_FORMATIONCHANGELEADSUBCOMMAND_HPP
#define WSF_SIXDOF_FORMATIONCHANGELEADSUBCOMMAND_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_FormationCommand.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT FormationChangeLeadSubCommand : public FormationCommand
{
public:
   FormationChangeLeadSubCommand();
   FormationChangeLeadSubCommand(const FormationChangeLeadSubCommand& aOther) = default;
   ~FormationChangeLeadSubCommand() override                                  = default;

   FormationChangeLeadSubCommand* Clone() const override { return new FormationChangeLeadSubCommand{*this}; }

   const char* GetScriptClassName() const override { return "WsfSixDOF_FormationChangeLeadSubCommand"; }

   std::string Type() const override { return "CHANGELEADSUBFORMATION"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   std::string GetNewLeaderName() const { return mNewLeaderName; }
   void        SetNewLeaderName(const std::string& aNewLeaderName) { mNewLeaderName = aNewLeaderName; }

   double                            Execute(double aSimTime) override;
   bool                              AcceptCommand(Formation* aFormationPtr) override;
   bool                              CanPropagate() const override { return false; }
   void                              ComputeCommonTransformation(Formation* aFormationPtr) override;
   std::unique_ptr<FormationCommand> ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr) override;

private:
   std::string mNewLeaderName;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONCHANGELEADSUBCOMMAND_HPP
