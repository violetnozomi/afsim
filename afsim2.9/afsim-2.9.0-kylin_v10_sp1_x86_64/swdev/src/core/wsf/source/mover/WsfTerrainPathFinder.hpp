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
//*   This class is used to find a path from A to B in a 3D environment with
//*   weighted nodes.  It uses the UtGraph class to build a set of nodes that
//*   hold the weights for each square in a lat/lon grid.  Squares are measured
//*   in arc degrees rather than meters because the class needs to work on a
//*   spherical earth.  Extending from WsfPathfinder, this class makes use of
//*   available terrain data and analyzes the height data as well.  The class
//*   makes use of the Restricted Quadtree Triangulation algorithm
//*   to reduce the amount of nodes used to represent the pathing grid.
//*
//* AUTHOR
//*   Nick Chinnici
//*
//* NOTES:
//*   For more information about the Restricted Quadtree Triangulation algorithm
//*    visit: http://portal.acm.org/citation.cfm?id=288216.288219
//***************************************************************************

#ifndef WSFTERRAINPATHFINDER_HPP
#define WSFTERRAINPATHFINDER_HPP

#include "wsf_export.h"

#include "WsfObject.hpp"
#include "WsfPathFinder.hpp"

class WSF_EXPORT WsfTerrainPathFinder : public WsfPathFinder
{
public:
   WsfTerrainPathFinder(WsfScenario* aScenarioPtr);
   ~WsfTerrainPathFinder() override = default;
   bool             Initialize(WsfSimulation* aSimulationPtr) override;
   const WsfPFNode* GetClosestNode(const WsfGeoPoint& aPointPtr) override;
   const WsfPFNode* GetClosestNodePos(float x, const WsfGeoPoint& aPointPtr);
   bool             ProcessInput(UtInput& aInput) override;

   typedef struct
   {
      double Lat;
      double Lon;
   } sTerrainLatLon;

   typedef struct
   {
      sTerrainLatLon Pt1;
      sTerrainLatLon Pt2;
      sTerrainLatLon Pt3;
   } sTerrainTriCell;

   bool ComparePoints(sTerrainLatLon aPt1, sTerrainLatLon aPt2);

private:
   void RQTRefineMesh(float aMetric);
   void RQTRefineMesh();

   std::string ConvertToDegMinSec(double aLoc);
   void        RQTEvaluateLine(int x, int y, float metric, int stride);
   void        RQTMarkPointDependants(int x, int y, int stride);
   double      RQTPointLineDistance(WsfPFNode* APtr, WsfPFNode* BPtr, WsfPFNode* PPtr);
   void        ComputeNodeNormals();
   void        GeneratePathFinderLight();
   void        WriteOutAsZones();
   void        LoadGridInfo();

   bool shortest_path(const_node_iterator aSrcNodeIter,
                      const_node_iterator aDstNodeIter,
                      NodeList&           aPath,
                      double&             aCost,
                      const cost_func*    aCostFuncPtr) const override;


   float mRQTMetric;
   int mChunkSize; // used in the RQT algorithm to define the minimum chunk size (i.e. 3x3, 5x5, etc.) during calculation.
   std::string mRQTFile;
   bool        mUseRQTFile;
   std::string mNavMeshZonesFile;
   float       mMaxNormalAngle;


   class pf_cost_func : public PFGraph::cost_func
   {
   public:
      double operator()(const WsfPFEdge& aEdge, const WsfPFNode& aNode1, const WsfPFNode& aNode2) const override
      {
         if (aNode2.mWeight == std::numeric_limits<double>::max())
         {
            return std::numeric_limits<double>::max();
         }

         double cost = (aEdge.mLength * 2) + aNode2.mWeight; // scale the distance to make it more influential
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

#endif
