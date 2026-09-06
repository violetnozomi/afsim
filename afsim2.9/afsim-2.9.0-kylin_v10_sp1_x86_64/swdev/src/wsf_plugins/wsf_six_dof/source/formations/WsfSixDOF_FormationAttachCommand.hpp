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

#ifndef WSF_SIXDOF_FORMATIONATTACHCOMMAND_HPP
#define WSF_SIXDOF_FORMATIONATTACHCOMMAND_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_FormationCommand.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT FormationAttachCommand : public FormationCommand
{
public:
   FormationAttachCommand();
   FormationAttachCommand(const FormationAttachCommand& aOther) = default;
   ~FormationAttachCommand() override                           = default;

   FormationAttachCommand* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfSixDOF_FormationAttachCommand"; }

   std::string Type() const override { return "ATTACH"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   double                            Execute(double aSimTime) override;
   bool                              AcceptCommand(Formation* aFormationPtr) override;
   bool                              CanPropagate() const override { return false; }
   void                              ComputeCommonTransformation(Formation* aFormationPtr) override;
   std::unique_ptr<FormationCommand> ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr) override;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONATTACHCOMMAND_HPP
