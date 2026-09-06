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

#ifndef WSFFORMATIONRENAMECOMMAND_HPP
#define WSFFORMATIONRENAMECOMMAND_HPP

#include "wsf_p6dof_export.h"

#include "WsfFormationCommand.hpp"

class WSF_P6DOF_EXPORT WsfFormationRenameCommand : public WsfFormationCommand
{
public:
   WsfFormationRenameCommand();
   WsfFormationRenameCommand(const WsfFormationRenameCommand& aOther) = default;
   ~WsfFormationRenameCommand() override                              = default;

   WsfFormationRenameCommand* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfFormationRenameCommand"; }

   std::string Type() const override { return "RENAME"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   std::string GetNewName() const { return mNewName; }
   void        SetNewName(const std::string& aNewName) { mNewName = aNewName; }

   double                               Execute(double aSimTime) override;
   bool                                 AcceptCommand(WsfFormation* aFormationPtr) override;
   bool                                 CanPropagate() const override { return false; }
   void                                 ComputeCommonTransformation(WsfFormation* aFormationPtr) override;
   std::unique_ptr<WsfFormationCommand> ComputeTransformation(WsfFormation* aParentPtr, WsfFormation* aChildPtr) override;

private:
   std::string mNewName;
};

#endif // WSFFORMATIONRENAMECOMMAND_HPP
