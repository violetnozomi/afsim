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

#ifndef WSFSIXDOFAUTOPILOTACTION_HPP
#define WSFSIXDOFAUTOPILOTACTION_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Route.hpp"
#include "WsfSixDOF_VehicleData.hpp"
class Waypoint;

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT AutopilotAction
{
public:
   AutopilotAction()        = default;
   ~AutopilotAction()       = default;
   AutopilotAction& operator=(const AutopilotAction&) = delete;

   AutopilotAction* Clone() const;

   // Waypoint info ..............................................................

   // This returns the previous waypoint
   const Waypoint* GetPrevWaypoint() const { return mPrevWaypointPtr; }

   // This returns the current waypoint
   const Waypoint* GetCurrWaypoint() const { return mCurrWaypointPtr; }

   // This returns the next waypoint (beyond the current waypoint)
   const Waypoint* GetNextWaypoint() const { return mNextWaypointPtr; }

   // This returns the current route segment
   const Route::sRouteSegment* GetCurrSegment() const { return mCurrSegmentPtr; }

   // This returns the next route segment
   const Route::sRouteSegment* GetNextSegment() const { return mNextSegmentPtr; }

   // Set the previous, current, and next waypoints needed to navigate this action,
   // as well as the route segments between the waypoints
   void SetNavWaypoints(const Waypoint*             aPrevWpt,
                        const Waypoint*             aCurrWpt,
                        const Waypoint*             aNextWpt,
                        const Route::sRouteSegment* aCurrSegment,
                        const Route::sRouteSegment* aNextSegment);

   // Lateral channel (yaw/roll) .................................................

   // TODO -- We still need to support:
   //  Lateral::Taxi_Waypoint
   //  Lateral::Taxi_Heading
   //  Lateral::Point

   void SetLateralChannelMode(Lateral::Mode aMode);
   void SetStabilizingChannelMode(Lateral::Mode aMode);
   void SetVerticalChannelMode(Vertical::Mode aMode);
   void SetSpeedChannelMode(Speed::Mode aMode);

   float GetHeading_rad() const { return mHeading_rad; }
   void  SetHeading_rad(float aHeading_rad);
   float GetHeading_deg() const;
   void  SetHeading_deg(float aHeading_deg);

   float GetBank_rad() const { return mBank_rad; }
   void  SetBank_rad(float aBank_rad);
   float GetBank_deg() const;
   void  SetBank_deg(float aBank_deg);

   float GetRollRate_dps() const { return mRollRate_dps; }
   void  SetRollRate_dps(float aRollRate_dps);

   float GetRollDelta_deg() const { return mRollDelta_deg; }
   void  SetRollDelta_deg(float aRollDelta_deg);

   float GetYawGLoad_g() const { return mYawGLoad_g; }
   void  SetYawGLoad_g(float aGLoad_g);

   float GetYawRate_dps() const { return mYawRate_dps; }
   void  SetYawRate_dps(float aYawRate_dps);

   float GetBeta_deg() const { return mBeta_deg; }
   void  SetBeta_deg(float aBeta_deg);

   // Speed channel ..............................................................

   float GetForwardAcceleration_g() const { return mForwardAcceleration_g; }
   void  SetForwardAcceleration_g(float aAcceleration_g);

   float GetCalibratedAirSpeed_KCAS() const { return mCalibratedAirSpeed_KCAS; }
   void  SetCalibratedAirSpeed_KCAS(float aCalibratedAirSpeed_KCAS);

   float GetTrueAirSpeed_KTAS() const { return mTrueAirSpeed_KTAS; }
   void  SetTrueAirSpeed_KTAS(float aTrueAirSpeed_KTAS);

   float GetMach() const { return mMach; }
   void  SetMach(float aMach);

   float GetSpeed_fps() const { return mSpeed_fps; }
   void  SetSpeed_fps(float aSpeed_fps);

   float GetThrottle() const { return mThrottle; }
   void  SetThrottle(float aThrottle);

   void SetNoSpeedControl();

   // Vertical channel (pitch) ...................................................

   // TODO - We still need to support:
   //  Vertical::Point

   float GetAltitudeMSL_ft() const { return mAltitudeMSL_ft; }
   float GetAltitudeMSL_m() const;
   void  SetAltitudeMSL_ft(float aAltitudeMSL_ft);

   float GetVerticalRate_fpm() const { return mVerticalRate_fpm; }
   void  SetVerticalRate_fpm(float aVerticalRate_fpm);

   float GetPitchGLoad_g() const { return mPitchGLoad_g; }
   void  SetPitchGLoad_g(float aGLoad_g);

   float GetPitchAngle_deg() const { return mPitchAngle_deg; }
   void  SetPitchAngle_deg(float aPitchAngle_deg);

   float GetPitchRate_dps() const { return mPitchRate_dps; }
   void  SetPitchRate_dps(float aPitchRate_dps);

   float GetFlightPathAngle_deg() const;
   void  SetFlightPathAngle_deg(float aVerticalAngle_deg);

   float GetDeltaPitch_deg() const { return mDeltaPitch_deg; }
   void  SetDeltaPitch_deg(float aDeltaPitch_deg);

   float GetAlpha_deg() const { return mAlpha_deg; }
   void  SetAlpha_deg(float aAlpha_deg);

   // Mode accessors .............................................................

   Lateral::Mode  GetLateralChannelMode() const { return mLateralChannelMode; }
   Lateral::Mode  GetStabilizingChannelMode() const { return mStabilizingChannelMode; }
   Vertical::Mode GetVerticalChannelMode() const { return mVerticalChannelMode; }
   Speed::Mode    GetSpeedChannelMode() const { return mSpeedChannelMode; }

   // Turn direction accessor
   CommonController::eAutopilotTurnDir GetAPTurnDir() const { return mTurnDirection; }

protected:
   AutopilotAction(const AutopilotAction& aSrc) = default;

   // Data needed to navigate the route
   const Waypoint*             mPrevWaypointPtr = nullptr;
   const Waypoint*             mCurrWaypointPtr = nullptr;
   const Waypoint*             mNextWaypointPtr = nullptr;
   const Route::sRouteSegment* mCurrSegmentPtr  = nullptr;
   const Route::sRouteSegment* mNextSegmentPtr  = nullptr;

   // Lateral channel

   float mHeading_rad    = 0.0f; // -pi to pi abs, or -INF to INF for rel
   float mRateOfTurn_dps = 0.0f;
   float mBank_rad       = 0.0f; // -pi to pi abs, or -INF to INF for rel
   float mRadiusFt       = 0.0f;
   float mRollRate_dps   = 0.0f;
   float mRollDelta_deg  = 0.0f;
   float mYawGLoad_g     = 0.0f;
   float mYawRate_dps    = 0.0f;
   float mBeta_deg       = 0.0f;

   // Vertical channel

   float mAltitudeMSL_ft      = 0.0f;
   float mVerticalRate_fpm    = 0.0f;
   float mFlightPathAngle_rad = 0.0f;
   float mPitchAngle_deg      = 0.0f;
   float mPitchRate_dps       = 0.0f;
   float mPitchGLoad_g        = 0.0f;
   float mDeltaPitch_deg      = 0.0f;
   float mAlpha_deg           = 0.0f;

   // Speed channel

   float mMach                    = 0.0f;
   float mTrueAirSpeed_KTAS       = 0.0f;
   float mCalibratedAirSpeed_KCAS = 0.0f;
   float mForwardAcceleration_g   = 0.0f;
   float mSpeed_fps               = 0.0f;
   float mThrottle                = 0.0f;

   CommonController::eAutopilotTurnDir mTurnDirection = CommonController::TD_DEFAULT;

   Vertical::Mode mVerticalChannelMode    = Vertical::Undefined;
   Lateral::Mode  mLateralChannelMode     = Lateral::Undefined;
   Lateral::Mode  mStabilizingChannelMode = Lateral::Undefined;
   Speed::Mode    mSpeedChannelMode       = Speed::Undefined;
};
} // namespace six_dof
} // namespace wsf

#endif // COMMONAIRAUTOPILOTACTION_H
