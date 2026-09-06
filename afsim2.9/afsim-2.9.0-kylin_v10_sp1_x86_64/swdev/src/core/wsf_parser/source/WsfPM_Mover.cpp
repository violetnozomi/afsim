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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfPM_Mover.hpp"

#include "WsfPM_Root.hpp"

bool WsfPM_Mover::IsWaypointMover(const WsfPProxyNode& aNode) noexcept
{
   return aNode.IsOfType("WSF_WAYPOINT_MOVER");
}

WsfPM_Mover::WsfPM_Mover() {}

WsfPM_Mover::WsfPM_Mover(const WsfPProxyNode& aNode)
   : WsfPM_PlatformPart(aNode)
{
}

WsfPM_Route WsfPM_Mover::DefaultRoute() const
{
   return WsfPM_Route(*this + "defaultRoute");
}

std::string WsfPM_Mover::UsingRoute() const
{
   return GetValue().GetAttr("useRoute").ValueToString();
}

WsfPM_Route WsfPM_Mover::InitialRoute() const
{
   std::string usingName = UsingRoute();

   if (!usingName.empty())
   {
      return WsfPM_Root(Root()).routes().FindT(usingName);
   }

   return DefaultRoute();
}

wsf::proxy::AltitudeReferenceEnum WsfPM_Mover::DefaultAltitudeReference() const noexcept
{
   if (IsA_StructTypeOf("WSF_AIR_MOVER"))
   {
      return wsf::proxy::AltitudeReferenceEnum::MSL;
   }
   else
   {
      return wsf::proxy::AltitudeReferenceEnum::AGL;
   }
}

WsfProxy::Acceleration WsfPM_WaypointMover::MaximumRadialAcceleration() const
{
   return GetAttributeT<WsfProxy::Acceleration>("maximumRadialAcceleration");
}

WsfProxy::Bool WsfPM_WaypointMover::SwitchOnPassing() const
{
   return GetAttributeT<WsfProxy::Bool>("switchOnPassing");
}

WsfProxy::Acceleration WsfPM_WaypointMover::MaximumLinearAcceleration() const
{
   return GetAttributeT<WsfProxy::Acceleration>("maximumLinearAcceleration");
}

WsfProxy::Speed WsfPM_WaypointMover::MaximumClimbRate() const
{
   return GetAttributeT<WsfProxy::Speed>("maximumClimbRate");
}

WsfProxy::Speed WsfPM_WaypointMover::DefaultClimbRate() const
{
   return GetAttributeT<WsfProxy::Speed>("defaultClimbRate");
}

WsfProxy::Acceleration WsfPM_WaypointMover::DefaultLinearAcceleration() const
{
   return GetAttributeT<WsfProxy::Acceleration>("defaultLinearAcceleration");
}

WsfProxy::Acceleration WsfPM_WaypointMover::TurnG_Limit() const
{
   return GetAttributeT<WsfProxy::Acceleration>("turnG_Limit");
}

WsfProxy::AngularRate WsfPM_WaypointMover::TurnRateLimit() const
{
   return GetAttributeT<WsfProxy::AngularRate>("turnRateLimit");
}

WsfProxy::AngularRate WsfPM_WaypointMover::MaximumTurnRate() const
{
   return GetAttributeT<WsfProxy::AngularRate>("maximumTurnRate");
}

WsfProxy::Angle WsfPM_WaypointMover::BankAngleLimit() const
{
   return GetAttributeT<WsfProxy::Angle>("bankAngleLimit");
}

WsfProxy::Length WsfPM_WaypointMover::MinimumAltitude() const
{
   return GetAttributeT<WsfProxy::Length>("minimumAltitude");
}

WsfProxy::Length WsfPM_WaypointMover::MaximumAltitude() const
{
   return GetAttributeT<WsfProxy::Length>("maximumAltitude");
}

WsfProxy::Speed WsfPM_WaypointMover::MinimumSpeed() const
{
   return GetAttributeT<WsfProxy::Speed>("minimumSpeed");
}

WsfProxy::Speed WsfPM_WaypointMover::MaximumSpeed() const
{
   return GetAttributeT<WsfProxy::Speed>("maximumSpeed");
}

WsfProxy::Angle WsfPM_WaypointMover::MaximumFlightPathAngle() const
{
   return GetAttributeT<WsfProxy::Angle>("maximumFlightPathAngle");
}

WsfProxy::Acceleration WsfPM_WaypointMover::BodyG_Limit() const
{
   return GetAttributeT<WsfProxy::Acceleration>("bodyG_Limit");
}

WsfProxy::AngularRate WsfPM_WaypointMover::RollRateLimit() const
{
   return GetAttributeT<WsfProxy::AngularRate>("rollRateLimit");
}

WsfProxy::Double WsfPM_WaypointMover::HeadingPursuitGain() const
{
   return GetAttributeT<WsfProxy::Double>("headingPursuitGain");
}

WsfProxy::Bool WsfPM_WaypointMover::IsOnGround() const
{
   return GetAttributeT<WsfProxy::Bool>("isOnGround");
}

WsfPM_Waypoint::EndOfPathOption WsfPM_WaypointMover::EndOfPath() const
{
   int eop;
   GetValue().GetAttr("endOfPath").GetIntegerValue(eop);
   if (eop < WsfPM_Waypoint::cEP_EXTRAPOLATE || eop > WsfPM_Waypoint::cEP_REMOVE)
   {
      return WsfPM_Waypoint::cEP_EXTRAPOLATE;
   }
   return (WsfPM_Waypoint::EndOfPathOption)eop;
}
