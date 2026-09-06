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

#ifndef WSFFORMATIONDETACHCOMMAND_HPP
#define WSFFORMATIONDETACHCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include "WsfFormationCommand.hpp"

class WSF_P6DOF_EXPORT WsfFormationDetachCommand : public WsfFormationCommand
{
public:
   WsfFormationDetachCommand();
   WsfFormationDetachCommand(const WsfFormationDetachCommand& aOther) = default;
   ~WsfFormationDetachCommand() override                              = default;

   WsfFormationDetachCommand* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfFormationDetachCommand"; }

   std::string Type() const override { return "DETACH"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   double                               Execute(double aSimTime) override;
   bool                                 AcceptCommand(WsfFormation* aFormationPtr) override;
   bool                                 CanPropagate() const override { return false; }
   void                                 ComputeCommonTransformation(WsfFormation* aFormationPtr) override;
   std::unique_ptr<WsfFormationCommand> ComputeTransformation(WsfFormation* aParentPtr, WsfFormation* aChildPtr) override;
};

#endif // WSFFORMATIONDETACHCOMMAND_HPP
