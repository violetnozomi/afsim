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

#ifndef WSF_SIXDOF_FORMATIONADDSUBCOMMAND_HPP
#define WSF_SIXDOF_FORMATIONADDSUBCOMMAND_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_FormationCommand.hpp"
#include "WsfSixDOF_FormationOffset.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT FormationAddSubCommand : public FormationCommand
{
public:
   FormationAddSubCommand();
   FormationAddSubCommand(const FormationAddSubCommand& aOther) = default;
   ~FormationAddSubCommand() override                           = default;

   FormationAddSubCommand* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfSixDOF_FormationAddSubCommand"; }

   std::string Type() const override { return "ADDSUBFORMATION"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   //! Get the relative name of the parent to which the indicated formation will be added.
   std::string GetParentRelativeName() const { return mParentRelativeName; }

   //! Get the name of the formation to add.
   std::string GetFormationToAdd() const { return mFormName; }

   //! Get the offset with which the formation will be added.
   FormationOffset GetOffset() const { return mOffset; }

   //! Set the relative name of the parent to which the indicated formation will be added.
   void SetParentRelativeName(const std::string& aParentRelativeName) { mParentRelativeName = aParentRelativeName; }

   //! Set the name of the formation to add.
   void SetFormationToAdd(const std::string& aFormName) { mFormName = aFormName; }

   //! Set the offset with which the formation will be added.
   void SetOffset(const FormationOffset& aOffset) { mOffset = aOffset; }

   double                            Execute(double aSimTime) override;
   bool                              AcceptCommand(Formation* aFormationPtr) override;
   bool                              CanPropagate() const override { return false; }
   void                              ComputeCommonTransformation(Formation* aFormationPtr) override;
   std::unique_ptr<FormationCommand> ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr) override;

private:
   bool       VerifyProposedChild(Formation* aFormationPtr) const;
   Formation* GetProposedParent(Formation* aFormationPtr) const;

   std::string     mParentRelativeName;
   std::string     mFormName;
   FormationOffset mOffset;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONADDSUBCOMMAND_HPP
