// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfInstantAttitudeController.hpp"

#include "WsfPlatform.hpp"
#include "WsfSpaceMoverBase.hpp"

WsfInstantAttitudeController::WsfInstantAttitudeController()
   : WsfAttitudeController{}
   , mTargetPsiECI{0.0}
   , mTargetThetaECI{0.0}
   , mTargetPhiECI{0.0}
{
}

void WsfInstantAttitudeController::SetTargetOrientationECI(double aPsiECI, double aThetaECI, double aPhiECI)
{
   mTargetPsiECI   = aPsiECI;
   mTargetThetaECI = aThetaECI;
   mTargetPhiECI   = aPhiECI;
}

void WsfInstantAttitudeController::GetTargetOrientationECI(double& aPsiECI, double& aThetaECI, double& aPhiECI) const
{
   aPsiECI   = mTargetPsiECI;
   aThetaECI = mTargetThetaECI;
   aPhiECI   = mTargetPhiECI;
}

void WsfInstantAttitudeController::UpdateOrientation(double aDeltaT)
{
   GetMover()->GetPlatform()->SetOrientationECI(mTargetPsiECI, mTargetThetaECI, mTargetPhiECI);
}

bool WsfInstantAttitudeController::AtTargetOrientation() const
{
   return true;
}
