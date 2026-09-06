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

#include "WsfSixDOF_FormationTurnToHeadingCommand.hpp"

#include "UtEarth.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfPlatform.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_FormationAttachCommand.hpp"
#include "WsfSixDOF_FormationChangeOffsetCommand.hpp"
#include "WsfSixDOF_FormationCommandSequence.hpp"
#include "WsfSixDOF_FormationDetachCommand.hpp"
#include "WsfSixDOF_FormationManeuverLimitsCommand.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "maneuvers/WsfSixDOF_ManeuverRelativeTimeConstraint.hpp"
#include "maneuvers/WsfSixDOF_ManeuverRollConstraint.hpp"
#include "maneuvers/WsfSixDOF_TurnToHeadingManeuver.hpp"

wsf::six_dof::FormationTurnToHeadingCommand::FormationTurnToHeadingCommand()
   : FormationCommand{}
   , mTargetHeading{0.0}
   , mMinSpeedRatio{0.9}
   , mMaxSpeedRatio{1.1}
   , mSmallThetaLimit{0.3} // ~17 deg
   , mMaxBankAngleDeg{-1.0}
   , mMaxRollRateDegPerSec{-1.0}
   , mOneTimeComputation{false}
   , mFormationLeaderPtr{nullptr}
   , mInitialHeading{0.0}
   , mTheta{0.0}
   , mDelayFactor{0.0}
   , mTurnRadius{0.0}
   , mLeaderSpeed{1.0}
   , mTurnType{TurnType::cPLAIN_TURN}
   , mParentTurnType{TurnType::cPLAIN_TURN}
   , mInitialDelay{0.0}
   , mFinalDelay{0.0}
   , mFarthestRight{0.0}
   , mFarthestLeft{0.0}
   , mManeuverPtr{nullptr}
   , mRecheckInterval{-1.0}
   , mNotifyWhenDone{nullptr}
   , mDependentCount{0}
   , mFinalExecution{false}
{
}

wsf::six_dof::FormationTurnToHeadingCommand* wsf::six_dof::FormationTurnToHeadingCommand::Clone() const
{
   return new FormationTurnToHeadingCommand{*this};
}

wsf::six_dof::FormationTurnToHeadingCommand* wsf::six_dof::FormationTurnToHeadingCommand::TransformClone() const
{
   auto retvalPtr              = Clone();
   retvalPtr->mTurnType        = TurnType::cPLAIN_TURN;
   retvalPtr->mParentTurnType  = mTurnType;
   retvalPtr->mFarthestRight   = 0.0;
   retvalPtr->mFarthestLeft    = 0.0;
   retvalPtr->mManeuverPtr     = nullptr;
   retvalPtr->mRecheckInterval = -1.0;
   retvalPtr->mNotifyWhenDone  = nullptr;
   retvalPtr->mDependentCount  = 0;
   retvalPtr->mFinalExecution  = false;
   return retvalPtr;
}

double wsf::six_dof::FormationTurnToHeadingCommand::Execute(double aSimTime)
{
   Formation* formPtr = GetFormation();
   if (GetDebug())
   {
      auto out = ut::log::debug() << "Executing command. Turning formation to heading.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Formation: " << formPtr->GetQualifiedName();
      out.AddNote() << "Heading: " << mTargetHeading;
   }

   double retval{-1.0};
   if (mFinalExecution)
   {
      if (mParentTurnType == TurnType::cDELAYED_TURN)
      {
         FormationOffset swappedOffset = formPtr->GetOffset();
         swappedOffset.SetRight(-swappedOffset.GetRight());
         formPtr->SetOffset(swappedOffset);
      }
      if (mNotifyWhenDone)
      {
         mNotifyWhenDone->DependenceSatisfied();
      }
   }
   else if (formPtr->IsLeaf())
   {
      if (mManeuverPtr == nullptr)
      {
         Mover* moverPtr = formPtr->GetMemberMover();
         if (moverPtr != nullptr)
         {
            auto mvrPtr = ut::make_unique<TurnToHeadingManeuver>(mTargetHeading);

            auto constraintPtr = ut::make_unique<ManeuverRelativeTimeConstraint>();
            constraintPtr->SetConstraintTime(mInitialDelay);
            mvrPtr->SetEntryConstraint(std::move(constraintPtr));

            mManeuverPtr = mvrPtr.get();
            moverPtr->ExecuteManeuver(aSimTime, std::move(mvrPtr));
            mRecheckInterval = moverPtr->GetUpdateInterval();
            retval           = aSimTime + mRecheckInterval;
         }
         else
         {
            throw std::runtime_error("TurnToHeading command should only execute on leaves with members.");
         }
      }
      else if (!mManeuverPtr->IsCompleted())
      {
         retval = aSimTime + mRecheckInterval;
      }
      else
      {
         retval          = aSimTime + mFinalDelay;
         mFinalExecution = true;
      }
   }
   else
   {
      retval = aSimTime + 0.1;
      if (AllDependentsFinished())
      {
         mFinalExecution = true;
      }
   }

   return retval;
}

bool wsf::six_dof::FormationTurnToHeadingCommand::AcceptCommand(Formation* aFormationPtr)
{
   return !aFormationPtr->IsLeaf() || aFormationPtr->HasMemberPlatform();
}

void wsf::six_dof::FormationTurnToHeadingCommand::ComputeCommonTransformation(Formation* aFormationPtr)
{
   if (!mOneTimeComputation)
   {
      mFormationLeaderPtr  = aFormationPtr->GetFormationLeader();
      WsfPlatform* platPtr = mFormationLeaderPtr->GetMemberPlatform();
      double       pitch, roll;
      platPtr->GetOrientationNED(mInitialHeading, pitch, roll);
      mTheta       = UtMath::NormalizeAngleMinusPi_Pi(mTargetHeading - mInitialHeading);
      mDelayFactor = (1.0 - sin(mTheta - UtMath::cPI_OVER_2)) / cos(mTheta - UtMath::cPI_OVER_2);

      Mover* moverPtr = mFormationLeaderPtr->GetMemberMover();
      double accTurn  = UtEarth::cACCEL_OF_GRAVITY * tan(moverPtr->GetBankAngleMax() * UtMath::cRAD_PER_DEG);
      mTurnRadius     = mFormationLeaderPtr->GetMemberPlatform()->GetSpeedSquared() / accTurn;

      FindOverallDelayTimes(aFormationPtr, mInitialDelay, mFinalDelay);
      mLeaderSpeed = platPtr->GetSpeed();
      mInitialDelay /= mLeaderSpeed;
      mFinalDelay /= mLeaderSpeed;

      mOneTimeComputation = true;
   }

   // Compute speed ratios
   GreatestExtents(aFormationPtr, aFormationPtr, mFarthestRight, mFarthestLeft);
   double outsideDiff;
   double insideDiff;
   if (mTheta > 0.0)
   {
      outsideDiff = -mFarthestLeft;
      insideDiff  = mFarthestRight;
   }
   else
   {
      outsideDiff = mFarthestRight;
      insideDiff  = -mFarthestLeft;
   }
   double speedMaxRatio = 1.0 + outsideDiff / mTurnRadius;
   double speedMinRatio = 1.0 - insideDiff / mTurnRadius;

   // Which sort of turn will it be?
   if (speedMaxRatio <= mMaxSpeedRatio && speedMinRatio >= mMinSpeedRatio)
   {
      mTurnType = TurnType::cPLAIN_TURN;
   }
   else
   {
      if (fabs(mTheta) > mSmallThetaLimit)
      {
         mTurnType = TurnType::cDELAYED_TURN;
      }
      else
      {
         mTurnType = TurnType::cSMALL_ANGLE_TURN;
      }
   }

   if (GetDebug())
   {
      auto out = ut::log::debug() << "Computed common transformation for formation.";
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Formation: " << aFormationPtr->GetQualifiedName();
      out.AddNote() << "Turn Type: " << TurnTypeDescription();
   }
}

std::unique_ptr<wsf::six_dof::FormationCommand>
wsf::six_dof::FormationTurnToHeadingCommand::ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr)
{
   std::unique_ptr<FormationCommand> retvalPtr{nullptr};
   if (mTurnType == TurnType::cPLAIN_TURN || mTurnType == TurnType::cSMALL_ANGLE_TURN)
   {
      // In a plain turn, the leader will execute the turn, and the others will just keep
      // station, so they receive no command. It cannot be the case that some formation
      // is eligible for a plain turn, but its children are not, because we compute the
      // limit on plain turn for the worst case of all subformations.
      if (aParentPtr->GetLeadSubFormation() == aChildPtr)
      {
         auto cmdPtr = std::unique_ptr<FormationTurnToHeadingCommand>(TransformClone());
         AddDependent(cmdPtr.get());

         if (IsTurnLimited() && aChildPtr->IsLeaf())
         {
            auto limitsPtr = ut::make_unique<FormationManeuverLimitsCommand>();
            SetupTurnLimits(limitsPtr.get());

            auto resetPtr = ut::make_unique<FormationManeuverLimitsCommand>();

            auto seqPtr = ut::make_unique<FormationCommandSequence>();
            seqPtr->AppendCommand(std::move(limitsPtr));
            seqPtr->AppendCommand(std::move(cmdPtr));
            seqPtr->AppendCommand(std::move(resetPtr));

            retvalPtr = std::move(seqPtr);
         }
         else
         {
            retvalPtr = std::move(cmdPtr);
         }
      }
   }
   else if (mTurnType == TurnType::cDELAYED_TURN)
   {
      FormationOffset offset = aChildPtr->GetOffsetFrom(*aParentPtr);
      double          d      = offset.GetRight() * mDelayFactor;
      double          l      = offset.GetAhead();
      double          w      = (d - l) / mLeaderSpeed;

      auto seqPtr = ut::make_unique<FormationCommandSequence>();

      auto detachPtr = ut::make_unique<FormationDetachCommand>();
      seqPtr->AppendCommand(std::move(detachPtr));

      if (IsTurnLimited() && aChildPtr->IsLeaf())
      {
         auto limitsPtr = ut::make_unique<FormationManeuverLimitsCommand>();
         SetupTurnLimits(limitsPtr.get());
         seqPtr->AppendCommand(std::move(limitsPtr));
      }

      auto clonedPtr = std::unique_ptr<FormationTurnToHeadingCommand>(TransformClone());
      AddDependent(clonedPtr.get());
      clonedPtr->ShiftDelays(w);
      seqPtr->AppendCommand(std::move(clonedPtr));

      auto attPtr = ut::make_unique<FormationAttachCommand>();
      seqPtr->AppendCommand(std::move(attPtr));

      if (IsTurnLimited() && aChildPtr->IsLeaf())
      {
         auto resetPtr = ut::make_unique<FormationManeuverLimitsCommand>();
         seqPtr->AppendCommand(std::move(resetPtr));
      }

      retvalPtr = std::move(seqPtr);
   }

   return retvalPtr;
}

void wsf::six_dof::FormationTurnToHeadingCommand::ShiftDelays(double aExtraInitialDelay)
{
   mInitialDelay += aExtraInitialDelay;
   mFinalDelay -= aExtraInitialDelay;
}

void wsf::six_dof::FormationTurnToHeadingCommand::GreatestExtents(Formation* aRootPtr,
                                                                  Formation* aFormationPtr,
                                                                  double&    aRightmost,
                                                                  double&    aLeftmost)
{
   if (aFormationPtr->IsLeaf())
   {
      aRightmost = std::max(aRightmost, aFormationPtr->GetOffset().GetRight());
      aLeftmost  = std::min(aLeftmost, aFormationPtr->GetOffset().GetRight());
   }
   else
   {
      auto func = [&aRightmost, &aLeftmost, aRootPtr, aFormationPtr](Formation* aSubPtr)
      {
         if (aSubPtr->IsAttached())
         {
            double right{0.0};
            double left{0.0};
            FormationTurnToHeadingCommand::GreatestExtents(aRootPtr, aSubPtr, right, left);
            double offsetRight{0.0};
            if (aFormationPtr != aRootPtr)
            {
               offsetRight = aFormationPtr->GetOffset().GetRight();
            }
            aRightmost = std::max(aRightmost, right + offsetRight);
            aLeftmost  = std::min(aLeftmost, left + offsetRight);
         }
      };
      aFormationPtr->InvokeOnSubFormations(func);
   }
}

double wsf::six_dof::FormationTurnToHeadingCommand::MaxGLoadForTurn(Formation* aFormationPtr)
{
   double retval{-20.0};
   if (aFormationPtr->IsLeaf())
   {
      retval = aFormationPtr->GetMemberMover()->MaxPotentialManeuverGLoad();
   }
   else
   {
      auto func = [&retval](Formation* aSubPtr)
      {
         if (aSubPtr->IsAttached())
         {
            retval = std::max(retval, FormationTurnToHeadingCommand::MaxGLoadForTurn(aSubPtr));
         }
      };
      aFormationPtr->InvokeOnSubFormations(func);
   }
   return retval;
}

void wsf::six_dof::FormationTurnToHeadingCommand::FindOverallDelayTimes(Formation* aRootPtr,
                                                                        double&    aInitialDelay,
                                                                        double&    aFinalDelay) const
{
   std::vector<Formation*> leaves{};
   CollectAllLeaves(aRootPtr, leaves);

   for (Formation* leaf : leaves)
   {
      FormationOffset offset = leaf->GetOffsetFrom(*aRootPtr);
      double          d      = offset.GetRight() * mDelayFactor;
      double          l      = offset.GetAhead();
      double          w      = d - l;

      aInitialDelay = std::max(aInitialDelay, -w);
      aFinalDelay   = std::max(aFinalDelay, w);
   }
}

void wsf::six_dof::FormationTurnToHeadingCommand::CollectAllLeaves(Formation*               aFormationPtr,
                                                                   std::vector<Formation*>& aLeaves) const
{
   auto func = [&aLeaves, this](Formation* aSubPtr)
   {
      if (aSubPtr->IsLeaf())
      {
         aLeaves.push_back(aSubPtr);
      }
      else
      {
         CollectAllLeaves(aSubPtr, aLeaves);
      }
   };
   aFormationPtr->InvokeOnSubFormations(func);
}

void wsf::six_dof::FormationTurnToHeadingCommand::DependenceSatisfied()
{
   --mDependentCount;
}

void wsf::six_dof::FormationTurnToHeadingCommand::AddDependent(FormationTurnToHeadingCommand* aDependentPtr)
{
   ++mDependentCount;
   aDependentPtr->mNotifyWhenDone = this;
}

void wsf::six_dof::FormationTurnToHeadingCommand::SetupTurnLimits(FormationManeuverLimitsCommand* aLimitsPtr)
{
   if (mMaxBankAngleDeg > 0.0)
   {
      aLimitsPtr->SetBankAngleMaxDeg(mMaxBankAngleDeg);
   }
   if (mMaxRollRateDegPerSec > 0.0)
   {
      aLimitsPtr->SetRollRateMaxDegPerSec(mMaxRollRateDegPerSec);
   }
}

std::string wsf::six_dof::FormationTurnToHeadingCommand::TurnTypeDescription() const
{
   std::string retval{};
   switch (mTurnType)
   {
   case TurnType::cPLAIN_TURN:
      retval = std::string("PLAIN TURN");
      break;
   case TurnType::cDELAYED_TURN:
      retval = std::string("DELAYED TURN");
      break;
   case TurnType::cSMALL_ANGLE_TURN:
      retval = std::string("SMALL ANGLE TURN");
      break;
   }
   return retval;
}
