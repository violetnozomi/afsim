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

#ifndef WSFPM_WAYPOINT_HPP
#define WSFPM_WAYPOINT_HPP

#include "WsfPM_ObjectList.hpp"
#include "WsfPProxyBasicValues.hpp"
#include "WsfPProxyNode.hpp"

class WSF_PARSER_EXPORT WsfPM_Waypoint : public WsfPProxyNode
{
public:
   WsfPM_Waypoint();
   WsfPM_Waypoint(const WsfPProxyNode& aNode);

   enum WaypointType
   {
      cPOSITION,
      cOFFSET,
      cTURN,
      cOTHER
   };

   enum EndOfPathOption
   {
      cEP_DEFAULT,
      cEP_EXTRAPOLATE,
      cEP_STOP,
      cEP_REMOVE
   };

   enum TurnDirection
   {
      cTD_SHORTEST,
      cTD_LEFT,
      cTD_RIGHT
   };

   WaypointType GetWaypointType() const;
   //! Get the position
   //! @return the position as a WsfProxy::Position
   WsfProxy::Position Position() const noexcept;
   //! Get the position
   //! @return the position as a WsfPProxyNode
   WsfPProxyNode     PositionNode() const noexcept;
   WsfProxy::Speed   Speed() const;
   WsfProxy::Speed   ClimbRate() const;
   bool              UseDefaultClimbRate() const;
   WsfProxy::Speed   DiveRate() const;
   WsfProxy::Length  Altitude() const;
   WsfProxy::Length2 Offset() const;
   WsfProxy::Angle   HeadingChange() const;
   //! Get the altitude reference property
   //! @retval 0 if the property is "unset"
   //! @retval 1 if the property is set as MSL
   //! @retval 2 if the property is set as AGL
   //! @note "unset":  Since the altitude reference always has a default, calls to IsUnset will always return true.
   WsfProxy::Int GetAltitudeReference() const;
   //! Get the altitude reference property
   //! @retval AltitudeReferenceEnum::Default if this waypoint is on a global route and the property is unset
   //! @retval AltitudeReferenceEnum::MSL if the property is set as MSL
   //! @retval AltitudeReferenceEnum::AGL if the property is set as AGL
   //! @retval WsfPM_Mover::DefaultAltitudeReference if this waypoint is on a local route and the property is unset
   //! @see AltitudeReferenceEnum
   wsf::proxy::AltitudeReferenceEnum AltitudeReferenceValue() const noexcept;
   EndOfPathOption                   EndOfPath() const;
   WsfProxy::Bool                    SwitchOnApproach() const;
   TurnDirection                     GetTurnDirection() const;
   WsfProxy::Acceleration            LinearAcceleration() const;
   bool                              UseDefaultLinearAcceleration() const;
   WsfProxy::Acceleration            RadialAcceleration() const;
   bool                              UseDefaultRadialAcceleration() const;
   WsfProxy::Angle                   BankAngleLimit() const;
   WsfProxy::Acceleration            TurnG_Limit() const;
   WsfProxy::Time                    PauseTime() const;
   WsfProxy::Length                  Distance() const;
   WsfProxy::Time                    Time() const;
   WsfProxy::Time*                   TimeToPoint() const { return GetValue().GetAttr("timeToPoint"); }
   //! Get the heading
   //! @return the initial heading as a WsfProxy::Angle
   WsfProxy::Angle Heading() const noexcept;
   //! Get the heading
   //! @return the initial heading as a WsfPProxyNode
   WsfPProxyNode HeadingNode() const noexcept;
   std::string   ExecuteScript() const { return GetValue().GetAttr("executeScript").ValueToString(); }
   std::string   GotoLabel() const { return GetValue().GetAttr("gotoLabel").ValueToString(); }
   std::string   Label() const { return GetValue().GetAttr("label").ValueToString(); }

   bool SetWaypointType(WaypointType aType);
   bool SetAltitudeReference(const WsfProxy::Int& aAltRef);
   bool SetAltitudeReferenceValue(wsf::proxy::AltitudeReferenceEnum aVal) noexcept;
   bool SetAltitude(WsfProxy::Length aAltitude);
   bool SetPosition(WsfProxy::Position aPosition);
   bool SetOffset(WsfProxy::Length2 aOffset);
   bool SetTurnDirection(TurnDirection aDirection);
   bool SetGotoLabel(const std::string& aGoto);
   bool SetRadialAcceleration(WsfProxy::Acceleration aRadialAccel);
   bool SetEndOfPath(EndOfPathOption aEndOfPath);
   bool SetSwitchOnApproach(WsfProxy::Bool aSwitchOnApproach);
};

using WsfPM_WaypointList = WsfPM_ObjectList<WsfPM_Waypoint>;

namespace wsf
{
namespace proxy
{

//! Converts a WsfProxy::Position (for latitude and longitude), WsfProxy::Length (for altitude), and WsfProxy::Int (for
//! altitude reference) to a std::string
//! @param aLatLon is the latitude and longitude
//! @param aAltitude is the altitude
//! @param aAltitudeRef is the altitude reference
//! @return a string representing the position, altitude, and altitude reference
std::string WSF_PARSER_EXPORT StringFromPosition(const WsfProxy::Position& aLatLon,
                                                 const WsfProxy::Length&   aAltitude,
                                                 const WsfProxy::Int&      aAltitudeRef) noexcept;

//! Converts a std::string to a WsfProxy::Position (for latitude and longitude), WsfProxy::Length (for altitude), and
//! WsfProxy::Int (for altitude reference)
//! @param aString is the string representation of the position
//! @param aLatLon is the latitude and longitude
//! @param aAltitude is the altitude
//! @param aAltitudeRef is the altitude reference
//! @return whether or not the conversion succeeded
//! @note This will edit aAltitude and aAltitudeRef if the string contains a valid altitude and starts with a valid
//! position.
bool WSF_PARSER_EXPORT PositionFromString(const std::string&  aString,
                                          WsfProxy::Position& aLatLon,
                                          WsfProxy::Length&   aAltitude,
                                          WsfProxy::Int&      aAltitudeRef) noexcept;

} // namespace proxy
} // namespace wsf
#endif
