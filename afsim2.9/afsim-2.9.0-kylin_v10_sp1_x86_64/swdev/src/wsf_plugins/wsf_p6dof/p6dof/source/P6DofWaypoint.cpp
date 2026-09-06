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

#include "P6DofWaypoint.hpp"

P6DofWaypoint::P6DofWaypoint()
   : mPosition_lla(UtLLAPos(0, 0, 0))
   , mLabel("")
   , mGoTo("")
   , mFollowHorizontalTrack(true)
   , mFollowVerticalTrack(false)
   , mWaypointOnPassing(false)
   , mId(-1)
{
   // We will use a 2-g turn (60 deg bank) as the default
   SetMaxTurnG(TURN_G_TYPE_PILOT, 2.0);

   // we will use a default speed of 400 ktas
   SetSpeed(SPD_TYPE_TAS_KNOTS, 400);
}

P6DofWaypoint::P6DofWaypoint(double aLat_m, double aLon_m, double aAlt_m)
   : mPosition_lla(UtLLAPos(aLat_m, aLon_m, aAlt_m))
   , mLabel("")
   , mGoTo("")
   , mFollowHorizontalTrack(true)
   , mFollowVerticalTrack(false)
   , mWaypointOnPassing(false)
   , mId(-1)
{
   // Default turn G to produce a 60 degree bank
   SetMaxTurnG(TURN_G_TYPE_PILOT, 2.0);

   // Default speed to fly at 400 ktas
   SetSpeed(SPD_TYPE_TAS_KNOTS, 400);
}

P6DofWaypoint* P6DofWaypoint::Clone()
{
   return new P6DofWaypoint(*this);
}

void P6DofWaypoint::SetFollowHorizontalTrack(bool aCommand)
{
   mFollowHorizontalTrack = aCommand;
}

void P6DofWaypoint::SetFollowVerticalTrack(bool aCommand)
{
   mFollowVerticalTrack = aCommand;
}

void P6DofWaypoint::SetWaypointOnPassing(bool aCommand)
{
   mWaypointOnPassing = aCommand;
}

bool P6DofWaypoint::FollowHorizontalTrack() const
{
   return mFollowHorizontalTrack;
}

bool P6DofWaypoint::FollowVerticalTrack() const
{
   return mFollowVerticalTrack;
}

bool P6DofWaypoint::WaypointOnPassing() const
{
   return mWaypointOnPassing;
}

int P6DofWaypoint::GetId() const
{
   return mId;
}

void P6DofWaypoint::SetLLA(const UtLLAPos& aPos_lla)
{
   mPosition_lla = aPos_lla;
}

UtLLAPos P6DofWaypoint::GetLLA() const
{
   return mPosition_lla;
}

void P6DofWaypoint::SetSpeed(sSpeed aSpeed)
{
   mSpeed = aSpeed;
}

void P6DofWaypoint::SetSpeed(eSpeedType aType, double aValue)
{
   mSpeed.type = aType;
   mSpeed.val  = aValue;
}

P6DofWaypoint::sSpeed P6DofWaypoint::GetSpeed() const
{
   return mSpeed;
}

void P6DofWaypoint::SetMaxTurnG(eTurnGType aType, double aTurnG)
{
   mMaxG_g.type = aType;
   mMaxG_g.val  = aTurnG;
}

P6DofWaypoint::sTurnG P6DofWaypoint::GetMaxTurnG() const
{
   return mMaxG_g;
}

void P6DofWaypoint::SetLabel(const std::string& aLabel)
{
   mLabel = aLabel;
}

std::string P6DofWaypoint::GetLabel() const
{
   return mLabel;
}

void P6DofWaypoint::SetGoTo(const std::string& aLabel)
{
   mGoTo = aLabel;
}

void P6DofWaypoint::SetId(int aId)
{
   mId = aId;
}

std::string P6DofWaypoint::GetGoTo() const
{
   return mGoTo;
}
