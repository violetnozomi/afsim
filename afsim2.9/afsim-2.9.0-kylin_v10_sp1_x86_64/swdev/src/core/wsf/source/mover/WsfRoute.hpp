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

#ifndef WSFROUTE_HPP
#define WSFROUTE_HPP

#include "wsf_export.h"

#include <memory>
#include <string>
#include <vector>

class UtInput;
class UtInputBlock;
#include "UtLLPos.hpp"
#include "UtLog.hpp"
class UtScriptClass;
class UtScriptRef;
class UtScriptTypes;

#include "WsfAuxDataEnabled.hpp"
#include "WsfObject.hpp"
#include "WsfStringId.hpp"
#include "WsfWaypoint.hpp"

//! A collection of WsfWaypoint objects that represent a path to be followed.

class WSF_EXPORT WsfRoute : public WsfObject, public WsfAuxDataEnabled
{
public:
   static constexpr const char* cTYPE_KIND = "route";

   using WaypointVector    = std::vector<WsfWaypoint>;
   using WayptVecIter      = std::vector<WsfWaypoint>::iterator;
   using WaypointPtrVector = std::vector<WsfWaypoint*>;
   using WaypointSegment   = std::pair<WsfWaypoint, WsfWaypoint>;

   struct RouteIntersection
   {
      int     mWaypointIndex1;
      int     mWaypointIndex2;
      UtLLPos mPoint;
   };

   WsfRoute() = default;
   WsfRoute(const std::string& aName);
   WsfRoute(const WsfRoute& aSrc);
   ~WsfRoute() override;

   WsfRoute&          operator=(const WsfRoute& aRhs);
   WsfWaypoint&       operator[](size_t aIndex);
   const WsfWaypoint& operator[](size_t aIndex) const;

   WsfRoute*    Clone() const override;
   virtual bool Initialize(double aSimTime = 0.0);
   bool         ProcessInput(UtInput& aInput) override;

   const char* GetScriptClassName() const override;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   static std::unique_ptr<UtScriptClass> CreateIteratorScriptClass(const std::string& aClassName,
                                                                   UtScriptTypes*     aScriptTypesPtr);

   void Append(const WsfWaypoint& aWaypoint);
   void Append(const WsfRoute& aRoute);

   void Insert(WayptVecIter aIter, const WsfWaypoint& aWaypoint);
   void Insert(int aIndex, const WsfWaypoint& aWaypoint);

   void Remove(unsigned int aWaypointIndex);

   void Transform(double aRefLat, double aRefLon, double aHeading);

   void TransformNED(double aNorth, double aEast, double aDown);

   void GetSubroute(const WsfWaypoint& aFromWaypoint, const WsfWaypoint& aToWaypoint, WsfRoute& aRoute) const;

   void GetSubroute(int aStartIndex, int aEndIndex, WsfRoute& aRoute) const;

   void AppendSubroute(int aStartIndex, int aEndIndex, WsfRoute& aRoute) const;

   void GetAllNodes(WaypointPtrVector& aWaypointPtrVector);

   int GetAdjacentNodeIndices(int aWaypointIndex, int* aAdjacentNodeIndices) const;

   int FindWaypointIndex(double aLat, double aLon, double aAlt);
   int FindWaypointIndex(const WsfWaypoint& aWpt);

   bool IsBetweenWaypoints(const double       aPointLL[2],
                           const unsigned int aFirstWaypointIndex,
                           const unsigned int aSecondWaypointIndex,
                           const double       aTolerancePercentage = 0.01) const;

   double GetDistance(int aFromWaypointIndex, int aToWaypointIndex) const;

   double GetDistanceOffset(const double aFromLL[2], const double aToLL[2], const double aPointLL[2], double& aOffsetDist) const;

   double GetDistanceAlongRoute(const double aFromLL[2], const double aToLL[2]) const;

   double GetDistanceAlongRoute(const double aFromLL[2], const double aToLL[2], double& aOffsetDist) const;

   bool GetEndPointsAtDistance(double aDistance, int& aWaypointIndex) const;

   static bool ProjectOntoRoute(const double aPointLL[2],
                                const double aSegmentStartLL[2],
                                const double aSegmentEndLL[2],
                                double       aWaypointLL[2]);

   bool PointAlongRoute(double aDistanceFromStart, double aPointAlongRoute[2], int& aSegmentIndex);

   bool ProjectOntoRoute(const double aPointLL[2], int& aSegmentIndex, double aProjectedLL[2], double& aDistance) const;

   bool ProjectOntoRoute(const double aPointLL[2], double& aDistanceFromRoute, double& aDistanceAlongRoute) const;

   int ProjectOntoRouteFast(const double aPointLL[2], double& aDistanceFromRoute, double& aDistanceAlongRoute) const;

   bool ProjectOntoRouteGivenClosestWaypoint(double aLat,
                                             double aLon,
                                             int    aClosestWaypointIndex,
                                             int&   aSegmentIndex,
                                             double aProjectedLL[2]);

   int Intersect(WsfRoute& aOtherRoute, std::vector<RouteIntersection>& aIntersections);

   /** Get the total distance along all waypoints in the route.
      @returns The sum of the lengths between consecutive waypoints in the route.
   */
   double GetTotalLength() const { return mTotalLength; }

   double GetTravelTime() const;

   double GetTravelTimeToPosition(const double aPosition[2]) const;

   WsfWaypoint&       GetWaypointAt(size_t aIndex);
   const WsfWaypoint& GetWaypointAt(size_t aIndex) const;

   unsigned int GetWaypointIndexAt(WsfStringId aLabelId) const;

   bool IsGlobalRoute() const;

   void SetNetworkRouteId(unsigned int aRouteId);

   /** Get the network route Id associated with this route.
      @returns The route Id associated with this route.
   */
   unsigned int GetNetworkRouteId() const { return mRouteId; }

   void SetSpeed(double aSpeed);

   //! Get the speed at the requested waypoint on a route
   //! @return The speed at a specific waypoint along a route.
   double GetSpeed(unsigned int aIndex) const { return mWaypoints[aIndex].GetSpeed(); };

   //! Gets the vector of the actual waypoints
   //! @return Returns the WaypointVector mWaypoints
   const WaypointVector& GetWaypoints() const { return mWaypoints; }

   //! Return an iterator to the first waypoint in the route.
   WayptVecIter Begin() { return mWaypoints.begin(); }

   //! Return an iterator to the end of the vector
   //! (i.e., whether the iterator is past the last waypoint in the vector).
   WayptVecIter End() { return mWaypoints.end(); }

   //! Remove all waypoints from a route.
   void Clear();

   //! Determine whether the route contains any waypoints.
   //! @returns 'true' if the route contains waypoints; 'false' otherwise.
   bool Empty() const { return mWaypoints.empty(); }

   //! Get the number of waypoints in the route.
   //! @returns The number of waypoints in the route.
   unsigned int GetSize() const { return static_cast<unsigned int>(mWaypoints.size()); }

   //! Adds details to the notes of aStream.
   void Print(ut::log::MessageStream& aStream) const;

   //! Swap two WsfRoute's efficiently
   void Swap(WsfRoute& aRhs)
   {
      std::swap(mRouteId, aRhs.mRouteId);
      std::swap(mRouteId, aRhs.mRouteId);
      std::swap(mWaypoints, aRhs.mWaypoints);
      std::swap(mTotalLength, aRhs.mTotalLength);
      std::swap(mWaypointReferenceCountPtr, aRhs.mWaypointReferenceCountPtr);
      std::swap(mTrackedReferencePtr, aRhs.mTrackedReferencePtr);
   }

   //! Returns the reference count for waypoints owned by this route.
   //! If the route is resized, waypoint references are invalidated.
   //! This allows scripts to access waypoint references safely.
   UtReferenceCount* GetWaypointReferenceCount()
   {
      if (mWaypointReferenceCountPtr == nullptr)
      {
         mWaypointReferenceCountPtr = new UtReferenceCount(1);
      }
      return mWaypointReferenceCountPtr;
   }

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfObject::Serialize(aBuff);
      aBuff& mRouteId& mWaypoints& mLabelId& mTotalLength;
   }

   UtScriptRef* ScriptRef();
   UtScriptRef* ConstScriptRef() const;
   UtScriptRef* ScriptRefManage();

   static void ProcessInputBlock(UtInputBlock& aInputBlock, WsfRoute* aRoutePtr, bool aAllowName);
   bool        ProcessNavigationInput(UtInput& aInput);
   void        ProcessWaypoint(UtInput& aInput, WsfWaypoint& aWaypoint);

private:
   //! A simple struct representing a route segment on the curved surface of the earth.
   class Segment
   {
   public:
      Segment() = default;
      Segment(double aLat1, double aLon1, double aLat2, double aLon2)
         : mBegin(aLat1, aLon1)
         , mEnd(aLat2, aLon2)
      {
      }

      bool operator<(const Segment& aRhs) { return mBegin.GetLat() < aRhs.mBegin.GetLat(); }

      UtLLPos mBegin;
      UtLLPos mEnd;
   };
   using SegmentList = std::vector<Segment>;

   //! Defines an intersection location
   struct Intersection
   {
      //! Indices of the intersecting line segments.
      //! as passed into the IntersectLines method.
      int     mSegmentIndices[2];
      Segment mSegments[2]; // The actual line segments as passed in.
      UtLLPos mPoint;
   };
   using IntersectionList = std::vector<Intersection>;

   double ComputeDistance(unsigned int& aFirstWptIndex, unsigned int& aSecondWptIndex);
   void   ClearWaypointReference()
   {
      if (mWaypointReferenceCountPtr != nullptr)
      {
         mWaypointReferenceCountPtr->RemoveStrongRef();
         mWaypointReferenceCountPtr = nullptr;
      }
   }

   double CalculateTurnWaypointDistance(const WsfWaypoint& aWaypoint) const;

   double ProcessTurnWaypoint(const WsfWaypoint& aWaypoint, double aNewHeading, double aCurrentHeading);

   // void UpdateRouteIdentifier();

   static void IntersectSegments(const SegmentList& aSegmentList, IntersectionList& aIntersections);

   static bool GreatCircleIntersect(const Segment& aSegment1, const Segment& aSegment2, UtLLPos& aInteceptPosition);

   unsigned int   mRouteId = 0;              // Identifier in the context of a route network
   WaypointVector mWaypoints{};              // The actual waypoints
   bool           mOffsetRouteValid = false; // Convert offset waypoints to be relative to a single origin
                                             // rather than the last waypoint when a route is tranformed.

   WsfStringId       mLabelId{}; // Current label ID during reading.
   double            mTotalLength               = 0.0;
   UtReferenceCount* mWaypointReferenceCountPtr = nullptr;

}; // class WsfRoute

// Specialize std::swap() for WsfRoute
namespace std
{
template<>
inline void swap(WsfRoute& aLhs, WsfRoute& aRhs)
{
   aLhs.Swap(aRhs);
}

} // namespace std

#endif
