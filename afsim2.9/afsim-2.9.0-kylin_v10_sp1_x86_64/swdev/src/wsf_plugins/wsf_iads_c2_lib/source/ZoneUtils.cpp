// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "ZoneUtils.hpp"
#include <cassert>
#include <iostream>
#include <iadsLib/NumericUtils.hpp>
#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"
#include "WsfZone.hpp"
#include "WsfZoneDefinition.hpp"
#include "WsfGeoPoint.hpp"
#include "vclMath/Geometry.hpp"
#include <logger.hpp>

#include "WsfSimulation.hpp"

#include "WsfIADSC2ScenarioExtension.hpp"

namespace ZoneUtils
{
   // =================================================================================================
   // @return GeoPoint representing the given point in ECS relative to the entity.
   // @param  entity   An Entity relative to which we want to interpret the input ECS point.
   // @param  point    The mX and mY fields are ECS X and Y coordinates.
   static WsfGeoPoint convertEcsToGeoPoint(UtEntity& entity, const WsfZone::Point& point)
   {
      double wcsLoc[3];
      double ecsLoc[3] = { point.mX, point.mY, 0.0 };
      entity.ConvertECSToWCS(ecsLoc, wcsLoc);
      double lat, lon, alt;
      entity.ConvertWCSToLLA(wcsLoc, lat, lon, alt);
      return WsfGeoPoint(lat, lon, 0);
   }

   // =================================================================================================
   static bool PointIsInAngularExtents(WsfSimulation* sim_ptr, WsfZoneDefinition& zone, const WsfGeoPoint& point)
   {
      bool inExtents = true;
      double startAngle;
      double stopAngle;
      zone.GetAngleBounds(startAngle, stopAngle);
      // 0 .. 2*PI becomes 0 .. 0 after normalization. If angles are equal after normalization,
      // assume this means that you want the full circle and skip the calculations.
      startAngle = UtMath::NormalizeAngle0_TwoPi(startAngle);
      stopAngle = UtMath::NormalizeAngle0_TwoPi(stopAngle);
      if (!NumericUtils::NearlyEqual(startAngle, stopAngle))
      {
         // Find the angle of the point relative to the zone major axis.
         UtEntity entity;
         WsfGeoPoint centroid = GetZoneCentroid(sim_ptr, zone);
         entity.SetLocationLLA(centroid.GetLat(), centroid.GetLon(), centroid.GetAlt());
         entity.SetOrientationNED(zone.GetReferenceHeading(), 0, 0);
         double wcsPoint[3];
         point.GetLocationWCS(wcsPoint);
         double wcsRelative[3];
         entity.GetRelativeLocationWCS(wcsPoint, wcsRelative);
         double relAz;
         double relEl;
         entity.ComputeAspect(wcsRelative, relAz, relEl);

         inExtents = UtMath::AngleIsBetween(relAz, startAngle, stopAngle);
      }
      return inExtents;
   }

   // =================================================================================================
   static double GetDistanceFromEllipseToPoint(WsfSimulation* sim_ptr, WsfZoneDefinition& zone, const WsfGeoPoint& point)
   {
      assert(zone.GetShapeType() == WsfZoneDefinition::cELLIPTICAL);
      double distance = 0;

      WsfGeoPoint centroid(GetZoneCentroid(sim_ptr, zone));

      if (PointIsInAngularExtents(sim_ptr, zone, point))
      {
         // The parametric equations for an ellipse yield the point at a given angle relative to
         // the origin of the ellipse. If the ellipse semi-major axis aligned with North, the
         // angle would simply be the bearing of the point from the ellipse centroid.  Because the
         // ellipse can be aligned with any compass heading, we need to compensate for that. Once we
         // have determined the heading to the point relative to the semi-major axis, it is a simple
         // matter to substitute into the following parametric equations.
         //    x = a cos(t)
         //    y = b sin(t)
         // Once we have X and Y relative to the ellipse, we convert those local coordinates into
         // world coordinates and calculate the distance.

         // Note: All locally declared angles are in radians. AFNES Zone functions return degrees, while WsfGeoPoint
         //       returns radians from TrueBearingTo!!!

         // This difference tells us how far around the ellipse we need to go in angular space.
         // Note: zone::GetReferenceHeading is assumed to return radians.
         auto globalLogger = IADSC2ScenarioExtension::GetGlobalLogger(sim_ptr->GetScenario());

         double ellipseAngleRadians = centroid.TrueBearingTo(point) - zone.GetReferenceHeading();
         ellipseAngleRadians = UtMath::NormalizeAngle0_TwoPi(ellipseAngleRadians);
         HCL_TRACE_LOGGER(globalLogger) << "Ellipse angle = " << ellipseAngleRadians << " " << ellipseAngleRadians* UtMath::cDEG_PER_RAD << std::endl;
         double x = zone.GetLonAxisLength() / 2.0 * cos(ellipseAngleRadians);
         double y = zone.GetLatAxisLength() / 2.0 * sin(ellipseAngleRadians);
         double ellipsePointECS[3] = { x, y, 0.0 };
         HCL_TRACE_LOGGER(globalLogger) << "Ellipse point = " << x << ", " << y << std::endl;

         // X and Y are in the ellipse's local coordinate frame. Need to transform
         // them into WCS.
         UtEntity uteRef;  //reference entity used to make the conversions
         double arrayCentroid[3];
         double ellipsePointWCS[3];
         centroid.GetLocationWCS(arrayCentroid);
         uteRef.SetLocationWCS(arrayCentroid);
         uteRef.SetOrientationNED(zone.GetReferenceHeading(), 0.0, 0.0);
         uteRef.ConvertECSToWCS(ellipsePointECS, ellipsePointWCS);
         double pointWCS[3];
         double pointECS[3];
         point.GetLocationWCS(pointWCS);
         uteRef.ConvertWCSToECS(pointWCS, pointECS);
         HCL_TRACE_LOGGER(globalLogger) << "Point in ECS: " << pointECS[0] << ", " << pointECS[1] << ", " << pointECS[2] << std::endl;

         // Finally, calculate the distance from the point on the ellipse to the
         // input point.
         distance = WsfGeoPoint(ellipsePointWCS).GroundRangeTo(point);
      }
      else
      {
         // If we are outside the angular bounds that were specified, then just return the distance to the
         // centroid
         distance = centroid.GroundRangeTo(point);
      }
      return distance;
   }

   // =================================================================================================
   // For zones that are relative to a platform, it is necessary to recalculate the centroid to
   // account for motion of the platform.
   //
   // @return true if we could get a handle to the platform.
   // @param zone  The zone for which we desire the centroid.
   // @param relEcsCentroid The centroid of the zone's set of points that are all relative to the platform.
   // @param wcsCentroid The calculated centroid in WCS coordinates.
   static bool RecalcCentroid(WsfSimulation* sim_ptr, const WsfZoneDefinition& zone, const WsfGeoPoint& relEcsCentroid, WsfGeoPoint& wcsCentroid)
   {
      bool success = false;
      wcsCentroid.SetLocationLLA(0, 0, 0); // Just put something in there.
      WsfPlatform* platform = zone.GetReferencePlatform(sim_ptr);
      if (platform != nullptr)
      {
         double lat, lon, alt, headingRadians, pitch, roll;
         // Create a temporary entity that is located at the same location as the platform and has the same heading.
         // Then we can use the entity class's functions to do the transforms for us.
         // You could just use the reference platform for these calculations if you wanted the zone to
         // rotate with the platform's pitch and roll. We just want to use the heading.
         platform->GetLocationLLA(lat, lon, alt);
         platform->GetOrientationNED(headingRadians, pitch, roll);
         UtEntity utE;
         utE.SetLocationLLA(lat, lon, 0.0);
         utE.SetOrientationNED(headingRadians, 0.0, 0.0); // don't use pitch and roll`
         double relEcsCentroidArray[3];
         double wcsCentroidArray[3];
         // The relEcsCentroid is supposed to be relative to the platform in ECS, so just use the entity's
         // conversion function to calculate the WCS coordinates of the ECS coordinates.
         utE.ConvertECSToWCS(relEcsCentroidArray, wcsCentroidArray);
         wcsCentroid.SetLocationWCS(wcsCentroidArray);
         success = true;
      }
      return success;
   }

   // =================================================================================================
   // @return WCS centroid of the zone.
   WsfGeoPoint GetZoneCentroid(WsfSimulation* sim_ptr, WsfZoneDefinition& zone)
   {
      WsfGeoPoint wcsCentroid;
      // only works when: mRelativeTo == cINTERNAL && mPointsAreLatLon == true
      if (zone.GetShapeType() == WsfZoneDefinition::cPOLYGONAL)
      {
         if (zone.GetRelativeType() == WsfZoneDefinition::cINTERNAL)
         {
            //In this case, we just have lat/lon points. No adjustment needed.
            wcsCentroid = zone.Centroid();
         }
         else
         {
            RecalcCentroid(sim_ptr, zone, zone.Centroid(), wcsCentroid);
         }
      }
      else // circular, elliptical, or spherical
      {
         if (zone.GetRelativeType() == WsfZoneDefinition::cINTERNAL)
         {
            wcsCentroid = zone.Reference();
         }
         else
         {
            // relative to the reference platform
            RecalcCentroid(sim_ptr, zone, zone.Reference(), wcsCentroid);
         }
      }

      return wcsCentroid;
   }

   // @return true If the line segment defined by beginPoint and endPoint intersects the polygon described by the points vector.
   // @param beginPoint First point of the line segment.
   // @param endPoint   End point of the line segment.
   // @param points     Vector of points the comprise the polygon.
   // @param intersectionPoint Point at which the line segment intersects a segment of the polygon
   // @param intersectDistance The distance of the intersect point from the beginPoint of the line segment.
   static bool FindIntersection(const WsfGeoPoint& beginPoint, const WsfGeoPoint& endPoint, const std::vector<WsfGeoPoint>& points,
                                WsfGeoPoint& intersectionPoint, double& intersectDistance)
   {
      bool foundIntersection = false;
      intersectDistance = 1.0e20;

      // Loop through all the vertices, starting with the last and first point.
      double xi, yi;

      // prime the pump
      double latLast = points.back().GetLat();
      double lonLast = points.back().GetLon();

      // The "auto" type is convenient for cases where you use an STL iterator.
      // Let the compiler figure out the long name.
      for (const auto& point : points)
      {
         double lat = point.GetLat();
         double lon = point.GetLon();

         if (vm::intersectLineSegments(
            lonLast, latLast,
            lon, lat,
            beginPoint.GetLon(), beginPoint.GetLat(),
            endPoint.GetLon(), endPoint.GetLat(),
            xi, yi)
            )
         {
            foundIntersection = true;
            WsfGeoPoint tempPoint(yi, xi, 0);
            double distance = tempPoint.GroundRangeTo(beginPoint);
            if (distance < intersectDistance)
            {
               intersectDistance = distance;
               intersectionPoint = tempPoint;
            }
         }

         // Save this vertex as the last vertex.
         latLast = lat;
         lonLast = lon;
      }
      return foundIntersection;
   }

   // =================================================================================================
   // Computes a set of WsfGeoPoints that define the vertices of the zone geometry.  It does the right
   // thing for cases where the points are relative to something else. For ellipses and circles,
   // it approximates the shape with the number of points specified in the local constant, numPoints.
   // @param zone The zone from which to extract the vertices
   // @param points A list of the vertices of the polygon.
   void GetLatLonPoints(WsfSimulation* sim_ptr, WsfZoneDefinition& zone, std::vector<WsfGeoPoint>& points)
   {
      points.clear();
      const int numPoints = 16; // More points will be more accurate, but take more time in intersection calcs.
      WsfGeoPoint centroid = GetZoneCentroid(sim_ptr, zone); // use this rather than Zone::Centroid();
      UtEntity uteRef;  //reference entity used to make the conversions
      double arrayCentroid[3];
      centroid.GetLocationWCS(arrayCentroid);
      uteRef.SetLocationWCS(arrayCentroid);
      uteRef.SetOrientationNED(zone.GetReferenceHeading(), 0.0, 0.0);

      switch (zone.GetShapeType())
      {
         case WsfZoneDefinition::cNO_SHAPE:
            break;
         case WsfZoneDefinition::cCIRCULAR:
         {
            double minAngle;
            double maxAngle;
            double a = 0;
            double b = 0;
            // Use the ellipse equation with a == b for circle.
            if (zone.GetShapeType() == WsfZoneDefinition::cCIRCULAR)
            {
               a = b = zone.GetRadius();
            }
            else
            {
               a = zone.GetLonAxisLength() / 2.0;
               b = zone.GetLatAxisLength() / 2.0;
            }
            zone.GetAngleBounds(minAngle, maxAngle);
            if (NumericUtils::LessThan((maxAngle - minAngle), UtMath::cTWO_PI))
            {
               // If we don't have the whole shape, then the centroid should be the first point.
               points.emplace_back(centroid.GetLat(), centroid.GetLon(), 0);
            }
            double stepSize = UtMath::cTWO_PI / (double)numPoints;
            for (double angle = minAngle; angle < maxAngle; angle += stepSize)
            {
               double x = a * cos(angle);
               double y = b * sin(angle);
               WsfZone::Point ellipsePointECS(x, y);

               // X and Y are in the ellipse's local coordinate frame. Need to transform
               // them into lat/lon.
               WsfGeoPoint geoPoint = convertEcsToGeoPoint(uteRef, ellipsePointECS);
               points.emplace_back(geoPoint.GetLat(), geoPoint.GetLon(), 0);
            }
         }
         break;
         case WsfZoneDefinition::cELLIPTICAL:
            break;
         case WsfZoneDefinition::cPOLYGONAL:
            // Ensure that points are lat/lon - not relative.
            if (zone.GetRelativeType() == WsfZoneDefinition::cOBSERVER)
            {
               // convert to lat/lon
               std::vector<WsfZone::Point> ecsPoints = zone.GetPoints();

               for (auto& ecsPoint : ecsPoints)
               {
                  WsfGeoPoint geoPoint = convertEcsToGeoPoint(uteRef, ecsPoint);
                  points.emplace_back(geoPoint.GetLat(), geoPoint.GetLon(), 0);
               }
            }
            else
            {
               // use points as-is
               std::vector<WsfZone::Point> llaPoints = zone.GetPoints();
               for (auto& llaPoint : llaPoints)
               {
                  points.emplace_back(llaPoint.mX, llaPoint.mY, 0);
               }
            }
            break;
         case WsfZoneDefinition::cSPHERICAL:
            break;
         default:
            break;
      }
   }

   // =================================================================================================
   bool GetDistanceFromPointToZone(WsfSimulation* sim_ptr, double wcsPoint[3], WsfZoneDefinition& zone, double& distance)
   {
      assert(zone.GetRelativeType() == WsfZoneDefinition::cINTERNAL);

      WsfGeoPoint point;
      point.SetLocationWCS(wcsPoint);
      distance = -1.0;

      /* Query the DA zone with the track position. */
      double arrayZoneCentroid[3];
      WsfGeoPoint centroid = GetZoneCentroid(sim_ptr, zone); // use this rather than Zone::Centroid();
      centroid.GetLocationWCS(arrayZoneCentroid);

      // If it is a relative zone, then the eyepoint parameter (param 2) has to be set; otherwise
      // you will get a null pointer exception.  If it is an internal zone, then
      // you can pass a null for the eyepoint and 0 for the look heading (param 3).
      if (zone.PointIsInside(sim_ptr, wcsPoint, arrayZoneCentroid, zone.GetReferenceHeading()))
      {
         HCL_TRACE_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(sim_ptr->GetScenario())) << "Point is inside polygon. Distance = 0.0" << std::endl;
         distance = 0.0;
      }
      else if (zone.GetShapeType() == WsfZoneDefinition::cCIRCULAR || zone.GetShapeType() == WsfZoneDefinition::cSPHERICAL)
      {
         distance = centroid.GroundRangeTo(point);
         if (PointIsInAngularExtents(sim_ptr, zone, point))
         {
            // If within angular extents, range will be to the outside of the circle. Account
            // for this by subtracting off the radius.
            // Otherwise, assume that the distance is to the centroid.
            distance = distance - zone.GetRadius();
         }
      }
      else if (zone.GetShapeType() == WsfZoneDefinition::cELLIPTICAL)
      {
         distance = GetDistanceFromEllipseToPoint(sim_ptr, zone, point);
      }
      else if (zone.GetShapeType() == WsfZoneDefinition::cPOLYGONAL)
      {
         std::vector<WsfGeoPoint> points;
         GetLatLonPoints(sim_ptr, zone, points);
         WsfGeoPoint intersectionPoint;
         bool foundIntersection = FindIntersection(point, centroid, points, intersectionPoint, distance);
         if (!foundIntersection)
         {
            // This case should not happen unless you have a non-convex shape where the
            // the centroid is outside of the shape and you are also unlucky and the
            // point is on the side where the polygon is not between it and the centroid.
            distance = -1.0;
         }
      }
      return (distance >= 0);
   } /* GetDistanceFromPointToZone */

   // =================================================================================================
   double GetDistanceFromPointToZoneCenter(WsfSimulation* sim_ptr, double wcsPoint[3], WsfZoneDefinition& zone)
   {
      assert(zone.GetRelativeType() == WsfZoneDefinition::cINTERNAL);

      WsfGeoPoint point;
      point.SetLocationWCS(wcsPoint);

      /* Query the DA zone with the track position. */
      double arrayZoneCentroid[3];
      WsfGeoPoint centroid = GetZoneCentroid(sim_ptr, zone); // use this rather than Zone::Centroid();
      centroid.GetLocationWCS(arrayZoneCentroid);

      return centroid.GroundRangeTo(point);
   } /* GetDistanceFromPointToZoneCenter */

   // =================================================================================================
   // @return heading in radians of the velocity vector starting at the wcsPosition
   // @param wcsPosition WCS position vector giving the origin of the vector
   // @param wcsVelocity WCS velocity vector giving direction the the vector
   static double GetHeading(double wcsPosition[3], double wcsVelocity[3])
   {
      WsfGeoPoint pos(wcsPosition);
      double pointingVec[3];
      // Just create a point out in the direction of the velocity vector and calculate the bearing to that point.
      UtVec3d::Normalize(pointingVec, wcsVelocity);
      double wcsOffset[3];
      UtVec3d::Multiply(wcsOffset, pointingVec, 1000.0);
      double newPos[3];
      UtVec3d::Add(newPos, wcsOffset, wcsPosition);
      WsfGeoPoint newPosPoint(newPos);
      double headingRadians = pos.TrueBearingTo(newPosPoint);
      return headingRadians;
   }

   // =================================================================================================
   // When we are determining if a target will at some point cross into a zone, we calculate a line segment
   // from the target's current point to some destination point and then see if the segment intersects
   // the zone geometry. A simple point we could pick is the centroid and then use the distance to the
   // centroid as the distance for our second point in the case where we have a point and a velocity or
   // a point and a direction. But there are some cases (like an ellipse at a 45 degree angle relative
   // to the line segment) where this would not be enough distance to ensure that the line will intersect
   // the geometry.  To allow for this, we
   static double GetExtraDistanceBasedOnShape(WsfZoneDefinition& zone)
   {
      double extraDistance = 0.0;
      switch (zone.GetShapeType())
      {
         case WsfZoneDefinition::cCIRCULAR:
         case WsfZoneDefinition::cSPHERICAL:
            extraDistance = zone.GetRadius();
            break;
         case WsfZoneDefinition::cELLIPTICAL:
            extraDistance = zone.GetLonAxisLength();
            break;
         case WsfZoneDefinition::cPOLYGONAL:
         {
            double southernLat, westernLon, northernLat, easternLon;
            zone.GetLatLonExtrema(southernLat, westernLon, northernLat, easternLon);
            double headingDegrees = 0;
            // use the diagonal distance.
            UtSphericalEarth::GreatCircleHeadingAndDistance(southernLat, easternLon, northernLat, westernLon,
                                                            headingDegrees, extraDistance);
         }
         break;
         default:
            extraDistance = 0.0;
      }
      return extraDistance;
   }
   // =================================================================================================
   bool isProjectedInsideZonePosVel(WsfSimulation* sim_ptr, WsfZoneDefinition& zone, double wcsPosition[3], double wcsVelocity[3])
   {
      assert(zone.GetRelativeType() == WsfZoneDefinition::cINTERNAL);

      // Calculate the heading based on the velocity and defer the calculation to the function
      // that using a position and a heading.
      double headingRadians = GetHeading(wcsPosition, wcsVelocity);
      return isProjectedInsideZonePointHeading(sim_ptr, zone, wcsPosition, headingRadians);
   }

   // Because we are comparing a double parameter with a double constant, the use of == should be OK.
   static bool ValueSet(double val)
   {
      return val == WsfZone::cUNSPECIFIED - val;
   }

   // =================================================================================================
   // return true if the average altitude from the two points is in the zone's min/max altitude range.
   static bool WithinAltitudeBounds(WsfZoneDefinition& zone, double WcsPosition1[3], double WcsPosition2[3])
   {
      bool inBounds = true;
      WsfGeoPoint pos1(WcsPosition1);
      WsfGeoPoint pos2(WcsPosition2);
      double avgAlt = (pos1.GetAlt() + pos2.GetAlt()) / 2.0;
      double minAlt, maxAlt;
      zone.GetAltBounds(minAlt, maxAlt);
      if ((ValueSet(minAlt) && avgAlt < minAlt) ||
         (ValueSet(maxAlt) && avgAlt > maxAlt))
      {
         inBounds = false;
      }
      return inBounds;
   }

   // =================================================================================================
   // This function is used by the other two isProjectedInsideZone... functions.  This function does not
   // try to extend the segment to see if it would ever intersect the zone.  The assumption is that
   // you know what you are doing and have computed the points far enough apart that their segment is all that
   // should be considerd. See the other two functions for examples of this.
   bool isProjectedInsideZone2Points(WsfSimulation* sim_ptr, WsfZoneDefinition& zone, double WcsPosition1[3], double WcsPosition2[3])
   {
      assert(zone.GetRelativeType() == WsfZoneDefinition::cINTERNAL);

      bool insideZone = zone.PointIsInside(sim_ptr, WcsPosition1, WcsPosition1, 0, 0);

      if (!insideZone && WithinAltitudeBounds(zone, WcsPosition1, WcsPosition2))
      {
         std::vector<WsfGeoPoint> points;
         GetLatLonPoints(sim_ptr, zone, points);
         WsfGeoPoint pos1(WcsPosition1);
         WsfGeoPoint pos2(WcsPosition2);
         WsfGeoPoint intersectionPoint;
         double distance = 0;
         insideZone = FindIntersection(pos1, pos2, points, intersectionPoint, distance);
      }

      return insideZone;
   }

   // =================================================================================================
   bool isProjectedInsideZonePointHeading(WsfSimulation* sim_ptr, WsfZoneDefinition& zone, double wcsPosition[3], double headingRadians)
   {
      assert(zone.GetRelativeType() == WsfZoneDefinition::cINTERNAL);

      // Calculate a second point that is guaranteed to be on the other side of the zone so that the
      // segment will for sure pass through the zone if the segment is pointed in the right direction.
      // Then defer the calculation to the function that takes two points as parameters.

      WsfGeoPoint centroid = GetZoneCentroid(sim_ptr, zone);
      double distance = centroid.GetDistanceFrom(wcsPosition);

      // Add a fudge factor onto the distance to ensure that our resulting line segment is long enough.
      double extraDistance = GetExtraDistanceBasedOnShape(zone);
      distance += extraDistance;

      double endPointLat, endPointLon;
      double targetLat, targetLon, targetAlt;
      UtEntity::ConvertWCSToLLA(wcsPosition, targetLat, targetLon, targetAlt);
      UtSphericalEarth::ExtrapolateGreatCirclePosition(targetLat, targetLon, headingRadians * UtMath::cDEG_PER_RAD, distance, endPointLat, endPointLon);
      double endPointWcs[3];
      UtEntity::ConvertLLAToWCS(endPointLat, endPointLon, 0, endPointWcs);

      return isProjectedInsideZone2Points(sim_ptr, zone, wcsPosition, endPointWcs);
   }

   // =================================================================================================
   bool GetClosestPointInZone(WsfSimulation* sim_ptr, double wcsPoint[3], double wcsZonePoint[3], WsfZoneDefinition& zone)
   {
      WsfGeoPoint point;
      point.SetLocationWCS(wcsPoint);

      /* Query the DA zone with the track position. */
      double arrayZoneCentroid[3];
      WsfGeoPoint centroid = GetZoneCentroid(sim_ptr, zone); // use this rather than Zone::Centroid();
      centroid.GetLocationWCS(arrayZoneCentroid);

      // If it is a relative zone, then the eye-point parameter (param 2) has to be set; otherwise
      // you will get a null pointer exception.  If it is an internal zone, then
      // you can pass a null for the eye-point and 0 for the look heading (param 3).
      if (zone.PointIsInside(sim_ptr, wcsPoint, arrayZoneCentroid, zone.GetReferenceHeading()))
      {
         HCL_TRACE_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(sim_ptr->GetScenario())) << "Point is inside polygon. Distance = 0.0" << std::endl;
         wcsZonePoint[0] = wcsPoint[0];
         wcsZonePoint[1] = wcsPoint[1];
         wcsZonePoint[2] = wcsPoint[2];
         return true;
      }
      else if (zone.GetShapeType() == WsfZoneDefinition::cCIRCULAR || zone.GetShapeType() == WsfZoneDefinition::cSPHERICAL)
      {
         wcsZonePoint[0] = wcsPoint[0] - arrayZoneCentroid[0];
         wcsZonePoint[1] = wcsPoint[1] - arrayZoneCentroid[1];
         wcsZonePoint[2] = wcsPoint[2] - arrayZoneCentroid[2];

         double magnitude = zone.GetRadius() / sqrt(wcsZonePoint[0] * wcsZonePoint[0] + wcsZonePoint[1] * wcsZonePoint[1] + wcsZonePoint[2] * wcsZonePoint[2]);

         wcsZonePoint[0] = arrayZoneCentroid[0] + wcsZonePoint[0] * magnitude;
         wcsZonePoint[1] = arrayZoneCentroid[1] + wcsZonePoint[1] * magnitude;
         wcsZonePoint[2] = arrayZoneCentroid[2] + wcsZonePoint[2] * magnitude;
         return true;
      }
      else if (zone.GetShapeType() == WsfZoneDefinition::cELLIPTICAL)
      {
         wcsZonePoint[0] = wcsPoint[0] - arrayZoneCentroid[0];
         wcsZonePoint[1] = wcsPoint[1] - arrayZoneCentroid[1];
         wcsZonePoint[2] = wcsPoint[2] - arrayZoneCentroid[2];

         double magnitude = (centroid.GroundRangeTo(point) - GetDistanceFromEllipseToPoint(sim_ptr, zone, point)) / sqrt(wcsZonePoint[0] * wcsZonePoint[0] + wcsZonePoint[1] * wcsZonePoint[1] + wcsZonePoint[2] * wcsZonePoint[2]);

         wcsZonePoint[0] = arrayZoneCentroid[0] + wcsZonePoint[0] * magnitude;
         wcsZonePoint[1] = arrayZoneCentroid[1] + wcsZonePoint[1] * magnitude;
         wcsZonePoint[2] = arrayZoneCentroid[2] + wcsZonePoint[2] * magnitude;
         return true;
      }
      else if (zone.GetShapeType() == WsfZoneDefinition::cPOLYGONAL)
      {
         bool foundClosestPoint = false;
         std::vector<WsfGeoPoint> points;
         GetLatLonPoints(sim_ptr, zone, points);
         WsfGeoPoint intersectionPoint;
         double distance;
         if (FindIntersection(point, centroid, points, intersectionPoint, distance))
         {
            // This case should not happen unless you have a non-convex shape where the
            // the centroid is outside of the shape and you are also unlucky and the
            // point is on the side where the polygon is not between it and the centroid.
            intersectionPoint.GetLocationWCS(wcsZonePoint);
            foundClosestPoint = true;
         }
         for (WsfGeoPoint vertex : points)
         {
            double tempDistance = vertex.GroundRangeTo(point);
            if (tempDistance < distance)
            {
               distance = tempDistance;
               vertex.GetLocationWCS(wcsZonePoint);
               foundClosestPoint = true;
            }
         }
         return foundClosestPoint;
      }
      return false;
   } /* GetClosestPointInZone */
}
