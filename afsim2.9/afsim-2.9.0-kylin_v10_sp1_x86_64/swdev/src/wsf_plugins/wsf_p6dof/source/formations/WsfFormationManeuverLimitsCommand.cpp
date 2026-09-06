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

#include "WsfFormationManeuverLimitsCommand.hpp"

#include "WsfFormation.hpp"
#include "WsfP6DOF_Mover.hpp"

WsfFormationManeuverLimitsCommand::WsfFormationManeuverLimitsCommand()
   : WsfFormationCommand{}
   , mPitchG_LoadMax{0.0}
   , mRollRateMaxDegPerSec{0.0}
   , mBankAngleMaxDeg{0.0}
   , mSettingG_LoadMax{false}
   , mSettingRollRateMax{false}
   , mSettingBankAngleMax{false}
{
}

WsfFormationManeuverLimitsCommand* WsfFormationManeuverLimitsCommand::Clone() const
{
   return new WsfFormationManeuverLimitsCommand{*this};
}

double WsfFormationManeuverLimitsCommand::Execute(double aSimTime)
{
   WsfFormation* formPtr = GetFormation();
   if (formPtr && formPtr->IsLeaf())
   {
      WsfP6DOF_Mover* moverPtr = formPtr->GetMemberMover();
      if (moverPtr)
      {
         if (!mSettingBankAngleMax && !mSettingG_LoadMax && !mSettingRollRateMax)
         {
            moverPtr->RevertLimitsToDefaults();
         }
         else
         {
            if (mSettingBankAngleMax)
            {
               moverPtr->SetBankAngleMax(mBankAngleMaxDeg);
            }
            if (mSettingG_LoadMax)
            {
               moverPtr->SetPitchGLoadMax(mPitchG_LoadMax);
            }
            if (mSettingRollRateMax)
            {
               moverPtr->SetRollRateMax(mRollRateMaxDegPerSec);
            }
         }
      }
      else
      {
         throw std::runtime_error("Unable to set limits on member with no P6DOF mover");
      }
   }
   else
   {
      throw std::runtime_error("WsfFormationManeuverLimitsCommand can only be executed on leaves");
   }
   return -1.0;
}

bool WsfFormationManeuverLimitsCommand::AcceptCommand(WsfFormation* aFormationPtr)
{
   return aFormationPtr->IsLeaf();
}

void WsfFormationManeuverLimitsCommand::ComputeCommonTransformation(WsfFormation* aFormationPtr) {}

std::unique_ptr<WsfFormationCommand> WsfFormationManeuverLimitsCommand::ComputeTransformation(WsfFormation* aParentPtr,
                                                                                              WsfFormation* aChildPtr)
{
   return ut::clone(this);
}

void WsfFormationManeuverLimitsCommand::SetPitchG_LoadMax(double aPitchG_LoadMax)
{
   mPitchG_LoadMax   = aPitchG_LoadMax;
   mSettingG_LoadMax = true;
}

void WsfFormationManeuverLimitsCommand::SetRollRateMaxDegPerSec(double aRollRateMaxDegPerSec)
{
   mRollRateMaxDegPerSec = aRollRateMaxDegPerSec;
   mSettingRollRateMax   = true;
}

void WsfFormationManeuverLimitsCommand::SetBankAngleMaxDeg(double aBankAngleMaxDeg)
{
   mBankAngleMaxDeg     = aBankAngleMaxDeg;
   mSettingBankAngleMax = true;
}
