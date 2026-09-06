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

#ifndef WSFPM_MOVER_HPP
#define WSFPM_MOVER_HPP

#include "wsf_parser_export.h"

#include "WsfPM_ObjectMap.hpp"
#include "WsfPM_PlatformPart.hpp"
#include "WsfPM_Route.hpp"

class WSF_PARSER_EXPORT WsfPM_Mover : public WsfPM_PlatformPart
{
public:
   static bool IsWaypointMover(const WsfPProxyNode& aNode) noexcept;

   WsfPM_Mover();
   WsfPM_Mover(const WsfPProxyNode& aNode);

   WsfPM_Route DefaultRoute() const;
   std::string UsingRoute() const;
   WsfPM_Route InitialRoute() const;
   std::string StartAt() const { return GetValue().GetAttr("startAt").ValueToString(); }
   //! Get the default altitude reference for a waypoint
   //! @retval AltitudeReferenceEnum::MSL if the waypoint is defined on a WSF_AIR_MOVER
   //! @retval AltitudeReferenceEnum::AGL if the waypoint is defined on other WSF_ROUTE_MOVERs
   wsf::proxy::AltitudeReferenceEnum DefaultAltitudeReference() const noexcept;
};

class WSF_PARSER_EXPORT WsfPM_WaypointMover : public WsfPM_Mover
{
public:
   WsfPM_WaypointMover()
      : WsfPM_Mover()
   {
   }
   WsfPM_WaypointMover(const WsfPProxyNode& aNode)
      : WsfPM_Mover(aNode)
   {
      assert(IsWaypointMover(aNode));
   }

   WsfProxy::Acceleration          MaximumRadialAcceleration() const;
   WsfProxy::Acceleration          MaximumLinearAcceleration() const;
   WsfProxy::Speed                 MaximumClimbRate() const;
   WsfProxy::Speed                 DefaultClimbRate() const;
   WsfProxy::Acceleration          DefaultLinearAcceleration() const;
   WsfProxy::Acceleration          TurnG_Limit() const;
   WsfProxy::Acceleration          BodyG_Limit() const;
   WsfProxy::AngularRate           TurnRateLimit() const;
   WsfProxy::AngularRate           RollRateLimit() const;
   WsfProxy::AngularRate           MaximumTurnRate() const;
   WsfProxy::Angle                 BankAngleLimit() const;
   WsfProxy::Angle                 MaximumFlightPathAngle() const;
   WsfProxy::Length                MinimumAltitude() const;
   WsfProxy::Length                MaximumAltitude() const;
   WsfProxy::Speed                 MinimumSpeed() const;
   WsfProxy::Speed                 MaximumSpeed() const;
   WsfProxy::Bool                  SwitchOnPassing() const;
   WsfProxy::Double                HeadingPursuitGain() const;
   WsfProxy::Bool                  IsOnGround() const;
   WsfPM_Waypoint::EndOfPathOption EndOfPath() const;
};

using WsfPM_MoverMap = WsfPM_ObjectMapT<WsfPM_Mover>;

#endif
