// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "P6DofAutopilotAction.hpp"

#include "P6DofWaypoint.hpp"
#include "UtMath.hpp"

P6DofAutopilotAction::P6DofAutopilotAction()
   : mPrevWaypointPtr(nullptr)
   , mCurrWaypointPtr(nullptr)
   , mNextWaypointPtr(nullptr)
   , mCurrSegmentPtr(nullptr)
   , mNextSegmentPtr(nullptr)
   , mHeading_rad(0.0f)
   , mRateOfTurn_dps(0.0f)
   , mBank_rad(0.0f)
   , mRadiusFt(0.0f)
   , mRollRate_dps(0.0f)
   , mRollDelta_deg(0.0f)
   , mYawGLoad_g(0.0f)
   , mYawRate_dps(0.0f)
   , mBeta_deg(0.0f)
   , mAltitudeMSL_ft(0.0f)
   , mVerticalRate_fpm(0.0f)
   , mFlightPathAngle_rad(0.0f)
   , mPitchAngle_deg(0.0f)
   , mPitchRate_dps(0.0f)
   , mPitchGLoad_g(0.0f)
   , mDeltaPitch_deg(0.0f)
   , mAlpha_deg(0.0f)
   , mMach(0.0f)
   , mTrueAirSpeed_KTAS(0.0f)
   , mCalibratedAirSpeed_KCAS(0.0f)
   , mForwardAcceleration_g(0.0f)
   , mSpeed_fps(0.0f)
   , mTurnDirection(P6DofCommonController::TD_DEFAULT)
   , mLateralChannelMode(P6DOF::Lateral::Undefined)
   , mStabilizingChannelMode(P6DOF::Lateral::Undefined)
   , mVerticalChannelMode(P6DOF::Vertical::Undefined)
   , mSpeedChannelMode(P6DOF::Speed::Undefined)
{
}

P6DofAutopilotAction* P6DofAutopilotAction::Clone() const
{
   return new P6DofAutopilotAction(*this);
}

void P6DofAutopilotAction::SetNavWaypoints(const P6DofWaypoint*             aPrevWpt,
                                           const P6DofWaypoint*             aCurrWpt,
                                           const P6DofWaypoint*             aNextWpt,
                                           const P6DofRoute::sRouteSegment* aCurrSegment,
                                           const P6DofRoute::sRouteSegment* aNextSegment)
{
   // Set the waypoint
   mPrevWaypointPtr = aPrevWpt;
   mCurrWaypointPtr = aCurrWpt;
   mNextWaypointPtr = aNextWpt;

   mCurrSegmentPtr = aCurrSegment;
   mNextSegmentPtr = aNextSegment;

   // Set the modes
   mLateralChannelMode  = P6DOF::Lateral::Waypoint;
   mVerticalChannelMode = P6DOF::Vertical::Waypoint;
   mSpeedChannelMode    = P6DOF::Speed::Waypoint;
}

void P6DofAutopilotAction::SetLateralChannelMode(P6DOF::Lateral::Mode aMode)
{
   mLateralChannelMode = aMode;
}

void P6DofAutopilotAction::SetStabilizingChannelMode(P6DOF::Lateral::Mode aMode)
{
   mStabilizingChannelMode = aMode;
}

void P6DofAutopilotAction::SetVerticalChannelMode(P6DOF::Vertical::Mode aMode)
{
   mVerticalChannelMode = aMode;
}

void P6DofAutopilotAction::SetSpeedChannelMode(P6DOF::Speed::Mode aMode)
{
   mSpeedChannelMode = aMode;
}

void P6DofAutopilotAction::SetHeading_deg(float aHeading_deg)
{
   SetHeading_rad((float)(aHeading_deg * UtMath::cRAD_PER_DEG));
}

void P6DofAutopilotAction::SetHeading_rad(float aHeading_rad)
{
   mHeading_rad = aHeading_rad;
}

float P6DofAutopilotAction::GetHeading_deg() const
{
   return (float)(mHeading_rad * UtMath::cDEG_PER_RAD);
}

void P6DofAutopilotAction::SetBank_rad(float aBank_rad)
{
   mBank_rad = aBank_rad;
}

void P6DofAutopilotAction::SetBank_deg(float aBank_deg)
{
   mBank_rad = (float)(aBank_deg * UtMath::cRAD_PER_DEG);
}

float P6DofAutopilotAction::GetBank_deg() const
{
   return (float)(mBank_rad * UtMath::cDEG_PER_RAD);
}

void P6DofAutopilotAction::SetRollRate_dps(float aRollRate_dps)
{
   mRollRate_dps = aRollRate_dps;
}

void P6DofAutopilotAction::SetRollDelta_deg(float aRollDelta_deg)
{
   // We need to "remember" the current roll angle
   mRollDelta_deg = aRollDelta_deg;
}

void P6DofAutopilotAction::SetYawGLoad_g(float aGLoad_g)
{
   mYawGLoad_g = aGLoad_g;
}

void P6DofAutopilotAction::SetYawRate_dps(float aYawRate_dps)
{
   mYawRate_dps = aYawRate_dps;
}

void P6DofAutopilotAction::SetBeta_deg(float aBeta_deg)
{
   mBeta_deg = aBeta_deg;
}

void P6DofAutopilotAction::SetMach(float aMach)
{
   mMach = aMach;
}

void P6DofAutopilotAction::SetTrueAirSpeed_KTAS(float aTrueAirSpeed_KTAS)
{
   mTrueAirSpeed_KTAS = aTrueAirSpeed_KTAS;
}

void P6DofAutopilotAction::SetCalibratedAirSpeed_KCAS(float aCalibratedAirSpeed_KCAS)
{
   mCalibratedAirSpeed_KCAS = aCalibratedAirSpeed_KCAS;
}

void P6DofAutopilotAction::SetForwardAcceleration_g(float aAcceleration_g)
{
   mForwardAcceleration_g = aAcceleration_g;
}

void P6DofAutopilotAction::SetSpeed_fps(float aSpeed_fps)
{
   mSpeed_fps = aSpeed_fps;
}

void P6DofAutopilotAction::SetNoSpeedControl()
{
   mSpeedChannelMode = P6DOF::Speed::Undefined;
}

float P6DofAutopilotAction::GetAltitudeMSL_m() const
{
   return (float)(mAltitudeMSL_ft * UtMath::cM_PER_FT);
}

void P6DofAutopilotAction::SetAltitudeMSL_ft(float aAltitudeMSL_ft)
{
   mAltitudeMSL_ft = aAltitudeMSL_ft;
}

void P6DofAutopilotAction::SetVerticalRate_fpm(float aVerticalRate_fpm)
{
   mVerticalRate_fpm = aVerticalRate_fpm;
}

void P6DofAutopilotAction::SetPitchGLoad_g(float aGLoad_g)
{
   mPitchGLoad_g = aGLoad_g;
}

void P6DofAutopilotAction::SetPitchAngle_deg(float aPitchAngle_deg)
{
   mPitchAngle_deg = aPitchAngle_deg;
}

void P6DofAutopilotAction::SetPitchRate_dps(float aPitchRate_dps)
{
   mPitchRate_dps = aPitchRate_dps;
}

float P6DofAutopilotAction::GetFlightPathAngle_deg() const
{
   return (float)(mFlightPathAngle_rad * UtMath::cDEG_PER_RAD);
}

void P6DofAutopilotAction::SetFlightPathAngle_deg(float aVerticalAngle_deg)
{
   mFlightPathAngle_rad = (float)(aVerticalAngle_deg * UtMath::cRAD_PER_DEG);
}

void P6DofAutopilotAction::SetDeltaPitch_deg(float aDeltaPitch_deg)
{
   mDeltaPitch_deg = aDeltaPitch_deg;
}

void P6DofAutopilotAction::SetAlpha_deg(float aAlpha_deg)
{
   mAlpha_deg = aAlpha_deg;
}
