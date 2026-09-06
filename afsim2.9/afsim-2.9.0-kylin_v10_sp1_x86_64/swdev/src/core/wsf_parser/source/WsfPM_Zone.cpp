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

#include "WsfPM_Zone.hpp"

#include "ColorProxyUtil.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "WsfPM_Platform.hpp"
#include "WsfPM_Root.hpp"
#include "WsfPProxyList.hpp"

namespace
{
bool ZoneSetUsesZone(const WsfPM_ZoneSet& aZoneSetNode, const std::string& aUsedZoneName)
{
   WsfPM_List inclusionZones = aZoneSetNode.InclusionZones();
   for (size_t i = 0; i < inclusionZones.size(); ++i)
   {
      if (aUsedZoneName == inclusionZones[i].GetValue().ValueToString())
      {
         return true;
      }
   }

   WsfPM_List exclusionZones = aZoneSetNode.ExclusionZones();
   for (size_t i = 0; i < exclusionZones.size(); ++i)
   {
      if (aUsedZoneName == exclusionZones[i].GetValue().ValueToString())
      {
         return true;
      }
   }

   return false;
}
} // namespace

WsfProxy::Latitude WsfPM_ZonePoint::Latitude() const
{
   return GetAttributeT<WsfProxy::Latitude>("latitude");
}

WsfProxy::Longitude WsfPM_ZonePoint::Longitude() const
{
   return GetAttributeT<WsfProxy::Longitude>("longitude");
}

WsfProxy::Length2 WsfPM_ZonePoint::GetPoint() const
{
   return GetAttributeT<WsfProxy::Length2>("point");
}

WsfProxy::String WsfPM_ZonePoint::MGRS() const
{
   return GetAttributeT<WsfProxy::String>("mgrs");
}

WsfProxy::Length WsfPM_ZonePoint::Range() const
{
   return GetAttributeT<WsfProxy::Length>("range");
}

WsfProxy::Angle WsfPM_ZonePoint::Bearing() const
{
   return GetAttributeT<WsfProxy::Angle>("bearing");
}

double WsfPM_ZonePoint::GetX() const
{
   WsfProxy::Length2 pt = GetPoint();
   if (!pt.IsUnset())
   {
      return pt.mX;
   }
   return static_cast<double>(Latitude().GetValue());
}

double WsfPM_ZonePoint::GetY() const
{
   WsfProxy::Length2 pt = GetPoint();
   if (!pt.IsUnset())
   {
      return pt.mY;
   }
   return static_cast<double>(Longitude().GetValue());
}

WsfPM_ZoneDefinition::DefinitionType WsfPM_ZoneDefinition::GetDefinitionType() const
{
   std::set<std::string> refStack;
   auto                  dt = static_cast<DefinitionType>(GetZoneAttr<WsfProxy::Int>("type", refStack).GetValue());
   if (dt < cTYPE_NOT_SET || dt > cSPHERICAL)
   {
      dt = cTYPE_NOT_SET;
   }
   return dt;
}

WsfPM_ZonePoint WsfPM_ZoneDefinition::GetPointNode(size_t aIndex) const
{
   WsfPM_ZonePointList points = PointList();
   if (points && aIndex < PointCount())
   {
      return points[aIndex];
   }
   return WsfPM_ZonePoint{};
}

ut::optional<UtColor> WsfPM_Zone::GetFillColor() const
{
   if (IsDefinition())
   {
      const WsfPM_ZoneDefinition zoneDefNode{*this};
      return zoneDefNode.GetFillColor();
   }

   auto color = ColorProxyUtil::GetColor(*this + "fillColor");
   if (!color.has_value())
   {
      color = ColorProxyUtil::GetColor(GetSourceZone() + "fillColor");
   }
   return color;
}

ut::optional<UtColor> WsfPM_Zone::GetLineColor() const
{
   if (IsDefinition())
   {
      const WsfPM_ZoneDefinition zoneDefNode{*this};
      return zoneDefNode.GetLineColor();
   }

   auto color = ColorProxyUtil::GetColor(*this + "lineColor");
   if (!color.has_value())
   {
      color = ColorProxyUtil::GetColor(GetSourceZone() + "lineColor");
   }
   return color;
}

WsfPM_ZoneDefinition::AuxDataMap WsfPM_ZoneDefinition::AuxData() const
{
   AuxDataMap          auxData;
   WsfPProxyObjectMap* mapPtr;
   if (GetAttrName() == "vals")
   {
      mapPtr = GetValue().GetObjectMap();
   }
   else
   {
      mapPtr = GetValue().GetAttr("auxData").GetAttr("vals").GetObjectMap();
   }
   if (mapPtr)
   {
      const auto& auxValues = mapPtr->GetValues();
      for (const auto& value : auxValues)
      {
         auxData.emplace(value.first, value.second.ValueToString());
      }
   }
   return auxData;
}

std::vector<WsfPM_Zone::Point> WsfPM_ZoneDefinition::Points() const
{
   std::vector<Point>  pointVec;
   WsfPM_ZonePointList pointNodes = PointList();
   size_t              numPoints  = pointNodes.size();

   if (pointNodes.IsValid() && numPoints > 0)
   {
      WsfPM_ZonePoint     pointNode   = pointNodes[0];
      bool                useLatLon   = !pointNode.Latitude().IsUnset();
      bool                useMGRS     = !pointNode.MGRS().IsUnset();
      bool                usePolar    = !pointNode.Bearing().IsUnset();
      bool                hasRefZone  = !ReferenceZone().empty();
      ut::optional<Point> optAnchorPt = ReferencePoint();

      for (size_t i = 0; i < numPoints; i++)
      {
         pointNode = pointNodes[i];
         if (pointNode.IsNull())
         {
            continue;
         }

         Point newPoint;
         if (useLatLon)
         {
            double lat = pointNode.Latitude().GetValue();
            double lon = pointNode.Longitude().GetValue();

            if (hasRefZone && optAnchorPt)
            {
               UtEntity pointEntity;
               pointEntity.SetLocationLLA(lat, lon, 0.0);

               UtEntity refEntity;
               Point    anchorPt = optAnchorPt.value();
               refEntity.SetLocationLLA(anchorPt[0], anchorPt[1], 0);
               refEntity.SetOrientationNED(Heading().GetValue(), 0.0, 0.0);

               double offsetECS[3];
               refEntity.GetRelativeLocationNED(&pointEntity, offsetECS);

               newPoint[0] = offsetECS[0];
               newPoint[1] = offsetECS[1];
            }
            else
            {
               newPoint[0] = lat;
               newPoint[1] = lon;
            }
         }
         else if (useMGRS)
         {
            std::string mgrs = pointNode.MGRS().GetValue();
            double      lat, lon;
            UtEllipsoidalEarth::ConvertMGRSToLL(mgrs, lat, lon);

            if (hasRefZone && optAnchorPt)
            {
               UtEntity pointEntity;
               pointEntity.SetLocationLLA(lat, lon, 0.0);

               UtEntity refEntity;
               Point    anchorPt = optAnchorPt.value();
               refEntity.SetLocationLLA(anchorPt[0], anchorPt[1], 0);
               refEntity.SetOrientationNED(Heading().GetValue(), 0.0, 0.0);

               double offsetECS[3];
               refEntity.GetRelativeLocationNED(&pointEntity, offsetECS);

               newPoint[0] = offsetECS[0];
               newPoint[1] = offsetECS[1];
            }
            else
            {
               newPoint[0] = lat;
               newPoint[1] = lon;
            }
         }
         else if (usePolar)
         {
            double range   = pointNode.Range().GetValue();
            double bearing = pointNode.Bearing().GetValue();
            newPoint[0]    = range * std::cos(bearing);
            newPoint[1]    = range * std::sin(bearing);
         }
         else // use offset
         {
            newPoint[0] = pointNode.GetX();
            newPoint[1] = pointNode.GetY();
         }

         pointVec.push_back(newPoint);
      }
   }

   return pointVec;
}

WsfPM_Zone::Point WsfPM_ZoneDefinition::GetPoint(size_t aIndex) const
{
   WsfPM_ZonePointList pointNodes = PointList();

   if (pointNodes.IsValid() && aIndex < pointNodes.size())
   {
      WsfPM_ZonePoint     pointNode   = pointNodes[aIndex];
      bool                useLatLon   = !pointNode.Latitude().IsUnset();
      bool                useMGRS     = !pointNode.MGRS().IsUnset();
      bool                usePolar    = !pointNode.Bearing().IsUnset();
      bool                hasRefZone  = !ReferenceZone().empty();
      ut::optional<Point> optAnchorPt = ReferencePoint();

      Point newPoint;
      if (useLatLon)
      {
         double lat = pointNode.Latitude().GetValue();
         double lon = pointNode.Longitude().GetValue();

         if (hasRefZone && optAnchorPt)
         {
            UtEntity pointEntity;
            pointEntity.SetLocationLLA(lat, lon, 0.0);

            UtEntity refEntity;
            Point    anchorPt = optAnchorPt.value();
            refEntity.SetLocationLLA(anchorPt[0], anchorPt[1], 0);
            refEntity.SetOrientationNED(Heading().GetValue(), 0.0, 0.0);

            double offsetECS[3];
            refEntity.GetRelativeLocationNED(&pointEntity, offsetECS);

            newPoint[0] = offsetECS[0];
            newPoint[1] = offsetECS[1];
         }
         else
         {
            newPoint[0] = lat;
            newPoint[1] = lon;
         }
      }
      else if (useMGRS)
      {
         std::string mgrs = pointNode.MGRS().GetValue();
         double      lat, lon;
         UtEllipsoidalEarth::ConvertMGRSToLL(mgrs, lat, lon);

         if (hasRefZone && optAnchorPt)
         {
            UtEntity pointEntity;
            pointEntity.SetLocationLLA(lat, lon, 0.0);

            UtEntity refEntity;
            Point    anchorPt = optAnchorPt.value();
            refEntity.SetLocationLLA(anchorPt[0], anchorPt[1], 0);
            refEntity.SetOrientationNED(Heading().GetValue(), 0.0, 0.0);

            double offsetECS[3];
            refEntity.GetRelativeLocationNED(&pointEntity, offsetECS);

            newPoint[0] = offsetECS[0];
            newPoint[1] = offsetECS[1];
         }
         else
         {
            newPoint[0] = lat;
            newPoint[1] = lon;
         }
      }
      else if (usePolar)
      {
         double range   = pointNode.Range().GetValue();
         double bearing = pointNode.Bearing().GetValue();
         newPoint[0]    = range * std::cos(bearing);
         newPoint[1]    = range * std::sin(bearing);
      }
      else // use offset
      {
         newPoint[0] = pointNode.GetX();
         newPoint[1] = pointNode.GetY();
      }

      return newPoint;
   }

   return Point{};
}


bool WsfPM_ZoneDefinition::UseLatLon() const
{
   std::set<std::string> refStack;
   WsfPM_ZoneDefinition  pointsDefNode = PointsDefNode(refStack);
   return pointsDefNode.GetValue().GetAttr("useLatLon").GetBoolValue();
}

bool WsfPM_ZoneDefinition::UseMGRS() const
{
   std::set<std::string> refStack;
   WsfPM_ZoneDefinition  pointsDefNode = PointsDefNode(refStack);
   return pointsDefNode.GetValue().GetAttr("useMGRS").GetBoolValue();
}

bool WsfPM_ZoneDefinition::UsePolar() const
{
   std::set<std::string> refStack;
   WsfPM_ZoneDefinition  pointsDefNode = PointsDefNode(refStack);
   return pointsDefNode.GetValue().GetAttr("usePolar").GetBoolValue();
}

bool WsfPM_ZoneDefinition::UseOffset() const
{
   std::set<std::string> refStack;
   WsfPM_ZoneDefinition  pointsDefNode = PointsDefNode(refStack);
   bool                  useLatLon     = pointsDefNode.GetValue().GetAttr("useLatLon").GetBoolValue();
   bool                  useMGRS       = pointsDefNode.GetValue().GetAttr("useMGRS").GetBoolValue();
   bool                  usePolar      = pointsDefNode.GetValue().GetAttr("usePolar").GetBoolValue();

   return !(useLatLon || useMGRS || usePolar);
}

bool WsfPM_ZoneDefinition::IsAbsolute() const
{
   std::set<std::string> refStack;
   WsfPM_ZoneDefinition  pointsDefNode = PointsDefNode(refStack);
   bool                  useLatLon     = pointsDefNode.GetValue().GetAttr("useLatLon").GetBoolValue();
   bool                  useMGRS       = pointsDefNode.GetValue().GetAttr("useMGRS").GetBoolValue();
   bool                  hasRefZone    = !ReferenceZone().empty();

   return (useLatLon || useMGRS) && !hasRefZone;
}

WsfProxy::Angle WsfPM_ZoneDefinition::Heading() const noexcept
{
   // try parent platform
   WsfPM_Platform parentPlatNode = ParentPlatformNode();
   if (parentPlatNode.IsValid())
   {
      auto position = GetAttributeT<WsfProxy::Position>("position");
      if (position.IsSet())
      {
         std::set<std::string> refStack;
         return GetZoneAttr<WsfProxy::Angle>("heading", refStack);
      }
      else
      {
         auto heading = GetAttributeT<WsfProxy::Angle>("heading");
         if (heading.IsUnset())
         {
            heading = parentPlatNode.InitialHeading();
         }
         return heading;
      }
   }

   // try reference platform
   WsfPM_Platform refPlatNode = ReferencePlatformNode();
   if (refPlatNode.IsValid())
   {
      WsfPProxyNode positionNode = *this + "position";
      WsfPProxyNode refPlatDecl  = *this + "referencePlatform";
      // if reference platform declared before position
      if (positionNode.IsValid() &&
          refPlatDecl.GetLastDocumentRange().GetPosition() < positionNode.GetLastDocumentRange().GetPosition())
      {
         std::set<std::string> refStack;
         return GetZoneAttr<WsfProxy::Angle>("heading", refStack);
      }
      else
      {
         WsfPProxyNode headingNode = *this + "heading";
         // if heading is set on this zone and reference platform is declared before heading
         if (headingNode.IsValid() &&
             refPlatDecl.GetLastDocumentRange().GetPosition() < headingNode.GetLastDocumentRange().GetPosition())
         {
            // add platform heading to zone heading?
            std::set<std::string> refStack;
            return GetZoneAttr<WsfProxy::Angle>("heading", refStack);
         }
         else
         {
            return refPlatNode.InitialHeading();
         }
      }
   }

   // inherit through reference chain
   std::set<std::string> refStack;
   return GetZoneAttr<WsfProxy::Angle>("heading", refStack);
}

WsfPProxyNode WsfPM_ZoneDefinition::HeadingNode() const noexcept
{
   return *this + "heading";
}

WsfProxy::Position WsfPM_ZoneDefinition::Position() const noexcept
{
   // try parent platform
   WsfPM_Platform parentPlatNode = ParentPlatformNode();
   if (parentPlatNode.IsValid())
   {
      auto position = GetAttributeT<WsfProxy::Position>("position");
      if (position.IsSet())
      {
         return position;
      }
      else
      {
         return parentPlatNode.InitialLocation();
      }
   }

   // try reference platform
   WsfPM_Platform refPlatNode = ReferencePlatformNode();
   if (refPlatNode.IsValid())
   {
      WsfPProxyNode positionNode = *this + "position";
      WsfPProxyNode refPlatDecl  = *this + "referencePlatform";
      // if position is declared on this zone and reference platform is declared before position
      if (positionNode.IsValid() &&
          refPlatDecl.GetLastDocumentRange().GetPosition() < positionNode.GetLastDocumentRange().GetPosition())
      {
         return GetAttributeT<WsfProxy::Position>("position");
      }
      else
      {
         return refPlatNode.InitialLocation();
      }
   }

   // try this zone
   return GetAttributeT<WsfProxy::Position>("position");
}

WsfPProxyNode WsfPM_ZoneDefinition::PositionNode() const noexcept
{
   return *this + "position";
}

std::string WsfPM_ZoneDefinition::ReferencePlatform() const
{
   return GetValue().GetAttr("referencePlatform").ValueToString();
}

std::string WsfPM_ZoneDefinition::ReferenceZone() const
{
   return GetValue().GetAttr("referenceZone").ValueToString();
}

bool WsfPM_ZoneDefinition::Negative() const
{
   return GetValue().GetAttr("negative").GetBoolValue();
}

bool WsfPM_ZoneDefinition::ReferencePoint(double& aX, double& aY, bool& aIsLatLon) const
{
   ut::optional<WsfPM_Zone::Point> optPt = ReferencePoint();
   if (optPt.has_value())
   {
      WsfPM_Zone::Point pt = optPt.value();
      aX                   = pt[0];
      aY                   = pt[1];
      aIsLatLon            = true;
      return true;
   }

   return false;
}

ut::optional<WsfPM_Zone::Point> WsfPM_ZoneDefinition::ReferencePoint() const
{
   std::set<std::string> refStack;
   WsfPM_ZoneDefinition  pointsDefNode = PointsDefNode(refStack);

   // try inheriting position
   WsfProxy::Position pos = pointsDefNode.Position();
   if (!pos.IsUnset())
   {
      return Point{pos.GetLatitude(), pos.GetLongitude()};
   }

   // use first point as reference point, try lat/lon then mgrs
   WsfPM_ZonePointList zonePointNodes{pointsDefNode + "points"};
   if (zonePointNodes.IsValid() && zonePointNodes.size() > 0)
   {
      WsfPM_ZonePoint     ptNode = zonePointNodes[0];
      WsfProxy::Latitude  lat    = ptNode.Latitude();
      WsfProxy::Longitude lon    = ptNode.Longitude();

      if (!lat.IsUnset() && !lon.IsUnset())
      {
         return Point{lat.GetValue(), lon.GetValue()};
      }

      std::string mgrs = ptNode.MGRS().GetValue();
      if (!mgrs.empty())
      {
         double lat, lon;
         UtEllipsoidalEarth::ConvertMGRSToLL(mgrs, lat, lon);
         return Point{lat, lon};
      }
   }

   return ut::optional<Point>{};
}

WsfPM_ZoneDefinition WsfPM_ZoneDefinition::ReferenceZoneNode() const
{
   std::string refZoneName = ReferenceZone();
   if (refZoneName.empty())
   {
      return WsfPM_ZoneDefinition{};
   }

   WsfPM_Root rootNode{Root()};
   WsfPM_Zone refZoneNode;

   // try looking on parent platform
   std::string parentPlatName = ParentPlatform();
   if (!parentPlatName.empty())
   {
      WsfPM_Platform parentPlatNode = rootNode.platforms().Find(parentPlatName);
      if (parentPlatNode.IsValid())
      {
         refZoneNode = parentPlatNode.Zone().Find(refZoneName);
      }
   }

   // try looking for global zones
   if (refZoneNode.IsNull())
   {
      refZoneNode = rootNode.zones().Find(refZoneName);
   }

   return WsfPM_ZoneDefinition{refZoneNode}; // null cascades up
}

WsfPM_Platform WsfPM_ZoneDefinition::ReferencePlatformNode() const
{
   std::string refPlatName = ReferencePlatform();

   if (!refPlatName.empty())
   {
      WsfPM_Root     rootNode{Root()};
      WsfPM_Platform refPlatNode = rootNode.platforms().Find(refPlatName);
      if (refPlatNode.IsValid())
      {
         return refPlatNode;
      }
   }

   return WsfPM_Platform{};
}

WsfPM_Platform WsfPM_ZoneDefinition::AnchorPlatformNode() const
{
   WsfPM_Root rootNode{Root()};

   WsfPM_Platform anchorPlatNode = ReferencePlatformNode();
   if (anchorPlatNode.IsNull())
   {
      anchorPlatNode = ParentPlatformNode();
   }

   return anchorPlatNode; // null cascades
}

WsfProxy::Length WsfPM_ZoneDefinition::AnchorPlatformAltitude() const
{
   WsfPM_Platform anchorPlatNode = AnchorPlatformNode();

   if (anchorPlatNode.IsValid() && anchorPlatNode.Mover().IsValid() && anchorPlatNode.Mover().InitialRoute().IsValid())
   {
      WsfPM_Route    routeNode     = anchorPlatNode.Mover().InitialRoute();
      WsfPM_Waypoint firstWaypoint = routeNode.GetWaypoint(0);
      return firstWaypoint.Altitude();
   }
   else
   {
      return anchorPlatNode.Altitude();
   }
}

bool WsfPM_ZoneDefinition::IsReferencePlatformValid() const
{
   if (GetAttributeT<WsfProxy::String>("referencePlatform").IsUnset())
   {
      return true;
   }
   auto                        refPlat = ReferencePlatform();
   WsfPM_Root                  root(Root());
   std::vector<WsfPM_Platform> allPlatforms;
   root.platforms().GetValuesT(allPlatforms);
   for (size_t i = 0; i < allPlatforms.size(); ++i)
   {
      if (allPlatforms[i].GetName() == refPlat)
      {
         return true;
      }
   }
   return false;
}

size_t WsfPM_ZoneDefinition::PointCount() const
{
   WsfPM_ZonePointList pointNodes = PointList();
   if (pointNodes.IsValid())
   {
      return pointNodes.size();
   }
   return 0;
}

WsfProxy::Length WsfPM_ZoneDefinition::MinimumAltitude() const
{
   std::set<std::string> refStack;
   return GetZoneAttr<WsfProxy::Length>("minimumAltitude", refStack);
}

WsfProxy::Length WsfPM_ZoneDefinition::MaximumAltitude() const
{
   std::set<std::string> refStack;
   return GetZoneAttr<WsfProxy::Length>("maximumAltitude", refStack);
}

WsfProxy::Length WsfPM_ZoneDefinition::MinimumRadius() const
{
   std::set<std::string> refStack;
   return GetZoneAttr<WsfProxy::Length>("minimumRadius", refStack);
}

WsfProxy::Length WsfPM_ZoneDefinition::MaximumRadius() const
{
   std::set<std::string> refStack;
   return GetZoneAttr<WsfProxy::Length>("maximumRadius", refStack);
}

WsfProxy::Length WsfPM_ZoneDefinition::LongitudinalAxis() const
{
   std::set<std::string> refStack;
   return GetZoneAttr<WsfProxy::Length>("longitudinalAxis", refStack);
}

WsfProxy::Length WsfPM_ZoneDefinition::LateralAxis() const
{
   std::set<std::string> refStack;
   return GetZoneAttr<WsfProxy::Length>("lateralAxis", refStack);
}

WsfProxy::Angle WsfPM_ZoneDefinition::StartAngle() const
{
   std::set<std::string> refStack;
   return GetZoneAttr<WsfProxy::Angle>("startAngle", refStack);
}

WsfProxy::Angle WsfPM_ZoneDefinition::StopAngle() const
{
   std::set<std::string> refStack;
   return GetZoneAttr<WsfProxy::Angle>("stopAngle", refStack);
}

WsfPM_ZonePointList WsfPM_ZoneDefinition::PointList() const
{
   std::set<std::string> refStack;
   WsfPM_ZoneDefinition  pointsDefNode = PointsDefNode(refStack);
   return WsfPM_ZonePointList{pointsDefNode + "points"};
}

template<typename BASIC_VALUE>
BASIC_VALUE WsfPM_ZoneDefinition::GetZoneAttr(const std::string& aAttrName, std::set<std::string>& aRefStack) const
{
   static_assert(
      std::is_base_of<WsfProxy::BasicValue, BASIC_VALUE>::value,
      "WsfPM_ZoneDefinition::GetZoneAttr requires accessing a value of a type inheriting from WsfProxy::BasicValue.");

   auto myAttr = GetAttributeT<BASIC_VALUE>(aAttrName);
   if (!myAttr.IsUnset())
   {
      return myAttr;
   }

   std::string          refZoneName = ReferenceZone();
   WsfPM_ZoneDefinition refZoneNode = ReferenceZoneNode();
   if (refZoneNode.IsNull() || aRefStack.count(refZoneName))
   {
      // return the unset attr if the ref zone doesn't exist
      return myAttr;
   }

   // recursive call up the reference chain to see if this is inherited
   aRefStack.insert(refZoneName);
   return refZoneNode.GetZoneAttr<BASIC_VALUE>(aAttrName, aRefStack);
}

WsfPM_ZoneDefinition WsfPM_ZoneDefinition::PointsDefNode(std::set<std::string>& aRefStack) const
{
   std::string         zoneName = GetName();
   WsfPM_ZonePointList pointNodeList{*this + "points"};

   if (pointNodeList.IsValid() && pointNodeList.size() > 0)
   {
      return *this;
   }

   std::string          refZoneName = ReferenceZone();
   WsfPM_ZoneDefinition refZoneNode = ReferenceZoneNode();
   if (refZoneNode.IsNull() || aRefStack.count(refZoneName))
   {
      return *this;
   }

   aRefStack.insert(refZoneName);
   return refZoneNode.PointsDefNode(aRefStack);
}

ut::optional<UtColor> WsfPM_ZoneDefinition::GetFillColor() const
{
   std::set<std::string> refStack;
   return GetColorAttr("fillColor", refStack);
}

ut::optional<UtColor> WsfPM_ZoneDefinition::GetLineColor() const
{
   std::set<std::string> refStack;
   return GetColorAttr("lineColor", refStack);
}

ut::optional<UtColor> WsfPM_ZoneDefinition::GetColorAttr(const std::string& aAttrName, std::set<std::string>& aRefStack) const
{
   // try this zone
   auto color = ColorProxyUtil::GetColor(*this + aAttrName);
   if (color.has_value())
   {
      return color;
   }

   // try zone reference chain
   WsfPM_ZoneDefinition refZoneNode = ReferenceZoneNode();
   if (refZoneNode.IsValid())
   {
      std::string refZoneName = ReferenceZone();
      if (aRefStack.count(refZoneName) == 0)
      {
         aRefStack.insert(refZoneName);
         return refZoneNode.GetColorAttr(aAttrName, aRefStack);
      }
   }

   // try source zone
   return ut::optional<UtColor>{};
}

WsfPM_Zone WsfPM_Zone::GetSourceZone() const
{
   WsfPM_Zone src = *this;
   for (;;)
   {
      WsfPProxyNode source = src.GetInherited();
      if (!source || source.IsOfBasicType())
      {
         break;
      }
      src = source;
   }
   return src;
}

std::string WsfPM_Zone::ParentPlatform() const
{
   if (GetPath().size() > 2)
   {
      std::string parentPlatName = GetPath()[1].ToString();
      WsfPM_Root  rootNode{Root()};
      if (rootNode.platforms().Exists(parentPlatName))
      {
         return parentPlatName;
      }
   }

   return "Global Zones";
}

WsfPM_Platform WsfPM_Zone::ParentPlatformNode() const
{
   std::string parentPlatName = ParentPlatform();

   if (!parentPlatName.empty())
   {
      WsfPM_Root     rootNode{Root()};
      WsfPM_Platform parentPlatNode = rootNode.platforms().Find(parentPlatName);
      if (parentPlatNode.IsValid())
      {
         return parentPlatNode;
      }
   }

   return WsfPM_Platform{};
}

std::vector<WsfPM_Zone> WsfPM_Zone::GetReferencingZones() const
{
   std::vector<WsfPM_Zone> referencingZones;
   std::string             zoneName = GetName();
   if (zoneName.empty())
   {
      return referencingZones;
   }

   WsfPM_Root rootNode{Root()};
   // find global zones
   std::vector<WsfPM_Zone> globalZones;
   rootNode.zones().GetAllValuesT(globalZones);
   for (const WsfPM_Zone& zoneNode : globalZones)
   {
      WsfPM_ZoneDefinition zoneDefNode{zoneNode};
      if (zoneDefNode.IsValid() && zoneDefNode.ReferenceZone() == zoneName)
      {
         referencingZones.push_back(zoneNode);
      }
   }

   // find local zones (scoped to platform)
   std::vector<WsfPM_Platform> platforms;
   rootNode.platforms().GetAllValuesT(platforms);
   for (const WsfPM_Platform& platformNode : platforms)
   {
      std::vector<WsfPM_Zone> localZones;
      platformNode.Zone().GetAllValuesT(localZones);
      for (const WsfPM_Zone& zoneNode : localZones)
      {
         WsfPM_ZoneDefinition zoneDefNode{zoneNode};
         if (zoneDefNode.IsValid() && zoneDefNode.ReferenceZone() == zoneName)
         {
            referencingZones.push_back(zoneNode);
         }
      }
   }

   return referencingZones;
}

std::vector<WsfPM_ZoneSet> WsfPM_Zone::GetUsingZoneSets() const
{
   std::vector<WsfPM_ZoneSet> usingZoneSets;
   std::string                myName = GetName();

   WsfPM_Root              rootNode{Root()};
   std::string             parentPlatName = ParentPlatform();
   std::vector<WsfPM_Zone> zoneNodes;
   // if this is a global zone, only global zone sets may use
   if (parentPlatName == "Global Zones")
   {
      rootNode.zones().GetAllValuesT(zoneNodes);
   }
   // if this is a platform-defiend zone, only zones on this platform may use
   else
   {
      WsfPM_Platform parentPlatNode = ParentPlatformNode();
      if (parentPlatNode.IsValid())
      {
         parentPlatNode.Zone().GetAllValuesT(zoneNodes);
      }
   }

   for (const WsfPM_Zone& zoneNode : zoneNodes)
   {
      if (zoneNode.IsZoneSet())
      {
         WsfPM_ZoneSet zoneSetNode{zoneNode};
         if (ZoneSetUsesZone(zoneSetNode, myName))
         {
            usingZoneSets.push_back(zoneSetNode);
         }
      }
   }

   return usingZoneSets;
}

bool WsfPM_Zone::Rename(std::string aNewName)
{
   WsfPProxyPath oldPath     = GetPath();
   std::string   currentName = GetName();
   if (currentName == aNewName)
   {
      return false;
   }
   WsfPM_Root root(Root());
   if (root.zones() == GetParent()) // if global zone
   {
      WsfPM_ZoneMap zoneMap = root.zones();
      if (!zoneMap.CanMove(currentName, aNewName))
      {
         return false;
      }
      zoneMap.Move(currentName, aNewName);
      // update path to new name
      mPath.Back().SetMapKey(aNewName);
      Invalidate();
   }
   else
   {
      WsfPProxyNode parentParent = GetParent().GetParent();
      if (parentParent && parentParent.IsA_StructTypeOf("Platform")) // zone belongs to platform
      {
         WsfPM_Platform owner(parentParent);
         WsfPM_ZoneMap  zoneMap = owner.Zone();
         if (!zoneMap.CanMove(currentName, aNewName))
         {
            return false;
         }
         zoneMap.Move(currentName, aNewName);
         // update path to new name
         mPath.Back().SetMapKey(aNewName);
         Invalidate();
      }
      else
      {
         return false;
      }
   }
   GetProxy()->RecordObjectRename(oldPath, GetPath());
   return true;
}
