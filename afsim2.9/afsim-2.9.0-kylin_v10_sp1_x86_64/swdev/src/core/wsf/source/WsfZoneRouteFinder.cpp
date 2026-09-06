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

#include "WsfZoneRouteFinder.hpp"

#include <limits>
#include <string>

#include "UtConvexHull.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfRoute.hpp"
#include "WsfScenario.hpp"
#include "WsfWaypoint.hpp"
#include "WsfZone.hpp"
#include "WsfZoneDefinition.hpp"
#include "script/WsfScriptContext.hpp"


WsfZoneRouteFinder::WsfZoneRouteFinder(WsfSimulation* aSimPtr)
   : mGeoStart()
   , mGeoEnd()
   , mStart()
   , mEnd()
   , mSafeStart()
   , mSafeEnd()
   , mDraw(*aSimPtr)
   , mSimulationPtr(aSimPtr)
   , mZoneGraph()
   , mAvoidZones()
   , mLocalZones()
   , mBestZonePathDist(0.0)
   , mBestZonePath()
   , mZonePathStack()
{
   mDraw.SetId(mDraw.GetNewId());
   mDraw.SetDuration(WsfDraw::cFOREVER);
   mDraw.SetEllipseMode(WsfDraw::cELLIPSE_LINE);
   mDraw.SetLineStyle(WsfDraw::cSOLID);
   mDraw.SetLineSize(2);
   mDraw.SetPointSize(4);
}

WsfZoneRouteFinder::WsfZoneRouteFinder(const WsfZoneRouteFinder& aSrc)
   : mGeoStart(aSrc.mGeoStart)
   , mGeoEnd(aSrc.mGeoEnd)
   , mStart(aSrc.mStart)
   , mEnd(aSrc.mEnd)
   , mSafeStart(aSrc.mSafeStart)
   , mSafeEnd(aSrc.mSafeEnd)
   , mDraw(aSrc.mDraw)
   , mSimulationPtr(aSrc.mSimulationPtr)
   , mZoneGraph(aSrc.mZoneGraph)
   , mAvoidZones(aSrc.mAvoidZones)
   , mLocalZones(aSrc.mLocalZones)
   , mBestZonePathDist(aSrc.mBestZonePathDist)
   , mBestZonePath(aSrc.mBestZonePath)
   , mZonePathStack(aSrc.mZonePathStack)
{
}

WsfZoneRouteFinder::~WsfZoneRouteFinder()
{
   for (auto& nodePtr : mZoneGraph)
   {
      delete nodePtr;
   }
}

std::unique_ptr<UtScriptClass> WsfZoneRouteFinder::CreateScriptClass(const std::string& aClassName,
                                                                     UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptZoneRouteFinderClass>(aClassName, aScriptTypesPtr);
}

void WsfZoneRouteFinder::ClearAvoidanceZones()
{
   mAvoidZones.clear();
}

void WsfZoneRouteFinder::ShortestZoneGraphPathRecursive(ZoneGraphNode* aNodePtr, UtVec2d& aTarget, double aDist)
{
   if (aDist > mBestZonePathDist) // already longer than previously found path, stop looking down this edge
   {
      return;
   }

   if (aNodePtr->mLoc == aTarget)
   {
      // we've reached the target node
      mBestZonePathDist = aDist;
      mBestZonePath     = mZonePathStack;
      return;
   }

   for (auto* nextPtr : aNodePtr->mEdges)
   {
      if (!nextPtr->mVisited)
      {
         nextPtr->mVisited = true;
         mZonePathStack.push_back(nextPtr);
         UtVec2d edge   = aNodePtr->mLoc - nextPtr->mLoc;
         double  length = edge.Magnitude();
         ShortestZoneGraphPathRecursive(nextPtr, aTarget, aDist + length);
         mZonePathStack.pop_back();
         nextPtr->mVisited = false;
      }
   }
}

bool WsfZoneRouteFinder::Intersection(const Line2d& line1, const Line2d& line2, UtVec2d& point)
{
   UtVec2d vec1 = line1.IntoVec();
   UtVec2d vec2 = line2.IntoVec();

   double s, t;
   double C = UtVec2d::CrossProduct(vec1, vec2);
   if (C == 0.0)
   {
      // line segments are parallel
      return false;
   }
   s = (-vec1[1] * (line1.mBeg[0] - line2.mBeg[0]) + vec1[0] * (line1.mBeg[1] - line2.mBeg[1])) / C;
   t = (vec2[0] * (line1.mBeg[1] - line2.mBeg[1]) - vec2[1] * (line1.mBeg[0] - line2.mBeg[0])) / C;

   if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
   {
      // line segments intersect, populate the point
      point[0] = line1.mBeg[0] + (t * vec1[0]);
      point[1] = line1.mBeg[1] + (t * vec1[1]);
      return true;
   }
   return false;
}

double WsfZoneRouteFinder::ClosestPointOnLineSegment(const UtVec2d& aPoint,
                                                     const UtVec2d& aLine1,
                                                     const UtVec2d& aLine2,
                                                     UtVec2d&       aClosest)
{
   UtVec2d line = aLine2 - aLine1;
   double  lnSq = line.MagnitudeSquared();
   if (lnSq == 0.0)
   {
      // line is a point
      aClosest = aLine1;
   }
   else
   {
      // consider the entire line (not just segment) modeled as "l1 + t (l2-l1)"
      // find projection of aPoint onto that line, it is at:  t = ((p-l1).(l2-l1)) / (|l2-l1|)^2
      UtVec2d temp = aPoint - aLine1;
      double  t    = UtVec2d::DotProduct(temp, line) / lnSq;
      if (t < 0.0)
      {
         // point is beyond l1
         aClosest = aLine1;
      }
      else if (t > 1.0)
      {
         // point is beyond l2
         aClosest = aLine2;
      }
      else
      {
         // point is on the segment
         aClosest = aLine1 + (t * line);
      }
   }
   return (aPoint - aClosest).Magnitude();
}

void WsfZoneRouteFinder::DrawPolygon(double aDuration, const UtVec3d& aColor, std::vector<UtVec2d>& aPolygon)
{
   if (aPolygon.empty())
   {
      return;
   }
   mDraw.SetDuration(aDuration);
   mDraw.SetColor(aColor[0], aColor[1], aColor[2]);
   mDraw.BeginLines();
   double  wcs1[3], wcs2[3];
   UtVec2d vec1 = aPolygon[aPolygon.size() - 1];
   for (const UtVec2d& vec2 : aPolygon)
   {
      GetGlobal(vec1, wcs1);
      GetGlobal(vec2, wcs2);
      mDraw.VertexWCS(wcs1);
      mDraw.VertexWCS(wcs2);
      vec1 = vec2;
   }
   mDraw.End();
}

void WsfZoneRouteFinder::DrawGraph(double aDuration, const UtVec3d& aColor)
{
   mDraw.SetDuration(aDuration);
   mDraw.SetColor(aColor[0], aColor[1], aColor[2]);
   double wcs1[3], wcs2[3];
   for (ZoneGraphNode* nodePtr : mZoneGraph)
   {
      GetGlobal(nodePtr->mLoc, wcs1);
      mDraw.BeginLines();
      for (auto& edge : nodePtr->mEdges)
      {
         GetGlobal(edge->mLoc, wcs2);
         mDraw.VertexWCS(wcs1);
         mDraw.VertexWCS(wcs2);
      }
      mDraw.End();
   }
}

void WsfZoneRouteFinder::DrawAvoidances(double aDuration, const UtVec3d& aColor)
{
   mDraw.SetDuration(aDuration);
   mDraw.SetColor(aColor[0], aColor[1], aColor[2]);
   std::vector<WsfZoneDefinition*>::iterator it;
   for (it = mAvoidZones.begin(); it != mAvoidZones.end(); ++it)
   {
      (*it)->Draw(mDraw);
   }
}

void WsfZoneRouteFinder::AvoidZone(WsfZone* aZonePtr)
{
   // See if the zone is already in mAvoidZones
   auto found = std::find(mAvoidZones.begin(), mAvoidZones.end(), aZonePtr);

   if (found == mAvoidZones.end())
   {
      WsfZoneDefinition* zonePtr = dynamic_cast<WsfZoneDefinition*>(aZonePtr);
      if (zonePtr && zonePtr->GetShapeType() == WsfZoneDefinition::cPOLYGONAL &&
          zonePtr->GetRelativeType() == WsfZoneDefinition::cINTERNAL && zonePtr->PointsAreLatLon() &&
          zonePtr->GetPoints().size() > 1)
      {
         mAvoidZones.push_back(zonePtr);
      }
      else
      {
         auto out = ut::log::warning() << "Unable to avoid zone.";
         out.AddNote() << "Zone: " << aZonePtr->GetName();
         out.AddNote() << "It is not a polygonal zone with lat-lon points.";
      }
   }
}

const WsfZone* WsfZoneRouteFinder::GetAvoidanceZone(size_t aIndex) const noexcept
{
   if (aIndex < mAvoidZones.size())
   {
      return mAvoidZones[aIndex];
   }
   return nullptr;
}

UtVec2d WsfZoneRouteFinder::NearestSafePoint(const UtVec2d& aPt)
{
   // pseudo code - algorithm:
   //   while (current point is in a zone)
   //     add zone points to convex hull
   //     shift current point to outside of convex hull
   bool                 inAZone = false;
   std::vector<UtVec2d> avoidPoints;
   std::vector<UtVec2d> hullPoints;

   bool useConvexHull = false; // do not use a convex hull until more than one avoidance zone is being avoided

   // when finding points on a line, once in a while floating point rounding errors will
   // give a point that is ever so slightly still included inside the polygon
   // this std::set is to avoiding checking that case repeatedly

   // std::set<int> avoiding;


   for (int i = 0; i < (int)mAvoidZones.size(); ++i)
   {
      WsfZoneDefinition* zonePtr = mAvoidZones[i];
      if (zonePtr->WithinPolygonSides(aPt[1], aPt[0]))
      {
         useConvexHull = (inAZone == true); // will be true for 2nd zone
         inAZone       = true;
         avoidPoints.insert(avoidPoints.end(), mLocalZones[i].begin(), mLocalZones[i].end());
         // avoiding.insert(i);
      }
   }

   UtVec2d closest = aPt;
   while (inAZone == true)
   {
      if (useConvexHull == true)
      {
         std::vector<UtVec2d> tempPoints(avoidPoints);
         hullPoints = UtConvexHull::ConvexHull(
            tempPoints); // this method could change the input argument, so we use a throw away vector
         // DrawPolygon(10, UtVec3d(0.75, 0, 0), hullPoints);
      }
      else
      {
         hullPoints = avoidPoints;
      }

      double shortestDist = std::numeric_limits<double>::max();
      for (size_t i = 0; i < hullPoints.size(); ++i)
      {
         UtVec2d tempPt;
         size_t  prev = (i == 0) ? (hullPoints.size() - 1) : (i - 1);
         double  dist = ClosestPointOnLineSegment(aPt, hullPoints[prev], hullPoints[i], tempPt);
         if (dist < shortestDist)
         {
            shortestDist = dist;
            closest      = tempPt;
         }
      }

      // nudge closest away from aPt, just a tad
      // this gets it off the polygon line and safe from future inclusion checks
      UtVec2d nudge = (closest - aPt);         // direction of nudge
      nudge *= (0.000001 / nudge.Magnitude()); // approximately 10 meters if X/Y is lon/lat
      closest += nudge;

      // std::vector<UtVec2d> pathOut;
      // pathOut.push_back(aPt);
      // pathOut.push_back(closest);
      // DrawPolygon(10, UtVec3d(0, 0, 0.75), pathOut);

      inAZone = false;
      for (int i = 0; i < (int)mAvoidZones.size(); ++i)
      {
         // if (avoiding.find(i) == avoiding.end())
         {
            WsfZoneDefinition* zonePtr = mAvoidZones[i];
            if (zonePtr->WithinPolygonSides(closest[1], closest[0])) // prevent same zone from containing the point again???
            {
               inAZone       = true;
               useConvexHull = true; // avoiding at least two or more zones here, use the hull
               avoidPoints.insert(avoidPoints.end(), mLocalZones[i].begin(), mLocalZones[i].end());
               // avoiding.insert(i);
               break;
            }
         }
      }
   }

   return closest;
}

WsfRoute* WsfZoneRouteFinder::RouteAroundZones(const WsfGeoPoint& aFrom, const WsfGeoPoint& aTo, double aSpeed)
{
   mLocalZones.clear();

   // create local (2D) representations of all avoidance zones
   for (WsfZoneDefinition* zonePtr : mAvoidZones)
   {
      // known to be a polygonal zone, with lat/lon coordinates if here
      std::vector<WsfGeoPoint> geoPoints;
      std::vector<UtVec2d>     locPoints;
      zonePtr->GetPoints(geoPoints);
      for (auto& geoPoint : geoPoints)
      {
         UtVec2d loc = GetLocal(geoPoint);
         locPoints.push_back(loc);
      }
      mLocalZones.push_back(locPoints);
   }

   mGeoStart  = aFrom;
   mStart     = GetLocal(mGeoStart);
   mSafeStart = NearestSafePoint(mStart);

   mGeoEnd  = aTo;
   mEnd     = GetLocal(mGeoEnd);
   mSafeEnd = NearestSafePoint(mEnd);

   // create "empty" non-connected graph
   for (auto& nodePtr : mZoneGraph)
   {
      delete nodePtr;
   }
   mZoneGraph.clear();
   mZoneGraph.push_back(new ZoneGraphNode(mSafeStart)); // start location
   mZoneGraph.push_back(new ZoneGraphNode(mSafeEnd));   // target location

   for (const auto& zone : mLocalZones)
   {
      for (const auto& point : zone)
      {
         mZoneGraph.push_back(new ZoneGraphNode(point));
      }
   }

   // connect all the nodes (where possible) of the search graph
   for (int i = 0; i < (int)mZoneGraph.size(); ++i)
   {
      // check if current graph node (at i) is connected to any other graph nodes (at i+1 to N)
      for (int j = i + 1; j < (int)mZoneGraph.size(); ++j)
      {
         ZoneGraphNode* aPtr = mZoneGraph[i];
         ZoneGraphNode* bPtr = mZoneGraph[j];
         Line2d         line(aPtr->mLoc, bPtr->mLoc);
         // this does not check if the line is completely included inside of a zone
         // not an issue yet, but could be a TODO
         if (Intersects(line, mLocalZones) == false)
         {
            // graph nodes can be connected; line between does not cross any avoidance zones
            aPtr->mEdges.push_back(bPtr);
            bPtr->mEdges.push_back(aPtr);
         }
      }
   }

   ZoneGraphNode* startPtr = mZoneGraph[0];
   mBestZonePathDist       = std::numeric_limits<double>::max();
   mBestZonePath.clear();
   mZonePathStack.clear();
   mZonePathStack.push_back(startPtr); // seed it

   ShortestZoneGraphPathRecursive(startPtr, mSafeEnd, 0.0);

   WsfRoute* shortestRoutePtr = new WsfRoute();

   if (!mBestZonePath.empty())
   {
      // convert the mBestZonePath to a WsfRoute
      // scale altitude for a constant climb/dive across the whole route
      double endAlt = mGeoEnd.GetAlt();
      double delta  = endAlt - mGeoStart.GetAlt();
      double length = mGeoStart.GetDistanceFrom(mGeoEnd);
      for (ZoneGraphNode* nodePtr : mBestZonePath)
      {
         WsfGeoPoint pt1 = GetGlobal(nodePtr->mLoc);
         // scale alt based on "distance along route" towards end
         double ptAlt = endAlt - ((pt1.GetDistanceFrom(mGeoEnd) / length) * delta);
         pt1.SetAltitudeMSL(ptAlt);
         WsfWaypoint wpt1(pt1.GetLat(), pt1.GetLon(), pt1.GetAlt(), aSpeed);
         shortestRoutePtr->Append(wpt1);
      }
   }
   return shortestRoutePtr;
}

bool WsfZoneRouteFinder::Intersects(const Line2d& aLine, std::vector<UtVec2d> aPolygon)
{
   if (aPolygon.size() < 2)
   {
      return false;
   }
   UtVec2d point; // stub variable
   for (size_t i = 0; i < aPolygon.size(); ++i)
   {
      size_t  next = (i == aPolygon.size() - 1) ? (0) : (i + 1);
      size_t  prev = (i == 0) ? (aPolygon.size() - 1) : (i - 1);
      UtVec2d a    = aPolygon[i];    // origin
      UtVec2d b    = aPolygon[next]; // next leg
      UtVec2d c    = aPolygon[prev]; // prev leg
      Line2d  polyLine(a, b);

      // if the lines touch at one end, we don't consider that an intersection with the zone,
      if (a == aLine.mBeg || a == aLine.mEnd)
      {
         // do not check line-over-line intersection, the lines share an end-point
         // check polygon inclusion though
         // only check in reference to polyline's first point, the next polyline will check the next point
         UtVec2d d = (a == aLine.mBeg) ? (aLine.mEnd) : (aLine.mBeg);
         b -= a;
         c -= a;
         d -= a;
         double cb = c.CrossProduct(b);
         double bd = b.CrossProduct(d);
         double cd = c.CrossProduct(d);
         if ((cb > 0 && cd > 0 && bd < 0) || (cb < 0 && (cd > 0 || bd < 0)))
         {
            // line is inside polygon
            return true;
         }
         else
         {
            // line is outside polygon
            continue;
         }
      }
      else if (b == aLine.mBeg || b == aLine.mEnd)
      {
         // do not check line-over-line intersection, the lines share an end-point
         // let next polyline check polygon inclusion
      }
      else if (Intersection(aLine, polyLine, point))
      {
         return true;
      }
   }
   return false;
}

bool WsfZoneRouteFinder::Intersects(const Line2d& aLine, std::vector<std::vector<UtVec2d>> aPolygons)
{
   std::vector<std::vector<UtVec2d>>::iterator pit;
   for (pit = aPolygons.begin(); pit != aPolygons.end(); ++pit)
   {
      if (Intersects(aLine, *pit))
      {
         return true;
      }
   }
   return false;
}

UtVec2d WsfZoneRouteFinder::GetLocal(const WsfGeoPoint& pt)
{
   return UtVec2d(pt.GetLon(), pt.GetLat());
}

void WsfZoneRouteFinder::GetGlobal(const UtVec2d& pt, double wcs[3])
{
   WsfGeoPoint geo = GetGlobal(pt);
   geo.GetLocationWCS(wcs);
}

WsfGeoPoint WsfZoneRouteFinder::GetGlobal(const UtVec2d& pt)
{
   return WsfGeoPoint(pt[1], pt[0], mGeoStart.GetAlt());
}

WsfScriptZoneRouteFinderClass::WsfScriptZoneRouteFinderClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mConstructible = true;
   mCloneable     = true;

   SetClassName("WsfZoneRouteFinder");

   AddMethod(ut::make_unique<AvoidZone_1>("AvoidZone")); // AvoidZone(WsfZone zone);
   AddMethod(ut::make_unique<AvoidZone_2>("AvoidZone")); // AvoidZone(string zoneName);
   AddMethod(ut::make_unique<NumberOfAvoidanceZones>()); // NumberOfAvoidanceZones();
   AddMethod(ut::make_unique<GetAvoidanceZone>());       // GetAvoidanceZone(int index);
   AddMethod(ut::make_unique<RouteAroundZones>()); // RouteAroundZones(WsfGeoPoint beg, WsfGeoPoint end, double speed);
   AddMethod(ut::make_unique<ClearAvoidanceZones>());
   AddMethod(ut::make_unique<DrawGraph>());
   AddMethod(ut::make_unique<DrawAvoidances>());
}

WsfScriptZoneRouteFinderClass::~WsfScriptZoneRouteFinderClass() {}

// virtual
void* WsfScriptZoneRouteFinderClass::Create(const UtScriptContext& aContext)
{
   return static_cast<void*>(new WsfZoneRouteFinder(WsfScriptContext::GetSIMULATION(aContext)));
}

// virtual
void* WsfScriptZoneRouteFinderClass::Clone(void* aObjectPtr)
{
   WsfZoneRouteFinder* objPtr = static_cast<WsfZoneRouteFinder*>(aObjectPtr);
   assert(objPtr != nullptr);
   return objPtr->Clone();
}

// virtual
void WsfScriptZoneRouteFinderClass::Destroy(void* aObjectPtr)
{
   WsfZoneRouteFinder* objPtr = static_cast<WsfZoneRouteFinder*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneRouteFinderClass, WsfZoneRouteFinder, AvoidZone_1, 1, "void", "WsfZone")
{
   WsfZone* zonePtr = static_cast<WsfZone*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->AvoidZone(zonePtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneRouteFinderClass, WsfZoneRouteFinder, AvoidZone_2, 1, "void", "string")
{
   std::string zoneName = aVarArgs[0].GetString();
   WsfZone*    zonePtr  = dynamic_cast<WsfZone*>(WsfScriptContext::GetSCENARIO(aContext)->FindType("zone", zoneName));
   if (zonePtr != nullptr)
   {
      aObjectPtr->AvoidZone(zonePtr);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneRouteFinderClass, WsfZoneRouteFinder, NumberOfAvoidanceZones, 0, "int", "")
{
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->NumberOfAvoidanceZones()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneRouteFinderClass, WsfZoneRouteFinder, GetAvoidanceZone, 1, "WsfZone", "int")
{
   auto zonePtr = aObjectPtr->GetAvoidanceZone(ut::cast_to_size_t(aVarArgs[0].GetInt()));
   aReturnVal.SetPointer(UtScriptRef::Ref(zonePtr->Clone(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneRouteFinderClass,
                        WsfZoneRouteFinder,
                        RouteAroundZones,
                        3,
                        "WsfRoute",
                        "WsfGeoPoint, WsfGeoPoint, double")
{
   WsfGeoPoint* srcPtr      = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfGeoPoint* tgtPtr      = static_cast<WsfGeoPoint*>(aVarArgs[1].GetPointer()->GetAppObject());
   double       speed       = aVarArgs[2].GetDouble();
   WsfRoute*    solutionPtr = aObjectPtr->RouteAroundZones(*srcPtr, *tgtPtr, speed);
   aReturnVal.SetPointer(solutionPtr->ScriptRefManage());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneRouteFinderClass, WsfZoneRouteFinder, ClearAvoidanceZones, 0, "void", "")
{
   aObjectPtr->ClearAvoidanceZones();
}


UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneRouteFinderClass, WsfZoneRouteFinder, DrawGraph, 2, "void", "double, Vec3")
{
   double   duration = aVarArgs[0].GetDouble();
   UtVec3d* colorPtr = static_cast<UtVec3d*>(aVarArgs[1].GetPointer()->GetAppObject());
   aObjectPtr->DrawGraph(duration, *colorPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneRouteFinderClass, WsfZoneRouteFinder, DrawAvoidances, 2, "void", "double, Vec3")
{
   double   duration = aVarArgs[0].GetDouble();
   UtVec3d* colorPtr = static_cast<UtVec3d*>(aVarArgs[1].GetPointer()->GetAppObject());
   aObjectPtr->DrawAvoidances(duration, *colorPtr);
}
