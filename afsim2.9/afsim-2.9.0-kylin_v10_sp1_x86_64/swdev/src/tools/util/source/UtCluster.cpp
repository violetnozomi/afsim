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

#include "UtCluster.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>

#include "UtCast.hpp"
#include "UtConvexHull.hpp"
#include "UtDictionary.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtLineSegment.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"

////! For a Cluster's Convex Hull, calculate the major and minor axes lengths,
////! and the compass orientation of the major axis.
// void UtCluster::ConvexHullGeometry(double& aMajorHeading,
//                                    double& aMajorAxis,
//                                    double& aMinorAxis)
//{
//    UpdateConvexHull();
//    UtEntity centroid;
//    double meanWCS[3];
//    MeanLocWCS(meanWCS);
//    centroid.SetLocationWCS(meanWCS);
//    vector< pair<double, double> > NEpoints;
//    unsigned int i;
//
//    if (mConvexHull.size() <= 1)
//    {
//       aMajorHeading = 0.0;
//       aMajorAxis = 0.0;
//       aMinorAxis = 0.0;
//       return;
//    }
//
//    // Store the NE locations, relative to the mean.
//    double ned[3], wcs[3];
//    for (i = 0; i != mConvexHull.size(); ++i)
//    {
//       mConvexHull[i]->GetLocationWCS(wcs);
//       centroid.ConvertWCSToNED(wcs, ned);
//       NEpoints.push_back(make_pair(ned[0], ned[1]));
//    }
//
//    // Find the major axis, which is along the two points that are farthest apart:
//    double maxDist = 0.0;
//    unsigned int savedIndexFrom = 0;
//    unsigned int savedIndexTo = 0;
//    unsigned int indexFrom;
//    unsigned int indexTo;
//    for (indexFrom = 0; indexFrom != mConvexHull.size(); ++indexFrom)
//    {
//       for (indexTo = 0; indexTo != mConvexHull.size(); ++indexTo)
//       {
//          if (indexFrom != indexTo)
//          {
//             double dn = NEpoints[indexFrom].first - NEpoints[indexTo].first;
//             double de = NEpoints[indexFrom].second - NEpoints[indexTo].second;
//             double dist = sqrt(dn * dn + de * de);
//             if (dist > maxDist)
//             {
//                savedIndexFrom = indexFrom;
//                savedIndexTo = indexTo;
//                maxDist = dist;
//             }
//          }
//       }
//    }
//
//    double dNorth = NEpoints[savedIndexTo].first - NEpoints[savedIndexFrom].first;
//    double dEast  = NEpoints[savedIndexTo].second - NEpoints[savedIndexFrom].second;
//    aMajorHeading = atan2(dEast, dNorth);
//    aMajorAxis = maxDist;
//
//    double maxRight = 0.0;
//    double maxLeft = 0.0;
//
//    for (i = 0; i != mConvexHull.size(); ++i)
//    {
//       double dr = sin(aMajorHeading) * NEpoints[i].first + cos(aMajorHeading) * NEpoints[i].second;
//       if (dr > maxRight)
//       {
//          maxRight = dr;
//       }
//       if (dr < maxLeft)
//       {
//          maxLeft = dr;
//       }
//    }
//
//    aMinorAxis = maxRight - maxLeft;
//
//    assert(aMajorAxis >= aMinorAxis);
//*/
// }

///////////////////////////////////////////////////////////////////////////////

UtClusterManager::UtClusterManager(ClusteringMethod aMethod, DistanceFunction aDistType)
   : mNumClusters(0)
   , mClusterMethod(aMethod)
   , mDistanceType(aDistType)
   , mDistanceLimit(-1.0)
{
   // SetName("none");
   // SetType("UtClusterManager");
}

double UtClusterManager::SlantRange(double aLocWCS[3], double aOtherLocWCS[3])
{
   double locDiffWCS[3];
   UtVec3d::Subtract(locDiffWCS, aLocWCS, aOtherLocWCS);
   return UtVec3d::Magnitude(locDiffWCS);
}


double UtClusterManager::SlantRange(UtClusterObject& aObject, double aOtherLocWCS[3])
{
   double thisLocWCS[3];
   aObject.GetLocationWCS(thisLocWCS);
   return SlantRange(thisLocWCS, aOtherLocWCS);
}


double UtClusterManager::SlantRange(UtClusterObject& aObject, UtClusterObject& aObject2)
{
   double thisLocWCS[3];
   aObject.GetLocationWCS(thisLocWCS);
   return SlantRange(aObject2, thisLocWCS);
}

// taken from UtMeasurementUtil::GroundRange()
double UtClusterManager::GroundRange(double aLocWCS[3], double aOtherLocWCS[3])
{
   // Use the definition of the dot product to get the angle between the WCS locations.
   double cosTheta =
      UtVec3d::DotProduct(aLocWCS, aOtherLocWCS) / (UtVec3d::Magnitude(aLocWCS) * UtVec3d::Magnitude(aOtherLocWCS));
   cosTheta     = std::max(-1.0, std::min(cosTheta, 1.0));
   double theta = acos(cosTheta);
   // Arc length (ground range) = earth radius * theta
   return UtSphericalEarth::cEARTH_RADIUS * theta;
}


double UtClusterManager::GroundRange(UtClusterObject& aObject, double aOtherLocWCS[3])
{
   double thisLocWCS[3];
   aObject.GetLocationWCS(thisLocWCS);
   return GroundRange(thisLocWCS, aOtherLocWCS);
}


double UtClusterManager::GroundRange(UtClusterObject& aObject, UtClusterObject& aObject2)
{
   double thisLocWCS[3];
   aObject.GetLocationWCS(thisLocWCS);
   return GroundRange(aObject2, thisLocWCS);
}


double UtClusterManager::DistanceBetweenSingle(std::vector<UtClusterObject*>& aClusters,
                                               std::vector<UtClusterObject*>& aClusters2)
{
   double minDist = std::numeric_limits<double>::max();
   for (size_t i = 0; i < aClusters.size(); ++i)
   {
      for (size_t j = 0; j < aClusters2.size(); ++j)
      {
         minDist = std::min(SlantRange(*aClusters[i], *aClusters2[i]), minDist);
      }
   }
   return minDist;
}


double UtClusterManager::DistanceBetweenComplete(std::vector<UtClusterObject*>& aClusters,
                                                 std::vector<UtClusterObject*>& aClusters2)
{
   double maxDist = std::numeric_limits<double>::min();
   for (auto& cluster : aClusters)
   {
      for (auto& cluster2 : aClusters2)
      {
         maxDist = std::max(SlantRange(*cluster, *cluster2), maxDist);
      }
   }
   return maxDist;
}


double UtClusterManager::DistanceBetween(UtClusterObject& aObject, UtClusterObject& aObject2, bool aUseCustomIfExists)
{
   double dist = 0.0;

   if (aUseCustomIfExists == true && UserDistanceFunction(&aObject, &aObject2, dist))
   {
      return dist;
   }

   if (mDistanceType == c2D_POSITION_ONLY)
   {
      // take off altitude difference, only 2d difference
      dist = GroundRange(aObject, aObject2);
   }
   else
   {
      // take 3d difference
      dist = SlantRange(aObject, aObject2);
   }

   // if (mDistanceType == cPOSITION_VELOCITY)
   //{
   //   if (aObject.VelocityValid() && aObject2.VelocityValid())
   //   {
   //      double tVelWCS[3];
   //      double t2VelWCS[3];
   //      aObject.GetVelocityWCS(tVelWCS);
   //      aObject2.GetVelocityWCS(t2VelWCS);
   //      //scale the velocity difference
   //      double maxVelDiff = UtVec3d::Magnitude(tVelWCS) + UtVec3d::Magnitude(t2VelWCS);
   //      maxVelDiff = max(1.0, maxVelDiff); //protect against divide by zero
   //      double scale = 0.0;
   //      if (mDistanceLimit > 0.0)
   //      {
   //         //at a maximum, the distance will be increased by 25% of the distance-limit
   //         scale = 0.25 * mDistanceLimit / maxVelDiff;
   //      }
   //      else
   //      {
   //         //at a maximum, the distance will be increased by 15% of the current distance
   //         scale = 0.15 * dist / maxVelDiff;
   //      }
   //      dist += scale * SlantRange(tVelWCS, t2VelWCS);
   //   }
   //}
   return dist;
}


double UtClusterManager::DistanceBetween(std::vector<UtClusterObject*>& aClusters, std::vector<UtClusterObject*>& aClusters2)
{
   double dist = 0.0;

   double aWCS[3];
   double bWCS[3];
   UtClusterManager::MeanLocWCS(aClusters, aWCS);
   UtClusterManager::MeanLocWCS(aClusters2, bWCS);

   if (mDistanceType == c2D_POSITION_ONLY)
   {
      // take off altitude difference, only 2d difference
      dist = GroundRange(aWCS, bWCS);
   }
   else
   {
      // take 3d difference
      dist = SlantRange(aWCS, bWCS);
   }

   // double dist = 0.0;
   // if (mDistanceType == cPOSITION_VELOCITY)
   //{
   //   if (aCluster.VelocityValid() && aCluster2.VelocityValid())
   //   {
   //      aCluster.MeanVelWCS(aWCS);
   //      aCluster2.MeanVelWCS(bWCS);
   //      //scale the velocity difference
   //      double maxVelDiff = UtVec3d::Magnitude(aWCS) + UtVec3d::Magnitude(bWCS);
   //      double scale = 0.0;
   //      if (mDistanceLimit > 0.0)
   //      {
   //         //at a maximum, the distance will be increased by 25% of the distance-limit
   //         scale = 0.25 * mDistanceLimit / maxVelDiff;
   //      }
   //      else
   //      {
   //         //at a maximum, the distance will be increased by 15% of the current distance
   //         scale = 0.15 * dist / maxVelDiff;
   //      }
   //      dist += scale * SlantRange(aWCS, bWCS);
   //   }
   //}
   return dist;
}


void UtClusterManager::MeanLocWCS(std::vector<UtClusterObject*>& aClusters, double aLocationWCS[3])
{
   double dCount    = static_cast<double>(aClusters.size());
   double locWCS[3] = {0.0, 0.0, 0.0};
   aLocationWCS[0]  = 0.0;
   aLocationWCS[1]  = 0.0;
   aLocationWCS[2]  = 0.0;
   for (auto& cluster : aClusters)
   {
      // update the fields for finding the mean
      cluster->GetLocationWCS(locWCS);
      // add in each members contribution to the average mean location
      // (don't average at end, number can get too large)
      aLocationWCS[0] += (locWCS[0] / dCount);
      aLocationWCS[1] += (locWCS[1] / dCount);
      aLocationWCS[2] += (locWCS[2] / dCount);
   }
}


std::vector<UtClusterObject*> UtClusterManager::ConvexHull(std::vector<UtClusterObject*>& aClusters)
{
   std::vector<UtClusterObject*> hull;
   // convert input into a vector<UtVec2d> for UtConvexHull
   std::vector<UtVec2d>                points;
   std::map<UtVec2d, UtClusterObject*> mapVecMembers;
   for (UtClusterObject* ptr : aClusters)
   {
      UtVec2d vec;
      double  alt;
      ptr->GetLocationLLA(vec[0], vec[1], alt);
      points.push_back(vec);
      mapVecMembers[vec] = ptr; // just store pointers for now (clone later if needed)
   }

   // find the convex hull
   points = UtConvexHull::ConvexHull(points);

   // now convert back to vector of UtClusterObjects
   for (const auto& point : points)
   {
      UtClusterObject* ptr = mapVecMembers[point];
      hull.push_back(ptr);
   }
   return hull;
}

bool CompareClusterObjectById(UtClusterObject* a, UtClusterObject* b)
{
   return (a->Id() < b->Id());
}

unsigned int UtClusterManager::UniqueId(std::vector<UtClusterObject*>& aClusters)
{
   sort(aClusters.begin(), aClusters.end(), CompareClusterObjectById);
   std::ostringstream oss;
   const char*        separator = ",";
   // concatenate all sorted member ids into string
   for (auto& it : aClusters)
   {
      oss << it->Id() << separator; // id string will end with separator, but that is ok
   }
   return ut::safe_cast<unsigned int, int>(
      UtDictionary::GetDictionary().GetNumber(oss.str())); // a unique ID based on the member ids
}


UtClusterObject* UtClusterManager::NearestMember(UtClusterObject& aObject, std::vector<UtClusterObject*>& aClusters)
{
   // only returns null of aCluster is empty
   if (aClusters.empty())
   {
      return nullptr;
   }
   double           minDist = std::numeric_limits<double>::max();
   UtClusterObject* closest = nullptr;
   for (auto& cluster : aClusters)
   {
      double dist = DistanceBetween(aObject, *cluster, false);
      if (dist < minDist)
      {
         minDist = dist;
         closest = cluster;
      }
   }
   return closest;
}


void UtClusterManager::NearestEdgePoint(UtClusterObject*               aObject,
                                        std::vector<UtClusterObject*>& aClusters,
                                        double                         aLocWCS[3],
                                        size_t&                        index1,
                                        size_t&                        index2)
{
   if (aObject == nullptr || aClusters.empty())
   {
      aLocWCS[0] = aLocWCS[1] = aLocWCS[2] = 0.0;
      index1 = index2 = 0;
      return;
   }

   if (aClusters.size() == 1)
   {
      index1 = index2 = 0;
      aClusters[0]->GetLocationWCS(aLocWCS);
      return;
   }

   // get convex hull
   std::vector<UtClusterObject*> hull = ConvexHull(aClusters);

   // find nearest point to all [finite] hull edge lines
   UtVec3d testPt;
   aObject->GetLocationWCS(testPt.GetData());
   double  minDist = std::numeric_limits<double>::max();
   UtVec3d nearest;

   size_t i1 = 0;
   size_t i2 = 0;

   for (size_t i = 0; i < hull.size(); ++i)
   {
      size_t  next = (i == (hull.size() - 1)) ? (0) : (i + 1); // check for wrap around
      UtVec3d loc1;
      UtVec3d loc2;
      hull[i]->GetLocationWCS(loc1.GetData());
      hull[next]->GetLocationWCS(loc2.GetData());
      UtLineSegment edge(loc1.GetData(), loc2.GetData());
      UtVec3d       closestToEdge;
      double        mod;
      edge.PointOfClosestApproach(testPt.GetData(), closestToEdge.GetData(), mod, true);
      double dist = (testPt - closestToEdge).Magnitude();
      if (dist < minDist)
      {
         minDist = dist;
         nearest = closestToEdge;
         if (mod <= 0.0)
         {
            i1 = i2 = i;
         }
         else if (mod >= 1.0)
         {
            i1 = i2 = next;
         }
         else
         {
            i1 = i;
            i2 = next;
         }
      }
   }
   aLocWCS[0] = nearest[0];
   aLocWCS[1] = nearest[1];
   aLocWCS[2] = nearest[2];
   // reconcile the indices (don't pass back indices that relevant to the hull, but for the whole cluster)
   for (size_t i = 0; i < aClusters.size(); ++i)
   {
      if (aClusters[i] == hull[i1])
      {
         index1 = i;
      }
      if (aClusters[i] == hull[i2])
      {
         index2 = i;
      }
   }
}


void UtClusterManager::CrossLength(std::vector<UtClusterObject*>& aClusters,
                                   double                         refLocWCS[3],
                                   double&                        dist,
                                   double                         leftLocWCS[3],
                                   double                         rightLocWCS[3])
{
   UtVec3d ref(refLocWCS);
   UtVec3d mean;
   MeanLocWCS(aClusters, mean.GetData());

   UtVec3d line = (mean - ref);
   // orthogonal unit vector: perpendicular to line from reference point to cluster mean
   UtVec3d orth;
   orth.CrossProduct(line, ref); // make orth actually orthogonal
   orth.Normalize();

   // find scalar projection of all cluster points onto the orthogonal unit vector
   double  leftMost  = 0.0;
   double  rightMost = 0.0;
   UtVec3d P;
   for (auto& cluster : aClusters)
   {
      cluster->GetLocationWCS(P.GetData());
      P.Subtract(mean); // turn P into a vector (originating from mean)
      double scalarProjection =
         orth.DotProduct(P); // this is how far left (negative) or right (positive) from mean the point is
      leftMost  = std::min(leftMost, scalarProjection);
      rightMost = std::max(rightMost, scalarProjection);
   }
   UtVec3d left   = mean + (orth * leftMost);
   UtVec3d right  = mean + (orth * rightMost);
   leftLocWCS[0]  = left[0];
   leftLocWCS[1]  = left[1];
   leftLocWCS[2]  = left[2];
   rightLocWCS[0] = right[0];
   rightLocWCS[1] = right[1];
   rightLocWCS[2] = right[2];
   dist           = (left - right).Magnitude();
}


double UtClusterManager::DistanceBetween(UtClusterObject& aObject, std::vector<UtClusterObject*>& aClusters)
{
   double dist = 0.0;
   double clusterWCS[3];
   UtClusterManager::MeanLocWCS(aClusters, clusterWCS);

   if (mDistanceType == c2D_POSITION_ONLY)
   {
      // take off altitude difference, only 2d difference
      dist = GroundRange(aObject, clusterWCS);
   }
   else
   {
      // take 3d difference
      dist = SlantRange(aObject, clusterWCS);
   }

   // double dist = 0.0;
   // if (mDistanceType == cPOSITION_VELOCITY)
   //{
   //    if (aCluster.VelocityValid() && aObject.VelocityValid())
   //    {
   //       double cVelWCS[3];
   //       double tVelWCS[3];
   //       aCluster.MeanVelWCS(cVelWCS);
   //       aObject.GetVelocityWCS(tVelWCS);
   //       //scale the velocity difference
   //       double maxVelDiff = UtVec3d::Magnitude(cVelWCS) + UtVec3d::Magnitude(tVelWCS);
   //       double scale = 0.0;
   //       if (mDistanceLimit > 0.0)
   //       {
   //          //at a maximum, the distance will be increased by 25% of the distance-limit
   //          scale = 0.25 * mDistanceLimit / maxVelDiff;
   //       }
   //       else
   //       {
   //          //at a maximum, the distance will be increased by 15% of the current distance
   //          scale = 0.15 * dist / maxVelDiff;
   //       }
   //       dist += scale * SlantRange(cVelWCS, tVelWCS);
   //    }
   // }
   return dist;
}


void UtClusterManager::ClusterKMeans(std::size_t                                 aClusterLimit,
                                     std::vector<UtClusterObject*>&              aClusterObjects,
                                     std::vector<std::vector<UtClusterObject*>>& aClusters)
{
   std::map<std::string, int>     idToClusterIndex;
   std::vector<UtClusterObject*>* clusterPtr = nullptr;

   // run K-Means through eight iterations at max
   // will probably run fewer than eight, because of quiescence
   int    MaxIters = 8;
   size_t N        = aClusterObjects.size();

   aClusters.clear();
   aClusters.resize(aClusterLimit);
   // initialize the K clusters with some entities
   // K is always <= N, so this is safe
   for (size_t i = 0; i < aClusterLimit; ++i)
   {
      UtClusterObject* objectPtr = aClusterObjects[i];
      aClusters[i].push_back(objectPtr);
      idToClusterIndex[objectPtr->Id()] = static_cast<int>(i);
   }
   for (size_t i = aClusterLimit; i < N; ++i)
   {
      idToClusterIndex[aClusterObjects[i]->Id()] = -1;
   }

   bool clustersChanged = true;
   // run K-Means for MaxIters or until quiescence has been reached
   for (int tries = 0; tries < MaxIters && clustersChanged == true; ++tries)
   {
      clustersChanged = false;
      for (size_t i = 0; i < N; ++i)
      {
         UtClusterObject* objectPtr = aClusterObjects[i];
         // assign each entity to cluster [mean] that is closest
         int closestIndex = -1;

         double minDist = std::numeric_limits<double>::max();
         for (size_t j = 0; j < aClusterLimit; ++j)
         {
            clusterPtr = &aClusters[j];
            double d   = DistanceBetween(*objectPtr, *clusterPtr);

            if (d < minDist)
            {
               minDist      = d;
               closestIndex = static_cast<int>(j);
            }
         }

         int curIndex = idToClusterIndex[objectPtr->Id()];
         if (curIndex != closestIndex)
         {
            if (curIndex >= 0)
            {
               std::vector<UtClusterObject*>& c = aClusters[curIndex];
               c.erase(remove(c.begin(), c.end(), objectPtr), c.end());
            }
            aClusters[closestIndex].push_back(objectPtr);
            idToClusterIndex[objectPtr->Id()] = closestIndex;
            clustersChanged                   = true;
         }
      }
   }
}

void UtClusterManager::ClusterHierarchicalTree(double                                      aClusterDistLimit,
                                               const std::vector<UtClusterObject*>&        aClusterObjects,
                                               std::vector<std::vector<UtClusterObject*>>& aClusters)
{
   // Place each cluster object into its own cluster collection
   for (UtClusterObject* it : aClusterObjects)
   {
      aClusters.emplace_back(1, it);
   }

   std::size_t numClustersRemaining = aClusters.size(); // Clusters remaining to combine

   // Return if there are no clusters to combine
   if (numClustersRemaining <= 1)
   {
      return;
   }

   // Until either there are no two clusters such that their distances
   // are below the specified limit, or there is not a single remaining
   // cluster, continue trying to combine clusters
   double clusterDistMin;
   do
   {
      std::vector<UtClusterObject*>* cluster1Ptr =
         &aClusters.front(); // As a default, use a reference to the beginning of the cluster collection
      std::vector<UtClusterObject*>* cluster2Ptr =
         &aClusters.back(); // As a default, use a reference to the end of the cluster collection
      clusterDistMin = std::numeric_limits<double>::max(); // As a default, use the maximum limit

      // Find the two closest clusters
      for (std::size_t i = 0; i < numClustersRemaining; ++i)
      {
         for (std::size_t j = 0; j < i; ++j)
         {
            double clusterDist = DistanceBetween(aClusters[i], aClusters[j]);
            if (clusterDist < clusterDistMin)
            {
               cluster1Ptr    = &aClusters[i];
               cluster2Ptr    = &aClusters[j];
               clusterDistMin = clusterDist;
            }
         }
      }

      // If two clusters were found, the mininum distance will not be
      // the default; move all objects from cluster2 to cluster1
      if (clusterDistMin <= aClusterDistLimit)
      {
         // Append all references to cluster objects in cluster2 to cluster1
         cluster1Ptr->insert(cluster1Ptr->end(), cluster2Ptr->begin(), cluster2Ptr->end());

         // Delete the cluster from the cluster collection that cluster2 is referencing
         auto it = std::remove(aClusters.begin(), aClusters.end(), *cluster2Ptr);
         aClusters.erase(it, aClusters.end());
      }

      // Update remaining cluster count
      numClustersRemaining = aClusters.size();
   } while (clusterDistMin <= aClusterDistLimit && numClustersRemaining > 1);
}

void UtClusterManager::ClusterHierarchicalTreeOptimized(double                                      aClusterDistLimit,
                                                        std::vector<UtClusterObject*>&              aClusterObjects,
                                                        std::vector<std::vector<UtClusterObject*>>& aClusters)
{
   size_t          N = aClusterObjects.size();
   ProximityMatrix mat(N, N);
   size_t          i;
   size_t          j;

   // start with N clusters and then join them together (agglomerative, building)
   for (i = 0; i < N; ++i)
   {
      UtClusterObject* objectPtr = aClusterObjects[i];
      aClusters.emplace_back(1, objectPtr);
      for (j = 0; j < N; ++j)
      {
         if (i == j)
         {
            mat.Set(i, j, std::numeric_limits<double>::max());
         }
         else
         {
            UtClusterObject* objectPtr2 = aClusterObjects[j];
            mat.Set(i, j, DistanceBetween(*objectPtr, *objectPtr2));
         }
      }
   }

   size_t C = aClusters.size(); // == N;  // C stands for current number of clusters left to join
   if (C <= 1)
   {
      return; //  here
   }
   size_t r      = 0;
   size_t c      = 0;
   double lowest = mat.GetMinIndices(r, c); // this gives us the two indices of the nearest clusters

   while (lowest <= aClusterDistLimit && C > 1)
   {
      // adjust the proximity matrix for joining these two clusters
      if (mClusterMethod == cHIERARCHICAL_TREE_COMPLETE)
      {
         // c == specific column, r == specific row, i = variable
         // copy max( m(i,r), m(i,c) ) to r(i,r), then delete all m(i,c)
         for (i = 0; i < mat.Rows(); ++i)
         {
            mat.Set(i, r, std::max(mat.Get(i, r), mat.Get(i, c)));
         }
         mat.DeleteColumn(c);

         // copy max( m(r,i), m(c,i) ) to r(r,i), then delete all m(c,i)
         for (i = 0; i < mat.Cols(); ++i)
         {
            mat.Set(r, i, std::max(mat.Get(r, i), mat.Get(c, i)));
         }
         mat.DeleteRow(c); // yes, delete the row that cluster indexed at 'c' owns, each cluster owns a row & a column
      }
      else // mClusterMethod==cHIERARCHICAL_TREE_SINGLE
      {
         // c == specific column, r == specific row, i = variable
         // copy min( m(i,r), m(i,c) ) to r(i,r), then delete all m(i,c)
         for (i = 0; i < mat.Rows(); ++i)
         {
            mat.Set(i, r, std::min(mat.Get(i, r), mat.Get(i, c)));
         }
         mat.DeleteColumn(c);

         // copy min( m(r,i), m(c,i) ) to r(r,i), then delete all m(c,i)
         for (i = 0; i < mat.Cols(); ++i)
         {
            mat.Set(r, i, std::min(mat.Get(r, i), mat.Get(c, i)));
         }
         mat.Set(r, r, std::numeric_limits<double>::max()); // put the max place holder back in, for distance to itself
         mat.DeleteRow(c); // yes, delete the row that cluster indexed at 'c' owns, each cluster owns a row & a column
      }

      // join the two clusters
      aClusters[r].insert(aClusters[r].end(), aClusters[c].begin(), aClusters[c].end());
      // remove & erase the second cluster
      aClusters.erase(aClusters.begin() + c);

      C      = aClusters.size();        // C stands for current number of clusters left to join
      lowest = mat.GetMinIndices(r, c); // this gives us the two indices of the nearest clusters
   }
}

void UtClusterManager::ClusterHierarchicalTree(std::size_t                                 aClusterLimit,
                                               const std::vector<UtClusterObject*>&        aClusterObjects,
                                               std::vector<std::vector<UtClusterObject*>>& aClusters)
{
   // Place each cluster object into its own cluster collection
   for (UtClusterObject* it : aClusterObjects)
   {
      aClusters.emplace_back(1, it);
   }

   std::size_t numClustersRemaining = aClusters.size(); // Clusters remaining to combine

   // Until there are the specified number clusters
   // remaining, continue trying to combine clusters
   while (numClustersRemaining > aClusterLimit)
   {
      std::vector<UtClusterObject*>* cluster1Ptr =
         &aClusters.front(); // As a default, use a reference to the beginning of the cluster collection
      std::vector<UtClusterObject*>* cluster2Ptr =
         &aClusters.back(); // As a default, use a reference to the end of the cluster collection
      double clusterDistMin = std::numeric_limits<double>::max(); // As a default, use the maximum limit

      // Find the two closest clusters
      for (std::size_t i = 0; i < numClustersRemaining; ++i)
      {
         for (std::size_t j = 0; j < i; ++j)
         {
            double clusterDist = DistanceBetween(aClusters[i], aClusters[j]);
            if (clusterDist < clusterDistMin)
            {
               cluster1Ptr    = &aClusters[i];
               cluster2Ptr    = &aClusters[j];
               clusterDistMin = clusterDist;
            }
         }
      }

      // Append all references to cluster objects in cluster2 to cluster1
      cluster1Ptr->insert(cluster1Ptr->end(), cluster2Ptr->begin(), cluster2Ptr->end());

      // Delete the cluster from the cluster collection that cluster2 is referencing
      auto it = std::remove(aClusters.begin(), aClusters.end(), *cluster2Ptr);
      aClusters.erase(it, aClusters.end());

      // Update remaining cluster count
      numClustersRemaining = aClusters.size();
   }
}

void UtClusterManager::ClusterHierarchicalTreeOptimized(std::size_t                                 aClusterLimit,
                                                        std::vector<UtClusterObject*>&              aClusterObjects,
                                                        std::vector<std::vector<UtClusterObject*>>& aClusters)
{
   size_t          N = aClusterObjects.size();
   ProximityMatrix mat(N, N);
   size_t          i;
   size_t          j;

   // start with N clusters and then join them together (agglomerative, building)
   for (i = 0; i < N; ++i)
   {
      UtClusterObject* objectPtr = aClusterObjects[i];
      aClusters.emplace_back(1, objectPtr);
      for (j = 0; j < N; ++j)
      {
         if (i == j)
         {
            mat.Set(i, j, std::numeric_limits<double>::max());
         }
         else
         {
            UtClusterObject* objectPtr2 = aClusterObjects[j];
            mat.Set(i, j, DistanceBetween(*objectPtr, *objectPtr2));
         }
      }
   }

   size_t C = aClusters.size(); // == N;  // C stands for current number of clusters left to join

   while (C > aClusterLimit)
   {
      size_t r = 0;
      size_t c = 0;
      mat.GetMinIndices(r, c); // this gives us the two indices of the nearest clusters
      if (r == c)
      {
         break;
      }
      // adjust the proximity matrix for joining these two clusters
      if (mClusterMethod == cHIERARCHICAL_TREE_COMPLETE)
      {
         // copy max( m(i,r), m(i,c) ) to r(i,r), then delete all m(i,c)
         for (i = 0; i < mat.Rows(); ++i)
         {
            mat.Set(i, r, std::max(mat.Get(i, r), mat.Get(i, c)));
         }
         mat.DeleteColumn(c);

         // copy max( m(r,i), m(c,i) ) to r(r,i), then delete all m(c,i)
         for (i = 0; i < mat.Cols(); ++i)
         {
            mat.Set(r, i, std::max(mat.Get(r, i), mat.Get(c, i)));
         }
         mat.DeleteRow(c); // yes, delete the row that cluster indexed at 'c' owns, each cluster owns a row & a column
      }
      else // mClusterMethod==cHIERARCHICAL_TREE_SINGLE
      {
         // copy min( m(i,r), m(i,c) ) to r(i,r), then delete all m(i,c)
         for (i = 0; i < mat.Rows(); ++i)
         {
            mat.Set(i, r, std::min(mat.Get(i, r), mat.Get(i, c)));
         }
         mat.DeleteColumn(c);

         // copy min( m(r,i), m(c,i) ) to r(r,i), then delete all m(c,i)
         for (i = 0; i < mat.Cols(); ++i)
         {
            mat.Set(r, i, std::min(mat.Get(r, i), mat.Get(c, i)));
         }
         mat.Set(r, r, std::numeric_limits<double>::max()); // put the max place holder back in, for distance to itself
         mat.DeleteRow(c); // yes, delete the row that cluster indexed at 'c' owns, each cluster owns a row & a column
      }

      // join the two clusters
      aClusters[r].insert(aClusters[r].end(), aClusters[c].begin(), aClusters[c].end());
      // remove & erase the second cluster
      // delete &clusters[c]; //TODO - this necessary any more?
      aClusters.erase(aClusters.begin() + c);

      C = aClusters.size(); // C stands for current number of clusters left to join
   }
}


//! Performs a clustering algorithms on the entities passed in.
//! Erases existing clusters and creates new clusters to store in 'mClusters'.
//! Algorithm options include:
//! -> K-Mean clustering
//! -> H-Tree clustering (complete, single, or average linkage)
void UtClusterManager::GetClusters(std::vector<UtClusterObject*>&              aObjects,
                                   std::vector<std::vector<UtClusterObject*>>& aClusters)
{
   // clear any pre-existing clusters
   for (auto& clusters : aClusters)
   {
      for (auto& cluster : clusters)
      {
         delete cluster;
      }
      clusters.clear();
   }
   aClusters.clear();

   size_t N = aObjects.size();
   if (N == 0)
   {
      return;
   }
   // assign number of clusters, use mNumClusters if valid
   size_t K = std::min(mNumClusters, N); // can't have more clusters than entities
   if (K == 0)                           // uh oh, K not valid yet, lets just guess
   {
      K = (size_t)std::max(sqrt((double)N) + 0.5, 1.0); // best guess at proper number of clusters
   }

   switch (mClusterMethod)
   {
   case cHIERARCHICAL_TREE_AVERAGE:
   {
      if (mDistanceLimit > 0.0)
      {
         // cluster with a stopping point based on the minimum distance allowed between a cluster and it's members
         ClusterHierarchicalTree(mDistanceLimit, aObjects, aClusters);
      }
      else
      {
         // cluster with a stopping point based on the desired number of clusters
         ClusterHierarchicalTree(K, aObjects, aClusters);
      }
      break;
   }
   case cHIERARCHICAL_TREE_COMPLETE:
   case cHIERARCHICAL_TREE_SINGLE:
   {
      if (mDistanceLimit > 0.0)
      {
         // cluster with a stopping point based on the minimum distance allowed between a cluster and it's members
         ClusterHierarchicalTreeOptimized(mDistanceLimit, aObjects, aClusters);
      }
      else
      {
         // cluster with a stopping point based on the desired number of clusters
         ClusterHierarchicalTreeOptimized(K, aObjects, aClusters);
      }
      break;
   }
   case cK_MEANS:
   default:
      ClusterKMeans(K, aObjects, aClusters);
      break;
   }
}
