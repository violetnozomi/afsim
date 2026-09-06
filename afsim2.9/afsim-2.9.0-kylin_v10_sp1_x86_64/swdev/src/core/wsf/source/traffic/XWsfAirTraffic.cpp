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

#include "traffic/XWsfAirTraffic.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <memory>
#include <sstream>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfAirMover.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfGroundMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTerrain.hpp"
#include "WsfWaypoint.hpp"
#include "WsfWaypointMover.hpp"

class XWsfAirTrafficExtension : public WsfScenarioExtension
{
public:
   XWsfAirTrafficExtension() {}

   bool ProcessInput(UtInput& aInput) override { return mData.ProcessInput(aInput); }

   void SimulationCreated(WsfSimulation& aSimulation) override
   {
      if (mData.IsRequested()) // only create when requested
      {
         aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<XWsfAirTraffic>(aSimulation, mData));
      }
   }

   XWsfAirTrafficData mData;
};

void WSF_EXPORT Register_air_traffic(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("air_traffic"))
   {
      aApplication.RegisterExtension("air_traffic",
                                     ut::make_unique<WsfDefaultApplicationExtension<XWsfAirTrafficExtension>>());
   }
}

XWsfAirTraffic::XWsfAirTraffic(WsfSimulation& aSimulation, XWsfAirTrafficData& aData)
   : XWsfAirTrafficData(aData)
   , mTakeOff("XWSF_AIR_TRAFFIC_TAKEOFF")
   , mAscending("XWSF_AIR_TRAFFIC_ASCENDING")
   , mCruising("XWSF_AIR_TRAFFIC_CRUISING")
   , mDescending("XWSF_AIR_TRAFFIC_DESCENDING")
   , mTouchdown("XWSF_AIR_TRAFFIC_TOUCHDOWN")
   , mComplete("XWSF_AIR_TRAFFIC_COMPLETE")
   , mEnterLoiter("XWSF_AIR_TRAFFIC_ENTER_LOITER")
   , mGoHome("XWSF_AIR_TRAFFIC_GO_HOME")
   , mEveryoneLand(0)
{
}

// virtual
XWsfAirTraffic::~XWsfAirTraffic() {}

/** Initialize the air traffic object.

  This must be invoked AFTER WsfSimulation::Initialize() as it adds events and
  platforms to the simulation.

  @return true  if initialization successfully completed.
  @return false if initialization could not be successfully completed.
*/

bool XWsfAirTraffic::Initialize()
{
   bool initializeOk = true;

   wsf::Terrain terrain(GetSimulation().GetTerrainInterface());
   // Compute critical points for the runway.

   mFlights.clear();
   mEveryoneLand = 0;

   // Make sure the destinations and aircraft types in each airbase are valid...

   Airbases::iterator airbaseIter;
   for (airbaseIter = mAirbases.begin(); airbaseIter != mAirbases.end(); ++airbaseIter)
   {
      unsigned int i;

      Airbase* airbasePtr = &airbaseIter->second;

      ComputeRunwayParameters(airbasePtr->mRunway);

      // Make sure the aircraft types are defined.
      for (i = 0; i < airbasePtr->mAircrafts.size(); ++i)
      {
         if (!DoesAircraftTypeExist(airbasePtr->mAircrafts[i].mAircraftTypeId))
         {
            auto out = ut::log::error() << "Invalid 'aircraft_type'.";
            out.AddNote() << "Airbase: " << airbasePtr->mNameId;
            out.AddNote() << "Aircraft Type: " << airbasePtr->mAircrafts[i].mAircraftTypeId;
            initializeOk = false;
         }
      }

      // Make sure the destinations are defined.
      for (i = 0; i < airbasePtr->mDestinations.size(); ++i)
      {
         if (!DoesAirbaseExist(airbasePtr->mDestinations[i].mAirbaseNameId))
         {
            auto out = ut::log::error() << "Undefined airbase destination.";
            out.AddNote() << "Airbase: " << airbasePtr->mNameId;
            out.AddNote() << "Destination: " << airbasePtr->mDestinations[i].mAirbaseNameId;
            initializeOk = false;
         }
      }
   }

   // Initialize airbase data and compute time the next update is required...

   double nextEventTime = std::numeric_limits<double>::max();
   for (airbaseIter = mAirbases.begin(); airbaseIter != mAirbases.end(); ++airbaseIter)
   {
      Airbase* airbasePtr = &airbaseIter->second;

      airbasePtr->mIsActive          = true;
      airbasePtr->mFlightNumber      = 0;
      airbasePtr->mNextDepartureTime = 0.0;
      if (airbasePtr->mIsActive)
      {
         if (!airbasePtr->mDestinations.empty())
         {
            nextEventTime = std::min(nextEventTime, airbasePtr->mNextDepartureTime);
         }
         nextEventTime = std::min(nextEventTime, airbasePtr->mDeactivationTime);
      }

      // Generate a simulation platform for each of the airbases so they can be visualized.

      WsfPlatform* platformPtr = new WsfPlatform(GetScenario());
      platformPtr->SetName(airbasePtr->mNameId);
      platformPtr->SetType("Airbase");
      platformPtr->SetIcon("Runway");
      platformPtr->SetSide("green");
      WsfGroundMover* moverPtr = new WsfGroundMover(GetScenario());
      moverPtr->SetIsOnRoad(true);
      platformPtr->SetMover(moverPtr);
      WsfWaypoint waypoint;
      waypoint.SetLat(airbasePtr->mRunway.mMidLat);
      waypoint.SetLon(airbasePtr->mRunway.mMidLon);
      waypoint.SetHeading(airbasePtr->mRunway.mHeading * UtMath::cRAD_PER_DEG);
      WsfRoute route;
      route.Append(waypoint);
      moverPtr->SetRoute(GetSimulation().GetSimTime(), route);
      if (!GetSimulation().AddPlatform(platformPtr))
      {
         auto out = ut::log::error() << "Unable to add airbase platform.";
         out.AddNote() << "Platform: " << platformPtr->GetName();
         delete platformPtr;
         initializeOk = false;
      }
   }

   // Schedule the event that will deactivate or takeoff as required...

   if (nextEventTime < (0.1 * std::numeric_limits<double>::max()))
   {
      nextEventTime = std::min(nextEventTime, mEveryoneLandTime);
      GetSimulation().AddEvent(ut::make_unique<AirTrafficEvent>(nextEventTime, this));
   }

   if (!initializeOk)
   {
      throw WsfSimulation::InitializeError();
   }

   return initializeOk;
}

/** Process airbase deactivations and takeoffs as required.

  This method is called via XWsfAirTraffic::AirTrafficEvent::Execute() when
  the event is dispatched by the simulation.

  The return value should be the time the event should be rescheduled (i.e.:
  the time of the NEXT scheduled takeoff or airbase deactivation.
*/

double XWsfAirTraffic::ProcessEvent(double aSimTime)
{
   // Handle flight diversions.
   ProcessDivertFlights(aSimTime);

   // Check for departures.
   Airbases::iterator airbaseIter;
   for (airbaseIter = mAirbases.begin(); airbaseIter != mAirbases.end(); ++airbaseIter)
   {
      Airbase* airbasePtr = &airbaseIter->second;

      // Process a departure.
      if (airbasePtr->mIsActive && (!airbasePtr->mDestinations.empty()) && (!mEveryoneLand) &&
          (aSimTime >= airbasePtr->mNextDepartureTime))


      {
         Aircraft    aircraft;
         Destination destination;

         bool local = DrawLocalOrRemote(*airbasePtr);
         if (local)
         {
            // Pick a destination.
            if (!GetRandomDestination(*airbasePtr, airbasePtr->mLocalDestinations, destination))
            {
               // If no destinations are available then simply try again in 10 minutes
               airbasePtr->mNextDepartureTime = aSimTime + 600.0;
               continue;
            }

            // Pick an aircraft.
            if (!GetRandomLocalAircraft(*airbasePtr, aircraft))
            {
               ut::log::error() << "Could not find a local aircraft type to use.";
               continue;
            }
         }
         else
         {
            // Pick a destination.
            if (!GetRandomDestination(*airbasePtr, airbasePtr->mDestinations, destination))
            {
               // If no destinations are available then simply try again in 10 minutes
               airbasePtr->mNextDepartureTime = aSimTime + 600.0;
               continue;
            }
            Airbase* destAirbasePtr = &mAirbases[destination.mAirbaseNameId];

            // Pick an aircraft.
            if (!GetRandomAircraft(*airbasePtr, *destAirbasePtr, aircraft))
            {
               auto out = ut::log::error() << "No aircraft type may land at destination.";
               out.AddNote() << "Destination: " << destination.mAirbaseNameId;
               continue;
            }
         }

         // Add the aircraft to the simulation.
         AddToSimulation(aircraft, local, *airbasePtr, destination, aSimTime);

         // Pick a departure interval.
         double departureInterval = -1.0;
         while (departureInterval <= 0.0)
         {
            // Allow 30 seconds either side of the departure interval.
            departureInterval = GetSimulation().GetRandom().Normal(airbasePtr->mDepartureInterval, 30.0);
         }
         airbasePtr->mNextDepartureTime = aSimTime + departureInterval;
      }
   }

   // Determine the next time we need to be called.
   double nextEventTime = std::numeric_limits<double>::max();
   for (airbaseIter = mAirbases.begin(); airbaseIter != mAirbases.end(); ++airbaseIter)
   {
      Airbase* airbasePtr = &airbaseIter->second;

      if (airbasePtr->mIsActive)
      {
         if ((!airbasePtr->mDestinations.empty()) && (!mEveryoneLand))
         {
            nextEventTime = std::min(nextEventTime, airbasePtr->mNextDepartureTime);
         }
         nextEventTime = std::min(nextEventTime, airbasePtr->mDeactivationTime);
      }
   }
   // Don't reschedule the event if nothing left to do.
   if (nextEventTime > (0.1 * std::numeric_limits<double>::max()))
   {
      nextEventTime = -1.0;
   }
   return nextEventTime;
}

void XWsfAirTraffic::ProcessDivertFlights(double aSimTime)
{
   // Check for deactivations...

   if ((aSimTime >= mEveryoneLandTime) && (!mEveryoneLand))
   {
      mEveryoneLand = 1;
      if (!mFlights.empty())
      {
         { // RAII block
            auto out = ut::log::info() << "Issuing order to land all flights.";
            out.AddNote() << "T = " << aSimTime;
         }
         for (auto& flight : mFlights)
         {
            // Divert every flight to closest base
            DivertFlight(aSimTime, flight);
         }
      }
   }

   bool               basesDeactivated = false;
   Airbases::iterator airbaseIter;
   for (airbaseIter = mAirbases.begin(); airbaseIter != mAirbases.end(); ++airbaseIter)
   {
      Airbase* airbasePtr = &airbaseIter->second;

      if (airbasePtr->mIsActive && (aSimTime >= airbasePtr->mDeactivationTime))
      {
         // Airport is going from active to inactive...
         auto out = ut::log::info() << "Deactivating airbase.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Airbase: " << airbasePtr->mNameId;
         airbasePtr->mIsActive = false;
         basesDeactivated      = true;
      }
   }

   // If any bases were deactivated then divert any flights heading to them..
   if (basesDeactivated)
   {
      for (auto& flight : mFlights)
      {
         Airbase* airbasePtr = &mAirbases[flight.mDestinationId];
         if (!airbasePtr->mIsActive)
         {
            // Base is inactive - reroute the aircraft...
            DivertFlight(aSimTime, flight);
         }
      }
   }
}

/** Process simulation input.

  Determine if the current command is one that pertains to this object.  If it
  is then we process the command and return 'true'.  If it isn't one of our
  commands then we simply return 'false'.
*/

bool XWsfAirTrafficData::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "air_traffic")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         std::string command(aInput.GetCommand());
         if ((command == "airbase") || (command == "airport"))
         {
            ProcessAirbaseBlock(aInput);
         }
         else if (command == "aircraft_type")
         {
            ProcessAircraftTypeBlock(aInput);
         }
         else if (command == "everyone_land_time")
         {
            aInput.ReadValueOfType(mEveryoneLandTime, UtInput::cTIME);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return myCommand;
}

/** Process a waypoint callback.

  This method is called via XWsfAirTraffic::UpdateFlightStatusCallback::Execute() when a
  waypoint event is encountered.  It will update the platform state as required.
*/

void XWsfAirTraffic::UpdateFlightStatus(double aSimTime, WsfPlatform* aPlatformPtr, WsfStringId& aFlightStatus)
{
   // Find the flight in the list of active platforms.

   std::vector<Flight>::iterator flightIter;
   for (flightIter = mFlights.begin(); flightIter != mFlights.end(); ++flightIter)
   {
      if (aPlatformPtr->GetIndex() == (*flightIter).mPlatformIndex)
      {
         break;
      }
   }

   if (flightIter == mFlights.end())
   {
      auto out = ut::log::error() << "Unable to locate flight for platform.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << aPlatformPtr->GetName();
      return;
   }

   Flight& flight = *flightIter;
   flight.mStatus = aFlightStatus;

   // If the flight is complete the delete the platform from the simulation and delete
   // the flight from our internal tables.

   if (aFlightStatus == mComplete)
   {
      GetSimulation().DeletePlatform(aSimTime, aPlatformPtr);
      mFlights.erase(flightIter);
   }
   else if (aFlightStatus == mCruising)
   {
      if (flight.mDiversionPending)
      {
         flight.mDiversionPending = false;
         GenerateDiversionPath(flight);
      }
   }
   else if (aFlightStatus == mEnterLoiter)
   {
      double        loiterTime      = 0.0;
      AircraftType* aircraftTypePtr = &mAircraftTypes[flight.mAircraftTypeId];
      if (aircraftTypePtr != nullptr)
      {
         loiterTime = GetRandomLoiterTime(*aircraftTypePtr);
      }
      GetSimulation().AddEvent(ut::make_unique<ExitLoiterEvent>(aPlatformPtr->GetNameId(), aSimTime + loiterTime));
   }
}

// private
void XWsfAirTraffic::AddCallback(WsfPlatform* aPlatformPtr, WsfStringId& aCallbackName)
{
   WsfCallback* callbackPtr = new UpdateFlightStatusCallback(this, aCallbackName);
   callbackPtr->SetName(aCallbackName);
   aPlatformPtr->AddComponent(callbackPtr);
}

/** Add a new flight to the simulation. */

// private
void XWsfAirTraffic::AddToSimulation(const Aircraft&    aAircraft,
                                     bool               aLocalAircraft,
                                     Airbase&           aAirbase,
                                     const Destination& aDestination,
                                     double             aSimTime)
{
   AircraftType& aircraftType = mAircraftTypes[aAircraft.mAircraftTypeId];

   std::string destinationString = "local";
   Airbase*    destinationPtr    = nullptr;
   if (!aLocalAircraft)
   {
      destinationPtr    = &mAirbases[aDestination.mAirbaseNameId];
      destinationString = destinationPtr->mNameId.GetString();
   }

   { // RAII block
      auto out = ut::log::info() << "Adding flight to simulation.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Airbase: " << aAirbase.mNameId;
      out.AddNote() << "Destination: " << destinationString;
      out.AddNote() << "Aircraft Type: " << aircraftType.mTypeId;
   }

   // Create the platform and mover...
   std::string  platformType = aircraftType.mTypeId.GetString();
   WsfPlatform* platformPtr  = (WsfPlatform*)GetScenario().CloneType("platform_type", platformType);
   if (platformPtr == nullptr)
   {
      auto out = ut::log::error() << "Unable to create aircraft of type.";
      out.AddNote() << "Aircraft Type: " << platformType;
      return;
   }

   WsfMover* moverPtr = platformPtr->GetMover();
   if (moverPtr == nullptr || !moverPtr->IsA_TypeOf("WSF_AIR_MOVER"))
   {
      auto out = ut::log::error() << "Aircraft mover must be a WSF_AIR_MOVER.";
      out.AddNote() << "Aircraft Type: " << platformType;
      out.AddNote() << "Mover Type: " << (moverPtr ? moverPtr->GetType() : "(None)");
      return;
   }

   // Create a name for the platform
   ++aAirbase.mFlightNumber;
   std::ostringstream oss;
   oss << platformType << '_' << aAirbase.mNameId << '_' << destinationString << '_' << aAirbase.mFlightNumber;
   platformPtr->SetName(oss.str());

   // Create the callbacks that will be triggered from waypoints.

   AddCallback(platformPtr, mTakeOff);
   AddCallback(platformPtr, mAscending);
   AddCallback(platformPtr, mCruising);
   AddCallback(platformPtr, mDescending);
   AddCallback(platformPtr, mTouchdown);
   AddCallback(platformPtr, mComplete);
   AddCallback(platformPtr, mEnterLoiter);

   // Select a cruise altitude

   float minAlt    = aircraftType.mMinimumCruiseAltitude;
   float maxAlt    = aircraftType.mMaximumCruiseAltitude;
   float cruiseAlt = GetSimulation().GetRandom().Uniform(minAlt, maxAlt);

   // Assuming the 'standard' 300 ft/nm ascent/descent rate, make sure we can get to the selected cruise
   // altitude before we get half-way to the destination.  If not then decrease the altitude.

   double destLat = aLocalAircraft ? aDestination.mLatitude : destinationPtr->mLat;
   double destLon = aLocalAircraft ? aDestination.mLongitude : destinationPtr->mLon;

   double heading;
   double distance;
   UtSphericalEarth::GreatCircleHeadingAndDistance(aAirbase.mLat, aAirbase.mLon, destLat, destLon, heading, distance);
   double distanceNM = distance / UtMath::cM_PER_NM;
   maxAlt            = static_cast<float>(0.5 * distanceNM * 300.0 /* ft/nm */ * UtMath::cM_PER_FT);
   if (cruiseAlt > maxAlt)
   {
      auto out = ut::log::info() << "Adjusting cruise altitude.";
      out.AddNote() << "Aircraft: " << platformPtr->GetName();
      out.AddNote() << "Old Altitude: " << cruiseAlt * UtMath::cFT_PER_M << " ft";
      out.AddNote() << "New Altitude: " << maxAlt * UtMath::cFT_PER_M << " ft";
      cruiseAlt = maxAlt;
   }

   // Select a cruise speed
   float cruiseSpeed = 0.0;
   while (cruiseSpeed <= 100.0)
   {
      cruiseSpeed = static_cast<float>(
         GetSimulation().GetRandom().Normal(aircraftType.mMeanCruiseSpeed, aircraftType.mSigmaCruiseSpeed));
   }

   // Generate the route...

   WsfRoute route;
   if (aLocalAircraft)
   {
      GenerateAscentPath(aAirbase, cruiseSpeed, route);
      GenerateLoiterPath(aircraftType.mRouteIds, aDestination, cruiseAlt, cruiseSpeed, route);
   }
   else
   {
      GenerateAscentPath(aAirbase, *destinationPtr, cruiseAlt, cruiseSpeed, route);
   }

   // Go from the final point to the destination
   const WsfWaypoint& waypoint = route[route.GetSize() - 1];

   unsigned int numWptsBeforeDescent = route.GetSize();
   if (aLocalAircraft)
   {
      GenerateDescentPath(route, waypoint.GetLat(), waypoint.GetLon(), cruiseAlt, cruiseSpeed, aAirbase);
   }
   else
   {
      GenerateDescentPath(route, waypoint.GetLat(), waypoint.GetLon(), cruiseAlt, cruiseSpeed, *destinationPtr);
   }

   // Mark our waypoint that we will exit to head home after loitering.
   if (numWptsBeforeDescent < route.GetSize())
   {
      route[numWptsBeforeDescent].SetLabelId(mGoHome);
   }

   platformPtr->GetMover()->SetRoute(GetSimulation().GetSimTime(), route);

   if (GetSimulation().AddPlatform(aSimTime, platformPtr))
   {
      Flight flight;
      flight.mPlatformIndex  = platformPtr->GetIndex();
      flight.mDestinationId  = aDestination.mAirbaseNameId;
      flight.mOriginationId  = aAirbase.mNameId;
      flight.mAircraftTypeId = aAircraft.mAircraftTypeId;
      flight.mCruiseAltitude = cruiseAlt;
      flight.mCruiseSpeed    = cruiseSpeed;
      flight.mStatus         = mTakeOff;
      mFlights.push_back(flight);
   }
   else
   {
      auto out = ut::log::error() << "Unable to add platform to simulation.";
      out.AddNote() << "Platform: " << platformPtr->GetName();
      delete platformPtr;
   }
}

/** Compute the signed heading change to go from one heading to another.
 *
 * @param aCurrentHeading current heading in range [0, 360).
 * @param aTargetHeading target heading in range [0, 360).
 *
 * @return heading change in the range [-180, 180].
 *         The value is positive for a right turn and negative for a left turn.
 */

double XWsfAirTraffic::ComputeHeadingChange(double aCurrentHeading, double aTargetHeading) const
{
   double headingChange = NormalizeHeading(aTargetHeading) - NormalizeHeading(aCurrentHeading);
   if (headingChange > 180.0)
   {
      headingChange -= 360.0;
   }
   if (headingChange < -180.0)
   {
      headingChange += 360.0;
   }
   return headingChange;
}

/** Compute the critical parameters for a runway.
 *
 * @param aRunway the runway whose parameters are to be computed.
 */

// private
void XWsfAirTraffic::ComputeRunwayParameters(Runway& aRunway)
{
   // FAA Terminology:
   //
   // MAP: Missed Approach Point.  This is the runway threshold.
   //
   // FAF: Final Approach Fix.  5 nm out from the MAP.
   //
   // Final Segment: the segment from FAF to MAP.
   //
   // IAF:  Intermediate Approach Fix.  There may be more than one of these points,
   //       but the final one is at 10 nm out from the MAP.
   //
   // Intermediate Segment: The segment from the final IAF to FAF.
   //
   // Also, the glide slope is nominally 3 degrees.
   //
   // All other descents assume 300 ft/nm which corresponds to the midpoint of the FAA recommended.
   // Get the height at each end of the runway.

   // Get the terrain height at each end.

   wsf::Terrain terrain(GetSimulation().GetTerrainInterface());
   terrain.GetElevInterp(aRunway.mBegLat, aRunway.mBegLon, aRunway.mBegHeight);
   terrain.GetElevInterp(aRunway.mEndLat, aRunway.mEndLon, aRunway.mEndHeight);

   // Compute the heading and length of the runway.

   UtSphericalEarth::GreatCircleHeadingAndDistance(aRunway.mBegLat,
                                                   aRunway.mBegLon,
                                                   aRunway.mEndLat,
                                                   aRunway.mEndLon,
                                                   aRunway.mHeading,
                                                   aRunway.mLength);

   // Compute the midpoint of the runway.

   UtSphericalEarth::ExtrapolateGreatCirclePosition(aRunway.mBegLat,
                                                    aRunway.mBegLon,
                                                    aRunway.mHeading,
                                                    0.5 * aRunway.mLength,
                                                    aRunway.mMidLat,
                                                    aRunway.mMidLon);

   double refLat  = aRunway.mBegLat;
   double refLon  = aRunway.mBegLon;
   double heading = NormalizeHeading(aRunway.mHeading + 180.0);
   double distance;

   // Compute the FAF (Final Approach Fix) that starts the 'final segment'.

   distance = 5.0 /* nm */ * UtMath::cM_PER_NM;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(refLat, refLon, heading, distance, aRunway.mLatFAF, aRunway.mLonFAF);
   aRunway.mAltFAF = static_cast<float>(aRunway.mBegHeight + (distance * tan(3.0 * UtMath::cRAD_PER_DEG)));

   // Compute the IAF (Intermediate Approach Fix) that starts the 'intermediate segment'.

   distance = 10.0 /* nm */ * UtMath::cM_PER_NM;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(refLat, refLon, heading, distance, aRunway.mLatIAF, aRunway.mLonIAF);
   aRunway.mAltIAF = static_cast<float>(aRunway.mBegHeight + (distance * tan(3.0 * UtMath::cRAD_PER_DEG)));

   // Compute the IAF's for the right and left base legs.

   refLat   = aRunway.mLatIAF;
   refLon   = aRunway.mLonIAF;
   distance = 5.0 /* nm */ * UtMath::cM_PER_NM;

   // Right base leg

   heading = NormalizeHeading(aRunway.mHeading + 90.0);
   UtSphericalEarth::ExtrapolateGreatCirclePosition(refLat, refLon, heading, distance, aRunway.mLatRightIAF, aRunway.mLonRightIAF);

   // Left  base leg

   heading = NormalizeHeading(aRunway.mHeading - 90.0);
   UtSphericalEarth::ExtrapolateGreatCirclePosition(refLat, refLon, heading, distance, aRunway.mLatLeftIAF, aRunway.mLonLeftIAF);
}

/** Divert a flight to another airbase. */

// private
void XWsfAirTraffic::DivertFlight(double aSimTime, Flight& aFlight)
{
   double currentLat;
   double currentLon;
   double currentAlt;
   double heading;
   double range;

   // We probably won't get here on a 'complete' flight, but just in case of an event timing
   // problem we'll just ignore it...

   if (aFlight.mStatus == mComplete)
   {
      return;
   }

   // We don't divert a flight once it has touched down and is rolling out.  I guess we
   // could institute a 'touch-and-go', but we'll just ignore it for now...

   if (aFlight.mStatus == mTouchdown)
   {
      return;
   }

   WsfPlatform* platformPtr = GetSimulation().GetPlatformByIndex(aFlight.mPlatformIndex);
   if (platformPtr == nullptr)
   {
      auto out = ut::log::info() << "Diverting unknown flight.";
      out.AddNote() << "Platform Index: " << aFlight.mPlatformIndex;
      return;
   }

   // Vector aircraft to closest airbase with sufficient runway size

   platformPtr->GetLocationLLA(currentLat, currentLon, currentAlt);

   WsfStringId bestAirbaseId;
   float       bestRange     = 9999999999.0F;
   float       minRunwaySize = mAircraftTypes[aFlight.mAircraftTypeId].mMinimumRunwayLength;

   Airbases::iterator airbaseIter;
   for (airbaseIter = mAirbases.begin(); airbaseIter != mAirbases.end(); ++airbaseIter)
   {
      Airbase* airbasePtr = &airbaseIter->second;

      float runwaySize = static_cast<float>(airbasePtr->mRunway.mLength);
      if ((airbasePtr->mIsActive) && (runwaySize >= minRunwaySize))
      {
         // meets criteria - check range
         UtSphericalEarth::GreatCircleHeadingAndDistance(currentLat,
                                                         currentLon,
                                                         airbasePtr->mLat,
                                                         airbasePtr->mLon,
                                                         heading,
                                                         range);
         if (range < bestRange)
         {
            // found the current best airbase
            bestRange     = static_cast<float>(range);
            bestAirbaseId = airbasePtr->mNameId;
         }
      }
   }
   // Make sure we found at least one airbase

   if (bestAirbaseId == nullptr)
   {
      auto out = ut::log::error() << "Diverting flight. Could not find a suitable alternative airbase.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Flight: " << platformPtr->GetName();
      out.AddNote() << "Status:" << aFlight.mStatus;
      out.AddNote() << "Destination: " << mAirbases[aFlight.mDestinationId].mNameId;
      return; // TODO - just let it keep going for now!
   }

   if (aFlight.mDestinationId != bestAirbaseId)
   {
      auto out = ut::log::info() << "Diverting flight.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Flight: " << platformPtr->GetName();
      out.AddNote() << "Status: " << aFlight.mStatus;
      out.AddNote() << "Old Destination: " << mAirbases[aFlight.mDestinationId].mNameId;
      out.AddNote() << "New Destination: " << mAirbases[bestAirbaseId].mNameId;

      aFlight.mDestinationId = bestAirbaseId;

      if ((aFlight.mStatus == mTakeOff) || (aFlight.mStatus == mAscending))
      {
         out.AddNote() << "Diversion will be delayed until ascent is complete.";
         aFlight.mDiversionPending = true;
      }
      else
      {
         out.Send();
         GenerateDiversionPath(aFlight);
      }
   }
   else
   {
      auto out = ut::log::info() << "Platform is continuing flight to original destination.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << platformPtr->GetName();
      out.AddNote() << "Status:" << aFlight.mStatus;
      out.AddNote() << "Destination: " << mAirbases[aFlight.mDestinationId].mNameId;
   }
}

/** Generate an ascent path from the originating airbase. */

// private
void XWsfAirTraffic::GenerateAscentPath(const Airbase& aOrigination,
                                        const Airbase& aDestination,
                                        float          aCruiseAlt,
                                        float          aCruiseSpeed,
                                        WsfRoute&      aRoute)
{
   static double takeOffDistance = 5000.0 /* ft */ * UtMath::cM_PER_FT;          // m
   static double takeOffSpeed    = 150.0 /* kts */ * UtMath::cM_PER_NM / 3600.0; // m/s
   static double maneuverSpeed   = 200.0 /* kts */ * UtMath::cM_PER_NM / 3600.0; // m/s
   static double climbSpeed      = 250.0 /* kts */ * UtMath::cM_PER_NM / 3600.0; // m/s

   WsfWaypoint waypoint;
   double      latRef  = aOrigination.mRunway.mBegLat;
   double      lonRef  = aOrigination.mRunway.mBegLon;
   double      heading = aOrigination.mRunway.mHeading;
   double      distance;

   // Start of takeoff roll

   waypoint.SetLat(aOrigination.mRunway.mBegLat);
   waypoint.SetLon(aOrigination.mRunway.mBegLon);
   waypoint.SetAlt(2.0);
   waypoint.SetAltRef(WsfPath::cALT_REF_AGL);
   waypoint.SetSpeed(10.0);
   aRoute.Append(waypoint);

   // Liftoff

   distance = takeOffDistance;
   double lat, lon;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(latRef, lonRef, heading, distance, lat, lon);
   waypoint.SetLat(lat);
   waypoint.SetLon(lon);
   waypoint.SetSpeed(takeOffSpeed);
   waypoint.SetScriptId(mAscending);
   aRoute.Append(waypoint);
   waypoint.SetScriptId(nullptr);

   // Continue to go straight, climbing at a 3-deg angle to an altitude of 1000 ft
   // and accelerating to the maneuvering speed.

   double airbaseHeight = 0.5 * (aOrigination.mRunway.mBegHeight + aOrigination.mRunway.mEndHeight);
   double alt1kft       = 1000.0 /* ft */ * UtMath::cM_PER_FT;

   distance = takeOffDistance + alt1kft / tan(3.0 * UtMath::cRAD_PER_DEG);
   UtSphericalEarth::ExtrapolateGreatCirclePosition(latRef, lonRef, heading, distance, lat, lon);
   waypoint.SetLat(lat);
   waypoint.SetLon(lon);
   waypoint.SetAltRef(WsfPath::cALT_REF_MSL);
   waypoint.SetAlt(airbaseHeight + alt1kft);
   waypoint.SetSpeed(maneuverSpeed);
   // When we hit this point we will start turning.  Compute the radial acceleration
   // that will give us a standard turn (360.0 deg in 2 min = 3 deg/sec)
   waypoint.SetRadialAccel(2.0 * UtMath::cPI * waypoint.GetSpeed() / 120.0);
   aRoute.Append(waypoint);

   // Determine the target point at the destination airbase.  We'll try for a straight-in approach
   // and if that is not possible then we'll select either the left or right base.
   //
   // This probably isn't done this way in real life... Such corrections are probably made mid-flight.
   // Doing it this way, however, eliminates turns in shorter flights.

   double latDest = aDestination.mRunway.mLatIAF;
   double lonDest = aDestination.mRunway.mLonIAF;
   double tempHeading;
   double tempDistance;
   UtSphericalEarth::GreatCircleHeadingAndDistance(waypoint.GetLat(), waypoint.GetLon(), latDest, lonDest, tempHeading, tempDistance);
   double headingChange = ComputeHeadingChange(tempHeading, aDestination.mRunway.mHeading);
   if (headingChange > 90.0)
   {
      latDest = aDestination.mRunway.mLatRightIAF;
      lonDest = aDestination.mRunway.mLonRightIAF;
   }
   else if (headingChange < -90.0)
   {
      latDest = aDestination.mRunway.mLatLeftIAF;
      lonDest = aDestination.mRunway.mLonLeftIAF;
   }

   // Compute the reference point for the turn as though the lead turn starts at this waypoint.

   UtSphericalEarth::GreatCircleHeadingAndDistance(waypoint.GetLat(), waypoint.GetLon(), latDest, lonDest, tempHeading, tempDistance);
   headingChange     = fabs(ComputeHeadingChange(heading, tempHeading));
   double turnRadius = 120.0 * maneuverSpeed / (2.0 * UtMath::cPI);
   distance          = turnRadius * tan(0.5 * headingChange * UtMath::cRAD_PER_DEG);
   UtSphericalEarth::ExtrapolateGreatCirclePosition(waypoint.GetLat(), waypoint.GetLon(), heading, distance, latRef, lonRef);

   // Compute the new heading from the reference point.

   UtSphericalEarth::GreatCircleHeadingAndDistance(latRef, lonRef, latDest, lonDest, heading, distance);

   // Turn towards the target location and climb to 10kft at the climb speed.
   //
   // I (JAJ) saw in one manual that standard air traffic control assumes a descent rate of
   // 250-350 ft/nm.  If we assume the midpoint of 300 ft/nm and figuring that we have to
   // make up 9000 ft, it will take us 30 nm to get to 10000 ft.

   double alt10kft = 10000.0 /* ft */ * UtMath::cM_PER_FT;
   distance        = 30.0 /* nm */ * UtMath::cM_PER_NM;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(latRef, lonRef, heading, distance, lat, lon);
   waypoint.SetLat(lat);
   waypoint.SetLon(lon);
   waypoint.SetAlt(alt10kft);
   waypoint.SetSpeed(climbSpeed);
   waypoint.SetRadialAccel(2.0 * UtMath::cPI * waypoint.GetSpeed() / 120.0);
   waypoint.SetLinearAccel(UtEarth::cACCEL_OF_GRAVITY); // Assume 1g linear acceleration
   aRoute.Append(waypoint);

   // Increase speed and climb to cruising speed and altitude.

   if (aCruiseAlt > alt10kft)
   {
      double deltaAltFT      = (aCruiseAlt - alt10kft) * UtMath::cFT_PER_M;
      double deltaDistanceNM = deltaAltFT / 300.0 /* ft/nm */;
      distance += deltaDistanceNM * UtMath::cM_PER_NM;
   }
   else
   {
      distance += 3000.0;
   }
   UtSphericalEarth::ExtrapolateGreatCirclePosition(latRef, lonRef, heading, distance, lat, lon);
   waypoint.SetLat(lat);
   waypoint.SetLon(lon);
   waypoint.SetAlt(aCruiseAlt);
   waypoint.SetSpeed(aCruiseSpeed);
   waypoint.SetRadialAccel(2.0 * UtMath::cPI * waypoint.GetSpeed() / 120.0);
   waypoint.SetLinearAccel(UtEarth::cACCEL_OF_GRAVITY); // Assume 1g linear acceleration
   waypoint.SetScriptId(mCruising);
   aRoute.Append(waypoint);
}

void XWsfAirTraffic::GenerateAscentPath(const Airbase& aAirbase, float aCruiseSpeed, WsfRoute& aRoute)
{
   static double takeOffDistance = 5000.0 * UtMath::cM_PER_FT; // m
   static double takeOffSpeed    = aCruiseSpeed * 0.50;        // 50% cruise speed m/s
   static double maneuverSpeed   = aCruiseSpeed * 0.75;        // 75% cruise speed m/s

   WsfWaypoint waypoint;
   double      latRef  = aAirbase.mRunway.mBegLat;
   double      lonRef  = aAirbase.mRunway.mBegLon;
   double      heading = aAirbase.mRunway.mHeading;
   double      lat, lon, distance;

   // Start of takeoff roll
   waypoint.SetLat(aAirbase.mRunway.mBegLat);
   waypoint.SetLon(aAirbase.mRunway.mBegLon);
   waypoint.SetAlt(2.0);
   waypoint.SetAltRef(WsfPath::cALT_REF_AGL);
   waypoint.SetSpeed(0.0);
   aRoute.Append(waypoint);

   // Liftoff
   distance = takeOffDistance;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(latRef, lonRef, heading, distance, lat, lon);
   waypoint.SetLat(lat);
   waypoint.SetLon(lon);
   waypoint.SetSpeed(takeOffSpeed);
   // waypoint.SetScriptId(mAscending);
   aRoute.Append(waypoint);
   waypoint.SetScriptId(nullptr);

   // Continue to go straight, climbing at a 3-deg angle to an altitude of 1000 ft
   // and accelerating to the maneuvering speed.
   double airbaseHeight = 0.5 * (aAirbase.mRunway.mBegHeight + aAirbase.mRunway.mEndHeight);
   double alt1kft       = 1000.0 * UtMath::cM_PER_FT;

   distance = takeOffDistance + alt1kft / tan(3.0 * UtMath::cRAD_PER_DEG);
   UtSphericalEarth::ExtrapolateGreatCirclePosition(latRef, lonRef, heading, distance, lat, lon);
   waypoint.SetLat(lat);
   waypoint.SetLon(lon);
   waypoint.SetAltRef(WsfPath::cALT_REF_MSL);
   waypoint.SetAlt(airbaseHeight + alt1kft);
   waypoint.SetSpeed(maneuverSpeed);
   // When we hit this point we will start turning.  Compute the radial acceleration
   // that will give us a standard turn (360.0 deg in 2 min = 3 deg/sec)
   waypoint.SetRadialAccel(2.0 * UtMath::cPI * waypoint.GetSpeed() / 120.0);
   aRoute.Append(waypoint);
}

void XWsfAirTraffic::GenerateLoiterPath(const std::vector<WsfStringId>& aRouteIds,
                                        const Destination&              aDestination,
                                        float                           aCruiseAlt,
                                        float                           aCruiseSpeed,
                                        WsfRoute&                       aRoute)
{
   if (!aRouteIds.empty())
   {
      // Select a random loiter route.
      size_t    loiterRouteIndex = GetSimulation().GetRandom().Uniform<size_t>(0, aRouteIds.size() - 1);
      WsfRoute* loiterRoutePtr   = WsfRouteTypes::Get(GetScenario()).Clone(aRouteIds[loiterRouteIndex]);
      if (loiterRoutePtr != nullptr)
      {
         // Choose a location where the loiter route will be placed.
         WsfWaypoint wpt;
         GetRandomWaypoint(aDestination, wpt);

         // Choose an angle to rotate the path.
         double theta = GetSimulation().GetRandom().Uniform(0.0, 360.0);

         // Transform the loiter route to the location and by the rotation angle.
         loiterRoutePtr->Transform(wpt.GetLat(), wpt.GetLon(), theta);

         // Set the cruise altitude and speed.
         for (unsigned int wptId = 0; wptId < loiterRoutePtr->GetSize(); ++wptId)
         {
            (*loiterRoutePtr)[wptId].SetAlt(aCruiseAlt);
            (*loiterRoutePtr)[wptId].SetSpeed(aCruiseSpeed);
         }

         (*loiterRoutePtr)[0].SetScriptId(mEnterLoiter);
         aRoute.Append(*loiterRoutePtr);

         delete loiterRoutePtr;
         loiterRoutePtr = nullptr;
      }
   }
   else
   {
      WsfWaypoint wpt;
      GetRandomWaypoint(aDestination, wpt);
      wpt.SetAlt(aCruiseAlt);
      wpt.SetSpeed(aCruiseSpeed);
      aRoute.Append(wpt);
   }
}

/** Generate the descent path from the current point to the destination. */

void XWsfAirTraffic::GenerateDescentPath(WsfRoute&      aRoute,
                                         double         aCurrentLat,
                                         double         aCurrentLon,
                                         double         aCurrentAlt,
                                         double         aCurrentSpeed,
                                         const Airbase& aDestination)
{
   static double approachSpeed   = 250.0 /* kts */ * UtMath::cM_PER_NM / 3600.0;
   static double landingDistance = 6000.0 /* ft */ * UtMath::cM_PER_FT;          // m
   static double landingSpeed    = 150.0 /* kts */ * UtMath::cM_PER_NM / 3600.0; // m/s

   const Runway& runway = aDestination.mRunway;

   // Compute the turn radius for a standard rate turn on to the base/intermediate segments

   double turnRadius = 120.0 * approachSpeed / (2.0 * UtMath::cPI);

   WsfWaypoint waypoint;
   double      lat, lon;
   double      latRef;
   double      lonRef;
   double      heading;
   double      distance;

   // Determine if a 'straight-in' approach can be performed.  If it can't then determine if they
   // must use the right base or the left base.

   double latBase = runway.mLatIAF;
   double lonBase = runway.mLonIAF;
   double altBase = runway.mAltIAF;
   UtSphericalEarth::GreatCircleHeadingAndDistance(latBase, lonBase, aCurrentLat, aCurrentLon, heading, distance);
   double inboundHeading = NormalizeHeading(heading + 180.0);
   double headingChange  = ComputeHeadingChange(inboundHeading, runway.mHeading);

   // Assume a base segment is not needed.

   double finalHeading       = inboundHeading;
   double finalHeadingChange = fabs(headingChange);
   bool   needBaseSegment    = false;

   if (fabs(headingChange) > 90.0)
   {
      needBaseSegment = true;
      if (headingChange > 0.0)
      {
         // Need right base segment
         finalHeading = NormalizeHeading(runway.mHeading - 90.0);
         latRef       = runway.mLatRightIAF;
         lonRef       = runway.mLonRightIAF;
      }
      else
      {
         // Need left base segment
         finalHeading = NormalizeHeading(runway.mHeading + 90.0);
         latRef       = runway.mLatLeftIAF;
         lonRef       = runway.mLonLeftIAF;
      }
      finalHeadingChange = 90.0;

      // Lead the turn to where we start the base segment

      heading       = NormalizeHeading(inboundHeading + 180.0);
      headingChange = fabs(headingChange) - 90.0;
      distance      = turnRadius * tan(0.5 * headingChange * UtMath::cRAD_PER_DEG);
      UtSphericalEarth::ExtrapolateGreatCirclePosition(latRef, lonRef, heading, distance, latBase, lonBase);
   }

   // Compute descent path from current point to entry point of landing pattern.
   // If the altitude is below 10kft then we don't do anything.

   double alt10kft = 10000.0 * UtMath::cM_PER_FT;
   if (aCurrentAlt > alt10kft)
   {
      // We do this backwards to get the point at which we start descending...

      heading = NormalizeHeading(inboundHeading + 180.0);

      // Ascend from base altitude to 10kft

      double deltaAltFT      = (alt10kft - altBase) * UtMath::cFT_PER_M;
      double deltaDistanceNM = deltaAltFT / 300.0 /* ft/nm */;
      distance               = deltaDistanceNM * UtMath::cM_PER_NM;
      double distance10kft   = distance;

      // Ascend from 10kft to cruise altitude

      deltaAltFT      = (aCurrentAlt - alt10kft) * UtMath::cFT_PER_M;
      deltaDistanceNM = deltaAltFT / 300.0 /* ft/nm */;
      distance += deltaDistanceNM * UtMath::cM_PER_NM;

      // Put a waypoint where we start to descent from the altitude to 10kft.

      UtSphericalEarth::ExtrapolateGreatCirclePosition(latBase, lonBase, heading, distance, lat, lon);
      waypoint.SetLat(lat);
      waypoint.SetLon(lon);
      waypoint.SetAlt(aCurrentAlt);
      waypoint.SetSpeed(aCurrentSpeed);
      waypoint.SetRadialAccel(2.0 * UtMath::cPI * waypoint.GetSpeed() / 120.0);
      waypoint.SetScriptId(mDescending);
      aRoute.Append(waypoint);
      waypoint.SetScriptId(nullptr);

      // Put a waypoint where we cross 10kft and drop the speed to 250 kts.

      distance = distance10kft;
      UtSphericalEarth::ExtrapolateGreatCirclePosition(latBase, lonBase, heading, distance, lat, lon);
      waypoint.SetLat(lat);
      waypoint.SetLon(lon);
      waypoint.SetAlt(alt10kft);
      waypoint.SetSpeed(approachSpeed);
      waypoint.SetRadialAccel(2.0 * UtMath::cPI * waypoint.GetSpeed() / 120.0);
      aRoute.Append(waypoint);
   }

   if (needBaseSegment)
   {
      waypoint.SetLat(latBase);
      waypoint.SetLon(lonBase);
      waypoint.SetAlt(runway.mAltIAF);
      waypoint.SetSpeed(approachSpeed);
      waypoint.SetRadialAccel(2.0 * UtMath::cPI * waypoint.GetSpeed() / 120.0);
      aRoute.Append(waypoint);
   }

   // Start of the intermediate segment.  Note that we lead the turn...

   latRef   = runway.mLatIAF;
   lonRef   = runway.mLonIAF;
   heading  = NormalizeHeading(finalHeading + 180.0);
   distance = turnRadius * tan(0.5 * finalHeadingChange * UtMath::cRAD_PER_DEG);
   UtSphericalEarth::ExtrapolateGreatCirclePosition(latRef, lonRef, heading, distance, lat, lon);
   waypoint.SetLat(lat);
   waypoint.SetLon(lon);
   waypoint.SetAlt(runway.mAltIAF);
   waypoint.SetSpeed(approachSpeed);
   waypoint.SetRadialAccel(2.0 * UtMath::cPI * waypoint.GetSpeed() / 120.0);
   aRoute.Append(waypoint);

   // Start of the final segment

   waypoint.SetLat(runway.mLatFAF);
   waypoint.SetLon(runway.mLonFAF);
   waypoint.SetAlt(runway.mAltFAF);
   waypoint.SetSpeed(approachSpeed);
   waypoint.SetRadialAccel(2.0 * UtMath::cPI * waypoint.GetSpeed() / 120.0);
   aRoute.Append(waypoint);

   // Middle marker is at 3000 ft.

   latRef   = runway.mBegLat;
   lonRef   = runway.mBegLon;
   heading  = NormalizeHeading(runway.mHeading + 180.0);
   distance = 3000.0 /* ft */ * UtMath::cM_PER_FT;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(latRef, lonRef, heading, distance, lat, lon);
   waypoint.SetLat(lat);
   waypoint.SetLon(lon);
   waypoint.SetAlt(runway.mBegHeight + (distance * tan(3.0 * UtMath::cRAD_PER_DEG)));
   aRoute.Append(waypoint);

   // Inner marker is at 1000 ft.

   distance = 1000.0 /* ft */ * UtMath::cM_PER_FT;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(latRef, lonRef, heading, distance, lat, lon);
   waypoint.SetLat(lat);
   waypoint.SetLon(lon);
   waypoint.SetAlt(runway.mBegHeight + (distance * tan(3.0 * UtMath::cRAD_PER_DEG)));
   waypoint.SetSpeed(landingSpeed);
   aRoute.Append(waypoint);

   // Touchdown

   waypoint.SetLat(latRef);
   waypoint.SetLon(lonRef);
   waypoint.SetAlt(2.0);
   waypoint.SetAltRef(WsfPath::cALT_REF_AGL);
   waypoint.SetSpeed(landingSpeed);
   waypoint.SetScriptId(mTouchdown);
   waypoint.SetLinearAccel((landingSpeed * landingSpeed) / (2 * landingDistance));
   aRoute.Append(waypoint);

   // Roll-out and stop

   distance = landingDistance;
   heading  = runway.mHeading;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(latRef, lonRef, heading, distance, lat, lon);
   waypoint.SetLat(lat);
   waypoint.SetLon(lon);
   waypoint.SetAlt(2.0);
   waypoint.SetSpeed(0.5); // A waypoint with zero velocity will never be reached with current route mover
   waypoint.SetScriptId(mComplete);
   aRoute.Append(waypoint);
}

/** Compute the diversion path from the current point to touchdown */

// private
void XWsfAirTraffic::GenerateDiversionPath(Flight& aFlight)
{
   WsfPlatform* platformPtr = GetSimulation().GetPlatformByIndex(aFlight.mPlatformIndex);
   if (platformPtr == nullptr)
   {
      return;
   }

   Airbase*    airbasePtr = &mAirbases[aFlight.mDestinationId];
   WsfRoute    route;
   WsfWaypoint waypoint;
   double      heading;
   double      distance;
   double      lat;
   double      lon;
   double      latRef;
   double      lonRef;

   double currentLat;
   double currentLon;
   double currentAlt;
   platformPtr->GetLocationLLA(currentLat, currentLon, currentAlt);
   double tempPitch;
   double tempRoll;
   platformPtr->GetOrientationNED(heading, tempPitch, tempRoll);
   double currentHeading = NormalizeHeading(heading * UtMath::cDEG_PER_RAD);

   // Calculate radial acceleration for standard rate turn at cruise speed.

   double turnRadius = 120.0 * aFlight.mCruiseSpeed / (2.0 * UtMath::cPI);

   // Add a waypoint along path to go back to cruise alt and vel

   if (aFlight.mStatus != mCruising)
   {
      double deltaAltFT      = fabs(currentAlt - aFlight.mCruiseAltitude) * UtMath::cFT_PER_M;
      double deltaDistanceNM = deltaAltFT / 300.0 /* ft/nm */;
      distance               = deltaDistanceNM * UtMath::cM_PER_NM;
      UtSphericalEarth::ExtrapolateGreatCirclePosition(currentLat, currentLon, currentHeading, distance, lat, lon);
      waypoint.SetLat(lat);
      waypoint.SetLon(lon);
      waypoint.SetAlt(aFlight.mCruiseAltitude);
      waypoint.SetSpeed(aFlight.mCruiseSpeed);
      waypoint.SetRadialAccel(2.0 * UtMath::cPI * waypoint.GetSpeed() / 120.0);
      waypoint.SetLinearAccel(UtEarth::cACCEL_OF_GRAVITY); // Assume 1g linear acceleration
      route.Append(waypoint);

      // Recompute the heading...

      UtSphericalEarth::GreatCircleHeadingAndDistance(waypoint.GetLat(),
                                                      waypoint.GetLon(),
                                                      currentLat,
                                                      currentLon,
                                                      currentHeading,
                                                      distance);
      currentHeading = NormalizeHeading(currentHeading + 180.0);
   }
   else
   {
      UtSphericalEarth::ExtrapolateGreatCirclePosition(currentLat, currentLon, currentHeading, 500.0, lat, lon);
      waypoint.SetLat(lat);
      waypoint.SetLon(lon);
      waypoint.SetAlt(aFlight.mCruiseAltitude);
      waypoint.SetSpeed(aFlight.mCruiseSpeed);
      waypoint.SetRadialAccel(2.0 * UtMath::cPI * waypoint.GetSpeed() / 120.0);
      route.Append(waypoint);
   }
   currentLat = waypoint.GetLat();
   currentLon = waypoint.GetLon();
   currentAlt = waypoint.GetAlt();

   // Turn towards IAF of target airbase

   UtSphericalEarth::GreatCircleHeadingAndDistance(currentLat,
                                                   currentLon,
                                                   airbasePtr->mRunway.mLatIAF,
                                                   airbasePtr->mRunway.mLonIAF,
                                                   heading,
                                                   distance);
   double headingChange = ComputeHeadingChange(currentHeading, heading);

   // If the heading change is too much then turn 90 degrees and try again.

   if (fabs(headingChange) > 135.0)
   {
      if (headingChange > 0.0)
      {
         // Turn right
         heading = NormalizeHeading(currentHeading + 90.0);
      }
      else
      {
         // Turn left
         heading = NormalizeHeading(currentHeading - 90.0);
      }
      headingChange = 90.0;
      distance      = turnRadius * tan(0.5 * fabs(headingChange) * UtMath::cRAD_PER_DEG);
      UtSphericalEarth::ExtrapolateGreatCirclePosition(currentLat, currentLon, currentHeading, distance, latRef, lonRef);

      UtSphericalEarth::ExtrapolateGreatCirclePosition(latRef, lonRef, heading, 10000.0, lat, lon);
      waypoint.SetLat(lat);
      waypoint.SetLon(lon);
      route.Append(waypoint);
      currentLat = waypoint.GetLat();
      currentLon = waypoint.GetLon();
      UtSphericalEarth::GreatCircleHeadingAndDistance(waypoint.GetLat(), waypoint.GetLon(), latRef, lonRef, currentHeading, distance);
      currentHeading = NormalizeHeading(currentHeading + 180.0);

      // Recompute the heading change to the target IAF.

      UtSphericalEarth::GreatCircleHeadingAndDistance(currentLat,
                                                      currentLon,
                                                      airbasePtr->mRunway.mLatIAF,
                                                      airbasePtr->mRunway.mLonIAF,
                                                      heading,
                                                      distance);
      headingChange = ComputeHeadingChange(currentHeading, heading);
   }

   // Compute lead distance...
   distance = turnRadius * tan(0.5 * fabs(headingChange) * UtMath::cRAD_PER_DEG);
   UtSphericalEarth::ExtrapolateGreatCirclePosition(currentLat, currentLon, currentHeading, distance, latRef, lonRef);

   // Compute bearing and range from reference point to target point.

   double bearingToDest;
   double rangeToDest;
   UtSphericalEarth::GreatCircleHeadingAndDistance(latRef,
                                                   lonRef,
                                                   airbasePtr->mRunway.mLatIAF,
                                                   airbasePtr->mRunway.mLonIAF,
                                                   bearingToDest,
                                                   rangeToDest);

   // Compute the heading change needed go towards to target point..

   headingChange     = ComputeHeadingChange(currentHeading, bearingToDest);
   double newHeading = NormalizeHeading(currentHeading + headingChange);

   // Determine the center point of the turn and turn exit point

   double centerPointBearing = 0.0;
   double exitPointBearing   = 0.0;
   if (headingChange >= 0.0)
   {
      // Turning right
      centerPointBearing = NormalizeHeading(currentHeading + 90.0);
      exitPointBearing   = NormalizeHeading(newHeading - 90.0);
   }
   else
   {
      // Turning left
      centerPointBearing = NormalizeHeading(currentHeading - 90.0);
      exitPointBearing   = NormalizeHeading(newHeading + 90.0);
   }

   double centerLat;
   double centerLon;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(currentLat, currentLon, centerPointBearing, turnRadius, centerLat, centerLon);

   double exitLat;
   double exitLon;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(centerLat, centerLon, exitPointBearing, turnRadius, exitLat, exitLon);

   // Extrapolate past the exit point so we can straighten out.

   UtSphericalEarth::ExtrapolateGreatCirclePosition(exitLat, exitLon, newHeading, 1000.0, lat, lon);
   waypoint.SetLat(lat);
   waypoint.SetLon(lon);
   route.Append(waypoint);
   currentLat = waypoint.GetLat();
   currentLon = waypoint.GetLon();
   currentAlt = waypoint.GetAlt();
   GenerateDescentPath(route, currentLat, currentLon, currentAlt, waypoint.GetSpeed(), *airbasePtr);

   WsfAirMover* moverPtr = dynamic_cast<WsfAirMover*>(platformPtr->GetMover());
   if (moverPtr != nullptr)
   {
      moverPtr->UpdateRoute(GetSimulation().GetSimTime(), route);
   }
}

/** Normalize a heading to be in the range [0, 360) */

// private
double XWsfAirTrafficData::NormalizeHeading(double aHeading)
{
   return UtMath::NormalizeAngle0_360(aHeading);
}

/** Process the 'airbase' / 'end_airbase' block. */

// private
void XWsfAirTrafficData::ProcessAirbaseBlock(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput);

   Airbase     airbase;
   std::string airbaseName;
   aInput.ReadValue(airbaseName);
   airbase.mNameId.SetString(airbaseName);
   aInput.ReadValueOfType(airbase.mLat, UtInput::cLATITUDE);
   aInput.ReadValueOfType(airbase.mLon, UtInput::cLONGITUDE);

   double runwayLength  = 5000.0 * UtMath::cM_PER_FT;
   double runwayHeading = 0.0;
   bool   haveRunway    = false;

   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "aircraft")
      {
         Aircraft    aircraft;
         std::string aircraftType;
         aInput.ReadValue(aircraftType);
         aircraft.mAircraftTypeId.SetString(aircraftType);
         aInput.ReadValue(aircraft.mFraction);
         aInput.ValueInClosedRange(aircraft.mFraction, 0.0, 1.0);
         airbase.mAircrafts.push_back(aircraft);
      }
      else if (command == "deactivation_time")
      {
         aInput.ReadValueOfType(airbase.mDeactivationTime, UtInput::cTIME);
         aInput.ValueGreaterOrEqual(airbase.mDeactivationTime, 0.0);
      }
      else if (command == "departure_interval")
      {
         aInput.ReadValueOfType(airbase.mDepartureInterval, UtInput::cTIME);
         aInput.ValueGreater(airbase.mDepartureInterval, 0.0);
      }
      else if (command == "destination")
      {
         Destination destination;
         aInput.ReadValue(airbaseName);
         destination.mAirbaseNameId.SetString(airbaseName);
         aInput.ReadValue(destination.mFraction);
         aInput.ValueInClosedRange(destination.mFraction, 0.0, 1.0);
         airbase.mDestinations.push_back(destination);
      }
      else if (command == "local_destination")
      {
         Destination destination;
         aInput.ReadValueOfType(destination.mLatitude, UtInput::cLATITUDE);
         aInput.ReadValueOfType(destination.mLongitude, UtInput::cLONGITUDE);
         aInput.ReadValueOfType(destination.mRadius, UtInput::cLENGTH);
         destination.mRadius /= 2.0;
         aInput.ReadValue(destination.mFraction);
         aInput.ValueInClosedRange(destination.mFraction, 0.0, 1.0);
         airbase.mLocalDestinations.push_back(destination);
      }
      else if (command == "runway_heading")
      {
         aInput.ReadValue(runwayHeading);
         aInput.ValueInClosedRange(runwayHeading, 0.0, 360.0);
      }
      else if (command == "runway_length")
      {
         aInput.ReadValueOfType(runwayLength, UtInput::cLENGTH);
         aInput.ValueGreater(runwayLength, 0.0);
      }
      else if (command == "runway")
      {
         haveRunway = true;
         aInput.ReadValueOfType(airbase.mRunway.mBegLat, UtInput::cLATITUDE);
         aInput.ReadValueOfType(airbase.mRunway.mBegLon, UtInput::cLONGITUDE);
         aInput.ReadValueOfType(airbase.mRunway.mEndLat, UtInput::cLATITUDE);
         aInput.ReadValueOfType(airbase.mRunway.mEndLon, UtInput::cLONGITUDE);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   // If 'runway' not specified then generate a 'runway' from the airbase location and
   // the 'runway_heading' and 'runway_length'.

   if (!haveRunway)
   {
      UtSphericalEarth::ExtrapolateGreatCirclePosition(airbase.mLat,
                                                       airbase.mLon,
                                                       runwayHeading,
                                                       0.5 * runwayLength,
                                                       airbase.mRunway.mEndLat,
                                                       airbase.mRunway.mEndLon);
      runwayHeading = NormalizeHeading(runwayHeading + 180.0);
      UtSphericalEarth::ExtrapolateGreatCirclePosition(airbase.mLat,
                                                       airbase.mLon,
                                                       runwayHeading,
                                                       0.5 * runwayLength,
                                                       airbase.mRunway.mBegLat,
                                                       airbase.mRunway.mBegLon);
   }


   // Make sure destination fractions add to 1.0

   if (!airbase.mDestinations.empty())
   {
      double total = 0.0;
      for (auto& destination : airbase.mDestinations)
      {
         total += destination.mFraction;
      }

      if (fabs(total - 1.0) > 1.0E-6)
      {
         throw UtInput::BadValue(aInput, "Destination fractions must sum to 1.0");
      }
   }

   // Make sure the local destination fractions add to 1.0

   if (!airbase.mLocalDestinations.empty())
   {
      double total = 0.0;
      for (auto& localDestination : airbase.mLocalDestinations)
      {
         total += localDestination.mFraction;
      }

      if (fabs(total - 1.0) > 1.0E-6)
      {
         throw UtInput::BadValue(aInput, "Local destination fractions must sum to 1.0");
      }
   }


   // Make sure aircraft fractions add to 1.0

   if (!airbase.mAircrafts.empty())
   {
      double total = 0.0;
      for (auto& aircraft : airbase.mAircrafts)
      {
         total += aircraft.mFraction;
      }

      if (fabs(total - 1.0) > 1.0E-6)
      {
         throw UtInput::BadValue(aInput, "Aircraft fractions must sum to 1.0");
      }
   }
   mAirbases[airbase.mNameId] = airbase;
}

/** Process the 'aircraft_type' / 'end_aircraft_type' block. */

// private
void XWsfAirTrafficData::ProcessAircraftTypeBlock(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput);
   AircraftType aircraftType;
   std::string  aircraftTypeName;
   aInput.ReadValue(aircraftTypeName);
   aircraftType.mTypeId.SetString(aircraftTypeName);
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "minimum_cruise_altitude")
      {
         aInput.ReadValueOfType(aircraftType.mMinimumCruiseAltitude, UtInput::cLENGTH);
         aInput.ValueGreater(aircraftType.mMinimumCruiseAltitude, 0.0F);
      }
      else if (command == "maximum_cruise_altitude")
      {
         aInput.ReadValueOfType(aircraftType.mMaximumCruiseAltitude, UtInput::cLENGTH);
         aInput.ValueGreater(aircraftType.mMaximumCruiseAltitude, 0.0F);
      }
      else if (command == "mean_cruise_speed")
      {
         aInput.ReadValueOfType(aircraftType.mMeanCruiseSpeed, UtInput::cSPEED);
         aInput.ValueGreater(aircraftType.mMeanCruiseSpeed, 0.0F);
      }
      else if (command == "sigma_cruise_speed")
      {
         aInput.ReadValueOfType(aircraftType.mSigmaCruiseSpeed, UtInput::cSPEED);
         aInput.ValueGreaterOrEqual(aircraftType.mSigmaCruiseSpeed, 0.0F);
      }
      else if (command == "maximum_operating_range")
      {
         aInput.ReadValueOfType(aircraftType.mMaximumOperatingRange, UtInput::cLENGTH);
         aInput.ValueGreater(aircraftType.mMaximumOperatingRange, 0.0F);
      }
      else if (command == "minimum_runway_length")
      {
         aInput.ReadValueOfType(aircraftType.mMinimumRunwayLength, UtInput::cLENGTH);
         aInput.ValueGreater(aircraftType.mMinimumRunwayLength, 0.0F);
      }
      else if (command == "local")
      {
         aircraftType.mLocal = true;
      }
      else if (command == "mean_loiter_time")
      {
         aInput.ReadValueOfType(aircraftType.mLoiterTime, UtInput::cTIME);
      }
      else if (command == "sigma_loiter_time")
      {
         aInput.ReadValueOfType(aircraftType.mSigmaLoiterTime, UtInput::cTIME);
      }
      else if (command == "loiter_route")
      {
         std::string loiterRoute;
         aInput.ReadValue(loiterRoute);
         aircraftType.mRouteIds.emplace_back(loiterRoute);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   mAircraftTypes[aircraftType.mTypeId] = aircraftType;

   // If no loiter time is specified, use the default value.
   if (aircraftType.mLoiterTime <= 0.0)
   {
      aircraftType.mLoiterTime = aircraftType.mDefaultLoiterTime;
   }
}

bool XWsfAirTraffic::GetRandomDestination(const Airbase& aAirbase, const Destinations& aDestinations, Destination& aDestination)
{
   unsigned int i;

   // Determine the available fraction of the destinations.
   double fractionAvailable = 0.0;

   for (i = 0; i < aDestinations.size(); ++i)
   {
      WsfStringId destAirbaseId = aDestinations[i].mAirbaseNameId;

      // If the destAirbaseId is 0, it means we are traveling to a local area
      // and we don't care whether the destination is active.
      bool ok = (!destAirbaseId) ? true : mAirbases[destAirbaseId].mIsActive;
      if (ok)
      {
         fractionAvailable += aDestinations[i].mFraction;
      }
   }

   // Now select a destination.
   if (fractionAvailable != 0.0)
   {
      double randomValue = GetSimulation().GetRandom().Uniform(0.0, fractionAvailable);
      double fractionSum = 0.0;
      for (i = 0; i < aDestinations.size(); ++i)
      {
         WsfStringId destAirbaseId = aDestinations[i].mAirbaseNameId;

         // If the destAirbaseId is 0, it means we are traveling to a local area
         // and we don't care whether the destination is active.
         bool ok = (!destAirbaseId) ? true : mAirbases[destAirbaseId].mIsActive;
         if (ok)
         {
            fractionSum += aDestinations[i].mFraction;
            if (randomValue <= fractionSum)
            {
               aDestination = aDestinations[i];
               return true;
            }
         }
      }
   }
   return false;
}

bool XWsfAirTraffic::GetRandomAircraft(const Airbase& aAirbase, const Airbase& aDestAirbase, Aircraft& aAircraft)
{
   unsigned int i;
   unsigned int j;

   // Determine distance from departure to destination
   double flightHeading;
   double flightDistance;
   UtSphericalEarth::GreatCircleHeadingAndDistance(aAirbase.mLat,
                                                   aAirbase.mLon,
                                                   aDestAirbase.mLat,
                                                   aDestAirbase.mLon,
                                                   flightHeading,
                                                   flightDistance);
   std::vector<bool> aircraftOK;
   double            fractionAvailable = 0.0;
   double            fractionSum       = 0.0;
   for (i = 0; i < aAirbase.mAircrafts.size(); ++i)
   {
      aircraftOK.push_back(false);
      for (j = 0; j < aDestAirbase.mAircrafts.size(); ++j)
      {
         if (aAirbase.mAircrafts[i].mAircraftTypeId == aDestAirbase.mAircrafts[j].mAircraftTypeId)
         {
            WsfStringId typeId   = aAirbase.mAircrafts[i].mAircraftTypeId;
            double      maxRange = (static_cast<double>(mAircraftTypes[typeId].mMaximumOperatingRange));
            if (maxRange >= flightDistance)
            {
               fractionAvailable += aAirbase.mAircrafts[i].mFraction;
               aircraftOK[i] = true;
            }
         }
      }
   }

   // Now select an aircraft type to launch
   double randomValue = GetSimulation().GetRandom().Uniform(0.0, fractionAvailable);
   fractionSum        = 0.0;
   for (i = 0; i < aAirbase.mAircrafts.size(); ++i)
   {
      if (aircraftOK[i])
      {
         fractionSum += aAirbase.mAircrafts[i].mFraction;
         if (randomValue <= fractionSum)
         {
            aAircraft = aAirbase.mAircrafts[i];
            return true;
         }
      }
   }
   return false;
}

bool XWsfAirTraffic::GetRandomLocalAircraft(const Airbase& aAirbase, Aircraft& aAircraft)
{
   unsigned int i;

   double fractionAvailable = 0.0;
   double fractionSum       = 0.0;
   for (i = 0; i < aAirbase.mAircrafts.size(); ++i)
   {
      WsfStringId typeId = aAirbase.mAircrafts[i].mAircraftTypeId;
      if (mAircraftTypes[typeId].mLocal)
      {
         fractionAvailable += aAirbase.mAircrafts[i].mFraction;
      }
   }

   // Now select an aircraft type to launch
   double randomValue = GetSimulation().GetRandom().Uniform(0.0, fractionAvailable);
   fractionSum        = 0.0;
   for (i = 0; i < aAirbase.mAircrafts.size(); ++i)
   {
      WsfStringId typeId = aAirbase.mAircrafts[i].mAircraftTypeId;
      if (mAircraftTypes[typeId].mLocal)
      {
         fractionSum += aAirbase.mAircrafts[i].mFraction;
         if (randomValue <= fractionSum)
         {
            aAircraft = aAirbase.mAircrafts[i];
            return true;
         }
      }
   }
   return false;
}

//! Returns true or a local aircraft and false if non-local.
//! The draw is weighted based on the fraction of each type of aircraft
//! and whether they are local or not.  For example, if we have the following
//! Aircraft A 30% non-local
//! Aircraft B 50% non-local
//! Aircraft C 20% local
//! then, this method will return true 20% of the time.
// private
bool XWsfAirTraffic::DrawLocalOrRemote(const Airbase& aAirbase)
{
   bool   local              = false;
   double uniformRandom      = GetSimulation().GetRandom().Uniform<double>();
   double cumulativeFraction = 0.0;

   std::vector<Aircraft>::const_iterator iter;
   for (iter = aAirbase.mAircrafts.begin(); iter != aAirbase.mAircrafts.end(); ++iter)
   {
      cumulativeFraction += iter->mFraction;
      if (uniformRandom < cumulativeFraction)
      {
         local = mAircraftTypes[iter->mAircraftTypeId].mLocal;
         break;
      }
   }
   return local;
}

void XWsfAirTraffic::GetRandomWaypoint(const Destination& aDestination, WsfWaypoint& aWaypoint)
{
   // Calculate a random point inside the region
   double theta  = GetSimulation().GetRandom().Uniform(0.0, 360.0);
   double radius = GetSimulation().GetRandom().Uniform(0.0, aDestination.mRadius);
   double lat, lon;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(aDestination.mLatitude, aDestination.mLongitude, theta, radius, lat, lon);
   aWaypoint.SetLat(lat);
   aWaypoint.SetLon(lon);
}

double XWsfAirTraffic::GetRandomLoiterTime(const AircraftType& aAircraftType)
{
   double loiterTime = -1.0;
   while (loiterTime < 0.0)
   {
      loiterTime = aAircraftType.mLoiterTime + (aAircraftType.mSigmaLoiterTime * GetSimulation().GetRandom().Gaussian());
   }
   return loiterTime;
}

bool XWsfAirTrafficData::IsRequested() const
{
   return !mAirbases.empty();
}
