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

#ifndef P6DOFAUTOPILOTACTION_H
#define P6DOFAUTOPILOTACTION_H

#include "p6dof_export.h"

#include "P6DofRoute.hpp"

class P6DofWaypoint;

class P6DOF_EXPORT P6DofAutopilotAction
{
public:
   P6DofAutopilotAction();
   ~P6DofAutopilotAction()       = default;
   P6DofAutopilotAction& operator=(const P6DofAutopilotAction&) = delete;

   P6DofAutopilotAction* Clone() const;

   // Waypoint info ..............................................................

   // This returns the previous waypoint
   const P6DofWaypoint* GetPrevWaypoint() const { return mPrevWaypointPtr; }

   // This returns the current waypoint
   const P6DofWaypoint* GetCurrWaypoint() const { return mCurrWaypointPtr; }

   // This returns the next waypoint (beyond the current waypoint)
   const P6DofWaypoint* GetNextWaypoint() const { return mNextWaypointPtr; }

   // This returns the current route segment
   const P6DofRoute::sRouteSegment* GetCurrSegment() const { return mCurrSegmentPtr; }

   // This returns the next route segment
   const P6DofRoute::sRouteSegment* GetNextSegment() const { return mNextSegmentPtr; }

   // Set the previous, current, and next waypoints needed to navigate this action,
   // as well as the route segments between the waypoints
   void SetNavWaypoints(const P6DofWaypoint*             aPrevWpt,
                        const P6DofWaypoint*             aCurrWpt,
                        const P6DofWaypoint*             aNextWpt,
                        const P6DofRoute::sRouteSegment* aCurrSegment,
                        const P6DofRoute::sRouteSegment* aNextSegment);

   // Lateral channel (yaw/roll) .................................................

   // TODO -- We still need to support:
   //  Lateral::Taxi_Waypoint
   //  Lateral::Taxi_Heading
   //  Lateral::Point

   void SetLateralChannelMode(P6DOF::Lateral::Mode aMode);
   void SetStabilizingChannelMode(P6DOF::Lateral::Mode aMode);
   void SetVerticalChannelMode(P6DOF::Vertical::Mode aMode);
   void SetSpeedChannelMode(P6DOF::Speed::Mode aMode);

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

   P6DOF::Lateral::Mode  GetLateralChannelMode() const { return mLateralChannelMode; }
   P6DOF::Lateral::Mode  GetStabilizingChannelMode() const { return mStabilizingChannelMode; }
   P6DOF::Vertical::Mode GetVerticalChannelMode() const { return mVerticalChannelMode; }
   P6DOF::Speed::Mode    GetSpeedChannelMode() const { return mSpeedChannelMode; }

   // Turn direction accessor
   P6DofCommonController::eAutopilotTurnDir GetAPTurnDir() const { return mTurnDirection; }

protected:
   P6DofAutopilotAction(const P6DofAutopilotAction& aSrc) = default;

   // Data needed to navigate the route
   const P6DofWaypoint*             mPrevWaypointPtr;
   const P6DofWaypoint*             mCurrWaypointPtr;
   const P6DofWaypoint*             mNextWaypointPtr;
   const P6DofRoute::sRouteSegment* mCurrSegmentPtr;
   const P6DofRoute::sRouteSegment* mNextSegmentPtr;

   // Lateral channel

   float mHeading_rad; // -pi to pi abs, or -INF to INF for rel
   float mRateOfTurn_dps;
   float mBank_rad; // -pi to pi abs, or -INF to INF for rel
   float mRadiusFt;
   float mRollRate_dps;
   float mRollDelta_deg;
   float mYawGLoad_g;
   float mYawRate_dps;
   float mBeta_deg;

   // Vertical channel

   float mAltitudeMSL_ft;
   float mVerticalRate_fpm;
   float mFlightPathAngle_rad;
   float mPitchAngle_deg;
   float mPitchRate_dps;
   float mPitchGLoad_g;
   float mDeltaPitch_deg;
   float mAlpha_deg;

   // Speed channel

   float mMach;
   float mTrueAirSpeed_KTAS;
   float mCalibratedAirSpeed_KCAS;
   float mForwardAcceleration_g;
   float mSpeed_fps;

   P6DofCommonController::eAutopilotTurnDir mTurnDirection;

   P6DOF::Lateral::Mode  mLateralChannelMode;
   P6DOF::Lateral::Mode  mStabilizingChannelMode;
   P6DOF::Vertical::Mode mVerticalChannelMode;
   P6DOF::Speed::Mode    mSpeedChannelMode;
};

#endif // P6DOFAUTOPILOTACTION_H
