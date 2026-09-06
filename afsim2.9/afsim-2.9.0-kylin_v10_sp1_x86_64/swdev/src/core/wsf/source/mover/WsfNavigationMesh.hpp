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

#ifndef WSFNAVIGATIONMESH_HPP
#define WSFNAVIGATIONMESH_HPP

#include "wsf_export.h"

#include <limits>
#include <map>

#include "UtVec3.hpp"
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfMover.hpp"
#include "WsfNavigationCell.hpp"
#include "WsfPathFinder.hpp"
#include "WsfZone.hpp"
#include "WsfZoneDefinition.hpp"
#include "script/WsfScriptObjectClass.hpp"

#define FLOAT_EPSILON 1.0E-6F

class WSF_EXPORT WsfNavigationMesh : public WsfPathFinder
{
public:
   WsfNavigationMesh(WsfScenario* aScenarioPtr);
   ~WsfNavigationMesh() override;
   bool        ProcessInput(UtInput& aInput) override;
   const char* GetScriptClassName() const override;
   enum ZONE_INTERACTION
   {
      NO_INTERACTION,
      ONE_VERT_INSIDE,
      TWO_VERTS_INSIDE,
      THREE_VERTS_INSIDE,
      ZONE_VERT_INSIDE
   };

   void CreateNavigationMesh();
   // void CreateGridOfCentroids();
   void                         GetCellNeighbors(WsfNavigationCell* aNavCellPtr);
   int                          GetXSize() { return mXSize; }
   int                          GetYSize() { return mYSize; }
   int                          GetNumCells() { return static_cast<int>(mMeshCells.size()); }
   void                         DebugDrawMesh();
   void                         DebugDrawZones() override;
   WsfPathFinder::node_iterator GetGrid(long aX, long aY);
   bool                         Initialize(WsfSimulation* aSimulationPtr) override;

   bool shortest_path(const_node_iterator aSrcNodeIter,
                      const_node_iterator aDstNodeIter,
                      NodeList&           aPath,
                      double&             aCost,
                      const cost_func*    aCostFuncPtr = nullptr) const override;

   bool FindPath(const WsfGeoPoint& aStartPtr, WsfGeoPoint& aEndPtr, WsfRoute& aRoute, double& aCost) override;

   const WsfPFNode*                GetClosestNode(const WsfGeoPoint& aPointPtr) override;
   void                            RefineFromCurrentIndex(int aIndex);
   bool                            RefineFromCurrentPosition(int aIndex, double aLat, double aLon, double aAlt, WsfRoute& aRoute, const WsfRoute* aOriginalRoute);
   WsfNavigationCell*              GetCellForPoint(const WsfGeoPoint& aPoint);
   std::vector<WsfNavigationCell*> GetCellsForPoint(const WsfGeoPoint& aPoint);

   void TessellateCell(WsfNavigationCell* aCellPtr, WsfZoneDefinition* aZonePtr, int aCurrentTessLevel);

   ZONE_INTERACTION GetCellZoneInteraction(WsfNavigationCell* aCellPtr, WsfZoneDefinition* aZonePtr);
   void             ReNumberMeshCells();
   void             SetMeshBoundries(double aLLA[3]);
   void             SetMeshCenter();
   void             InsertCellIntoGrid(WsfNavigationCell* aNavCellPtr);
   void             LinkGridNeighbors(WsfNavigationCell* aNavCellPtr);
   int              GetFurthestVisiblePoint(int aIndex, WsfRoute& aRoute);
   bool             LineOfSightTest(WsfNavigationCell* aFromCellPtr, int aIndexFrom, int aIndexTo, WsfRoute& aRoute);

   void         MovePointAlongPath(WsfGeoPoint& aPoint, int aIndexFrom, int aIndexTo);
   void         MovePointTowardsCentroid(WsfGeoPoint& aPoint, int aIndexFrom, WsfNavigationCell* aCellPtr);
   bool         ComputeSetRoute(WsfMover& aMover, WsfRoute& aRoute, int& aInitialPointIndex) override;
   bool         ComputeFindPath(WsfMover& aMover, WsfRoute& aRoute) override;
   void         GetRouteIntersectionPoints();
   void         CleanUpDuplicatePoints();
   WsfGeoPoint* FindClosestValidPoint(double aSimTime, const WsfGeoPoint& aGeoPoint) override;

   WsfGeoPoint GetRandomLocation() override;
   bool        IsNavMesh() override { return true; }

   void   SetRefineUpdateTime(double aTime) { mRefineUpdateTime = aTime; }
   double GetRefineUpdateTime() { return mRefineUpdateTime; }

   void   SetNavMeshThinkTime(double aTime) { mNavMeshThinkTime = aTime; }
   double GetNavMeshThinkTime() { return mNavMeshThinkTime; }

private:
   void GenerateExportedNavMesh();
   void ImportNavMesh();

   class pf_cost_func : public PFGraph::cost_func
   {
   public:
      double operator()(const WsfPFEdge& aEdge, const WsfPFNode& aNode1, const WsfPFNode& aNode2) const override
      {
         double cost = (aEdge.mLength * 2); // scale the distance to make it more influential
         return cost;
      }

      double operator()(const WsfPFNode& aNode1, const WsfPFNode& aNode2) const override
      {
         double estimate = aNode2.mLoc.GetDistanceFrom(aNode1.mLoc);
         return estimate;
      }

      // adding consider_node function to the cost_func.   This will flag a node to be ignored during pathfinding.
      bool consider_node(const WsfPFNode& aNode1) const override
      {
         if (aNode1.mWeight ==
             std::numeric_limits<double>::max()) // NO_FLY_ZONES are marked as std::numeric_limits<double>::max()
                                                 // and won't be considered in pathfinding
         {
            return false;
         }
         else
         {
            return true;
         }
      }
   };

protected:
   std::vector<WsfNavigationCell*>           mMeshCells;
   std::vector<WsfNavigationCell*>::iterator mMeshCellsIterator;
   WsfDraw*                                  mDebugMeshDrawPtr;
   WsfDraw*                                  mDebugRouteDrawPtr;
   std::vector<WsfPFNode*>                   mMeshNodes;
   std::vector<WsfGeoPoint>                  mAdjustedPath;
   std::vector<WsfNavigationCell*> mGetCellForPointCache; // GetCellForPoint is an expensive function, keep a cache of
                                                          // recently accessed cells.

private:
   double                                     mMaxLat;
   double                                     mMinLat;
   double                                     mMaxLon;
   double                                     mMinLon;
   double                                     mCenterLLA[3];
   double                                     mRefineUpdateTime;
   double                                     mNavMeshThinkTime;
   std::string                                mInputFile;
   std::string                                mOutputFile;
   bool                                       mLoadBinaryFile;
   std::map<unsigned int, WsfNavigationCell*> mCellMap;
};

//! Define script methods for WsfPathfinder.
class WSF_EXPORT WsfScriptNavigationMeshClass : public WsfScriptObjectClass
{
public:
   WsfScriptNavigationMeshClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(DebugDrawMesh); // NO_DOC | FOR_TEST_ONLY
};

#endif
