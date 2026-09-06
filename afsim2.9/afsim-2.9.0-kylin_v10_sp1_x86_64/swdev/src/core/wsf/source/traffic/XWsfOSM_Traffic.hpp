// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef XWSFOSMTRAFFIC_HPP
#define XWSFOSMTRAFFIC_HPP

#include "wsf_export.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class UtInput;
#include "UtCallbackHolder.hpp"

class WsfMover;
#include "WsfOSM_TrafficInput.hpp"
class WsfPlatform;
class WsfRoadMover;
#include "WsfRoute.hpp"
class WsfRouteNetwork;
#include "WsfSimulationExtension.hpp"

//! XWsfOSM_Traffic provides basic background traffic capability. Based on the provided input
//! file, a specified number of ground vehicles are created and placed randomly on the specified
//! road network. The road networks themselves should be generated via the OSM Converter, but user-generated
//! road networks can still be used. This implementation relies on route and waypoint aux_data to specify important
//! details such as speed limits, route density (how congested individual routes should be on average),
//! intersection restraints and road type. The initial number of vehicles is always maintained, so whenever
//! a vehicle reaches a route endpoint without intersections, it is placed at some other endpoint without an
//! intersection heading in the opposite direction.
class WSF_EXPORT XWsfOSM_Traffic : public WsfSimulationExtension, public WsfOSM_TrafficInput
{
public:
   struct SGroundTraffic
   {
      int          mIdentifier{0}; //! Unique identifier for ground vehicle
      WsfStringId  mVehicleTypeId; //! Vehicle type
      bool         mAlive{false};  //! Is this vehicle alive?
      int          mNetwork{-1};   //! Index value of network vehicle is traveling on
      double       mSpeed{0.0};    //! Speed (meters/sec)
      WsfPlatform* mRoadPlatform{nullptr};
      size_t       mRoadPlatformIndex{0};
      bool         mConvoyVehicle{false}; //! Was this vehicle defined as a convoy vehicle?
      std::string  mCurrentRouteName;
      int          mConvoyNumber{-1};
      bool         mCatchUp{false};
      bool         mAtIntersection{false};

      //! scripted vehicles need to be added for tracking purposes (collisions, etc.),
      //! but their behavior should not be controlled by the background traffic code
      bool mScripted{false};
   };

   enum class IntersectionRestriction
   {
      cNONE,
      cSTOP,
      cTRAFFIC
   };

   class QueuedOperation
   {
   public:
      QueuedOperation(int aVehicleId, WsfRoute& aNewRoute, double aTime, bool aJump = false)
         : mVehicleId(aVehicleId)
         , mNewRoute(aNewRoute)
         , mAddedTime(aTime)
         , mJump(aJump)
      {
      }

      int      mVehicleId;
      WsfRoute mNewRoute;
      double   mAddedTime;
      bool     mJump;
   };

   class QueuedConvoyOperation : public QueuedOperation
   {
   public:
      QueuedConvoyOperation(double aPrecedingTime, int aVehicleId, WsfRoute& aNewRoute, double aTime, bool aJump = false)
         : QueuedOperation(aVehicleId, aNewRoute, aTime, aJump)
         , mPrecedingTime(aPrecedingTime)
      {
      }

      double mPrecedingTime;
   };

   class QueuedSpeedChange
   {
   public:
      QueuedSpeedChange(int aVehicleId, unsigned int aIndex, double aSpeed, double aTime)
         : mVehicleId(aVehicleId)
         , mIndex(aIndex)
         , mSpeed(aSpeed)
         , mTime(aTime)
      {
      }

      int          mVehicleId;
      unsigned int mIndex;
      double       mSpeed;
      double       mTime;
   };

   class WeightedRouteEdge
   {
   public:
      WeightedRouteEdge(int aWeight, unsigned int aRouteIndex, unsigned int aWaypointIndex)
         : mWeight(aWeight)
         , mRouteIndex(aRouteIndex)
         , mWaypointIndex(aWaypointIndex)
      {
      }

      int          mWeight;
      unsigned int mRouteIndex;
      unsigned int mWaypointIndex;
   };

   struct RoutePair
   {
   public:
      RoutePair(const std::string& aSource, const std::string& aDestination, const std::string& aNodeId)
         : mSourceRoute(aSource)
         , mDestinationRoute(aDestination)
         , mNodeId(aNodeId)
      {
      }

      bool operator==(const RoutePair& aOther) const
      {
         return (mSourceRoute == aOther.mSourceRoute) && (mDestinationRoute == aOther.mDestinationRoute) &&
                (mNodeId == aOther.mNodeId);
      }

      std::string mSourceRoute;
      std::string mDestinationRoute;
      std::string mNodeId;
   };

   struct RoutePairHash
   {
      std::size_t operator()(const RoutePair& aNode) const
      {
         return ((std::hash<std::string>()(aNode.mSourceRoute) ^ (std::hash<std::string>()(aNode.mDestinationRoute) << 1)) >>
                 1) ^
                (std::hash<std::string>()(aNode.mNodeId) << 1);
      }
   };

   class WeightedRouteNode
   {
   public:
      WeightedRouteNode(const std::string& aId)
         : mId(aId)
      {
      }

      bool operator==(const WeightedRouteNode& aOther) const { return (mId == aOther.mId); }

      std::string                            mId;
      mutable std::vector<WeightedRouteEdge> mEdges;
      mutable std::vector<std::string>       mSourceRoutes; // string = base route name
      double                                 mWaitTime{0.0};
      IntersectionRestriction                mIntersection{IntersectionRestriction::cNONE};
   };

   struct WeightedRouteNodeHash
   {
      std::size_t operator()(const WeightedRouteNode& aNode) const { return (std::hash<std::string>()(aNode.mId)); }
   };

   class TrackerDetails
   {
   public:
      TrackerDetails(int aId, double aDistance)
         : mVehicleId(aId)
         , mDistanceAlongRoute(aDistance)
      {
      }
      bool operator<(const TrackerDetails& aDetail2) const
      {
         if (mDistanceAlongRoute == aDetail2.mDistanceAlongRoute)
         {
            return mVehicleId > aDetail2.mVehicleId;
         }
         return (mDistanceAlongRoute < aDetail2.mDistanceAlongRoute);
      }
      int    mVehicleId;
      double mDistanceAlongRoute;
   };

   class QueuedRoadRelease
   {
   public:
      QueuedRoadRelease(const std::string& aName, double aTime, bool aRoad)
         : mName(aName)
         , mTime(aTime)
         , mRoad(aRoad)
      {
      }

      bool operator<(const QueuedRoadRelease& aDetail2) const { return mTime > aDetail2.mTime; }

      std::string mName;
      double      mTime;
      bool        mRoad;
   };

   XWsfOSM_Traffic(WsfOSM_TrafficInput& aData);
   ~XWsfOSM_Traffic() override;
   XWsfOSM_Traffic(const XWsfOSM_Traffic& aRoadTraffic) = delete;
   XWsfOSM_Traffic& operator=(const XWsfOSM_Traffic& aRoadTraffic) = delete;

   void AddedToSimulation() override;
   bool Initialize() override;

   static XWsfOSM_Traffic* Find(const WsfSimulation& aSimulation);

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void AdvanceTime(double aSimTime);

   // Script accessors
   std::vector<WsfRoute*>    GetRoutesOnNetwork(const std::string& aNetworkName);
   WsfRoute*                 GetIndividualRoute(const std::string& aNetworkName,
                                                const std::string& aBaseName,
                                                int                aLaneNumber    = 1,
                                                bool               aDirection     = true,
                                                int                aSegmentNumber = 1);
   std::vector<WsfPlatform*> GetPlatformsAlongRoute(const std::string& aNetworkName, const std::string& aRouteName);
   WsfWaypoint* GetNextWaypoint(const std::string& aNetworkname, const std::string& aRouteName, double aDistance);
   std::string  GetTrafficFlow(double aSimTime, const std::string& aNetworkName, const std::string& aNodeId);
   std::string  GetNodeOnRoute(const std::string& aNetworkName, const std::string& aRouteName, bool aFirstOrLast);
   std::vector<WsfRoute*> GetConnectedRoutes(const std::string& aNetworkName, const std::string& aNodeId);

   bool AddScriptedVehicle(WsfPlatform* aPlatformPtr, const std::string& aNetworkName, WsfRoute* aRoute, int& aVehicleNumber);

   bool AddScriptedConvoy(const std::vector<WsfPlatform*> aPlatformList,
                          const std::string&              aNetworkName,
                          WsfRoute*                       aRoute,
                          double                          aInitialSpacing);

   void UpdateScriptedVehicleRoute(WsfPlatform* aPlatformPtr, WsfRoute* aRoute, int aWaypointIndex = 0);

   bool ChangeScriptedLane(WsfPlatform* aPlatformPtr, bool aDirection);

   bool ScriptLockIntersection(const std::string& aNetworkName, const std::string& aNodeId);
   bool ScriptUnlockIntersection(const std::string& aNetworkName, const std::string& aNodeId);
   void ScriptGoOffroad(WsfPlatform* aVehicle, const WsfGeoPoint& aGeoPoint);

private:
   //! Stores relevant information for each network in the road traffic
   struct TrafficInformation
   {
      //! The modified road network that this network corresponds to.
      std::vector<std::unique_ptr<WsfRoute>> mRoadRoutes;


      //! Graph corresponding to modified road network.
      std::unordered_set<WeightedRouteNode, WeightedRouteNodeHash> mRoadGraphs;

      //! Key = route id, value = vector of vehicle ids on the route.
      std::unordered_map<std::string, std::unordered_set<unsigned int>>         mRouteVehicleTracker;
      std::unordered_map<std::string, std::unordered_set<unsigned int>>         mAdjacentLanes;
      std::unordered_map<RoutePair, std::deque<QueuedOperation>, RoutePairHash> mQueuedOperations;

      //! Queued operations for convoys changing routes.
      std::unordered_map<int, std::deque<QueuedOperation>>   mQueuedConvoys;
      std::unordered_map<int, std::deque<QueuedSpeedChange>> mQueuedConvoySpeeds; //! Queued background convoy speed
                                                                                  //! changes (GoToSpeed with an offset
                                                                                  //! time parameter does not work)


      std::unordered_map<std::string, unsigned int> mIntersectionsInUse; //! Key = node/intersection id, value = convoy
                                                                         //! number reserving the intersection: background
                                                                         //! convoys (after the 1st vehicle) can ignore
                                                                         //! traffic (e.g. light turns red after the first
                                                                         //! vehicle of the convoy passes but before the
                                                                         //! rest of the convoy passes).
      std::unordered_map<std::string, std::unordered_map<unsigned int, double>>
         mAtIntersection; //! Key = route name, value = map of vehicle number to projected distance along route.
                          //! The projection is necessary because vehicles can be still moving/deceleration before
                          //! stopping at the intersection.
      std::unordered_set<std::string> mReservedRoads; //! List of reserved roads connected to the edge of the map: will
                                                      //! prevent vehicles from entering the road.
      std::deque<QueuedRoadRelease> mQueuedRoadRelease; //! Queued operation to un-reserve roads or intersections.
   };

   void AddPlatformToSimulation(double aSimTime, int aVehicleNumber);

   int  SlotNumberHandler(WsfStringId aVehicleTypeId);
   int  AllocateVehicle(WsfStringId aVehicleTypeId);
   void ClearVehicleList();
   bool CreateVehicle(const RoadTrafficNetworkInput& aNetworkInput,
                      double                         aCreateTime,
                      int&                           aVehicleNumber,
                      bool                           aAtSource = false);

   bool CreateConvoyVehicles(const RoadTrafficNetworkInput& aNetworkInput, double aCreateTime);

   void DeletePlatformFromSimulation(double aSimTime, int aVehicleNumber);

   bool ValidateVehicleTypes(RoadTrafficNetworkInput& aNetwork);

   void BuildSplitRoads(unsigned int aNetIndex, RoadTrafficNetworkInput& aNetwork);
   void BuildWeightedGraph(unsigned int aNetIndex, RoadTrafficNetworkInput& aNetwork);
   bool ValidateAuxData(RoadTrafficNetworkInput& aNetwork);
   bool CheckNetworkValidity(unsigned int aNetIndex) const;
   void CreateInitialPathForVehicle(const RoadTrafficNetworkInput& aNetwork,
                                    int                            aVehicleNumber,
                                    const std::string&             aRoute    = "",
                                    double                         aDistance = 0.0);

   double      GetRandomSpeed(const WsfRoute&        aRoute,
                              const RouteAuxMapping& aRouteAuxMap,
                              double                 aVehicleSpeed,
                              bool                   aIgnoreSpeedLimit);
   void        AssignRandomSpeeds(WsfRoute&              aRoute,
                                  const RouteAuxMapping& aRouteAuxMap,
                                  double                 aVehicleSpeed,
                                  bool                   aIgnoreSpeedLimit,
                                  bool                   aConvoy = false);
   std::string GetRandomSourceRoute(unsigned int aNetworkId) const;
   void        VehiclePathEndedHandler(double aSimTime, WsfMover* aMover);
   void        PlatformDeletedHandler(double aSimTime, WsfPlatform* aPlatform);
   void        CatchUpHandler(double aSimTime, int aVehicleNumber, double aCatchupSpeed);
   void        ConvoyCatchUpHandler(double aSimTime, int aConvoyNumber, unsigned int aIndex, double aCatchupSpeed);

   void TrackerCreationHelper(std::vector<TrackerDetails>& aTracker, unsigned int aVehicleNumber);

   // Lane Change Helper Functions
   bool CheckLaneAvailability(int                                aVehicleNumber,
                              unsigned int                       aLaneIndex,
                              const std::vector<TrackerDetails>& aTracker,
                              double                             aDistance,
                              double*                            aNextPoint);
   void ChangeLane(int aVehicleNumber, unsigned int aLaneIndex, double aDistance, const double* aNextPoint);

   WsfRoute CopyOriginalRoute(unsigned int aNetworkId, unsigned int aRouteIndex, unsigned int aWaypointIndex);


   //! Advance Time Helper Functions
   //! The aCurrentRoute parameter refers to the entire road that the vehicle is on, not the mover's actual route
   //@{
   const double cSPACING = 4.0;
   void         HandleRegularOperations(double aSimTime);
   void         HandleRoadRelease(double aSimTime);
   //@}

   //! These two functions relate to functionality that is low-priority. It may be re-visited at a later time
   //@{
   void HandleQueuedConvoy(double aSimTime);
   void HandleConvoySpeedChange(double aSimTime);
   //@}

   void GoToSpeedHelper(double              aSimTime,
                        WsfMover*           aMover,
                        double              aSpeed,
                        double              aDifference,
                        int                 aVehicleNumber,
                        TrafficInformation& aNetworkTraffic);
   bool ApproachIntersectionDecelerate(double                             aSimTime,
                                       bool                               aDecelerate,
                                       const WsfRoute*                    aCurrentRoute,
                                       WsfMover*                          aMover,
                                       double                             aCurrentDistance,
                                       const std::vector<TrackerDetails>& aVehiclesOnRoute,
                                       int                                aIndex,
                                       TrafficInformation&                aNetworkTraffic);
   bool HandleStoppedVehicles(double                             aSimTime,
                              bool                               aHandleStop,
                              const WsfRoute*                    aCurrentRoute,
                              WsfMover*                          aMover,
                              double                             aCurrentDistance,
                              const std::vector<TrackerDetails>& aVehiclesOnRoute,
                              int                                aIndex,
                              TrafficInformation&                aNetworkTraffic);
   bool HandleCollisions(double                                                              aSimTime,
                         bool                                                                aHandleCollisions,
                         WsfRoute*                                                           aCurrentRoute,
                         WsfMover*                                                           aMover,
                         double                                                              aCurrentDistance,
                         const std::unordered_map<std::string, std::vector<TrackerDetails>>& aDetailedTracker,
                         const std::vector<TrackerDetails>&                                  aVehiclesOnRoute,
                         int                                                                 aIndex,
                         TrafficInformation&                                                 aNetworkTraffic);
   void ModifyVehicleSpeed(double                             aSimTime,
                           bool                               aModifySpeed,
                           RoadTrafficNetworkInput*           aNetworkPtr,
                           WsfRoute*                          aCurrentRoute,
                           WsfMover*                          aMover,
                           const std::vector<TrackerDetails>& aVehiclesOnRoute,
                           int                                aIndex,
                           TrafficInformation&                aNetworkTraffic);

   std::vector<SGroundTraffic> mTraffic;
   UtCallbackHolder            mCallbacks;
   int                         mNextVehicleIdentifier{0};
   int                         mNumVehiclesActive{0};
   int                         mMaxVehiclesActive{0};
   double                      mNextUpdateTime{0.0}; //! Simulation time when the next update
   double                      mNextConvoyUpdateTime{std::numeric_limits<double>::max()};
   double                      mNextConvoySpeedTime{std::numeric_limits<double>::max()};
   double                      mNextConvoyCatchUpTime{std::numeric_limits<double>::max()};
   double                      mNextRoadReleaseTime{std::numeric_limits<double>::max()};

   std::deque<QueuedSpeedChange> mQueuedCatchUp; //! Queued operations for when a background convoy vehicle needs to catch up
   std::unordered_map<int, std::vector<int>> mConvoyIndices; //! Keeps track of which vehicles are parts of which convoys
   std::unordered_set<int>                   mCatchUpVehicles;
   std::unordered_set<unsigned int> mStoppedVehicles; //! List of vehicles that are currently stopped outside of intersections
   std::unordered_map<std::string, unsigned int>        mNetworkToIndex;
   std::unordered_map<unsigned int, TrafficInformation> mRoadInfo; //! unsigned int key = netIndex from vehicle's mNetwork,
                                                                   //! value = information associated with the network
};

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

//! Define script methods for WsfOSM_Traffic.
class WSF_EXPORT WsfScriptOSM_TrafficClass : public UtScriptClass
{
public:
   WsfScriptOSM_TrafficClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Traffic);
   UT_DECLARE_SCRIPT_METHOD(TrafficRoutes);
   UT_DECLARE_SCRIPT_METHOD(IndividualRoute);
   UT_DECLARE_SCRIPT_METHOD(VehiclesAlongRoute);
   UT_DECLARE_SCRIPT_METHOD(NextPointInRoute);
   UT_DECLARE_SCRIPT_METHOD(IntersectionFlow);
   UT_DECLARE_SCRIPT_METHOD(IntersectionOnRoute);
   UT_DECLARE_SCRIPT_METHOD(GetConnectedRoads);

   UT_DECLARE_SCRIPT_METHOD(AddVehicleToTraffic);
   UT_DECLARE_SCRIPT_METHOD(AddConvoyToTraffic);
   UT_DECLARE_SCRIPT_METHOD(FollowRoute_1);
   UT_DECLARE_SCRIPT_METHOD(FollowRoute_2);
   UT_DECLARE_SCRIPT_METHOD(ChangeLane);
   UT_DECLARE_SCRIPT_METHOD(LockIntersection);
   UT_DECLARE_SCRIPT_METHOD(UnlockIntersection);
   UT_DECLARE_SCRIPT_METHOD(GoOffroad);
};

#endif
