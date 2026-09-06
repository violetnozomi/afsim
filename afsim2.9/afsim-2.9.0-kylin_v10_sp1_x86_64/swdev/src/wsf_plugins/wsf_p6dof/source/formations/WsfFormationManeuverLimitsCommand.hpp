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

#ifndef WSFFORMATIONMANEUVERLIMITSCOMMAND_HPP
#define WSFFORMATIONMANEUVERLIMITSCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include "WsfFormationCommand.hpp"

class WSF_P6DOF_EXPORT WsfFormationManeuverLimitsCommand : public WsfFormationCommand
{
public:
   WsfFormationManeuverLimitsCommand();
   WsfFormationManeuverLimitsCommand(const WsfFormationManeuverLimitsCommand& aOther) = default;
   ~WsfFormationManeuverLimitsCommand() override                                      = default;

   WsfFormationManeuverLimitsCommand* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfFormationManeuverLimits"; }
   std::string Type() const override { return "MANEUVER-LIMITS"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   double                               Execute(double aSimTime) override;
   bool                                 AcceptCommand(WsfFormation* aFormationPtr) override;
   bool                                 CanPropagate() const override { return true; }
   void                                 ComputeCommonTransformation(WsfFormation* aFormationPtr) override;
   std::unique_ptr<WsfFormationCommand> ComputeTransformation(WsfFormation* aParentPtr, WsfFormation* aChildPtr) override;

   double GetPitchG_LoadMax() const { return mPitchG_LoadMax; }
   double GetRollRateMaxDegPerSec() const { return mRollRateMaxDegPerSec; }
   double GetBankAngleMaxDeg() const { return mBankAngleMaxDeg; }

   void SetPitchG_LoadMax(double aPitchG_LoadMax);
   void SetRollRateMaxDegPerSec(double aRollRateMaxDegPerSec);
   void SetBankAngleMaxDeg(double aBankAngleMaxDeg);

private:
   double mPitchG_LoadMax;
   double mRollRateMaxDegPerSec; // deg / sec
   double mBankAngleMaxDeg;      // deg
   bool   mSettingG_LoadMax;
   bool   mSettingRollRateMax;
   bool   mSettingBankAngleMax;
};

#endif // WSFFORMATIONMANEUVERLIMITSCOMMAND_HPP
