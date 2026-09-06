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

#ifndef WSFFORMATIONATTACHCOMMAND_HPP
#define WSFFORMATIONATTACHCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include "WsfFormationCommand.hpp"

class WSF_P6DOF_EXPORT WsfFormationAttachCommand : public WsfFormationCommand
{
public:
   WsfFormationAttachCommand();
   WsfFormationAttachCommand(const WsfFormationAttachCommand& aOther) = default;
   ~WsfFormationAttachCommand() override                              = default;

   WsfFormationAttachCommand* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfFormationAttachCommand"; }

   std::string Type() const override { return "ATTACH"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   double                               Execute(double aSimTime) override;
   bool                                 AcceptCommand(WsfFormation* aFormationPtr) override;
   bool                                 CanPropagate() const override { return false; }
   void                                 ComputeCommonTransformation(WsfFormation* aFormationPtr) override;
   std::unique_ptr<WsfFormationCommand> ComputeTransformation(WsfFormation* aParentPtr, WsfFormation* aChildPtr) override;
};

#endif // WSFFORMATIONATTACHCOMMAND_HPP
