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

#ifndef WSFROUTEFINDER_HPP
#define WSFROUTEFINDER_HPP

#include <limits>
#include <map>
#include <memory>
#include <set>
#include <string>

#include "UtEntity.hpp"
#include "UtScriptClassDefine.hpp"
#include "UtVec2.hpp"
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfObject.hpp"
class WsfPlatform;
#include "WsfRoute.hpp"

class WSF_EXPORT WsfRouteFinder : public WsfObject
{
public:
   // defines behavior for when starting point or targeted point are contained inside an avoidance region
   enum ImpossibleRouteResponse
   {
      SHRINK_AVOIDANCES            = 1,
      SHIFT_OUTSIDE_AVOIDANCES     = 2,
      IGNORE_IMPOSSIBLE_AVOIDANCES = 3
   };

   enum OrientationAround
   {
      CLOCKWISE        = 0x01,
      COUNTERCLOCKWISE = 0x02,
      NEITHER          = 0x03
   };

   WsfRouteFinder(WsfSimulation* aSimulationPtr);
   WsfRouteFinder(const WsfRouteFinder& aSrc);
   WsfRouteFinder& operator=(const WsfRouteFinder&) = delete;
   ~WsfRouteFinder() override;

   WsfRouteFinder* Clone() const override { return new WsfRouteFinder(*this); }

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void Avoid(WsfPlatform& aPlatform, double aRadius);
   void Avoid(const WsfGeoPoint& aPoint, double aRadius);
   void ClearAvoidances();

   WsfRoute* Route(double aTime, const WsfGeoPoint& aFrom, const WsfGeoPoint& aTo, double speed, int branchIndex = 0);
   // returns the avoidances relevant for the last computed route (1 avoidance per leg of 2 route points)
   WsfRoute* RouteAvoidances();

   void DrawRoute(double duration, const UtVec3d& color);
   void DrawGraph(double duration, const UtVec3d& color);
   void DrawAvoidances(double duration, const UtVec3d& color);

   ImpossibleRouteResponse GetImpossibleRouteResponse(void) { return mImpossibleRouteResponse; }
   void SetImpossibleRouteResponse(ImpossibleRouteResponse aResponse) { mImpossibleRouteResponse = aResponse; }
   void SetMaxArcLength(double aArcLength) { mMaxArcLength = aArcLength; }
   WsfSimulation* GetSimulation() const { return mSimulationPtr; }

private:
   class Line2d
   {
   public:
      Line2d()
         : mBeg()
         , mEnd()
      {
      }
      Line2d(const UtVec2d& a, const UtVec2d& b)
         : mBeg(a)
         , mEnd(b)
      {
      }

      UtVec2d IntoVec() const { return (mEnd - mBeg); }

      UtVec2d mBeg;
      UtVec2d mEnd;
   };

   class Circle2d
   {
   public:
      Circle2d()
         : mLoc()
         , mRadius(0.0)
      {
      }
      Circle2d(const UtVec2d& aPoint, double aRadius)
         : mLoc(aPoint)
         , mRadius(aRadius)
      {
      }

      UtVec2d mLoc;
      double  mRadius;
   };

   class Avoidance
   {
   public:
      Avoidance(const UtVec2d& aLocation, double aRadius, double aDistance, const WsfGeoPoint& aOrigPt, unsigned short aID)
         : mCircle(aLocation, aRadius)
         , mDistance(aDistance)
         , mID(aID)
         , mOriginalPoint(aOrigPt)
      {
         mNear = mDistance - mCircle.mRadius;
         mFar  = mDistance + mCircle.mRadius;
      }
      Avoidance(const UtVec2d& aLocation, unsigned short aID = 65535)
         : mCircle(aLocation, 0)
         , mDistance(0)
         , mNear(0)
         , mFar(0)
         , mID(aID)
         , mOriginalPoint()
      {
      }

      void SetRadius(double aRadius)
      {
         mCircle.mRadius = aRadius;
         mNear           = mDistance - mCircle.mRadius;
         mFar            = mDistance + mCircle.mRadius;
      }

      bool operator<(const Avoidance& aRhs) const { return (mNear < aRhs.mNear); }

      bool operator==(const Avoidance& aRhs) const
      {
         return (mCircle.mLoc.Equals(aRhs.mCircle.mLoc) && (mCircle.mRadius == aRhs.mCircle.mRadius));
      }

      Circle2d mCircle;

      double mDistance;
      double mNear;
      double mFar;

      unsigned short mID;

      WsfGeoPoint mOriginalPoint;

      std::vector<Avoidance*> mOverlaps;
   };

   class GraphBranch;
   class GraphNode
   {
   public:
      GraphNode(Avoidance* aAvoidPtr)
         : mAvoidPtr(aAvoidPtr)
         , mBranches()
      {
      }
      ~GraphNode()
      {
         for (std::vector<GraphBranch*>::iterator it = mBranches.begin(); it != mBranches.end(); ++it)
         {
            delete *it;
         }
      }

      Avoidance*                mAvoidPtr; // not responsible to clean this up, class owns the object
      std::vector<GraphBranch*> mBranches;
   };

   class GraphBranch
   {
   public:
      GraphBranch(const Line2d& aLine, GraphNode* aNodePtr)
         : mFromPoint(aLine.mBeg)
         , mToPoint(aLine.mEnd)
         , mNodePtr(aNodePtr)
         , mVisited(false)
      {
      }
      GraphBranch(const UtVec2d& aFromPoint, const UtVec2d& aToPoint, GraphNode* aNodePtr)
         : mFromPoint(aFromPoint)
         , mToPoint(aToPoint)
         , mNodePtr(aNodePtr)
         , mVisited(false)
      {
      }

      OrientationAround Orientation();
      UtVec2d           mFromPoint; // point of this node
      UtVec2d           mToPoint;   // point of this node
      GraphNode*        mNodePtr;   // adjacent avoidance to this node point
      // do not delete this on deconstruction,
      // branch instances are owned by nodes, so nodes clean up
      bool mVisited;
   };


   //! @name algorithms for generating graph & searching it for best route
   //@{
   GraphNode*                GenerateGraph();
   std::vector<GraphBranch*> FindBestGraphPath();                          // searches mGraphNodes
   std::vector<GraphBranch*> FindGraphPathUsingBranch(unsigned int index); // searches mGraphNodes down branch at index
                                                                           // -> index 0 = best branch, index 1 = 2nd
                                                                           // best branch, etc...
   void FindBestGraphPathRecursive(GraphNode*        next,
                                   UtVec2d&          prev,
                                   double            dist                = 0,
                                   OrientationAround enteringOrientation = NEITHER);
   //@}


   //! @name utility methods for route generation or debugging
   //@{
   //! for converting between global 3D WCS coordinates and local 2D flat coordinates
   //! east  = positive X axis,   north = positive Y axis
   UtVec2d     GetLocal(const double wcs[3]);
   UtVec2d     GetLocal(const WsfGeoPoint& pt);
   void        GetGlobal(const UtVec2d& pt, double wcs[3]);
   WsfGeoPoint GetGlobal(const UtVec2d& pt);
   //! render a circle using the class draw object
   void DrawCircle(const WsfGeoPoint& center, double radius);
   //! tests if the circle contains the point
   bool Contains(const Circle2d& circle, const UtVec2d& point);
   //! tests if the larger circle contains the smaller circle
   bool Contains(const Circle2d& a, const Circle2d& b);
   //! tests if the two circles intersect each other
   bool Intersects(const Circle2d& a, const Circle2d& b);
   //! tests if the line segment intersect the circle
   bool Intersects(const Line2d& line, const Circle2d& circle);
   //! tests if the two line segments intersect each other
   bool Intersection(const Line2d& line1, const Line2d& line2, UtVec2d& point);
   //! finds the smallest arc length between points along a circle edge
   double ArcLength(const Circle2d& circle, const UtVec2d& fromPt, const UtVec2d& toPt);
   //! finds the arc length between points along a circle edge, traveling in the 'clockwise' direction
   double ArcLength(const Circle2d& circle, const UtVec2d& fromPt, const UtVec2d& toPt, OrientationAround orientation);
   //! finding tangent lines (between circles &/or points)
   bool OuterTangents(const Circle2d& a, const Circle2d& b, Line2d& cw, Line2d& ccw, bool onEdge = false);
   bool InnerTangents(const Circle2d& a, const Circle2d& b, Line2d& cw, Line2d& ccw, bool onEdge = false);
   std::pair<UtVec2d, UtVec2d> TangentPoints(const UtVec2d& src, const Circle2d& circle, bool onEdge = false);
   //! testing geometry against all the avoidance regions
   std::vector<Avoidance*> GetAvoidancesIntersectedBy(const Line2d& aLine);
   std::vector<Avoidance*> GetAvoidancesContaining(const UtVec2d& aPoint);
   bool                    IntersectsAnAvoidance(const Line2d& aLine);
   bool                    LocalInsertAvoidance(const Avoidance& avoid);
   //@}

   //! tunable parameter to determine how dense or spare a route is with route points when traveling around an avoidance arc
   double mMaxArcLength; // default: max double value

   //! decides what to do with degenerate starting locations or target locations
   ImpossibleRouteResponse mImpossibleRouteResponse;
   //! small value to use when shifting points off of tangent lines, etc...
   double mSmallFudge;
   double mVerySmallFudge;
   double mPercentAvoidancePad;

   //! dirty bit set to true whenever something to avoid was added
   bool mAvoidancesChanged;
   //! keeps track of the time the last route was found
   double mCurrentRouteTime;
   //! utility for holding last route index used in search
   int mLastRouteIndex;
   //! last computed route, at time mCurrentRouteTime
   WsfRoute mRoute;
   WsfRoute mRouteAvoidances;
   //! utility variable holding route reference point
   UtEntity mReferenceEntity;
   //! utility variables holding source & target locations for last route search
   WsfGeoPoint mGeoSource;
   WsfGeoPoint mGeoTarget;
   UtVec2d     mSource; // actual starting point
   UtVec2d     mFirst;  // first valid point outside of avoidances to start route finding from
   UtVec2d     mTarget;
   Avoidance*  mSourceAvoidancePtr;
   Avoidance*  mTargetAvoidancePtr;
   //! for drawing
   WsfDraw mDraw;
   //! keeps record of platforms to avoid
   std::map<size_t, double> mAvoidPlatformIdRadius;
   //! keeps record of static locations to avoid
   std::vector<std::pair<WsfGeoPoint, double>> mAvoidLocationRadius;
   //! snapshots of all avoidances for current route generation
   std::vector<Avoidance> mAvoidances;
   unsigned short         mNextAvoidanceId;
   //! private variables for the graph generation & searching algorithms
   double                    mBestPathDist;
   std::vector<GraphNode*>   mGraphNodes;
   std::vector<GraphBranch*> mBestPath; // do not need to delete this on deconstruction, objects owned by mGraphNodes
   std::vector<GraphBranch*> mGraphPathStack; // do not need to delete this on deconstruction, objects owned by mGraphNodes
   WsfSimulation*            mSimulationPtr;
};

//! Define script methods for WsfRouteFinder.
class WSF_EXPORT WsfScriptRouteFinderClass : public UtScriptClass
{
public:
   WsfScriptRouteFinderClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~WsfScriptRouteFinderClass() override;

   void* Create(const UtScriptContext& aContext) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Avoid_1);
   UT_DECLARE_SCRIPT_METHOD(Avoid_2);
   UT_DECLARE_SCRIPT_METHOD(ClearAvoidances);
   UT_DECLARE_SCRIPT_METHOD(Route_1);
   UT_DECLARE_SCRIPT_METHOD(Route_2);
   UT_DECLARE_SCRIPT_METHOD(RouteAvoidances);
   UT_DECLARE_SCRIPT_METHOD(DrawRoute);
   UT_DECLARE_SCRIPT_METHOD(DrawGraph);
   UT_DECLARE_SCRIPT_METHOD(DrawAvoidances);
   UT_DECLARE_SCRIPT_METHOD(ImpossibleRouteResponse);
   UT_DECLARE_SCRIPT_METHOD(SetImpossibleRouteResponse);
   UT_DECLARE_SCRIPT_METHOD(SetMaxArcLength);
};

#endif
