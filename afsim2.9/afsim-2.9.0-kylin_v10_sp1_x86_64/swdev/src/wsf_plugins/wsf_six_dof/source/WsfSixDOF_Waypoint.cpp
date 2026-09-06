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

#include "WsfSixDOF_Waypoint.hpp"

wsf::six_dof::Waypoint::Waypoint(double aLat_m, double aLon_m, double aAlt_m)
   : mPosition_lla(UtLLAPos(aLat_m, aLon_m, aAlt_m))
{
}

wsf::six_dof::Waypoint* wsf::six_dof::Waypoint::Clone()
{
   return new Waypoint(*this);
}

void wsf::six_dof::Waypoint::SetFollowHorizontalTrack(bool aCommand)
{
   mFollowHorizontalTrack = aCommand;
}

void wsf::six_dof::Waypoint::SetFollowVerticalTrack(bool aCommand)
{
   mFollowVerticalTrack = aCommand;
}

void wsf::six_dof::Waypoint::SetWaypointOnPassing(bool aCommand)
{
   mWaypointOnPassing = aCommand;
}

bool wsf::six_dof::Waypoint::FollowHorizontalTrack() const
{
   return mFollowHorizontalTrack;
}

bool wsf::six_dof::Waypoint::FollowVerticalTrack() const
{
   return mFollowVerticalTrack;
}

bool wsf::six_dof::Waypoint::WaypointOnPassing() const
{
   return mWaypointOnPassing;
}

int wsf::six_dof::Waypoint::GetId() const
{
   return mId;
}

void wsf::six_dof::Waypoint::SetLLA(const UtLLAPos& aPos_lla)
{
   mPosition_lla = aPos_lla;
}

UtLLAPos wsf::six_dof::Waypoint::GetLLA() const
{
   return mPosition_lla;
}

void wsf::six_dof::Waypoint::SetSpeed(sSpeed aSpeed)
{
   mSpeed = aSpeed;
}

void wsf::six_dof::Waypoint::SetSpeed(eSpeedType aType, double aValue)
{
   mSpeed.type = aType;
   mSpeed.val  = aValue;
}

wsf::six_dof::Waypoint::sSpeed wsf::six_dof::Waypoint::GetSpeed() const
{
   return mSpeed;
}

void wsf::six_dof::Waypoint::SetMaxTurnG(eTurnGType aType, double aTurnG)
{
   mMaxG_g.type = aType;
   mMaxG_g.val  = aTurnG;
}

wsf::six_dof::Waypoint::sTurnG wsf::six_dof::Waypoint::GetMaxTurnG() const
{
   return mMaxG_g;
}

void wsf::six_dof::Waypoint::SetLabel(const std::string& aLabel)
{
   mLabel = aLabel;
}

std::string wsf::six_dof::Waypoint::GetLabel() const
{
   return mLabel;
}

void wsf::six_dof::Waypoint::SetGoTo(const std::string& aLabel)
{
   mGoTo = aLabel;
}

void wsf::six_dof::Waypoint::SetId(int aId)
{
   mId = aId;
}

std::string wsf::six_dof::Waypoint::GetGoTo() const
{
   return mGoTo;
}
