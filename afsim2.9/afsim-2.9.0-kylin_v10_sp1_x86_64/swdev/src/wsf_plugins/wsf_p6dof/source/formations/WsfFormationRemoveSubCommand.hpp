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

#ifndef WSFFORMATIONREMOVESUBCOMMAND_HPP
#define WSFFORMATIONREMOVESUBCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include "WsfFormationCommand.hpp"

class WSF_P6DOF_EXPORT WsfFormationRemoveSubCommand : public WsfFormationCommand
{
public:
   WsfFormationRemoveSubCommand();
   WsfFormationRemoveSubCommand(const WsfFormationRemoveSubCommand& aOther) = default;
   ~WsfFormationRemoveSubCommand() override                                 = default;

   WsfFormationRemoveSubCommand* Clone() const override { return new WsfFormationRemoveSubCommand{*this}; }

   const char* GetScriptClassName() const override { return "WsfFormationRemoveSubCommand"; }

   std::string Type() const override { return "REMOVESUBFORMATION"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   //! Return the relative name of the subformation to remove.
   std::string GetSubformationToRemove() const { return mSubToRemove; }

   //! Set the relative name of the subformation to remove.
   void SetSubformationToRemove(const std::string& aName) { mSubToRemove = aName; }

   double                               Execute(double aSimTime) override;
   bool                                 AcceptCommand(WsfFormation* aFormationPtr) override;
   bool                                 CanPropagate() const override { return false; }
   void                                 ComputeCommonTransformation(WsfFormation* aFormationPtr) override;
   std::unique_ptr<WsfFormationCommand> ComputeTransformation(WsfFormation* aParentPtr, WsfFormation* aChildPtr) override;

private:
   WsfFormation* ValidateSubformation(WsfFormation* aFormationPtr) const;

   std::string mSubToRemove;
};

#endif // WSFFORMATIONREMOVESUBCOMMAND_HPP
