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

//* PURPOSE:
//*   This class is used to find a path from A to B in a 2D environment with
//*   weighted nodes.  It uses the UtGraph class to build a set of nodes that
//*   hold the weights for each square in a lat/lon grid.  Squares are measured
//*   in arc degrees rather than meters because the class needs to work on a
//*   spherical earth.
//*
//* AUTHOR
//*   Michael Williams
//*
//* NOTES:
//*   None
//***************************************************************************

#ifndef WSFPATHFINDER_HPP
#define WSFPATHFINDER_HPP

#include "wsf_export.h"

#include <limits>
#include <list>
#include <map>
#include <vector>

class pf_cost_func;
#include "UtGraph.hpp"
class UtInput;
class UtInputBlock;
class WsfDraw;
#include "WsfGeoPoint.hpp"
#include "WsfObject.hpp"
#include "WsfObjectTypeList.hpp"
class WsfPlatform;
class WsfRoute;
#include "WsfRouteComputer.hpp"
#include "script/WsfScriptObjectClass.hpp"
class WsfSimulation;
class WsfZone;

// PFNode and PFEdge have to be defined out here because they're used for the
// definition of WsfPathFinder, an inheritor of a templated UtGraph class.

class WSF_EXPORT WsfPFNode
{
public:
   WsfPFNode(long aX, long aY, float aBaseWeight, WsfGeoPoint& aLoc);

   bool operator==(const WsfPFNode& aRhs) const;

   bool operator!=(const WsfPFNode& aRhs) const;

   bool operator<(const WsfPFNode& aRhs) const;

   void setMarked(bool b);
   bool getMarked();
   void AddNeighbor(WsfPFNode* aNodePtr);

   std::vector<WsfPFNode*> mNeighbors;
   long                    mX;
   long                    mY;
   float                   mNormalAngle;
   float                   mBaseWeight; // mutable because we need to modify it through const iterators
   mutable double          mWeight;
   bool        mMarkedPt; // points are marked as significant or insignificant by the RQT reduction algorithm
   WsfGeoPoint mLoc;
   int         mNetworkIndex;
};

//! This struct represents an edge between two nodes. This is
//! used to represent links that exist between comm networks.

struct WsfPFEdge
{
   WsfPFEdge(WsfPFNode* aSrcNodePtr, WsfPFNode* aDstNodePtr)
      : mSrcNodePtr(aSrcNodePtr)
      , mDstNodePtr(aDstNodePtr)
   {
      mLength = aSrcNodePtr->mLoc.GetDistanceFrom(aDstNodePtr->mLoc);
   }

   bool operator==(const WsfPFEdge& aRhs) const
   {
      return ((mSrcNodePtr == aRhs.mSrcNodePtr) && (mDstNodePtr == aRhs.mDstNodePtr));
   }

   WsfPFNode* mSrcNodePtr;
   WsfPFNode* mDstNodePtr;
   double     mLength;
};

class WSF_EXPORT WsfPathFinder :
   public WsfUncloneableObject,
   public UtGraphT<WsfPFNode, WsfPFEdge, false, double>,
   public WsfRouteComputer
{
public:
   using PFGraph = UtGraphT<WsfPFNode, WsfPFEdge, false, double>;

   WsfPathFinder(WsfScenario* aScenarioPtr, WsfGeoPoint& aUpperLeftPtr, WsfGeoPoint& aLowerRightPtr, double aGridSizeDegrees);
   WsfPathFinder(WsfScenario* aScenarioPtr);
   ~WsfPathFinder() override;

   virtual bool Initialize(WsfSimulation* aSimulationPtr);

   bool ProcessInput(UtInput& aInput) override;

   const char* GetScriptClassName() const override;

   // Add aZonePtr with weight aWeight.  Higher weights mean stronger avoidance.
   // Does not allow duplicates.
   void AddZone(WsfZone* aZonePtr, double aWeight);

   // Remove aZonePtr if it exists.
   void RemoveZone(WsfZone* aZonePtr);

   void RecalculateWeights();

   virtual bool FindPath(const WsfGeoPoint& aStartPtr, WsfGeoPoint& aEndPtr, WsfRoute& aRoute, double& aCost);

   virtual const WsfPFNode* GetClosestNode(const WsfGeoPoint& aPointPtr);

   void          print() const;
   node_iterator GetGrid(long aX, long aY);
   node_iterator GetGridAbsolute(unsigned long idx);
   void          SetGrid(unsigned long aX, unsigned long aY, node_iterator aIterPtr);
   void          SetGridAbsolute(unsigned long aX, node_iterator aIterPtr);

   void         DebugDrawGrid();
   virtual void DebugDrawZones();

   WsfGeoPoint GetNodeLocation(int nodeIndex);

   long                 GetXSize() { return mXSize; }
   long                 GetYSize() { return mYSize; }
   bool                 GetConsiderNode(const WsfPFNode& aNode);
   bool                 GetConsiderNode(const WsfGeoPoint& aGeoPoint);
   int                  GetNumberOfZones();
   double               GetGridSizeDegrees() { return mGridSizeDegrees; }
   WsfGeoPoint          GetUpperLeft() { return mUpperLeft; }
   WsfGeoPoint          GetLowerRight() { return mLowerRight; }
   WsfGeoPoint*         FindClosestPointOnEdge(double aSimTime, const WsfGeoPoint& aGeoPoint);
   virtual WsfGeoPoint* FindClosestValidPoint(double aSimTime, const WsfGeoPoint& aGeoPoint);
   WsfGeoPoint*         FindClosestPointInQuadrant(const WsfGeoPoint& aGeoPoint, int aQuadrant);
   WsfZone*             GetZoneByName(const std::string& aZoneName);

   bool ComputeFindPath(WsfMover& aMover, WsfRoute& aRoute) override;
   bool ComputeSetRoute(WsfMover& aMover, WsfRoute& aRoute, int& aInitialPointIndex) override;
   bool ComputeUpdateRoute(WsfMover& aMover, WsfRoute& aRoute) override;
   bool ComputeExtrapolate(WsfMover& aMover, WsfRoute& aRoute) override;

   bool                ReturnToRoute(WsfMover& aMover, WsfRoute& aRoute) override;
   virtual WsfGeoPoint GetRandomLocation();
   virtual void        SetDebugEnabled(bool aDebugEnabled) { mDebugEnabled = aDebugEnabled; }
   bool                DebugEnabled() const { return mDebugEnabled; }
   WsfSimulation*      GetSimulation() const { return mSimulationPtr; }

protected:
   bool ContainsZone(WsfZone* aZonePtr);
   bool ContainsZone(const std::string& aZoneName);

   bool PointIsInGridBounds(int x, int y);

   // void LoadGridInfo();
   // void GeneratePathFinderLight();
   bool IsDuplicateEdge(node_iterator nodeIter, const WsfPFNode& aNode2, const WsfPFEdge& tmpEdge);
   bool shortest_path(const_node_iterator aSrcNodeIter,
                      const_node_iterator aDstNodeIter,
                      NodeList&           aPath,
                      double&             aCost,
                      const cost_func*    aCostFuncPtr = nullptr) const override;

   WsfScenario*               mScenarioPtr;
   WsfSimulation*             mSimulationPtr;
   std::list<WsfZone*>        mZones;
   std::map<WsfZone*, double> mZoneWeights;

   WsfGeoPoint mUpperLeft;
   WsfGeoPoint mLowerRight;

   double mGridSizeDegrees;

   long mXSize;
   long mYSize;

   std::vector<node_iterator> mGrid;
   bool                       mInitialized; // only true when mUpperLeft,
   // mLowerRight, and mGridSizeDegrees
   // have been set and mGrid has been
   // allocated

   WsfDraw* mDebugDrawObjPtr;
   bool     mDebugEnabled;

   class pf_cost_func : public PFGraph::cost_func
   {
   public:
      double operator()(const WsfPFEdge& aEdge, const WsfPFNode& aNode1, const WsfPFNode& aNode2) const override
      {
         double cost = aEdge.mLength * aNode2.mWeight;
         return cost;
      }

      double operator()(const WsfPFNode& aNode1, const WsfPFNode& aNode2) const override
      {
         double estimate = aNode2.mLoc.GetDistanceFrom(aNode1.mLoc);
         return estimate;
      }

      // adding consider_node function to the cost_func.   This will flag a node to be ignored during path-finding.
      bool consider_node(const WsfPFNode& aNode1) const override
      {
         // NO_FLY_ZONES are marked as DBL_MAX and won't be considered in path-finding
         return (aNode1.mWeight == std::numeric_limits<double>::max() ? false : true);
      }
   };
};

class WsfPathFinderTypes : public WsfObjectTypeList<WsfPathFinder>
{
public:
   WsfPathFinderTypes(WsfScenario& aScenario);
   std::unique_ptr<WsfPathFinder> ProcessPathFinderInput(UtInput& aInput);
   virtual LoadResult             LoadInstance(UtInput& aInput);
   void                           ProcessBlock(UtInputBlock& aInputBlock, WsfPathFinder* aPFPtr, bool init);
   bool                           ProcessInput(UtInput& aInput) override;

   // static WsfPathFinder* FindPathFinder(const std::string& aName);

   // static bool AddPathFinder(const std::string& aTypeName,
   //                         WsfPathFinder*     aTypePtr);
   // static void ClearTypes();

   // static WsfObjectTypeList<WsfPathFinder> mTypeList;   //!< The list of pathfinders
};

//! Retains the list of active path finders
//! Currently, the path finder implementation cannot be used with more than one simulation at a time
class WsfPathFinderList
{
public:
   WsfPathFinderList();
   ~WsfPathFinderList() = default;
   void           Add(std::unique_ptr<WsfPathFinder> aPathFinderPtr);
   WsfPathFinder* Find(const std::string& aPathFinderName) const;
   bool           Initialize(WsfSimulation* aSimulationPtr);

private:
   WsfPathFinderList(const WsfPathFinderList& aSrc);
   using PathFinderMap = std::map<std::string, std::unique_ptr<WsfPathFinder>>;

   WsfSimulation* mSimulationPtr;
   PathFinderMap  mPathFinders;
};

//! Define script methods for WsfPathfinder.
class WSF_EXPORT WsfScriptPathFinderClass : public WsfScriptObjectClass
{
public:
   WsfScriptPathFinderClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(FindClosestPointOnEdge);
   UT_DECLARE_SCRIPT_METHOD(FindClosestValidPoint);
   UT_DECLARE_SCRIPT_METHOD(RandomLocation);
   UT_DECLARE_SCRIPT_METHOD(NearEndOfPath);
   UT_DECLARE_SCRIPT_METHOD(SetDebugEnabled);
   UT_DECLARE_SCRIPT_METHOD(DebugEnabled);
   UT_DECLARE_SCRIPT_METHOD(DebugDrawGrid);
   UT_DECLARE_SCRIPT_METHOD(DebugDrawZones);
};

#endif
