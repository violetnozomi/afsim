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

#include "traffic/XWsfRoadTraffic.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <memory>
#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtProcessorTime.hpp"
#include "UtRandom.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoadMover.hpp"
#include "WsfRouteNetwork.hpp"
#include "WsfRouteNetworkTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"
#include "WsfStringId.hpp"
#include "WsfWaypoint.hpp"

namespace
{

class RoadTrafficScenarioExtension : public WsfScenarioExtension
{
public:
   ~RoadTrafficScenarioExtension() override = default;

   void SimulationCreated(WsfSimulation& aSimulation) override;
   bool ProcessInput(UtInput& aInput) override;

private:
   bool ProcessRoadTraffic(UtInput& aInput);
   bool ProcessNetworkBlock(UtInput& aInput, XWsfRoadTraffic::RoadTrafficNetworkInput& aNetwork);
   void ProcessVehicleBlock(UtInput& aInput, XWsfRoadTraffic::VehicleInput& aVehicle);
   void ProcessWeightedRegionBlock(UtInput& aInput, XWsfRoadTraffic::WeightedRegionInput& aWeightedRegion);
   void ProcessConvoyBlock(UtInput& aInput, XWsfRoadTraffic::ConvoyInput& aConvoyInput);

   XWsfRoadTraffic::Data mData;
};

// ================================================================================
// public virtual
void RoadTrafficScenarioExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<XWsfRoadTraffic>(mData));
}

} // namespace

void WSF_EXPORT Register_road_traffic(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("road_traffic"))
   {
      aApplication.RegisterExtension("road_traffic",
                                     ut::make_unique<WsfDefaultApplicationExtension<RoadTrafficScenarioExtension>>());
   }
}

// virtual
XWsfRoadTraffic::~XWsfRoadTraffic()
{
   if (mData.mMaxVehiclesActive > 0)
   {
      auto out = ut::log::debug() << "road_traffic: Debug dump:";
      out.AddNote() << "Maximum vehicle array size: " << mData.mTraffic.size();
      out.AddNote() << "Maximum number of active vehicles: " << mData.mMaxVehiclesActive;
      out.AddNote() << "Current number of active vehicles: " << mData.mNumVehiclesActive;
      ClearVehicleList();
   }
}

// Initialize the road traffic manager.
// virtual
bool XWsfRoadTraffic::Initialize()
{
   // If we aren't using the road traffic simulation, return true
   // so we don't cause an initialization error.
   if (!mData.mProcessedInput)
   {
      return true;
   }

   if (!mData.Initialize(GetSimulation().GetScenario()))
   {
      return false;
   }

   mCallbacks.Add(WsfObserver::AdvanceTime(&GetSimulation()).Connect(&XWsfRoadTraffic::AdvanceTime, this));

   bool         ok = true;
   unsigned int netIndex;

   mData.mNextVehicleIdentifier = 0;

   ClearVehicleList();

   if (mData.GetNetworks().empty())
   {
      // No roads...
      mData.mNextUpdateTime = 1.0E+20;
      return false;
   }

   std::vector<WsfRouteNetwork*> routeNetworkPtrs(mData.GetNetworks().size());

   // Verify input data that cannot be done until Initialize()
   // a) Requested road networks are defined
   // b) Requested vehicle (platform) types are defined
   // c) Build the weighted route segment list.

   for (netIndex = 0; netIndex < mData.GetNetworks().size(); ++netIndex)
   {
      RoadTrafficNetworkInput& netInput    = static_cast<RoadTrafficNetworkInput&>(*mData.GetNetworks()[netIndex]);
      std::string              networkName = mData.GetNetworks()[netIndex]->mNetworkId;

      routeNetworkPtrs[netIndex] = netInput.mRouteNetworkPtr;
      if (netInput.mRouteNetworkPtr != nullptr)
      {
         if (!ValidateVehicleTypes(netInput))
         {
            ok = false;
         }
      }
      else
      {
         auto out = ut::log::error() << "road_traffic: Undefined network.";
         out.AddNote() << "Network: " << networkName;
         ok = false;
      }

      netInput.mWeightedRouteSegments.clear();
      BuildWeightedRouteSegmentList(*netInput.mRouteNetworkPtr, netInput);
   }

   if (ok)
   {
      // Create the vehicles.

      for (netIndex = 0; netIndex < mData.GetNetworks().size(); ++netIndex)
      {
         // Create the convoy vehicles first.
         RoadTrafficNetworkInput* networkPtr = static_cast<RoadTrafficNetworkInput*>(mData.GetNetworks()[netIndex].get());
         assert(networkPtr);

         int convoyCount = static_cast<int>(networkPtr->mConvoyInput.size());
         for (int convoyId = 0; convoyId < convoyCount; ++convoyId)
         {
            int vehOffset     = 0;
            int vehInputCount = static_cast<int>(networkPtr->mConvoyInput[convoyId].mVehicleInput.size());
            for (int vehInputId = 0; vehInputId < vehInputCount; ++vehInputId)
            {
               int vehCount = networkPtr->mConvoyInput[convoyId].mVehicleInput[vehInputId].mCount;
               for (int vehId = 0; vehId < vehCount; ++vehId)
               {
                  int vehicleNumber;
                  if (CreateConvoyVehicle(networkPtr->mConvoyInput[convoyId].mVehicleInput[vehInputId].mTypeId,
                                          vehicleNumber))
                  {
                     // Stagger the pause times to achieve an equally spaced convoy.
                     double pauseTime = vehOffset * networkPtr->mConvoyInput[convoyId].mPauseTime;
                     ++vehOffset;

                     // Initialize the convoy vehicle.
                     WsfMover* moverPtr = GetMover(mData.mTraffic[vehicleNumber].mRoadPlatform);
                     if (moverPtr != nullptr)
                     {
                        WsfRoadMover* roadMoverPtr = dynamic_cast<WsfRoadMover*>(moverPtr);
                        if (roadMoverPtr != nullptr)
                        {
                           roadMoverPtr->SetRoadNetworkId(networkPtr->mNetworkId);
                           roadMoverPtr->SetStartLat(networkPtr->mConvoyInput[convoyId].mStartLatitude);
                           roadMoverPtr->SetStartLon(networkPtr->mConvoyInput[convoyId].mStartLongitude);
                           roadMoverPtr->SetEndLat(networkPtr->mConvoyInput[convoyId].mEndLatitude);
                           roadMoverPtr->SetEndLon(networkPtr->mConvoyInput[convoyId].mEndLongitude);
                           roadMoverPtr->SetSpeed(networkPtr->mConvoyInput[convoyId].mSpeed);
                           roadMoverPtr->SetPauseTime(pauseTime);
                           roadMoverPtr->SetUseClosestWaypoint(networkPtr->mConvoyInput[convoyId].mUseClosestWaypoint);
                        }
                        AddPlatformToSimulation(0.0, vehicleNumber);
                     }
                     else
                     {
                        auto out = ut::log::error() << "road_traffic: Could not initialize convoy vehicle because an "
                                                       "invalid mover type was used.";
                        out.AddNote() << "Make sure to use either a WSF_ROAD_MOVER or WSF_GROUND_MOVER.";
                        ok = false;
                        break;
                     }
                  }
               }
            }
         }

         // Next, create the background traffic.

         int vehicleCount = mData.GetNetworks()[netIndex]->mVehicleCount;

         for (int vehicleIndex = 0; vehicleIndex < vehicleCount; ++vehicleIndex)
         {
            int vehicleNumber;
            if (CreateVehicle(static_cast<RoadTrafficNetworkInput&>(*mData.GetNetworks()[netIndex]), 0.0, vehicleNumber))
            {
               // The vehicle we created might have a WsfRoadMover mover, which needs to have additional attributes set
               WsfRoadMover* moverPtr =
                  dynamic_cast<WsfRoadMover*>(mData.mTraffic[vehicleNumber].mRoadPlatform->GetMover());
               if (moverPtr != nullptr)
               {
                  moverPtr->SetRoadNetworkId(mData.GetNetworks()[netIndex]->mNetworkId);
                  moverPtr->SetSpeed(mData.mTraffic[vehicleNumber].mSpeed);
               }
               AddPlatformToSimulation(0.0, vehicleNumber);
            }
         }
      }
   }

   // Force Update() to do its thing the first time through.  This must be done after the
   // above because AddPlatformToSimulation() updates mData.mNextUpdateTime.

   mData.mNextUpdateTime = 0.0;

   if (!ok)
   {
      ut::log::error() << "road_traffic: Unable to initialize.";
      throw WsfSimulation::InitializeError();
   }

   return true;
}

//! Create a list of all the edges in the route network graph.  Calculate a weight for each
//! edge based on the average of its two node endpoint weights.  Calculate the node weights
//! as the maximum weight contributed by the weighted regions in the network. Sort the edge
//! list in reverse order, normalize the weights, and calculate a cumulative weight for each
//! edge in the edge list based on the order they appear in the list.  Once this is built we
//! can perform a random draw and look up which edge the value corresponds to.  This allows
//! us to randomly distribute bodies on the road network relative to the weighted regions.
// private
void XWsfRoadTraffic::BuildWeightedRouteSegmentList(WsfRouteNetwork& aRouteNetwork, RoadTrafficNetworkInput& aNetInput)
{
   unsigned int segId;

   // Build the initial weighted route segment list and compute the total length of the network.

   WsfRoute::WaypointPtrVector nodes;
   aNetInput.mTotalDist = 0.0;
   // For each route.
   for (unsigned int routeId = 0; routeId < aRouteNetwork.GetRouteCount(); ++routeId)
   {
      aRouteNetwork[routeId].GetAllNodes(nodes);
      aNetInput.mTotalDist += aRouteNetwork[routeId].GetTotalLength();

      // For each node in the route.
      for (unsigned int nodeId = 0; nodeId < nodes.size(); ++nodeId)
      {
         unsigned int frId = nodeId;
         unsigned int toId = frId + 1;
         if (toId < nodes.size())
         {
            // Create and add a weighted route segment to the list.
            aNetInput.mWeightedRouteSegments.push_back(
               WeightedRouteSegment(&aRouteNetwork[routeId], *nodes[frId], *nodes[toId]));
         }
      }
   }

   // Calculate the node weights based on the weighted regions defined in this network.
   // For each weighted route segment.

   double maxWeight[2];
   double weight;
   double totalWeight = 0.0;
   for (segId = 0; segId < aNetInput.mWeightedRouteSegments.size(); ++segId)
   {
      maxWeight[0] = 1.0;
      maxWeight[1] = 1.0;

      // For each weighted region.
      for (unsigned int regId = 0; regId < aNetInput.mWeightedRegionInput.size(); ++regId)
      {
         // Weight of Node1 due to the weighted region.
         weight =
            CalculateNodeWeight(aNetInput.mWeightedRegionInput[regId], aNetInput.mWeightedRouteSegments[segId].mNode1);

         // We only care about the maximum.
         maxWeight[0] = std::max(weight, maxWeight[0]);

         // Weight of Node2 due to the weighted region.
         weight =
            CalculateNodeWeight(aNetInput.mWeightedRegionInput[regId], aNetInput.mWeightedRouteSegments[segId].mNode2);

         // We only care about the maximum.
         maxWeight[1] = std::max(weight, maxWeight[1]);
      }

      // Keep the maximum weight defined above as the node weight.
      aNetInput.mWeightedRouteSegments[segId].mNode1Weight = maxWeight[0];
      aNetInput.mWeightedRouteSegments[segId].mNode2Weight = maxWeight[1];

      // Calculate the segment edge weight as the average of the two nodes' weights.
      aNetInput.mWeightedRouteSegments[segId].mSegmentWeight = (maxWeight[0] + maxWeight[1]) / 2.0;

      totalWeight += aNetInput.mWeightedRouteSegments[segId].mSegmentWeight;
   }

   // Normalize each segment's edge weight based on the totalWeight.
   if (totalWeight >= 0.0)
   {
      for (segId = 0; segId < aNetInput.mWeightedRouteSegments.size(); ++segId)
      {
         aNetInput.mWeightedRouteSegments[segId].mSegmentWeight /= totalWeight;
      }
   }

   // Add a final "dummy" segment.
   aNetInput.mWeightedRouteSegments.push_back(WeightedRouteSegment());

   // Sort the list in decreasing order.
   sort(aNetInput.mWeightedRouteSegments.begin(), aNetInput.mWeightedRouteSegments.end());
   reverse(aNetInput.mWeightedRouteSegments.begin(), aNetInput.mWeightedRouteSegments.end());

   // For each segment compute the cumulative weight.
   for (segId = 1; segId < aNetInput.mWeightedRouteSegments.size(); ++segId)
   {
      aNetInput.mWeightedRouteSegments[segId].mCumulativeWeight =
         (aNetInput.mWeightedRouteSegments[segId - 1].mSegmentWeight +
          aNetInput.mWeightedRouteSegments[segId - 1].mCumulativeWeight);
   }
}

//! Calculate the node weight as the maximum contribution of any weighted regions that the node
//! falls within.  The weighted regions provide an inner and outer radius relative to a location.
//! If a node falls within the bounds of one of these weighted regions a weight is assigned to the node.
//! The weight is a linear interpolation between the inner and outer weights and the node's location
//! relative to the inner and outer radii.
double XWsfRoadTraffic::CalculateNodeWeight(const WeightedRegionInput& aWeightedRegion, const WsfWaypoint& aNode)
{
   double weight = 0.0;

   double heading;
   double distance;
   UtSphericalEarth::GreatCircleHeadingAndDistance(aWeightedRegion.mLatitude,
                                                   aWeightedRegion.mLongitude,
                                                   aNode.GetLat(),
                                                   aNode.GetLon(),
                                                   heading,
                                                   distance);

   // Calculate the weight of the first node.
   if ((distance >= aWeightedRegion.mInnerRadius) && (distance < aWeightedRegion.mOuterRadius))
   {
      // Linearly interpolate the weight between the values at the inner and outer radius.
      weight = ((distance - aWeightedRegion.mInnerRadius) / (aWeightedRegion.mOuterRadius - aWeightedRegion.mInnerRadius));
      weight *= fabs(aWeightedRegion.mOuterWeight - aWeightedRegion.mInnerWeight);
      weight += aWeightedRegion.mInnerWeight;
   }

   return weight;
}

// private
void XWsfRoadTraffic::AddPlatformToSimulation(double aSimTime, int aVehicleNumber)
{
   // Add the platform to the simulation.

   WsfPlatform* platformPtr = mData.mTraffic[aVehicleNumber].mRoadPlatform;
   assert(platformPtr != nullptr);
   if (mData.mDebug)
   {
      auto out = ut::log::debug() << "road_traffic: Adding vehicle to simulation.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Vehicle: " << platformPtr->GetName();
      out.AddNote() << "Vehicle Number: " << aVehicleNumber;
   }

   if (GetSimulation().AddPlatform(aSimTime, platformPtr))
   {
      mData.mNextUpdateTime = std::min(mData.mTraffic[aVehicleNumber].mOffRoadTime, mData.mNextUpdateTime);
      mData.mTraffic[aVehicleNumber].mRoadPlatformIndex = platformPtr->GetIndex();
      ++mData.mNumVehiclesActive;
      mData.mMaxVehiclesActive = std::max(mData.mNumVehiclesActive, mData.mMaxVehiclesActive);
   }
}

//! Allocate a vehicle and return its index within the mData.mTraffic array.
// private
int XWsfRoadTraffic::AllocateVehicle(WsfStringId aVehicleTypeId)
{
   std::string vehicleType = aVehicleTypeId;

   // Determine if there is an available slot...
   //
   // A slot is 'available' if it does not have a RoadPlatform assigned to it.
   // This routine does not create the RoadPlatform so technically the slot is
   // still available until the entry gets a RoadPlatform assigned to it.

   int          vehicleNumber = -1;
   unsigned int i;
   unsigned int trafficSize = static_cast<unsigned int>(mData.mTraffic.size());
   for (i = 0; i < trafficSize; ++i)
   {
      if (mData.mTraffic[i].mRoadPlatform == nullptr)
      {
         vehicleNumber = i;
         break;
      }
   }

   // If there are no unused slots then allocate a new one...

   if (vehicleNumber < 0)
   {
      SGroundTraffic vehicle;
      mData.mTraffic.push_back(vehicle);
      vehicleNumber = static_cast<int>(mData.mTraffic.size() - 1);
      if (mData.mDebug)
      {
         auto out = ut::log::debug() << "road_traffic: Allocating new vehicle using new slot.";
         out.AddNote() << "Vehicle Number: " << vehicleNumber;
         out.AddNote() << "Type: " << vehicleType;
      }
   }
   else if (mData.mDebug)
   {
      auto out = ut::log::debug() << "road_traffic: Allocating new vehicle using old slot.";
      out.AddNote() << "Vehicle Number: " << vehicleNumber;
      out.AddNote() << "Type: " << vehicleType;
   }

   // Initialize vehicle fields

   mData.mTraffic[vehicleNumber].mIdentifier        = ++mData.mNextVehicleIdentifier;
   mData.mTraffic[vehicleNumber].mVehicleTypeId     = aVehicleTypeId;
   mData.mTraffic[vehicleNumber].mAlive             = true;
   mData.mTraffic[vehicleNumber].mVehicleOffRoad    = false;
   mData.mTraffic[vehicleNumber].mStationary        = false;
   mData.mTraffic[vehicleNumber].mNetwork           = -1;
   mData.mTraffic[vehicleNumber].mSpeed             = 0.0;
   mData.mTraffic[vehicleNumber].mTravelTime        = 0.0;
   mData.mTraffic[vehicleNumber].mStartTime         = 0.0;
   mData.mTraffic[vehicleNumber].mDeadTime          = 0.0;
   mData.mTraffic[vehicleNumber].mOffRoadTime       = 0.0;
   mData.mTraffic[vehicleNumber].mRoadPlatformIndex = 0;
   mData.mTraffic[vehicleNumber].mConvoyVehicle     = false;

   // Instantiate a platform of the proper type

   mData.mTraffic[vehicleNumber].mRoadPlatform = (WsfPlatform*)GetScenario().CloneType("platform_type", vehicleType);
   if (mData.mTraffic[vehicleNumber].mRoadPlatform == nullptr)
   {
      { // RAII block
         auto out = ut::log::error() << "road_traffic: Invalid platform type.";
         out.AddNote() << "Type: " << vehicleType;
      }
      assert(mData.mTraffic[vehicleNumber].mRoadPlatform != nullptr);
      return vehicleNumber;
   }

   // Set the name of the platform
   std::ostringstream oss;
   oss << vehicleType << '_' << mData.mTraffic[vehicleNumber].mIdentifier;
   mData.mTraffic[vehicleNumber].mRoadPlatform->SetName(oss.str());

   return vehicleNumber;
}

// private
void XWsfRoadTraffic::ClearVehicleList()
{
   mData.mTraffic.clear();

   mData.mNumVehiclesActive = 0;
   mData.mMaxVehiclesActive = 0;
}

//! Get a random waypoint on the route network.  Waypoints are randomly placed on
//! the weighted regions defined by the current route network.
//! Returns true if successful, false otherwise.
// private
bool XWsfRoadTraffic::GetRandomWaypoint(const RoadTrafficNetworkInput& aNetwork,
                                        WsfWaypoint&                   aWaypoint,
                                        WsfWaypoint&                   aRouteSegEndWpt1,
                                        WsfWaypoint&                   aRouteSegEndWpt2)
{
   bool ok = false;

   if (!aNetwork.mWeightedRouteSegments.empty())
   {
      double randomWeight = GetSimulation().GetRandom().Uniform<double>();

      // find the weighted route segment that encloses randomWeight.
      auto itr = find(aNetwork.mWeightedRouteSegments.begin(), aNetwork.mWeightedRouteSegments.end(), randomWeight);
      if (itr != aNetwork.mWeightedRouteSegments.end())
      {
         // Get the segment length.
         double segmentLength =
            itr->mRoutePtr->GetDistance(itr->mNode1.GetPositionInRoute(), itr->mNode2.GetPositionInRoute());
         double wStart = itr->mCumulativeWeight;
         double wEnd   = wStart + itr->mSegmentWeight;

         // Linearly interpolate to get the actual position into this segment.
         double distIntoNodeSegment = ((randomWeight - wStart) / (wEnd - wStart)) * segmentLength;

         // Get the endpoints that enclose the distance into to WsfRoute.
         double distIntoRoute = itr->mRoutePtr->GetDistance(0, itr->mNode1.GetPositionInRoute());
         distIntoRoute += distIntoNodeSegment;
         int segmentIndex;
         if (itr->mRoutePtr->GetEndPointsAtDistance(distIntoRoute, segmentIndex))
         {
            aRouteSegEndWpt1 = itr->mRoutePtr->GetWaypointAt(segmentIndex);
            aRouteSegEndWpt2 = itr->mRoutePtr->GetWaypointAt(segmentIndex + 1);
            double distIntoSegment =
               distIntoNodeSegment - itr->mRoutePtr->GetDistance(itr->mNode1.GetPositionInRoute(), segmentIndex);

            // Interpolate to determine the actual point.
            double heading;
            double distance;
            UtSphericalEarth::GreatCircleHeadingAndDistance(aRouteSegEndWpt1.GetLat(),
                                                            aRouteSegEndWpt1.GetLon(),
                                                            aRouteSegEndWpt2.GetLat(),
                                                            aRouteSegEndWpt2.GetLon(),
                                                            heading,
                                                            distance);

            double lat, lon;
            UtSphericalEarth::ExtrapolateGreatCirclePosition(aRouteSegEndWpt1.GetLat(),
                                                             aRouteSegEndWpt1.GetLon(),
                                                             heading,
                                                             distIntoSegment,
                                                             lat,
                                                             lon);
            aWaypoint.SetLat(lat);
            aWaypoint.SetLon(lon);
            aWaypoint.SetRouteId(itr->mRoutePtr->GetNetworkRouteId());

            ok = true;
         }
      }
   }
   return ok;
}

//! Creates a random path on the specified network.
//! Returns true if successful, false otherwise.
// private
bool XWsfRoadTraffic::CreateRandomPath(const RoadTrafficNetworkInput& aNetwork, WsfRoute& aRoute)
{
   bool        ok = false;
   WsfWaypoint wpt1;       // First random point.
   WsfWaypoint wpt1Seg[2]; // The route segment that encloses the first random point.
   WsfWaypoint wpt2;       // Second random point.
   WsfWaypoint wpt2Seg[2]; // The route segment that encloses the second random point.

   // Get a pointer to the WsfRouteNetwork.
   std::string      networkName     = aNetwork.mNetworkId;
   WsfRouteNetwork* routeNetworkPtr = WsfRouteNetworkTypes::Get(GetScenario()).Find(networkName);
   assert(routeNetworkPtr);

   // Get 2 random points on the road network.
   if (GetRandomWaypoint(aNetwork, wpt1, wpt1Seg[0], wpt1Seg[1]) &&
       GetRandomWaypoint(aNetwork, wpt2, wpt2Seg[0], wpt2Seg[1]))
   {
      if (!mData.GetNetworks().empty())
      {
         if (routeNetworkPtr->GeneratePathOffRouteToRoute(wpt1.GetLat(), wpt1.GetLon(), wpt2.GetLat(), wpt2.GetLon(), aRoute))
         {
            // Make sure that the route isn't too short.
            if ((aRoute.GetTotalLength() > 100.0) || (aNetwork.mTotalDist < 1000.0))
            {
               ok = true;
            }
         }
      }
   }
   return ok;
}

//! Creates a random path for the specified vehicle on the specified network.
//! Returns the path length in meters or -1 if the path is invalid.
// private
double XWsfRoadTraffic::CreateRandomPathForVehicle(const RoadTrafficNetworkInput& aNetwork, int aVehicleNumber)
{
   double routeDist = -1.0;

   // Create a random path on the specified network.
   bool     ok    = false;
   int      count = 0;
   WsfRoute route;
   while (!ok)
   {
      ok = CreateRandomPath(aNetwork, route);

      // Don't loop forever.
      ++count;
      if (count >= 100)
      {
         break;
      }
   }

   if (ok)
   {
      // Copy the vehicle speed into each of the route's waypoints.
      for (unsigned int ii = 0; ii < route.GetSize(); ++ii)
      {
         route[ii].SetSpeed(mData.mTraffic[aVehicleNumber].mSpeed);
         route[ii].SetAlt(0.5);
      }

      routeDist = route.GetTotalLength();

      // Assign the random path to the appropriate mover.
      WsfMover* moverPtr = mData.mTraffic[aVehicleNumber].mRoadPlatform->GetMover();
      if (moverPtr != nullptr)
      {
         moverPtr->SetRoute(GetSimulation().GetSimTime(), route);
      }
      else
      {
         auto out = ut::log::error() << "road_traffic: Cannot set the mover's path because the mover is null.";
         out.AddNote() << "Platform: " << mData.mTraffic[aVehicleNumber].mRoadPlatform->GetName();

         routeDist = -1.0;
      }
   }
   return routeDist;
}

//! Calculates an offroad waypoint and updates the simulation object to use it.
// private
void XWsfRoadTraffic::GoOffRoad(const RoadTrafficNetworkInput& aNetwork, int aVehicleNumber)
{
   WsfMover* moverPtr = mData.mTraffic[aVehicleNumber].mRoadPlatform->GetMover();
   if (moverPtr != nullptr)
   {
      // Get our current position.
      double lat;
      double lon;
      double alt;
      moverPtr->GetPlatform()->GetLocationLLA(lat, lon, alt);

      // The angle to travel off road.
      double heading = GetSimulation().GetRandom().Uniform(0.0, 360.0);

      // The distance we should travel offroad.
      double distance = GetSimulation().GetRandom().Uniform(aNetwork.mMinDistOffRoad, aNetwork.mMaxDistOffRoad);

      // Calculate the lat, lon that corresponds to the heading and distance.
      double offRoadLat;
      double offRoadLon;
      UtSphericalEarth::ExtrapolateGreatCirclePosition(lat, lon, heading, distance, offRoadLat, offRoadLon);

      // Create a route with this waypoint and update the mover with it.
      WsfRoute route;
      route.Append(WsfWaypoint(offRoadLat, offRoadLon, 0.5, mData.mTraffic[aVehicleNumber].mSpeed));
      moverPtr->UpdateRoute(GetSimulation().GetSimTime(), route);
   }
}

// private
void XWsfRoadTraffic::DeletePlatformFromSimulation(double aSimTime, int aVehicleNumber)
{
   if (mData.mDebug)
   {
      auto out = ut::log::debug() << "road_traffic: Deleting vehicle.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Vehicle Number: " << aVehicleNumber;
   }

   // Delete the platform from the simulation (if it hasn't been broken)

   if (GetSimulation().PlatformExists(mData.mTraffic[aVehicleNumber].mRoadPlatformIndex))
   {
      GetSimulation().DeletePlatform(aSimTime, mData.mTraffic[aVehicleNumber].mRoadPlatform);
   }
   else
   {
      delete mData.mTraffic[aVehicleNumber].mRoadPlatform;
   }

   // Mark the vehicle slot as empty to it can be reused

   mData.mTraffic[aVehicleNumber].mIdentifier        = 0;
   mData.mTraffic[aVehicleNumber].mVehicleTypeId     = nullptr;
   mData.mTraffic[aVehicleNumber].mAlive             = false;
   mData.mTraffic[aVehicleNumber].mVehicleOffRoad    = false;
   mData.mTraffic[aVehicleNumber].mStationary        = false;
   mData.mTraffic[aVehicleNumber].mNetwork           = -1;
   mData.mTraffic[aVehicleNumber].mSpeed             = 0.0;
   mData.mTraffic[aVehicleNumber].mTravelTime        = 0.0;
   mData.mTraffic[aVehicleNumber].mStartTime         = 0.0;
   mData.mTraffic[aVehicleNumber].mDeadTime          = 0.0;
   mData.mTraffic[aVehicleNumber].mOffRoadTime       = 0.0;
   mData.mTraffic[aVehicleNumber].mRoadPlatform      = nullptr;
   mData.mTraffic[aVehicleNumber].mRoadPlatformIndex = 0;
   mData.mTraffic[aVehicleNumber].mConvoyVehicle     = false;

   --mData.mNumVehiclesActive;
   mData.mNumVehiclesActive = std::max(mData.mNumVehiclesActive, 0);
}

//! Reverse a vehicles route.
// private.
void XWsfRoadTraffic::ReverseVehicleRoute(int aVehicleNumber, double aSimTime)
{
   WsfPlatform* platformPtr = GetSimulation().GetPlatformByIndex(mData.mTraffic[aVehicleNumber].mRoadPlatformIndex);
   if ((platformPtr != nullptr) && (platformPtr->GetMover() != nullptr))
   {
      WsfMover*       moverPtr    = platformPtr->GetMover();
      const WsfRoute* oldRoutePtr = moverPtr->GetRoute();
      WsfRoute        newRoute;

      // Okay, this is really ugly. The current route may be much longer than what is needed. Simply reversing
      // the route will cause an immediate jump to the end of the route instead of just turning around.
      //
      // Therefore we have to find out where we are at and generate a reversal from this point.

      double notUsed;
      double curLL[2];
      platformPtr->GetLocationLLA(curLL[0], curLL[1], notUsed);

      int    begIndex;
      double projLL[2];
      double distance;
      if (oldRoutePtr && !oldRoutePtr->ProjectOntoRoute(curLL, begIndex, projLL, distance))
      {
         // ProjectOntoRoute can fail if the vehicle has traveled even slightly past the final point.
         // If this happens we just reverse the entire route.
         begIndex = oldRoutePtr->GetSize() - 1;
      }

      // If the current location and the location of the first waypoint to be copied are not nearly coincident,
      // make the first waypoint the current location.
      if (oldRoutePtr) // has route
      {
         const WsfWaypoint& begWpt = oldRoutePtr->GetWaypointAt(begIndex);
         UtSphericalEarth::GreatCircleHeadingAndDistance(curLL[0], curLL[1], begWpt.GetLat(), begWpt.GetLon(), notUsed, distance);
         if (distance > 1.0)
         {
            WsfWaypoint curWpt(curLL[0], curLL[1], 0.0, platformPtr->GetSpeed());
            // If the current speed is zero, the vehicle actually hit the end of the route and is paused.
            // If that is the case the speed at the last waypoint used in the current route is used.
            if (curWpt.GetSpeed() <= 0.0)
            {
               curWpt.SetSpeed(begWpt.GetSpeed());
            }
            newRoute.Append(curWpt);
         }
         // Copy the route that has been traversed, in the reverse order.
         for (int index = begIndex; index >= 0; --index)
         {
            newRoute.Append(oldRoutePtr->GetWaypointAt(index));
         }

         moverPtr->SetRoute(aSimTime, newRoute);
      }

      // NOTE: Any off-road time is included on the initial traversal of the route (even though reversal vehicles
      //       never travel off-road). On subsequent traversals (due to reversal), the off-road time and dead time
      //       are set to the total time of the initial traversal.

      double travelTime = mData.mTraffic[aVehicleNumber].mDeadTime - mData.mTraffic[aVehicleNumber].mStartTime;

      mData.mTraffic[aVehicleNumber].mAlive          = true;
      mData.mTraffic[aVehicleNumber].mVehicleOffRoad = false;
      mData.mTraffic[aVehicleNumber].mStationary     = false;
      mData.mTraffic[aVehicleNumber].mStartTime      = aSimTime;
      mData.mTraffic[aVehicleNumber].mOffRoadTime    = travelTime + aSimTime;
      mData.mTraffic[aVehicleNumber].mDeadTime       = travelTime + aSimTime;
   }
}

//! Generate a vehicle that will travel on the specified road network.
//! Returns 'true' if successful or 'false' if not.
// private
bool XWsfRoadTraffic::CreateVehicle(const RoadTrafficNetworkInput& aNetworkInput, double aCreateTime, int& aVehicleNumber)
{
   if (mData.mDebug)
   {
      auto out = ut::log::debug() << "road_traffic: Creating vehicle.";
      out.AddNote() << "T = " << aCreateTime;
   }

   // Determine which type of vehicle should be created on the road.
   // NOTE: If for some reason we don't explicitly select a vehicle,
   // the last one will automatically get selected.
   double r           = GetSimulation().GetRandom().Uniform<double>();
   double cumFraction = 0.0;
   assert(!aNetworkInput.mVehicleInput.empty());
   const VehicleInput* vehInputPtr = nullptr;
   for (unsigned int i = 0; i < aNetworkInput.mVehicleInput.size(); ++i)
   {
      vehInputPtr = &(aNetworkInput.mVehicleInput[i]);
      cumFraction += vehInputPtr->mFraction;
      if (r < cumFraction)
      {
         break;
      }
   }
   if (!vehInputPtr)
   {
      return false;
   }
   // Allocate a vehicle of the selected type
   aVehicleNumber = AllocateVehicle(vehInputPtr->mTypeId);
   int nv         = aVehicleNumber;

   // Get the network index that this vehicle is on.
   for (unsigned int netId = 0; netId < mData.GetNetworks().size(); ++netId)
   {
      if (mData.GetNetworks()[netId]->mNetworkId == aNetworkInput.mNetworkId)
      {
         mData.mTraffic[nv].mNetwork = netId;
         break;
      }
   }

   // Check to see if this vehicle is stationary.
   if ((fabs(vehInputPtr->mMeanSpeed) < 1.0E-10) && (fabs(vehInputPtr->mSigmaSpeed) < 1.0E-10))
   {
      mData.mTraffic[nv].mStationary = true;
      mData.mTraffic[nv].mSpeed      = 0.0;
   }
   else
   {
      // compute vehicle speed and limit it if a maximum is defined.
      double speed = -1.0;
      while (speed <= 0.0)
      {
         double r1 = GetSimulation().GetRandom().Gaussian();
         speed     = vehInputPtr->mMeanSpeed + (vehInputPtr->mSigmaSpeed * r1);
      }
      mData.mTraffic[nv].mSpeed = speed;
      if (aNetworkInput.mMaxSpeed > 1.0E-10)
      {
         mData.mTraffic[nv].mSpeed = std::min(speed, aNetworkInput.mMaxSpeed);
      }
   }

   // Initialize a stationary vehicle.
   if (mData.mTraffic[nv].mStationary)
   {
      WsfWaypoint wpt;
      WsfWaypoint endPt1;
      WsfWaypoint endPt2;
      if (GetRandomWaypoint(aNetworkInput, wpt, endPt1, endPt2))
      {
         // Set the stationary body's position.
         mData.mTraffic[nv].mRoadPlatform->SetLocationLLA(wpt.GetLat(), wpt.GetLon(), 0.5);

         // Calculate the body's heading based on the road segments heading.
         double heading;
         double distance;
         UtSphericalEarth::GreatCircleHeadingAndDistance(endPt1.GetLat(),
                                                         endPt1.GetLon(),
                                                         endPt2.GetLat(),
                                                         endPt2.GetLon(),
                                                         heading,
                                                         distance);

         mData.mTraffic[nv].mRoadPlatform->SetOrientationNED(heading * UtMath::cRAD_PER_DEG, 0.0, 0.0);

         double simEndTime               = GetSimulation().GetEndTime();
         mData.mTraffic[nv].mTravelTime  = simEndTime;
         mData.mTraffic[nv].mStartTime   = aCreateTime;
         mData.mTraffic[nv].mOffRoadTime = simEndTime;
         mData.mTraffic[nv].mDeadTime    = simEndTime;
      }
      else
      {
         { // RAII block
            auto out = ut::log::error() << "road_traffic: Unable to create a random waypoint for a stationary vehicle.";
            out.AddNote() << "Vehicle: " << mData.mTraffic[nv].mRoadPlatform->GetName();
         }
         DeletePlatformFromSimulation(aCreateTime, nv);
         return false;
      }
   }
   // Initialize a moving vehicle.
   else
   {
      // Create a random path for the vehicle.
      double routeDist     = -1.0;
      int    routeAttempts = 0;
      while ((routeDist <= 0.0) && (routeAttempts < 25))
      {
         ++routeAttempts;
         routeDist = CreateRandomPathForVehicle(aNetworkInput, nv);
      }
      if (routeDist < 0.0)
      {
         { // RAII block
            auto out = ut::log::error() << "road_traffic: Unable to create a random path.";
            out.AddNote() << "Vehicle: " << mData.mTraffic[nv].mRoadPlatform->GetName();
         }
         DeletePlatformFromSimulation(aCreateTime, nv);
         return false;
      }
      mData.mTraffic[nv].mRouteDist = routeDist;

      // Compute the time required to traverse the entire route.
      double routeTime = mData.mTraffic[nv].mRouteDist / mData.mTraffic[nv].mSpeed;

      // Compute vehicle travel time based on the user provided mean and sigma times.
      double travelTime = -1.0;
      while (travelTime <= 0.0)
      {
         double r1  = GetSimulation().GetRandom().Gaussian();
         travelTime = aNetworkInput.mMeanTravelTime + (aNetworkInput.mSigmaTravelTime * r1);
      }

      // Limit the travel time to the shorter of the "route time" or "mean time + sigma"
      travelTime = std::min(routeTime, travelTime);

      mData.mTraffic[nv].mTravelTime  = travelTime;
      mData.mTraffic[nv].mStartTime   = aCreateTime;
      mData.mTraffic[nv].mOffRoadTime = mData.mTraffic[nv].mTravelTime + aCreateTime;
      mData.mTraffic[nv].mDeadTime    = mData.mTraffic[nv].mOffRoadTime + aNetworkInput.mPauseTime;
   }

   return true;
}

//! Create a convoy vehicle of type aVehicleType. aVehicleNumber is the
//! vehicle's index into the mData.mTraffic array.
//! Returns true if successful and false otherwise.
bool XWsfRoadTraffic::CreateConvoyVehicle(WsfStringId aVehicleType, int& aVehicleNumber)
{
   // Allocate a vehicle of the selected type
   if (mData.mDebug)
   {
      ut::log::debug() << "road_traffic: Creating convoy vehicle.";
   }
   aVehicleNumber = AllocateVehicle(aVehicleType);

   double simEndTime                             = GetSimulation().GetEndTime();
   mData.mTraffic[aVehicleNumber].mConvoyVehicle = true;
   mData.mTraffic[aVehicleNumber].mTravelTime    = simEndTime;
   mData.mTraffic[aVehicleNumber].mStartTime     = 0.0;
   mData.mTraffic[aVehicleNumber].mOffRoadTime   = simEndTime;
   mData.mTraffic[aVehicleNumber].mDeadTime      = simEndTime;
   return true;
}

namespace
{

// ================================================================================
// public virtual
bool RoadTrafficScenarioExtension::ProcessInput(UtInput& aInput)
{
   if (aInput.GetCommand() == "road_traffic")
   {
      mData.mProcessedInput = true;
      ProcessRoadTraffic(aInput);
   }
   else
   {
      return false;
   }
   return true;
}

// ================================================================================
// private
bool RoadTrafficScenarioExtension::ProcessRoadTraffic(UtInput& aInput)
{
   bool myCommand = true;

   UtInputBlock inputBlock(aInput, "end_road_traffic");
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "network")
      {
         std::string networkName;
         aInput.ReadValue(networkName);
         WsfStringId networkId = networkName;

         // Note: Duplicate network specifications are allowed.  Each specification is treated
         // independently from other specifications.

         auto networkInputPtr = ut::make_unique<XWsfRoadTraffic::RoadTrafficNetworkInput>(networkId);
         ProcessNetworkBlock(aInput, *networkInputPtr);
         mData.GetNetworks().emplace_back(std::move(networkInputPtr));
      }
      else if (command == "verbose")
      {
         mData.mVerbose = true;
      }
      else if (command == "debug")
      {
         mData.mDebug = true;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   return myCommand;
}

// ================================================================================
// private
bool RoadTrafficScenarioExtension::ProcessNetworkBlock(UtInput& aInput, XWsfRoadTraffic::RoadTrafficNetworkInput& aNetwork)
{
   bool myCommand = true;

   UtInputBlock inputBlock(aInput, "end_network");
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "weighted_region")
      {
         aNetwork.mWeightedRegionInput.push_back(XWsfRoadTraffic::WeightedRegionInput());
         ProcessWeightedRegionBlock(aInput, aNetwork.mWeightedRegionInput.back());
      }
      else if (command == "end_of_path_option")
      {
         std::string endOfPathOption;
         aInput.ReadValue(endOfPathOption);
         if (endOfPathOption == "reverse_direction")
         {
            mData.mEndOfPathOption = XWsfRoadTraffic::cREVERSE_DIRECTION;
         }
         else if (endOfPathOption == "respawn")
         {
            mData.mEndOfPathOption = XWsfRoadTraffic::cRESPAWN;
         }
         else
         {
            throw UtInput::BadValue(aInput, "Invalid end of path option " + endOfPathOption);
         }
      }
      else if (command == "convoy")
      {
         aNetwork.mConvoyInput.push_back(XWsfRoadTraffic::ConvoyInput());
         ProcessConvoyBlock(aInput, aNetwork.mConvoyInput.back());
      }
      else if (command == "minimum_distance_off_road")
      {
         double distance;
         aInput.ReadValueOfType(distance, UtInput::cLENGTH);
         aInput.ValueGreater(distance, 0.0);
         aNetwork.mMinDistOffRoad = distance;
      }
      else if (command == "maximum_distance_off_road")
      {
         double distance;
         aInput.ReadValueOfType(distance, UtInput::cLENGTH);
         aInput.ValueGreater(distance, 0.0);
         aNetwork.mMaxDistOffRoad = distance;
      }
      else if (command == "pause_time_off_road")
      {
         double pauseTime;
         aInput.ReadValueOfType(pauseTime, UtInput::cTIME);
         aInput.ValueGreater(pauseTime, 0.0);
         aNetwork.mPauseTime = pauseTime;
      }
      else if (command == "maximum_speed")
      {
         double maxSpeed;
         aInput.ReadValueOfType(maxSpeed, UtInput::cSPEED);
         aInput.ValueGreater(maxSpeed, 0.0);
         aNetwork.mMaxSpeed = maxSpeed;
      }
      else if (command == "mean_travel_time")
      {
         double travelTime;
         aInput.ReadValueOfType(travelTime, UtInput::cTIME);
         aInput.ValueGreater(travelTime, 0.0);
         aNetwork.mMeanTravelTime = travelTime;
      }
      else if (command == "sigma_travel_time")
      {
         double travelTime;
         aInput.ReadValueOfType(travelTime, UtInput::cTIME);
         aInput.ValueGreaterOrEqual(travelTime, 0.0);
         aNetwork.mSigmaTravelTime = travelTime;
      }
      else if (command == "vehicle")
      {
         std::string vehicleType;
         aInput.ReadValue(vehicleType);
         // Make sure we're not duplicating an existing specification
         WsfStringId typeId = vehicleType;
         for (unsigned int i = 0; i < aNetwork.mVehicleInput.size(); ++i)
         {
            if (typeId == aNetwork.mVehicleInput[i].mTypeId)
            {
               throw UtInput::BadValue(aInput, "Duplicate vehicle type specified: " + vehicleType);
            }
         }
         aNetwork.mVehicleInput.push_back(XWsfRoadTraffic::VehicleInput(vehicleType));
         ProcessVehicleBlock(aInput, aNetwork.mVehicleInput.back());
      }
      else
      {
         myCommand = mData.ProcessCommand(aInput, aNetwork);
      }
   }

   // Ensure parameter consistency
   aNetwork.VerifyData(aInput);

   return myCommand;
}

// ================================================================================
// private
void RoadTrafficScenarioExtension::ProcessVehicleBlock(UtInput& aInput, XWsfRoadTraffic::VehicleInput& aVehicle)
{
   UtInputBlock inputBlock(aInput, "end_vehicle");
   std::string  command;
   while (inputBlock.ReadCommand())
   {
      command = inputBlock.GetCommand();
      if (command == "fraction")
      {
         double fraction;
         aInput.ReadValue(fraction);
         aInput.ValueGreater(fraction, 0.0);
         aInput.ValueLessOrEqual(fraction, 1.0);
         aVehicle.mFraction = fraction;
      }
      else if (command == "mean_speed")
      {
         double speed;
         aInput.ReadValueOfType(speed, UtInput::cSPEED);
         aInput.ValueGreaterOrEqual(speed, 0.0);
         aVehicle.mMeanSpeed = speed;
      }
      else if (command == "sigma_speed")
      {
         double speed;
         aInput.ReadValueOfType(speed, UtInput::cSPEED);
         aInput.ValueGreaterOrEqual(speed, 0.0);
         aVehicle.mSigmaSpeed = speed;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

// ================================================================================
// private
void RoadTrafficScenarioExtension::ProcessWeightedRegionBlock(UtInput&                              aInput,
                                                              XWsfRoadTraffic::WeightedRegionInput& aWeightedRegion)
{
   UtInputBlock inputBlock(aInput, "end_weighted_region");
   std::string  command;
   while (inputBlock.ReadCommand())
   {
      command = inputBlock.GetCommand();
      if (command == "latitude")
      {
         aInput.ReadValueOfType(aWeightedRegion.mLatitude, UtInput::cLATITUDE);
      }
      else if (command == "longitude")
      {
         aInput.ReadValueOfType(aWeightedRegion.mLongitude, UtInput::cLONGITUDE);
      }
      else if (command == "inner_radius")
      {
         aInput.ReadValueOfType(aWeightedRegion.mInnerRadius, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(aWeightedRegion.mInnerRadius, 0.0);
      }
      else if (command == "outer_radius")
      {
         aInput.ReadValueOfType(aWeightedRegion.mOuterRadius, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(aWeightedRegion.mOuterRadius, 0.0);
      }
      else if (command == "inner_weight")
      {
         aInput.ReadValue(aWeightedRegion.mInnerWeight);
         aInput.ValueGreaterOrEqual(aWeightedRegion.mInnerWeight, 1.0);
      }
      else if (command == "outer_weight")
      {
         aInput.ReadValue(aWeightedRegion.mOuterWeight);
         aInput.ValueGreaterOrEqual(aWeightedRegion.mOuterWeight, 1.0);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

// ================================================================================
// private
void RoadTrafficScenarioExtension::ProcessConvoyBlock(UtInput& aInput, XWsfRoadTraffic::ConvoyInput& aConvoyInput)
{
   UtInputBlock inputBlock(aInput, "end_convoy");
   std::string  command;
   while (inputBlock.ReadCommand())
   {
      command = inputBlock.GetCommand();
      if (command == "start_position")
      {
         aInput.ReadValueOfType(aConvoyInput.mStartLatitude, UtInput::cLATITUDE);
         aInput.ReadValueOfType(aConvoyInput.mStartLongitude, UtInput::cLONGITUDE);
      }
      else if (command == "end_position")
      {
         aInput.ReadValueOfType(aConvoyInput.mEndLatitude, UtInput::cLATITUDE);
         aInput.ReadValueOfType(aConvoyInput.mEndLongitude, UtInput::cLONGITUDE);
      }
      else if (command == "spacing")
      {
         aInput.ReadValueOfType(aConvoyInput.mSpacing, UtInput::cLENGTH);
         aInput.ValueGreater(aConvoyInput.mSpacing, 0.0);
      }
      else if (command == "speed")
      {
         aInput.ReadValueOfType(aConvoyInput.mSpeed, UtInput::cSPEED);
         aInput.ValueGreater(aConvoyInput.mSpeed, 0.0);
      }
      else if (command == "use_closest_waypoint")
      {
         aConvoyInput.mUseClosestWaypoint = true;
      }
      else if (command == "vehicle")
      {
         // Read the number of vehicles of this type.
         int count;
         aInput.ReadValue(count);
         aInput.ValueGreater(count, 0);

         // The vehicle type.
         std::string vehicleType;
         aInput.ReadValue(vehicleType);

         XWsfRoadTraffic::VehicleInput vehInput(vehicleType);
         vehInput.mCount = count;
         aConvoyInput.mVehicleInput.push_back(vehInput);
         ProcessVehicleBlock(aInput, aConvoyInput.mVehicleInput.back());
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   // Calculate the required pause time to achieve the desired spacing.
   if (aConvoyInput.mSpeed > 0.0)
   {
      aConvoyInput.mPauseTime = aConvoyInput.mSpacing / aConvoyInput.mSpeed;
   }
}

} // namespace

//! Update determines if vehicles need to be added, deleted or teleported.
//! NOTE: Actual vehicle motion occurs as part of the normal simulation process.
void XWsfRoadTraffic::AdvanceTime(double aSimTime)
{
   if (aSimTime >= mData.mNextUpdateTime)
   {
      mData.mNextUpdateTime = 1.0E+10;

      // update vehicle states and check for vehicles to delete
      int trafficSize = static_cast<int>(mData.mTraffic.size());
      for (int vehicleNumber = 0; vehicleNumber < trafficSize; ++vehicleNumber)
      {
         // If the platform no longer exists in the simulation remove it.
         if (!GetSimulation().PlatformExists(mData.mTraffic[vehicleNumber].mRoadPlatformIndex))
         {
            DeletePlatformFromSimulation(aSimTime, vehicleNumber);
            continue;
         }

         if (!mData.mTraffic[vehicleNumber].mConvoyVehicle && !mData.mTraffic[vehicleNumber].mStationary &&
             mData.mTraffic[vehicleNumber].mAlive)
         {
            // If the vehicle is moving along the road and hasn't gone offroad yet.
            if (aSimTime < mData.mTraffic[vehicleNumber].mOffRoadTime)
            {
               mData.mNextUpdateTime = std::min(mData.mTraffic[vehicleNumber].mOffRoadTime, mData.mNextUpdateTime);
            }
            // If the vehicle is moving offroad.
            else if ((aSimTime >= mData.mTraffic[vehicleNumber].mOffRoadTime) &&
                     (aSimTime < mData.mTraffic[vehicleNumber].mDeadTime))
            {
               if (mData.mEndOfPathOption == cRESPAWN)
               {
                  int netId = mData.mTraffic[vehicleNumber].mNetwork;
                  // We are about to go offroad so change the vehicles current waypoint route
                  // to reflect this.
                  if (!mData.mTraffic[vehicleNumber].mVehicleOffRoad)
                  {
                     GoOffRoad(static_cast<RoadTrafficNetworkInput&>(*mData.GetNetworks()[netId]), vehicleNumber);
                     mData.mTraffic[vehicleNumber].mVehicleOffRoad = true;
                  }
               }
               mData.mNextUpdateTime = std::min(mData.mTraffic[vehicleNumber].mDeadTime, mData.mNextUpdateTime);
            }
            // Otherwise, remove the vehicle and create a new one.
            else
            {
               if (mData.mEndOfPathOption == cREVERSE_DIRECTION)
               {
                  ReverseVehicleRoute(vehicleNumber, aSimTime);
               }
               else
               {
                  int netId = mData.mTraffic[vehicleNumber].mNetwork;

                  // Remove the vehicle.
                  DeletePlatformFromSimulation(aSimTime, vehicleNumber);

                  // Create and add a new vehicle.
                  int newVehicleNumber;
                  if (CreateVehicle(static_cast<RoadTrafficNetworkInput&>(*mData.GetNetworks()[netId]),
                                    aSimTime,
                                    newVehicleNumber))
                  {
                     // The vehicle we created might have a WsfRoadMover mover, which needs to have additional attributes set
                     WsfRoadMover* moverPtr =
                        dynamic_cast<WsfRoadMover*>(mData.mTraffic[newVehicleNumber].mRoadPlatform->GetMover());
                     if (moverPtr != nullptr)
                     {
                        moverPtr->SetRoadNetworkId(mData.GetNetworks()[netId]->mNetworkId);
                     }
                     AddPlatformToSimulation(aSimTime, newVehicleNumber);
                  }
                  else
                  {
                     ut::log::error() << "road_traffic: Could not create new vehicle.";
                  }
               }
               mData.mNextUpdateTime = std::min(mData.mTraffic[vehicleNumber].mDeadTime, mData.mNextUpdateTime);
            }
         }

         if (mData.mTraffic[vehicleNumber].mConvoyVehicle || mData.mTraffic[vehicleNumber].mStationary)
         {
            if (aSimTime < mData.mTraffic[vehicleNumber].mDeadTime)
            {
               mData.mNextUpdateTime = std::min(mData.mNextUpdateTime, mData.mTraffic[vehicleNumber].mDeadTime);
            }
            else
            {
               DeletePlatformFromSimulation(aSimTime, vehicleNumber);
            }
         }
      }
   }
}

//! Validate the list of vehicles types
// private
bool XWsfRoadTraffic::ValidateVehicleTypes(RoadTrafficNetworkInput& aNetwork)
{
   bool ok = true;
   for (unsigned int i = 0; i < aNetwork.mVehicleInput.size(); ++i)
   {
      // Make sure all the road vehicle types exist.
      std::string typeName(aNetwork.mVehicleInput[i].mTypeId);
      if (GetScenario().FindType("platform_type", typeName) == nullptr)
      {
         auto out = ut::log::error() << "road_traffic: Vehicle type does not exist.";
         out.AddNote() << "Network: " << aNetwork.mNetworkId;
         out.AddNote() << "Vehicle Type: " << typeName;
         ok = false;
      }
   }

   // Make sure all the convoy vehicle types exist.
   for (unsigned int convoyId = 0; convoyId < aNetwork.mConvoyInput.size(); ++convoyId)
   {
      int vehInputCount = static_cast<int>(aNetwork.mConvoyInput[convoyId].mVehicleInput.size());
      for (int vehInputId = 0; vehInputId < vehInputCount; ++vehInputId)
      {
         int vehCount = aNetwork.mConvoyInput[convoyId].mVehicleInput[vehInputId].mCount;
         for (int vehId = 0; vehId < vehCount; ++vehId)
         {
            std::string typeName(aNetwork.mConvoyInput[convoyId].mVehicleInput[vehInputId].mTypeId);
            if (GetScenario().FindType("platform_type", typeName) == nullptr)
            {
               auto out = ut::log::error() << "road_traffic: Convoy vehicle type does not exist.";
               out.AddNote() << "Network: " << aNetwork.mNetworkId;
               out.AddNote() << "Vehicle Type: " << typeName;
               ok = false;
            }
         }
      }
   }
   return ok;
}

WsfMover* XWsfRoadTraffic::GetMover(WsfPlatform* aPlatformPtr)
{
   WsfMover* roadMoverPtr = dynamic_cast<WsfRoadMover*>(aPlatformPtr->GetMover());
   if (roadMoverPtr == nullptr)
   {
      WsfGroundMover* groundMoverPtr = dynamic_cast<WsfGroundMover*>(aPlatformPtr->GetMover());
      if (groundMoverPtr != nullptr)
      {
         roadMoverPtr = new WsfRoadMover(*groundMoverPtr);
         aPlatformPtr->SetMover(roadMoverPtr);
      }
   }
   return roadMoverPtr;
}
