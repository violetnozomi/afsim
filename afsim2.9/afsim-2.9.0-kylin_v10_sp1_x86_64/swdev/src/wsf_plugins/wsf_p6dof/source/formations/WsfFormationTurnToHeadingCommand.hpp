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

#ifndef WSFFORMATIONTURNTOHEADINGCOMMAND_HPP
#define WSFFORMATIONTURNTOHEADINGCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include <vector>

#include "WsfFormationCommand.hpp"
class WsfFormationManeuverLimitsCommand;
class WsfTurnToHeadingManeuver;

class WSF_P6DOF_EXPORT WsfFormationTurnToHeadingCommand : public WsfFormationCommand
{
public:
   WsfFormationTurnToHeadingCommand();
   WsfFormationTurnToHeadingCommand(const WsfFormationTurnToHeadingCommand& aOther) = default;
   ~WsfFormationTurnToHeadingCommand() override                                     = default;

   WsfFormationTurnToHeadingCommand& operator=(const WsfFormationTurnToHeadingCommand& aOther) = delete;

   WsfFormationTurnToHeadingCommand* Clone() const override;

   WsfFormationTurnToHeadingCommand* TransformClone() const;

   const char* GetScriptClassName() const override { return "WsfFormationTurnToHeadingCommand"; }
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

   double                               Execute(double aSimTime) override;
   bool                                 AcceptCommand(WsfFormation* aFormationPtr) override;
   bool                                 CanPropagate() const override { return true; }
   void                                 ComputeCommonTransformation(WsfFormation* aFormationPtr) override;
   std::unique_ptr<WsfFormationCommand> ComputeTransformation(WsfFormation* aParentPtr, WsfFormation* aChildPtr) override;

   void ShiftDelays(double aExtraInitialDelay);

   static void GreatestExtents(WsfFormation* aRootPtr, WsfFormation* aFormationPtr, double& aRightmost, double& aLeftmost);
   static double MaxGLoadForTurn(WsfFormation* aFormationPtr);
   void          FindOverallDelayTimes(WsfFormation* aRootPtr, double& aInitialDelay, double& aFinalDelay) const;
   void          CollectAllLeaves(WsfFormation* aFormationPtr, std::vector<WsfFormation*>& aLeaves) const;

   bool AllDependentsFinished() const { return mDependentCount == 0; }
   void DependenceSatisfied();
   void AddDependent(WsfFormationTurnToHeadingCommand* aDependentPtr);

private:
   bool IsTurnLimited() const { return mMaxBankAngleDeg > 0.0 || mMaxRollRateDegPerSec > 0.0; }
   void SetupTurnLimits(WsfFormationManeuverLimitsCommand* aLimitsPtr);

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
   bool          mOneTimeComputation;
   WsfFormation* mFormationLeaderPtr;
   double        mInitialHeading;
   double        mTheta;
   double        mDelayFactor;
   double        mTurnRadius;
   double        mLeaderSpeed;

   // stuff to compute for each command
   TurnType                  mTurnType;
   TurnType                  mParentTurnType;
   double                    mInitialDelay;
   double                    mFinalDelay;
   double                    mFarthestRight;
   double                    mFarthestLeft;
   WsfTurnToHeadingManeuver* mManeuverPtr;
   double                    mRecheckInterval;

   // related to command completion
   WsfFormationTurnToHeadingCommand* mNotifyWhenDone;
   int                               mDependentCount;
   bool                              mFinalExecution;
};

#endif // WSFFORMATIONTURNTOHEADINGCOMMAND_HPP
