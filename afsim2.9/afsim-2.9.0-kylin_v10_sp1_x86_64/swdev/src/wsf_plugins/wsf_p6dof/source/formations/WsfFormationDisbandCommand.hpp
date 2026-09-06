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

#ifndef WSFFORMATIONDISBANDCOMMAND_HPP
#define WSFFORMATIONDISBANDCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include "WsfFormationCommand.hpp"

class WSF_P6DOF_EXPORT WsfFormationDisbandCommand : public WsfFormationCommand
{
public:
   WsfFormationDisbandCommand();
   WsfFormationDisbandCommand(const WsfFormationDisbandCommand& aOther) = default;
   ~WsfFormationDisbandCommand() override                               = default;

   WsfFormationDisbandCommand* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfFormationDisbandCommand"; }

   std::string Type() const override { return "DISBAND"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   double                               Execute(double aSimTime) override;
   bool                                 AcceptCommand(WsfFormation* aFormationPtr) override;
   bool                                 CanPropagate() const override { return false; }
   void                                 ComputeCommonTransformation(WsfFormation* aFormationPtr) override;
   std::unique_ptr<WsfFormationCommand> ComputeTransformation(WsfFormation* aParentPtr, WsfFormation* aChildPtr) override;
};

#endif // WSFFORMATIONDISBANDCOMMAND_HPP
