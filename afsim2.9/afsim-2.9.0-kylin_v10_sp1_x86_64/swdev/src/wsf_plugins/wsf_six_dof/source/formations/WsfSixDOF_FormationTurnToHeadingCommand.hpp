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

#ifndef WSF_SIXDOF_FORMATIONTURNTOHEADINGCOMMAND_HPP
#define WSF_SIXDOF_FORMATIONTURNTOHEADINGCOMMAND_HPP

#include "wsf_six_dof_export.h"

#include <vector>

#include "WsfSixDOF_FormationCommand.hpp"

namespace wsf
{
namespace six_dof
{
class FormationManeuverLimitsCommand;
class TurnToHeadingManeuver;

class WSF_SIX_DOF_EXPORT FormationTurnToHeadingCommand : public FormationCommand
{
public:
   FormationTurnToHeadingCommand();
   FormationTurnToHeadingCommand(const FormationTurnToHeadingCommand& aOther) = default;
   ~FormationTurnToHeadingCommand() override                                  = default;

   FormationTurnToHeadingCommand& operator=(const FormationTurnToHeadingCommand& aOther) = delete;

   FormationTurnToHeadingCommand* Clone() const override;

   FormationTurnToHeadingCommand* TransformClone() const;

   const char* GetScriptClassName() const override { return "WsfSixDOF_FormationTurnToHeadingCommand"; }
   std::string Type() const override { return "TURN-TO-HEADING"; }

   bool IsInstantaneous() const override { return true; }
   bool IsSequence() const override { return false; }

   double GetTargetHeading() const { return mTargetHeading; }
   void   SetTargetHeading(double aTargetHeading) { mTargetHeading = aTargetHeading; }

   double GetMinSpeedRatio() const { return mMinSpeedRatio; }
   void   SetMinSpeedRatio(double aMinSpeedRatio) { mMinSpeedRatio = aMinSpeedRatio; }

   double GetMaxSpeedRatio() const { return mMaxSpeedRatio; }
   void   SetMaxSpeedRatio(double aMaxSpeedRatio) { mMaxSpeedRatio = aMaxSpeedRatio; }

   double GetSmallThetaLimit() const { return mSmallThetaLimit; }
   void   SetSmallThetaLimit(double aSmallThetaLimit) { mSmallThetaLimit = aSmallThetaLimit; }

   double GetMaxBankAngle() const { return mMaxBankAngleDeg; }
   void   SetMaxBankAngle(double aMaxBankAngleDeg) { mMaxBankAngleDeg = aMaxBankAngleDeg; }

   double GetMaxRollRate() const { return mMaxRollRateDegPerSec; }
   void   SetMaxRollRate(double aMaxRollRateDegPerSec) { mMaxRollRateDegPerSec = aMaxRollRateDegPerSec; }

   double                            Execute(double aSimTime) override;
   bool                              AcceptCommand(Formation* aFormationPtr) override;
   bool                              CanPropagate() const override { return true; }
   void                              ComputeCommonTransformation(Formation* aFormationPtr) override;
   std::unique_ptr<FormationCommand> ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr) override;

   void ShiftDelays(double aExtraInitialDelay);

   static void   GreatestExtents(Formation* aRootPtr, Formation* aFormationPtr, double& aRightmost, double& aLeftmost);
   static double MaxGLoadForTurn(Formation* aFormationPtr);
   void          FindOverallDelayTimes(Formation* aRootPtr, double& aInitialDelay, double& aFinalDelay) const;
   void          CollectAllLeaves(Formation* aFormationPtr, std::vector<Formation*>& aLeaves) const;

   bool AllDependentsFinished() const { return mDependentCount == 0; }
   void DependenceSatisfied();
   void AddDependent(FormationTurnToHeadingCommand* aDependentPtr);

private:
   bool IsTurnLimited() const { return mMaxBankAngleDeg > 0.0 || mMaxRollRateDegPerSec > 0.0; }
   void SetupTurnLimits(FormationManeuverLimitsCommand* aLimitsPtr);

   enum class TurnType
   {
      cPLAIN_TURN,
      cDELAYED_TURN,
      cSMALL_ANGLE_TURN
   };

   std::string TurnTypeDescription() const;

   // parameters to set behavior of the command
   double mTargetHeading;
   double mMinSpeedRatio;
   double mMaxSpeedRatio;
   double mSmallThetaLimit;
   double mMaxBankAngleDeg;
   double mMaxRollRateDegPerSec;

   // stuff to compute one time only
   bool       mOneTimeComputation;
   Formation* mFormationLeaderPtr;
   double     mInitialHeading;
   double     mTheta;
   double     mDelayFactor;
   double     mTurnRadius;
   double     mLeaderSpeed;

   // stuff to compute for each command
   TurnType               mTurnType;
   TurnType               mParentTurnType;
   double                 mInitialDelay;
   double                 mFinalDelay;
   double                 mFarthestRight;
   double                 mFarthestLeft;
   TurnToHeadingManeuver* mManeuverPtr;
   double                 mRecheckInterval;

   // related to command completion
   FormationTurnToHeadingCommand* mNotifyWhenDone;
   int                            mDependentCount;
   bool                           mFinalExecution;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONTURNTOHEADINGCOMMAND_HPP
