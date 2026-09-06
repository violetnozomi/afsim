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

#ifndef WSFPM_ZONE_HPP
#define WSFPM_ZONE_HPP

#include "wsf_parser_export.h"

#include <map>
#include <set>
#include <string>
#include <type_traits>

#include "UtColor.hpp"
#include "UtOptional.hpp"
#include "UtVec2.hpp"
#include "WsfPM_ObjectList.hpp"
#include "WsfPM_ObjectMap.hpp"
#include "WsfPProxyNode.hpp"

class WSF_PARSER_EXPORT WsfPM_ZonePoint : public WsfPProxyNode
{
public:
   WsfPM_ZonePoint(const WsfPProxyNode& aNode)
      : WsfPProxyNode(aNode)
   {
   }
   WsfPM_ZonePoint() {}

   WsfProxy::Latitude  Latitude() const;
   WsfProxy::Longitude Longitude() const;
   WsfProxy::Length2   GetPoint() const;
   WsfProxy::String    MGRS() const;
   WsfProxy::Length    Range() const;
   WsfProxy::Angle     Bearing() const;

   double GetX() const;
   double GetY() const;
};

using WsfPM_ZonePointList = WsfPM_ObjectList<WsfPM_ZonePoint>;
class WsfPM_Platform;
class WsfPM_ZoneSet;

class WSF_PARSER_EXPORT WsfPM_Zone : public WsfPProxyNode
{
public:
   using Point = UtVec2d;
   enum ZoneType
   {
      cZONE_SET,
      cZONE_DEFINITION
   };
   WsfPM_Zone() {}
   WsfPM_Zone(const WsfPProxyNode& aNode)
      : WsfPProxyNode(aNode)
   {
   }

   bool IsDefinition() const
   {
      const WsfPProxyType* typePtr = GetValue().GetType();
      return (typePtr && typePtr->mTypeName == "ZoneDefinition");
   }
   bool IsZoneSet() const
   {
      const WsfPProxyType* typePtr = GetValue().GetType();
      return (typePtr && typePtr->mTypeName == "ZoneSet");
   }
   bool       Rename(std::string aNewName);
   WsfPM_Zone GetSourceZone() const;

   //! @brief Returns the name of the parent platform, or "Global Zones" if there is no parent platform.
   std::string ParentPlatform() const;

   //! @brief Returns the platform proxy node representing this zone's parent platform.
   WsfPM_Platform ParentPlatformNode() const;

   //! @brief Returns a list of zones that reference this zone.
   std::vector<WsfPM_Zone> GetReferencingZones() const;

   //! @brief Returns a list of zone sets that use this zone.
   std::vector<WsfPM_ZoneSet> GetUsingZoneSets() const;

   virtual ut::optional<UtColor> GetFillColor() const;
   virtual ut::optional<UtColor> GetLineColor() const;
};
class WSF_PARSER_EXPORT WsfPM_ZoneDefinition : public WsfPM_Zone
{
public:
   WsfPM_ZoneDefinition() {}
   WsfPM_ZoneDefinition(const WsfPProxyNode& aNode)
      : WsfPM_Zone(aNode)
   {
   }

   enum DefinitionType
   {
      cTYPE_NOT_SET = 0,
      cCIRCULAR     = 1,
      cELLIPTICAL,
      cPOLYGONAL,
      cSPHERICAL
   };

   //! @brief Returns the definition type of the zone: circular, elliptical, spherical, or polygonal
   DefinitionType GetDefinitionType() const;

   void SetDefinitionType(DefinitionType aType)
   {
      GetValue().GetAttr("type").SetIntegerValue(static_cast<int>(aType));
      NotifyAttributeChanged("type");
   }

   //! @brief Returns the value of the minimum altitude property. If unset, returns the minimum altitude inherited
   //! through the references tree if there is one.
   WsfProxy::Length MinimumAltitude() const;

   //! @brief Returns the value of the maximum altitude property. If unset, returns the maximum altitude inherited
   //! through the references tree if there is one.
   WsfProxy::Length MaximumAltitude() const;

   //! @brief Returns the value of the minimum radius property. If unset, returns the minimum radius inherited through
   //! the references tree if there is one.
   WsfProxy::Length MinimumRadius() const;

   //! @brief Returns the value of the maximum radius property. If unset, returns the maximum radius inherited through
   //! the references tree if there is one.
   WsfProxy::Length MaximumRadius() const;

   //! @brief Returns the value of the longitudal axis property. If unset, returns the longitudal axis inherited through
   //! the references tree if there is one.
   WsfProxy::Length LongitudinalAxis() const;

   //! @brief Returns the value of the lateral axis property. If unset, returns the lateral axis inherited through the
   //! references tree if there is one.
   WsfProxy::Length LateralAxis() const;

   //! @brief Returns the value of the start angle property. If unset, returns the start angle inherited through the
   //! references tree if there is one.
   WsfProxy::Angle StartAngle() const;

   //! @brief Returns the value of the stop angle property. If unset, returns the stop angle inherited through the
   //! references tree if there is one.
   WsfProxy::Angle StopAngle() const;


   //! @brief Returns whether the points on this polygonal zone are defined using lat/lon coordinates.
   bool UseLatLon() const;

   //! @brief Returns whether the points on this polygonal zone are defined using MGRS coordinates.
   bool UseMGRS() const;

   //! @brief Returns whether the points on this polygonal zone are defined using polar coordinates.
   bool UsePolar() const;

   //! @brief Returns whether the points on this polygonal zone are defined as an offset from a position.
   bool UseOffset() const;

   //! @brief Returns whether this zone is absolute -- that is, defined using absolute coordinates (lat/lon or MGRS) AND
   //! defined without a reference zone.
   bool IsAbsolute() const;

   //! Get the heading as a WsfProxy::Angle
   //! @retval "the zone heading" if set
   //! @retval "the platform heading" if unset
   WsfProxy::Angle Heading() const noexcept;

   //! Get the heading
   //! @return the heading as a WsfPProxyNode
   WsfPProxyNode HeadingNode() const noexcept;

   //! Get the position as a WsfProxy::Position
   //! @retval "the zone position" if set
   //! @retval "the platform position" if unset
   WsfProxy::Position Position() const noexcept;

   //! Get the position
   //! @return the position as a WsfPProxyNode
   WsfPProxyNode PositionNode() const noexcept;


   //! @brief Returns the value of the reference platform property.
   std::string ReferencePlatform() const;

   //! @brief Returns the value of the reference zone property.
   std::string ReferenceZone() const;

   //! @brief Returns the value of the negative property.
   bool Negative() const;

   using AuxDataMap = std::map<std::string, std::string>;
   AuxDataMap AuxData() const;

   //! @brief Returns the points that define this polygonal zone.
   std::vector<WsfPM_Zone::Point> Points() const;

   //! @brief Returns the point at the given index.
   //! If there is no point at this index, returns an empty instance.
   //! @param aIndex is the index of the point in question.
   WsfPM_Zone::Point GetPoint(size_t aIndex) const;

   //! @brief Returns the point proxy node at the given index.
   //! If there is no point at this index, returns a null zone point proxy node.
   //! @param aIndex is the index of the point in question.
   WsfPM_ZonePoint GetPointNode(size_t aIndex) const;

   //! @brief Returns the number of points on this polygonal zone.
   size_t PointCount() const;


   //! @brief Returns a list of zone point proxy nodes on this poygonal zone node.
   //! If none are defined for this zone, returns a list of inherited zone point proxy nodes.
   WsfPM_ZonePointList PointList() const;

   //! @brief Calculates the reference point for a relative zone defined using absolute coordinates.
   //! @param aX will be the latitude/x component of the point.
   //! @param aY will be the longitude/y component of the point.
   //! @param aIsLatLon will be true if the point is absolute.
   //! @returns true if this zone has a reference point, false otherwise.
   bool ReferencePoint(double& aX, double& aY, bool& aIsLatLon) const;

   //! @brief Calculates the reference point for a relative zone defined using absolute coordinates.
   //! @param aX will be the latitude/x component of the point.
   //! @param aY will be the longitude/y component of the point.
   //! @returns true if this zone has a reference point, false otherwise.
   bool ReferencePoint(double& aX, double& aY) const
   {
      bool _;
      return ReferencePoint(aX, aY, _);
   }

   //! @brief Calculates the reference point for a relative zone defined using absolute coordinates.
   //! @returns the point if this is a relative zone with absolute coordinates, and an empty optional otherwise.
   ut::optional<WsfPM_Zone::Point> ReferencePoint() const;

   //! @brief Returns the zone proxy node representing this zone's reference zone.
   WsfPM_ZoneDefinition ReferenceZoneNode() const;

   //! @brief Returns the platform proxy node representing this zone's anchor platform.
   WsfPM_Platform AnchorPlatformNode() const;

   //! @brief Returns the platform proxy node representing this zone's reference platform.
   WsfPM_Platform ReferencePlatformNode() const;

   //! @brief Returns the altitude property of this zone's anchor platform.
   WsfProxy::Length AnchorPlatformAltitude() const;

   bool IsReferencePlatformValid() const;

   ut::optional<UtColor> GetFillColor() const override;

   ut::optional<UtColor> GetLineColor() const override;

private:
   template<typename BASIC_VALUE>
   BASIC_VALUE GetZoneAttr(const std::string& aAttrName, std::set<std::string>& aRefStack) const;

   ut::optional<UtColor> GetColorAttr(const std::string& aAttrName, std::set<std::string>& aRefStack) const;

   WsfPM_ZoneDefinition PointsDefNode(std::set<std::string>& aRefStack) const;
};

template class WSF_PARSER_EXPORT WsfPM_ObjectList<WsfPM_ZoneDefinition>;
using WsfPM_ZoneDefinitionList = WsfPM_ObjectList<WsfPM_ZoneDefinition>;

class WSF_PARSER_EXPORT WsfPM_ZoneSet : public WsfPM_Zone
{
public:
   WsfPM_ZoneSet()
      : WsfPM_Zone()
   {
   }
   WsfPM_ZoneSet(const WsfPProxyNode& aNode)
      : WsfPM_Zone(aNode)
   {
   }

   // list of zone names...
   WsfPM_List               InclusionZones() const { return *this + "inclusionZones"; }
   WsfPM_List               ExclusionZones() const { return *this + "exclusionZones"; }
   WsfPM_ZoneDefinitionList EmbeddedInclusionZones() const { return *this + "embeddedInclusionZones"; }
   WsfPM_ZoneDefinitionList EmbeddedExclusionZones() const { return *this + "embeddedExclusionZones"; }
};

using WsfPM_ZoneMap = WsfPM_ObjectMapT<WsfPM_Zone>;

#endif
