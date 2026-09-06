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

#ifndef WSFFORMATIONADDSUBCOMMAND_HPP
#define WSFFORMATIONADDSUBCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include "WsfFormationCommand.hpp"
#include "WsfFormationOffset.hpp"

class WSF_P6DOF_EXPORT WsfFormationAddSubCommand : public WsfFormationCommand
{
public:
   WsfFormationAddSubCommand();
   WsfFormationAddSubCommand(const WsfFormationAddSubCommand& aOther) = default;
   ~WsfFormationAddSubCommand() override                              = default;

   WsfFormationAddSubCommand* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfFormationAddSubCommand"; }

   std::string Type() const override { return "ADDSUBFORMATION"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   //! Get the relative name of the parent to which the indicated formation will be added.
   std::string GetParentRelativeName() const { return mParentRelativeName; }

   //! Get the name of the formation to add.
   std::string GetFormationToAdd() const { return mFormName; }

   //! Get the offset with which the formation will be added.
   WsfFormationOffset GetOffset() const { return mOffset; }

   //! Set the relative name of the parent to which the indicated formation will be added.
   void SetParentRelativeName(const std::string& aParentRelativeName) { mParentRelativeName = aParentRelativeName; }

   //! Set the name of the formation to add.
   void SetFormationToAdd(const std::string& aFormName) { mFormName = aFormName; }

   //! Set the offset with which the formation will be added.
   void SetOffset(const WsfFormationOffset& aOffset) { mOffset = aOffset; }

   double                               Execute(double aSimTime) override;
   bool                                 AcceptCommand(WsfFormation* aFormationPtr) override;
   bool                                 CanPropagate() const override { return false; }
   void                                 ComputeCommonTransformation(WsfFormation* aFormationPtr) override;
   std::unique_ptr<WsfFormationCommand> ComputeTransformation(WsfFormation* aParentPtr, WsfFormation* aChildPtr) override;

private:
   bool          VerifyProposedChild(WsfFormation* aFormationPtr) const;
   WsfFormation* GetProposedParent(WsfFormation* aFormationPtr) const;

   std::string        mParentRelativeName;
   std::string        mFormName;
   WsfFormationOffset mOffset;
};

#endif // WSFFORMATIONADDSUBCOMMAND_HPP
