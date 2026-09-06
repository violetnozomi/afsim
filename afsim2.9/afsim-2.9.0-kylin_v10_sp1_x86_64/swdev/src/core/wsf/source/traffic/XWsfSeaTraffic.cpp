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

#include "traffic/XWsfSeaTraffic.hpp"

#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfWaypoint.hpp"

// Initialize static data members.
const double XWsfSeaTraffic::cEPSILON(0.000001);

class XWsfSeaTrafficExtension : public WsfScenarioExtension
{
public:
   XWsfSeaTrafficExtension()           = default;
   ~XWsfSeaTrafficExtension() override = default;

   void SimulationCreated(WsfSimulation& aSimulation) override
   {
      aSimulation.RegisterExtension(GetExtensionName(), ut::clone(mPrototype));
   }
   bool           ProcessInput(UtInput& aInput) override { return mPrototype.ProcessInput(aInput); }
   XWsfSeaTraffic mPrototype;
};


void WSF_EXPORT Register_sea_traffic(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("sea_traffic"))
   {
      aApplication.RegisterExtension("sea_traffic",
                                     ut::make_unique<WsfDefaultApplicationExtension<XWsfSeaTrafficExtension>>());
   }
}

//------------------------------------------------------------------------
//! The XWsfSeaTraffic constructor
//! aSimulation: The simulation that the sea traffic simulation will
//!              participate in.
//------------------------------------------------------------------------
XWsfSeaTraffic::XWsfSeaTraffic()
   : mProcessedInput(false)
   , mDepVehiclesFraction(0.0)
   , mLocVehiclesFraction(0.0)
{
}

XWsfSeaTraffic::XWsfSeaTraffic(const XWsfSeaTraffic& aSrc)
   : mProcessedInput(aSrc.mProcessedInput)
   , mDepVehiclesFraction(aSrc.mDepVehiclesFraction)
   , mLocVehiclesFraction(aSrc.mLocVehiclesFraction)
   , mSeaPorts(aSrc.mSeaPorts)
   , mSeaLanes(aSrc.mSeaLanes)
   , mDepVehicles(aSrc.mDepVehicles)
   , mLocVehicles(aSrc.mLocVehicles)
{
}

//------------------------------------------------------------------------
//! The XWsfSeaTraffic destructor.
//! Cleans up the port, lane, and vehicle data.
//------------------------------------------------------------------------
XWsfSeaTraffic::~XWsfSeaTraffic() {}

//------------------------------------------------------------------------
//! Processes the input object passed from the simulation via the input
//! processor. XWsfSeaTraffic handles the sea_traffic input block.
//! If a different input block is encountered false is returned.
//! Throws an input exception if an error is encountered.
//! Returns true if successful.
//! aInput: The input object.
//------------------------------------------------------------------------
bool XWsfSeaTraffic::ProcessInput(UtInput& aInput)
{
   // Handle the sea_traffic block.
   if ((!mProcessedInput) && (aInput.GetCommand() == "sea_traffic"))
   {
      mProcessedInput = true;
      ProcessSeaTraffic(aInput);
   }
   else
   {
      return false;
   }
   return true;
}

//------------------------------------------------------------------------
//! Initializes anything that cannot be determined during the ProcessInput
//! stage and checks for errors.
//! Returns true if successful.
//! aInput: the input object.
//------------------------------------------------------------------------
bool XWsfSeaTraffic::Initialize()
{
   bool success = true;

   // If we aren't using the sea traffic simulation, return true
   // so we don't cause an initialization error.
   if (!mProcessedInput)
   {
      return success;
   }

   // Initialize the ports, lanes, and vehicles.

   Ports::iterator portIter;
   for (portIter = mSeaPorts.begin(); portIter != mSeaPorts.end(); ++portIter)
   {
      // All the departure and local vehicle departure intervals must be calculated
      // before initializing each port.
      if (mDepVehiclesFraction != 0.0)
      {
         portIter->second.mDepVehiclesInterval = portIter->second.mDepartureInterval / mDepVehiclesFraction;
      }
      if (mLocVehiclesFraction != 0.0)
      {
         portIter->second.mLocVehiclesInterval = portIter->second.mDepartureInterval / mLocVehiclesFraction;
      }
   }

   for (portIter = mSeaPorts.begin(); portIter != mSeaPorts.end(); ++portIter)
   {
      success &= portIter->second.Initialize(*this);
   }

   Lanes::iterator laneIter;
   for (laneIter = mSeaLanes.begin(); laneIter != mSeaLanes.end(); ++laneIter)
   {
      success &= laneIter->second.Initialize(*this);
   }

   Vehicles::iterator vehicleIter;
   for (vehicleIter = mDepVehicles.begin(); vehicleIter != mDepVehicles.end(); ++vehicleIter)
   {
      success &= vehicleIter->second.Initialize(*this);
   }

   for (vehicleIter = mLocVehicles.begin(); vehicleIter != mLocVehicles.end(); ++vehicleIter)
   {
      success &= vehicleIter->second.Initialize(*this);
   }

   // If everything initialized correctly write out a sedit file that can be
   // used to display the shipping lanes and generate the initial entity laydown.
   if (success)
   {
      WriteScenarioEditorFile();
      GenerateInitialEntities();
   }
   return success;
}

//------------------------------------------------------------------------
//! Creates a vehicle that departs the given port at the current simulation
//! time. Each call to HandleDeparture schedules another event, which
//! calls Departure again at aSimTime + "the port's departure interval".
//! This guarantees a vehicle will depart a given port at its defined
//! departure interval.
//! aPortId:  The port's name as a string id.
//! aSimTime: The current simulation time.
//------------------------------------------------------------------------
void XWsfSeaTraffic::HandleDeparture(WsfStringId aPortId, double aSimTime)
{
   bool success = true;

   if (mSeaPorts.find(aPortId) == mSeaPorts.end())
   {
      auto out = ut::log::info() << "sea_traffic: Port does not exist.";
      out.AddNote() << "Port: " << aPortId;
      return;
   }

   // Get a pointer to the port that the vehicle will depart from
   SeaPort& port = mSeaPorts[aPortId];
   // Determine if this is a local or remote departure.
   bool localDeparture = GetSimulation().GetRandom().Bernoulli(1.0 - mDepVehiclesFraction);

   // Generate local departure.
   if (localDeparture)
   {
      { // RAII block
         auto out = ut::log::info() << "sea_traffic: Local departure from port.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Port: " << aPortId;
      }

      Vehicle* vehiclePtr = GetRandomVehicle(mLocVehicles);
      if (vehiclePtr != nullptr)
      {
         success = GenerateDeparture(port, true, *vehiclePtr, 0.0, aSimTime);
      }
   }
   // Generate a remote departure.
   else
   {
      { // RAII block
         auto out = ut::log::info() << "sea_traffic: Departure from port.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Port: " << aPortId;
      }

      Vehicle* vehiclePtr = GetRandomVehicle(mDepVehicles);
      if (vehiclePtr != nullptr)
      {
         success = GenerateDeparture(port, false, *vehiclePtr, 0.0, aSimTime);
      }
   }

   if (success)
   {
      { // RAII block
         auto out = ut::log::info() << "sea_traffic: Scheduled another departure from port.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Scheduled T = " << aSimTime + port.mDepartureInterval;
         out.AddNote() << "Port: " << aPortId;
      }
      GetSimulation().AddEvent(ut::make_unique<DepartureEvent>(this, aPortId, aSimTime + port.mDepartureInterval));
   }
   else
   {
      ut::log::error() << "sea_traffic: Could not generate entity, so no more departure events will be scheduled.";
   }
}

//------------------------------------------------------------------------
//! When a platform enters a loiter pattern an 'ExitLoiter' event is
//! scheduled some time in the future to break it out of its loiter pattern.
//! The loiter time is calculated based on the platform's type, which is
//! mapped to the data entered in the input file.
//! aPlatformPtr: A pointer to the simulation platform.
//! aSimTime:     The current simulation time.
//------------------------------------------------------------------------
void XWsfSeaTraffic::HandleEnterLoiter(WsfPlatform* aPlatformPtr, double aSimTime)
{
   double loiterTime = 0.0;

   auto iter = mLocVehicles.find(aPlatformPtr->GetTypeId());
   if (iter != mLocVehicles.end())
   {
      loiterTime = GetRandomLoiterTime(iter->second);
   }
   GetSimulation().AddEvent(ut::make_unique<ExitLoiterEvent>(aPlatformPtr->GetNameId(), aSimTime + loiterTime));
}

//------------------------------------------------------------------------
//! Returns the list of sea lanes that have the given port as one of
//! the endpoints.
//! aPortId:  The port's name as a string id.
//! aLaneIds: The returned list of sea lane string ids.
//------------------------------------------------------------------------
void XWsfSeaTraffic::GetSeaLaneIds(WsfStringId aPortId, std::vector<WsfStringId>& aLaneIds)
{
   aLaneIds.clear();
   Lanes::iterator iter;
   for (iter = mSeaLanes.begin(); iter != mSeaLanes.end(); ++iter)
   {
      // If either of the lane's endpoints are the same as aPortId
      // add that lane to the ids list.
      if ((iter->second.mPortId[0] == aPortId) || (iter->second.mPortId[1] == aPortId))
      {
         aLaneIds.push_back(iter->first);
      }
   }
}

//------------------------------------------------------------------------
//! Parses the sea_traffic ... end_sea_traffic input block.
//! Returns true if successful. Throws an input exception if an error
//! is encountered.
//! aInput: The input object.
//------------------------------------------------------------------------
// private
bool XWsfSeaTraffic::ProcessSeaTraffic(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput, "end_sea_traffic");
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "port")
      {
         std::string name;
         aInput.ReadCommand(name);
         WsfStringId nameId = name;

         if (DoesPortExist(nameId))
         {
            throw UtInput::BadValue(aInput, "XWsfSeaTraffic: Port " + name + " has already been defined");
         }

         SeaPort port(nameId);
         if (!port.ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
         else
         {
            mSeaPorts[nameId] = port;
         }
      }
      else if (command == "lane")
      {
         std::string name;
         aInput.ReadCommand(name);
         WsfStringId nameId = name;

         if (DoesLaneExist(nameId))
         {
            throw UtInput::BadValue(aInput, "XWsfSeaTraffic: Lane " + name + " has already been defined");
         }

         SeaLane lane(nameId);
         if (!lane.ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
         else
         {
            mSeaLanes[nameId] = lane;
         }
      }
      else if (command == "departure_traffic")
      {
         aInput.ReadValue(mDepVehiclesFraction);
         ProcessDepartureTraffic(aInput);
      }
      else if (command == "local_traffic")
      {
         aInput.ReadValue(mLocVehiclesFraction);
         ProcessLocalTraffic(aInput);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   // Make sure the fraction of departure vehicles and local vehicles is 1.0
   if ((1.0 - (mDepVehiclesFraction + mLocVehiclesFraction)) > cEPSILON)
   {
      throw UtInput::BadValue(aInput, "XWsfSeaTraffic: The fraction of departure and local vehicles must be 1.0");
   }

   // Make sure the sum of all the departure vehicle types is 1.0
   double             cumulativeFraction = 0.0;
   Vehicles::iterator vehicleIter;
   for (vehicleIter = mDepVehicles.begin(); vehicleIter != mDepVehicles.end(); ++vehicleIter)
   {
      cumulativeFraction += vehicleIter->second.mFraction;
   }

   if (fabs(1.0 - cumulativeFraction) > cEPSILON)
   {
      throw UtInput::BadValue(aInput,
                              "XWsfSeaTraffic: The sum of each of the departure vehicle type fractions must be 1.0");
   }

   // Make sure the sum of all the local vehicle types is 1.0
   bool haveLocalTraffic = false;
   cumulativeFraction    = 0.0;
   for (vehicleIter = mLocVehicles.begin(); vehicleIter != mLocVehicles.end(); ++vehicleIter)
   {
      cumulativeFraction += vehicleIter->second.mFraction;
      haveLocalTraffic = true;
   }

   if (haveLocalTraffic && (fabs(1.0 - cumulativeFraction) > cEPSILON))
   {
      throw UtInput::BadValue(aInput, "XWsfSeaTraffic: The sum of each of the local vehicle type fractions must be 1.0");
   }

   return true;
}

//------------------------------------------------------------------------
//! Parses the departure_traffic ... end_departure_traffic input block.
//! Returns true if successful. Throws an input exception if an error
//! is encountered.
//! aInput: The input object.
//------------------------------------------------------------------------
// private
void XWsfSeaTraffic::ProcessDepartureTraffic(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput, "end_departure_traffic");
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "ship")
      {
         std::string type;
         aInput.ReadCommand(type);
         WsfStringId typeId = type;

         if (DoesDepVehicleExist(typeId))
         {
            throw UtInput::BadValue(aInput, "XWsfSeaTraffic: Departure ship " + type + " has already been defined");
         }

         Vehicle vehicle(typeId);
         if (!vehicle.ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
         else
         {
            mDepVehicles[typeId] = vehicle;
         }
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

//------------------------------------------------------------------------
//! Parses the local_traffic ... end_local_traffic input block.
//! Returns true if successful. Throws an input exception if an error
//! is encountered.
//! aInput: The input object.
//------------------------------------------------------------------------
// private
void XWsfSeaTraffic::ProcessLocalTraffic(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput, "end_local_traffic");
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "ship")
      {
         std::string type;
         aInput.ReadCommand(type);
         WsfStringId typeId = type;

         if (DoesLocVehicleExist(typeId))
         {
            throw UtInput::BadValue(aInput, "XWsfSeaTraffic: Local ship " + type + " has already been defined");
         }

         Vehicle vehicle(typeId);
         if (!vehicle.ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
         else
         {
            mLocVehicles[typeId] = vehicle;
         }
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

//------------------------------------------------------------------------
//! Generates the initial entities needed to populate each of the shipping
//! lanes at the correct density based on each port's departure interval.
//! Local vehicles are created based on the port's departure interval and
//! split evenly across all the local regions defined for that port.
//------------------------------------------------------------------------
// private
void XWsfSeaTraffic::GenerateInitialEntities()
{
   Ports::iterator portIter;
   for (portIter = mSeaPorts.begin(); portIter != mSeaPorts.end(); ++portIter)
   {
      SeaPort* portPtr = &portIter->second;

      // Calculate the maximum travel time from this port.
      double maxTravelTime = CalcMaxTravelTime(*portPtr, mDepVehicles);

      // Calculate the number of vehicles needed to properly fill the shipping lanes leaving this port.
      unsigned int numDepartureVehicles = static_cast<unsigned int>(maxTravelTime / portPtr->mDepVehiclesInterval);

      // Calculate the number of local vehicles needed to maintain the user
      // specified remote/local departure fraction.
      // unsigned int numLocalVehicles = static_cast<unsigned int>(maxTravelTime / portPtr->mLocVehiclesInterval);

      unsigned int i;
      // Generate the departure entities.
      // Create the vehicles leaving this port and place them at a distance along their route to
      // create the proper initial distribution.
      for (i = 0; i < numDepartureVehicles; ++i)
      {
         Vehicle* vehiclePtr = GetRandomVehicle(mDepVehicles);
         if (vehiclePtr != nullptr)
         {
            // Calculate the distance along the route as the time spent traveling multiplied
            // by the vehicles speed (i.e. (time = i*interval) * (speed)).
            double distanceAlongRoute = i * portPtr->mDepVehiclesInterval * vehiclePtr->mSpeed;

            // Actually generate the departure at a sim time of 0 seconds.
            GenerateDeparture(*portPtr, false, *vehiclePtr, distanceAlongRoute, 0.0);
         }
      }

      // Generate the local entities
      // Create the vehicles that stay in this port's local area.

      // Calculate the number of vehicles per local region (Divide them evenly).
      // unsigned int numVehiclesPerLocalRegion = numLocalVehicles / portPtr->mLocalTrafficRegions.size();

      // for (i = 0; i < portPtr->mLocalTrafficRegions.size(); ++i)
      //{
      //    for (unsigned int j = 1; j <= numVehiclesPerLocalRegion; ++j)
      //    {
      //       Vehicle* vehiclePtr = GetRandomVehicle(mLocVehicles);
      //       if (vehiclePtr != 0)
      //       {
      //          // Calculate the distance along the route as the time spent traveling multiplied
      //          // by the vehicles speed (i.e. (time = i*interval) * (speed)).
      //          double distanceAlongRoute = j * portPtr->mLocVehiclesInterval * vehiclePtr->mSpeed;

      //         // Actually generate the departure at a sim time of 0 seconds.
      //         if (! GenerateDeparture(*portPtr, true, *vehiclePtr, distanceAlongRoute, 0.0))
      //         {
      //            break;
      //         }
      //      }
      //   }
      //}
   }
}

//------------------------------------------------------------------------
//! Creates a vehicle of the given type that departs the given port at the
//! specified time.
//! aPort:               The port the vehicle will depart from.
//! aLocalDeparture:     Specifies if the vehicle is local traffic or is
//!                      departing to another port.
//! aVehicle:            The type of vehicle to create.
//! aDistanceAlongRoute: This is the distance you want the vehicle to start
//!                      along its route. This is used during initialization.
//! aSimTime:            The simulation time to introduce the entity.
//------------------------------------------------------------------------
// private
bool XWsfSeaTraffic::GenerateDeparture(SeaPort& aPort,
                                       bool     aLocalDeparture,
                                       Vehicle& aVehicle,
                                       double   aDistanceAlongRoute,
                                       double   aSimTime)
{
   bool success = false;
   // The vehicle is going to stay in the local area.
   if (aLocalDeparture)
   {
      double speed = GetRandomSpeed(aVehicle);

      // Pick a local region for the vehicle to travel to.
      SeaRegion region;
      GetRandomRegion(aPort, region);

      WsfRoute route;
      GetRandomRoute(aPort, region, aVehicle, speed, route);

      if (aDistanceAlongRoute < route.GetTotalLength())
      {
         // Update the route to start the vehicle at the specified distance
         // along the route.
         if (aDistanceAlongRoute > 0.0)
         {
            UpdateRoute(aDistanceAlongRoute, route);
         }

         // Now that we have the departure route, actually generate the entity.
         success = GenerateEntity(aVehicle, route, aSimTime);
      }
   }
   // Otherwise, the vehicle is traveling to another port.
   else
   {
      // Pick a shipping lane.
      SeaLane* lanePtr = GetRandomLane(aPort);
      if (lanePtr != nullptr)
      {
         // Determine the destination port.
         WsfStringId dstPortId  = lanePtr->GetDestinationPort(aPort.mNameId);
         SeaPort*    dstPortPtr = &mSeaPorts[dstPortId];
         if (dstPortPtr != nullptr)
         {
            // Determine a speed for the vehicle.
            double speed = GetRandomSpeed(aVehicle);

            // Generate a route between this port and the destination port.
            WsfRoute route;
            GetRandomRoute(*lanePtr, aPort, *dstPortPtr, speed, route);

            // Make sure the distance along the route that we're requesting isn't
            // longer that the route itself.
            if (aDistanceAlongRoute < route.GetTotalLength())
            {
               // Update the route to start the vehicle at the specified distance
               // along the route.
               if (aDistanceAlongRoute > 0.0)
               {
                  UpdateRoute(aDistanceAlongRoute, route);
               }

               // Now that we have the departure route, actually generate the entity.
               success = GenerateEntity(aVehicle, route, aSimTime);
            }
         }
      }
   }
   return success;
}

//------------------------------------------------------------------------
//! Creates a platform, initializes it with the provided route, and
//! inserts it into the simulation. Returns true if successful.
//! aVehicle: The type vehicle to create the entity as.
//! aRoute:   The route that the entity will travel.
//! aSimTime: The current simulation time.
//------------------------------------------------------------------------
// private
bool XWsfSeaTraffic::GenerateEntity(Vehicle& aVehicle, WsfRoute& aRoute, double aSimTime)
{
   std::string type = aVehicle.mTypeId;

   // Create the platform
   WsfPlatform* platformPtr = (WsfPlatform*)GetScenario().CloneType("platform_type", type);
   if (platformPtr != nullptr)
   {
      // Set the platform's name as its type concatenated with a unique number
      static unsigned int uid = 0;
      std::ostringstream  oss;
      oss << type << '_' << ++aVehicle.mCount << '_' << ++uid;
      platformPtr->SetName(oss.str());

      // Add a mover.
      WsfMover* moverPtr = platformPtr->GetMover();
      if (moverPtr == nullptr)
      {
         { // RAII block
            auto out = ut::log::error() << "sea_traffic: Cannot set the mover's path because the mover is null.";
            out.AddNote() << "Platform: " << platformPtr->GetName();
         }
         delete platformPtr;
         return false;
      }
      aRoute[aRoute.GetSize() - 1].SetEndOfPathOption(WsfPath::cEP_REMOVE);
      moverPtr->SetRoute(aSimTime, aRoute);

      // Add the platform to the simulation
      if (!GetSimulation().AddPlatform(aSimTime, platformPtr))
      {
         { // RAII block
            auto out = ut::log::error() << "sea_traffic: There was a problem adding the platform to the simulation.";
            out.AddNote() << "Platform: " << platformPtr->GetName();
         }
         delete platformPtr;
         return false;
      }
   }
   else
   {
      auto out = ut::log::error() << "sea_traffic: Invalid platform type.";
      out.AddNote() << "Type: " << type;
      return false;
   }
   return true;
}

//------------------------------------------------------------------------
//! Returns a route that starts at the given distance along the provided
//! route.
//! aDistanceAlongRoute: The distance (meters) along the route, starting
//!                      from the first waypoint.
//! aRoute:              The route to modify and return by reference.
//------------------------------------------------------------------------
// private
void XWsfSeaTraffic::UpdateRoute(double aDistanceAlongRoute, WsfRoute& aRoute)
{
   int wptIndex;
   // Get the segment at the specified distance along the route
   if (aRoute.GetEndPointsAtDistance(aDistanceAlongRoute, wptIndex))
   {
      double       distanceIntoSegment = aDistanceAlongRoute - aRoute.GetDistance(0, wptIndex);
      WsfWaypoint& wpt1                = aRoute[wptIndex];
      WsfWaypoint& wpt2                = aRoute[wptIndex + 1];
      double       heading;
      double       distance;
      UtSphericalEarth::GreatCircleHeadingAndDistance(wpt1.GetLat(), wpt1.GetLon(), wpt2.GetLat(), wpt2.GetLon(), heading, distance);

      // Generate a waypoint that is at the correct location into the route based on the distance along the route.
      // This will be used as the route's new first waypoint.
      WsfWaypoint wpt = wpt1; // Preserves the speed and altitude values.
      double      lat, lon;
      UtSphericalEarth::ExtrapolateGreatCirclePosition(wpt1.GetLat(), wpt1.GetLon(), heading, distanceIntoSegment, lat, lon);
      wpt.SetLat(lat);
      wpt.SetLon(lon);

      WsfRoute route;
      aRoute.GetSubroute(wptIndex + 1, aRoute.GetSize() - 1, route);
      aRoute.Clear();
      aRoute.Append(wpt);
      aRoute.Append(route);
   }
}

//------------------------------------------------------------------------
//! Returns randomly one of the vehicles defined by this simulation, but
//! weighted so that each type is represented by the fraction it defines.
//! For example, if a CARGO vehicle is defined with a fractional value of
//! 0.60, then a vehicle type of CARGO will be returned 60% of the time.
//! If no vehicles exist, null is returned.
//! aVehicles: The list of vehicles to choose from.
//------------------------------------------------------------------------
// private
XWsfSeaTraffic::Vehicle* XWsfSeaTraffic::GetRandomVehicle(Vehicles& aVehicles)
{
   double uniformRandom      = GetSimulation().GetRandom().Uniform<double>();
   double cumulativeFraction = 0.0;

   Vehicles::iterator iter;
   for (iter = aVehicles.begin(); iter != aVehicles.end(); ++iter)
   {
      cumulativeFraction += iter->second.mFraction;
      if (uniformRandom < cumulativeFraction)
      {
         return &iter->second;
      }
   }
   return nullptr;
}

//------------------------------------------------------------------------
//! Returns randomly one of the sea lanes that connect to the given port.
//! aPort: The port that contains a list of outgoing lanes, which are
//!        used to choose the random lane from.
//------------------------------------------------------------------------
// private
XWsfSeaTraffic::SeaLane* XWsfSeaTraffic::GetRandomLane(SeaPort& aPort)
{
   if (!aPort.mSeaLaneIds.empty())
   {
      double uniformRandom      = GetSimulation().GetRandom().Uniform<double>();
      double cumulativeFraction = 0.0;

      int                                 laneIndex = 0;
      std::vector<double>::const_iterator iter;
      for (iter = aPort.mDstPortWeights.begin(); iter != aPort.mDstPortWeights.end(); ++iter)
      {
         cumulativeFraction += *iter;
         if (uniformRandom < cumulativeFraction)
         {
            return &mSeaLanes[aPort.mSeaLaneIds[laneIndex]];
         }
         ++laneIndex;
      }
   }
   return nullptr;
}

//------------------------------------------------------------------------
//! Returns a random path along a given lane.
//! The path concatenates the departure port's "port route" with
//! the sea lane's route and the arrival port's "port route"
//! to form the complete path ("port route" is the route into (out of)
//! a port).
//! aLane:        The lane that is used to construct the route from.
//! aSrcPort:     The source port that is used to construct the
//!               route from.
//! aDstPort:     The destination port that is used to construct the
//!               route from.
//! aSpeed:       The speed that the vehicle will be traveling. This
//!               just defaults each waypoint to have this speed.
//! aRoute:       The route that is returned by reference.
//! aActualRoute: A flag that if true, returns the actual waypoints that
//!               that define the route. Otherwise, a random path is
//!               returned.
//------------------------------------------------------------------------
// private
void XWsfSeaTraffic::GetRandomRoute(SeaLane&  aLane,
                                    SeaPort&  aSrcPort,
                                    SeaPort&  aDstPort,
                                    double    aSpeed,
                                    WsfRoute& aRoute,
                                    bool      aActualRoute)
{
   std::vector<SeaRegion>::iterator         iter;
   std::vector<SeaRegion>::reverse_iterator rIter;

   WsfWaypoint wpt;

   // Append the port's position
   GetRandomWaypoint(aSrcPort.mPosition, wpt, aActualRoute);
   aRoute.Append(wpt);

   // Append the source port's port route, if we aren't ignoring it.
   if (!aLane.IgnorePortRoute(aSrcPort.mNameId))
   {
      for (rIter = aSrcPort.mPortRoute.rbegin(); rIter != aSrcPort.mPortRoute.rend(); ++rIter)
      {
         GetRandomWaypoint(*rIter, wpt, aActualRoute);
         aRoute.Append(wpt);
      }
   }

   // Append the lane's main route
   // If the source port is the lanes first port then use the waypoints in order
   if (aLane.mPortId[0] == aSrcPort.mNameId)
   {
      for (iter = aLane.mLaneRoute.begin(); iter != aLane.mLaneRoute.end(); ++iter)
      {
         GetRandomWaypoint(*iter, wpt, aActualRoute);
         aRoute.Append(wpt);
      }
   }
   // otherwise, the waypoints need to be reversed
   else
   {
      for (rIter = aLane.mLaneRoute.rbegin(); rIter != aLane.mLaneRoute.rend(); ++rIter)
      {
         GetRandomWaypoint(*rIter, wpt, aActualRoute);
         aRoute.Append(wpt);
      }
   }

   // Append the destination port's port route, if we aren't ignoring it.
   if (!aLane.IgnorePortRoute(aDstPort.mNameId))
   {
      for (iter = aDstPort.mPortRoute.begin(); iter != aDstPort.mPortRoute.end(); ++iter)
      {
         GetRandomWaypoint(*iter, wpt, aActualRoute);
         aRoute.Append(wpt);
      }
   }

   // Append the destination port's position
   GetRandomWaypoint(aDstPort.mPosition, wpt, aActualRoute);
   aRoute.Append(wpt);

   // Set the speed of each waypoint as the vehicle's speed.
   for (unsigned int ii = 0; ii < aRoute.GetSize(); ++ii)
   {
      // Set the last waypoint's speed value to zero
      aRoute[ii].SetSpeed((ii == (aRoute.GetSize() - 1)) ? 0.1 : aSpeed);
      aRoute[ii].SetLinearAccel(0.0);
      aRoute[ii].SetAlt(0.5);
   }
}

//------------------------------------------------------------------------
//! Returns a random path from the given port to the given region and back.
//! The route consists of the path from the given port to the given region,
//! a path inside the region, and the path back to the port.  The aVehicle
//! parameter is used to calculate the loiter time used in the local region.
//! aPort:       The port that the route will leave from and return to.
//! aRegion:     The region that the route will run to and return from.
//! aVehicle:    The vehicle type that will travel the route.
//! aSpeed:      The speed that the entity will travel. This is used to
//!              make sure the route is long enough to accommodate the
//!              requested loiter time.
//! aRoute:      The route that is returned by reference.
//------------------------------------------------------------------------
// private
void XWsfSeaTraffic::GetRandomRoute(SeaPort& aPort, SeaRegion& aRegion, Vehicle& aVehicle, double aSpeed, WsfRoute& aRoute)
{
   aRoute.Clear();

   WsfWaypoint curWpt;
   WsfWaypoint prvWpt;

   double altitude = 0.5;

   // As the first waypoint, use a random location inside the port's boundary.
   GetRandomWaypoint(aPort.mPosition, curWpt);
   aRoute.Append(curWpt);

   // If we have a route defined for this vehicle type, use it.
   if (!aVehicle.mRouteIds.empty())
   {
      // Select a random loiter route.
      size_t    loiterRouteIndex = GetSimulation().GetRandom().Uniform<size_t>(0, aVehicle.mRouteIds.size() - 1);
      WsfRoute* loiterRoutePtr =
         WsfRouteTypes::Get(GetSimulation().GetScenario()).Clone(aVehicle.mRouteIds[loiterRouteIndex]);
      if (loiterRoutePtr != nullptr)
      {
         // Choose a location where the loiter route will be placed.
         GetRandomWaypoint(aRegion, curWpt);

         // Choose an angle to rotate the path.
         double theta = GetSimulation().GetRandom().Uniform(0.0, 360.0);

         // Transform the loiter route to the location and by the rotation angle.
         loiterRoutePtr->Transform(curWpt.GetLat(), curWpt.GetLon(), theta);
         (*loiterRoutePtr)[0].SetScriptId("XWSF_SEA_TRAFFIC_ENTER_LOITER_ACTION");
         aRoute.Append(*loiterRoutePtr);

         delete loiterRoutePtr;
         loiterRoutePtr = nullptr;
      }
   }
   // Otherwise, generate a random route.
   else
   {
      double loiterTime = GetRandomLoiterTime(aVehicle);

      // Append as many waypoints as necessary to accommodate the loiter time.
      // Use random waypoints inside the destination region.
      double time = 0.0;
      GetRandomWaypoint(aRegion, prvWpt);
      aRoute.Append(prvWpt);
      while (time <= loiterTime)
      {
         GetRandomWaypoint(aRegion, curWpt);
         aRoute.Append(curWpt);

         double heading;
         double distance;
         UtSphericalEarth::GreatCircleHeadingAndDistance(prvWpt.GetLat(),
                                                         prvWpt.GetLon(),
                                                         curWpt.GetLat(),
                                                         curWpt.GetLon(),
                                                         heading,
                                                         distance);
         time += distance / aSpeed;

         prvWpt = curWpt;
      }
   }

   // As the last waypoint, use a random location inside the port's boundary.
   GetRandomWaypoint(aPort.mPosition, curWpt);
   curWpt.SetLabelId("XWSF_SEA_TRAFFIC_HOME_LABEL");
   aRoute.Append(curWpt);

   for (unsigned int ii = 0; ii < aRoute.GetSize(); ++ii)
   {
      // Set the last waypoint's speed value to zero
      aRoute[ii].SetSpeed(aSpeed); //(ii == (aRoute.GetSize()-1)) ? 0.1f : static_cast<float>(aSpeed);
      aRoute[ii].SetLinearAccel(0.0);
      aRoute[ii].SetAlt(altitude);
   }
}

//------------------------------------------------------------------------
//! Returns a waypoint located inside of the given region.
//! aRegion:         The region to generate the waypoint inside of.
//! aWaypoint:       The waypoint that is returned by reference.
//! aActualWaypoint: A flag if true, returns the actual waypoint location
//!                  and if false, returns a random waypoint.
//------------------------------------------------------------------------
// private
void XWsfSeaTraffic::GetRandomWaypoint(SeaRegion& aRegion, WsfWaypoint& aWaypoint, bool aActualWaypoint)
{
   // Return the actual waypoint
   if (aActualWaypoint)
   {
      aWaypoint.SetLat(aRegion.mLat);
      aWaypoint.SetLon(aRegion.mLon);
   }
   // Otherwise, calculate a random point inside the region
   else
   {
      double theta  = GetSimulation().GetRandom().Uniform(0.0, 360.0);
      double radius = GetSimulation().GetRandom().Uniform(0.0, aRegion.mRadius);
      double lat, lon;
      UtSphericalEarth::ExtrapolateGreatCirclePosition(aRegion.mLat, aRegion.mLon, theta, radius, lat, lon);
      aWaypoint.SetLat(lat);
      aWaypoint.SetLon(lon);
   }
}

void XWsfSeaTraffic::GetRandomRegion(SeaPort& aPort, SeaRegion& aRegion)
{
   double uniformRandom      = GetSimulation().GetRandom().Uniform<double>();
   double cumulativeFraction = 0.0;

   std::vector<SeaRegion>::const_iterator iter;
   for (iter = aPort.mLocalTrafficRegions.begin(); iter != aPort.mLocalTrafficRegions.end(); ++iter)
   {
      cumulativeFraction += iter->mFraction;
      if (uniformRandom < cumulativeFraction)
      {
         aRegion = *iter;
         break;
      }
   }
}

//------------------------------------------------------------------------
//! Returns a speed randomly distributed about the vehicle's
//! mean speed.
//! aVehicle: The vehicle type to generate the random speed from.
//------------------------------------------------------------------------
// private
double XWsfSeaTraffic::GetRandomSpeed(Vehicle& aVehicle)
{
   // Compute vehicle speed
   double speed = -1.0;
   while (speed <= 0.0)
   {
      speed = aVehicle.mSpeed + (aVehicle.mSigmaSpeed * GetSimulation().GetRandom().Gaussian());
   }
   return speed;
}

//------------------------------------------------------------------------
//! Returns a loiter time randomly distributed about the vehicle's mean
//! loiter time.
//! aVehicle: The vehicle type to generate the random loiter time from.
//------------------------------------------------------------------------
// private
double XWsfSeaTraffic::GetRandomLoiterTime(Vehicle& aVehicle)
{
   double loiterTime = -1.0;
   while (loiterTime < 0.0)
   {
      loiterTime = aVehicle.mLoiterTime + (aVehicle.mSigmaLoiterTime * GetSimulation().GetRandom().Gaussian());
   }
   return loiterTime;
}

//------------------------------------------------------------------------
//! Returns the maximum time a vehicle will take to traverse the longest
//! shipping lane that leaves the given port.
//! aPort:     The port that contains the outgoing lanes.
//! aVehicles: The vehicle list used to determine the slowest speed.
//------------------------------------------------------------------------
// private
double XWsfSeaTraffic::CalcMaxTravelTime(SeaPort& aPort, Vehicles& aVehicles)
{
   double maxTravelTime   = 0.0;
   double maxLaneLength   = -9999999.0;
   double minVehicleSpeed = 9999999.0;

   // Find the length of the longest lane
   std::vector<WsfStringId>::const_iterator laneIdIter;
   for (laneIdIter = aPort.mSeaLaneIds.begin(); laneIdIter != aPort.mSeaLaneIds.end(); ++laneIdIter)
   {
      SeaLane* lanePtr = &mSeaLanes[*laneIdIter];
      if (lanePtr != nullptr)
      {
         // Get pointers to each of the ports
         SeaPort* srcPortPtr = &mSeaPorts[lanePtr->mPortId[0]];
         SeaPort* dstPortPtr = &mSeaPorts[lanePtr->mPortId[1]];

         if (srcPortPtr && dstPortPtr)
         {
            WsfRoute route;
            GetRandomRoute(*lanePtr, *srcPortPtr, *dstPortPtr, 0.0, route, true);

            if (route.GetTotalLength() > maxLaneLength)
            {
               maxLaneLength = route.GetTotalLength();
            }
         }
      }
   }

   // Find the slowest vehicle
   Vehicles::iterator vehicleIter;
   for (vehicleIter = aVehicles.begin(); vehicleIter != aVehicles.end(); ++vehicleIter)
   {
      Vehicle* vehiclePtr = &vehicleIter->second;

      if (vehiclePtr->mSpeed < minVehicleSpeed)
      {
         minVehicleSpeed = vehiclePtr->mSpeed;
      }
   }

   if (minVehicleSpeed != 0.0)
   {
      maxTravelTime = maxLaneLength / minVehicleSpeed;
      maxTravelTime *= 1.1;
   }
   return maxTravelTime;
}

//------------------------------------------------------------------------
//! Writes out a Scenario Editor file that contains the shipping lanes.
//! This can be viewed in Scenario Editor and Clouseau.
//------------------------------------------------------------------------
// private
void XWsfSeaTraffic::WriteScenarioEditorFile()
{
   std::string outFileName = "sea_traffic.sed";

   std::ofstream ofs(outFileName.c_str());
   if (!ofs)
   {
      auto out = ut::log::error() << "sea_traffic: Unable to open output file.";
      out.AddNote() << "File: " << outFileName;
      return;
   }
   // Create a zone for each of the shipping lanes
   Lanes::iterator laneIter;
   for (laneIter = mSeaLanes.begin(); laneIter != mSeaLanes.end(); ++laneIter)
   {
      SeaLane* lanePtr = &laneIter->second;

      // Get pointers to each of the ports
      SeaPort* srcPortPtr = &mSeaPorts[lanePtr->mPortId[0]];
      SeaPort* dstPortPtr = &mSeaPorts[lanePtr->mPortId[1]];

      if (srcPortPtr && dstPortPtr)
      {
         WsfRoute route;
         GetRandomRoute(*lanePtr, *srcPortPtr, *dstPortPtr, 0.0, route, true);

         // Write out the zone for the route
         ofs << "zone" << std::endl;
         ofs << "name: " << lanePtr->mNameId << std::endl;
         ofs << "line_color: 1.0 1.0 0.0" << std::endl;
         ofs << "line_width: 1.0" << std::endl;
         ofs << "non_closed" << std::endl;
         ofs << "stationary" << std::endl;

         UtLatPos lat;
         UtLonPos lon;
         for (unsigned int ii = 0; ii < route.GetSize(); ++ii)
         {
            lat = route[ii].GetLat();
            lon = route[ii].GetLon();
            ofs << "lla: " << lat << " " << lon << " 500 m msl" << std::endl;
         }

         ofs << "end_zone" << std::endl;
      }
   }
   ofs.close();
}

XWsfSeaTraffic* XWsfSeaTraffic::Clone() const
{
   return new XWsfSeaTraffic(*this);
}


//------------------------------------------------------------------------
//! Parses the port ... end_port input block.
//! Returns true if successful. Throws an input exception if an error
//! is encountered.
//! aInput: The input object.
//------------------------------------------------------------------------
bool XWsfSeaTraffic::SeaPort::ProcessInput(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput, "end_port");
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "position")
      {
         double lat;
         double lon;
         double radius;
         aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
         aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
         aInput.ReadValueOfType(radius, UtInput::cLENGTH);
         radius /= 2.0;
         mPosition = SeaRegion(lat, lon, radius);
      }
      else if (command == "port_route")
      {
         UtInputBlock routeBlock(aInput, "end_port_route");
         while (routeBlock.ReadCommand())
         {
            std::string command1(aInput.GetCommand());
            if (command1 == "position")
            {
               double lat;
               double lon;
               double radius;
               aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
               aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
               aInput.ReadValueOfType(radius, UtInput::cLENGTH);
               radius /= 2.0;
               mPortRoute.emplace_back(lat, lon, radius);
            }
            else
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
      }
      else if (command == "local_traffic_region")
      {
         double lat;
         double lon;
         double radius;
         double fraction;
         aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
         aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
         aInput.ReadValueOfType(radius, UtInput::cLENGTH);
         radius /= 2.0;
         aInput.ReadValue(fraction);
         mLocalTrafficRegions.emplace_back(lat, lon, radius, fraction);
      }
      else if (command == "departure_interval")
      {
         double interval;
         aInput.ReadValueOfType(interval, UtInput::cTIME);
         aInput.ValueGreater(interval, 0.0);
         mDepartureInterval = interval;
      }
      else if (command == "use_all_lanes")
      {
         mUseAllLanes = true;
      }
      else if (command == "lane")
      {
         std::string name;
         aInput.ReadCommand(name);
         mSeaLaneIds.emplace_back(name);
         double weight;
         aInput.ReadValue(weight);
         mDstPortWeights.push_back(weight);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   // Make sure that no lanes are specified if the use_all_lanes option is used.
   if (mUseAllLanes && (!mSeaLaneIds.empty()))
   {
      throw UtInput::BadValue(aInput, "You cannot specify individual lanes and the use_all_lanes option at the same time, please pick one or the other");
   }

   // If the departure interval is zero, give it a default value.
   if (mDepartureInterval <= 0.0)
   {
      mDepartureInterval = 60.0; // seconds
   }
   return true;
}

//------------------------------------------------------------------------
//! Verifies any data that can't be verified during the ProcessInput phase
//! and schedules the first departure event.
//! Returns true if successful.
//! aSeaTraffic: The sea traffic simulation.
//------------------------------------------------------------------------
bool XWsfSeaTraffic::SeaPort::Initialize(XWsfSeaTraffic& aSeaTraffic)
{
   // If the user specified to use all the lanes, get the all lanes that have this
   // port as an endpoint.
   if (mUseAllLanes)
   {
      aSeaTraffic.GetSeaLaneIds(mNameId, mSeaLaneIds);
   }

   std::vector<WsfStringId>::const_iterator laneIdIter;
   for (laneIdIter = mSeaLaneIds.begin(); laneIdIter != mSeaLaneIds.end(); ++laneIdIter)
   {
      if (!aSeaTraffic.DoesLaneExist(*laneIdIter))
      {
         auto out = ut::log::error() << "sea_traffic: Lane does not exist.";
         out.AddNote() << "Lane: " << *laneIdIter;
         return false;
      }
   }

   // If the use_all_lanes option was specified we don't have explicit departure weights,
   // so calculate them.
   if (mUseAllLanes)
   {
      // Calculate the sum of all the destination port's departure intervals. This will
      // be used to weight what fraction of departing vehicles go to each destination port.
      double dstPortDepartureIntervalSum = 0.0;
      for (laneIdIter = mSeaLaneIds.begin(); laneIdIter != mSeaLaneIds.end(); ++laneIdIter)
      {
         SeaLane* lanePtr = aSeaTraffic.GetSeaLane(*laneIdIter);

         WsfStringId dstPortId = lanePtr->GetDestinationPort(mNameId);

         SeaPort* dstPortPtr = aSeaTraffic.GetSeaPort(dstPortId);

         dstPortDepartureIntervalSum += 1.0 / dstPortPtr->mDepVehiclesInterval;
      }

      // Calculate a destination port weight, which dictates what percentage of traffic leaving
      // this port go to a given port.  We favour destination ports with more outgoing traffic.
      for (laneIdIter = mSeaLaneIds.begin(); laneIdIter != mSeaLaneIds.end(); ++laneIdIter)
      {
         SeaLane* lanePtr = aSeaTraffic.GetSeaLane(*laneIdIter);

         WsfStringId dstPortId = lanePtr->GetDestinationPort(mNameId);

         SeaPort* dstPortPtr = aSeaTraffic.GetSeaPort(dstPortId);

         // Favour ports with lower departure intervals (i.e. more outgoing traffic).
         mDstPortWeights.push_back((1.0 / dstPortPtr->mDepVehiclesInterval) / dstPortDepartureIntervalSum);
      }
   }

   // Make sure all the weights sum to 1.0
   double                              cumulativeFraction = 0.0;
   std::vector<double>::const_iterator laneWeightIter;
   for (laneWeightIter = mDstPortWeights.begin(); laneWeightIter != mDstPortWeights.end(); ++laneWeightIter)
   {
      cumulativeFraction += *laneWeightIter;
   }

   if ((cumulativeFraction != 0.0) && (fabs(1.0 - cumulativeFraction) > cEPSILON))
   {
      auto out = ut::log::error() << "sea_traffic: The sum of the lane weights must be 1.0.";
      out.AddNote() << "Current Sum: " << cumulativeFraction;
      return false;
   }

   if (mDstPortWeights.size() != mSeaLaneIds.size())
   {
      ut::log::error() << "sea_traffic: The number of port weights does not match the number of outgoing lanes.";
      return false;
   }

   if (!mSeaLaneIds.empty())
   {
      // Schedule the first departure from this port.
      aSeaTraffic.GetSimulation().AddEvent(ut::make_unique<DepartureEvent>(&aSeaTraffic, mNameId, 0.0));
   }

   cumulativeFraction = 0.0;
   std::vector<SeaRegion>::const_iterator localRegionIter;
   for (localRegionIter = mLocalTrafficRegions.begin(); localRegionIter != mLocalTrafficRegions.end(); ++localRegionIter)
   {
      cumulativeFraction += localRegionIter->mFraction;
   }

   if ((cumulativeFraction != 0.0) && (fabs(1.0 - cumulativeFraction) > cEPSILON))
   {
      auto out = ut::log::error() << "sea_traffic: The sum of the local traffic region weights must be 1.0.";
      out.AddNote() << "Current Sum: " << cumulativeFraction;
      return false;
   }

   return true;
}


//------------------------------------------------------------------------
//! Parses the lane ... end_lane input block.
//! Returns true if successful. Throws an input exception if an error
//! is encountered.
//! aInput: The input object.
//------------------------------------------------------------------------
bool XWsfSeaTraffic::SeaLane::ProcessInput(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput, "end_lane");
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "port")
      {
         if (mPortCount < 2)
         {
            std::string portName;
            aInput.ReadCommand(portName);
            mPortId[mPortCount] = portName;

            // Check for optional argument "ignore_port_route"
            std::string option;
            aInput.ReadCommand(option);
            mPortIgnoreRoute[mPortCount] = true;
            if (option != "ignore_port_route")
            {
               mPortIgnoreRoute[mPortCount] = false;
               aInput.PushBack(option);
            }
            ++mPortCount;
         }
         else
         {
            throw UtInput::BadValue(aInput, "XWsfSeaTraffic: Only two ports can be used to define a sea lane");
         }
      }
      else if (command == "lane_route")
      {
         UtInputBlock routeBlock(aInput, "end_lane_route");
         while (routeBlock.ReadCommand())
         {
            std::string command1(aInput.GetCommand());
            if (command1 == "position")
            {
               double lat;
               double lon;
               double radius;
               aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
               aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
               aInput.ReadValueOfType(radius, UtInput::cLENGTH);
               radius /= 2.0;
               mLaneRoute.emplace_back(lat, lon, radius);
            }
            else
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return true;
}

//------------------------------------------------------------------------
//! Verifies any data that can't be verified during the ProcessInput phase.
//! Returns true if successful.
//! aSeaTraffic: The sea traffic simulation.
//------------------------------------------------------------------------
bool XWsfSeaTraffic::SeaLane::Initialize(XWsfSeaTraffic& aSeaTraffic)
{
   // Make sure two ports were specified
   if (mPortCount != 2)
   {
      auto out = ut::log::error() << "sea_traffic: Must specify two ports to properly define a lane.";
      out.AddNote() << "Ports Specified: " << mPortCount;
      return false;
   }

   // Make sure both ports are valid
   if (!aSeaTraffic.DoesPortExist(mPortId[0]))
   {
      auto out = ut::log::error() << "sea_traffic: Port does not exist.";
      out.AddNote() << "Port: " << mPortId[0];
      return false;
   }

   if (!aSeaTraffic.DoesPortExist(mPortId[1]))
   {
      auto out = ut::log::error() << "sea_traffic: Port does not exist.";
      out.AddNote() << "Port: " << mPortId[1];
      return false;
   }
   return true;
}


//------------------------------------------------------------------------
//! Parses the ship ... end_ship block.
//! Returns true if successful. Throws an input exception if an error
//! is encountered.
//! aInput: The input object.
//------------------------------------------------------------------------
bool XWsfSeaTraffic::Vehicle::ProcessInput(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput, "end_ship");
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "fraction")
      {
         double fraction;
         aInput.ReadValue(fraction);
         aInput.ValueGreater(fraction, 0.0);
         mFraction = fraction;
      }
      else if (command == "mean_loiter_time")
      {
         double loiterTime;
         aInput.ReadValueOfType(loiterTime, UtInput::cTIME);
         mLoiterTime = loiterTime;
      }
      else if (command == "sigma_loiter_time")
      {
         double sigmaLoiterTime;
         aInput.ReadValueOfType(sigmaLoiterTime, UtInput::cTIME);
         mSigmaLoiterTime = sigmaLoiterTime;
      }
      else if (command == "loiter_route")
      {
         std::string routeName;
         aInput.ReadValue(routeName);
         mRouteIds.emplace_back(routeName);
      }
      else if (command == "mean_speed")
      {
         double speed;
         aInput.ReadValueOfType(speed, UtInput::cSPEED);
         aInput.ValueGreater(speed, 0.0);
         mSpeed = speed;
      }
      else if (command == "sigma_speed")
      {
         double sigmaSpeed;
         aInput.ReadValueOfType(sigmaSpeed, UtInput::cSPEED);
         aInput.ValueGreater(sigmaSpeed, 0.0);
         mSigmaSpeed = sigmaSpeed;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return true;
}

//------------------------------------------------------------------------
//! Verifies any data that can't be verified during the ProcessInput phase.
//! Returns true if successful.
//! aSeaTraffic: The sea traffic simulation.
//------------------------------------------------------------------------
bool XWsfSeaTraffic::Vehicle::Initialize(XWsfSeaTraffic& aSeaTraffic)
{
   // Make sure a fraction was specified.
   if (mFraction <= 0.0)
   {
      ut::log::error() << "sea_traffic: Must specify the fraction of " << mTypeId << "s to create.";
      return false;
   }

   // If no speed is specified, use the default value.
   if (mSpeed <= 0.0)
   {
      mSpeed = mDefaultSpeed;
   }

   // If no loiter time is specified, use the default value.
   if (mLoiterTime <= 0.0)
   {
      mLoiterTime = mDefaultLoiterTime;
   }

   // Make sure each of the defined loiter routes is valid.
   std::vector<WsfStringId>::const_iterator iter;
   for (iter = mRouteIds.begin(); iter != mRouteIds.end(); ++iter)
   {
      if (WsfRouteTypes::Get(aSeaTraffic.GetScenario()).Find(*iter) == nullptr)
      {
         auto out = ut::log::error() << "sea_traffic: Invalid loiter route.";
         out.AddNote() << "Route: " << *iter;
         return false;
      }
   }
   return true;
}
