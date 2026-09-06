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

#ifndef WSFFORMATIONCHANGELEADSUBCOMMAND_HPP
#define WSFFORMATIONCHANGELEADSUBCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include "WsfFormationCommand.hpp"

class WSF_P6DOF_EXPORT WsfFormationChangeLeadSubCommand : public WsfFormationCommand
{
public:
   WsfFormationChangeLeadSubCommand();
   WsfFormationChangeLeadSubCommand(const WsfFormationChangeLeadSubCommand& aOther) = default;
   ~WsfFormationChangeLeadSubCommand() override                                     = default;

   WsfFormationChangeLeadSubCommand* Clone() const override { return new WsfFormationChangeLeadSubCommand{*this}; }

   const char* GetScriptClassName() const override { return "WsfFormationChangeLeadSubCommand"; }

   std::string Type() const override { return "CHANGELEADSUBFORMATION"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   std::string GetNewLeaderName() const { return mNewLeaderName; }
   void        SetNewLeaderName(const std::string& aNewLeaderName) { mNewLeaderName = aNewLeaderName; }

   double                               Execute(double aSimTime) override;
   bool                                 AcceptCommand(WsfFormation* aFormationPtr) override;
   bool                                 CanPropagate() const override { return false; }
   void                                 ComputeCommonTransformation(WsfFormation* aFormationPtr) override;
   std::unique_ptr<WsfFormationCommand> ComputeTransformation(WsfFormation* aParentPtr, WsfFormation* aChildPtr) override;

private:
   std::string mNewLeaderName;
};

#endif // WSFFORMATIONCHANGELEADSUBCOMMAND_HPP
