// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include "test_zone.hpp"

#include <ZoneUtils.hpp>
#include <UtMath.hpp>
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "WsfZoneDefinition.hpp"
#include "WsfGeoPoint.hpp"
#include <cstdio>
#include <vclMath/GeodeticToECEF.hpp>
#include <iadsLib\NumericUtils.hpp>

#include <logger.hpp>
// Includes all of the optional projects which can be compiled with AFNES
//bc #include "Configure.hpp"
//bc #include "OptionalProjects.hpp"


bool TestAssert(const std::string& description, bool condition)
{
   cout << description << " " << (condition ? "PASSED" : "FAILED") << endl;
   return condition;
}


void TestRotation()
{
#ifndef JLKPLUGIN_TEMP_COMPILEWILLCRASH
   WsfGeoPoint referencePoint(0.0, 0.0, 0.0);
   WsfGeoPoint targetPoint(0.1, 0.0, 0.0);
   double targetWCS[3];
   double targetECS[3];
   targetPoint.GetLocationWCS(targetWCS);
   double heading = 0;
   UtEntity uteRef;  //reference entity used to make the conversions
   uteRef.SetLocationLLA(referencePoint.GetLat(), referencePoint.GetLon(), referencePoint.GetAlt());
   uteRef.SetOrientationNED(heading, 0.0, 0.0);
   uteRef.ConvertWCSToECS(targetWCS, targetECS);
   printf("Distance to target = %f\n", referencePoint.GetDistanceFrom(targetPoint));
   printf("Ground range to target = %f\n", referencePoint.GroundRangeTo(targetPoint));
   FILE* f = fopen("test.csv", "w");
   fprintf(f, "heading, x, y, z\n");
   for (heading = 0; heading < 360.0; heading += 1.0)
   {
      {
         UtEntity uteRef;  //reference entity used to make the conversions
         uteRef.SetLocationLLA(referencePoint.GetLat(), referencePoint.GetLon(), referencePoint.GetAlt());
         uteRef.SetOrientationNED(heading * UtMath::cRAD_PER_DEG, 0.0, 0.0);
         uteRef.ConvertWCSToECS(targetWCS, targetECS);
         fprintf(f, "%f, %f, %f, %f\n", heading, targetECS[0], targetECS[1], targetECS[2]);
      }
   }
   fclose(f);
#endif
}

void TestCircle()
{
#ifndef JLKPLUGIN_TEMP_COMPILEWILLCRASH
   // Bob changes
   double zoneLat = 0.0;
   double zoneLon = 0.0;
   double zoneAlt = 0.0;
   // NOT_USED double pointLat = 0.0;
   // NOT_USED double pointLon = 0.0;
   // NOT_USED double pointAlt = 0.0;

   cout << endl;

   WsfGeoPoint point(0.0, 0.0, 0.0);
   double wcsPoint[3];
   double distance = -99.0;
   WsfZoneDefinition circle;
   circle.SetShapeType(WsfZoneDefinition::cCIRCULAR);
   circle.SetRelativeType(WsfZoneDefinition::cINTERNAL);
   circle.SetRadialBounds(0.0, 10000.0);
   circle.SetReference(zoneLat, zoneLon, zoneAlt);
   point.GetLocationWCS(wcsPoint);
   bool success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, circle, distance);
   cout << "Test 1. Point at center of circle. Shape type =  " << circle.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << distance << endl << endl;
   TestAssert("Test 1. Point at center of circle. Distance should be 0.0", distance == 0);

   // Now create a point that is outside of the circle.
   zoneLat = 2.0;
   circle.SetReference(zoneLat, zoneLon, zoneAlt);
   double test2Distance = 0;
   success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, circle, test2Distance);
   cout << "Test 2. Point at 2 deg south of circle centroid. Shape type =  " << circle.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << distance << endl << endl;
   TestAssert("Test 2. Point at center of circle. Distance should be 0.0", distance == 0);

   // Now create a point that is outside of the circle and to the east.
   // NOT_USED zoneLat = 0.0;
   // NOT_USED zoneLon = 2.0;
   circle.SetReference(0.0, 0.0, 0.0);
   WsfGeoPoint eastPoint(0.0, 2.0, 0.0);
   eastPoint.GetLocationWCS(wcsPoint);
   circle.SetAngleBounds(UtMath::cPI, 3.0 * UtMath::cPI / 2.0); // zone runs from 180 - 270. Point is at 90.
   double test3Distance = 0;
   success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, circle, test3Distance);
   cout << "Test 3. Point outside of circle, not in angular bounds, so distance should be to centroid."
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << distance << endl << endl;
   TestAssert("Test 3. Point outside angular bounds. Distance should be greater than test 2.", test3Distance > test2Distance);
#endif
}

void TestEllipse()
{
#ifndef JLKPLUGIN_TEMP_COMPILEWILLCRASH
   cout << "======================================================================================================" << endl;
   cout << "TestEllipse" << endl;
   cout << "======================================================================================================" << endl;
   WsfGeoPoint point(0.0, 0.0, 0.0);
   double wcsPoint[3];
   bool success;
   cout << "======================================================================================================" << endl;
   double latRef = 0;
   double lonRef = 0;
   double headingRef = 0;
   WsfZoneDefinition ellipse;
   ellipse.SetRelativeType(WsfZoneDefinition::cINTERNAL);
   ellipse.SetShapeType(WsfZoneDefinition::cELLIPTICAL);
   double lateralAxis = 2500;
   double longitudinalAxis = 6000;
   ellipse.SetEllipticalAxes(lateralAxis, longitudinalAxis);

   cout << "======================================================================================================" << endl;
   headingRef = 0; // Back to north. Should be covering the point now.
   latRef = 0.025; // Should be about 3KM from (0,0)
   lateralAxis = 2500;
   longitudinalAxis = 6000;
   ellipse.SetEllipticalAxes(lateralAxis, longitudinalAxis);
   ellipse.SetReference(latRef, lonRef, headingRef * UtMath::cRAD_PER_DEG);
   point.GetLocationWCS(wcsPoint);
   double test1Distance = 0;
   success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, ellipse, test1Distance);
   cout << "Test 1. Point at (0,0,0) ellipse at (0.025, 0, 0) heading = " << headingRef << ". Shape type =  " << ellipse.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << test1Distance << endl << endl;
   TestAssert("Test 1. Ellipse vertical, should contain point, expect distance = 0", test1Distance == 0);

   cout << "======================================================================================================" << endl;
   headingRef = 45;
   latRef = 0.025; // Should be about 3KM from (0,0)
   lateralAxis = 2500;
   longitudinalAxis = 6000;
   ellipse.SetEllipticalAxes(lateralAxis, longitudinalAxis);
   ellipse.SetReference(latRef, lonRef, headingRef * UtMath::cRAD_PER_DEG);
   point.GetLocationWCS(wcsPoint);
   double test2Distance = 0;
   success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, ellipse, test2Distance);
   cout << "Test 2. Point at (0,0,0) ellipse at (0.025, 0, 0) heading = " << headingRef << ". Shape type =  " << ellipse.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << test2Distance << endl << endl;
   TestAssert("Test 2. Ellipse at 45, expect distance > 0", test2Distance > 0);

   cout << "======================================================================================================" << endl;
   headingRef = 90; // point it east. The point is to the south, so this should make the range larger than test 4 by about 500m
   latRef = 0.025; // Should be about 3KM from (0,0)
   lateralAxis = 2500;// Since we are rotated 90 degrees, the point should be about 500m south of the edge.
   longitudinalAxis = 6000;
   ellipse.SetEllipticalAxes(lateralAxis, longitudinalAxis);
   ellipse.SetReference(latRef, lonRef, headingRef * UtMath::cRAD_PER_DEG);
   point.GetLocationWCS(wcsPoint);
   double test3Distance = 0;
   success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, ellipse, test3Distance);
   cout << "Test 3. Point at (0,0,0) ellipse at (0.025, 0, 0) heading = " << headingRef << ". Shape type =  " << ellipse.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << test3Distance << endl << endl;
   TestAssert("Test 3. Ellipse at 90, expect distance > 0", test3Distance > 0);

   cout << "======================================================================================================" << endl;
   headingRef = 135;
   latRef = 0.025; // Should be about 3KM from (0,0)
   lateralAxis = 2500;
   longitudinalAxis = 6000;
   ellipse.SetEllipticalAxes(lateralAxis, longitudinalAxis);
   ellipse.SetReference(latRef, lonRef, headingRef * UtMath::cRAD_PER_DEG);
   point.GetLocationWCS(wcsPoint);
   double test4Distance = 0;
   success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, ellipse, test4Distance);
   cout << "Test 4. Point at (0,0,0) ellipse at (0.025, 0, 0) heading = " << headingRef << ". Shape type =  " << ellipse.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << test4Distance << endl << endl;
   TestAssert("Test 4. Ellipse at 135, expect distance > 0", test4Distance > 0);

   cout << "======================================================================================================" << endl;
   headingRef = 180;
   latRef = 0.025; // Should be about 3KM from (0,0)
   lateralAxis = 2500;
   longitudinalAxis = 6000;
   ellipse.SetEllipticalAxes(lateralAxis, longitudinalAxis);
   ellipse.SetReference(latRef, lonRef, headingRef * UtMath::cRAD_PER_DEG);
   point.GetLocationWCS(wcsPoint);
   double test5Distance = 0;
   success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, ellipse, test5Distance);
   cout << "Test 5. Point at (0,0,0) ellipse at (0.025, 0, 0) heading = " << headingRef << ". Shape type =  " << ellipse.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << test5Distance << endl << endl;
   TestAssert("Test 5. Ellipse at 180, expect distance same as Test 1.", NumericUtils::NearlyEqual(test5Distance, test1Distance));

   cout << "======================================================================================================" << endl;
   headingRef = 225;
   latRef = 0.025; // Should be about 3KM from (0,0)
   lateralAxis = 2500;
   longitudinalAxis = 6000;
   ellipse.SetEllipticalAxes(lateralAxis, longitudinalAxis);
   ellipse.SetReference(latRef, lonRef, headingRef * UtMath::cRAD_PER_DEG);
   point.GetLocationWCS(wcsPoint);
   double test6Distance = 0;
   success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, ellipse, test6Distance);
   cout << "Test 6. Point at (0,0,0) ellipse at (0.025, 0, 0) heading = " << headingRef << ". Shape type =  " << ellipse.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << test6Distance << endl << endl;
   TestAssert("Test 6. Ellipse at 225, expect distance same as Test 4.", NumericUtils::NearlyEqual(test6Distance, test4Distance));

   cout << "======================================================================================================" << endl;
   headingRef = 270;
   latRef = 0.025; // Should be about 3KM from (0,0)
   lateralAxis = 2500;
   longitudinalAxis = 6000;
   ellipse.SetEllipticalAxes(lateralAxis, longitudinalAxis);
   ellipse.SetReference(latRef, lonRef, headingRef * UtMath::cRAD_PER_DEG);
   point.GetLocationWCS(wcsPoint);
   double test7Distance = 0;
   success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, ellipse, test7Distance);
   cout << "Test 7. Point at (0,0,0) ellipse at (0.025, 0, 0) heading = " << headingRef << ". Shape type =  " << ellipse.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << test7Distance << endl << endl;
   TestAssert("Test 7. Ellipse at 270, expect distance same as Test 3.", NumericUtils::NearlyEqual(test7Distance, test3Distance));

   cout << "======================================================================================================" << endl;
   lateralAxis = 1000;
   longitudinalAxis = 2000;
   ellipse.SetEllipticalAxes(lateralAxis, longitudinalAxis);
   ellipse.SetReference(0, 0, 0);
   point.SetLocationLLA(-2.0, 2.0, 0); // should be 135 relative to longitudinal axis
   point.GetLocationWCS(wcsPoint);
   double test10Distance = 0;
   success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, ellipse, test10Distance);
   cout << "Test 10. Point at (-2.0, 2.0, 0) ellipse at (0, 0, 0) heading = " << headingRef << ". Shape type =  "
      << ellipse.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success << ", distance = " << test10Distance << endl << endl;

   cout << "======================================================================================================" << endl;
   ellipse.SetAngleBounds(0, UtMath::cPI_OVER_2); // 0 .. 90. Does not include the point.
   double outOfBoundsDistance = 0;
   point.GetLocationWCS(wcsPoint);
   success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, ellipse, outOfBoundsDistance);
   cout << "Test 11. Point at (0,0,0) ellipse at (0.025, 0, 0) heading = " << headingRef << ". Shape type =  " << ellipse.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << outOfBoundsDistance << " Expect distance to be greater than previous test "
      << (outOfBoundsDistance > test10Distance) << endl << endl;
   TestAssert("Test 11. Repeat of Test 10 with angle extents excluding point. Distance should go up.", (outOfBoundsDistance > test10Distance));
#endif
}

void TestPolygon()
{
#ifndef JLKPLUGIN_TEMP_COMPILEWILLCRASH
   cout << "======================================================================================================" << endl;
   cout << "TestPolygon" << endl;
   cout << "======================================================================================================" << endl;
   WsfZoneDefinition polygon;
   double distance;
   double latRef = 0;
   double lonRef = 0;
   double headingRef = 0;
   double wcsPoint[3];
   polygon.SetRelativeType(WsfZoneDefinition::cINTERNAL);
   polygon.SetShapeType(WsfZoneDefinition::cPOLYGONAL);
   polygon.SetReference(latRef, lonRef, headingRef);
   WsfGeoPoint polygonRefPoint(latRef, lonRef, 0);
   std::vector<WsfGeoPoint> points;
   WsfGeoPoint targetPoint(0.2, 0.05, 0); // straight above and in the middle of the rectangle
   printf("Ground range to target = %f\n", polygonRefPoint.GroundRangeTo(targetPoint));

   // Simple rectangle.
   points.push_back(WsfGeoPoint(0.0, 0.0, 0.0));
   points.push_back(WsfGeoPoint(0.1, 0.0, 0.0));
   points.push_back(WsfGeoPoint(0.1, 0.1, 0.0));
   points.push_back(WsfGeoPoint(0.0, 0.1, 0.0));
   polygon.SetPolyPoints(points);
   targetPoint.GetLocationWCS(wcsPoint);
   bool success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, polygon, distance);
   cout << "Test 1. Target at: " << targetPoint.GetLat() << ", " << targetPoint.GetLon() << "headingRef: " << headingRef
      << " Shape type =  " << polygon.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << distance << endl << endl;

   // Target inside rectangle
   targetPoint.SetLocationLLA(0, 0, 0);
   targetPoint.GetLocationWCS(wcsPoint);
   success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, polygon, distance);
   cout << "Test 2. Target at: " << targetPoint.GetLat() << ", " << targetPoint.GetLon() << "headingRef: " << headingRef
      << " Shape type =  " << polygon.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << distance << endl << endl;

   // 6 vertices
   targetPoint.SetLocationLLA(0.07, 0.15, 0);
   points.clear();
   points.push_back(WsfGeoPoint(0.0, 0.0, 0.0));
   points.push_back(WsfGeoPoint(0.1, 0.0, 0.0));
   points.push_back(WsfGeoPoint(0.1, 0.1, 0.0));
   points.push_back(WsfGeoPoint(0.05, 0.15, 0.0));
   points.push_back(WsfGeoPoint(0.0, 0.1, 0.0));
   points.push_back(WsfGeoPoint(-0.05, 0.05, 0.0));
   polygon.SetPolyPoints(points);
   targetPoint.GetLocationWCS(wcsPoint);
   success = ZoneUtils::GetDistanceFromPointToZone(wcsPoint, polygon, distance);
   cout << "Test 3. Target at: " << targetPoint.GetLat() << ", " << targetPoint.GetLon() << "headingRef: " << headingRef
      << " Shape type =  " << polygon.GetShapeType()
      << " GetDistanceFromPointToZone returned " << success
      << ", distance = " << distance << endl << endl;
#endif
}

void TestProjection()
{
#ifndef JLKPLUGIN_TEMP_COMPILEWILLCRASH
   cout << "======================================================================================================" << endl;
   cout << "Test Projections" << endl;
   cout << "======================================================================================================" << endl;
   // 1. Projected outside of a polygon.
   // 1.a. 2 points

   WsfZoneDefinition polygon;
   double latRef = 0;
   double lonRef = 0;
   double headingRef = 0;
   polygon.SetRelativeType(WsfZoneDefinition::cINTERNAL);
   polygon.SetShapeType(WsfZoneDefinition::cPOLYGONAL);
   polygon.SetReference(latRef, lonRef, headingRef);
   WsfGeoPoint polygonRefPoint(latRef, lonRef, 0);
   std::vector<WsfGeoPoint> points;
   points.push_back(WsfGeoPoint(0.0, 0.0, 0.0));
   points.push_back(WsfGeoPoint(0.1, 0.0, 0.0));
   points.push_back(WsfGeoPoint(0.1, 0.1, 0.0));
   points.push_back(WsfGeoPoint(0.0, 0.1, 0.0));
   polygon.SetPolyPoints(points);

   WsfGeoPoint pos1(0.2, 0.05, 0); // straight above and in the middle of the rectangle
   double wcsPos1[3];
   pos1.GetLocationWCS(wcsPos1);
   WsfGeoPoint pos2(0.3, 0.05, 0); // higher than the first point, so segment should be out of polygon
   double wcsPos2[3];
   pos2.GetLocationWCS(wcsPos2);

   bool projInZone = ZoneUtils::isProjectedInsideZone2Points(polygon, wcsPos1, wcsPos2);
   TestAssert("Test 1.a. Start above polygon, second point outside polygon. Expect not in zone.", projInZone == false);

   // 1.b. Point & velocity
   double velocity[3];
   UtVec3d::Subtract(velocity, wcsPos2, wcsPos1);
   UtVec3d::Normalize(velocity, velocity);
   UtVec3d::Multiply(velocity, 100.0);
   projInZone = ZoneUtils::isProjectedInsideZonePosVel(polygon, wcsPos2, velocity);
   TestAssert("Test 1.b. Start above polygon, velocity carries it above polygon. Expect not in zone.", projInZone == false);

   // 1.c. Point & heading
   double heading = pos1.TrueBearingTo(pos2);
   projInZone = ZoneUtils::isProjectedInsideZonePointHeading(polygon, wcsPos2, heading);
   TestAssert("Test 1.c. Start above polygon, heading carries it above polygon. Expect not in zone.", projInZone == false);

   // 2. Projected into polygon
   // 2.a. 2 points
   WsfGeoPoint pointInsidePoly(0.06, 0.06, 0);
   double wcsPointInside[3];
   pointInsidePoly.GetLocationWCS(wcsPointInside);
   projInZone = ZoneUtils::isProjectedInsideZone2Points(polygon, wcsPointInside, wcsPos2);
   TestAssert("Test 2.a. Start above polygon, segment ends in polygon. Expect in zone.", projInZone == true);

   // 2.b. Point & velocity
   UtVec3d::Subtract(velocity, wcsPointInside, wcsPos2);
   UtVec3d::Normalize(velocity, velocity);
   UtVec3d::Multiply(velocity, 100.0);
   projInZone = ZoneUtils::isProjectedInsideZonePosVel(polygon, wcsPos2, velocity);
   TestAssert("Test 2.b. Start above polygon, velocity toward polygon. Expect in zone.", projInZone == true);

   // 2.c. Point & heading
   heading = pos1.TrueBearingTo(pointInsidePoly);
   projInZone = ZoneUtils::isProjectedInsideZonePointHeading(polygon, wcsPos2, heading);
   TestAssert("Test 2.c. Start above polygon, heading toward polygon. Expect in zone.", projInZone == true);

   // 3. Projected into circle
   WsfZoneDefinition circle;
   latRef = 30.0;
   lonRef = 30.0;
   headingRef = 0;
   WsfGeoPoint pointOutsideCircle(latRef + 3.0, lonRef, 0);
   WsfGeoPoint secondPointOutsideCircle(latRef + 3.0, lonRef + 1.0, 0);
   circle.SetRelativeType(WsfZoneDefinition::cINTERNAL);
   circle.SetShapeType(WsfZoneDefinition::cCIRCULAR);
   circle.SetReference(latRef, lonRef, headingRef);
   circle.SetRadialBounds(0, 50000);

   // 3.a. 2 points
   WsfGeoPoint pointInsideCircle(latRef + 0.1, lonRef + 0.1, 0);
   double wcsOutsideCircle[3];
   pointOutsideCircle.GetLocationWCS(wcsOutsideCircle);
   pointInsideCircle.GetLocationWCS(wcsPointInside);
   projInZone = ZoneUtils::isProjectedInsideZone2Points(circle, wcsPointInside, wcsOutsideCircle);
   TestAssert("Test 3.a. Segment ends in circle. Expect in zone.", projInZone == true);

   // 3.b. Point & velocity
   UtVec3d::Subtract(velocity, wcsPointInside, wcsOutsideCircle);
   UtVec3d::Normalize(velocity, velocity);
   UtVec3d::Multiply(velocity, 100.0);
   projInZone = ZoneUtils::isProjectedInsideZonePosVel(circle, wcsOutsideCircle, velocity);
   TestAssert("Test 3.b. Start above circle, velocity toward circle. Expect in zone.", projInZone == true);


   // 3.c. Point & heading
   heading = pointOutsideCircle.TrueBearingTo(wcsPointInside);
   projInZone = ZoneUtils::isProjectedInsideZonePointHeading(circle, wcsOutsideCircle, heading);
   TestAssert("Test 3.c. Start above circle, heading toward circle. Expect in zone.", projInZone == true);

   // 4. Projected outside circle
   // 4.a. 2 points
   double secondWcsOutsideCircle[3];
   secondPointOutsideCircle.GetLocationWCS(secondWcsOutsideCircle);
   projInZone = ZoneUtils::isProjectedInsideZone2Points(circle, wcsOutsideCircle, secondWcsOutsideCircle);
   TestAssert("Test 4.a. Start above circle, two points parallel to circle. Expect not in zone.", projInZone == false);

   // 4.b. Point & velocity
   UtVec3d::Subtract(velocity, wcsOutsideCircle, secondWcsOutsideCircle);
   UtVec3d::Normalize(velocity, velocity);
   UtVec3d::Multiply(velocity, 100.0);
   projInZone = ZoneUtils::isProjectedInsideZonePosVel(circle, wcsOutsideCircle, velocity);
   TestAssert("Test 4.b. Start above circle, velocity parallel to circle. Expect not in zone.", projInZone == false);

   // 4.c. Point & heading
   heading = pointOutsideCircle.TrueBearingTo(secondPointOutsideCircle);
   projInZone = ZoneUtils::isProjectedInsideZonePointHeading(circle, wcsOutsideCircle, heading);
   TestAssert("Test 4.c. Start above circle, heading parallel to circle. Expect not in zone.", projInZone == false);

   // 5. Projected into ellipse
   WsfZoneDefinition ellipse;
   latRef = 30.0;
   lonRef = 30.0;
   headingRef = 0;
   WsfGeoPoint pointOutsideEllipse(latRef + 3.0, lonRef, 0);
   double wcsOutsideEllipse[3];
   pointOutsideEllipse.GetLocationWCS(wcsOutsideEllipse);
   WsfGeoPoint pointInsideEllipse(latRef, lonRef, 0);
   double wcsInsideEllipse[3];
   pointInsideEllipse.GetLocationWCS(wcsInsideEllipse);
   WsfGeoPoint secondPointOutsideEllipse(latRef + 3.0, lonRef + 1.0, 0);
   double wcsSecondPointOutsideEllipse[3];
   secondPointOutsideEllipse.GetLocationWCS(wcsSecondPointOutsideEllipse);
   ellipse.SetRelativeType(WsfZoneDefinition::cINTERNAL);
   ellipse.SetShapeType(WsfZoneDefinition::cELLIPTICAL);
   ellipse.SetReference(latRef, lonRef, headingRef);
   ellipse.SetEllipticalAxes(10000, 50000);

   // 5.a. 2 points
   projInZone = ZoneUtils::isProjectedInsideZone2Points(ellipse, wcsOutsideEllipse, wcsInsideEllipse);
   TestAssert("Test 5.a. One point outside ellipse, one point inside. Expect in zone.", projInZone == true);

   // 5.b. Point & velocity
   UtVec3d::Subtract(velocity, wcsInsideEllipse, wcsOutsideEllipse);
   UtVec3d::Normalize(velocity, velocity);
   UtVec3d::Multiply(velocity, 100.0);
   projInZone = ZoneUtils::isProjectedInsideZonePosVel(ellipse, wcsOutsideEllipse, velocity);
   TestAssert("Test 5.b. Start above ellipse, velocity toward ellipse. Expect in zone.", projInZone == true);

   // 5.c. Point & heading
   heading = pointOutsideEllipse.TrueBearingTo(pointInsideEllipse);
   projInZone = ZoneUtils::isProjectedInsideZonePointHeading(ellipse, wcsOutsideEllipse, heading);
   TestAssert("Test 5.c. Start above ellipse, heading toward ellipse. Expect in zone.", projInZone == true);

   // 6. Projected outside ellipse
   // 6.a. 2 points
   projInZone = ZoneUtils::isProjectedInsideZone2Points(ellipse, wcsOutsideEllipse, wcsSecondPointOutsideEllipse);
   TestAssert("Test 6.a. Both points outside zone, parallel to zone. Expect not in zone.", projInZone == false);

   // 6.b. Point & velocity
   UtVec3d::Subtract(velocity, wcsOutsideEllipse, wcsSecondPointOutsideEllipse);
   UtVec3d::Normalize(velocity, velocity);
   UtVec3d::Multiply(velocity, 100.0);
   projInZone = ZoneUtils::isProjectedInsideZonePosVel(ellipse, wcsOutsideCircle, velocity);
   TestAssert("Test 6.b. Start above ellipse, velocity parallel to ellipse. Expect not in zone.", projInZone == false);

   // 6.c. Point & heading
   heading = pointOutsideEllipse.TrueBearingTo(secondPointOutsideEllipse);
   projInZone = ZoneUtils::isProjectedInsideZonePointHeading(ellipse, wcsOutsideEllipse, heading);
   TestAssert("Test 6.c. Start above ellipse, heading toward ellipse. Expect in zone.", projInZone == false);
#endif
}

void test_zone()
{
   TestCircle();
   TestEllipse();
   TestPolygon();
   TestProjection();
}
