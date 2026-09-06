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

#include "WsfSixDOF_AutopilotAction.hpp"

#include "UtMath.hpp"
#include "WsfSixDOF_Waypoint.hpp"

wsf::six_dof::AutopilotAction* wsf::six_dof::AutopilotAction::Clone() const
{
   return new AutopilotAction(*this);
}

void wsf::six_dof::AutopilotAction::SetNavWaypoints(const Waypoint*             aPrevWpt,
                                                    const Waypoint*             aCurrWpt,
                                                    const Waypoint*             aNextWpt,
                                                    const Route::sRouteSegment* aCurrSegment,
                                                    const Route::sRouteSegment* aNextSegment)
{
   // Set the waypoint
   mPrevWaypointPtr = aPrevWpt;
   mCurrWaypointPtr = aCurrWpt;
   mNextWaypointPtr = aNextWpt;

   mCurrSegmentPtr = aCurrSegment;
   mNextSegmentPtr = aNextSegment;

   // Set the modes
   mLateralChannelMode  = Lateral::Waypoint;
   mVerticalChannelMode = Vertical::Waypoint;
   mSpeedChannelMode    = Speed::Waypoint;
}

void wsf::six_dof::AutopilotAction::SetLateralChannelMode(Lateral::Mode aMode)
{
   mLateralChannelMode = aMode;
}

void wsf::six_dof::AutopilotAction::SetStabilizingChannelMode(Lateral::Mode aMode)
{
   mStabilizingChannelMode = aMode;
}

void wsf::six_dof::AutopilotAction::SetVerticalChannelMode(Vertical::Mode aMode)
{
   mVerticalChannelMode = aMode;
}

void wsf::six_dof::AutopilotAction::SetSpeedChannelMode(Speed::Mode aMode)
{
   mSpeedChannelMode = aMode;
}

void wsf::six_dof::AutopilotAction::SetHeading_deg(float aHeading_deg)
{
   SetHeading_rad(aHeading_deg * static_cast<float>(UtMath::cRAD_PER_DEG));
}

void wsf::six_dof::AutopilotAction::SetHeading_rad(float aHeading_rad)
{
   mHeading_rad = aHeading_rad;
}

float wsf::six_dof::AutopilotAction::GetHeading_deg() const
{
   return mHeading_rad * static_cast<float>(UtMath::cDEG_PER_RAD);
}

void wsf::six_dof::AutopilotAction::SetBank_rad(float aBank_rad)
{
   mBank_rad = aBank_rad;
}

void wsf::six_dof::AutopilotAction::SetBank_deg(float aBank_deg)
{
   mBank_rad = aBank_deg * static_cast<float>(UtMath::cRAD_PER_DEG);
}

float wsf::six_dof::AutopilotAction::GetBank_deg() const
{
   return mBank_rad * static_cast<float>(UtMath::cDEG_PER_RAD);
}

void wsf::six_dof::AutopilotAction::SetRollRate_dps(float aRollRate_dps)
{
   mRollRate_dps = aRollRate_dps;
}

void wsf::six_dof::AutopilotAction::SetRollDelta_deg(float aRollDelta_deg)
{
   // We need to "remember" the current roll angle
   mRollDelta_deg = aRollDelta_deg;
}

void wsf::six_dof::AutopilotAction::SetYawGLoad_g(float aGLoad_g)
{
   mYawGLoad_g = aGLoad_g;
}

void wsf::six_dof::AutopilotAction::SetYawRate_dps(float aYawRate_dps)
{
   mYawRate_dps = aYawRate_dps;
}

void wsf::six_dof::AutopilotAction::SetBeta_deg(float aBeta_deg)
{
   mBeta_deg = aBeta_deg;
}

void wsf::six_dof::AutopilotAction::SetMach(float aMach)
{
   mMach = aMach;
}

void wsf::six_dof::AutopilotAction::SetTrueAirSpeed_KTAS(float aTrueAirSpeed_KTAS)
{
   mTrueAirSpeed_KTAS = aTrueAirSpeed_KTAS;
}

void wsf::six_dof::AutopilotAction::SetCalibratedAirSpeed_KCAS(float aCalibratedAirSpeed_KCAS)
{
   mCalibratedAirSpeed_KCAS = aCalibratedAirSpeed_KCAS;
}

void wsf::six_dof::AutopilotAction::SetForwardAcceleration_g(float aAcceleration_g)
{
   mForwardAcceleration_g = aAcceleration_g;
}

void wsf::six_dof::AutopilotAction::SetSpeed_fps(float aSpeed_fps)
{
   mSpeed_fps = aSpeed_fps;
}

void wsf::six_dof::AutopilotAction::SetThrottle(float aThrottle)
{
   mThrottle = aThrottle;
}

void wsf::six_dof::AutopilotAction::SetNoSpeedControl()
{
   mSpeedChannelMode = Speed::Undefined;
}

float wsf::six_dof::AutopilotAction::GetAltitudeMSL_m() const
{
   return mAltitudeMSL_ft * static_cast<float>(UtMath::cM_PER_FT);
}

void wsf::six_dof::AutopilotAction::SetAltitudeMSL_ft(float aAltitudeMSL_ft)
{
   mAltitudeMSL_ft = aAltitudeMSL_ft;
}

void wsf::six_dof::AutopilotAction::SetVerticalRate_fpm(float aVerticalRate_fpm)
{
   mVerticalRate_fpm = aVerticalRate_fpm;
}

void wsf::six_dof::AutopilotAction::SetPitchGLoad_g(float aGLoad_g)
{
   mPitchGLoad_g = aGLoad_g;
}

void wsf::six_dof::AutopilotAction::SetPitchAngle_deg(float aPitchAngle_deg)
{
   mPitchAngle_deg = aPitchAngle_deg;
}

void wsf::six_dof::AutopilotAction::SetPitchRate_dps(float aPitchRate_dps)
{
   mPitchRate_dps = aPitchRate_dps;
}

float wsf::six_dof::AutopilotAction::GetFlightPathAngle_deg() const
{
   return mFlightPathAngle_rad * static_cast<float>(UtMath::cDEG_PER_RAD);
}

void wsf::six_dof::AutopilotAction::SetFlightPathAngle_deg(float aVerticalAngle_deg)
{
   mFlightPathAngle_rad = aVerticalAngle_deg * static_cast<float>(UtMath::cRAD_PER_DEG);
}

void wsf::six_dof::AutopilotAction::SetDeltaPitch_deg(float aDeltaPitch_deg)
{
   mDeltaPitch_deg = aDeltaPitch_deg;
}

void wsf::six_dof::AutopilotAction::SetAlpha_deg(float aAlpha_deg)
{
   mAlpha_deg = aAlpha_deg;
}
