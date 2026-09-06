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

#ifndef UTCLUSTER_HPP
#define UTCLUSTER_HPP

#include "ut_export.h"

#include <algorithm>
#include <limits>
#include <map>
#include <string>
#include <vector>

#include "UtVec2.hpp"

// Abstract class (pure virtual) used for interface
class UtClusterObject
{
public:
   explicit UtClusterObject(const std::string& aType)
      : mType(aType)
      , mId()
   {
   }

   virtual ~UtClusterObject() = default;
   virtual std::string        Id() { return mId; }
   virtual bool               VelocityValid()                                          = 0;
   virtual void               GetLocationWCS(double aLocationWCS[3])                   = 0;
   virtual void               GetLocationLLA(double& aLat, double& aLon, double& aAlt) = 0;
   virtual void               GetVelocityWCS(double aVelocityWCS[3])                   = 0;
   virtual const std::string& Type() { return mType; }

   operator const std::string&() const { return mId; }

protected:
   std::string mType;
   std::string mId;
};


//! UtClusterManager is a cluster creator & maintainer.
//!
//! UtClusterManager provides the processing desired for dynamic clustering.
//! It is used to form clusters from a list of entities and it maintains those clusters.
//! Cluster algorithms implemented at this time:
//! <ul>
//! <li> K-Means Clustering.
//! <li> Hierarchical Tree Clustering, single-linkage.
//! <li> Hierarchical Tree Clustering, complete-linkage. (default)
//! <li> Hierarchical Tree Clustering, average-linkage.
//! </ul>
//!
//! @see UtCluster
class UT_EXPORT UtClusterManager
{
public:
   //! The manager's clustering mode; it specifies which algorithm will be used.
   enum ClusteringMethod
   {
      cK_MEANS,                   //<! K-Means, very simple clustering
      cHIERARCHICAL_TREE_AVERAGE, //<! H-Tree average-linkage, cluster distance is mean location distance
      cHIERARCHICAL_TREE_COMPLETE, //<! H-Tree complete-linkage, cluster distance is max between any two members (default)
      cHIERARCHICAL_TREE_SINGLE    //<! H-Tree single-linkage, cluster distance is min between any two members
   };

   //! The manager's method for calculating distance.
   enum DistanceFunction
   {
      c2D_POSITION_ONLY, //<! entities closest together laterally will be clustered
      cPOSITION_ONLY     //,     //<! entities closest together (all three dimensions) will be clustered (default)
      // cPOSITION_VELOCITY  //<! entities heading in different direction will be considered as further away
   };

   UtClusterManager(ClusteringMethod aMethod = cHIERARCHICAL_TREE_COMPLETE, DistanceFunction aDistType = cPOSITION_ONLY);

   virtual ~UtClusterManager() = default;

   //! @name 'clustering' method(s).
   //@{
   void GetClusters(std::vector<UtClusterObject*>& aObjects, std::vector<std::vector<UtClusterObject*>>& aClusters);
   //@}

   //! @name 'manager configuration' methods.
   //@{
   void             SetNumClustersToCreate(size_t aNum) { mNumClusters = aNum; }
   size_t           GetNumClustersToCreate() { return mNumClusters; }
   void             SetDistanceLimit(double aDist) { mDistanceLimit = aDist; }
   double           GetDistanceLimit() { return mDistanceLimit; }
   void             SetClusterMethod(ClusteringMethod aMethod) { mClusterMethod = aMethod; }
   ClusteringMethod GetClusterMethod() { return mClusterMethod; }
   void             SetDistanceFunction(DistanceFunction aFunction) { mDistanceType = aFunction; }
   DistanceFunction GetDistanceFunction() { return mDistanceType; }
   //@}

   //! @name 'utility' methods.
   //@{
   double DistanceBetween(UtClusterObject& aObject, std::vector<UtClusterObject*>& aClusters);

   double DistanceBetween(UtClusterObject& aObject, UtClusterObject& aEntity2, bool aUseCustomIfExists = true);

   double DistanceBetween(std::vector<UtClusterObject*>& aClusters, std::vector<UtClusterObject*>& aClusters2);

   virtual bool UserDistanceFunction(UtClusterObject*, UtClusterObject*, double&) { return false; }

   void MeanLocWCS(std::vector<UtClusterObject*>& aClusters, double aLocationWCS[3]);

   std::vector<UtClusterObject*> ConvexHull(std::vector<UtClusterObject*>& aClusters);

   unsigned int UniqueId(std::vector<UtClusterObject*>& aClusters); // WARNING - this will sort and possibly change the
                                                                    // order of 'aCluster' members

   UtClusterObject* NearestMember(UtClusterObject& aObject, std::vector<UtClusterObject*>& aClusters);

   void NearestEdgePoint(UtClusterObject*               aObject,
                         std::vector<UtClusterObject*>& aClusters,
                         double  aLocWCS[3], // nearest location to any bounding (convex hull) edge of cluster
                         size_t& index1,     // index of one cluster member of the nearest edge
                         size_t& index2);    // index of other cluster member of the nearest edge
                                             // index1 == index2 when nearest point is a cluster member & not on an edge
   void CrossLength(std::vector<UtClusterObject*>& aClusters,
                    double                         refLocWCS[3],
                    double&                        dist,
                    double                         leftLocWCS[3],
                    double                         rightLocWCS[3]);

   void Delete(std::vector<UtClusterObject*>& aObjects);

   double SlantRange(double aLocWCS[3], double aOtherLocWCS[3]);
   double SlantRange(UtClusterObject& aObject, double aOtherLocWCS[3]);
   double SlantRange(UtClusterObject& aObject, UtClusterObject& aObject2);
   double GroundRange(double aLocWCS[3], double aOtherLocWCS[3]);
   double GroundRange(UtClusterObject& aObject, double aOtherLocWCS[3]);
   double GroundRange(UtClusterObject& aObject, UtClusterObject& aObject2);
   //@}

private:
   //! The number of clusters to create, if possible, and if no distance limit is specified
   size_t mNumClusters;
   //! The clustering algorithm to use
   ClusteringMethod mClusterMethod;
   //! The type of distance calculation to be done when clustering
   DistanceFunction mDistanceType;
   //! The max distance allowed between an object and it's cluster
   double mDistanceLimit;

   //! Get min distance between any two cluster members
   double DistanceBetweenSingle(std::vector<UtClusterObject*>& aClusters, std::vector<UtClusterObject*>& aClusters2);
   //! Get max distance between any two cluster members
   double DistanceBetweenComplete(std::vector<UtClusterObject*>& aClusters, std::vector<UtClusterObject*>& aClusters2);

   //! @name 'specific clustering algorithm' methods.
   //! Clustering stops when desired number of clusters (aClusterLimit) has been reached
   //@{
   void ClusterKMeans(std::size_t                                 aClusterLimit,
                      std::vector<UtClusterObject*>&              aClusterObjects,
                      std::vector<std::vector<UtClusterObject*>>& aClusters);
   void ClusterHierarchicalTree(std::size_t                                 aClusterLimit,
                                const std::vector<UtClusterObject*>&        aClusterObjects,
                                std::vector<std::vector<UtClusterObject*>>& aClusters);
   void ClusterHierarchicalTreeOptimized(std::size_t                                 aClusterLimit,
                                         std::vector<UtClusterObject*>&              aClusterObjects,
                                         std::vector<std::vector<UtClusterObject*>>& aClusters);
   //@}

   //! @name 'specific dynamic clustering algorithm' methods.
   //! Clustering stops when a distance limit (aClusterDistLimit) has been reached between clusters and their members
   //@{
   void ClusterHierarchicalTree(double                                      aClusterDistLimit,
                                const std::vector<UtClusterObject*>&        aClusterObjects,
                                std::vector<std::vector<UtClusterObject*>>& aClusters);
   void ClusterHierarchicalTreeOptimized(double                                      aClusterDistLimit,
                                         std::vector<UtClusterObject*>&              aClusterObjects,
                                         std::vector<std::vector<UtClusterObject*>>& aClusters);
   //@}
};


/*
   ProximityMatrix is useful for doing n^2 distance calculations between sets.
   It allows for row and column manipulation.
   \n
*/
class ProximityMatrix
{
public:
   ProximityMatrix(size_t aNumRow, size_t aNumCol)
      : mRows(std::vector<std::vector<double>>(aNumRow))
   {
      for (size_t i = 0; i < aNumRow; ++i)
      {
         mRows[i] = std::vector<double>(aNumCol, 0.0);
      }
   }

   ProximityMatrix(ProximityMatrix& aMat)
   {
      size_t r = aMat.Rows();
      size_t c = aMat.Cols();
      mRows    = std::vector<std::vector<double>>(r);
      for (size_t i = 0; i < r; ++i)
      {
         mRows[i] = std::vector<double>(c, 0.0);
         for (size_t j = 0; j < c; ++j)
         {
            mRows[i][j] = aMat.Get(i, j);
         }
      }
   }

   bool ValidIndices(size_t aRow, size_t aCol) { return (aRow < Rows() && aCol < Cols()); }

   size_t Rows() { return mRows.size(); }

   size_t Cols()
   {
      if (!mRows.empty())
      {
         return mRows[0].size();
      }
      return 0;
   }

   double Get(size_t aRow, size_t aCol)
   {
      if (ValidIndices(aRow, aCol))
      {
         return mRows[aRow][aCol];
      }
      return 0.0;
   }

   void Set(size_t aRow, size_t aCol, double aVal)
   {
      if (ValidIndices(aRow, aCol))
      {
         mRows[aRow][aCol] = aVal;
      }
   }

   void SetColumn(size_t aCol, double aVal)
   {
      if (ValidIndices(0, aCol))
      {
         for (size_t i = 0; i < Rows(); ++i)
         {
            mRows[i][aCol] = aVal;
         }
      }
   }

   void SetRow(size_t aRow, double aVal)
   {
      if (ValidIndices(aRow, 0))
      {
         for (size_t i = 0; i < Cols(); ++i)
         {
            mRows[aRow][i] = aVal;
         }
      }
   }

   void DeleteRow(size_t aRow)
   {
      if (ValidIndices(aRow, 0))
      {
         mRows.erase(mRows.begin() + aRow);
      }
   }

   void DeleteColumn(size_t aCol)
   {
      if (ValidIndices(0, aCol))
      {
         size_t R = Rows();
         for (size_t i = 0; i < R; ++i)
         {
            mRows[i].erase(mRows[i].begin() + aCol);
         }
      }
   }

   double GetMinIndices(size_t& aRow, size_t& aCol)
   {
      size_t R   = Rows();
      size_t C   = Cols();
      double min = std::numeric_limits<double>::max();
      for (size_t i = 0; i < R; ++i)
      {
         for (size_t j = 0; j < C; ++j)
         {
            if (min > Get(i, j))
            {
               min  = Get(i, j);
               aRow = i;
               aCol = j;
            }
         }
      }
      return min;
   }

   double GetMaxIndices(size_t& aRow, size_t& aCol)
   {
      size_t R   = Rows();
      size_t C   = Cols();
      double max = std::numeric_limits<double>::min();
      for (size_t i = 0; i < R; ++i)
      {
         for (size_t j = 0; j < C; ++j)
         {
            if (max < Get(i, j))
            {
               max  = Get(i, j);
               aRow = i;
               aCol = j;
            }
         }
      }
      return max;
   }

private:
   std::vector<std::vector<double>> mRows;
};

#endif
