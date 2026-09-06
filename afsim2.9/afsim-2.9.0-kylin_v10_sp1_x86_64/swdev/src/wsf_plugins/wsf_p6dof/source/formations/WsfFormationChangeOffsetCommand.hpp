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

#ifndef WSFFORMATIONCHANGEOFFSETCOMMAND_HPP
#define WSFFORMATIONCHANGEOFFSETCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include <vector>

#include "WsfFormationCommand.hpp"
#include "WsfFormationOffset.hpp"

class WSF_P6DOF_EXPORT WsfFormationChangeOffsetCommand : public WsfFormationCommand
{
public:
   WsfFormationChangeOffsetCommand();
   WsfFormationChangeOffsetCommand(const WsfFormationChangeOffsetCommand& aOther) = default;
   ~WsfFormationChangeOffsetCommand() override                                    = default;

   WsfFormationChangeOffsetCommand* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfFormationChangeOffset"; }

   std::string Type() const override { return "CHANGE-OFFSET"; }

   bool IsInstantaneous() const override { return false; }
   bool IsSequence() const override { return false; }

   double                               Execute(double aSimTime) override;
   bool                                 AcceptCommand(WsfFormation* aFormationPtr) override;
   bool                                 CanPropagate() const override { return false; }
   void                                 ComputeCommonTransformation(WsfFormation* aFormationPtr) override;
   std::unique_ptr<WsfFormationCommand> ComputeTransformation(WsfFormation* aParentPtr, WsfFormation* aChildPtr) override;

   // add and remove offset values
   void               AddOffset(WsfFormationOffset& aOffset);
   WsfFormationOffset GetOffset(int aOffsetIndex) { return mOffsets[aOffsetIndex + 1]; }
   size_t             GetOffsetCount() const { return mOffsets.size() - 1; }

   double GetTransitionTime() const { return mTransitionTime; }
   void   SetTransitionTime(double aTransitionTime) { mTransitionTime = aTransitionTime; }

private:
   // some kind of way to store the offsets on the path
   std::vector<WsfFormationOffset> mOffsets;
   double                          mTransitionTime;
   double                          mInitialTime;
};

#endif // WSFFORMATIONCHANGEOFFSETCOMMAND_HPP
