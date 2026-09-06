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

#include "WsfPM_Waypoint.hpp"

#include "UtLexicalCast.hpp"
#include "WsfPM_Mover.hpp"

WsfPM_Waypoint::WsfPM_Waypoint() {}

WsfPM_Waypoint::WsfPM_Waypoint(const WsfPProxyNode& aNode)
   : WsfPProxyNode(aNode)
{
}

WsfPM_Waypoint::WaypointType WsfPM_Waypoint::GetWaypointType() const
{
   int wptTypeInt = -1;

   GetValue().GetAttr("waypointType").GetIntegerValue(wptTypeInt);

   switch (wptTypeInt)
   {
   case 0:
      return cPOSITION;
   case 1:
      return cOFFSET;
   case 2:
      return cTURN;
   default:
      return cOTHER;
   }
}

WsfProxy::Position WsfPM_Waypoint::Position() const noexcept
{
   return GetAttributeT<WsfProxy::Position>("position");
}

WsfPProxyNode WsfPM_Waypoint::PositionNode() const noexcept
{
   return *this + "position";
}

bool WsfPM_Waypoint::SetPosition(WsfProxy::Position aPosition)
{
   return AssignAttribute("position", aPosition);
}

WsfProxy::Speed WsfPM_Waypoint::Speed() const
{
   return GetAttributeT<WsfProxy::Speed>("speed");
}

WsfProxy::Length WsfPM_Waypoint::Altitude() const
{
   return GetAttributeT<WsfProxy::Length>("altitude");
}
bool WsfPM_Waypoint::SetAltitude(WsfProxy::Length aAltitude)
{
   return AssignAttribute("altitude", aAltitude);
}

WsfProxy::Length2 WsfPM_Waypoint::Offset() const
{
   return GetAttributeT<WsfProxy::Length2>("offset");
}

bool WsfPM_Waypoint::SetOffset(WsfProxy::Length2 aOffset)
{
   return AssignAttribute("offset", aOffset);
}

WsfProxy::Angle WsfPM_Waypoint::HeadingChange() const
{
   return GetAttributeT<WsfProxy::Angle>("headingChange");
}

WsfProxy::Int WsfPM_Waypoint::GetAltitudeReference() const
{
   return GetAttributeT<WsfProxy::Int>("altitudeReference");
}

wsf::proxy::AltitudeReferenceEnum WsfPM_Waypoint::AltitudeReferenceValue() const noexcept
{
   WsfProxy::Int altRef    = GetAltitudeReference();
   auto          altRefVal = static_cast<wsf::proxy::AltitudeReferenceEnum>(altRef.GetValue());
   if (IsValid() && IsOfType("Waypoint") && altRefVal == wsf::proxy::AltitudeReferenceEnum::Default)
   {
      WsfPM_WaypointList waypoints{GetParent()};
      if (waypoints && waypoints.IsA_ListTypeOf("List/Waypoint"))
      {
         WsfPM_Route route{waypoints.GetParent()};
         if (route && route.IsOfType("Route"))
         {
            WsfPM_Mover mover{route.GetParent()};
            if (mover && mover.IsOfType("Mover"))
            {
               altRefVal = mover.DefaultAltitudeReference();
            }
         }
      }
   }
   return altRefVal;
}

WsfPM_Waypoint::EndOfPathOption WsfPM_Waypoint::EndOfPath() const
{
   int ival = cEP_DEFAULT;
   GetValue().GetAttr("endOfPath").GetIntegerValue(ival);
   if (ival < cEP_DEFAULT || ival > cEP_REMOVE)
   {
      return cEP_DEFAULT;
   }
   return (EndOfPathOption)ival;
}


WsfProxy::Bool WsfPM_Waypoint::SwitchOnApproach() const
{
   return GetAttributeT<WsfProxy::Bool>("switchOnApproach");
}

WsfPM_Waypoint::TurnDirection WsfPM_Waypoint::GetTurnDirection() const
{
   int ival = cTD_SHORTEST;
   GetValue().GetAttr("turnDirection").GetIntegerValue(ival);
   if (ival < cTD_SHORTEST || ival > cTD_RIGHT)
   {
      return cTD_SHORTEST;
   }
   return (TurnDirection)ival;
}

bool WsfPM_Waypoint::SetAltitudeReference(const WsfProxy::Int& aAltRef)
{
   return AssignAttribute("altitudeReference", aAltRef);
}

bool WsfPM_Waypoint::SetAltitudeReferenceValue(wsf::proxy::AltitudeReferenceEnum aVal) noexcept
{
   return AssignAttribute("altitudeReference", static_cast<int>(aVal));
}

WsfProxy::Speed WsfPM_Waypoint::ClimbRate() const
{
   return GetAttributeT<WsfProxy::Speed>("climbRate");
}

bool WsfPM_Waypoint::UseDefaultClimbRate() const
{
   return GetValue().GetAttr("climbRateDefault").GetBoolValue();
}

WsfProxy::Speed WsfPM_Waypoint::DiveRate() const
{
   return GetAttributeT<WsfProxy::Speed>("diveRate");
}

WsfProxy::Acceleration WsfPM_Waypoint::LinearAcceleration() const
{
   return GetAttributeT<WsfProxy::Acceleration>("linearAcceleration");
}
bool WsfPM_Waypoint::UseDefaultLinearAcceleration() const
{
   return GetValue().GetAttr("linearAccelerationDefault").GetBoolValue();
}

WsfProxy::Acceleration WsfPM_Waypoint::RadialAcceleration() const
{
   return GetAttributeT<WsfProxy::Acceleration>("radialAcceleration");
}
bool WsfPM_Waypoint::UseDefaultRadialAcceleration() const
{
   return GetValue().GetAttr("radialAccelerationDefault").GetBoolValue();
}
WsfProxy::Time WsfPM_Waypoint::PauseTime() const
{
   return GetAttributeT<WsfProxy::Time>("pauseTime");
}

WsfProxy::Angle WsfPM_Waypoint::Heading() const noexcept
{
   return GetAttributeT<WsfProxy::Angle>("heading");
}

WsfPProxyNode WsfPM_Waypoint::HeadingNode() const noexcept
{
   return *this + "heading";
}

WsfProxy::Length WsfPM_Waypoint::Distance() const
{
   return GetAttributeT<WsfProxy::Length>("distance");
}

WsfProxy::Time WsfPM_Waypoint::Time() const
{
   return GetAttributeT<WsfProxy::Time>("time");
}

WsfProxy::Angle WsfPM_Waypoint::BankAngleLimit() const
{
   return GetAttributeT<WsfProxy::Angle>("bankAngleLimit");
}

WsfProxy::Acceleration WsfPM_Waypoint::TurnG_Limit() const
{
   return GetAttributeT<WsfProxy::Acceleration>("turnG_Limit");
}

bool WsfPM_Waypoint::SetWaypointType(WaypointType aType)
{
   int wptTypeInt = -1;

   switch (aType)
   {
   case cPOSITION:
      wptTypeInt = 0;
      break;
   case cOFFSET:
      wptTypeInt = 1;
      break;
   case cTURN:
      wptTypeInt = 2;
      break;
   case cOTHER:
   default:
      wptTypeInt = -1;
   }
   return AssignAttribute("waypointType", wptTypeInt);
}

bool WsfPM_Waypoint::SetTurnDirection(TurnDirection aDirection)
{
   int ival = cTD_SHORTEST;
   switch (aDirection)
   {
   case cTD_SHORTEST:
      ival = 0;
      break;
   case cTD_LEFT:
      ival = 1;
      break;
   case cTD_RIGHT:
      ival = 2;
      break;
   }

   return AssignAttribute("turnDirection", ival);
}

bool WsfPM_Waypoint::SetGotoLabel(const std::string& aGoto)
{
   if (aGoto.empty())
   {
      return UnsetAttribute("gotoLabel");
   }
   else
   {
      return AssignAttribute("gotoLabel", aGoto);
   }
}

bool WsfPM_Waypoint::SetRadialAcceleration(WsfProxy::Acceleration aRadialAccel)
{
   return AssignAttribute("radialAcceleration", aRadialAccel);
}

bool WsfPM_Waypoint::SetEndOfPath(EndOfPathOption aEndOfPath)
{
   int ival = cEP_DEFAULT;
   switch (aEndOfPath)
   {
   case cEP_DEFAULT:
      ival = 0;
      break;
   case cEP_EXTRAPOLATE:
      ival = 1;
      break;
   case cEP_STOP:
      ival = 2;
      break;
   case cEP_REMOVE:
      ival = 3;
      break;
   }
   return AssignAttribute("endOfPath", ival);
}

bool WsfPM_Waypoint::SetSwitchOnApproach(WsfProxy::Bool aSwitchOnApproach)
{
   return AssignAttribute("switchOnApproach", aSwitchOnApproach);
}

std::string wsf::proxy::StringFromPosition(const WsfProxy::Position& aLatLon,
                                           const WsfProxy::Length&   aAltitude,
                                           const WsfProxy::Int&      aAltitudeRef) noexcept
{
   std::stringstream ss;

   ss << "position " << aLatLon.GetLatitude() << ' ' << aLatLon.GetLongitude() << ' ' << "altitude "
      << aAltitude.GetValue() << ' ';
   // Provide the correct string plus a trailing space
   auto altRefVal = static_cast<wsf::proxy::AltitudeReferenceEnum>(aAltitudeRef.GetValue());
   switch (altRefVal)
   {
   case wsf::proxy::AltitudeReferenceEnum::MSL:
      ss << "msl ";
      break;
   case wsf::proxy::AltitudeReferenceEnum::AGL:
      ss << "agl ";
      break;
   case wsf::proxy::AltitudeReferenceEnum::Default:
   default:
      ss << ' ';
      break;
   }

   return ss.str();
}

bool wsf::proxy::PositionFromString(const std::string&  aString,
                                    WsfProxy::Position& aLatLon,
                                    WsfProxy::Length&   aAltitude,
                                    WsfProxy::Int&      aAltitudeRef) noexcept
{
   std::string lla = aString;
   if (aString.length() > 9 && aString.substr(0, 9) == "position ")
   {
      lla = aString.substr(9);
   }
   UtStringUtil::TrimWhiteSpace(lla);
   UtLatPos          lat;
   UtLonPos          lon;
   std::stringstream ss(lla);
   if (ss >> lat >> lon)
   {
      aLatLon.SetLatitude(lat);
      aLatLon.SetLongitude(lon);
      std::string altitude;
      ss >> altitude;

      if (altitude == "altitude")
      {
         UtLengthValue alt;
         if (ss >> alt)
         {
            aAltitude.SetValue(alt);
            std::string aglStr;
            ss >> aglStr;
            wsf::proxy::AltitudeReferenceEnum altRef{wsf::proxy::AltitudeReferenceEnum::Default};
            if (aglStr == "msl")
            {
               altRef = wsf::proxy::AltitudeReferenceEnum::MSL;
            }
            else if (aglStr == "agl")
            {
               altRef = wsf::proxy::AltitudeReferenceEnum::AGL;
            }
            aAltitudeRef.SetValue(static_cast<int>(altRef));
         }
      }
      return true;
   }

   size_t delimPos = lla.find_first_of(',');
   if (delimPos == std::string::npos)
   {
      delimPos = lla.find_first_of(' ');
   }
   if (delimPos != std::string::npos)
   {
      std::string latStr = lla.substr(0, delimPos);
      std::string lonStr = lla.substr(delimPos + 1);
      if (ut::conversion::try_lexical_convert(latStr, lat) && ut::conversion::try_lexical_convert(lonStr, lon))
      {
         aLatLon.SetLatitude(lat);
         aLatLon.SetLongitude(lon);
         return true;
      }

      double dblLat, dblLon;
      if (ut::conversion::try_lexical_convert(latStr, dblLat) && ut::conversion::try_lexical_convert(lonStr, dblLon))
      {
         if (dblLon <= 180.0 && dblLon >= -180.0 && dblLat <= 90.0 && dblLat >= -90.0)
         {
            aLatLon.SetLatitude(dblLat);
            aLatLon.SetLongitude(dblLon);
            return true;
         }
      }
   }
   return false;
}
