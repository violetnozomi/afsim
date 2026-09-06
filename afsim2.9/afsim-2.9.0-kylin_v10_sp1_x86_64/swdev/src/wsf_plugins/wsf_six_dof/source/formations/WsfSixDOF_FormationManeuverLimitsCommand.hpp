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

#ifndef WSF_SIXDOF_FORMATIONMANEUVERLIMITSCOMMAND_HPP
#define WSF_SIXDOF_FORMATIONMANEUVERLIMITSCOMMAND_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_FormationCommand.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT FormationManeuverLimitsCommand : public FormationCommand
{
public:
   FormationManeuverLimitsCommand();
   FormationManeuverLimitsCommand(const FormationManeuverLimitsCommand& aOther) = default;
   ~FormationManeuverLimitsCommand() override                                   = default;

   FormationManeuverLimitsCommand* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfSixDOF_FormationManeuverLimits"; }
   std::string Type() const override { return "MANEUVER-LIMITS"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   double                            Execute(double aSimTime) override;
   bool                              AcceptCommand(Formation* aFormationPtr) override;
   bool                              CanPropagate() const override { return true; }
   void                              ComputeCommonTransformation(Formation* aFormationPtr) override;
   std::unique_ptr<FormationCommand> ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr) override;

   double GetPitchG_LoadMax() const { return mPitchG_LoadMax; }
   double GetRollRateMaxDegPerSec() const { return mRollRateMaxDegPerSec; }
   double GetBankAngleMaxDeg() const { return mBankAngleMaxDeg; }

   void SetPitchG_LoadMax(double aPitchG_LoadMax);
   void SetRollRateMaxDegPerSec(double aRollRateMaxDegPerSec);
   void SetBankAngleMaxDeg(double aBankAngleMaxDeg);

private:
   double mPitchG_LoadMax;
   double mRollRateMaxDegPerSec; // deg/sec
   double mBankAngleMaxDeg;      // deg
   bool   mSettingG_LoadMax;
   bool   mSettingRollRateMax;
   bool   mSettingBankAngleMax;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONMANEUVERLIMITSCOMMAND_HPP
