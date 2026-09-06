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

#ifndef WSFZONEROUTEFINDER_HPP
#define WSFZONEROUTEFINDER_HPP

#include <memory>

#include "UtScriptClassDefine.hpp"
#include "UtVec2.hpp"
#include "UtVec3.hpp" //included in WsfDraw.hpp, but also here for completeness
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfObject.hpp"
class WsfRoute;
class WsfZone;
class WsfZoneDefinition;


class WSF_EXPORT WsfZoneRouteFinder : public WsfObject
{
public:
   WsfZoneRouteFinder(WsfSimulation* aSimulationPtr);
   WsfZoneRouteFinder(const WsfZoneRouteFinder& aSrc);
   ~WsfZoneRouteFinder() override;
   WsfZoneRouteFinder& operator=(const WsfZoneRouteFinder&) = delete;

   WsfZoneRouteFinder* Clone() const override { return new WsfZoneRouteFinder(*this); }

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void ClearAvoidanceZones();

   void DrawGraph(double aDuration, const UtVec3d& aColor);
   void DrawAvoidances(double aDuration, const UtVec3d& aColor);

   WsfSimulation* GetSimulation() const { return mSimulationPtr; }

   void AvoidZone(WsfZone* aZonePtr);
   //! Get avoidance zone
   //! @param aIndex is the index of the avoidance zone
   //! @return the entry at the specified index in the list of avoidance zones
   const WsfZone* GetAvoidanceZone(size_t aIndex) const noexcept;
   //! Get the number of avoidance zones
   //! @return the number of avoidance zones
   size_t    NumberOfAvoidanceZones() const noexcept { return mAvoidZones.size(); }
   WsfRoute* RouteAroundZones(const WsfGeoPoint& aFrom, const WsfGeoPoint& aTo, double aSpeed);

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

   class ZoneGraphNode
   {
   public:
      ZoneGraphNode() = delete;
      ZoneGraphNode(const UtVec2d& aLocation)
         : mVisited(false)
         , mLoc(aLocation)
         , mEdges()
      {
      }

      bool                        mVisited;
      UtVec2d                     mLoc;
      std::vector<ZoneGraphNode*> mEdges;
   };

   //! @name utility methods for route generation or debugging
   //@{
   //! for converting between global 3D WCS coordinates and local 2D flat coordinates
   //! east  = positive X axis,   north = positive Y axis
   UtVec2d     GetLocal(const WsfGeoPoint& pt);
   void        GetGlobal(const UtVec2d& pt, double wcs[3]);
   WsfGeoPoint GetGlobal(const UtVec2d& pt);

   //! tests if the two line segments intersect each other
   bool Intersection(const Line2d& line1, const Line2d& line2, UtVec2d& point);

   // returns distance to closest point
   double ClosestPointOnLineSegment(const UtVec2d& aPoint, const UtVec2d& aLine1, const UtVec2d& aLine2, UtVec2d& aClosest);
   //@}


   bool Intersects(const Line2d& aLine, std::vector<UtVec2d> aPolygon);
   bool Intersects(const Line2d& aLine, std::vector<std::vector<UtVec2d>> aPolygons);

   UtVec2d NearestSafePoint(const UtVec2d& aPt);

   void DrawPolygon(double aDuration, const UtVec3d& aColor, std::vector<UtVec2d>& aPolygon);

   void ShortestZoneGraphPathRecursive(ZoneGraphNode* aNodePtr, UtVec2d& aTarget, double aDist = 0);

   //! utility variables holding source & target locations for last route search
   WsfGeoPoint mGeoStart;
   WsfGeoPoint mGeoEnd;
   UtVec2d     mStart;
   UtVec2d     mEnd;

   UtVec2d mSafeStart; // first valid point outside of avoidances to start route finding from
   UtVec2d mSafeEnd;   // first valid point outside of avoidances to end route finding to

   //! for drawing
   WsfDraw        mDraw;
   WsfSimulation* mSimulationPtr;

   //! private variables for the graph generation & searching algorithms

   std::vector<ZoneGraphNode*>       mZoneGraph;
   std::vector<WsfZoneDefinition*>   mAvoidZones;
   std::vector<std::vector<UtVec2d>> mLocalZones; // vector<UtVec2d> representation of mAvoidZones
   double                            mBestZonePathDist;
   std::vector<ZoneGraphNode*> mBestZonePath;  // do not need to delete this on deconstruction, objects owned elsewhere
   std::vector<ZoneGraphNode*> mZonePathStack; // do not need to delete this on deconstruction, objects owned elsewhere
};

//! Define script methods for WsfZoneRouteFinder.
class WSF_EXPORT WsfScriptZoneRouteFinderClass : public UtScriptClass
{
public:
   WsfScriptZoneRouteFinderClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~WsfScriptZoneRouteFinderClass() override;

   void* Create(const UtScriptContext& aContext) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(AvoidZone_1);
   UT_DECLARE_SCRIPT_METHOD(AvoidZone_2);
   UT_DECLARE_SCRIPT_METHOD(NumberOfAvoidanceZones);
   UT_DECLARE_SCRIPT_METHOD(GetAvoidanceZone);
   UT_DECLARE_SCRIPT_METHOD(RouteAroundZones);
   UT_DECLARE_SCRIPT_METHOD(ClearAvoidanceZones);
   UT_DECLARE_SCRIPT_METHOD(DrawGraph);
   UT_DECLARE_SCRIPT_METHOD(DrawAvoidances);
};

#endif
