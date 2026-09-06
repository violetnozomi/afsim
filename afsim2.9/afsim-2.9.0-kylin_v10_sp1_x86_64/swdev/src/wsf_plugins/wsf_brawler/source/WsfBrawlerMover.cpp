// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfBrawlerMover.hpp"

#include "BrawlerPlatform.hpp"
#include "UtAtmosphere.hpp"
#include "UtEarth.hpp"
#include "UtLog.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfBrawlerProcessor.hpp"
#include "WsfDraw.hpp"
#include "WsfPlatform.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

WsfBrawlerMover::WsfBrawlerMover(WsfScenario& aScenario)
   : WsfMover(aScenario)
   , BrawlerMover()
   , mDrawProjection(false)
   , mDrawPtr(nullptr)
   , mDebugEntityProjection()
   , mFuelPtr(nullptr)
   , mFlyingRoute(false)
   , mCurrentRouteIndex(0)
{
}

WsfBrawlerMover::WsfBrawlerMover(const WsfBrawlerMover& aSrc)
   : WsfMover(aSrc)
   , BrawlerMover(aSrc)
   , mDrawProjection(aSrc.mDrawProjection)
   , mDrawPtr(nullptr)
   , mDebugEntityProjection(aSrc.mDebugEntityProjection)
   , mFuelPtr(nullptr)
   , mRoute(aSrc.mRoute)
   , mFlyingRoute(aSrc.mFlyingRoute)
   , mCurrentRouteIndex(aSrc.mCurrentRouteIndex)
{
}

WsfBrawlerMover::~WsfBrawlerMover()
{
   delete mDrawPtr;
}


// virtual
WsfMover* WsfBrawlerMover::Clone() const
{
   return new WsfBrawlerMover(*this);
}

void WsfBrawlerMover::SwapMoverInitiated(double aSimTime, WsfMover* aOldMoverPtr)
{
   // Set the current platform time used in the Brawler code
   tsv = aSimTime;
}

// virtual
bool WsfBrawlerMover::Initialize(double aSimTime)
{
   bool ok = true;

   // try to find the WsfBrawlerProcessor (which derives from BrawlerPlatform)
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
   {
      WsfBrawlerProcessor* procPtr(dynamic_cast<WsfBrawlerProcessor*>(*iter));
      if (procPtr != nullptr)
      {
         mBrawlerPlatformPtr = procPtr;
         break;
      }
   }

   // if (mBrawlerPlatformPtr == 0)
   //{
   //    ut::log::warning() << GetPlatform()->GetName() << " does not have a WSF_BRAWLER_PROCESSOR on it!";
   // }

   // Get the pointer to a fuel object on the platform, if the fuel object exists and update the internal fuel quantity.
   // Otherwise, use brawler's internal representation of fuel
   if (GetPlatform()->FindByRole(mFuelPtr))
   {
      // Brawler gas uses slugs, GetInitialQuantity() returns kilograms
      gas = mFuelPtr->GetInitialQuantity() / UtMath::cKG_PER_SLUG;
   }

   ////////////////////////////////////////////////////////////////////////////
   if (mRoute.GetSize() > 0)
   {
      // first clear up any cDOUBLE_NOT_SET issues in the route
      double speedMPS  = 252.79999224; // brawler initial speed if not defined
      double altMeters = 3048.0;       //(10k ft)
      for (unsigned int i = 0; i < mRoute.GetSize(); ++i)
      {
         if (mRoute[i].GetAlt() == WsfPath::cUSE_PREVIOUS)
         {
            mRoute[i].SetAlt(altMeters);
         }
         else
         {
            altMeters = mRoute[i].GetAlt();
         }

         if (mRoute[i].GetSpeed() == WsfPath::cUSE_PREVIOUS)
         {
            mRoute[i].SetSpeed(speedMPS);
         }
         else
         {
            speedMPS = mRoute[i].GetSpeed();
         }
      }

      mFlyingRoute       = true;
      mCurrentRouteIndex = 0;
      WsfWaypoint start  = mRoute.GetWaypointAt(mCurrentRouteIndex);
      // initialize platform location, orientation, & speed
      if (mBrawlerPlatformPtr)
      {
         WsfBrawlerProcessor::Find(*GetPlatform())->SetLocationLLA(start.GetLat(), start.GetLon(), start.GetAlt());
      }

      double speed   = start.GetSpeed();
      double heading = 0.0;
      if (mRoute.GetSize() >= 2)
      {
         mCurrentRouteIndex = 1;

         WsfWaypoint& target = mRoute[mCurrentRouteIndex];
         if (target.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
         {
            double distance;
            UtSphericalEarth::GreatCircleHeadingAndDistance(start.GetLat(),
                                                            start.GetLon(),
                                                            target.GetLat(),
                                                            target.GetLon(),
                                                            heading,
                                                            distance);
            heading *= UtMath::cRAD_PER_DEG;
         }
      }
      GetPlatform()->SetOrientationNED(heading, 0.0, 0.0);
      double velocityNED[3] = {cos(heading) * speed, sin(heading) * speed, 0.0};
      GetPlatform()->SetVelocityNED(velocityNED);
   }

   GetPlatform()->SetEmptyMass(barems * BRAWLER::grav * UtMath::cKG_PER_LB);
   GetPlatform()->SetFuelMass(gas * BRAWLER::grav * UtMath::cKG_PER_LB);

   ////////////////////////////////////////////////////////////////////////////

   ok &= BrawlerMover::Initialize(aSimTime, *GetPlatform(), mBrawlerPlatformPtr);

   ////ensure platform starts "heading" in the same direction as velocity
   // UtVec3d vned = this->Velocity(); // feet/sec
   // vned *= UtMath::cM_PER_FT; // meters/sec
   // GetPlatform()->SetOrientationNED( atan2(vned[0], vned[1]) , 0, 0);
   // GetPlatform()->SetVelocityNED(vned.GetData());

   ok &= WsfMover::Initialize(aSimTime);

   if (mDrawProjection)
   {
      mDrawPtr = new WsfDraw(*GetSimulation());
   }

   return ok;
}

// virtual
bool WsfBrawlerMover::ProcessInput(UtInput& aInput)
{
   bool                      myCommand = true;
   std::string               command(aInput.GetCommand());
   std::unique_ptr<WsfRoute> routePtr = nullptr;
   if (WsfRouteTypes::Get(WsfScenario::FromInput(aInput)).LoadInstance(aInput, routePtr) && routePtr != nullptr)
   {
      mRoute = *routePtr;
   }
   else if (command == "aero_file")
   {
      std::string filename;
      aInput.ReadValue(filename);
      filename = aInput.LocateFile(filename);
      if (!BrawlerMover::LoadAeroConfig(filename))
      {
         throw UtInput::BadValue(aInput, "Could not load WSF_BRAWLER_MOVER aero_file!");
      }
   }
   else if (command == "draw_projection")
   {
      mDrawProjection = true;
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
void WsfBrawlerMover::Update(double aSimTime)
{
   // Check that the minimum amount of time has passed
   // before the next update
   double dT = aSimTime - tsv;
   if (dT <= mUpdateTimeTolerance)
   {
      return;
   }

   // Per WsfMover::Update(), derived classes must update mLastUpdateTime
   // on their own to match current simulation time
   WsfMover::Update(aSimTime);

   if (mFlyingRoute == true)
   {
      if (mCurrentRouteIndex >= 0 && mCurrentRouteIndex < (int)mRoute.GetSize())
      {
         WsfWaypoint wpt = mRoute.GetWaypointAt(mCurrentRouteIndex);

         // check in range (increment index if needed)
         double lat, lon, alt, lat2, lon2, alt2;
         double heading, distance;
         GetPlatform()->GetLocationLLA(lat, lon, alt);
         wpt.GetLocationLLA(lat2, lon2, alt2);
         UtSphericalEarth::GreatCircleHeadingAndDistance(lat, lon, lat2, lon2, heading, distance);
         double turnRadiusMeters = BrawlerMover::TurnRadius() * UtMath::cM_PER_FT;
         if (distance < turnRadiusMeters)
         {
            ++mCurrentRouteIndex;
            if (mCurrentRouteIndex == (int)mRoute.GetSize())
            {
               // TODO - assumed for now that we circle back and redo route at beginning
               mCurrentRouteIndex = 0;
            }
            wpt = mRoute.GetWaypointAt(mCurrentRouteIndex);
            wpt.GetLocationLLA(lat2, lon2, alt2);
            UtSphericalEarth::GreatCircleHeadingAndDistance(lat, lon, lat2, lon2, heading, distance);
         }

         // update BrawlerMover with proper command for current route point
         // this mostly duplicates previous commands unless turning, then heading will be updating
         double speed = wpt.GetSpeed() * UtMath::cFT_PER_M;
         double maxClimb;
         if (wpt.GetClimbRate() == WsfPath::cDOUBLE_NOT_SET)
         {
            maxClimb = ReasonableClimbRate(speed);
         }
         else
         {
            maxClimb = wpt.GetClimbRate() * UtMath::cFT_PER_M;
         }

         double gmxsu  = BrawlerMover::gmxsut;
         double gmxmpp = 4.0;
         if (mBrawlerPlatformPtr != nullptr)
         {
            // LBM - the brawler mover ignores any waypoint radial acceleration commands
            gmxsu = mBrawlerPlatformPtr->MaxTacticalGLoad_g();
            // aslct2.f : 187,  for ROUTEPOINT flying: gmxmpp = gmaxm
            gmxmpp = mBrawlerPlatformPtr->GetMIND()->MaxGeesForRouteFlying(); // gmaxm from MIND
         }
         // alt32.f : 122
         double gmx = std::min(gmxmpp, std::min(gmxsu, 4.0));
         BrawlerMover::CommandHeadingSpeedAltitude(heading, speed, alt2 * UtMath::cFT_PER_M, gmx, maxClimb);
      }
      else
      {
         // ERROR!
         ut::log::error() << "Brawler mover route point index out of range!";
         mFlyingRoute = false;
      }
   }
   else if (mCommandType == HEADING_SPEED_ALTITUDE) // update climb rate for non-route commands
   {
      mMaxClimb = ReasonableClimbRate(mSpeed);
   }

   // BrawlerMover::flyac(aSimTime, *((BrawlerPlatform*)GetPlatform()));
   // BrawlerMover::flyac(aSimTime);
   // BrawlerMover::Update(aSimTime, *((BrawlerPlatform*)GetPlatform()));

   BrawlerMover::UpdateState(aSimTime);

   // update owning platform
   UtEntity newState  = BrawlerMover::State(aSimTime);
   double   locWCS[3] = {0, 0, 0};
   double   velWCS[3] = {0, 0, 0};
   double   accWCS[3] = {0, 0, 0};
   double   oriWCS[3] = {0, 0, 0};

   newState.GetLocationWCS(locWCS);
   newState.GetVelocityWCS(velWCS);
   newState.GetAccelerationWCS(accWCS);
   newState.GetOrientationWCS(oriWCS[0], oriWCS[1], oriWCS[2]);

   GetPlatform()->SetLocationWCS(locWCS);
   GetPlatform()->SetVelocityWCS(velWCS);
   GetPlatform()->SetAccelerationWCS(accWCS);
   GetPlatform()->SetOrientationWCS(oriWCS[0], oriWCS[1], oriWCS[2]);
   GetPlatform()->SetEmptyMass(barems * BRAWLER::grav * UtMath::cKG_PER_LB);
   GetPlatform()->SetFuelMass(gas * BRAWLER::grav * UtMath::cKG_PER_LB);

   if (mBrawlerPlatformPtr != nullptr)
   {
      mBrawlerPlatformPtr->State().SetLocationWCS(locWCS);
      mBrawlerPlatformPtr->State().SetVelocityWCS(velWCS);
      mBrawlerPlatformPtr->State().SetAccelerationWCS(accWCS);
      mBrawlerPlatformPtr->State().SetOrientationWCS(oriWCS[0], oriWCS[1], oriWCS[2]);
   }

   if (mDrawProjection)
   {
      // double projTime = aSimTime + 5.0;
      double projDelta = 5.0;
      if (mBrawlerPlatformPtr != nullptr)
      {
         projDelta = mBrawlerPlatformPtr->ProjectedTimeDelta();
      }
      ProjectForward(projDelta, mDebugEntityProjection);
      // debug drawing
      double locWCS[3] = {0, 0, 0};
      mDebugEntityProjection.GetLocationWCS(locWCS);
      mDrawPtr->SetId(mDrawPtr->GetNewId());
      mDrawPtr->SetPointSize(4);
      // mDrawPtr->SetDuration( 2 * projTime ); //infinite if not set?
      mDrawPtr->SetColor(1.0, 0.5, 0.0);
      mDrawPtr->BeginPoints();
      mDrawPtr->VertexWCS(locWCS);
      mDrawPtr->VertexWCS(locWCS); // Need two point because of VESPA bug
      mDrawPtr->End();
   }
}


// virtual
void WsfBrawlerMover::UpdateFuel(double aTimeStep, double aInitialFlow, double aThrottle, double& aFuelQuantity, double& aVehicleMass)
{
   if (mFuelPtr != nullptr)
   {
      double oldQuantity = aFuelQuantity;

      // Brawler gas uses slugs, GetQuantityRemaining() returns kilograms
      aFuelQuantity = mFuelPtr->GetQuantityRemaining() / UtMath::cKG_PER_SLUG;
      aVehicleMass -= (oldQuantity - aFuelQuantity);
   }
   else
   {
      // No fuel object attached to the platform, so use internal fuel representation
      BrawlerMover::UpdateFuel(aTimeStep, aInitialFlow, aThrottle, aFuelQuantity, aVehicleMass);
   }
}

// virtual
WsfSpatialDomain WsfBrawlerMover::GetSpatialDomain() const
{
   return WSF_SPATIAL_DOMAIN_AIR;
}

void WsfBrawlerMover::TurnOn(double aSimTime)
{
   WsfMover::TurnOn(aSimTime);
   tsv = aSimTime;
}

// virtual
void WsfBrawlerMover::DeleteMover()
{
   WsfSimulation* simPtr  = GetSimulation();
   WsfPlatform*   platPtr = GetPlatform();
   if (platPtr != nullptr && simPtr != nullptr)
   {
      auto out = ut::log::warning() << "Brawler mover deleting Platform.";
      out.AddNote() << "Platform: " << platPtr->GetName();
      simPtr->DeletePlatform(simPtr->GetSimTime(), platPtr);
   }
   else
   {
      ut::log::error() << "Failed to delete mover, sim* or platform* are null!";
   }
}

// virtual
bool WsfBrawlerMover::FlyRates(const UtVec3d& aAngularRates, const UtVec3d& aLongitudinalAccel)
{
   UtVec3d angRatesInDeg(aAngularRates);
   angRatesInDeg.Multiply(UtMath::cDEG_PER_RAD);

   UtVec3d longAccelInFt(aLongitudinalAccel);
   longAccelInFt.Multiply(UtMath::cFT_PER_M);

   BrawlerMover::CommandRates(angRatesInDeg, longAccelInFt);
   mFlyingRoute = false;
   return true;
}

// virtual
bool WsfBrawlerMover::FlyHeadingSpeedAltitude(double aHeading, double aSpeed, double aAltitude, double aMaxGees, double aMaxClimb)
{
   BrawlerMover::CommandHeadingSpeedAltitude(aHeading,
                                             aSpeed * UtMath::cFT_PER_M,
                                             aAltitude * UtMath::cFT_PER_M,
                                             aMaxGees,
                                             aMaxClimb * UtMath::cFT_PER_M);
   mFlyingRoute = false;
   return true;
}

// virtual
bool WsfBrawlerMover::TurnInPlaneWithSpeed(const UtVec3d& aPlane, double aGees, double aSpeed)
{
   UtVec3d planeInFt(aPlane);
   planeInFt.Multiply(UtMath::cFT_PER_M);
   BrawlerMover::CommandPlaneManeuver(planeInFt, aGees, aSpeed * UtMath::cFT_PER_M, false);
   mFlyingRoute = false;
   return true;
}

// virtual
bool WsfBrawlerMover::TurnInPlaneWithThrottle(const UtVec3d& aPlane, double aGees, double aThrottle)
{
   UtVec3d planeInFt(aPlane);
   planeInFt.Multiply(UtMath::cFT_PER_M);
   BrawlerMover::CommandPlaneManeuver(aPlane,
                                      aGees,
                                      aThrottle, //[0-3]
                                      true);
   mFlyingRoute = false;
   return true;
}

// virtual
bool WsfBrawlerMover::FlyVectorWithSpeed(const UtVec3d& aVelocityVector, const double aMaxGees, const double aSpeed)
{
   UtVec3d vectorInFt(aVelocityVector);
   vectorInFt.Multiply(UtMath::cFT_PER_M);
   vectorInFt.Normalize();
   BrawlerMover::CommandVector(vectorInFt, aMaxGees, aSpeed * UtMath::cFT_PER_M, false);
   mFlyingRoute = false;
   return true;
}

// virtual
bool WsfBrawlerMover::FlyVectorWithThrottle(const UtVec3d& aVelocityVector, const double aMaxGees, const double aThrottle)
{
   UtVec3d vectorInFt(aVelocityVector);
   vectorInFt.Multiply(UtMath::cFT_PER_M);
   vectorInFt.Normalize();
   BrawlerMover::CommandVector(vectorInFt,
                               aMaxGees,
                               aThrottle, //[0-3]
                               true);
   mFlyingRoute = false;
   return true;
}

// virtual
bool WsfBrawlerMover::ProjectForward(double delta, UtEntity& projected)
{
   BrawlerMover::aproj3(delta, projected);
   return true;
}


// mCommandType = HEADING_SPEED_ALTITUDE
// mHeading = aHeading;
// mSpeed = aSpeed;
// mThrottleFlag = false;
// mAltitude = aAltitude;
// mMaxGees = aMaxGees;
// mMaxClimb = aMaxClimb;

// virtual
bool WsfBrawlerMover::GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude)
{
   // calculate heading to target lat/lon for use with HEADING_SPEED_ALTITUDE command type
   double lat, lon, alt;
   BrawlerMover::State(aSimTime).GetLocationLLA(lat, lon, alt);
   double heading, distance;
   UtSphericalEarth::GreatCircleHeadingAndDistance(lat, lon, aLatitude, aLongitude, heading, distance);

   if (mCommandType == HEADING_SPEED_ALTITUDE)
   {
      // overwrite heading/alt and done, no need to estimate other command parameters, use existing
      mHeading  = heading;
      mAltitude = aAltitude * UtMath::cFT_PER_M;
   }
   else
   {
      double speed    = BrawlerMover::State(aSimTime).GetSpeed() * UtMath::cFT_PER_M;
      double maxClimb = ReasonableClimbRate(speed);
      double maxG     = 6.0; // * BRAWLER::grav;
      BrawlerMover::CommandHeadingSpeedAltitude(heading, speed, aAltitude * UtMath::cFT_PER_M, maxG, maxClimb);
   }

   mFlyingRoute = false;
   return true;
}

// virtual
bool WsfBrawlerMover::TurnToHeading(double                 aSimTime,
                                    double                 aHeading,     // radians
                                    double                 aRadialAccel, // meters/second^2
                                    WsfPath::TurnDirection aTurnDirection)
{
   double maxG = aRadialAccel / UtEarth::cACCEL_OF_GRAVITY; //    / BRAWLER::grav;
   if (maxG <= 0.0)
   {
      maxG = 6.0; // brawler default
   }
   if (mCommandType == HEADING_SPEED_ALTITUDE)
   {
      // overwrite heading and Gs, done
      mHeading = aHeading * UtMath::cDEG_PER_RAD;
      mMaxGees = maxG;
   }
   else
   {
      // set heading, speed, & altitude???
      double speed    = BrawlerMover::State(aSimTime).GetSpeed() * UtMath::cFT_PER_M;
      double alt      = BrawlerMover::State(aSimTime).GetAltitude() * UtMath::cFT_PER_M;
      double maxClimb = ReasonableClimbRate(speed);
      BrawlerMover::CommandHeadingSpeedAltitude(aHeading * UtMath::cDEG_PER_RAD, speed, alt, maxG, maxClimb);
   }
   mFlyingRoute = false;
   return true;
}

// virtual
bool WsfBrawlerMover::GoToSpeed(double aSimTime,
                                double aSpeed, // meters/sec
                                double aLinearAccel,
                                bool   aKeepRoute)
{
   // TODO - aKeepRoute is ignored for now, implement this capability???
   double maxG = aLinearAccel / UtEarth::cACCEL_OF_GRAVITY; //    / BRAWLER::grav;
   if (maxG <= 0.0)
   {
      maxG = 6.0; // brawler default
   }
   if (mCommandType == HEADING_SPEED_ALTITUDE)
   {
      // overwrite speed and Gs, done
      mSpeed   = aSpeed * UtMath::cFT_PER_M;
      mMaxGees = maxG;
   }
   else
   {
      // set heading, speed, & altitude???
      double heading, pitch, roll;
      BrawlerMover::State(aSimTime).GetOrientationNED(heading, pitch, roll);
      double speed    = aSpeed * UtMath::cFT_PER_M;
      double alt      = BrawlerMover::State(aSimTime).GetAltitude() * UtMath::cFT_PER_M;
      double maxClimb = ReasonableClimbRate(speed);
      BrawlerMover::CommandHeadingSpeedAltitude(heading, speed, alt, maxG, maxClimb);
   }
   mFlyingRoute = false;
   return true;
}

// virtual
bool WsfBrawlerMover::GoToAltitude(double aSimTime,
                                   double aAltitude, // meters
                                   double aClimbDiveRate,
                                   bool   aKeepRoute)
{
   // TODO - aKeepRoute is ignored for now, implement this capability???
   double maxClimb = aClimbDiveRate * UtMath::cFT_PER_M;
   if (maxClimb <= 0)
   {
      double desiredSpeed = (mCommandType == HEADING_SPEED_ALTITUDE) ? (mSpeed) : (BrawlerMover::Speed());
      maxClimb            = ReasonableClimbRate(desiredSpeed);
   }
   if (mCommandType == HEADING_SPEED_ALTITUDE)
   {
      // overwrite speed and Gs, done
      mAltitude = aAltitude * UtMath::cFT_PER_M;
      mMaxClimb = maxClimb;
   }
   else
   {
      // set heading, speed, & altitude???
      double heading, pitch, roll;
      BrawlerMover::State(aSimTime).GetOrientationNED(heading, pitch, roll);
      double speed = BrawlerMover::State(aSimTime).GetSpeed() * UtMath::cFT_PER_M;
      double alt   = aAltitude * UtMath::cFT_PER_M;
      double maxG  = 6.0; //* BRAWLER::grav;
      BrawlerMover::CommandHeadingSpeedAltitude(heading, speed, alt, maxG, maxClimb);
   }
   mFlyingRoute = false;
   return true;
}

// virtual
bool WsfBrawlerMover::TurnToRelativeHeading(double                 aSimTime,
                                            double                 aHeadingChange,
                                            double                 aRadialAccel,
                                            WsfPath::TurnDirection aTurnDirection)
{
   double heading, pitch, roll;
   BrawlerMover::State(aSimTime).GetOrientationNED(heading, pitch, roll);
   return WsfBrawlerMover::TurnToHeading(aSimTime, heading + aHeadingChange, aRadialAccel, aTurnDirection);
}

// virtual
bool WsfBrawlerMover::ReturnToRoute(double aSimTime)
{
   mFlyingRoute = true;
   return true;
}

// virtual
bool WsfBrawlerMover::UpdateRoute(double aSimTime, const WsfRoute& aRoute)
{
   Update(aSimTime);
   // TODO - any checking for if route size > 0 ???
   mRoute             = aRoute;
   mCurrentRouteIndex = 0;
   mFlyingRoute       = true;
   // clear up any cFLOAT_NOT_SET issues in the route
   double speed = Speed() * UtMath::cM_PER_FT;
   double alt   = Altitude() * UtMath::cM_PER_FT;
   for (unsigned int i = 0; i < mRoute.GetSize(); ++i)
   {
      if (mRoute[i].GetAlt() == WsfPath::cUSE_PREVIOUS)
      {
         mRoute[i].SetAlt(alt);
      }
      else
      {
         alt = mRoute[i].GetAlt();
      }

      if (mRoute[i].GetSpeed() == WsfPath::cUSE_PREVIOUS)
      {
         mRoute[i].SetSpeed(speed);
      }
      else
      {
         speed = mRoute[i].GetSpeed();
      }
   }
   return true;
}

// virtual
// mostly copied from WsfRouteMover::GoToClosestPoint()
bool WsfBrawlerMover::GoToClosestPoint(double aSimTime)
{
   Update(aSimTime);
   unsigned int closestIndex    = 0;
   double       closestDSquared = UtMath::cDOUBLE_MAX;
   double       curLocWCS[3];
   GetPlatform()->GetLocationWCS(curLocWCS);
   double testDSquared;
   double testWCS[3];
   double diffWCS[3];
   for (unsigned int i = 0; i != mRoute.GetSize(); ++i)
   {
      const WsfWaypoint& wp = mRoute[i];
      if (wp.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
      {
         UtEntity::ConvertLLAToWCS(wp.GetLat(), wp.GetLon(), wp.GetAlt(), testWCS);
         UtVec3d::Subtract(diffWCS, testWCS, curLocWCS);
         testDSquared = UtVec3d::MagnitudeSquared(diffWCS);
         if (testDSquared < closestDSquared)
         {
            closestIndex    = i;
            closestDSquared = testDSquared;
         }
      }
   }
   if (closestDSquared < UtMath::cDOUBLE_MAX)
   {
      GoToWaypoint(aSimTime, closestIndex);
      return true;
   }
   return false;
}

// virtual
// copied from WsfRouteMover::GoToLabel()
bool WsfBrawlerMover::GoToLabel(double aSimTime, WsfStringId aLabelId)
{
   bool found = false;
   for (unsigned int i = 0; i < mRoute.GetSize(); ++i)
   {
      if (mRoute[i].GetLabelId() == aLabelId)
      {
         GoToWaypoint(aSimTime, i);
         found = true;
         break;
      }
   }
   return found;
}

// virtual
bool WsfBrawlerMover::GoToWaypoint(double aSimTime, unsigned int aDestination)
{
   Update(aSimTime);
   mCurrentRouteIndex = (aDestination >= mRoute.GetSize()) ? (mRoute.GetSize() - 1) : (aDestination);
   mFlyingRoute       = true;
   return true;
}

// virtual
double WsfBrawlerMover::GetThrottlePosition()
{
   return Throttle();
}

bool WsfBrawlerMover::Slice(double aDesiredHeading, double aDesiredAltitude, double aDesiredMach)
{
   double altitude = aDesiredAltitude;
   BrawlerMover::CommandSlice(aDesiredHeading, altitude, aDesiredMach);
   mFlyingRoute = false;
   return true;
}

bool WsfBrawlerMover::Prlvl(double aDesiredHeading, double aDesiredMach)
{
   BrawlerMover::CommandLevelTurn(aDesiredHeading, aDesiredMach, 0.0);
   return true;
}

bool WsfBrawlerMover::Prlvlg(double aDesiredHeading, double aDesiredMach, double aDesiredgees)
{
   BrawlerMover::CommandLevelTurn(aDesiredHeading, aDesiredMach, aDesiredgees);
   return true;
}

void WsfBrawlerMover::SetMaxThrottle(double aThrottle)
{
   BrawlerMover::SetMaxThrottle(aThrottle);
}

double WsfBrawlerMover::ReasonableClimbRate(double desiredSpeed)
{
   double speed  = BrawlerMover::Speed();
   double drag   = BrawlerMover::Drag(BrawlerMover::Alpha(), speed, BrawlerMover::Altitude());
   double thrust = BrawlerMover::Thrust(3.0);
   double weight = BrawlerMover::AircraftMass() * BRAWLER::grav;
   // ratio: how much of excess power to use for acceleration, rest is used for climb (quadratic scale)
   double ratio = (desiredSpeed > speed) ? (sqrt((desiredSpeed - speed) / desiredSpeed)) : (0.0);
   double power = (thrust - drag) * speed;        // excess power
   double climb = (1.0 - ratio) * power / weight; // max reasonable climb rate
   return climb;
}
