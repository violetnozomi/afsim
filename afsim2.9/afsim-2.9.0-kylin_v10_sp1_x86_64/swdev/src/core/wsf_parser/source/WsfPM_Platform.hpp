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

#ifndef WSFPM_PLATFORM_HPP
#define WSFPM_PLATFORM_HPP

#include "wsf_parser_export.h"

#include <string>
#include <vector>

#include "WsfPM_Comm.hpp"
#include "WsfPM_Fuel.hpp"
#include "WsfPM_Mover.hpp"
#include "WsfPM_Processor.hpp"
#include "WsfPM_Router.hpp"
#include "WsfPM_Sensor.hpp"
#include "WsfPM_Zone.hpp"
#include "WsfPProxyBasicValues.hpp"
#include "WsfPProxyNode.hpp"

class WSF_PARSER_EXPORT WsfPM_Platform : public WsfPProxyNode
{
public:
   WsfPM_Platform();
   WsfPM_Platform(const WsfPProxyNode& aNode);
   virtual ~WsfPM_Platform() = default;

   static const char* CommanderSelfValue();

   std::string    Icon() const;
   std::string    Side() const;
   bool           Destructible() const;
   WsfProxy::Time CreationTime() const;

   //! Get the heading
   //! @return the heading as a WsfProxy::Angle
   WsfProxy::Angle Heading() const;
   //! Get the heading
   //! @return the heading as a WsfPProxyNode
   WsfPProxyNode HeadingNode() const noexcept;
   //! Get the initial heading
   //! @return the initial heading as a WsfProxy::Angle
   WsfProxy::Angle InitialHeading() const;
   void            SetHeading(const WsfProxy::Angle& aAngle);
   WsfProxy::Angle Pitch() const;
   void            SetPitch(const WsfProxy::Angle& aAngle);
   WsfProxy::Angle Roll() const;
   void            SetRoll(const WsfProxy::Angle& aAngle);

   //! Get the position
   //! @return the position as a WsfProxy::Position
   WsfProxy::Position Position() const;
   //! Get the position
   //! @return the position as a WsfPProxyNode
   WsfPProxyNode PositionNode() const;
   bool          SetPosition(const WsfProxy::Position& aPosition);

   WsfProxy::Length Altitude() const;
   WsfPProxyNode    AltitudeNode() const;
   WsfProxy::Int    AltitudeReference() const;
   WsfPProxyNode    AltitudeReferenceNode() const;
   //! Get the altitude reference property
   //! @retval AltitudeReferenceEnum::MSL if the property is unset
   //! @retval AltitudeReferenceEnum::MSL if the property is set as MSL
   //! @retval AltitudeReferenceEnum::AGL if the property is set as AGL
   //! @see AltitudeReferenceEnum
   wsf::proxy::AltitudeReferenceEnum AltitudeReferenceValue() const noexcept;
   bool                              SetAltitude(const WsfProxy::Length& aAlt);
   bool                              SetAltitudeReference(const WsfProxy::Int& aAltRef);
   bool                              SetAltitudeReferenceValue(wsf::proxy::AltitudeReferenceEnum aAltRef) noexcept;

   //! Get the effective altitude of the platform
   //! @retval "the first waypoint's altitude" if the platform has a route
   //! @retval "the platform's altitude" if the platform has no route
   WsfProxy::Length EffectiveAltitude() const;
   //! Get the effective altitude reference of the platform
   //! @retval "the first waypoint's altitude reference" if the platform has a route
   //! @retval "the platform's altitude reference" if the platform has no route
   wsf::proxy::AltitudeReferenceEnum EffectiveAGL() const noexcept;
   bool                              SetEffectiveAltitude(WsfProxy::Length aAltitude);
   bool                              SetEffectiveAGL(bool aIsAGL) noexcept;

   std::string InfraredSignature() const;
   std::string RadarSignature() const;
   std::string OpticalSignature() const;
   std::string AcousticSignature() const;

   WsfPM_Fuel            Fuel() const;
   WsfPM_Mover           Mover() const;
   WsfPM_CommMap         Comms() const;
   WsfPM_RouterMap       Routers() const;
   WsfPM_SensorMap       Sensors() const;
   WsfPM_ProcessorMap    Processors() const;
   virtual WsfPProxyNode FindPart(const std::string& aName) const;
   WsfPM_ObjectMap       Commanders() const { return *this + "commandChains"; }
   WsfPM_ZoneMap         Zone() const { return *this + "zone"; }
   WsfPM_ObjectMap       Categories() const { return *this + "categories"; }

   bool AddPart(const WsfPProxyNode& partTypeNode, const std::string& partName);
   void SetIcon(const std::string& aIcon);
   void SetSide(const std::string& aSide);
   void SetDestructible(bool destructible);

   void               LocationLLA(double& aLat, double& aLon, double& aAlt) const;
   void               InitialLocationLLA(double& aLat, double& aLon, double& aAlt) const;
   WsfProxy::Position InitialLocation() const;
   bool               SetInitialLocation(const WsfProxy::Position& aPos) const;

   void SetInitialLocationLLA(double aLat, double aLon, double aAlt);

   std::string              CommanderName(const std::string& aChainName) const;
   bool                     RemoveFromCommandChain(const std::string& aChainName) const;
   bool                     SetCommanderName(std::string aChainName, std::string aCommanderName) const;
   std::vector<std::string> CommandChainNames() const;
   std::vector<std::string> CommNetworkNames() const;

   bool Rename(const std::string& aNewName);

   // Handle platform renaming.  Update command chain
   void PlatformRenamed(const std::string& aOldName, const std::string& aNewName);

   WsfPM_MoverMap          MoverMap() const;
   WsfPM_FuelMap           FuelMap() const;
   virtual WsfPM_ObjectMap GetTypeObjectMap(const WsfPProxyStructType& aProxyStruct);

private:
   std::string    TrueCommNetworkName(const std::string& aNetworkName) const;
   WsfPM_Waypoint FirstWaypoint() const;
};

using WsfPM_PlatformMap = WsfPM_ObjectMapT<WsfPM_Platform>;

#endif
