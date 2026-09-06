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

#include "WsfSixDOF_FormationManeuverLimitsCommand.hpp"

#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::FormationManeuverLimitsCommand::FormationManeuverLimitsCommand()
   : FormationCommand{}
   , mPitchG_LoadMax{0.0}
   , mRollRateMaxDegPerSec{0.0}
   , mBankAngleMaxDeg{0.0}
   , mSettingG_LoadMax{false}
   , mSettingRollRateMax{false}
   , mSettingBankAngleMax{false}
{
}

wsf::six_dof::FormationManeuverLimitsCommand* wsf::six_dof::FormationManeuverLimitsCommand::Clone() const
{
   return new FormationManeuverLimitsCommand{*this};
}

double wsf::six_dof::FormationManeuverLimitsCommand::Execute(double aSimTime)
{
   Formation* formPtr = GetFormation();
   if (formPtr && formPtr->IsLeaf())
   {
      Mover* moverPtr = formPtr->GetMemberMover();
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
         throw std::runtime_error("Unable to set limits on member with no valid mover");
      }
   }
   else
   {
      throw std::runtime_error("FormationManeuverLimitsCommand can only be executed on leaves");
   }
   return -1.0;
}

bool wsf::six_dof::FormationManeuverLimitsCommand::AcceptCommand(Formation* aFormationPtr)
{
   return aFormationPtr->IsLeaf();
}

void wsf::six_dof::FormationManeuverLimitsCommand::ComputeCommonTransformation(Formation* aFormationPtr) {}

std::unique_ptr<wsf::six_dof::FormationCommand>
wsf::six_dof::FormationManeuverLimitsCommand::ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr)
{
   return ut::clone(this);
}

void wsf::six_dof::FormationManeuverLimitsCommand::SetPitchG_LoadMax(double aPitchG_LoadMax)
{
   mPitchG_LoadMax   = aPitchG_LoadMax;
   mSettingG_LoadMax = true;
}

void wsf::six_dof::FormationManeuverLimitsCommand::SetRollRateMaxDegPerSec(double aRollRateMaxDegPerSec)
{
   mRollRateMaxDegPerSec = aRollRateMaxDegPerSec;
   mSettingRollRateMax   = true;
}

void wsf::six_dof::FormationManeuverLimitsCommand::SetBankAngleMaxDeg(double aBankAngleMaxDeg)
{
   mBankAngleMaxDeg     = aBankAngleMaxDeg;
   mSettingBankAngleMax = true;
}
