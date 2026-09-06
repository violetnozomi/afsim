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

#include "traffic/XWsfOSM_Traffic.hpp"

#include <algorithm>
#include <sstream>

#include "UtCast.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfOSM_TrafficInput.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfRoadMover.hpp"
#include "WsfRoute.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"
#include "WsfWaypointMover.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptObjectClass.hpp"
class WsfScriptOSM_TrafficClass;

namespace
{
class XWsfOSM_TrafficExtension : public WsfScenarioExtension, public WsfOSM_TrafficInput
{
public:
   XWsfOSM_TrafficExtension()           = default;
   ~XWsfOSM_TrafficExtension() override = default;

   void SimulationCreated(WsfSimulation& aSimulation) override
   {
      if (XWsfOSM_Traffic::Find(aSimulation) == nullptr)
      {
         aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<XWsfOSM_Traffic>(*this));
      }
   }
   bool ProcessInput(UtInput& aInput) override { return WsfOSM_TrafficInput::ProcessInput(aInput); }
};

class OSM_ApplicationExtension : public WsfApplicationExtension
{
public:
   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<XWsfOSM_TrafficExtension>());
   }
   void AddedToApplication(WsfApplication& aApplication) override
   {
      UtScriptTypes* typesPtr = aApplication.GetScriptTypes();
      typesPtr->Register(XWsfOSM_Traffic::CreateScriptClass("WsfOSM_Traffic", typesPtr));
   }
};

const std::string cLEFT_LANE_SUFFIX     = "__opposite";
const std::string cRIGHT_LANE_SUFFIX    = "__normal";
const std::string cBASE_ROUTE_AUX_NAME  = "baseRouteAuxName";
const std::string cBASE_ROUTE_AUX_INDEX = "baseRouteAuxIndex";
const std::string cOFFROAD_ID           = "OFFROAD_ID_FOR_ROUTE_TRACKER123";

bool CheckAttributeValidity(const WsfAttributeContainer& aContainer,
                            const std::string&           aVarName,
                            const std::string&           aType,
                            const std::string&           aErrorArg = "")
{
   if (aContainer.AttributeExists(aVarName))
   {
      auto&       attribute    = aContainer.GetAttribute(aVarName);
      std::string variableType = (aType == "unitary") ? "double" : aType;
      if (variableType.empty() || attribute.GetType() != variableType)
      {
         auto out = ut::log::error() << "osm_traffic: Unexpected variable type.";
         out.AddNote() << "Variable: " << aVarName;
         if (!aErrorArg.empty())
         {
            out.AddNote() << "On: " << aErrorArg;
         }
         out.AddNote() << "Actual Type: " << attribute.GetType();
         out.AddNote() << "Expected Type: " << aType;
         return false;
      }
   }
   // It is valid to have no aux_data field. There should only be an error when it is defined in a way that osm_traffic
   // won't understand.
   return true;
}

void GetMeanAndStdDev(double&                                     aMean,
                      double&                                     aStdDev,
                      const WsfRoute&                             aRoute,
                      const WsfOSM_TrafficInput::RouteAuxMapping& aRouteAuxMap,
                      double                                      aVehicleSpeed,
                      bool                                        aIgnoreSpeedLimit)
{
   double minSpeed = -1;
   double maxSpeed = -1;
   if (!aIgnoreSpeedLimit)
   {
      WsfUtil::GetAuxValue(aRoute, aRouteAuxMap.mMinSpeed, minSpeed);
      WsfUtil::GetAuxValue(aRoute, aRouteAuxMap.mMaxSpeed, maxSpeed);
   }
   if (maxSpeed > minSpeed && minSpeed > 0.0)
   {
      aMean   = (maxSpeed + minSpeed) / 2.0;
      aStdDev = (maxSpeed - minSpeed) / 4.0;
   }
   else if (maxSpeed > 0.0 && minSpeed < 0.0)
   {
      aMean   = 3.0 * maxSpeed / 4.0;
      aStdDev = maxSpeed / 8.0;
   }
   else if (minSpeed > 0.0 && maxSpeed < 0.0)
   {
      aMean   = minSpeed * 1.5;
      aStdDev = minSpeed / 4.0;
   }
   else if (aVehicleSpeed > 0.0)
   {
      aMean   = aVehicleSpeed;
      aStdDev = aVehicleSpeed / 8.0;
   }
   else
   {
      aMean   = 20.0;
      aStdDev = 3.0;
   }
}
} // namespace

void WSF_EXPORT Register_osm_traffic(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("osm_traffic"))
   {
      aApplication.RegisterExtension("osm_traffic", ut::make_unique<OSM_ApplicationExtension>());
   }
}

// =================================================================================================
XWsfOSM_Traffic::XWsfOSM_Traffic(WsfOSM_TrafficInput& aData)
   : WsfSimulationExtension()
   , WsfOSM_TrafficInput(aData)
{
}

// =================================================================================================
XWsfOSM_Traffic::~XWsfOSM_Traffic()
{
   if (mMaxVehiclesActive > 0)
   {
      if (mDebug)
      {
         auto out = ut::log::debug() << "osm_traffic: Debug dump:";
         out.AddNote() << "Maximum vehicle array size: " << mTraffic.size();
         out.AddNote() << "Maximum number of active vehicles: " << mMaxVehiclesActive;
         out.AddNote() << "Current number of active vehicles: " << mNumVehiclesActive;
      }
      ClearVehicleList();
   }
}

// =================================================================================================
void XWsfOSM_Traffic::AddedToSimulation()
{
   mCallbacks.Add(WsfObserver::AdvanceTime(&GetSimulation()).Connect(&XWsfOSM_Traffic::AdvanceTime, this));
   mCallbacks.Add(WsfObserver::PlatformDeleted(&GetSimulation()).Connect(&XWsfOSM_Traffic::PlatformDeletedHandler, this));
   mCallbacks.Add(WsfObserver::MoverPathEnded(&GetSimulation()).Connect(&XWsfOSM_Traffic::VehiclePathEndedHandler, this));
}

XWsfOSM_Traffic* XWsfOSM_Traffic::Find(const WsfSimulation& aSimulation)
{
   return static_cast<XWsfOSM_Traffic*>(aSimulation.FindExtension("osm_traffic"));
}

// =================================================================================================
// Initialize the road traffic manager.
// virtual
bool XWsfOSM_Traffic::Initialize()
{
   // If we aren't using the road traffic simulation, return true
   // so we don't cause an initialization error.
   if (!mProcessedInput)
   {
      return true;
   }

   if (!XWsfRouteNetwork::Initialize(GetSimulation().GetScenario()))
   {
      return false;
   }

   bool         ok = true;
   unsigned int netIndex;

   mNextVehicleIdentifier = 0;

   ClearVehicleList();
   if (mNetworks.empty())
   {
      // No roads...
      mNextUpdateTime = 1.0E+20;
      return false;
   }

   for (netIndex = 0; netIndex < mNetworks.size(); ++netIndex)
   {
      RoadTrafficNetworkInput& netInput    = static_cast<RoadTrafficNetworkInput&>(*mNetworks[netIndex]);
      std::string              networkName = mNetworks[netIndex]->mNetworkId;
      if (netInput.mRouteNetworkPtr != nullptr)
      {
         if (!ValidateVehicleTypes(netInput))
         {
            ok = false;
         }
      }
      else
      {
         auto out = ut::log::error() << "osm_traffic: Undefined network.";
         out.AddNote() << "Network: " << networkName;

         ok = false;
      }
      ok = ok && ValidateAuxData(netInput);
      if (ok)
      {
         BuildSplitRoads(netIndex, netInput);
         BuildWeightedGraph(netIndex, netInput);
         if (!CheckNetworkValidity(netIndex))
         {
            auto out = ut::log::error() << "osm_traffic: Network contains exits, but no acceptable entrances. Vehicles "
                                           "cannot be respawned after leaving the network.";
            out.AddNote() << "Network: " << networkName;
            ok = false;
         }
      }
   }

   if (ok)
   {
      // Create the vehicles.
      for (netIndex = 0; netIndex < mNetworks.size(); ++netIndex)
      {
         RoadTrafficNetworkInput& input = static_cast<RoadTrafficNetworkInput&>(*mNetworks[netIndex]);
         // Create Convoy Vehicles First
         CreateConvoyVehicles(input, 0.0);

         // create remaining background traffic
         int vehicleCount = mNetworks[netIndex]->mVehicleCount;
         for (int vehicleIndex = 0; vehicleIndex < vehicleCount; ++vehicleIndex)
         {
            int vehicleNumber;
            input.mNetIndex = netIndex;
            if (CreateVehicle(input, 0.0, vehicleNumber))
            {
               AddPlatformToSimulation(0.0, vehicleNumber);
            }
         }
      }
   }

   mNextUpdateTime = 0.0;

   if (!ok)
   {
      ut::log::error() << "osm_traffic: Unable to initialize.";
   }
   return ok;
}

// =================================================================================================
std::unique_ptr<UtScriptClass> XWsfOSM_Traffic::CreateScriptClass(const std::string& aClassName,
                                                                  UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptOSM_TrafficClass>(aClassName, aScriptTypesPtr);
}

// =================================================================================================
void XWsfOSM_Traffic::AddPlatformToSimulation(double aSimTime, int aVehicleNumber)
{
   // Add the platform to the simulation.
   WsfPlatform* platformPtr = mTraffic[aVehicleNumber].mRoadPlatform;
   assert(platformPtr != nullptr);
   if (mDebug)
   {
      auto out = ut::log::debug() << "osm_traffic: Adding vehicle to simulation.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Vehicle: " << platformPtr->GetName();
      out.AddNote() << "Vehicle Number: " << aVehicleNumber;
   }

   GetSimulation().AddPlatform(aSimTime, platformPtr);
   // mNextUpdateTime = std::min(mTraffic[aVehicleNumber].mOffRoadTime, mNextUpdateTime);
   mTraffic[aVehicleNumber].mRoadPlatformIndex = platformPtr->GetIndex();
   ++mNumVehiclesActive;
   mMaxVehiclesActive = std::max(mNumVehiclesActive, mMaxVehiclesActive);
}

int XWsfOSM_Traffic::SlotNumberHandler(WsfStringId aVehicleTypeId)
{
   const std::string& vehicleType   = aVehicleTypeId;
   int                vehicleNumber = -1;
   for (size_t i = 0; i < mTraffic.size(); ++i)
   {
      if (mTraffic[i].mRoadPlatform == nullptr)
      {
         vehicleNumber = static_cast<int>(i);
         break;
      }
   }

   // If there are no unused slots then allocate a new one...
   if (vehicleNumber < 0)
   {
      SGroundTraffic vehicle;
      mTraffic.push_back(vehicle);
      vehicleNumber = static_cast<int>(mTraffic.size() - 1);
      if (mDebug)
      {
         auto out = ut::log::debug() << "osm_traffic: Allocating new vehicle using new slot.";
         out.AddNote() << "Vehicle Number: " << vehicleNumber;
         out.AddNote() << "Type: " << vehicleType;
      }
   }
   else if (mDebug)
   {
      auto out = ut::log::debug() << "osm_traffic: Allocating new vehicle using old slot.";
      out.AddNote() << "Vehicle Number: " << vehicleNumber;
      out.AddNote() << "Type: " << vehicleType;
   }
   return vehicleNumber;
}

// =================================================================================================
//! Allocate a vehicle and return its index within the mTraffic array.
int XWsfOSM_Traffic::AllocateVehicle(WsfStringId aVehicleTypeId)
{
   const std::string& vehicleType = aVehicleTypeId;

   // Determine if there is an available slot...
   //
   // A slot is 'available' if it does not have a RoadPlatform assigned to it.
   // This routine does not create the RoadPlatform so technically the slot is
   // still available until the entry gets a RoadPlatform assigned to it.

   int vehicleNumber = SlotNumberHandler(aVehicleTypeId);

   // Initialize vehicle fields

   mTraffic[vehicleNumber].mIdentifier        = ++mNextVehicleIdentifier;
   mTraffic[vehicleNumber].mVehicleTypeId     = aVehicleTypeId;
   mTraffic[vehicleNumber].mAlive             = true;
   mTraffic[vehicleNumber].mNetwork           = -1;
   mTraffic[vehicleNumber].mSpeed             = 0.0;
   mTraffic[vehicleNumber].mRoadPlatformIndex = 0;
   mTraffic[vehicleNumber].mConvoyVehicle     = false;
   mTraffic[vehicleNumber].mCurrentRouteName  = "";
   mTraffic[vehicleNumber].mScripted          = false;

   // Instantiate a platform of the proper type
   mTraffic[vehicleNumber].mRoadPlatform = (WsfPlatform*)GetScenario().CloneType("platform_type", vehicleType);
   if (mTraffic[vehicleNumber].mRoadPlatform == nullptr)
   {
      { // RAII block
         auto out = ut::log::error() << "osm_traffic: Invalid platform type.";
         out.AddNote() << "Type: " << vehicleType;
      }
      assert(mTraffic[vehicleNumber].mRoadPlatform != nullptr);
      return vehicleNumber;
   }

   // Set the name of the platform
   std::ostringstream oss;
   oss << vehicleType << '_' << mTraffic[vehicleNumber].mIdentifier;
   mTraffic[vehicleNumber].mRoadPlatform->SetName(oss.str());

   return vehicleNumber;
}

// =================================================================================================
void XWsfOSM_Traffic::ClearVehicleList()
{
   mTraffic.clear();
   mNumVehiclesActive = 0;
   mMaxVehiclesActive = 0;
}

// =================================================================================================
void XWsfOSM_Traffic::DeletePlatformFromSimulation(double aSimTime, int aVehicleNumber)
{
   if (mDebug)
   {
      auto out = ut::log::debug() << "osm_traffic: Deleting vehicle.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Vehicle Number: " << aVehicleNumber;
   }

   // Delete the platform from the simulation (if it hasn't been broken)
   if (GetSimulation().PlatformExists(mTraffic[aVehicleNumber].mRoadPlatformIndex))
   {
      GetSimulation().DeletePlatform(aSimTime, mTraffic[aVehicleNumber].mRoadPlatform);
   }

   // Mark the vehicle slot as empty to it can be reused
   mTraffic[aVehicleNumber].mIdentifier        = 0;
   mTraffic[aVehicleNumber].mVehicleTypeId     = nullptr;
   mTraffic[aVehicleNumber].mAlive             = false;
   mTraffic[aVehicleNumber].mNetwork           = -1;
   mTraffic[aVehicleNumber].mSpeed             = 0.0;
   mTraffic[aVehicleNumber].mRoadPlatform      = nullptr;
   mTraffic[aVehicleNumber].mRoadPlatformIndex = 0;
   mTraffic[aVehicleNumber].mConvoyVehicle     = false;
   mTraffic[aVehicleNumber].mConvoyNumber      = -1;
   mTraffic[aVehicleNumber].mAtIntersection    = false;

   --mNumVehiclesActive;
   mNumVehiclesActive = std::max(mNumVehiclesActive, 0);
}

// =================================================================================================
//! Generate a vehicle that will travel on the specified road network.
//! Returns 'true' if successful or 'false' if not.
// private
bool XWsfOSM_Traffic::CreateVehicle(const RoadTrafficNetworkInput& aNetworkInput,
                                    double                         aCreateTime,
                                    int&                           aVehicleNumber,
                                    bool                           aAtSource)
{
   if (mDebug)
   {
      auto out = ut::log::debug() << "osm_traffic: Creating vehicle.";
      out.AddNote() << "T = " << aCreateTime;
   }

   // Determine which type of vehicle should be created on the road.
   // NOTE: If for some reason we don't explicitly select a vehicle,
   // the last one will automatically get selected.
   double r           = GetSimulation().GetRandom().Uniform<double>();
   double cumFraction = 0.0;
   assert(!aNetworkInput.mVehicleInput.empty());
   const VehicleInput* vehInputPtr = nullptr;
   for (const auto& vi : aNetworkInput.mVehicleInput)
   {
      vehInputPtr = &vi;
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
   for (unsigned int netId = 0; netId < mNetworks.size(); ++netId)
   {
      if (mNetworks[netId]->mNetworkId == aNetworkInput.mNetworkId)
      {
         mTraffic[nv].mNetwork = netId;
         break;
      }
   }

   // Check to see if this vehicle is stationary.
   if ((fabs(vehInputPtr->mMeanSpeed) < 1.0E-10) && (fabs(vehInputPtr->mSigmaSpeed) < 1.0E-10))
   {
      mTraffic[nv].mSpeed = 0.0;
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

      mTraffic[nv].mSpeed = speed;
      if (aNetworkInput.mMaxSpeed > 1.0E-10)
      {
         mTraffic[nv].mSpeed = std::min(speed, aNetworkInput.mMaxSpeed);
      }
   }

   if (aAtSource)
   {
      std::string sourceRoute = GetRandomSourceRoute(mTraffic[nv].mNetwork);
      if (!sourceRoute.empty())
      {
         CreateInitialPathForVehicle(aNetworkInput, nv, sourceRoute, 0.0);
      }
   }
   else
   {
      CreateInitialPathForVehicle(aNetworkInput, nv);
   }


   return true;
}

// =================================================================================================
bool XWsfOSM_Traffic::CreateConvoyVehicles(const RoadTrafficNetworkInput& aNetworkInput, double aCreateTime)
{
   TrafficInformation& networkTraffic = mRoadInfo[aNetworkInput.mNetIndex];
   int                 convoyNumber   = 1;
   auto&               convoyInput    = aNetworkInput.mConvoyInput;
   for (const auto& convoyIt : convoyInput)
   {
      std::string startingRoute = convoyIt.mStartingRoute;
      for (const auto& inputIt : convoyIt.mVehicleInput)
      {
         for (int i = 0; i < inputIt.mCount; ++i)
         {
            if (mDebug)
            {
               ut::log::debug() << "osm_traffic: Creating convoy vehicle.";
            }
            int vehicleNumber = AllocateVehicle(inputIt.mTypeId);

            // Get the network index that this vehicle is on.
            for (unsigned int netId = 0; netId < mNetworks.size(); ++netId)
            {
               if (mNetworks[netId]->mNetworkId == aNetworkInput.mNetworkId)
               {
                  mTraffic[vehicleNumber].mNetwork = netId;
                  break;
               }
            }
            AddPlatformToSimulation(0.0, vehicleNumber);

            // stack all convoy vehicles on top of each other, then start moving them based on the "time spacing" variable
            WsfRouteMover* rMover = dynamic_cast<WsfRouteMover*>(mTraffic[vehicleNumber].mRoadPlatform->GetMover());
            if (i == 0)
            {
               CreateInitialPathForVehicle(aNetworkInput, vehicleNumber, convoyIt.mStartingRoute, convoyIt.mDistanceAlongRoute);
            }
            else
            {
               WsfRoute routeCopy = *mTraffic[mConvoyIndices[convoyNumber][0]].mRoadPlatform->GetMover()->GetRoute();
               rMover->SetRoute(GetSimulation().GetSimTime(), routeCopy);
               networkTraffic.mRouteVehicleTracker[routeCopy.GetName()].emplace(vehicleNumber);
               mTraffic[vehicleNumber].mCurrentRouteName = routeCopy.GetName();

               rMover->Pause(aCreateTime, convoyIt.mSpacing * i);
            }
            mTraffic[vehicleNumber].mConvoyNumber = convoyNumber;
            mConvoyIndices[convoyNumber].emplace_back(vehicleNumber);
         }
      }
      ++convoyNumber;
   }
   return true;
}

// =================================================================================================
std::vector<WsfRoute*> XWsfOSM_Traffic::GetRoutesOnNetwork(const std::string& aNetworkName)
{
   auto                   networkIt = mNetworkToIndex.find(aNetworkName);
   std::vector<WsfRoute*> routeCopies;
   if (networkIt != mNetworkToIndex.end())
   {
      for (const auto& it : mRoadInfo[networkIt->second].mRoadRoutes)
      {
         routeCopies.emplace_back(it.get());
      }
   }
   return routeCopies;
}

// =================================================================================================
WsfRoute* XWsfOSM_Traffic::GetIndividualRoute(const std::string& aNetworkName,
                                              const std::string& aBaseName,
                                              int                aLaneNumber,
                                              bool               aDirection,
                                              int                aSegmentNumber)
{
   std::vector<WsfRoute*> networkRoutes = GetRoutesOnNetwork(aNetworkName);
   auto                   routeIt =
      std::find_if(networkRoutes.begin(),
                   networkRoutes.end(),
                   [aBaseName, aLaneNumber, aDirection, aSegmentNumber](const WsfRoute* aRoute)
                   {
                      std::string suffix = "__" + std::to_string(aSegmentNumber) +
                                           (aDirection ? cRIGHT_LANE_SUFFIX : cLEFT_LANE_SUFFIX) +
                                           std::to_string(aLaneNumber);
                      const std::string& fullName = aRoute->GetName();
                      return fullName.compare(0, aBaseName.length(), aBaseName) == 0 &&
                             fullName.compare(fullName.length() - suffix.length(), suffix.length(), suffix) == 0 &&
                             fullName.length() == aBaseName.length() + suffix.length();
                   });
   return (routeIt != networkRoutes.end()) ? (*routeIt)->Clone() : nullptr;
}

// =================================================================================================
std::vector<WsfPlatform*> XWsfOSM_Traffic::GetPlatformsAlongRoute(const std::string& aNetworkName,
                                                                  const std::string& aRouteName)
{
   std::vector<WsfPlatform*> moverList;
   auto                      networkIt = mNetworkToIndex.find(aNetworkName);
   if (networkIt != mNetworkToIndex.end())
   {
      unsigned int        networkId      = networkIt->second;
      TrafficInformation& networkTraffic = mRoadInfo[networkId];
      for (const auto& vehicleIt : networkTraffic.mRouteVehicleTracker[aRouteName])
      {
         moverList.emplace_back(mTraffic[vehicleIt].mRoadPlatform);
      }
   }
   return moverList;
}

// =================================================================================================
WsfWaypoint* XWsfOSM_Traffic::GetNextWaypoint(const std::string& aNetworkName, const std::string& aRouteName, double aDistance)
{
   auto routeVector = GetRoutesOnNetwork(aNetworkName);
   auto routeIt     = std::find_if(routeVector.begin(),
                               routeVector.end(),
                               [aRouteName](const WsfRoute* aRoute) { return aRoute->GetName() == aRouteName; });
   if (routeIt != routeVector.end())
   {
      auto& waypointList      = (*routeIt)->GetWaypoints();
      auto  subsequentPointIt = std::find_if(waypointList.begin(),
                                            waypointList.end(),
                                            [aDistance](const WsfWaypoint& aWaypoint)
                                            { return aWaypoint.GetDistanceAlongRoute() >= aDistance; });
      if (subsequentPointIt != waypointList.end())
      {
         return (*subsequentPointIt).Clone();
      }
      // return a default waypoint if a "next waypoint" isn't found
      return WsfWaypoint().Clone();
   }
   return WsfWaypoint().Clone();
}

// =================================================================================================
std::string XWsfOSM_Traffic::GetTrafficFlow(double aSimTime, const std::string& aNetworkName, const std::string& aNodeId)
{
   auto networkIt = mNetworkToIndex.find(aNetworkName);
   if (networkIt != mNetworkToIndex.end())
   {
      auto intersectionIt = std::find_if(mRoadInfo[networkIt->second].mRoadGraphs.begin(),
                                         mRoadInfo[networkIt->second].mRoadGraphs.end(),
                                         [aNodeId](const WeightedRouteNode& aNode) { return (aNodeId == aNode.mId); });
      if (intersectionIt != mRoadInfo[networkIt->second].mRoadGraphs.end())
      {
         double modTime = std::fmod(aSimTime, intersectionIt->mSourceRoutes.size() * intersectionIt->mWaitTime);
         for (unsigned int i = 0; i < intersectionIt->mSourceRoutes.size(); ++i)
         {
            if (modTime < (i + 1) * intersectionIt->mWaitTime)
            {
               return intersectionIt->mSourceRoutes[i];
            }
         }
      }
   }
   return std::string();
}

// =================================================================================================
std::string XWsfOSM_Traffic::GetNodeOnRoute(const std::string& aNetworkName, const std::string& aRouteName, bool aFirstOrLast)
{
   auto routeVector = GetRoutesOnNetwork(aNetworkName);
   auto routeIt     = std::find_if(routeVector.begin(),
                               routeVector.end(),
                               [aRouteName](const WsfRoute* aRoute) { return aRoute->GetName() == aRouteName; });
   if (routeIt != routeVector.end())
   {
      unsigned int index = (aFirstOrLast) ? 0 : (*routeIt)->GetSize() - 1;
      return (*routeIt)->GetWaypointAt(index).GetNodeId().GetString();
   }
   return std::string();
}

// =================================================================================================
std::vector<WsfRoute*> XWsfOSM_Traffic::GetConnectedRoutes(const std::string& aNetworkName, const std::string& aNodeId)
{
   std::vector<WsfRoute*> routeList;
   auto                   networkIt = mNetworkToIndex.find(aNetworkName);
   if (networkIt != mNetworkToIndex.end())
   {
      // Ignore routes connected to the edge of the route network
      if (aNodeId.find(mNetworks[networkIt->second]->mRouteNetworkPtr->cSTART_NODE_ID) == std::string::npos &&
          aNodeId.find(mNetworks[networkIt->second]->mRouteNetworkPtr->cSTOP_NODE_ID) == std::string::npos)
      {
         auto intersectionIt = std::find_if(mRoadInfo[networkIt->second].mRoadGraphs.begin(),
                                            mRoadInfo[networkIt->second].mRoadGraphs.end(),
                                            [aNodeId](const WeightedRouteNode& aNode) { return (aNodeId == aNode.mId); });
         if (intersectionIt != mRoadInfo[networkIt->second].mRoadGraphs.end())
         {
            for (const auto& edgeIt : intersectionIt->mEdges)
            {
               routeList.emplace_back(mRoadInfo[networkIt->second].mRoadRoutes[edgeIt.mRouteIndex].get());
            }
         }
      }
   }
   return routeList;
}

// =================================================================================================
// Get RouteName from the user for the tracker, because script method "FollowRoute" does not attach it to the mover immediately
bool XWsfOSM_Traffic::AddScriptedVehicle(WsfPlatform*       aPlatformPtr,
                                         const std::string& aNetworkName,
                                         WsfRoute*          aRoute,
                                         int&               aVehicleNumber)
{
   auto networkIt = mNetworkToIndex.find(aNetworkName);
   if (networkIt != mNetworkToIndex.end())
   {
      if (aPlatformPtr->GetMover() && !aRoute->GetName().empty())
      {
         auto const& routeIt =
            std::find_if(mRoadInfo[networkIt->second].mRoadRoutes.begin(),
                         mRoadInfo[networkIt->second].mRoadRoutes.end(),
                         [aRoute](std::unique_ptr<WsfRoute>& aRoad) { return aRoad->GetName() == aRoute->GetName(); });
         if (routeIt != mRoadInfo[networkIt->second].mRoadRoutes.end())
         {
            int                      vehicleNumber = SlotNumberHandler(aPlatformPtr->GetType());
            RoadTrafficNetworkInput* networkPtr =
               static_cast<RoadTrafficNetworkInput*>(mNetworks[networkIt->second].get());
            auto   typeIt       = std::find_if(networkPtr->mVehicleInput.begin(),
                                       networkPtr->mVehicleInput.end(),
                                       [aPlatformPtr](const VehicleInput& aInput)
                                       { return aInput.mTypeId == aPlatformPtr->GetTypeId(); });
            double vehicleSpeed = (typeIt != networkPtr->mVehicleInput.end()) ? typeIt->mMeanSpeed : 15.0;
            mTraffic[vehicleNumber].mRoadPlatform      = aPlatformPtr;
            mTraffic[vehicleNumber].mIdentifier        = ++mNextVehicleIdentifier;
            mTraffic[vehicleNumber].mVehicleTypeId     = aPlatformPtr->GetType();
            mTraffic[vehicleNumber].mAlive             = true;
            mTraffic[vehicleNumber].mNetwork           = networkIt->second;
            mTraffic[vehicleNumber].mSpeed             = vehicleSpeed;
            mTraffic[vehicleNumber].mRoadPlatformIndex = aPlatformPtr->GetIndex();
            mTraffic[vehicleNumber].mConvoyVehicle     = false;
            mTraffic[vehicleNumber].mCurrentRouteName  = "";
            mTraffic[vehicleNumber].mConvoyNumber      = -1;
            mTraffic[vehicleNumber].mCatchUp           = false;
            mTraffic[vehicleNumber].mAtIntersection    = false;
            mTraffic[vehicleNumber].mScripted          = true;

            bool ignoreSpeedLimits = (typeIt != networkPtr->mVehicleInput.end()) ? typeIt->mIgnoreSpeedLimits : false;
            AssignRandomSpeeds(*aRoute, networkPtr->mRouteAuxDataMap, vehicleSpeed, ignoreSpeedLimits);
            aPlatformPtr->GetMover()->SetRoute(GetSimulation().GetSimTime(), *aRoute->Clone());
            mRoadInfo[networkIt->second].mRouteVehicleTracker[aRoute->GetName()].insert(vehicleNumber);
            aVehicleNumber = vehicleNumber;
            return true;
         }
      }
   }
   return false;
}

// =================================================================================================
bool XWsfOSM_Traffic::AddScriptedConvoy(const std::vector<WsfPlatform*> aPlatformList,
                                        const std::string&              aNetworkName,
                                        WsfRoute*                       aRoute,
                                        double                          aInitialSpacing)
{
   auto networkIt = mNetworkToIndex.find(aNetworkName);
   if (networkIt != mNetworkToIndex.end())
   {
      RoadTrafficNetworkInput& netInput       = static_cast<RoadTrafficNetworkInput&>(*mNetworks[networkIt->second]);
      TrafficInformation&      networkTraffic = mRoadInfo[networkIt->second];
      auto                     routeIt =
         std::find_if(networkTraffic.mRoadRoutes.begin(),
                      networkTraffic.mRoadRoutes.end(),
                      [aRoute](std::unique_ptr<WsfRoute>& aRoad) { return aRoute->GetName() == aRoad->GetName(); });
      // linux complains when using generic std::max/min and for mRandom.uniform functions, so just manually cast
      // everything to the same type
      int newConvoyNumber = std::max(static_cast<unsigned int>(netInput.mConvoyInput.size()), 1u);
      while (mConvoyIndices.find(newConvoyNumber) != mConvoyIndices.end())
      {
         newConvoyNumber++;
      }
      if (routeIt != networkTraffic.mRoadRoutes.end())
      {
         for (size_t i = 0; i < aPlatformList.size(); ++i)
         {
            int vehicleNumber;
            if (AddScriptedVehicle(aPlatformList[i], aNetworkName, aRoute, vehicleNumber))
            {
               WsfRouteMover* rMover = dynamic_cast<WsfRouteMover*>(mTraffic[vehicleNumber].mRoadPlatform->GetMover());
               if (rMover)
               {
                  mTraffic[vehicleNumber].mConvoyVehicle = true;
                  mTraffic[vehicleNumber].mConvoyNumber  = newConvoyNumber;
                  mConvoyIndices[newConvoyNumber].emplace_back(vehicleNumber);
                  if (i > 0)
                  {
                     rMover->Pause(GetSimulation().GetSimTime(), GetSimulation().GetSimTime() + (aInitialSpacing * i));
                  }
               }
            }
            else
            {
               return false;
            }
         }
         // If creating on the edge of the map, lock the specific road. Otherwise, lock the associated intersection. For
         // now, do not allow convoys to be created at intersections
         // TODO: If there is only one cSTART_NODE or cEND_NODE and a convoy is generated at one of these nodes, it will
         // reserve the road. This normally means that no other vehicles can enter the road,
         //       but because there is only one start/end node, the road becomes the only option so vehicles will end up
         //       respawning on the road anyway. This shouldn't be a problem as long as there are more than two total
         //       routes without nodes at their start/endpoints.
         std::string nodeId = routeIt->get()->GetWaypointAt(0).GetNodeId();
         if (nodeId.find(mNetworks[networkIt->second]->mRouteNetworkPtr->cSTART_NODE_ID) != std::string::npos ||
             nodeId.find(mNetworks[networkIt->second]->mRouteNetworkPtr->cSTOP_NODE_ID) != std::string::npos)
         {
            networkTraffic.mReservedRoads.emplace(aRoute->GetName());
            networkTraffic.mQueuedRoadRelease.emplace_back(aRoute->GetName(),
                                                           GetSimulation().GetSimTime() +
                                                              (aInitialSpacing * aPlatformList.size()),
                                                           true);
            std::sort(networkTraffic.mQueuedRoadRelease.begin(), networkTraffic.mQueuedRoadRelease.end());
            mNextRoadReleaseTime = std::min(mNextRoadReleaseTime, networkTraffic.mQueuedRoadRelease.front().mTime);
            return true;
         }
         else if (!nodeId.empty())
         {
            // networkTraffic.mIntersectionsInUse.emplace(nodeId, newConvoyNumber);
            // networkTraffic.mQueuedRoadRelease.push_back(QueuedRoadRelease(aRoute->GetWaypointAt(0).GetNodeId(),
            // GetSimulation().GetSimTime() + (aInitialSpacing * aPlatformList.size()), false));
         }
         // std::sort(networkTraffic.mQueuedRoadRelease.begin(), networkTraffic.mQueuedRoadRelease.end(),
         // QueuedRoadReleaseCompare()); mNextRoadReleaseTime = std::min(mNextRoadReleaseTime,
         // networkTraffic.mQueuedRoadRelease.front().mTime);
      }
   }
   return false;
}

// =================================================================================================
void XWsfOSM_Traffic::UpdateScriptedVehicleRoute(WsfPlatform* aPlatformPtr, WsfRoute* aRoute, int aWaypointIndex)
{
   auto vehicleIt =
      std::find_if(mTraffic.begin(),
                   mTraffic.end(),
                   [aPlatformPtr](const SGroundTraffic& aInfo) { return aPlatformPtr == aInfo.mRoadPlatform; });
   if (vehicleIt != mTraffic.end())
   {
      auto        mover = aPlatformPtr->GetMover();
      std::string oldRouteName;
      if (mover && mover->GetRoute())
      {
         oldRouteName = mover->GetRoute()->GetName();
      }
      if (oldRouteName.empty())
      {
         oldRouteName = cOFFROAD_ID;
      }
      vehicleIt->mCurrentRouteName = aRoute->GetName();
      if (mover)
      {
         mover->UpdateRoute(GetSimulation().GetSimTime(), *aRoute);
         mover->GoToWaypoint(GetSimulation().GetSimTime(), aWaypointIndex);
      }

      unsigned int vehicleNumber = static_cast<unsigned int>(vehicleIt - mTraffic.begin());
      mRoadInfo[vehicleIt->mNetwork].mRouteVehicleTracker[oldRouteName].erase(vehicleNumber);
      mRoadInfo[vehicleIt->mNetwork].mRouteVehicleTracker[aRoute->GetName()].emplace(vehicleNumber);
   }
}

// =================================================================================================
bool XWsfOSM_Traffic::ChangeScriptedLane(WsfPlatform* aPlatformPtr, bool aDirection)
{
   auto vehicleIt =
      std::find_if(mTraffic.begin(),
                   mTraffic.end(),
                   [aPlatformPtr](const SGroundTraffic& aInfo) { return aPlatformPtr == aInfo.mRoadPlatform; });
   if (vehicleIt != mTraffic.end())
   {
      TrafficInformation& networkTraffic = mRoadInfo[vehicleIt->mNetwork];
      if (!aPlatformPtr->GetMover() || !aPlatformPtr->GetMover()->GetRoute())
      {
         return false;
      }
      size_t      routeId          = aPlatformPtr->GetMover()->GetRoute()->GetNetworkRouteId();
      WsfRoute*   currentRoute     = networkTraffic.mRoadRoutes[routeId].get();
      std::string currentRouteName = aPlatformPtr->GetMover()->GetRoute()->GetName();

      double firstLL[2];
      double currentLL[2];
      double garbage;
      currentRoute->GetWaypointAt(0).GetLocationLLA(firstLL[0], firstLL[1], garbage);
      aPlatformPtr->GetLocationLLA(currentLL[0], currentLL[1], garbage);
      double currentDistance = currentRoute->GetDistanceAlongRoute(firstLL, currentLL);
      if (currentDistance < 0 || currentDistance > currentRoute->GetDistance(0, currentRoute->GetSize() - 1))
      {
         return false;
      }

      auto laneListIt       = networkTraffic.mAdjacentLanes.find(currentRouteName);
      auto ChangeLaneHelper = [&](int aIndex, const std::string& aLaneName)
      {
         double                      newRouteOffset      = networkTraffic.mRoadRoutes[aIndex]->GetDistance(0, 1);
         double                      originalRouteOffset = currentRoute->GetDistance(0, 1);
         double                      nextPointDistance = currentDistance - originalRouteOffset + newRouteOffset + 10.0;
         std::vector<TrackerDetails> vehiclesOnRoute;
         auto                        listIt = networkTraffic.mRouteVehicleTracker.find(aLaneName);
         if (listIt != networkTraffic.mRouteVehicleTracker.end())
         {
            for (const auto& vehicleIt : listIt->second)
            {
               TrackerCreationHelper(vehiclesOnRoute, vehicleIt);
            }
            std::sort(vehiclesOnRoute.begin(), vehiclesOnRoute.end());
         }

         double nextPoint[2];
         if (CheckLaneAvailability(static_cast<int>(vehicleIt - mTraffic.begin()),
                                   aIndex,
                                   vehiclesOnRoute,
                                   nextPointDistance,
                                   nextPoint))
         {
            ChangeLane(static_cast<int>(vehicleIt - mTraffic.begin()), aIndex, nextPointDistance, nextPoint);
            return true;
         }
         return false;
      };

      if (laneListIt != networkTraffic.mAdjacentLanes.end())
      {
         for (const auto& routeIndexIt : laneListIt->second)
         {
            std::string laneName = networkTraffic.mRoadRoutes[routeIndexIt]->GetName();
            // switch to lane on the left/right of the current route
            if ((aDirection && laneName[laneName.size() - 1] == currentRouteName[currentRouteName.size() - 1] + 1) ||
                (!aDirection && laneName[laneName.size() - 1] == currentRouteName[currentRouteName.size() - 1] - 1))
            {
               return ChangeLaneHelper(routeIndexIt, laneName);
            }
         }
      }
   }
   return false;
}


// =================================================================================================
bool XWsfOSM_Traffic::ScriptLockIntersection(const std::string& aNetworkName, const std::string& aNodeId)
{
   auto networkIt = mNetworkToIndex.find(aNetworkName);
   if (networkIt != mNetworkToIndex.end())
   {
      unsigned int        networkId      = networkIt->second;
      TrafficInformation& networkTraffic = mRoadInfo[networkId];
      // Ignore routes connected to the edge of the route network
      // Additionally only convoys should ever need to lock an intersection, so omit non-convoys
      if (aNodeId.find(mNetworks[networkIt->second]->mRouteNetworkPtr->cSTART_NODE_ID) == std::string::npos &&
          aNodeId.find(mNetworks[networkIt->second]->mRouteNetworkPtr->cSTOP_NODE_ID) == std::string::npos)
      {
         auto intersectionIt = std::find_if(mRoadInfo[networkIt->second].mRoadGraphs.begin(),
                                            mRoadInfo[networkIt->second].mRoadGraphs.end(),
                                            [aNodeId](const WeightedRouteNode& aNode) { return (aNodeId == aNode.mId); });
         if (intersectionIt != mRoadInfo[networkIt->second].mRoadGraphs.end())
         {
            networkTraffic.mIntersectionsInUse.emplace(aNodeId, 0);
            return true;
         }
      }
   }
   return false;
}

// =================================================================================================
bool XWsfOSM_Traffic::ScriptUnlockIntersection(const std::string& aNetworkName, const std::string& aNodeId)
{
   auto networkIt = mNetworkToIndex.find(aNetworkName);
   if (networkIt != mNetworkToIndex.end())
   {
      unsigned int        networkId      = networkIt->second;
      TrafficInformation& networkTraffic = mRoadInfo[networkId];
      // Ignore routes connected to the edge of the route network
      if (aNodeId.find(mNetworks[networkIt->second]->mRouteNetworkPtr->cSTART_NODE_ID) == std::string::npos &&
          aNodeId.find(mNetworks[networkIt->second]->mRouteNetworkPtr->cSTOP_NODE_ID) == std::string::npos)
      {
         auto intersectionIt = std::find_if(mRoadInfo[networkIt->second].mRoadGraphs.begin(),
                                            mRoadInfo[networkIt->second].mRoadGraphs.end(),
                                            [aNodeId](const WeightedRouteNode& aNode) { return (aNodeId == aNode.mId); });
         if (intersectionIt != mRoadInfo[networkIt->second].mRoadGraphs.end())
         {
            networkTraffic.mIntersectionsInUse.erase(aNodeId);
            return true;
         }
      }
   }
   return false;
}

void XWsfOSM_Traffic::ScriptGoOffroad(WsfPlatform* aVehicle, const WsfGeoPoint& aGeoPoint)
{
   auto vehicleIt = std::find_if(mTraffic.begin(),
                                 mTraffic.end(),
                                 [aVehicle](const SGroundTraffic& aInfo) { return aVehicle == aInfo.mRoadPlatform; });
   if (vehicleIt != mTraffic.end())
   {
      TrafficInformation& networkTraffic = mRoadInfo[vehicleIt->mNetwork];
      unsigned int        vehicleNumber  = static_cast<unsigned int>(vehicleIt - mTraffic.begin());
      if (aVehicle->GetMover() && aVehicle->GetMover()->GetRoute()) // has mover and route
      {
         networkTraffic.mRouteVehicleTracker[aVehicle->GetMover()->GetRoute()->GetName()].erase(vehicleNumber);
      }
      networkTraffic.mRouteVehicleTracker[cOFFROAD_ID].emplace(vehicleNumber);
      vehicleIt->mCurrentRouteName = cOFFROAD_ID;
      if (aVehicle->GetMover())
      {
         aVehicle->GetMover()->GoToLocation(GetSimulation().GetSimTime(),
                                            aGeoPoint.GetLat(),
                                            aGeoPoint.GetLon(),
                                            aGeoPoint.GetAlt());
      }
   }
}

// =================================================================================================
// For collision detection, use route->GetDistanceAlongRoute (use routeId to get the first waypoint of the route, even
// if that isn't where we started so there is a constant point of comparison between two movers going along the same
// route (get route from mRoadRoutes))
void XWsfOSM_Traffic::AdvanceTime(double aSimTime)
{
   HandleRegularOperations(aSimTime);
   HandleConvoySpeedChange(aSimTime);
   HandleQueuedConvoy(aSimTime);
   HandleRoadRelease(aSimTime);
}

// =================================================================================================
bool XWsfOSM_Traffic::ValidateVehicleTypes(RoadTrafficNetworkInput& aNetwork)
{
   bool ok = true;
   // UtInput will already handle the vehicle_count, so no need to validate
   double totalFraction = 0.0;
   for (unsigned int i = 0; i < aNetwork.mVehicleInput.size(); ++i)
   {
      // Make sure all the road vehicle types exist.
      std::string typeName(aNetwork.mVehicleInput[i].mTypeId);
      if (GetScenario().FindType("platform_type", typeName) == nullptr)
      {
         auto out = ut::log::error() << "osm_traffic: Vehicle type does not exist.";
         out.AddNote() << "Network: " << aNetwork.mNetworkId;
         out.AddNote() << "Vehicle Type: " << typeName;
         ok = false;
      }
      totalFraction += aNetwork.mVehicleInput[i].mFraction;
   }
   if (totalFraction != 1.0)
   {
      auto out = ut::log::error() << "osm_traffic: The sum of all the vehicle 'fraction' values must be 1.0.";
      out.AddNote() << "Current Sum: " << totalFraction;
      ok = false;
   }
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
               auto out = ut::log::error() << "osm_traffic: Convoy vehicle type does not exist.";
               out.AddNote() << "Network: " << aNetwork.mNetworkId;
               out.AddNote() << "Vehicle Type: " << typeName;
               ok = false;
            }
         }
      }
   }
   return ok;
}

// =================================================================================================
void XWsfOSM_Traffic::BuildSplitRoads(unsigned int aNetIndex, RoadTrafficNetworkInput& aNetwork)
{
   // relevant info to be swapped: lat, lon, alt, aux_data, nodeID
   // relevant info to stay in original index: positionInRoute, DistanceAlongRoute
   // everything else doesn't matter for traffic purposes

   auto ReversePoints = [](WsfRoute* aRoute)
   {
      std::vector<WsfWaypoint> updatedPoints;
      std::vector<WsfWaypoint> waypointList = aRoute->GetWaypoints();
      for (int i = static_cast<int>(waypointList.size()) - 1; i >= 0; --i)
      {
         waypointList[i].SetPositionInRoute(waypointList[waypointList.size() - 1 - i].GetPositionInRoute());
         waypointList[i].SetDistanceAlongRoute(waypointList[waypointList.size() - 1 - i].GetDistanceAlongRoute());
         updatedPoints.emplace_back(waypointList[i]);
      }
      aRoute->Clear();
      for (const auto& it : updatedPoints)
      {
         aRoute->Append(it);
      }
   };

   auto ChangeAllWaypointRouteIds = [](unsigned int aId, WsfRoute* aRoute)
   {
      unsigned int size = aRoute->GetSize();
      for (unsigned int i = 0; i < size; ++i)
      {
         aRoute->GetWaypointAt(i).SetRouteId(aId);
      }
   };

   auto GetAngleBetweenPoints = [](const WsfWaypoint& aPrevPoint, const WsfWaypoint& aCurrentPoint)
   {
      double currentLLA[3];
      double prevLLA[3];
      aCurrentPoint.GetLocationLLA(currentLLA[0], currentLLA[1], currentLLA[2]);
      aPrevPoint.GetLocationLLA(prevLLA[0], prevLLA[1], prevLLA[2]);
      WsfGeoPoint currentGeoPoint(currentLLA[0], currentLLA[1], currentLLA[2]);
      WsfGeoPoint prevGeoPoint(prevLLA[0], prevLLA[1], prevLLA[2]);
      return prevGeoPoint.TrueBearingTo(currentGeoPoint);
   };

   // Abrupt close-to-90 degree turns on an individual route may look odd when split apart (because the new routes do
   // not have inherently longer distances). This is user responsibility
   auto OffsetRoute = [&](WsfRoute* aRoute, unsigned int aMultiplier)
   {
      // aDirection = true when lane is to the right of the original, false when it is on the left (frame of reference
      // as someone going along the route)
      unsigned int size     = aRoute->GetSize() - 1;
      double       angleRad = 0;
      for (unsigned int i = 0; i < size; ++i)
      {
         WsfWaypoint& currentPoint = aRoute->GetWaypointAt(i);
         angleRad = (0.5 * UtMath::cPI) + GetAngleBetweenPoints(currentPoint, aRoute->GetWaypointAt(i + 1));
         double currentLLA[3];
         currentPoint.GetLocationLLA(currentLLA[0], currentLLA[1], currentLLA[2]);
         WsfGeoPoint offset(currentLLA[0], currentLLA[1], currentLLA[2]);
         offset.Extrapolate(angleRad, 3.0 * static_cast<double>(aMultiplier));
         double newLat = offset.GetLat();
         double newLon = offset.GetLon();
         currentPoint.SetLat(newLat);
         currentPoint.SetLon(newLon);
      }
      // for the last point, just the same angle offset as the previous waypoint
      WsfWaypoint& lastPoint = aRoute->GetWaypointAt(size);
      WsfGeoPoint  offset(lastPoint.GetLat(), lastPoint.GetLon(), lastPoint.GetAlt());
      offset.Extrapolate(angleRad, 3.0 * static_cast<double>(aMultiplier));
      double newLat = offset.GetLat();
      double newLon = offset.GetLon();
      lastPoint.SetLat(newLat);
      lastPoint.SetLon(newLon);
   };

   TrafficInformation& networkTraffic   = mRoadInfo[aNetIndex];
   mNetworkToIndex[aNetwork.mNetworkId] = aNetIndex;

   networkTraffic.mRoadRoutes.clear();
   std::unordered_map<std::string, unsigned int> routeCountMap;
   WsfRouteNetwork*                              routeNetwork = aNetwork.mRouteNetworkPtr;
   auto                                          routeList    = routeNetwork->GetRoutes();
   for (auto& routeIndex : routeList)
   {
      auto uniqueRoute = ut::clone(routeIndex);
      // OSM can have duplicate route names for different segments of a road, particularly if the lane count changes, so
      // for now add a suffix to handle this This shouldn't be necessary in the future, since we will likely force the
      // user to have unique route names
      auto countIt = routeCountMap.find(uniqueRoute->GetName());
      if (countIt == routeCountMap.end())
      {
         routeCountMap.emplace(uniqueRoute->GetName(), 1);
      }
      else
      {
         routeCountMap[uniqueRoute->GetName()]++;
         uniqueRoute->SetName(uniqueRoute->GetName() + std::to_string('a' + routeCountMap[uniqueRoute->GetName()]));
      }

      // Split routes with multiple intersections into separate routes. Additionally, cut off the ends and beginnings of
      // routes so intersections can be modeled as an "empty space" (instead of having every lane of every road converge
      // onto one single point)
      const double cCUSHION = 5.0;
      double       beginningLL[2];
      int          temp;
      // Note: The first point of a route will always be a node (either a route network ending or an intersection),
      //       but instead of being the node itself, the first point will be some distance ahead to preserve spacing.
      uniqueRoute->PointAlongRoute(cCUSHION, beginningLL, temp);
      auto insertRoute = ut::clone(routeIndex);
      insertRoute->Clear();
      WsfWaypoint firstPoint = uniqueRoute->GetWaypointAt(0);
      firstPoint.SetLocationLLA(beginningLL[0], beginningLL[1], firstPoint.GetAlt());
      insertRoute->Append(firstPoint);
      // If the route has multiple nodes, it needs to be split into multiple routes. Add a numeric suffix to the route
      // name so the user can distinguish them The numbers will be assigned in an order based on how the route is
      // constructed (order of the waypoints in the scenario file)
      unsigned int sectionNumber = 1;
      insertRoute->SetName(insertRoute->GetName() + "__" + std::to_string(sectionNumber++));
      for (size_t i = 1; i < uniqueRoute->GetSize();
           ++i) // TODO-AWK performance improvement if you aren't changing the size here...
      {
         WsfWaypoint current = uniqueRoute->GetWaypointAt(i);
         if (!current.GetNodeId().Empty())
         {
            double nodeLL[2]     = {current.GetLat(), current.GetLon()};
            double distanceAlong = uniqueRoute->GetDistanceAlongRoute(beginningLL, nodeLL);

            // end the route some distance prior to the node: routes will always end with a node (either intersection or
            // end of network)
            WsfWaypoint previousPoint         = uniqueRoute->GetWaypointAt(i - 1);
            double      previousLL[2]         = {previousPoint.GetLat(), previousPoint.GetLon()};
            double      previousDistanceAlong = uniqueRoute->GetDistanceAlongRoute(beginningLL, previousLL);
            double      newDistanceAlong      = distanceAlong - cCUSHION;
            // clip off points between the cushion value and the original node
            // even if the resulting route looks odd, make sure there are at least 2 points in the route, regardless of position
            while (previousDistanceAlong >= newDistanceAlong && insertRoute->GetSize() > 2)
            {
               insertRoute->Remove(insertRoute->GetSize() - 1);
               previousPoint         = uniqueRoute->GetWaypointAt(insertRoute->GetSize() - 1);
               previousLL[0]         = previousPoint.GetLat();
               previousLL[1]         = previousPoint.GetLon();
               previousDistanceAlong = uniqueRoute->GetDistanceAlongRoute(beginningLL, previousLL);
            }
            // The PointAlongRoute function is not precise enough over such a small distance (if distanceAlong or
            // previousDistanceAlong is used as an input, the returned point is not the same as the original) Therefore,
            // it is necessary to manually find a lat/lon point that is the specified distance away from the node
            double currentLLA[3];
            current.GetLocationLLA(currentLLA[0], currentLLA[1], currentLLA[2]);
            double      angleRad = GetAngleBetweenPoints(previousPoint, current);
            WsfGeoPoint offsetPoint(currentLLA[0], currentLLA[1], currentLLA[2]);
            offsetPoint.Extrapolate(angleRad + UtMath::cPI, cCUSHION);
            double selectedPoint[2] = {offsetPoint.GetLat(), offsetPoint.GetLon()};

            current.SetLocationLLA(selectedPoint[0], selectedPoint[1], current.GetAlt());
            insertRoute->Append(current);

            // add identifying aux_data to easily access the original route
            insertRoute->GetAuxData().AddString(cBASE_ROUTE_AUX_NAME, insertRoute->GetName());
            insertRoute->GetAuxData().AddInt(cBASE_ROUTE_AUX_INDEX, static_cast<int>(networkTraffic.mRoadRoutes.size()));
            insertRoute->SetName(insertRoute->GetName() + cRIGHT_LANE_SUFFIX + std::to_string(1));
            insertRoute->SetNetworkRouteId(static_cast<unsigned int>(networkTraffic.mRoadRoutes.size()));
            networkTraffic.mRoadRoutes.emplace_back(std::move(insertRoute));

            // if the original route hasn't ended, generate a new route starting some distance away from the node
            if (i != static_cast<size_t>(uniqueRoute->GetSize() - 1))
            {
               insertRoute.reset(routeIndex->Clone());
               insertRoute->Clear();
               insertRoute->SetName(insertRoute->GetName() + "__" + std::to_string(sectionNumber++));
               WsfWaypoint newStart = uniqueRoute->GetWaypointAt(i);

               // Skip points that are less than cCUSHION distance away from the node (to avoid diagonal crossing at end of route)
               previousLL[0]         = newStart.GetLat();
               previousLL[1]         = newStart.GetLon();
               newDistanceAlong      = uniqueRoute->GetDistanceAlongRoute(beginningLL, previousLL) + cCUSHION;
               size_t offset         = 1;
               previousLL[0]         = uniqueRoute->GetWaypointAt(i + offset).GetLat();
               previousLL[1]         = uniqueRoute->GetWaypointAt(i + offset).GetLon();
               previousDistanceAlong = uniqueRoute->GetDistanceAlongRoute(beginningLL, previousLL);
               if (uniqueRoute->GetWaypointAt(i + offset).GetNodeId().Empty())
               {
                  while (previousDistanceAlong <= newDistanceAlong)
                  {
                     WsfWaypoint nextPoint = uniqueRoute->GetWaypointAt(i + (++offset));
                     // if the entire route is under cCUSHION distance (5m), just do nothing.
                     if (!nextPoint.GetNodeId().Empty())
                     {
                        offset = 1;
                        break;
                     }
                     previousLL[0]         = nextPoint.GetLat();
                     previousLL[1]         = nextPoint.GetLon();
                     previousDistanceAlong = uniqueRoute->GetDistanceAlongRoute(beginningLL, previousLL);
                  }
               }
               i                      = i + offset - 1;
               double      startAngle = GetAngleBetweenPoints(newStart, uniqueRoute->GetWaypointAt(i + 1));
               WsfGeoPoint offsetStart(newStart.GetLat(), newStart.GetLon(), newStart.GetAlt());
               offsetStart.Extrapolate(startAngle, cCUSHION);
               selectedPoint[0] = offsetStart.GetLat();
               selectedPoint[1] = offsetStart.GetLon();
               current.SetLocationLLA(selectedPoint[0], selectedPoint[1], newStart.GetAlt());
               insertRoute->Append(current);
            }
         }
         else
         {
            insertRoute->Append(current);
         }
      }
   }

   std::vector<WsfRoute*> modifiedRouteList = GetRoutesOnNetwork(routeNetwork->GetName());
   for (const auto& routeIt : modifiedRouteList)
   {
      WsfRoute singleRoute = *routeIt;
      int      lanes       = 1;
      bool     oneWay      = false;
      // extract relevant aux_data from the route
      WsfUtil::GetAuxValue(singleRoute, aNetwork.mRouteAuxDataMap.mOneWay, oneWay); // defaults to false if not defined
      WsfUtil::GetAuxValue(singleRoute, aNetwork.mRouteAuxDataMap.mLanes, lanes);
      lanes = std::max(lanes, (oneWay ? 1 : 2)); // minimum of 1 lane if 1-way, 2-lanes if not

      // split the route into different lanes
      std::vector<WsfRoute> routeLanes;
      int                   rightLaneCount = (lanes - 1) / 2;
      int                   leftLaneCount  = lanes / 2;

      // if lanes == 1, do nothing special
      // if oneWay, create routes to the left/right as needed
      // if lanes == 2 && !oneWay, create a route to the left going in the opposite direction with some offset (
      // if lanes == an odd number, start by making a route in the opposite direction to the left, and same direction to
      // the right (will have unequal numbers) if lanes == an even number, start by making a route in the opposite
      // direction to the left, and same direction to the right The aux data doesn't require a safety check because
      // these involve data that is programmatically added to the container
      for (int i = 0; i < rightLaneCount; ++i)
      {
         WsfRoute* addedLane = singleRoute.Clone();
         addedLane->SetName(addedLane->GetAuxDataConst().GetString(cBASE_ROUTE_AUX_NAME) + cRIGHT_LANE_SUFFIX +
                            std::to_string(i + 2));
         OffsetRoute(addedLane, i + 1);
         ChangeAllWaypointRouteIds(static_cast<unsigned int>(networkTraffic.mRoadRoutes.size()), addedLane);
         addedLane->SetNetworkRouteId(static_cast<unsigned int>(networkTraffic.mRoadRoutes.size()));
         std::unique_ptr<WsfRoute> rightLaneRoute(addedLane);
         networkTraffic.mRoadRoutes.emplace_back(std::move(rightLaneRoute));
      }

      for (int i = 0; i < leftLaneCount; ++i)
      {
         WsfRoute* addedLane = singleRoute.Clone();
         addedLane->SetName(addedLane->GetAuxDataConst().GetString(cBASE_ROUTE_AUX_NAME) + cLEFT_LANE_SUFFIX +
                            std::to_string(i + 1));
         if (!oneWay)
         {
            ReversePoints(addedLane);
         }
         OffsetRoute(addedLane, i + 1);
         ChangeAllWaypointRouteIds(static_cast<unsigned int>(networkTraffic.mRoadRoutes.size()), addedLane);
         addedLane->SetNetworkRouteId(static_cast<unsigned int>(networkTraffic.mRoadRoutes.size()));
         std::unique_ptr<WsfRoute> leftLaneRoute(addedLane);
         networkTraffic.mRoadRoutes.emplace_back(std::move(leftLaneRoute));
      }
   }
}

// =================================================================================================
void XWsfOSM_Traffic::BuildWeightedGraph(unsigned int aNetIndex, RoadTrafficNetworkInput& aNetwork)
{
   TrafficInformation& networkTraffic = mRoadInfo[aNetIndex];

   unsigned int routeCount = static_cast<unsigned int>(networkTraffic.mRoadRoutes.size());
   std::unordered_map<std::string, unsigned int> lookupLanes;
   for (unsigned int i = 0; i < routeCount; ++i)
   {
      lookupLanes.emplace(networkTraffic.mRoadRoutes[i]->GetName(), i);
   }
   for (unsigned int routeIndex = 0; routeIndex < routeCount; ++routeIndex)
   {
      unsigned int routeSize = networkTraffic.mRoadRoutes[routeIndex]->GetSize();
      for (unsigned int ptIndex = 0; ptIndex < routeSize; ++ptIndex)
      {
         // create a node in the graph for each node in the route
         auto&       graph     = networkTraffic.mRoadGraphs;
         WsfWaypoint nodePoint = networkTraffic.mRoadRoutes[routeIndex]->GetWaypointAt(ptIndex);
         std::string nodeId    = nodePoint.GetNodeId();
         if (!nodeId.empty())
         {
            auto nodeIter = graph.find(WeightedRouteNode(nodeId));
            if (nodeIter == graph.end())
            {
               WeightedRouteNode node(nodeId);
               // Handle intersection types: Assume that intersection aux_data is not found on single waypoints (only
               // nodes/intersections)
               node.mIntersection = IntersectionRestriction::cNONE;
               bool stopCheck     = false;
               bool trafficCheck  = false;
               WsfUtil::GetAuxValue(nodePoint, aNetwork.mWaypointAuxDataMap.mStop, stopCheck);
               WsfUtil::GetAuxValue(nodePoint, aNetwork.mWaypointAuxDataMap.mTrafficSignals, trafficCheck);
               if (stopCheck)
               {
                  node.mIntersection = IntersectionRestriction::cSTOP;
                  node.mWaitTime     = 1.0;
               }

               if (trafficCheck)
               {
                  node.mIntersection = IntersectionRestriction::cTRAFFIC;
                  node.mWaitTime     = GetSimulation().GetRandom().Uniform(8.0, 14.0);
               }
               networkTraffic.mRoadGraphs.emplace(node);
               nodeIter = graph.find(node);
            }
            // check if current point is the endpoint of the route before adding edges to the directed graph
            if (ptIndex + 1 < routeSize)
            {
               int weight = 1;
               WsfUtil::GetAuxValue(*networkTraffic.mRoadRoutes[routeIndex], aNetwork.mRouteAuxDataMap.mDensity, weight);
               weight = std::max(1, weight);
               // determine if there are adjacent lanes
               std::string laneName     = networkTraffic.mRoadRoutes[routeIndex]->GetName();
               std::string adjacentName = laneName;
               // roads with lanes are named such that the last character is some digit corresponding to the lane:
               // Assume you cannot have double digit lanes
               char lastChar = laneName.at(laneName.size() - 1);
               if (isdigit(lastChar))
               {
                  unsigned int laneNumber = lastChar - '0';
                  if (laneNumber > 0)
                  {
                     adjacentName[laneName.size() - 1] = lastChar - 1;
                     auto it                           = lookupLanes.find(adjacentName);
                     if (it != lookupLanes.end())
                     {
                        networkTraffic.mAdjacentLanes[laneName].emplace(it->second);
                     }
                  }
                  adjacentName[laneName.size() - 1] = lastChar + 1;
                  auto it                           = lookupLanes.find(adjacentName);
                  if (it != lookupLanes.end())
                  {
                     networkTraffic.mAdjacentLanes[laneName].emplace(it->second);
                  }
               }
               nodeIter->mEdges.emplace_back(weight, routeIndex, ptIndex);
               std::string baseName = "";
               WsfUtil::GetAuxValue(*networkTraffic.mRoadRoutes[routeIndex], cBASE_ROUTE_AUX_NAME, baseName);
               if (!baseName.empty() && std::find(nodeIter->mSourceRoutes.begin(), nodeIter->mSourceRoutes.end(), baseName) ==
                                           nodeIter->mSourceRoutes.end())
               {
                  nodeIter->mSourceRoutes.push_back(baseName);
               }
            }
         }
      }
   }
}

bool XWsfOSM_Traffic::CheckNetworkValidity(unsigned int aNetIndex) const
{
   int numSources =
      0; // number of entrances into the route network (can be 2-way). 2-way stop nodes can also count as entrances
   int  numSinks = 0; // Number of one-way exits out of the route network (Only 1-way stop nodes)
   auto it       = mRoadInfo.find(aNetIndex);
   if (it != mRoadInfo.end())
   {
      const TrafficInformation& trafficInfo = it->second;
      const std::string         cSOURCE     = mNetworks[aNetIndex]->mRouteNetworkPtr->cSTART_NODE_ID;
      const std::string         cSINK       = mNetworks[aNetIndex]->mRouteNetworkPtr->cSTOP_NODE_ID;
      for (const auto& nodeIt : trafficInfo.mRoadGraphs)
      {
         if (nodeIt.mId.find(cSOURCE) != std::string::npos)
         {
            numSources++;
         }
         else if (nodeIt.mId.find(cSINK) != std::string::npos)
         {
            // If an edge exists, it means the sink node was 2-way.
            if (nodeIt.mEdges.empty())
            {
               numSinks++;
            }
            else
            {
               numSources++;
            }
         }
      }
   }
   return !(numSinks > 0 && numSources == 0);
}

bool XWsfOSM_Traffic::ValidateAuxData(RoadTrafficNetworkInput& aNetwork)
{
   bool             valid        = true;
   WsfRouteNetwork* routeNetwork = aNetwork.mRouteNetworkPtr;
   auto             routeList    = routeNetwork->GetRoutes();
   // Go through all aux_data and output all errors, instead of just the first one.
   for (const auto& routeIt : routeList)
   {
      if (routeIt->HasAuxData())
      {
         const auto& auxData = routeIt->GetAuxDataConst();
         valid = valid && CheckAttributeValidity(auxData, aNetwork.mRouteAuxDataMap.mDensity, "int", routeIt->GetName());
         valid = valid && CheckAttributeValidity(auxData, aNetwork.mRouteAuxDataMap.mLanes, "int", routeIt->GetName());
         valid = valid &&
                 CheckAttributeValidity(auxData, aNetwork.mRouteAuxDataMap.mMaxSpeed, "unitary", routeIt->GetName());
         valid = valid &&
                 CheckAttributeValidity(auxData, aNetwork.mRouteAuxDataMap.mMinSpeed, "unitary", routeIt->GetName());
         valid = valid && CheckAttributeValidity(auxData, aNetwork.mRouteAuxDataMap.mOneWay, "bool", routeIt->GetName());
         valid =
            valid && CheckAttributeValidity(auxData, aNetwork.mRouteAuxDataMap.mSurface, "string", routeIt->GetName());
      }
      for (const auto& waypointIt : routeIt->GetWaypoints())
      {
         if (waypointIt.HasAuxData())
         {
            const auto& auxData = waypointIt.GetAuxDataConst();
            valid               = valid && CheckAttributeValidity(auxData,
                                                    aNetwork.mWaypointAuxDataMap.mTrafficSignals,
                                                    "bool",
                                                    "waypoints on " + routeIt->GetName());
            valid               = valid && CheckAttributeValidity(auxData,
                                                    aNetwork.mWaypointAuxDataMap.mStop,
                                                    "bool",
                                                    "waypoints on " + routeIt->GetName());
         }
      }
   }
   return valid;
}

// =================================================================================================
void XWsfOSM_Traffic::CreateInitialPathForVehicle(const RoadTrafficNetworkInput& aNetwork,
                                                  int                            aVehicleNumber,
                                                  const std::string&             aRoute,
                                                  double                         aDistance)
{
   TrafficInformation& networkTraffic = mRoadInfo[aNetwork.mNetIndex];
   auto const&         routeIt =
      std::find_if(networkTraffic.mRoadRoutes.begin(),
                   networkTraffic.mRoadRoutes.end(),
                   [&, aRoute](std::unique_ptr<WsfRoute>& aRoad) { return aRoad->GetName() == aRoute; });
   unsigned int selectedRouteIndex;
   double       distanceAlong = 0.0;
   if (routeIt == networkTraffic.mRoadRoutes.end())
   {
      // Select a random route to place the vehicle on, based on the route weights
      std::map<int, unsigned int> routeWeights; // key = lower bound of range, value = route index
      int                         totalWeight = 0;
      for (unsigned int i = 0; i < networkTraffic.mRoadRoutes.size(); ++i)
      {
         int weight = 1;
         WsfUtil::GetAuxValue(*networkTraffic.mRoadRoutes[i], aNetwork.mRouteAuxDataMap.mDensity, weight);
         routeWeights.emplace(totalWeight, i);
         totalWeight += weight;
      }
      routeWeights.emplace(totalWeight, -1);
      int  randomIntervalLowerBound = GetSimulation().GetRandom().Uniform(1, totalWeight - 1);
      auto intervalIt               = routeWeights.upper_bound(randomIntervalLowerBound);
      selectedRouteIndex            = (--intervalIt)->second;

      // Get a random point along the route to start at, make sure not to spawn two vehicles on top (very close) to each other
      const double cDISTANCEAPART     = 10.0;
      double       totalRouteDistance = networkTraffic.mRoadRoutes[selectedRouteIndex]
                                     ->GetWaypointAt(networkTraffic.mRoadRoutes[selectedRouteIndex]->GetSize() - 1)
                                     .GetDistanceAlongRoute();
      double beginLL[2];
      double temp;
      networkTraffic.mRoadRoutes[selectedRouteIndex]->GetWaypointAt(0).GetLocationLLA(beginLL[0], beginLL[1], temp);
      std::string routeName  = networkTraffic.mRoadRoutes[selectedRouteIndex]->GetName();
      auto&       vehicleSet = networkTraffic.mRouteVehicleTracker[routeName];
      while ((vehicleSet.size() + 1.5) * cDISTANCEAPART > totalRouteDistance)
      {
         randomIntervalLowerBound = GetSimulation().GetRandom().Uniform(1, totalWeight - 1);
         intervalIt               = routeWeights.upper_bound(randomIntervalLowerBound);
         selectedRouteIndex       = (--intervalIt)->second;
         networkTraffic.mRoadRoutes[selectedRouteIndex]->GetWaypointAt(0).GetLocationLLA(beginLL[0], beginLL[1], temp);
         totalRouteDistance = networkTraffic.mRoadRoutes[selectedRouteIndex]
                                 ->GetWaypointAt(networkTraffic.mRoadRoutes[selectedRouteIndex]->GetSize() - 1)
                                 .GetDistanceAlongRoute();
         routeName  = networkTraffic.mRoadRoutes[selectedRouteIndex]->GetName();
         vehicleSet = networkTraffic.mRouteVehicleTracker[routeName];
      }

      double randomDistance = -1;
      while (randomDistance < 0)
      {
         randomDistance = GetSimulation().GetRandom().Uniform(0.0, totalRouteDistance);
         auto tooCloseIt =
            std::find_if(vehicleSet.begin(),
                         vehicleSet.end(),
                         [&, selectedRouteIndex, randomDistance](int aIndex)
                         {
                            double otherLL[2];
                            double tempA;
                            mTraffic[aIndex].mRoadPlatform->GetLocationLLA(otherLL[0], otherLL[1], tempA);
                            double otherDistance =
                               networkTraffic.mRoadRoutes[selectedRouteIndex]->GetDistanceAlongRoute(beginLL, otherLL);
                            return (otherDistance < randomDistance + cDISTANCEAPART) &&
                                   (otherDistance > randomDistance - cDISTANCEAPART);
                         });
         // make sure the vehicle spawns at least 10m away from other vehicles
         randomDistance = (tooCloseIt == vehicleSet.end()) ? randomDistance : -1;
      }
      distanceAlong = randomDistance;
   }
   else
   {
      selectedRouteIndex = static_cast<unsigned int>(routeIt - networkTraffic.mRoadRoutes.begin());
      distanceAlong      = aDistance;
   }

   double selectedPoint[2];
   int    segmentIndex;
   networkTraffic.mRoadRoutes[selectedRouteIndex]->PointAlongRoute(distanceAlong, selectedPoint, segmentIndex);

   // Get the subsequent waypoint that occurs after the random location
   auto         waypointList      = networkTraffic.mRoadRoutes[selectedRouteIndex]->GetWaypoints();
   auto         subsequentPointIt = std::find_if(waypointList.begin(),
                                         waypointList.end(),
                                         [distanceAlong](WsfWaypoint aWaypoint)
                                         { return aWaypoint.GetDistanceAlongRoute() >= distanceAlong; });
   unsigned int nextWaypointIndex =
      (subsequentPointIt != waypointList.end()) ? subsequentPointIt->GetPositionInRoute() : 0;

   // First add the initial point that occurs between waypoints
   WsfRoute route = *networkTraffic.mRoadRoutes[selectedRouteIndex];
   route.Clear();
   WsfWaypoint startPoint;
   startPoint.SetLat(selectedPoint[0]);
   startPoint.SetLon(selectedPoint[1]);
   startPoint.SetAlt(5);
   route.Append(startPoint);

   // Start adding cloned points to the new route: stop when a node intersection is hit (a new 1-segment route will be
   // generated when this is complete) in the new route, the waypoints should be the identical except for
   // PositionInRoute, DistanceAlongRoute
   int remainingSize = static_cast<int>(networkTraffic.mRoadRoutes[selectedRouteIndex]->GetSize());
   for (int i = nextWaypointIndex; i < remainingSize; ++i)
   {
      WsfWaypoint point = networkTraffic.mRoadRoutes[selectedRouteIndex]->GetWaypointAt(i);
      point.SetAlt(5);

      // If not processed or explicitly paused immediately after, extrapolate can cause unusual behavior when reaching
      // end of route (sometimes reverses direction?) However, other end of path options cannot be used because it would
      // be impossible to maintain fluid movement of the mover through traffic lights (e.g. keep going if light is green)
      point.SetEndOfPathOption(WsfPath::cEP_EXTRAPOLATE);
      route.Append(point);
      if (!point.GetNodeId().GetString().empty())
      {
         break;
      }
   }

   // Convoys will maintain a constant speed throughout the route, except to avoid collisions and at intersections
   auto typeIt            = std::find_if(aNetwork.mVehicleInput.begin(),
                              aNetwork.mVehicleInput.end(),
                              [&](const VehicleInput& aInput)
                              { return aInput.mTypeId == mTraffic[aVehicleNumber].mRoadPlatform->GetTypeId(); });
   bool ignoreSpeedLimits = (typeIt != aNetwork.mVehicleInput.end()) ? typeIt->mIgnoreSpeedLimits : false;
   AssignRandomSpeeds(route, aNetwork.mRouteAuxDataMap, mTraffic[aVehicleNumber].mSpeed, ignoreSpeedLimits);

   // Assign the initial path to the appropriate mover.
   WsfMover* moverPtr = mTraffic[aVehicleNumber].mRoadPlatform->GetMover();
   if (moverPtr != nullptr)
   {
      moverPtr->SetRoute(GetSimulation().GetSimTime(), route);
      networkTraffic.mRouteVehicleTracker[route.GetName()].emplace(aVehicleNumber);
      mTraffic[aVehicleNumber].mCurrentRouteName = route.GetName();
   }
   else
   {
      auto out = ut::log::error() << "osm_traffic: Cannot set the mover's path because the mover is null.";
      out.AddNote() << "Platform: " << mTraffic[aVehicleNumber].mRoadPlatform->GetName();
   }
}

// =================================================================================================
double XWsfOSM_Traffic::GetRandomSpeed(const WsfRoute&        aRoute,
                                       const RouteAuxMapping& aRouteAuxMap,
                                       double                 aVehicleSpeed,
                                       bool                   aIgnoreSpeedLimit)
{
   double mean;
   double stdDev;
   GetMeanAndStdDev(mean, stdDev, aRoute, aRouteAuxMap, aVehicleSpeed, aIgnoreSpeedLimit);
   return GetSimulation().GetRandom().Gaussian(mean, stdDev);
}

// =================================================================================================
void XWsfOSM_Traffic::AssignRandomSpeeds(WsfRoute&              aRoute,
                                         const RouteAuxMapping& aRouteAuxMap,
                                         double                 aVehicleSpeed,
                                         bool                   aIgnoreSpeedLimit,
                                         bool                   aConvoy)
{
   double mean;
   double stdDev;
   GetMeanAndStdDev(mean, stdDev, aRoute, aRouteAuxMap, aVehicleSpeed, aIgnoreSpeedLimit);
   for (unsigned int i = 0; i < aRoute.GetSize(); ++i)
   {
      if (!aConvoy)
      {
         aRoute.GetWaypointAt(i).SetSpeed(GetSimulation().GetRandom().Gaussian(mean, stdDev));
      }
      else
      {
         aRoute.GetWaypointAt(i).SetSpeed(mean);
      }
   }
}

// =================================================================================================
std::string XWsfOSM_Traffic::GetRandomSourceRoute(unsigned int aNetworkId) const
{
   const TrafficInformation& trafficInfo = mRoadInfo.at(aNetworkId);
   const std::string         cSOURCE     = mNetworks[aNetworkId]->mRouteNetworkPtr->cSTART_NODE_ID;
   const std::string         cSINK       = mNetworks[aNetworkId]->mRouteNetworkPtr->cSTOP_NODE_ID;
   std::vector<std::string>  sourceList;
   for (const auto& nodeIt : trafficInfo.mRoadRoutes)
   {
      auto        waypoint = nodeIt->GetWaypointAt(0);
      std::string node     = waypoint.GetNodeId();
      if (node.find(cSOURCE) != std::string::npos || node.find(cSINK) != std::string::npos)
      {
         sourceList.push_back(nodeIt->GetName());
      }
   }
   if (!sourceList.empty())
   {
      size_t randomIndex = GetSimulation().GetRandom().Uniform<size_t>(0, sourceList.size() - 1);
      return sourceList[randomIndex];
   }
   return std::string{};
}

// =================================================================================================
void XWsfOSM_Traffic::VehiclePathEndedHandler(double aSimTime, WsfMover* aMover)
{
   int vehicleNumber = -1;
   int trafficSize   = static_cast<int>(mTraffic.size());
   for (int i = 0; i < trafficSize; ++i)
   {
      if (mTraffic[i].mRoadPlatform == aMover->GetPlatform())
      {
         vehicleNumber = i;
         break;
      }
   }
   if (vehicleNumber >= 0 && !mTraffic[vehicleNumber].mScripted)
   {
      WsfPlatform* ownerPlatform = GetSimulation().GetPlatformByIndex(mTraffic[vehicleNumber].mRoadPlatformIndex);
      WsfMover*    testMover     = ownerPlatform->GetMover();
      int          networkId     = mTraffic[vehicleNumber].mNetwork;

      if (!testMover)
      {
         ut::log::error() << "Owner Platform doesn't have a valid mover in XWsfOSM_Traffic::VehiclePathEndedHandler()";
         return;
      }

      const WsfRoute* route   = testMover->GetRoute();
      int             routeId = 0;
      if (!route)
      {
         ut::log::error()
            << "Owner Platform mover doesn't have a valid route in XWsfOSM_Traffic::VehiclePathEndedHandler()";
         return;
      }
      routeId = route->GetNetworkRouteId();
      WsfWaypoint        before;
      const WsfWaypoint& lastWaypoint = route->GetWaypointAt(route->GetSize() - 1);

      TrafficInformation& networkTraffic = mRoadInfo[networkId];

      // Use the original route instead of the mover's, since the mover sometimes won't have enough waypoints
      WsfWaypoint beforeLast =
         networkTraffic.mRoadRoutes[routeId]->GetWaypointAt(networkTraffic.mRoadRoutes[routeId]->GetSize() - 2);
      WsfStringId nodeId = lastWaypoint.GetNodeId();

      if (!nodeId.Empty())
      {
         auto nodeIt = networkTraffic.mRoadGraphs.find(nodeId.GetString());
         if (nodeIt != networkTraffic.mRoadGraphs.end())
         {
            // Convoys need separate logic because vehicles after the lead vehicle only need to follow the lead (does not
            // need to obey traffic) the lead vehicle still obeys normal traffic laws, and is handled like any other vehicle
            int  convoyNumber = mTraffic[vehicleNumber].mConvoyNumber;
            auto convoyIndexIt =
               std::find(mConvoyIndices[convoyNumber].begin(), mConvoyIndices[convoyNumber].end(), vehicleNumber);
            if (convoyNumber > 0 && convoyIndexIt != mConvoyIndices[convoyNumber].begin() &&
                convoyIndexIt != mConvoyIndices[convoyNumber].end())
            {
               // Convoys must be handled exclusively by queued operations because the route of the preceding vehicle is
               // not guaranteed to be the correct one for the current vehicle (the preceding vehicle can go through
               // multiple routes in just a few seconds), so just pause and handle later if it gets to this point
               WsfRouteMover* rMover = dynamic_cast<WsfRouteMover*>(mTraffic[*convoyIndexIt].mRoadPlatform->GetMover());
               rMover->Pause(aSimTime, std::numeric_limits<double>::max());
            }
            else
            {
               // Doesn't handle case where user provides a single node_id on a one-way, one lane route (specifies an
               // intersection when there isn't actually a intersection anywhere else in the route_network) Artificially
               // lower the weight associated with U-turns (routes sharing the same prefix) (increase the weight of all
               // other options by multiplying weight of non-U-turns)
               std::vector<WeightedRouteEdge> edgeCopies = nodeIt->mEdges;
               if (edgeCopies.empty())
               {
                  GetSimulation().DeletePlatform(aSimTime, aMover->GetPlatform());
                  int                      num;
                  RoadTrafficNetworkInput* networkPtr = static_cast<RoadTrafficNetworkInput*>(mNetworks[networkId].get());
                  if (networkPtr)
                  {
                     if (CreateVehicle(*networkPtr, aSimTime, num, true))
                     {
                        AddPlatformToSimulation(aSimTime, num);
                     }
                  }
               }
               else
               {
                  int totalWeight = 0;
                  for (auto& edgeCopy : edgeCopies)
                  {
                     std::string        nameEdge    = networkTraffic.mRoadRoutes[edgeCopy.mRouteIndex]->GetName();
                     const std::string& nameCurrent = route->GetName();
                     std::string        baseEdge    = nameEdge.substr(0, nameEdge.find_last_of('_'));
                     std::string        baseCurrent = nameCurrent.substr(0, nameCurrent.find_last_of('_'));
                     // The first point of the current route should not be the last point of the newly assigned route
                     std::string currentStartNode = route->GetWaypointAt(0).GetNodeId().GetString();
                     std::string edgeEndNode =
                        networkTraffic.mRoadRoutes[edgeCopy.mRouteIndex]
                           ->GetWaypointAt(networkTraffic.mRoadRoutes[edgeCopy.mRouteIndex]->GetSize() - 1)
                           .GetNodeId();
                     if (baseEdge != baseCurrent && currentStartNode != edgeEndNode)
                     {
                        edgeCopy.mWeight *= 6;
                     }
                     totalWeight += edgeCopy.mWeight;
                  }
                  int          random        = GetSimulation().GetRandom().Uniform(0, std::max(0, totalWeight - 1));
                  unsigned int selectedIndex = 0;
                  for (; selectedIndex < nodeIt->mEdges.size(); selectedIndex++)
                  {
                     if (random < edgeCopies[selectedIndex].mWeight)
                     {
                        break;
                     }
                     random -= edgeCopies[selectedIndex].mWeight;
                  }
                  // Add the new route
                  int firstPointIndex = nodeIt->mEdges[selectedIndex].mWaypointIndex;
                  int routeIndex      = nodeIt->mEdges[selectedIndex].mRouteIndex;
                  // Done in the following way instead of removing points in order to preserve "distanceAlongRoute"
                  WsfRoute newRoute = *networkTraffic.mRoadRoutes[routeIndex];
                  newRoute.Clear();
                  for (int waypointIndex = firstPointIndex;
                       waypointIndex < static_cast<int>(networkTraffic.mRoadRoutes[routeIndex]->GetSize());
                       waypointIndex++)
                  {
                     WsfWaypoint nextPoint = networkTraffic.mRoadRoutes[routeIndex]->GetWaypointAt(waypointIndex);
                     nextPoint.SetAlt(5);
                     nextPoint.SetNodeId(networkTraffic.mRoadRoutes[routeIndex]->GetWaypointAt(waypointIndex).GetNodeId());
                     newRoute.Append(nextPoint);
                     if (waypointIndex != firstPointIndex && !nextPoint.GetNodeId().GetString().empty())
                     {
                        break;
                     }
                  }
                  // This "TODO" is for future behavior that is not currently necessary, but may be required in the
                  // future
                  // TODO: Angles between routes can be found using the following code. Can be used in the future if
                  // intersections need to allow multiple routes to simultaneously cross
                  //       For more accurate behavior, it may be necessary to use the base routes (suffix "__normal1")
                  //       instead of lane route for angle calculations
                  // double ECEF1[3];
                  // double ECEF2[3];
                  // UtSphericalEarth::ConvertLLAToECEF(lastWaypoint.GetLat(), lastWaypoint.GetLon(),
                  // lastWaypoint.GetAlt(), ECEF1); UtSphericalEarth::ConvertLLAToECEF(beforeLast.GetLat(),
                  // beforeLast.GetLon(), beforeLast.GetAlt(), ECEF2); UtVec3<double> originalVector(ECEF2[0] -
                  // ECEF1[0], ECEF2[1] - ECEF1[1], ECEF2[2] - ECEF1[2]); WsfWaypoint lastWaypointNew =
                  // newRoute->GetWaypointAt(newRoute->GetSize() - 1); WsfWaypoint beforeLastNew =
                  // newRoute->GetWaypointAt(newRoute->GetSize() - 2);
                  // UtSphericalEarth::ConvertLLAToECEF(lastWaypointNew.GetLat(), lastWaypointNew.GetLon(),
                  // lastWaypointNew.GetAlt(), ECEF1); UtSphericalEarth::ConvertLLAToECEF(beforeLastNew.GetLat(),
                  // beforeLastNew.GetLon(), beforeLastNew.GetAlt(), ECEF2); UtVec3<double> nextVector(ECEF2[0] -
                  // ECEF1[0], ECEF2[1] - ECEF1[1], ECEF2[2] - ECEF1[2]); double angle =
                  // originalVector.AngleWith(nextVector) * 180 / (atan(1) * 4);

                  RoadTrafficNetworkInput& netInput = static_cast<RoadTrafficNetworkInput&>(*mNetworks[networkId]);
                  // old route name will be overwritten when the mover updates the route
                  const std::string& oldRouteName = route->GetName();
                  auto               typeIt =
                     std::find_if(netInput.mVehicleInput.begin(),
                                  netInput.mVehicleInput.end(),
                                  [&](const VehicleInput& aInput)
                                  { return aInput.mTypeId == mTraffic[vehicleNumber].mRoadPlatform->GetTypeId(); });
                  bool ignoreSpeedLimits = (typeIt != netInput.mVehicleInput.end()) ? typeIt->mIgnoreSpeedLimits : false;
                  AssignRandomSpeeds(newRoute, netInput.mRouteAuxDataMap, mTraffic[vehicleNumber].mSpeed, ignoreSpeedLimits);

                  WsfRouteMover* rMover   = dynamic_cast<WsfRouteMover*>(testMover);
                  std::string    baseName = "";
                  WsfUtil::GetAuxValue(route, cBASE_ROUTE_AUX_NAME, baseName);
                  RoutePair sourceDest(baseName, newRoute.GetName(), nodeId.GetString());

                  auto UpdateRouteCheckConvoy =
                     [&, aSimTime, vehicleNumber, nodeId, oldRouteName](WsfMover*        aMover,
                                                                        WsfRoute&        aNewRoute,
                                                                        const RoutePair& aRoutePair)
                  {
                     auto intersectionIt = networkTraffic.mIntersectionsInUse.find(nodeId);
                     if (intersectionIt == networkTraffic.mIntersectionsInUse.end())
                     {
                        // std::string oldRouteName = aMover->GetRoute()->GetName();
                        aMover->UpdateRoute(aSimTime, aNewRoute);
                        // if this is the first vehicle in a convoy, reserve the intersection
                        if (mTraffic[vehicleNumber].mConvoyNumber > 0)
                        {
                           networkTraffic.mIntersectionsInUse.emplace(nodeId, mTraffic[vehicleNumber].mConvoyNumber);
                           double spacing = netInput.mConvoyInput[mTraffic[vehicleNumber].mConvoyNumber - 1].mSpacing;
                           for (unsigned int j = 1; j < mConvoyIndices[mTraffic[vehicleNumber].mConvoyNumber].size(); ++j)
                           {
                              networkTraffic.mQueuedConvoys[mTraffic[vehicleNumber].mConvoyNumber].emplace_back(
                                 mConvoyIndices[mTraffic[vehicleNumber].mConvoyNumber][j],
                                 aNewRoute,
                                 aSimTime + (j * spacing));
                           }
                           std::sort(networkTraffic.mQueuedConvoys[mTraffic[vehicleNumber].mConvoyNumber].begin(),
                                     networkTraffic.mQueuedConvoys[mTraffic[vehicleNumber].mConvoyNumber].end(),
                                     [](const QueuedOperation& aOp1, const QueuedOperation& aOp2)
                                     { return (aOp1.mAddedTime < aOp2.mAddedTime); });
                           mNextConvoyUpdateTime =
                              std::min(networkTraffic.mQueuedConvoys[mTraffic[vehicleNumber].mConvoyNumber].front().mAddedTime,
                                       mNextConvoyUpdateTime);
                        }
                        networkTraffic.mRouteVehicleTracker[oldRouteName].erase(vehicleNumber);
                        networkTraffic.mRouteVehicleTracker[newRoute.GetName()].emplace(vehicleNumber);
                        mTraffic[vehicleNumber].mCurrentRouteName = newRoute.GetName();
                     }
                     else
                     {
                        // Other convoy vehicles outside of the lead are handled elsewhere, so this section only
                        // addresses normal background vehicles normal background vehicles will need to wait for the
                        // convoy to finish passing the intersection before moving again
                        WsfRouteMover* rMover = dynamic_cast<WsfRouteMover*>(aMover);
                        rMover->Pause(aSimTime, std::numeric_limits<double>::max());
                        networkTraffic.mQueuedOperations[aRoutePair].emplace_back(vehicleNumber, aNewRoute, aSimTime);
                        networkTraffic.mAtIntersection[oldRouteName][vehicleNumber] =
                           networkTraffic.mRoadRoutes[routeId]->GetDistance(0,
                                                                            networkTraffic.mRoadRoutes[routeId]->GetSize() -
                                                                               1);
                     }
                  };

                  // When creating routes, AFSIM adds text to the start / end node ids, so just check if it is contained
                  // in the route and hope the user doesn't have an identical node name (shouldn't since this has double underscores)
                  std::string nodeId2 = newRoute.GetWaypointAt(0).GetNodeId();
                  if (nodeId2.find(mNetworks[networkId]->mRouteNetworkPtr->cSTART_NODE_ID) != std::string::npos ||
                      nodeId2.find(mNetworks[networkId]->mRouteNetworkPtr->cSTOP_NODE_ID) != std::string::npos)
                  {
                     // First attempt to make a u-turn. If this is not possible, respawn the vehicle somewhere else
                     std::vector<unsigned int> possibleTurns;
                     std::vector<unsigned int> currentlyReservedTurns;
                     for (const auto& turnIt : nodeIt->mEdges)
                     {
                        std::string normalBase   = baseName + cRIGHT_LANE_SUFFIX;
                        std::string oppositeBase = baseName + cLEFT_LANE_SUFFIX;
                        if ((networkTraffic.mRoadRoutes[turnIt.mRouteIndex]->GetName().find(normalBase) != std::string::npos ||
                             networkTraffic.mRoadRoutes[turnIt.mRouteIndex]->GetName().find(oppositeBase) !=
                                std::string::npos))
                        {
                           if (networkTraffic.mReservedRoads.find(networkTraffic.mRoadRoutes[turnIt.mRouteIndex]->GetName()) ==
                               networkTraffic.mReservedRoads.end())
                           {
                              possibleTurns.emplace_back(turnIt.mRouteIndex);
                           }
                           else
                           {
                              currentlyReservedTurns.emplace_back(turnIt.mRouteIndex);
                           }
                        }
                     }
                     auto typeIt =
                        std::find_if(netInput.mVehicleInput.begin(),
                                     netInput.mVehicleInput.end(),
                                     [&](const VehicleInput& aInput)
                                     { return aInput.mTypeId == mTraffic[vehicleNumber].mRoadPlatform->GetTypeId(); });
                     bool ignoreSpeedLimits = (typeIt != netInput.mVehicleInput.end()) ? typeIt->mIgnoreSpeedLimits : false;
                     if (!possibleTurns.empty())
                     {
                        size_t selectedRouteIndex =
                           GetSimulation().GetRandom().Uniform<size_t>(0, possibleTurns.size() - 1);
                        newRoute = CopyOriginalRoute(networkId, possibleTurns[selectedRouteIndex], 0);
                        AssignRandomSpeeds(newRoute,
                                           netInput.mRouteAuxDataMap,
                                           mTraffic[vehicleNumber].mSpeed,
                                           ignoreSpeedLimits);
                        sourceDest = RoutePair(baseName, newRoute.GetName(), nodeId.GetString());
                        UpdateRouteCheckConvoy(aMover, newRoute, sourceDest);
                     }
                     else if (!currentlyReservedTurns.empty())
                     {
                        size_t selectedRouteIndex =
                           GetSimulation().GetRandom().Uniform<size_t>(0, currentlyReservedTurns.size() - 1);
                        newRoute = CopyOriginalRoute(networkId, currentlyReservedTurns[selectedRouteIndex], 0);
                        AssignRandomSpeeds(newRoute,
                                           netInput.mRouteAuxDataMap,
                                           mTraffic[vehicleNumber].mSpeed,
                                           ignoreSpeedLimits);
                        sourceDest = RoutePair(baseName, newRoute.GetName(), nodeId.GetString());
                        rMover->Pause(aSimTime, std::numeric_limits<double>::max());
                        networkTraffic.mQueuedOperations[sourceDest].emplace_back(vehicleNumber,
                                                                                  newRoute,
                                                                                  std::numeric_limits<double>::max());
                     }
                     else
                     {
                        // Set will instantly change the route (teleport), but is not connected to some of the display
                        // mechanisms in AFSIM applications (e.g. Warlock), so UpdateRoute is called to trigger a visual update
                        auto intersectionIt = networkTraffic.mIntersectionsInUse.find(nodeId);
                        // Only teleport when the intersection is available, otherwise queue the operation up via the
                        // UpdateROuteCheckConvoy lambda
                        if (intersectionIt == networkTraffic.mIntersectionsInUse.end())
                        {
                           aMover->SetRoute(aSimTime, newRoute);
                        }
                        UpdateRouteCheckConvoy(aMover, newRoute, sourceDest);
                     }
                  }
                  else
                  {
                     // Rotating priority on vehicles at an intersection (e.g. 4-way intersection, only 1 source route
                     // group active at a time)
                     // TODO: increase complexity of intersection logic in the future if more detail is necessary (like
                     // allowing two directions to go simultaneously)
                     if (nodeIt->mIntersection == IntersectionRestriction::cTRAFFIC)
                     {
                        auto sourceIt = std::find(nodeIt->mSourceRoutes.begin(), nodeIt->mSourceRoutes.end(), baseName);
                        if (sourceIt != nodeIt->mSourceRoutes.end())
                        {
                           unsigned int sourceIndex = static_cast<unsigned int>(sourceIt - nodeIt->mSourceRoutes.begin());
                           double       modTime = std::fmod(aSimTime, nodeIt->mSourceRoutes.size() * nodeIt->mWaitTime);
                           double       lowerBound = sourceIndex * nodeIt->mWaitTime;
                           double       upperBound = (sourceIndex + 1) * nodeIt->mWaitTime;
                           // Check whether the intersection is currently allowing vehicles through from this specific
                           // source route group (e.g. all the lanes in a direction from a road)
                           if (modTime >= lowerBound && modTime < upperBound)
                           {
                              // Check if there is already a backlog of vehicles and add to queue if there is, otherwise
                              // immediately update the route
                              if (networkTraffic.mQueuedOperations.find(sourceDest) !=
                                     networkTraffic.mQueuedOperations.end() &&
                                  !networkTraffic.mQueuedOperations[sourceDest].empty())
                              {
                                 rMover->Pause(aSimTime, std::numeric_limits<double>::max());
                                 networkTraffic.mQueuedOperations[sourceDest].emplace_back(vehicleNumber, newRoute, aSimTime);
                                 networkTraffic.mAtIntersection[oldRouteName].emplace(
                                    vehicleNumber,
                                    networkTraffic.mRoadRoutes[routeId]
                                       ->GetDistance(0, networkTraffic.mRoadRoutes[routeId]->GetSize() - 1));
                              }
                              else
                              {
                                 UpdateRouteCheckConvoy(testMover, newRoute, sourceDest);
                              }
                           }
                           else
                           {
                              rMover->Pause(aSimTime, std::numeric_limits<double>::max());
                              networkTraffic.mQueuedOperations[sourceDest].emplace_back(vehicleNumber, newRoute, aSimTime);
                              networkTraffic.mAtIntersection[oldRouteName].emplace(
                                 vehicleNumber,
                                 networkTraffic.mRoadRoutes[routeId]
                                    ->GetDistance(0, networkTraffic.mRoadRoutes[routeId]->GetSize() - 1));
                           }
                        }
                        else
                        {
                           UpdateRouteCheckConvoy(testMover, newRoute, sourceDest);
                        }
                     }
                     else if (nodeIt->mIntersection == IntersectionRestriction::cSTOP)
                     {
                        rMover->Pause(aSimTime, std::numeric_limits<double>::max());
                        networkTraffic.mQueuedOperations[sourceDest].emplace_back(vehicleNumber, newRoute, aSimTime);
                        networkTraffic.mAtIntersection[oldRouteName].emplace(
                           vehicleNumber,
                           networkTraffic.mRoadRoutes[routeId]->GetDistance(0,
                                                                            networkTraffic.mRoadRoutes[routeId]->GetSize() -
                                                                               1));
                     }
                     else
                     {
                        UpdateRouteCheckConvoy(testMover, newRoute, sourceDest);
                     }
                  }
               }
               // networkTraffic.mRouteVehicleTracker[oldRouteName].erase(vehicleNumber);
               // networkTraffic.mRouteVehicleTracker[newRoute.GetName()].emplace(vehicleNumber);
               // mTraffic[vehicleNumber].mCurrentRouteName = newRoute.GetName();
            }
         }
      }
   }
}

// =================================================================================================
void XWsfOSM_Traffic::PlatformDeletedHandler(double aSimTime, WsfPlatform* aPlatform)
{
   // Don't do anything when platforms are deleted due to the sim ending
   if (GetSimulation().GetCompletionReason() != WsfSimulation::CompletionReason::cNONE)
   {
      return;
   }
   auto vehicleIt = std::find_if(mTraffic.begin(),
                                 mTraffic.end(),
                                 [aPlatform](const SGroundTraffic& aInfo) { return aPlatform == aInfo.mRoadPlatform; });
   if (vehicleIt != mTraffic.end())
   {
      TrafficInformation& networkTraffic = mRoadInfo[vehicleIt->mNetwork];
      int                 vehicleNumber  = static_cast<int>(vehicleIt - mTraffic.begin());
      int                 convoyNumber   = mTraffic[vehicleNumber].mConvoyNumber;
      if (!mTraffic[vehicleNumber].mCurrentRouteName.empty())
      {
         // Handle the deletion of regular background traffic
         if (mTraffic[vehicleNumber].mConvoyNumber <= 0)
         {
            // Get rid of any queued operations involving this vehicle: there will only potentially be queued operations
            // when the vehicle is stopped
            if (mTraffic[vehicleNumber].mRoadPlatform->GetSpeed() == 0.0)
            {
               for (auto& queueIt : networkTraffic.mQueuedOperations)
               {
                  for (auto opIt = queueIt.second.begin(); opIt != queueIt.second.end();)
                  {
                     if (opIt->mVehicleId == vehicleNumber)
                     {
                        opIt = queueIt.second.erase(opIt);
                     }
                     else
                     {
                        ++opIt;
                     }
                  }
               }
            }
         }
         // Handle the deletion of scripted convoy vehicles
         else if (mTraffic[vehicleNumber].mScripted)
         {
            auto convoyVehicleIt =
               std::find(mConvoyIndices[convoyNumber].begin(), mConvoyIndices[convoyNumber].end(), vehicleNumber);
            if (convoyVehicleIt != mConvoyIndices[convoyNumber].end())
            {
               mConvoyIndices[convoyNumber].erase(convoyVehicleIt);
            }
         }
         // TODO: handle deletion of background convoy vehicles (low priority)
         else
         {
            // Lead vehicle was deleted, so the second vehicle now becomes the leader
            if (mConvoyIndices[convoyNumber][0] == vehicleNumber)
            {
            }
            // last vehicle of the convoy was deleted
            else if (mConvoyIndices[convoyNumber][mConvoyIndices[convoyNumber].size() - 1] == vehicleNumber)
            {
               mConvoyIndices[convoyNumber].erase(mConvoyIndices[convoyNumber].begin() +
                                                  (mConvoyIndices[convoyNumber].size() - 1));
            }
            // Middle vehicle is deleted, so "catchup" is required for the now lagging vehicles to maintain the original
            // spacing of the convoy If another middle vehicle is deleted while a "catchup" is happening, finish the
            // current catchup (minus the newly deleted vehicle), then handle the rest later
            else
            {
               auto convoyVehicleIt =
                  std::find(mConvoyIndices[convoyNumber].begin(), mConvoyIndices[convoyNumber].end(), vehicleNumber);
               if (convoyVehicleIt != mConvoyIndices[convoyNumber].end())
               {
                  // Get the index of vehicle after the deleted platform
                  auto index =
                     ut::safe_cast<unsigned int, ptrdiff_t>(convoyVehicleIt - mConvoyIndices[convoyNumber].begin());
                  // Set the catchup speed: Start by doubling the current speed, then account for the max mover speed of
                  // all the trailing vehicles (which can have differing types and max speeds)
                  double catchupSpeed = mTraffic[mConvoyIndices[convoyNumber][index - 1]].mRoadPlatform->GetSpeed() * 2;
                  for (unsigned int i = index; i < mConvoyIndices[convoyNumber].size(); ++i)
                  {
                     WsfRouteMover* rMover = dynamic_cast<WsfRouteMover*>(
                        mTraffic[mConvoyIndices[convoyNumber][index]].mRoadPlatform->GetMover());
                     catchupSpeed = std::min(catchupSpeed, rMover->GetMoverConstraints()->mMaxSpeed);
                  }
                  mConvoyIndices[convoyNumber].erase(convoyVehicleIt);
                  ConvoyCatchUpHandler(aSimTime, convoyNumber, index, catchupSpeed);
               }
            }
         }
         // Stop tracking this vehicle
         networkTraffic.mRouteVehicleTracker[mTraffic[vehicleNumber].mCurrentRouteName].erase(vehicleNumber);
         DeletePlatformFromSimulation(aSimTime, vehicleNumber);
      }
   }

   // FOR CONVOYS
   // always modify the convoyIndices variables when things are removed
   // if the vehicle is currently stopped/paused, get rid of all queued operations involving this vehicle, also fast
   // forward all subsequent indices' operations by the spacing variable if the vehicle was moving when it was deleted,
   // run a "catchup" function to speed up the lagging convoy vehicles until they are caught up
   //   based on the ahead vehicle's speed, speed up and calculate how much time the speedup is necessary before
   //   switching to the ahead vehicle's route if the ahead vehicle changes speed, do a recalculation
   // if the lead vehicle is deleted, the subsequent vehicle becomes the leader
   // if the leader is deleted at an intersection, find its queued operation, and replace the id with the subsequent
   // vehicle. furthermore, remove the subsequent vehicle from the
   //   the queued convoy operations and fast forward all subsequent indices' operations by the spacing variable
}

// =================================================================================================
void XWsfOSM_Traffic::ConvoyCatchUpHandler(double aSimTime, int aConvoyNumber, unsigned int aIndex, double aCatchupSpeed)
{
   int                      vehicleNumber = mConvoyIndices[aConvoyNumber][aIndex];
   RoadTrafficNetworkInput* networkPtr =
      static_cast<RoadTrafficNetworkInput*>(mNetworks[mTraffic[vehicleNumber].mNetwork].get());
   TrafficInformation& networkTraffic     = mRoadInfo[mTraffic[vehicleNumber].mNetwork];
   std::string         currentRouteName   = mTraffic[aIndex].mCurrentRouteName;
   int                 aheadVehicleNumber = mConvoyIndices[aConvoyNumber][aIndex - 1];
   auto                routeIt            = std::find_if(networkTraffic.mRoadRoutes.begin(),
                               networkTraffic.mRoadRoutes.end(),
                               [currentRouteName](std::unique_ptr<WsfRoute>& aRoad)
                               { return aRoad->GetName() == currentRouteName; });
   if (routeIt != networkTraffic.mRoadRoutes.end())
   {
      double temp;
      double beginLL[2];
      double currentLL[2];
      routeIt->get()->GetWaypointAt(0).GetLocationLLA(beginLL[0], beginLL[1], temp);
      mTraffic[aheadVehicleNumber].mRoadPlatform->GetLocationLLA(currentLL[0], currentLL[1], temp);
      double aheadDistance = routeIt->get()->GetDistanceAlongRoute(beginLL, currentLL);
      mTraffic[vehicleNumber].mRoadPlatform->GetLocationLLA(currentLL[0], currentLL[1], temp);
      double currentDistance = routeIt->get()->GetDistanceAlongRoute(beginLL, currentLL);
      double timeSpacing     = networkPtr->mConvoyInput[aConvoyNumber - 1].mSpacing;
      double aheadSpeed      = mTraffic[aheadVehicleNumber].mRoadPlatform->GetSpeed();
      // Formula for calculating the time that the behind vehicle should change speed to the vehicle its catching up to,
      // assuming both vehicles do not change speed for the duration If the "ahead" vehicle does change its speed, the
      // timeOfCatchUp needs to be recalculated and the Queued change will need to be overwritten (handled elsewhere)
      // aheadDistance
      // + (aheadSpeed * t) = currentDistance + (catchupSpeed * t) + (aheadSpeed * spacing)
      double timeOfCatchUp = (aheadDistance - currentDistance - (aheadSpeed * timeSpacing)) / (aCatchupSpeed - aheadSpeed);

      for (unsigned int i = aIndex; i < mConvoyIndices[aConvoyNumber].size(); ++i)
      {
         // Accelerate/Decelerate the mover to allow it to catch up
         mTraffic[mConvoyIndices[aConvoyNumber][i]].mCatchUp = true;
         WsfWaypointMover* rMover =
            dynamic_cast<WsfWaypointMover*>(mTraffic[mConvoyIndices[aConvoyNumber][i]].mRoadPlatform->GetMover());
         rMover->GoToSpeed(aSimTime, aCatchupSpeed, 10000, true);

         // Push a queued operation to return to the normal speed after the vehicle catches up
         mQueuedCatchUp.emplace_back(mConvoyIndices[aConvoyNumber][i], 0, aheadSpeed, aSimTime + timeOfCatchUp);
         std::sort(mQueuedCatchUp.begin(),
                   mQueuedCatchUp.end(),
                   [](const QueuedSpeedChange& aOp1, const QueuedSpeedChange& aOp2) { return (aOp1.mTime < aOp2.mTime); });
         mNextConvoyCatchUpTime = mQueuedCatchUp.front().mTime;
      }
   }
}

// =================================================================================================
void XWsfOSM_Traffic::TrackerCreationHelper(std::vector<TrackerDetails>& aTracker, unsigned int aVehicleNumber)
{
   TrafficInformation& networkTraffic  = mRoadInfo[mTraffic[aVehicleNumber].mNetwork];
   WsfPlatform*        vehiclePlatform = mTraffic[aVehicleNumber].mRoadPlatform;

   if (vehiclePlatform->GetMover() && vehiclePlatform->GetMover()->GetRoute()) // has mover and route
   {
      unsigned int vehicleRouteId = vehiclePlatform->GetMover()->GetRoute()->GetNetworkRouteId();
      WsfRoute*    vehicleRoute   = networkTraffic.mRoadRoutes[vehicleRouteId].get();
      double       beginLL[2];
      double       vehicleLL[2];
      double       temp;
      vehicleRoute->GetWaypointAt(0).GetLocationLLA(beginLL[0], beginLL[1], temp);
      vehiclePlatform->GetLocationLLA(vehicleLL[0], vehicleLL[1], temp);
      double distance = std::abs(vehicleRoute->GetDistanceAlongRoute(beginLL, vehicleLL));
      aTracker.emplace_back(TrackerDetails(aVehicleNumber, distance));
   }
}

// =================================================================================================
bool XWsfOSM_Traffic::CheckLaneAvailability(int                                aVehicleNumber,
                                            unsigned int                       aLaneIndex,
                                            const std::vector<TrackerDetails>& aTracker,
                                            double                             aDistance,
                                            double*                            aNextPoint)
{
   bool                changeLanes    = true;
   TrafficInformation& networkTraffic = mRoadInfo[mTraffic[aVehicleNumber].mNetwork];
   int                 tempSegment;
   if (networkTraffic.mRoadRoutes[aLaneIndex]->PointAlongRoute(aDistance, aNextPoint, tempSegment))
   {
      WsfRoute route                 = *networkTraffic.mRoadRoutes[aLaneIndex];
      double   aheadVehicleDistance  = route.GetWaypointAt(route.GetSize() - 1).GetDistanceAlongRoute();
      double   behindVehicleDistance = 0;
      if (!aTracker.empty())
      {
         auto aheadVehicleIt = std::upper_bound(aTracker.begin(),
                                                aTracker.end(),
                                                TrackerDetails(0, aDistance),
                                                [](const TrackerDetails& aDetail1, const TrackerDetails& aDetail2)
                                                { return aDetail1 < aDetail2; });
         if (aheadVehicleIt != aTracker.end())
         {
            aheadVehicleDistance = aheadVehicleIt->mDistanceAlongRoute;
            changeLanes          = changeLanes && (aheadVehicleDistance >
                                          aDistance + mTraffic[aheadVehicleIt->mVehicleId].mRoadPlatform->GetSpeed());
            // Handle case where there are no vehicles behind the target
            if (aheadVehicleIt != aTracker.begin())
            {
               auto behindVehicleIt  = aheadVehicleIt - 1;
               behindVehicleDistance = behindVehicleIt->mDistanceAlongRoute;
               changeLanes =
                  changeLanes &&
                  (behindVehicleDistance < aDistance - mTraffic[behindVehicleIt->mVehicleId].mRoadPlatform->GetSpeed());
               if ((mTraffic[aheadVehicleIt->mVehicleId].mConvoyNumber > 0) &&
                   (mTraffic[aheadVehicleIt->mVehicleId].mConvoyNumber ==
                    mTraffic[behindVehicleIt->mVehicleId].mConvoyNumber))
               {
                  changeLanes = false;
               }
            }
         }
         else
         {
            // Handles case where there are no vehicles ahead of the target
            behindVehicleDistance = aTracker[aTracker.size() - 1].mDistanceAlongRoute;
            changeLanes           = changeLanes &&
                          (behindVehicleDistance <
                           aDistance - mTraffic[aTracker[aTracker.size() - 1].mVehicleId].mRoadPlatform->GetSpeed());
         }
      }
   }
   return changeLanes;
}

// =================================================================================================
void XWsfOSM_Traffic::ChangeLane(int aVehicleNumber, unsigned int aLaneIndex, double aDistance, const double* aNextPoint)
{
   TrafficInformation& networkTraffic = mRoadInfo[mTraffic[aVehicleNumber].mNetwork];
   WsfRoute            newRoute       = *networkTraffic.mRoadRoutes[aLaneIndex];
   newRoute.Clear();
   WsfWaypoint startPoint;
   startPoint.SetLocationLLA(aNextPoint[0], aNextPoint[1], 5.0);
   newRoute.Append(startPoint);

   // Get the subsequent waypoint that occurs after the initial location
   auto waypointList = networkTraffic.mRoadRoutes[aLaneIndex]->GetWaypoints();
   auto subsequentPointIt =
      std::find_if(waypointList.begin(),
                   waypointList.end(),
                   [aDistance](WsfWaypoint aWaypoint) { return aWaypoint.GetDistanceAlongRoute() >= aDistance; });
   unsigned int nextWaypointIndex =
      (subsequentPointIt != waypointList.end()) ? subsequentPointIt->GetPositionInRoute() : 0;
   unsigned int remainingSize = static_cast<int>(networkTraffic.mRoadRoutes[aLaneIndex]->GetSize());
   for (unsigned int i = nextWaypointIndex; i < remainingSize; ++i)
   {
      WsfWaypoint point = networkTraffic.mRoadRoutes[aLaneIndex]->GetWaypointAt(i);
      point.SetAlt(5.0);
      newRoute.Append(point);
      if (!point.GetNodeId().GetString().empty())
      {
         break;
      }
   }
   RoadTrafficNetworkInput& netInput =
      static_cast<RoadTrafficNetworkInput&>(*mNetworks[mTraffic[aVehicleNumber].mNetwork]);
   auto typeIt            = std::find_if(netInput.mVehicleInput.begin(),
                              netInput.mVehicleInput.end(),
                              [&](const VehicleInput& aInput)
                              { return aInput.mTypeId == mTraffic[aVehicleNumber].mRoadPlatform->GetTypeId(); });
   bool ignoreSpeedLimits = (typeIt != netInput.mVehicleInput.end()) ? typeIt->mIgnoreSpeedLimits : false;
   AssignRandomSpeeds(newRoute, netInput.mRouteAuxDataMap, mTraffic[aVehicleNumber].mSpeed, ignoreSpeedLimits);
   if (mTraffic[aVehicleNumber].mRoadPlatform->GetMover() && mTraffic[aVehicleNumber].mRoadPlatform->GetMover()->GetRoute())
   {
      networkTraffic.mRouteVehicleTracker[mTraffic[aVehicleNumber].mRoadPlatform->GetMover()->GetRoute()->GetName()].erase(
         aVehicleNumber);
      networkTraffic.mRouteVehicleTracker[newRoute.GetName()].emplace(aVehicleNumber);
   }
   if (mTraffic[aVehicleNumber].mRoadPlatform->GetMover())
   {
      mTraffic[aVehicleNumber].mRoadPlatform->GetMover()->UpdateRoute(GetSimulation().GetSimTime(), newRoute);
   }
   mTraffic[aVehicleNumber].mCurrentRouteName = newRoute.GetName();
}

// =================================================================================================
WsfRoute XWsfOSM_Traffic::CopyOriginalRoute(unsigned int aNetworkId, unsigned int aRouteIndex, unsigned int aWaypointIndex)
{
   TrafficInformation& networkTraffic = mRoadInfo[aNetworkId];
   WsfRoute            newRoute       = *networkTraffic.mRoadRoutes[aRouteIndex];
   newRoute.Clear();
   for (unsigned int waypointIndex = aWaypointIndex; waypointIndex < networkTraffic.mRoadRoutes[aRouteIndex]->GetSize();
        waypointIndex++)
   {
      WsfWaypoint nextPoint = networkTraffic.mRoadRoutes[aRouteIndex]->GetWaypointAt(waypointIndex);
      nextPoint.SetAlt(5);
      nextPoint.SetNodeId(networkTraffic.mRoadRoutes[aRouteIndex]->GetWaypointAt(waypointIndex).GetNodeId());
      newRoute.Append(nextPoint);
      if (waypointIndex != aWaypointIndex && !nextPoint.GetNodeId().GetString().empty())
      {
         break;
      }
   }
   return newRoute;
}

// =================================================================================================
void XWsfOSM_Traffic::HandleRegularOperations(double aSimTime)
{
   if (aSimTime >= mNextUpdateTime)
   {
      for (auto& network : mNetworks)
      {
         RoadTrafficNetworkInput* networkPtr     = static_cast<RoadTrafficNetworkInput*>(network.get());
         TrafficInformation&      networkTraffic = mRoadInfo[networkPtr->mNetIndex];
         // Create a more detailed, sorted vehicle tracker (to optimize collision checking a bit)
         std::unordered_map<std::string, std::vector<TrackerDetails>> detailedTracker;
         for (const auto& trackerIt : networkTraffic.mRouteVehicleTracker)
         {
            if (trackerIt.first != cOFFROAD_ID)
            {
               for (const auto& vehicleIt : trackerIt.second)
               {
                  WsfPlatform* currentPlatform = mTraffic[vehicleIt].mRoadPlatform;
                  double       currentLL[2];
                  double       temp;
                  currentPlatform->GetLocationLLA(currentLL[0], currentLL[1], temp);
                  if (!currentPlatform->GetMover() || !currentPlatform->GetMover()->GetRoute())
                  {
                     continue;
                  }
                  unsigned int routeId      = currentPlatform->GetMover()->GetRoute()->GetNetworkRouteId();
                  WsfRoute*    currentRoute = networkTraffic.mRoadRoutes[routeId].get();
                  double       beginLL[2];
                  currentRoute->GetWaypointAt(0).GetLocationLLA(beginLL[0], beginLL[1], temp);
                  // Note: updated routes sometimes come out negative? (it seems if you ignore sign the actual values
                  // are still correct)
                  double distance = std::abs(currentRoute->GetDistanceAlongRoute(beginLL, currentLL));
                  detailedTracker[trackerIt.first].emplace_back(TrackerDetails(vehicleIt, distance));
               }
               std::sort(detailedTracker[trackerIt.first].begin(), detailedTracker[trackerIt.first].end());
            }
         }

         // Process the vehicles
         for (const auto& trackerIt : detailedTracker)
         {
            int   routeAmount     = static_cast<int>(trackerIt.second.size());
            auto& vehiclesOnRoute = trackerIt.second;
            for (int i = routeAmount - 1; i >= 0; --i)
            {
               // Speeds are assigned on a timer instead of using initial waypoint values when no other vehicular
               // operation (e.g. lane change, slowdown, etc.) is performed. This is because in the process of
               // accelerating/decelerating, if the vehicle passes a waypoint prior to the acceleration finishing, it
               // will go back to the original unintended speed
               bool didNothing    = true;
               int  vehicleNumber = vehiclesOnRoute[i].mVehicleId;
               if (!mTraffic[vehicleNumber].mScripted)
               {
                  double            currentDistance = vehiclesOnRoute[i].mDistanceAlongRoute;
                  WsfPlatform*      currentPlatform = mTraffic[vehicleNumber].mRoadPlatform;
                  WsfWaypointMover* wMover          = dynamic_cast<WsfWaypointMover*>(currentPlatform->GetMover());
                  unsigned int      routeId         = currentPlatform->GetMover()->GetRoute()->GetNetworkRouteId();
                  WsfRoute*         currentRoute    = networkTraffic.mRoadRoutes[routeId].get();
                  if (currentDistance < 0 ||
                      currentDistance > 1.5 * currentRoute->GetDistance(0, currentRoute->GetSize() - 1))
                  {
                     didNothing = false;
                  }

                  didNothing = ApproachIntersectionDecelerate(aSimTime,
                                                              didNothing,
                                                              currentRoute,
                                                              wMover,
                                                              currentDistance,
                                                              vehiclesOnRoute,
                                                              i,
                                                              networkTraffic);
                  didNothing = HandleStoppedVehicles(aSimTime,
                                                     didNothing,
                                                     currentRoute,
                                                     wMover,
                                                     currentDistance,
                                                     vehiclesOnRoute,
                                                     i,
                                                     networkTraffic);
                  didNothing = HandleCollisions(aSimTime,
                                                didNothing,
                                                currentRoute,
                                                wMover,
                                                currentDistance,
                                                detailedTracker,
                                                vehiclesOnRoute,
                                                i,
                                                networkTraffic);
                  ModifyVehicleSpeed(aSimTime, didNothing, networkPtr, currentRoute, wMover, vehiclesOnRoute, i, networkTraffic);
               }
            }
         }
      }

      // Handle Normal Queued Operations: do these every two seconds instead of every second
      // if (std::fmod(aSimTime, 2.0) > 0.9)
      for (auto& network : mNetworks)
      {
         RoadTrafficNetworkInput* networkPtr     = static_cast<RoadTrafficNetworkInput*>(network.get());
         TrafficInformation&      networkTraffic = mRoadInfo[networkPtr->mNetIndex];
         for (auto& queueIt : networkTraffic.mQueuedOperations)
         {
            if (!queueIt.second.empty())
            {
               WeightedRouteNode routeNode = *networkTraffic.mRoadGraphs.find(WeightedRouteNode(queueIt.first.mNodeId));
               auto              intersectionIt = networkTraffic.mIntersectionsInUse.find(routeNode.mId);

               // Do not allow vehicles to go through stop signs immediately or to pass intersections when there is a
               // convoy reservation
               if (!(routeNode.mIntersection == IntersectionRestriction::cSTOP &&
                     (queueIt.second.front().mAddedTime + 1.0) >= aSimTime) &&
                   intersectionIt == networkTraffic.mIntersectionsInUse.end())
               {
                  auto sourceIt =
                     std::find(routeNode.mSourceRoutes.begin(), routeNode.mSourceRoutes.end(), queueIt.first.mSourceRoute);
                  int sourceIndex = static_cast<int>(std::distance(routeNode.mSourceRoutes.begin(), sourceIt));

                  // Queued operations due to edges of the map being reserved will not have stop signs/traffic lights
                  // and should be let through as soon as the intersection allows
                  double lowerRange = (routeNode.mIntersection != IntersectionRestriction::cNONE) ?
                                         routeNode.mWaitTime * sourceIndex :
                                         0.0;
                  double upperRange = (routeNode.mIntersection != IntersectionRestriction::cNONE) ?
                                         routeNode.mWaitTime * (sourceIndex + 1) :
                                         std::numeric_limits<double>::max();

                  double modTime = std::fmod(aSimTime, routeNode.mSourceRoutes.size() * routeNode.mWaitTime);
                  // Only allow 1 vehicle per destination route per second
                  if (modTime >= lowerRange && modTime <= upperRange)
                  {
                     QueuedOperation front = queueIt.second.front();
                     if (networkTraffic.mReservedRoads.find(front.mNewRoute.GetName()) ==
                         networkTraffic.mReservedRoads.end())
                     {
                        WsfPlatform*   vehiclePlatform = mTraffic[front.mVehicleId].mRoadPlatform;
                        WsfRouteMover* rMover          = dynamic_cast<WsfRouteMover*>(vehiclePlatform->GetMover());
                        std::string    oldRouteName    = rMover->GetRoute()->GetName();
                        rMover->GoToSpeed(aSimTime, front.mNewRoute.GetWaypointAt(0).GetSpeed(), 10, true);

                        // if start/end node is reserved by a convoy, there will be queued operations involving
                        // start/end, so teleport the vehicle in that case
                        auto        networkId = mTraffic[front.mVehicleId].mNetwork;
                        std::string firstNode = front.mNewRoute.GetWaypointAt(0).GetNodeId().GetString();
                        if (firstNode.find(mNetworks[networkId]->mRouteNetworkPtr->cSTART_NODE_ID) != std::string::npos ||
                            firstNode.find(mNetworks[networkId]->mRouteNetworkPtr->cSTOP_NODE_ID) != std::string::npos)
                        {
                           rMover->SetRoute(aSimTime, front.mNewRoute);
                        }

                        rMover->UpdateRoute(aSimTime, front.mNewRoute);
                        rMover->Unpause(aSimTime, true);
                        mTraffic[front.mVehicleId].mAtIntersection = false;

                        networkTraffic.mAtIntersection[oldRouteName].erase(front.mVehicleId);

                        networkTraffic.mRouteVehicleTracker[oldRouteName].erase(front.mVehicleId);
                        networkTraffic.mRouteVehicleTracker[front.mNewRoute.GetName()].emplace(front.mVehicleId);
                        mTraffic[front.mVehicleId].mCurrentRouteName = front.mNewRoute.GetName();

                        // TODO: Intersection in use by another background convoy, so things aren't properly processed
                        // (should we even be popping?)
                        queueIt.second.pop_front();
                        if (mTraffic[front.mVehicleId].mConvoyNumber > 0)
                        {
                           networkTraffic.mIntersectionsInUse.emplace(front.mNewRoute.GetWaypointAt(0).GetNodeId(),
                                                                      mTraffic[front.mVehicleId].mConvoyNumber);
                           double spacing = networkPtr->mConvoyInput[mTraffic[front.mVehicleId].mConvoyNumber - 1].mSpacing;
                           for (unsigned int j = 1; j < mConvoyIndices[mTraffic[front.mVehicleId].mConvoyNumber].size();
                                ++j)
                           {
                              networkTraffic.mQueuedConvoys[mTraffic[front.mVehicleId].mConvoyNumber].emplace_back(
                                 mConvoyIndices[mTraffic[front.mVehicleId].mConvoyNumber][j],
                                 front.mNewRoute,
                                 aSimTime + (j * spacing));
                           }
                           std::sort(networkTraffic.mQueuedConvoys[mTraffic[front.mVehicleId].mConvoyNumber].begin(),
                                     networkTraffic.mQueuedConvoys[mTraffic[front.mVehicleId].mConvoyNumber].end(),
                                     [](const QueuedOperation& aOp1, const QueuedOperation& aOp2)
                                     { return (aOp1.mAddedTime < aOp2.mAddedTime); });
                           mNextConvoyUpdateTime =
                              std::min(networkTraffic.mQueuedConvoys[mTraffic[front.mVehicleId].mConvoyNumber].front().mAddedTime,
                                       mNextConvoyUpdateTime);

                           networkTraffic.mIntersectionsInUse.emplace(routeNode.mId,
                                                                      mTraffic[front.mVehicleId].mConvoyNumber);
                           auto convoyIt = networkTraffic.mQueuedConvoys.find(mTraffic[front.mVehicleId].mConvoyNumber);
                           if (convoyIt != networkTraffic.mQueuedConvoys.end())
                           {
                              mNextConvoyUpdateTime =
                                 std::min(convoyIt->second.front().mAddedTime, std::max(aSimTime, mNextConvoyUpdateTime));
                           }
                        }
                     }
                  }
               }
            } // end queue loop
         }
      }
      mNextUpdateTime = mNextUpdateTime + 1.0;
   }
}

// =================================================================================================
void XWsfOSM_Traffic::HandleRoadRelease(double aSimTime)
{
   if (aSimTime >= mNextRoadReleaseTime)
   {
      double nextTime = std::numeric_limits<double>::max();
      for (auto& network : mNetworks)
      {
         RoadTrafficNetworkInput* networkPtr     = static_cast<RoadTrafficNetworkInput*>(network.get());
         TrafficInformation&      networkTraffic = mRoadInfo[networkPtr->mNetIndex];
         if (!networkTraffic.mQueuedRoadRelease.empty())
         {
            QueuedRoadRelease front = networkTraffic.mQueuedRoadRelease.front();
            if (front.mRoad)
            {
               networkTraffic.mReservedRoads.erase(front.mName);
               for (auto queueIt : networkTraffic.mQueuedOperations)
               {
                  // update the queue times for vehicles waiting to make a u-turn
                  if (queueIt.first.mDestinationRoute == front.mName)
                  {
                     for (auto& individualIt : queueIt.second)
                     {
                        individualIt.mAddedTime = aSimTime;
                     }
                  }
               }
            }
            else
            {
               networkTraffic.mIntersectionsInUse.erase(front.mName);
            }

            networkTraffic.mQueuedRoadRelease.pop_front();
            if (!networkTraffic.mQueuedRoadRelease.empty())
            {
               nextTime = std::min(nextTime, networkTraffic.mQueuedRoadRelease.front().mTime);
            }
         }
      }
      mNextRoadReleaseTime = nextTime;
   }
}

// =================================================================================================
void XWsfOSM_Traffic::HandleQueuedConvoy(double aSimTime)
{
   // Handle Queued Background Convoy Operations
   if (aSimTime >= mNextConvoyUpdateTime)
   {
      double minTime = std::numeric_limits<double>::max();
      for (auto& network : mNetworks)
      {
         RoadTrafficNetworkInput* networkPtr     = static_cast<RoadTrafficNetworkInput*>(network.get());
         TrafficInformation&      networkTraffic = mRoadInfo[networkPtr->mNetIndex];
         // Handle Convoy Queued Operations
         for (auto& convoyIt : networkTraffic.mQueuedConvoys)
         {
            if (!convoyIt.second.empty())
            {
               QueuedOperation front = convoyIt.second.front();
               if (aSimTime >= front.mAddedTime)
               {
                  WsfRoute           newRoute        = front.mNewRoute;
                  WsfPlatform*       vehiclePlatform = mTraffic[front.mVehicleId].mRoadPlatform;
                  WsfRouteMover*     rMover          = dynamic_cast<WsfRouteMover*>(vehiclePlatform->GetMover());
                  const WsfRoute*    oldRoute        = rMover->GetRoute();
                  const std::string& oldRouteName    = oldRoute->GetName();
                  std::string        firstNode       = newRoute.GetWaypointAt(0).GetNodeId();
                  auto               networkId       = mTraffic[front.mVehicleId].mNetwork;
                  if (firstNode.find(mNetworks[networkId]->mRouteNetworkPtr->cSTART_NODE_ID) != std::string::npos ||
                      firstNode.find(mNetworks[networkId]->mRouteNetworkPtr->cSTOP_NODE_ID) != std::string::npos)
                  {
                     rMover->SetRoute(aSimTime, newRoute);
                  }
                  rMover->UpdateRoute(aSimTime, newRoute);
                  rMover->Unpause(aSimTime, true);
                  networkTraffic.mRouteVehicleTracker[oldRouteName].erase(front.mVehicleId);
                  networkTraffic.mRouteVehicleTracker[newRoute.GetName()].emplace(front.mVehicleId);
                  mTraffic[front.mVehicleId].mCurrentRouteName = newRoute.GetName();
                  networkTraffic.mAtIntersection[oldRouteName].erase(front.mVehicleId);

                  convoyIt.second.pop_front();
                  // Technically an intersection can appear in the queued convoys for multiple times if a u-turn occurs,
                  // so we should only check until the last vehicle of the convoy is reached
                  if (front.mVehicleId == mConvoyIndices[convoyIt.first][mConvoyIndices[convoyIt.first].size() - 1])
                  {
                     // un-reserve the intersection
                     auto intersectionIt =
                        std::find_if(networkTraffic.mIntersectionsInUse.begin(),
                                     networkTraffic.mIntersectionsInUse.end(),
                                     [convoyIt](const std::pair<std::string, unsigned int> aPair)
                                     { return aPair.second == static_cast<unsigned int>(convoyIt.first); });
                     if (intersectionIt != networkTraffic.mIntersectionsInUse.end())
                     {
                        networkTraffic.mIntersectionsInUse.erase(intersectionIt);
                     }
                  }
               }
            }
            if (!convoyIt.second.empty() && convoyIt.second.front().mAddedTime <= minTime)
            {
               minTime = convoyIt.second.front().mAddedTime;
            }
         }
      }
      mNextConvoyUpdateTime = minTime;
   }
}

// =================================================================================================
void XWsfOSM_Traffic::HandleConvoySpeedChange(double aSimTime)
{
   // Handle Background Convoy Speed Changes
   // There is sometimes a small delay when processing things (e.g. slow down at T = 95 but the actual time is T
   // = 95.002), so allow for a small cushion
   if (aSimTime >= mNextConvoySpeedTime)
   {
      double minTime = std::numeric_limits<double>::max();
      for (auto& network : mNetworks)
      {
         RoadTrafficNetworkInput* networkPtr     = static_cast<RoadTrafficNetworkInput*>(network.get());
         TrafficInformation&      networkTraffic = mRoadInfo[networkPtr->mNetIndex];
         for (auto& convoyIt : networkTraffic.mQueuedConvoySpeeds)
         {
            if (!convoyIt.second.empty())
            {
               QueuedSpeedChange speedChange = convoyIt.second.front();
               // potentially need to process multiple commands
               while (aSimTime >= speedChange.mTime)
               {
                  WsfWaypointMover* wMover =
                     dynamic_cast<WsfWaypointMover*>(mTraffic[speedChange.mVehicleId].mRoadPlatform->GetMover());
                  if (wMover)
                  {
                     wMover->GoToSpeed(aSimTime, speedChange.mSpeed, 10000, true);
                  }
                  convoyIt.second.pop_front();
                  speedChange = (!convoyIt.second.empty()) ?
                                   convoyIt.second.front() :
                                   QueuedSpeedChange(-1, 0, 0, std::numeric_limits<double>::max());
               }
            }
            if (!convoyIt.second.empty() && convoyIt.second.front().mTime <= minTime)
            {
               minTime = convoyIt.second.front().mTime;
            }
         }
      }
      mNextConvoySpeedTime = minTime;
   }
}

// =================================================================================================
// Helper function to handle speed changes that occur as a result of other vehicles traveling along the same route
void XWsfOSM_Traffic::GoToSpeedHelper(double              aSimTime,
                                      WsfMover*           aMover,
                                      double              aSpeed,
                                      double              aDifference,
                                      int                 aVehicleNumber,
                                      TrafficInformation& aNetworkTraffic)
{
   // When too close to another vehicle, decelerate (by 25%?) to increase the size of the cushion between vehicles
   // the other waypoints should still possess random speeds (should not remain the same speed, perhaps try to speed
   // up again at another checkpoint)
   double            newSpeed = (aDifference < aSpeed) ? (aSpeed * 0.75) : aSpeed;
   WsfWaypointMover* wMover   = dynamic_cast<WsfWaypointMover*>(aMover);
   double            temp;
   if (!aMover->GetRoute())
   {
      return;
   }
   // Get the subsequent waypoint that occurs after the initial location
   double beginLL[2] = {aMover->GetRoute()->GetWaypointAt(0).GetLat(), aMover->GetRoute()->GetWaypointAt(0).GetLon()};
   double currentLL[2];
   aMover->GetPlatform()->GetLocationLLA(currentLL[0], currentLL[1], temp);
   double       distanceAlong     = aMover->GetRoute()->GetDistanceAlongRoute(beginLL, currentLL);
   auto&        waypointList      = aMover->GetRoute()->GetWaypoints();
   auto         subsequentPointIt = std::find_if(waypointList.begin(),
                                         waypointList.end(),
                                         [distanceAlong](WsfWaypoint aWaypoint)
                                         { return aWaypoint.GetDistanceAlongRoute() >= distanceAlong; });
   unsigned int nextWaypointIndex =
      (subsequentPointIt != waypointList.end()) ? subsequentPointIt->GetPositionInRoute() : 0;
   if (wMover)
   {
      wMover->GoToSpeed(aSimTime, newSpeed, 100000, true);
   }
   // Background convoys are not fully supported, but may be re-examined in the future
   // update all of the subsequent convoy vehicles
   int convoyNumber = mTraffic[aVehicleNumber].mConvoyNumber;
   if (convoyNumber > 0)
   {
      RoadTrafficNetworkInput* networkPtr =
         static_cast<RoadTrafficNetworkInput*>(mNetworks[mTraffic[aVehicleNumber].mNetwork].get());
      double spacing    = networkPtr->mConvoyInput[convoyNumber - 1].mSpacing;
      int    multiplier = 1;
      for (unsigned int i = 1; i < mConvoyIndices[convoyNumber].size(); ++i)
      {
         double time = round((aSimTime + (spacing * multiplier++)) * 10) / 10;
         aNetworkTraffic.mQueuedConvoySpeeds[convoyNumber].emplace_back(mConvoyIndices[convoyNumber][i],
                                                                        nextWaypointIndex,
                                                                        newSpeed,
                                                                        time);
      }
      // Sometimes a lead convoy vehicle may change speed at T = X, then change speeds again at T = X + 1. In a long
      // convoy, this means that the queue may become out of order, so it needs to be sorted by execution time
      std::sort(aNetworkTraffic.mQueuedConvoySpeeds[convoyNumber].begin(),
                aNetworkTraffic.mQueuedConvoySpeeds[convoyNumber].end(),
                [](const QueuedSpeedChange& aOp1, const QueuedSpeedChange& aOp2) { return (aOp1.mTime < aOp2.mTime); });
      mNextConvoySpeedTime = std::min(mNextConvoySpeedTime, aSimTime + spacing);
   }
}

// =================================================================================================
bool XWsfOSM_Traffic::ApproachIntersectionDecelerate(double                             aSimTime,
                                                     bool                               aDecelerate,
                                                     const WsfRoute*                    aCurrentRoute,
                                                     WsfMover*                          aMover,
                                                     double                             aCurrentDistance,
                                                     const std::vector<TrackerDetails>& aVehiclesOnRoute,
                                                     int                                aIndex,
                                                     TrafficInformation&                aNetworkTraffic)
{
   // Decelerate vehicles approaching the intersection (including the furthest vehicle on the route)
   int               vehicleNumber    = aVehiclesOnRoute[aIndex].mVehicleId;
   int               routeAmount      = static_cast<int>(aVehiclesOnRoute.size());
   WsfWaypointMover* wMover           = dynamic_cast<WsfWaypointMover*>(aMover);
   bool              didNothing       = aDecelerate;
   double            distanceToFinish = aCurrentRoute->GetDistance(0, aCurrentRoute->GetSize() - 1) - aCurrentDistance;
   double            currentSpeed     = mTraffic[vehicleNumber].mRoadPlatform->GetSpeed();
   auto              atIntersectionIt = aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].find(vehicleNumber);
   std::string       lastNode = aCurrentRoute->GetWaypointAt(aCurrentRoute->GetSize() - 1).GetNodeId().GetString();
   std::string       baseName = "";
   WsfUtil::GetAuxValue(aCurrentRoute, cBASE_ROUTE_AUX_NAME, baseName);
   TrackerDetails aheadVehicle = (aIndex + 1 < routeAmount) ? aVehiclesOnRoute[aIndex + 1] : TrackerDetails(-1, -1);
   auto           aheadIt = aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].find(aheadVehicle.mVehicleId);
   if (aheadIt != aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].end())
   {
      distanceToFinish = aheadIt->second - aCurrentDistance - cSPACING;
   }
   if (didNothing && currentSpeed > 0.0 && distanceToFinish <= (2 * currentSpeed))
   {
      auto nodeIt    = aNetworkTraffic.mRoadGraphs.find(lastNode);
      auto networkId = mTraffic[vehicleNumber].mNetwork;
      if (nodeIt != aNetworkTraffic.mRoadGraphs.end() && nodeIt->mIntersection == IntersectionRestriction::cTRAFFIC)
      {
         auto sourceIt = std::find(nodeIt->mSourceRoutes.begin(), nodeIt->mSourceRoutes.end(), baseName);
         if (sourceIt != nodeIt->mSourceRoutes.end())
         {
            unsigned int sourceIndex = static_cast<unsigned int>(sourceIt - nodeIt->mSourceRoutes.begin());
            double       modTime     = std::fmod(aSimTime, nodeIt->mSourceRoutes.size() * nodeIt->mWaitTime);
            // offset the bounds so that the vehicle can decelerate if the light is currently green,
            // but will be red shortly later (or if it's red and will become green soon)
            double lowerBound = (sourceIndex * nodeIt->mWaitTime) - 1;
            double upperBound = ((sourceIndex + 1) * nodeIt->mWaitTime) - 1;
            if ((modTime < lowerBound || modTime > upperBound) ||
                aheadIt != aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].end())
            {
               double deceleration = std::abs((-1 * std::pow(currentSpeed, 2)) / (2 * (distanceToFinish)));
               wMover->GoToSpeed(aSimTime, 0, deceleration, true);
               std::string name = wMover->GetPlatform()->GetName();
               if (atIntersectionIt == aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].end())
               {
                  aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].emplace(vehicleNumber,
                                                                                    aCurrentDistance + distanceToFinish);
               }
               else
               {
                  aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()][vehicleNumber] =
                     aCurrentDistance + distanceToFinish;
               }
               didNothing = false;
            }
         }
      }
      else if (nodeIt != aNetworkTraffic.mRoadGraphs.end() && nodeIt->mIntersection == IntersectionRestriction::cSTOP)
      {
         double deceleration = std::abs((-1 * std::pow(currentSpeed, 2)) / (2 * (distanceToFinish)));
         wMover->GoToSpeed(aSimTime, 0, deceleration, true);
         if (atIntersectionIt == aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].end())
         {
            aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].emplace(vehicleNumber,
                                                                              aCurrentDistance + distanceToFinish);
         }
         else
         {
            aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()][vehicleNumber] = aCurrentDistance + distanceToFinish;
         }
         didNothing = false;
      }
      // if heading into the edge of the map, decelerate (but not to 0) and eventually attempt to make a u-turn
      // when the end of the route is reached
      else if ((nodeIt != aNetworkTraffic.mRoadGraphs.end()) &&
               ((nodeIt->mId.find(mNetworks[networkId]->mRouteNetworkPtr->cSTART_NODE_ID) != std::string::npos) ||
                (nodeIt->mId.find(mNetworks[networkId]->mRouteNetworkPtr->cSTOP_NODE_ID) != std::string::npos)) &&
               (nodeIt->mIntersection == IntersectionRestriction::cNONE))
      {
         // This is a deceleration into an attempted u-turn, so don't actually reach 0 when decelerating
         double deceleration = std::abs((-1 * std::pow(currentSpeed, 2)) / (3 * distanceToFinish));
         wMover->GoToSpeed(aSimTime, 0, deceleration, true);
         didNothing = false;
      }
   }
   return didNothing;
}

// =================================================================================================
bool XWsfOSM_Traffic::HandleStoppedVehicles(double                             aSimTime,
                                            bool                               aHandleStop,
                                            const WsfRoute*                    aCurrentRoute,
                                            WsfMover*                          aMover,
                                            double                             aCurrentDistance,
                                            const std::vector<TrackerDetails>& aVehiclesOnRoute,
                                            int                                aIndex,
                                            TrafficInformation&                aNetworkTraffic)
{
   // Handle vehicles that have stopped (or are very slow) in the middle of the road (pausing is reserved for
   // intersections) This should only happen when other vehicles need to wait for convoys to fully initialize (since
   // convoys start stacked and un-pause periodically) Also handles vehicles waiting at intersections Due to spaced out
   // intersections, sometimes a low speed and high speed going to the same destination at different times may end up
   // colliding
   bool              didNothing      = aHandleStop;
   WsfPlatform*      currentPlatform = aMover->GetPlatform();
   int               routeAmount     = static_cast<int>(aVehiclesOnRoute.size());
   int               vehicleNumber   = aVehiclesOnRoute[aIndex].mVehicleId;
   WsfWaypointMover* wMover          = dynamic_cast<WsfWaypointMover*>(aMover);
   if (didNothing && currentPlatform->GetSpeed() < cSPACING && !currentPlatform->GetMover()->IsPaused())
   {
      if (aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].find(vehicleNumber) !=
          aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].end())
      {
         if (aIndex + 1 < routeAmount)
         {
            TrackerDetails vehicleAhead = aVehiclesOnRoute[aIndex + 1];
            if (vehicleAhead.mDistanceAlongRoute - (cSPACING + 1) > aCurrentDistance)
            {
               wMover->GoToSpeed(aSimTime, cSPACING - 0.1, 10, true);
               aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()][vehicleNumber] = aCurrentDistance + cSPACING - 0.1;
               didNothing                                                               = false;
            }
         }
         else
         {
            wMover->GoToSpeed(aSimTime, 10, 10, true);
            aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()][vehicleNumber] =
               aCurrentRoute->GetDistance(0, aCurrentRoute->GetSize() - 1);
            didNothing = false;
         }
      }
      else
      {
         if (aIndex + 1 < routeAmount)
         {
            TrackerDetails vehicleAhead = aVehiclesOnRoute[aIndex + 1];
            double         difference   = vehicleAhead.mDistanceAlongRoute - aCurrentDistance;
            if (difference > 0)
            {
               double aheadSpeed = mTraffic[vehicleAhead.mVehicleId].mRoadPlatform->GetSpeed();
               if (difference > aheadSpeed)
               {
                  wMover->GoToSpeed(aSimTime, aheadSpeed, 10, true);
                  didNothing = false;
               }
            }
         }
         else
         {
            wMover->GoToSpeed(aSimTime, 20, 10, true);
            didNothing = false;
         }
      }
   }
   return didNothing;
}

// =================================================================================================
bool XWsfOSM_Traffic::HandleCollisions(double    aSimTime,
                                       bool      aHandleCollisions,
                                       WsfRoute* aCurrentRoute,
                                       WsfMover* aMover,
                                       double    aCurrentDistance,
                                       const std::unordered_map<std::string, std::vector<TrackerDetails>>& aDetailedTracker,
                                       const std::vector<TrackerDetails>& aVehiclesOnRoute,
                                       int                                aIndex,
                                       TrafficInformation&                aNetworkTraffic)
{
   bool         didNothing      = aHandleCollisions;
   size_t       routeAmount     = aVehiclesOnRoute.size();
   int          vehicleNumber   = aVehiclesOnRoute[aIndex].mVehicleId;
   WsfPlatform* currentPlatform = aMover->GetPlatform();
   if (static_cast<size_t>(aIndex + 1) < routeAmount &&
       (aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].find(vehicleNumber) ==
        aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].end()) &&
       didNothing)
   {
      TrackerDetails vehicleAhead = aVehiclesOnRoute[aIndex + 1];
      // Handle Normal Speed And Lane Changes
      if (!(mTraffic[vehicleNumber].mConvoyNumber > 0 &&
            vehicleNumber != mConvoyIndices[mTraffic[vehicleNumber].mConvoyNumber][0])) // && test)
      {
         // Override the random decision if the current vehicle is stuck between two convoy vehicles (attempt to change lanes)
         if (aIndex - 1 >= 0)
         {
            int behindConvoyNumber = mTraffic[aVehiclesOnRoute[aIndex - 1].mVehicleId].mConvoyNumber;
            int aheadConvoyNumber  = mTraffic[vehicleAhead.mVehicleId].mConvoyNumber;
            if (behindConvoyNumber > 0 && behindConvoyNumber == aheadConvoyNumber)
            {
               didNothing = ChangeScriptedLane(aMover->GetPlatform(), true);
               if (didNothing)
               {
                  didNothing = ChangeScriptedLane(aMover->GetPlatform(), false);
               }
            }
         }

         double otherDistance = vehicleAhead.mDistanceAlongRoute;
         double difference    = otherDistance - aCurrentDistance;

         // Due to how movers/platforms work, sometimes the "DistanceAlongRoute" will not be entirely accurate at the
         // beginning, particularly if an Update or SetRoute is called, it is better not to perform any operations near
         // the beginning of the route
         const double cBEGINNING_CUSHION = 1.0;
         bool         beginning = (aCurrentDistance <= cBEGINNING_CUSHION) && (otherDistance <= cBEGINNING_CUSHION);

         // Need to ignore "collision checks" when close to nodes, not just at the beginning/end of routes, since nodes
         // can occur in the middle of a road
         WsfRoute::WaypointPtrVector nodeVector;
         aCurrentRoute->GetAllNodes(nodeVector);
         auto nodeCheckIt = std::find_if(
            nodeVector.begin(),
            nodeVector.end(),
            [aCurrentRoute, aCurrentDistance, currentPlatform](const WsfWaypoint* aWaypoint)
            {
               double beginLL[2] = {aCurrentRoute->GetWaypointAt(0).GetLat(), aCurrentRoute->GetWaypointAt(0).GetLon()};
               double nodeLL[2]  = {aWaypoint->GetLat(), aWaypoint->GetLon()};
               double nodeDistance = aCurrentRoute->GetDistanceAlongRoute(beginLL, nodeLL);
               return (std::abs(nodeDistance - aCurrentDistance) <
                       (1.5 * currentPlatform->GetSpeed())); //  20.0); //(1.5 * currentPlatform->GetSpeed()));
            });
         bool nearNode = (nodeCheckIt != nodeVector.end());

         // Do not do anything when approaching intersections (defined as the area between the last waypoint and 2nd
         // last waypoint of the route
         if (didNothing && !beginning && !nearNode && difference > 0 && difference < 1.5 * currentPlatform->GetSpeed())
         {
            // Attempt a lane change 60% of the time, slow down 40% of the time: for simplicity convoys should not be
            // allowed to change lanes
            int decision = GetSimulation().GetRandom().Uniform(0, 10);
            // Override the random decision if the current vehicle is stuck between two convoy vehicles (attempt to change lanes)
            if (aIndex - 1 >= 0)
            {
               int behindConvoyNumber = mTraffic[aVehiclesOnRoute[aIndex - 1].mVehicleId].mConvoyNumber;
               int aheadConvoyNumber  = mTraffic[vehicleAhead.mVehicleId].mConvoyNumber;
               if (behindConvoyNumber > 0 && behindConvoyNumber == aheadConvoyNumber)
               {
                  decision = 0;
               }
            }
            const int cPERCENT = 6;
            if (!(mTraffic[vehicleNumber].mConvoyNumber > 0) && decision < cPERCENT)
            {
               auto laneListIt = aNetworkTraffic.mAdjacentLanes.find(aCurrentRoute->GetName());
               if (laneListIt != aNetworkTraffic.mAdjacentLanes.end())
               {
                  for (const auto& routeIndexIt : laneListIt->second)
                  {
                     double      nextPoint[2];
                     std::string laneRouteName = aNetworkTraffic.mRoadRoutes[routeIndexIt]->GetName();
                     auto        tracker =
                        std::find_if(aDetailedTracker.begin(),
                                     aDetailedTracker.end(),
                                     [laneRouteName](const std::pair<std::string, std::vector<TrackerDetails>>& aPair)
                                     { return aPair.first == laneRouteName; });
                     // Lanes that have never had vehicles on them are omitted from the detailed tracker, but they still
                     // exist and can be switched to
                     std::vector<TrackerDetails> trackerDetails =
                        (tracker != aDetailedTracker.end()) ? tracker->second : std::vector<TrackerDetails>();
                     if (CheckLaneAvailability(vehicleNumber, routeIndexIt, trackerDetails, aCurrentDistance + 10.0, nextPoint))
                     {
                        ChangeLane(vehicleNumber, routeIndexIt, aCurrentDistance + 10.0, nextPoint);
                        didNothing = false;
                        break;
                     }
                  }
               }
               if (didNothing)
               {
                  GoToSpeedHelper(aSimTime,
                                  currentPlatform->GetMover(),
                                  mTraffic[vehicleAhead.mVehicleId].mRoadPlatform->GetSpeed(),
                                  difference,
                                  vehicleNumber,
                                  aNetworkTraffic);
                  didNothing = false;
               }
            }
            else
            {
               GoToSpeedHelper(aSimTime,
                               currentPlatform->GetMover(),
                               mTraffic[vehicleAhead.mVehicleId].mRoadPlatform->GetSpeed(),
                               difference,
                               vehicleNumber,
                               aNetworkTraffic);
               didNothing = false;
            }
         }
      }
   }
   return didNothing;
}

// =================================================================================================
void XWsfOSM_Traffic::ModifyVehicleSpeed(double                             aSimTime,
                                         bool                               aModifySpeed,
                                         RoadTrafficNetworkInput*           aNetworkPtr,
                                         WsfRoute*                          aCurrentRoute,
                                         WsfMover*                          aMover,
                                         const std::vector<TrackerDetails>& aVehiclesOnRoute,
                                         int                                aIndex,
                                         TrafficInformation&                aNetworkTraffic)
{
   bool              didNothing      = aModifySpeed;
   int               vehicleNumber   = aVehiclesOnRoute[aIndex].mVehicleId;
   size_t            routeAmount     = aVehiclesOnRoute.size();
   WsfWaypointMover* wMover          = dynamic_cast<WsfWaypointMover*>(aMover);
   WsfPlatform*      currentPlatform = aMover->GetPlatform();
   if (didNothing &&
       (aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].find(vehicleNumber) ==
        aNetworkTraffic.mAtIntersection[aCurrentRoute->GetName()].end()) &&
       !mTraffic[vehicleNumber].mRoadPlatform->IsPaused())
   {
      // Do not modify speeds of non-lead convoy vehicles
      if ((mTraffic[vehicleNumber].mConvoyNumber) < 0 ||
          (mConvoyIndices[mTraffic[vehicleNumber].mConvoyNumber][0] == vehicleNumber))
      {
         double distance =
            aCurrentRoute->GetDistance(0, aCurrentRoute->GetSize() - 1) - aVehiclesOnRoute[aIndex].mDistanceAlongRoute;
         if (static_cast<size_t>(aIndex + 1) < routeAmount)
         {
            TrackerDetails vehicleAhead = aVehiclesOnRoute[aIndex + 1];
            double         currentSpeed = 0.0;
            if (mTraffic[aVehiclesOnRoute[aIndex].mVehicleId].mRoadPlatform->GetMover() &&
                mTraffic[aVehiclesOnRoute[aIndex].mVehicleId].mRoadPlatform->GetMover()->GetRoute())
            {
               currentSpeed =
                  mTraffic[aVehiclesOnRoute[aIndex].mVehicleId].mRoadPlatform->GetMover()->GetRoute()->GetWaypointAt(0).GetSpeed();
               distance = vehicleAhead.mDistanceAlongRoute - aVehiclesOnRoute[aIndex].mDistanceAlongRoute;
            }

            if (aVehiclesOnRoute[aIndex].mDistanceAlongRoute + (currentSpeed * 2.0) >= vehicleAhead.mDistanceAlongRoute)
            {
               didNothing = false;
            }
         }
         if (didNothing)
         {
            RoadTrafficNetworkInput& netInput =
               static_cast<RoadTrafficNetworkInput&>(*mNetworks[mTraffic[vehicleNumber].mNetwork]);
            auto typeIt            = std::find_if(aNetworkPtr->mVehicleInput.begin(),
                                       aNetworkPtr->mVehicleInput.end(),
                                       [currentPlatform](const VehicleInput& aInput)
                                       { return aInput.mTypeId == currentPlatform->GetTypeId(); });
            bool ignoreSpeedLimits = (typeIt != aNetworkPtr->mVehicleInput.end()) ? typeIt->mIgnoreSpeedLimits : false;
            double newSpeed =
               GetRandomSpeed(*aCurrentRoute, netInput.mRouteAuxDataMap, mTraffic[vehicleNumber].mSpeed, ignoreSpeedLimits);
            GoToSpeedHelper(aSimTime, wMover, newSpeed, distance, vehicleNumber, aNetworkTraffic);
         }
      }
   }
}

// =================================================================================================
void XWsfOSM_Traffic::CatchUpHandler(double aSimTime, int aVehicleNumber, double aCatchupSpeed)
{
   int  convoyNumber = mTraffic[aVehicleNumber].mConvoyNumber;
   auto vehicleIt = std::find(mConvoyIndices[convoyNumber].begin(), mConvoyIndices[convoyNumber].end(), aVehicleNumber);
   if (vehicleIt != mConvoyIndices[convoyNumber].begin() && vehicleIt != mConvoyIndices[convoyNumber].end())
   {
      // TODO: Does there need to be acceleration/deceleration to a new speed when re-spacing convoys
      auto        aheadVehicleIt   = vehicleIt - 1;
      std::string currentRouteName = mTraffic[*vehicleIt].mCurrentRouteName;
      if (currentRouteName == mTraffic[*aheadVehicleIt].mCurrentRouteName)
      {
         RoadTrafficNetworkInput* networkPtr =
            static_cast<RoadTrafficNetworkInput*>(mNetworks[mTraffic[*vehicleIt].mNetwork].get());
         TrafficInformation& networkTraffic = mRoadInfo[mTraffic[*vehicleIt].mNetwork];
         auto                routeIt        = std::find_if(networkTraffic.mRoadRoutes.begin(),
                                     networkTraffic.mRoadRoutes.end(),
                                     [currentRouteName](std::unique_ptr<WsfRoute>& aRoad)
                                     { return aRoad->GetName() == currentRouteName; });
         if (routeIt != networkTraffic.mRoadRoutes.end())
         {
            double temp;
            double beginLL[2];
            double currentLL[2];
            routeIt->get()->GetWaypointAt(0).GetLocationLLA(beginLL[0], beginLL[1], temp);
            mTraffic[*aheadVehicleIt].mRoadPlatform->GetLocationLLA(currentLL[0], currentLL[1], temp);
            double aheadDistance = routeIt->get()->GetDistanceAlongRoute(beginLL, currentLL);
            mTraffic[*vehicleIt].mRoadPlatform->GetLocationLLA(currentLL[0], currentLL[1], temp);
            double currentDistance = routeIt->get()->GetDistanceAlongRoute(beginLL, currentLL);
            double timeSpacing     = networkPtr->mConvoyInput[convoyNumber - 1].mSpacing;
            double aheadSpeed      = mTraffic[*aheadVehicleIt].mRoadPlatform->GetSpeed();
            // Formula for calculating the time that the behind vehicle should change speed to the vehicle its catching
            // up to, assuming both vehicles do not change speed for the duration. If the "ahead" vehicle does change
            // its speed, the timeOfCatchUp needs to be recalculated and the Queued change will need to be overwritten
            // (handled elsewhere)
            // aheadDistance + (aheadSpeed * t) = currentDistance + (catchupSpeed * t) + (aheadSpeed * spacing)
            double timeOfCatchUp =
               (aheadDistance - currentDistance - (aheadSpeed * timeSpacing)) / (aCatchupSpeed - aheadSpeed);
            mQueuedCatchUp.emplace_back(aVehicleNumber, 0, aheadSpeed, aSimTime + timeOfCatchUp);
            std::sort(mQueuedCatchUp.begin(),
                      mQueuedCatchUp.end(),
                      [](const QueuedSpeedChange& aOp1, const QueuedSpeedChange& aOp2)
                      { return (aOp1.mTime < aOp2.mTime); });
            mNextConvoyCatchUpTime = mQueuedCatchUp.front().mTime;
         }
      }
   }
}


// Script Methods

// =================================================================================================
WsfScriptOSM_TrafficClass::WsfScriptOSM_TrafficClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfOSM_Traffic");

   // Add each of the method objects to the class.
   AddStaticMethod(ut::make_unique<Traffic>());
   AddMethod(ut::make_unique<TrafficRoutes>());
   AddMethod(ut::make_unique<IndividualRoute>());
   AddMethod(ut::make_unique<VehiclesAlongRoute>());
   AddMethod(ut::make_unique<NextPointInRoute>());
   AddMethod(ut::make_unique<IntersectionFlow>());
   AddMethod(ut::make_unique<IntersectionOnRoute>());
   AddMethod(ut::make_unique<GetConnectedRoads>());
   AddMethod(ut::make_unique<AddVehicleToTraffic>());
   AddMethod(ut::make_unique<FollowRoute_1>("FollowRoute"));
   AddMethod(ut::make_unique<FollowRoute_2>("FollowRoute"));
   AddMethod(ut::make_unique<ChangeLane>());
   AddMethod(ut::make_unique<AddConvoyToTraffic>());
   AddMethod(ut::make_unique<LockIntersection>());
   AddMethod(ut::make_unique<UnlockIntersection>());
   AddMethod(ut::make_unique<GoOffroad>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, Traffic, 0, "WsfOSM_Traffic", "")
{
   XWsfOSM_Traffic* trafficExtension = XWsfOSM_Traffic::Find(*WsfScriptContext::GetSIMULATION(aContext));
   aReturnVal.SetPointer(new UtScriptRef(trafficExtension, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, TrafficRoutes, 1, "Array<WsfRoute>", "string")
{
   std::string networkName  = aVarArgs[0].GetString();
   auto        routes       = aObjectPtr->GetRoutesOnNetwork(networkName);
   auto        routeListPtr = ut::make_unique<UtScriptDataList>();
   for (auto routePtr : routes)
   {
      routeListPtr->emplace_back(routePtr->Clone()->ScriptRefManage());
   }
   aReturnVal.SetPointer(new UtScriptRef(routeListPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, IndividualRoute, 5, "WsfRoute", "string, string, int, bool, int")
{
   std::string networkName   = aVarArgs[0].GetString();
   std::string baseRouteName = aVarArgs[1].GetString();
   int         segmentNumber = aVarArgs[2].GetInt();
   bool        direction     = aVarArgs[3].GetBool();
   int         laneNumber    = aVarArgs[4].GetInt();
   aReturnVal.SetPointer(
      new UtScriptRef(aObjectPtr->GetIndividualRoute(networkName, baseRouteName, laneNumber, direction, segmentNumber),
                      aReturnClassPtr,
                      UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, VehiclesAlongRoute, 2, "WsfPlatformList", "string, string")
{
   std::string                networkName     = aVarArgs[0].GetString();
   std::string                routeName       = aVarArgs[1].GetString();
   std::vector<WsfPlatform*>* platformListPtr = new std::vector<WsfPlatform*>();
   std::vector<WsfPlatform*>  platformList    = aObjectPtr->GetPlatformsAlongRoute(networkName, routeName);
   for (const auto& it : platformList)
   {
      platformListPtr->emplace_back(it);
   }
   aReturnVal.SetPointer(new UtScriptRef(platformListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, NextPointInRoute, 3, "WsfWaypoint", "string, string, double")
{
   std::string networkName = aVarArgs[0].GetString();
   std::string routeName   = aVarArgs[1].GetString();
   double      distance    = aVarArgs[2].GetDouble();
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetNextWaypoint(networkName, routeName, distance),
                                         aReturnClassPtr,
                                         UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, IntersectionFlow, 3, "string", "double, string, string")
{
   double      selectedTime = aVarArgs[0].GetDouble();
   std::string networkName  = aVarArgs[1].GetString();
   std::string nodeId       = aVarArgs[2].GetString();
   aReturnVal.SetString(aObjectPtr->GetTrafficFlow(selectedTime, networkName, nodeId));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, IntersectionOnRoute, 3, "string", "string, string, bool")
{
   std::string networkName = aVarArgs[0].GetString();
   std::string routeName   = aVarArgs[1].GetString();
   bool        firstLast   = aVarArgs[2].GetBool();
   aReturnVal.SetString(aObjectPtr->GetNodeOnRoute(networkName, routeName, firstLast));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, GetConnectedRoads, 2, "Array<WsfRoute>", "string, string")
{
   std::string            networkName  = aVarArgs[0].GetString();
   std::string            nodeId       = aVarArgs[1].GetString();
   std::vector<WsfRoute*> routes       = aObjectPtr->GetConnectedRoutes(networkName, nodeId);
   UtScriptDataList*      routeListPtr = new UtScriptDataList;
   for (auto& route : routes)
   {
      routeListPtr->push_back(UtScriptData(route->Clone()->ConstScriptRef()));
   }
   aReturnVal.SetPointer(new UtScriptRef(routeListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, AddVehicleToTraffic, 3, "bool", "WsfPlatform, string, WsfRoute")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string  networkName = aVarArgs[1].GetString();
   WsfRoute*    route       = static_cast<WsfRoute*>(aVarArgs[2].GetPointer()->GetAppObject());
   int          temp;
   aReturnVal.SetBool(aObjectPtr->AddScriptedVehicle(platformPtr, networkName, route, temp));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, FollowRoute_1, 2, "void", "WsfPlatform, WsfRoute")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfRoute*    routePtr    = static_cast<WsfRoute*>(aVarArgs[1].GetPointer()->GetAppObject());
   aObjectPtr->UpdateScriptedVehicleRoute(platformPtr, routePtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, FollowRoute_2, 3, "void", "WsfPlatform, WsfRoute, int")
{
   WsfPlatform* platformPtr   = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfRoute*    routePtr      = static_cast<WsfRoute*>(aVarArgs[1].GetPointer()->GetAppObject());
   int          waypointIndex = aVarArgs[2].GetInt();
   aObjectPtr->UpdateScriptedVehicleRoute(platformPtr, routePtr, waypointIndex);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, ChangeLane, 2, "bool", "WsfPlatform, bool")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool         direction   = aVarArgs[1].GetBool();
   aReturnVal.SetBool(aObjectPtr->ChangeScriptedLane(platformPtr, direction));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass,
                        XWsfOSM_Traffic,
                        AddConvoyToTraffic,
                        4,
                        "bool",
                        "Array<WsfPlatform>, string, WsfRoute, double")
{
   std::vector<UtScriptData>* inputData =
      static_cast<std::vector<UtScriptData>*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::vector<WsfPlatform*> platformList;
   for (const auto& inputIt : *inputData)
   {
      platformList.emplace_back(static_cast<WsfPlatform*>(inputIt.GetPointer()->GetAppObject()));
   }
   std::string networkName = aVarArgs[1].GetString();
   WsfRoute*   route       = static_cast<WsfRoute*>(aVarArgs[2].GetPointer()->GetAppObject());
   double      spacing     = aVarArgs[3].GetDouble();
   aReturnVal.SetBool(aObjectPtr->AddScriptedConvoy(platformList, networkName, route, spacing));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, LockIntersection, 2, "bool", "string, string")
{
   std::string networkName = aVarArgs[0].GetString();
   std::string nodeName    = aVarArgs[1].GetString();
   aReturnVal.SetBool(aObjectPtr->ScriptLockIntersection(networkName, nodeName));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, UnlockIntersection, 2, "bool", "string, string")
{
   std::string networkName = aVarArgs[0].GetString();
   std::string nodeName    = aVarArgs[1].GetString();
   aReturnVal.SetBool(aObjectPtr->ScriptUnlockIntersection(networkName, nodeName));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOSM_TrafficClass, XWsfOSM_Traffic, GoOffroad, 2, "void", "WsfPlatform, WsfGeoPoint")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfGeoPoint* geoPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[1].GetPointer()->GetAppObject());
   aObjectPtr->ScriptGoOffroad(platformPtr, *geoPointPtr);
}
