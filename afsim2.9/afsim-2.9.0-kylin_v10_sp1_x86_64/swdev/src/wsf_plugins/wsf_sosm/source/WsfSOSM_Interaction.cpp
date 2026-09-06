// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSOSM_Interaction.hpp"

#include <cmath>

#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfPlatform.hpp"

// =================================================================================================
WsfSOSM_Interaction::WsfSOSM_Interaction(WsfEM_Interaction& aSource)
   : mSource(aSource)
{
}

// =================================================================================================
float WsfSOSM_Interaction::GetSlantRange()
{
   return static_cast<float>(mSource.mRcvrToTgt.mRange);
}

// =================================================================================================
float WsfSOSM_Interaction::GetSensorAltitude()
{
   return static_cast<float>(mSource.mRcvrLoc.mAlt);
}

// =================================================================================================
float WsfSOSM_Interaction::GetSensorSpeed()
{
   return static_cast<float>(mSource.GetReceiver()->GetPlatform()->GetSpeed());
}

// =================================================================================================
float WsfSOSM_Interaction::GetTargetAltitude()
{
   return static_cast<float>(mSource.mTgtLoc.mAlt);
}

// =================================================================================================
float WsfSOSM_Interaction::GetTargetSpeed()
{
   return static_cast<float>(mSource.GetTarget()->GetSpeed());
}

// =================================================================================================
float WsfSOSM_Interaction::GetTargetThrottle()
{
   float throttle = 0.5F;

   // TODO - need some way to determine if the engine is on/off.

   // Determine if the afterburner is on...
   WsfPlatform* targetPtr = mSource.GetTarget();
   if (targetPtr->GetAppearance(16, 1) != 0)
   {
      throttle = 1.0F;
   }
   return throttle;
}

// =================================================================================================
float WsfSOSM_Interaction::GetAbsoluteTargetElevation()
{
   double snrToTgtLocNED[3];
   mSource.GetReceiver()->GetAntenna()->ConvertWCS_ToNED(mSource.mTgtLoc.mLocWCS, snrToTgtLocNED);
   double ne       = sqrt(snrToTgtLocNED[0] * snrToTgtLocNED[0] + snrToTgtLocNED[1] * snrToTgtLocNED[1]);
   float  absTgtEl = static_cast<float>(atan2(-snrToTgtLocNED[2], ne));
   return absTgtEl;
}

// =================================================================================================
void WsfSOSM_Interaction::GetSensorToTargetAspect(float& aAzimuth, float& aElevation)
{
   double snrToTgtLocWCS[3];
   mSource.GetReceiver()->GetPlatform()->GetRelativeLocationWCS(mSource.GetTarget(), snrToTgtLocWCS);
   double snrToTgtAz;
   double snrToTgtEl;
   mSource.GetReceiver()->GetPlatform()->ComputeAspect(snrToTgtLocWCS, snrToTgtAz, snrToTgtEl);
   aAzimuth   = static_cast<float>(snrToTgtAz);
   aElevation = static_cast<float>(snrToTgtEl);
}

// =================================================================================================
void WsfSOSM_Interaction::GetTargetToSensorAspect(float& aAzimuth, float& aElevation)
{
   aAzimuth   = static_cast<float>(mSource.mTgtToRcvr.mAz);
   aElevation = static_cast<float>(mSource.mTgtToRcvr.mEl);
}
