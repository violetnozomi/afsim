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

#include "WsfAirMover.hpp"

#include "UtAtmosphere.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"

static const double sCANNOT_CRASH_SPEED = 1.0E+8;

WsfAirMover::WsfAirMover(const WsfScenario& aScenario)
   : WsfWaypointMover(aScenario)
   , mMaxImpactSpeed(sCANNOT_CRASH_SPEED)
   , mCrashedIntoGround(false)
   , mIsOnRunWay(false)
{
   mMoverConstraints.mMinAltitude = 0.0;
   SetEndOfPath(WsfPath::cEP_EXTRAPOLATE);
}

// virtual
WsfAirMover::~WsfAirMover() {}

// virtual
bool WsfAirMover::Initialize(double aSimTime)
{
   bool success = WsfWaypointMover::Initialize(aSimTime);
   mIsOnRunWay  = false;
   return success;
}

// virtual
bool WsfAirMover::ProcessInput(UtInput& aInput)
{
   std::string command(aInput.GetCommand());
   bool        ok = true;
   if (command == "maximum_impact_speed")
   {
      aInput.ReadValueOfType(mMaxImpactSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMaxImpactSpeed, 0.0);
   }
   // Prohibit commands not valid for this user type:
   else if (command == "on_road")
   {
      throw UtInput::BadValue(aInput, "Command " + command + " cannot be used with a WSF_AIR_MOVER.");
   }
   else
   {
      ok = WsfWaypointMover::ProcessInput(aInput);
   }
   return ok;
}

// virtual
WsfMover* WsfAirMover::Clone() const
{
   return new WsfAirMover(*this);
}

// virtual
double WsfAirMover::GetThrottlePosition()
{
   double alt_m = GetPlatform()->GetAltitude();
   double mach  = GetPlatform()->GetSpeed() / GetPlatform()->GetScenario().GetAtmosphere().SonicVelocity(alt_m);

   // If the mover is subsonic, return a throttle response between 1.0 and 2.0.
   // If the aircraft is supersonic, return a throttle response between 2.0 and 3.0 (afterburner).
   // Anything past mach 2.0 will set the throttle to its maximum afterburner value of 3.0
   return UtMath::Limit((mach + 1.0), 3.0);
}

//! See if target altitude is above ground.
bool WsfAirMover::DestIsInAir()
{
   bool toTheAir = false;

   if ((mTarget.mTargetType & WsfPathTarget::cALTITUDE))
   {
      double val = mTarget.mAltitude;
      toTheAir   = (val >= (GetHeightOfTerrain() + WsfPath::cGROUND_PROXIMITY_FUZZ));
   }
   return toTheAir;
}

//! Set the mover's state to transition from ground movement to air movement
void WsfAirMover::TakeOff(double aSimTime)
{
   // Executing Takeoff...
   SetIsOnGround(false);
   mIsOnRunWay = true;
   SetEndOfPath(WsfPath::cEP_EXTRAPOLATE);
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform is beginning takeoff (mIsOnGround = false).";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
}

//! Set the mover's state to transition from air movement to ground movement
void WsfAirMover::Land(double aSimTime)
{
   SetIsOnGround(true);
   SetEndOfPath(WsfPath::cEP_STOP);
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform touched down on runway (mIsOnGround = true).";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
}

//! Take action when the mover crashes into the ground.
//! Call the simulation observers and delete the player.
void WsfAirMover::CrashIntoGround(double aSimTime)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform has fatally crashed.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
   mCrashedIntoGround = true;
   WsfObserver::CrashedIntoGround(GetSimulation())(aSimTime, GetPlatform());
   GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
}

// protected virtual
void WsfAirMover::PostMoveUpdate(double aSimTime)
{
   if (false && DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform is getting post-move updated.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }

   // Allow the platform to takeoff, crash, or land.
   if (DestIsInAir() && IsOnGround())
   {
      TakeOff(aSimTime);
   }
   else if (mIsOnRunWay)
   {
      // Executing Ground Roll...
      double lat, lon, alt;
      GetPlatform()->GetLocationLLA(lat, lon, alt);
      double hat = alt - GetHeightOfTerrain();
      if (hat < 0.0)
      {
         GetPlatform()->SetLocationLLA(lat, lon, GetHeightOfTerrain() + mAltitudeOffset);
      }
      else if (hat > 5.0)
      {
         mIsOnRunWay = false;
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Platform has left the runway.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
      }
   }
   else if (!mIsOnGround)
   {
      UpdateHeightOfTerrain();

      // If it lands, do the update, and do not allow it to crash again.
      // Was previously in the air...
      double lat, lon, alt;
      GetPlatform()->GetLocationLLA(lat, lon, alt);
      double heightAboveTerrain = alt - GetHeightOfTerrain();
      if (heightAboveTerrain < -1.0)
      {
         AssessDamage(aSimTime);

         // Clamp height above terrain to the ground.
         GetPlatform()->SetLocationLLA(lat, lon, GetHeightOfTerrain() + mAltitudeOffset);

         // If destroyed, perform "crash" behavior; otherwise, "land" behavior.
         if (GetPlatform()->GetDamageFactor() < 1.0)
         {
            Land(aSimTime);
         }
         else if (!mCrashedIntoGround)
         {
            CrashIntoGround(aSimTime);
         }
      }
   }
}

// protected virtual
//! Assess damage on the owning platform due to a ground impact.
//! The dot product of the normal vector of the terrain at impact and the impact velocity is
//! compared with the max impact velocity.  If the calculated value is greater than the maximum,
//! a damage factor of "1.0" is assessed, and the platform is essentially broken.
void WsfAirMover::AssessDamage(double aSimTime)
{
   // Determine the normal component of its impact velocity with the ground.
   double lat, lon, alt;
   GetPlatform()->GetLocationLLA(lat, lon, alt);
   double normalNED[3] = {0.0, 0.0, 1.0};
   if (mTerrainPtr->IsEnabled())
   {
      mTerrainPtr->GetNormalNED(lat, lon, normalNED);
   }
   double velNED[3];
   GetPlatform()->GetVelocityNED(velNED);
   double velDotNormal = UtVec3d::DotProduct(velNED, normalNED);

   // If the normal component of the impact velocity is larger than the maximum specified
   // by the input, specify maximum damage on the platform.
   if (fabs(velDotNormal) > mMaxImpactSpeed) // Using fabs considers situations like flying
   {
      // into the side of a mountain.
      GetPlatform()->SetDamageFactor(1.0);
   }
}
