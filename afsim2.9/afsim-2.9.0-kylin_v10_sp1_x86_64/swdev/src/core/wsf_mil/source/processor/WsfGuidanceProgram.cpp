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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfGuidanceProgram.hpp"

#include <iomanip>
#include <iostream>
#include <string>

#include "UtCalendar.hpp"
#include "UtControl.hpp"
#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtLLPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtOrbitalState.hpp"
#include "UtPolynomial.hpp"
#include "UtScriptContext.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfApplication.hpp"
#include "WsfDateTime.hpp"
#include "WsfGuidanceComputer.hpp"
#include "WsfGuidedMover.hpp"
#include "WsfMil.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "script/WsfScriptContext.hpp"

const char* WsfGuidanceProgram::cGC_ORBIT_DECLARED      = "GC_ORBIT_DECLARED";
const char* WsfGuidanceProgram::cGC_DELTA_V_REMAINING   = "GC_DELTA_IV_REMAINING";
const char* WsfGuidanceProgram::cGC_BURN_TIME_REMAINING = "GC_BURN_TIME_REMAINING";
const char* WsfGuidanceProgram::cGC_FUEL_REMAINING      = "GC_FUEL_REMAINING";

namespace
{
enum RotationAxis
{
   cYAW_AXIS   = WsfGuidanceProgram::cZ_AXIS,
   cPITCH_AXIS = WsfGuidanceProgram::cY_AXIS,
   cROLL_AXIS  = WsfGuidanceProgram::cX_AXIS
};

const double cPHASE_DEFAULT_DOUBLE = 1.0E+30;
} // namespace

// ================================================================================================
//! Constructor.
WsfGuidanceProgram::WsfGuidanceProgram()
   : WsfObject()
   , mComputerPtr(nullptr)
   , mStartTime(0.0)
   , mDebugEnabled(false)
{
}

// ================================================================================================
//! Initialize the program.
//!
//! This is called by the owning guidance computer when it performs its initialization.
bool WsfGuidanceProgram::Initialize(double aSimTime, WsfGuidanceComputer* aComputerPtr)
{
   mComputerPtr = aComputerPtr;
   mDebugEnabled |= aComputerPtr->DebugEnabled();
   return true;
}

// ================================================================================================
//! Process input for the program.
//! @param aInput The current input stream.
//! @returns 'true' if the command was one recognized by the program or 'false' if not.
bool WsfGuidanceProgram::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "debug")
   {
      mDebugEnabled = true;
   }
   else
   {
      myCommand = WsfObject::ProcessInput(aInput);
   }
   return myCommand;
}

// ================================================================================================
//! Start using the program.
void WsfGuidanceProgram::Start(double aSimTime, double aCurTime)
{
   mStartTime = aCurTime;
}

// ================================================================================================
//! Terminate the program.
void WsfGuidanceProgram::Terminate(double aSimTime) {}

// ================================================================================================
//! If requested, add in G bias to fight drop due to gravity.
//! @param aEntity  [input] The weapon entity.
//! @param aGravityBiasFactor [input] The gravity bias factor.
//! @param aY_Accel [updated] The required ECS Y acceleration.
//! @param aZ_Accel [updated] The required ECS Z acceleration.
// static
void WsfGuidanceProgram::ApplyGravityBias(UtEntity& aEntity, double aGravityBiasFactor, double& aY_Accel, double& aZ_Accel)
{
   if (aGravityBiasFactor != 0.0)
   {
      // Compute the gravitational acceleration vector.
      double wpnLocWCS[3];
      double gravAclWCS[3];
      aEntity.GetLocationWCS(wpnLocWCS);
      ComputeGravitationalAcceleration(wpnLocWCS, gravAclWCS);

      // Compute the UPWARD vector due to gravity (pointing out from the Earth's center).
      // (Remember we're trying to fight gravity!).
      UtVec3d::Multiply(gravAclWCS, -aGravityBiasFactor);

      // Transform the vector into the ECS frame and add the lateral components to the required accelerations.
      double gravAclECS[3];
      aEntity.ConvertWCSVectorToECS(gravAclECS, gravAclWCS);
      aY_Accel += gravAclECS[1];
      aZ_Accel += gravAclECS[2];
   }
}

// ================================================================================================
//! If requested, add in Lateral bias to fly a curving path to the target
//! (used by radars to spread the clutter in doppler to boost SNR.  "Doppler Beam Sharpening")
//! @param aEntity  [input] The weapon entity.
//! @param aLateralBiasFactor [input] The lateral bias factor.
//! @param aY_Accel [updated] The required ECS Y acceleration.
//! @param aZ_Accel [updated] The required ECS Z acceleration.
// static
void WsfGuidanceProgram::ApplyLateralBias(UtEntity& aEntity, double aLateralBiasFactor, double& aY_Accel, double& aZ_Accel)
{
   if (aLateralBiasFactor != 0.0)
   {
      // Compute the lateral acceleration normal to NED heading direction.
      // Transform the vector into the ECS frame and add the components to the required accelerations.
      aY_Accel += aLateralBiasFactor * UtEarth::cACCEL_OF_GRAVITY;
   }
}

// ================================================================================================
//! Compute the gravitational acceleration vector at a specified location.
//! @param aLocWCS     [input] The WCS position of the location.
//! @param aGravAclWCS [output] The gravitational acceleration vector (pointing to the Earth's center).
// static
void WsfGuidanceProgram::ComputeGravitationalAcceleration(const double aLocWCS[3], double aGravAclWCS[3])
{
   // Compute the scalar gravitational acceleration.

   double r = UtVec3d::Magnitude(aLocWCS);
   double g = UtEarth::cGRAVITATIONAL_PARAMETER / (r * r); // gravitational acceleration

   // Compute the 'downward' (toward the center of the Earth) acceleration vector.
   //
   // The multiplication by '1/r' converts the location vector into a unit vector that points
   // from the center of the Earth to the location. The multiplication by '-g' makes the vector
   // with the proper magnitude and points it towards the center of the Earth.

   UtVec3d::Multiply(aGravAclWCS, aLocWCS, -g / r);
}

// ================================================================================================
//! Compute the 'climb/dive-to-altitude and hold' guidance.
// protected
void WsfGuidanceProgram::AltitudeGuidance(State& aState, Phase& aPhase, double& aZ_Accel)
{
   // If the timestep is too small, then don't alter the guidance.
   double deltaT = aState.mCurTime - mComputerPtr->GetLastUpdateTime();
   if (deltaT < 1.0E-6)
   {
      return;
   }

   double curAlt       = GetPlatform()->GetAltitude();
   double commandedAlt = aPhase.mCommandedAlt;
   if (aPhase.mCommandedAltIsAGL && GetPlatform()->GetTerrain().IsEnabled())
   {
      double terrainHeight = GetPlatform()->GetTerrainHeight();
      commandedAlt += terrainHeight;
   }

   double curPitch = aState.mFlightPathAngle;
   double curSpeed = GetPlatform()->GetSpeedWCS();
   double deltaAlt = commandedAlt - curAlt;

   // Determine the minimum and maximum allowable flight path angles.
   double maxPitchAngle = aPhase.mMaxPitchAngle;
   double minPitchAngle = -maxPitchAngle;
   if (aPhase.mMaxAscentRate > 0.0)
   {
      if (curSpeed > aPhase.mMaxAscentRate)
      {
         maxPitchAngle = std::min(maxPitchAngle, asin(aPhase.mMaxAscentRate / curSpeed));
      }
   }
   if (aPhase.mMaxDescentRate > 0.0)
   {
      if (curSpeed > aPhase.mMaxDescentRate)
      {
         minPitchAngle = std::max(minPitchAngle, -asin(aPhase.mMaxDescentRate / curSpeed));
      }
   }

   // Get the requested vertical speed for the current altitude difference.
   double reqVertSpeed = 0.0;
   if (deltaAlt >= 0.0) // Climbing
   {
      reqVertSpeed = sqrt(UtEarth::cACCEL_OF_GRAVITY * deltaAlt);
   }
   else
   {
      reqVertSpeed = -sqrt(-UtEarth::cACCEL_OF_GRAVITY * deltaAlt);
   }

   // Compute the requested pitch angle.
   double newPitch = 0.0;
   if (fabs(reqVertSpeed) < curSpeed)
   {
      newPitch = asin(reqVertSpeed / curSpeed);
      newPitch = UtMath::Limit(newPitch, minPitchAngle, maxPitchAngle);
   }
   else if (deltaAlt >= 0.0)
   {
      newPitch = maxPitchAngle;
   }
   else
   {
      newPitch = minPitchAngle;
   }

   double deltaPitch = newPitch - curPitch;

   // Compute the acceleration needed to complete the 'turn' (assumes circular motion)
   double normAccel = 0.0;
   if (fabs(deltaPitch) > 1.0E-6)
   {
      double S  = curSpeed * deltaT;
      double R  = S / fabs(deltaPitch);
      double a  = (curSpeed * curSpeed) / R;
      normAccel = a;
      if (deltaPitch < 0.0)
      {
         normAccel = -normAccel;
      }
   }

   // Apply the G-bias so altitude hold works... otherwise we keep falling below.
   normAccel += cos(aState.mFlightPathAngle) * UtEarth::cACCEL_OF_GRAVITY;

   // Copy to output, remembering that +Z is down
   aZ_Accel = -normAccel;

   // cout << "T=" << aState.mCurTime
   //      << " (" << mComputerPtr->GetTimeSinceLaunch(aState.mCurTime) << "); V=" << curSpeed
   //      << " Alt=" << GetPlatform()->GetAltitude() << " dh=" << deltaAlt
   //      << " Av=" << normAccel << ' ' << aZ_Accel << endl;
   // cout << "  Pold=" << curPitch * UtMath::cDEG_PER_RAD
   //      << " Pnew=" << newPitch * UtMath::cDEG_PER_RAD
   //      << "; Pdiff=" << deltaPitch * UtMath::cDEG_PER_RAD
   //      << "; Vvcur=" << curSpeed * sin(curPitch) << " Vvreq=" << reqVertSpeed << endl;
}

// ================================================================================================
// protected
void WsfGuidanceProgram::AngleOffsetGuidance(State& aState, Phase& aPhase, double& aY_Accel, double& aZ_Accel)
{
   double dt = aState.mEndTime - aState.mCurTime;
   if (dt <= 0.0)
   {
      return;
   }

   if (aPhase.mCommandedAzimuthOffset != WsfGuidanceComputer::cUNDEFINED_DOUBLE)
   {
      // Get the absolute bearing of the target from the weapon.
      double tgtLocNED[3];
      aState.mWpnState.GetRelativeLocationNED(&aState.mTgtState, tgtLocNED);
      double tgtBearing = atan2(tgtLocNED[1], tgtLocNED[0]);

      // Get the relative bearing of the target from the weapon.
      double relTgtBearing = UtMath::NormalizeAngleMinusPi_Pi(tgtBearing - aState.mWpnHeading);

      // Determine the magnitude and direction of the turn required to achieve the desired relative bearing.
      //
      // Note that the heading change is in the OPPOSITE direction of the difference.

      double headingChange = 0.0; // Positive for right turn, negative for left turn
      if (relTgtBearing >= 0.0)
      {
         headingChange = -(aPhase.mCommandedAzimuthOffset - relTgtBearing);
      }
      else
      {
         headingChange = -((-aPhase.mCommandedAzimuthOffset) - relTgtBearing);
      }

      double wpnVelNED[3];
      double wpnAclNED[3];
      aState.mWpnState.GetVelocityNED(wpnVelNED);
      aState.mWpnState.GetAccelerationNED(wpnAclNED);
      double avgWpnVelNED[3];
      UtVec3d::AddProduct(avgWpnVelNED, wpnVelNED, dt, wpnAclNED);
      UtVec3d::Add(avgWpnVelNED, wpnVelNED, avgWpnVelNED);
      UtVec3d::Multiply(avgWpnVelNED, 0.5);
      double speed = sqrt(avgWpnVelNED[0] * avgWpnVelNED[0] + avgWpnVelNED[1] * avgWpnVelNED[1]);
      // double speed = UtVec3d::Magnitude(avgWpnVelNED);
      if (speed > 1.0)
      {
         // TODO - Assuming no roll for now...
         double cmdAclECS[3] = {0.0, 1.0, 0.0};

         // Compute the acceleration needed to turn the requested amount in the current timestep

         double omega  = headingChange / dt;
         double cmdAcl = omega * speed;
         UtVec3d::Multiply(cmdAclECS, cmdAcl);
         aY_Accel = cmdAclECS[1];
      }
      else
      {
         // If the speed is zero (or very small), return the maximum value.
         aY_Accel = UtMath::Sign(static_cast<double>(aPhase.mMaxGeeCmd), headingChange);
      }
      // cout << "T=" << aState.mCurTime << " Hdg=" << aState.mWpnHeading * UtMath::cDEG_PER_RAD
      //    << " absTgtBrg=" << tgtBearing * UtMath::cDEG_PER_RAD
      //    << " relTgtBrg=" << relTgtBearing * UtMath::cDEG_PER_RAD
      //    << " want=" << aPhase.mCommandedAzimuthOffset * UtMath::cDEG_PER_RAD
      //    << " change=" << headingChange * UtMath::cDEG_PER_RAD
      //    << " speed=" << speed
      //    << " Ycmd=" << aY_Accel / UtEarth::cACCEL_OF_GRAVITY << " g" << endl;
   }
}

bool WsfGuidanceProgram::FlightPathAngleAchieved(const State& aState, double aCommandedFlightPathAngle) const
{
   bool   isComplete          = false;
   double thisFlightPathAngle = aState.mFlightPathAngle;
   double lastFlightPathAngle = mComputerPtr->GetLastFlightPathAngle();

   // Check to see if the program is complete... This is fundamentally the same as the one for
   // 'next_phase X on_commanded_flight_path_angle' in WsfGuidanceComputer::CheckForPhaseChange.
   if ((fabs(thisFlightPathAngle - aCommandedFlightPathAngle) <= WsfGuidanceComputer::cFLIGHT_PATH_ANGLE_TOLERANCE) ||
       ((thisFlightPathAngle <= aCommandedFlightPathAngle) && (lastFlightPathAngle >= aCommandedFlightPathAngle)) ||
       ((thisFlightPathAngle >= aCommandedFlightPathAngle) && (lastFlightPathAngle <= aCommandedFlightPathAngle)))
   {
      isComplete = true;
   }
   return isComplete;
}

// ================================================================================================
// protected
void WsfGuidanceProgram::FlightPathAngleGuidance(State&  aState,
                                                 Phase&  aPhase,
                                                 double  aCommandedFlightPathAngle,
                                                 double& aY_Accel,
                                                 double& aZ_Accel)
{
   // NOTE: The sign of 'pitchDelta' has the sign of the acceleration that needs to be applied.
   //       Remember that +Z is down, so a positive acceleration will pitch down.

   double pitchDelta = aState.mFlightPathAngle - aCommandedFlightPathAngle;

   // The angle is made very slightly larger to ensure the transition is made.
   // (See CheckForPhaseChange).
   pitchDelta += UtMath::Sign(WsfGuidanceComputer::cFLIGHT_PATH_ANGLE_TOLERANCE, pitchDelta);

   double dt = aState.mEndTime - aState.mCurTime;
   if ((dt > 0.0) && (pitchDelta != 0.0))
   {
      double speed = 0.0;
      if (GetPlatform()->GetCoordinateFrame() == UtEntity::UtEntity::cCF_INERTIAL)
      {
         // For ECI frames use only the axial speed. There is often a very large lateral speed during
         // the early stages of flight and we don't want that.
         //
         // Also note that GetVelocityECS returns the fixed and not inertial frame velocity. We need
         // to do a little fancy footwork here to get the inertial velocity in the ECS frame. It
         // probably doesn't matter much at this point, but we do it for completeness...

         double velECS[3];
         aState.mWpnState.GetVelocityECS(velECS);
         double velECI[3];
         aState.mWpnState.GetVelocityECI(velECI);
         UtVec3d::Normalize(velECS);
         double speedECI = UtVec3d::Normalize(velECI);
         UtVec3d::Multiply(velECS, speedECI);
         speed = velECS[0]; // take only the axial component
      }
      else
      {
         speed = aState.mWpnState.GetSpeed();
      }

      // If the speed is zero (or very small), simply return the maximum value.
      if (speed < 1.0)
      {
         aY_Accel = 0.0;
         aZ_Accel = UtMath::Sign(static_cast<double>(aPhase.mMaxGeeCmd), pitchDelta);
         return;
      }

      // Compute the acceleration needed to pitch the requested amount in the current timestep,
      // ignoring any effect that gravity may have on helping or hindering the change.
      double omega        = pitchDelta / dt;
      double cmdAcl       = omega * speed;
      double cmdAclECS[3] = {0.0, 0.0, cmdAcl};

      // Compute the local gravitational acceleration vector and convert it to the ECS frame.
      double wpnLocWCS[3];
      double gravAclWCS[3];
      aState.mWpnState.GetLocationWCS(wpnLocWCS);
      ComputeGravitationalAcceleration(wpnLocWCS, gravAclWCS);
      double gravAclECS[3];
      aState.mWpnState.ConvertWCSVectorToECS(gravAclECS, gravAclWCS);

      // Subtract out the gravitational effect from the commanded value. Otherwise if were pitching
      // down we might pitch too much, and if pitching up, we won't pitch up enough!
      UtVec3d::Subtract(cmdAclECS, cmdAclECS, gravAclECS);
      // aY_Accel = cmdAclECS[1]; //currently unused
      aZ_Accel = cmdAclECS[2];

      // TODO these may need to be combined with the existing aY_Accel and aZ_Accel if using
      // this guidance along with pronav...

      // cout << "T=" << aState.mCurTime << " FPG: " << aState.mFlightPathAngle * UtMath::cDEG_PER_RAD
      //    << " Cmd: " << mComputerPtr->GetCommandedFlightPathAngle() * UtMath::cDEG_PER_RAD
      //    << " Speed: " << speed << " Z_acl=" << aZ_Accel << endl;
   }
}

// ================================================================================================
// protected
void WsfGuidanceProgram::ProNavGuidance(State& aState, double aProNavGain, bool aAugmented, double& aY_Accel, double& aZ_Accel)
{
   double relTgtRange = UtVec3d::Magnitude(aState.mAimRelLocWCS);
   if (relTgtRange < 1.0)
   {
      aY_Accel = 0.0;
      aZ_Accel = 0.0;
      return;
   }

   double wpnVelWCS[3];
   aState.mWpnState.GetVelocityWCS(wpnVelWCS);

   double tgtVelWCS[3];
   aState.mTgtState.GetVelocityWCS(tgtVelWCS);

   double relTgtVelWCS[3];
   UtVec3d::Subtract(relTgtVelWCS, tgtVelWCS, wpnVelWCS);

   double omegaWCS[3];
   UtVec3d::CrossProduct(omegaWCS, aState.mAimRelLocWCS, relTgtVelWCS);
   UtVec3d::Multiply(omegaWCS, 1.0 / (relTgtRange * relTgtRange));

   double accelWCS[3];
   UtVec3d::CrossProduct(accelWCS, omegaWCS, wpnVelWCS);

   // If augmented pronav, also consider target acceleration.
   if (aAugmented)
   {
      double tgtAclWCS[3];
      aState.mTgtState.GetAccelerationWCS(tgtAclWCS);
      UtVec3d::AddProduct(accelWCS, accelWCS, -0.5, tgtAclWCS);
   }

   UtVec3d::Multiply(accelWCS, aProNavGain);

   // Transform the acceleration vector into the body frame.
   double accelECS[3];
   aState.mWpnState.ConvertWCSVectorToECS(accelECS, accelWCS);
   aY_Accel = accelECS[1];
   aZ_Accel = accelECS[2];
}

// ================================================================================================
// protected
void WsfGuidanceProgram::PursuitGuidance(State& aState, double aPursuitGain, double& aY_Accel, double& aZ_Accel)
{
   double relTgtRange = UtVec3d::Magnitude(aState.mAimRelLocWCS);
   if (relTgtRange < 1.0)
   {
      aY_Accel = 0.0;
      aZ_Accel = 0.0;
      return;
   }

   double wpnUnitVelECS[3] = {1.0, 0.0, 0.0}; // ECS velocity unit vector is always the ECS X axis.
   double zAxisECS[3];
   double pursuitVecECS[3];
   UtVec3d::CrossProduct(zAxisECS,
                         wpnUnitVelECS,
                         aState.mAimUnitVecECS); // Axis perpendicular to the maneuver plane (up or down)
   UtVec3d::CrossProduct(pursuitVecECS, zAxisECS, wpnUnitVelECS); // Lateral axis

   double pursuitMag = UtVec3d::Normalize(pursuitVecECS);
   aY_Accel          = pursuitMag * aPursuitGain * UtEarth::cACCEL_OF_GRAVITY * pursuitVecECS[1];
   aZ_Accel          = pursuitMag * aPursuitGain * UtEarth::cACCEL_OF_GRAVITY * pursuitVecECS[2];
}

// ================================================================================================
// ================================================================================================
//
// Start of classes for the pre-defined guidance programs.
//
// ================================================================================================
// ================================================================================================

namespace
{
// This isn't specifically a guidance program, but instead a utility object used for performing several
// diagnostic checks that assist the user in the creation of orbital launch vehicles.
class Diagnostics
{
public:
   Diagnostics()
      : mApogeeShown(false)
      , mBurnoutShown(false)
   {
   }
   void Start()
   {
      mApogeeShown  = false;
      mBurnoutShown = false;
   }
   void Execute(WsfGuidanceProgram& aProgram, WsfGuidanceProgram::State& aState);

   bool mApogeeShown;
   bool mBurnoutShown;
};

void Diagnostics::Execute(WsfGuidanceProgram& aProgram, WsfGuidanceProgram::State& aState)
{
   double loc[3];
   double vel[3];

   // Use the defined coordinate frame for these computations. Most of the time it will be ECI, but it
   // will be WCS when still within the atmosphere...

   if (aState.mWpnState.GetCoordinateFrame() == UtEntity::UtEntity::cCF_INERTIAL)
   {
      aState.mWpnState.GetLocationECI(loc);
      aState.mWpnState.GetVelocityECI(vel);
   }
   else
   {
      aState.mWpnState.GetLocationWCS(loc);
      aState.mWpnState.GetVelocityWCS(vel);
   }

   double r            = UtVec3d::Magnitude(loc);
   double orbitalSpeed = sqrt(UtEarth::cGRAVITATIONAL_PARAMETER / r);
   double v            = UtVec3d::Magnitude(vel);

   double cosLocVel = UtVec3d::DotProduct(loc, vel) / (r * v);
   double fpaRad    = UtMath::cPI_OVER_2 - acos(UtMath::Limit(cosLocVel, 1.0));

   // Check to see if an apogee condition has been reach.

   if (cosLocVel > 0.0)
   {
      // Flight path is upwards, so apogee is (potentially again) possible
      mApogeeShown = false;
   }
   else if (!mApogeeShown)
   {
      // Flight path is horizontal or downwards and this apogee condition hasn't been shown
      mApogeeShown = true;

      double deltaV_Remaining = 0.0;
      aProgram.GetPlatform()->GetMover()->GetPropertyDouble("delta_v_remaining", deltaV_Remaining);
      double burnTimeRemaining = 0.0;
      aProgram.GetPlatform()->GetMover()->GetPropertyDouble("burn_time_remaining", burnTimeRemaining);

      auto out = ut::log::info() << std::fixed << "Apogee detected.";
      aProgram.GetComputer()->PrintStatusHeader(aState.mSimTime, aState.mCurTime, out);
      out.AddNote() << "Alt: " << std::setprecision(0) << aState.mWpnState.GetAltitude() << " m";
      out.AddNote() << "Radius: " << std::setprecision(0) << r << " m";
      out.AddNote() << "Velocity: " << std::setprecision(2) << v << " m/s";
      if (deltaV_Remaining > 0.0)
      {
         out.AddNote() << "V_orbit: " << std::setprecision(2) << orbitalSpeed << " m/s";
         out.AddNote() << "Delta-V left: " << std::setprecision(2) << deltaV_Remaining << " m/s";
         out.AddNote() << "Time left: " << std::setprecision(2) << burnTimeRemaining << " s";
      }

      double burnoutSpeed = v + deltaV_Remaining;
      if ((deltaV_Remaining > 0.0) && (burnoutSpeed < orbitalSpeed))
      {
         aProgram.GetComputer()->PrintStatusHeader(aState.mSimTime, aState.mCurTime, out);
         double deltaV_Required   = orbitalSpeed - v;
         double deltaV_Deficiency = orbitalSpeed - burnoutSpeed;
         auto   def               = out.AddNote() << std::fixed << "POSSIBLE DELTA-V DEFICIENCY";
         def.AddNote() << "Required: " << std::setprecision(2) << deltaV_Required << " m/s";
         def.AddNote() << "Remaining: " << std::setprecision(2) << deltaV_Remaining << " m/s";
         def.AddNote() << "Deficiency: " << std::setprecision(2) << deltaV_Deficiency << " m/s"
                       << " (" << std::setprecision(4) << 100.0 * deltaV_Deficiency / orbitalSpeed << "%)";
      }
   }

   // Check for burnout...
   if ((!mBurnoutShown) && aProgram.GetPlatform()->IsExtrapolating()) // Mover has burned out...
   {
      mBurnoutShown = true;

      auto out = ut::log::info() << std::fixed << "Burnout detected.";
      aProgram.GetComputer()->PrintStatusHeader(aState.mSimTime, aState.mCurTime, out);
      out.AddNote() << "Alt: " << std::setprecision(0) << aState.mWpnState.GetAltitude() << " m";
      out.AddNote() << "Radius: " << std::setprecision(0) << r << " m";
      out.AddNote() << "Velocity: " << std::setprecision(2) << v << " m/s";
      out.AddNote() << "V_orbit: " << std::setprecision(2) << orbitalSpeed << " m/s";
      out.AddNote() << "Fpa: " << std::setprecision(2) << fpaRad * UtMath::cDEG_PER_RAD << " deg";


      if (v < orbitalSpeed)
      {
         aProgram.GetComputer()->PrintStatusHeader(aState.mSimTime, aState.mCurTime, out);
         out.AddNote() << std::fixed << "ORBITAL SPEED DEFICIENT AT BURNOUT";
         out.AddNote() << "Velocity: " << std::setprecision(2) << v << " m/s";
         out.AddNote() << "V_orbit: " << std::setprecision(2) << orbitalSpeed << " m/s";
         out.AddNote() << "Deficiency: " << std::setprecision(2) << orbitalSpeed - v << " m/s"
                       << " (" << std::setprecision(4) << 100.0 * (orbitalSpeed - v) / orbitalSpeed << "%)";
      }
   }
}

// =============================================================================================
// The legacy program implements the old-style guidance program which utilized the parameters
// defined directly in the phase. It is not directly referenced by the user, but is invoked when
// the user didn't specify any 'use_program' directives in a phase.

class LegacyProgram : public WsfGuidanceProgram
{
public:
   LegacyProgram();
   WsfGuidanceProgram* Clone() const override { return new LegacyProgram(*this); }
   Status              Execute(State& aState, Commands& aCommands) override;

   double mLastY_Accel;
   double mLastZ_Accel;
};

LegacyProgram::LegacyProgram()
   : WsfGuidanceProgram()
   , mLastY_Accel(0.0)
   , mLastZ_Accel(0.0)
{
}

WsfGuidanceProgram::Status LegacyProgram::Execute(State& aState, Commands& aCommands)
{
   WsfGuidanceComputer::Phase& phase            = mComputerPtr->GetCurrentPhase();
   double                      yAccel           = 0.0;
   double                      zAccel           = 0.0;
   bool                        applyGravityBias = true;

   // If the aimpoint is valid, perform guidance operations that depend on it.
   if (aState.mAimpointIsValid)
   {
      // Determine when to switch from velocity pursuit to proportional navigation:
      bool usePursuitGuidance = (phase.mVP_GainValue > 0.0);
      if ((phase.mPN_GainValue > 0.0) && (phase.mVP_GainValue > 0.0) && (aState.mAimUnitVecECS[0] > phase.mCosLOS_Offset))
      {
         usePursuitGuidance = false;
      }

      // Invoke the necessary guidance functions to compute the desired accelerations.
      if (usePursuitGuidance)
      {
         PursuitGuidance(aState, phase.mVP_GainValue, yAccel, zAccel);
      }
      else if (phase.mPN_GainValue > 0.0)
      {
         bool augmented = (phase.mPN_Method == Phase::cPN_AUGMENTED);
         ProNavGuidance(aState, phase.mPN_GainValue, augmented, yAccel, zAccel);
      }

      AngleOffsetGuidance(aState, phase, yAccel, zAccel);

      // Smooth the guidance commands to this point using the time constant.
      // We ONLY smooth the commands that are guiding to a point since the purpose is
      // to smooth out the fluctuation in the target position / line-of-sight rates.

      double timeConstant = mComputerPtr->GetTimeConstant(phase);
      if (timeConstant > 0.0)
      {
         double dt = aState.mEndTime - aState.mCurTime;
         if (dt > 0.0)
         {
            double f = exp(-dt / timeConstant);
            // Xnew = (Xold * f) + ((1 - f) * Xcmd)
            yAccel = (mLastY_Accel * f) + ((1.0 - f) * yAccel);
            zAccel = (mLastZ_Accel * f) + ((1.0 - f) * zAccel);
         }
      }
   }
   mLastY_Accel = yAccel;
   mLastZ_Accel = zAccel;

   // Process commanded flight path angle
   if (mComputerPtr->GetCommandedFlightPathAngle() != WsfGuidanceComputer::cUNDEFINED_DOUBLE)
   {
      FlightPathAngleGuidance(aState, phase, mComputerPtr->GetCommandedFlightPathAngle(), yAccel, zAccel);
      applyGravityBias = false;
   }

   // If an altitude was commanded, compute the vertical force needed to change altitude.
   if (phase.mCommandedAlt != WsfGuidanceComputer::cUNDEFINED_DOUBLE)
   {
      AltitudeGuidance(aState, phase, zAccel);
      applyGravityBias = false;
   }

   // Apply gravity bias if defined and not prohibited by a specific vertical guidance mode.
   if (applyGravityBias)
   {
      ApplyGravityBias(aState.mWpnState, phase.mGeeBiasValue, yAccel, zAccel);
   }

   ApplyLateralBias(aState.mWpnState, phase.mLateralGeeBiasValue, yAccel, zAccel);

   aCommands.mAccelCmd[1] = yAccel;
   aCommands.mAccelCmd[2] = zAccel;
   return cCONTINUE;
}

// =============================================================================================
class NullProgram : public WsfGuidanceProgram
{
public:
   WsfGuidanceProgram* Clone() const override { return new NullProgram(*this); }
   Status              Execute(State& aState, Commands& aCommands) override
   {
      UtVec3d::Set(aCommands.mAccelCmd, 0.0);
      UtVec3d::Set(aCommands.mAngleRateCmd, 0.0);
      return cCONTINUE;
   }
};

// =============================================================================================
class AltitudeProgram : public WsfGuidanceProgram
{
public:
   AltitudeProgram()
      : mCommandedAlt(cPHASE_DEFAULT_DOUBLE)
      , mCommandedAltIsAGL(false)
   {
   }

   WsfGuidanceProgram* Clone() const override { return new AltitudeProgram(*this); }
   bool                ProcessInput(UtInput& aInput) override;
   Status              Execute(State& aState, Commands& aCommands) override;

   double mCommandedAlt;
   bool   mCommandedAltIsAGL;
};

bool AltitudeProgram::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "altitude")
   {
      std::string altRef;
      aInput.ReadValueOfType(mCommandedAlt, UtInput::cLENGTH);
      // Check for optional 'msl' or 'agl' altitude reference ('msl' assumed if not specified).
      aInput.ReadValue(altRef);
      mCommandedAltIsAGL = false; // Assume 'msl'
      if (altRef == "agl")
      {
         mCommandedAltIsAGL = true;
      }
      else if (altRef != "msl")
      {
         aInput.PushBack(altRef);
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

WsfGuidanceProgram::Status AltitudeProgram::Execute(State& aState, Commands& aCommands)
{
   WsfGuidanceComputer::Phase& phase = mComputerPtr->GetCurrentPhase();

   // The commanded values in the phase are used because it allows changing from script AND it is
   // needed for pseudo-terrain following. If commanded values are explicitly specified in the
   // program then the program values are pushed into the phase where they can be accessed by
   // script and terrain following.

   if (mCommandedAlt != cPHASE_DEFAULT_DOUBLE)
   {
      phase.mCommandedAlt      = mCommandedAlt;
      phase.mCommandedAltIsAGL = mCommandedAltIsAGL;
   }

   if (phase.mCommandedAlt != WsfGuidanceComputer::cUNDEFINED_DOUBLE)
   {
      AltitudeGuidance(aState, phase, aCommands.mAccelCmd[2]);
   }
   return cCONTINUE;
}

// =============================================================================================
class AttitudeProgram : public WsfGuidanceProgram
{
public:
   AttitudeProgram()
      : mMoverPtr(nullptr)
      , mLastTime(0.0)
   {
      UtVec3d::Set(mAngle, WsfGuidanceComputer::cUNDEFINED_DOUBLE);
      UtVec3d::Set(mAngleRate, WsfGuidanceComputer::cUNDEFINED_DOUBLE);
      mIsBodyAngle[0] = mIsBodyAngle[1] = mIsBodyAngle[2] = true;
   }

   WsfGuidanceProgram* Clone() const override { return new AttitudeProgram(*this); }
   bool                ProcessInput(UtInput& aInput) override;
   void                Start(double aSimTime, double aCurTime) override;
   Status              Execute(State& aState, Commands& aCommands) override;

   void CheckAngleRate(UtInput& aInput, RotationAxis aAxis);

   WsfGuidedMover* mMoverPtr; // This program is only valid for WsfGuidedMover
   double          mAngle[3];
   double          mAngleRate[3];
   bool            mIsBodyAngle[3];
   double          mLastTime;
};

bool AttitudeProgram::ProcessInput(UtInput& aInput)
{
   // This is a little against my principles, but the rate commands are used for two different functions depending on:
   //
   // *) if an explicit angle is provided then the rate is that used to achieve that angle.
   // *) If an explicit angle is NOT provided then rate is the continuous rate to be used.
   //
   // From a user-perspective it is probably more intuitive to specify 'roll <angle> roll_rate <value>' for a specific
   // angle and 'roll_rate <value>' for a continuous value, but from a programmer-perspective it is more cumbersome. Oh
   // well, user wins...

   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "yaw") || (command == "yaw_fixed"))
   {
      aInput.ReadValueOfType(mAngle[cYAW_AXIS], UtInput::cANGLE);
      aInput.ValueInClosedRange(mAngle[cYAW_AXIS], -UtMath::cPI, UtMath::cPI);
      CheckAngleRate(aInput, cYAW_AXIS);
      mIsBodyAngle[cYAW_AXIS] = ((command == "yaw") ? true : false);
   }
   else if (command == "yaw_rate")
   {
      aInput.ReadValueOfType(mAngleRate[cYAW_AXIS], UtInput::cANGULAR_RATE);
      CheckAngleRate(aInput, cYAW_AXIS);
   }
   else if ((command == "pitch") || (command == "pitch_fixed"))
   {
      aInput.ReadValueOfType(mAngle[cPITCH_AXIS], UtInput::cANGLE);
      aInput.ValueInClosedRange(mAngle[cPITCH_AXIS], -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      CheckAngleRate(aInput, cPITCH_AXIS);
      mIsBodyAngle[cPITCH_AXIS] = ((command == "pitch") ? true : false);
   }
   else if (command == "pitch_rate")
   {
      aInput.ReadValueOfType(mAngleRate[cPITCH_AXIS], UtInput::cANGULAR_RATE);
      CheckAngleRate(aInput, cPITCH_AXIS);
   }
   else if (command == "roll")
   {
      aInput.ReadValueOfType(mAngle[cROLL_AXIS], UtInput::cANGLE);
      aInput.ValueInClosedRange(mAngle[cROLL_AXIS], -UtMath::cPI, UtMath::cPI);
      CheckAngleRate(aInput, cROLL_AXIS);
      mIsBodyAngle[cROLL_AXIS] = true;
   }
   else if (command == "roll_rate")
   {
      aInput.ReadValueOfType(mAngleRate[cROLL_AXIS], UtInput::cANGULAR_RATE);
      CheckAngleRate(aInput, cROLL_AXIS);
   }
   else if (command == "reset")
   {
      UtVec3d::Set(mAngle, 0.0);
      CheckAngleRate(aInput, cYAW_AXIS);
      CheckAngleRate(aInput, cPITCH_AXIS);
      CheckAngleRate(aInput, cROLL_AXIS);
   }
   else
   {
      myCommand = WsfGuidanceProgram::ProcessInput(aInput);
   }

   return myCommand;
}

void AttitudeProgram::CheckAngleRate(UtInput& aInput, RotationAxis aAxis)
{
   static std::string axisName[3] = {"yaw", "pitch", "roll"};
   if (mAngleRate[aAxis] != WsfGuidanceComputer::cUNDEFINED_DOUBLE)
   {
      if (mAngleRate[aAxis] == 0.0)
      {
         throw UtInput::BadValue(aInput, "'" + axisName[aAxis] + "_rate cannot be zero");
      }
      else if ((mAngleRate[aAxis] < 0.0) && (mAngle[aAxis] != WsfGuidanceComputer::cUNDEFINED_DOUBLE))
      {
         throw UtInput::BadValue(aInput,
                                 "'" + axisName[aAxis] + "_rate' cannot be negative if '" + axisName[aAxis] +
                                    "' is specified");
      }
   }
}

void AttitudeProgram::Start(double aSimTime, double aCurTime)
{
   static const double cDEFAULT_ANGLE_RATE = 10.0 * UtMath::cRAD_PER_DEG;

   WsfGuidanceProgram::Start(aSimTime, aCurTime);

   // The mover must be a WsfGuidedMover. Otherwise we simply ignore it... (This could be extended later).
   mMoverPtr = dynamic_cast<WsfGuidedMover*>(GetPlatform()->GetMover());

   // If an explict angle was specified use the default rate if none provided
   for (auto axis = 0; axis < 3; ++axis)
   {
      if ((mAngle[axis] != WsfGuidanceComputer::cUNDEFINED_DOUBLE) &&
          (mAngleRate[axis] == WsfGuidanceComputer::cUNDEFINED_DOUBLE))
      {
         mAngleRate[axis] = cDEFAULT_ANGLE_RATE;
      }
   }
   mLastTime = aCurTime;
}

WsfGuidanceProgram::Status AttitudeProgram::Execute(State& aState, Commands& aCommands)
{
   static const double cCOMPLETION_ANGLE_TOLERANCE = 0.01 * UtMath::cRAD_PER_DEG;
   // Angle limit
   static const double sAngleLimit[3] = {UtMath::cPI, UtMath::cPI_OVER_2, UtMath::cPI};

   // This program is only valid for WsfGuidedMover (may be extended later);
   if (mMoverPtr == nullptr)
   {
      auto out = ut::log::error() << "Guidance program is valid only with WSF_GUIDED_MOVER.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Type: " << GetBaseType();
      out.AddNote() << "Computer: " << mComputerPtr->GetName();
      return cCOMPLETE;
   }

   Status status = cCONTINUE;
   double dt     = aState.mEndTime - mLastTime;
   if (dt > 0.0)
   {
      // Get the current commanded attitude angles.
      // They are retrieved into a vector so this program can operate via the index of the axis of rotation.
      double currentAngle[3];
      mMoverPtr->GetCommandedAttitude(currentAngle[cYAW_AXIS], currentAngle[cPITCH_AXIS], currentAngle[cROLL_AXIS]);

      // There are two ways the user can affect the angle rate:
      // 1) If the time_constant is omitted then the angle rate will be the maximum_angular_velocity EXCEPT for any
      // residual at the end that would cause
      //    an overshoot. This is useful from a user view because the duration of the rotation can be easily computed
      //    (target_angle / maximum_angular_velocity).
      // 2) If a time_constant is provided, the angle rate will increase to the maximum angular_velocity at the start
      // and decrease to zero at the end.

      bool continuousUpdateNeeded = false;
      int  anglesTested           = 0;
      int  anglesCompleted        = 0;
      for (auto axis = 0; axis < 3; ++axis)
      {
         if (mAngle[axis] != WsfGuidanceComputer::cUNDEFINED_DOUBLE) // User specified an explicit angle
         {
            if (mIsBodyAngle[axis]) // 'yaw', 'pitch' or 'roll'
            {
               // Target angle is relative to the wind (velocity-vector oriented) frame ('yaw', 'pitch' or 'roll')
               double angleToGo              = UtMath::NormalizeAngleMinusPi_Pi(mAngle[axis] - currentAngle[axis]);
               double commandedRate          = fabs(angleToGo) / dt; // Rate required to complete in this timestep
               commandedRate                 = std::min(commandedRate, mAngleRate[axis]);
               aCommands.mAngleRateCmd[axis] = commandedRate * UtMath::Sign(angleToGo);
               ++anglesTested;
               if (fabs(angleToGo) <= cCOMPLETION_ANGLE_TOLERANCE)
               {
                  ++anglesCompleted;
               }
            }
            else
            {
               // Target angle is relative to the local fixed NED frame ('yaw_fixed' or 'pitch_fixed')
               // Convert to wind-relative angle and then treat line wind-relative update - except it is continuous
               // because the body is moving.
               double baseAngle[3];
               GetPlatform()->GetOrientationNED(baseAngle[cYAW_AXIS], baseAngle[cPITCH_AXIS], baseAngle[cROLL_AXIS]);
               double angle                  = mAngle[axis] - baseAngle[axis];
               angle                         = UtMath::Limit(angle, sAngleLimit[axis]);
               double angleToGo              = UtMath::NormalizeAngleMinusPi_Pi(angle - currentAngle[axis]);
               double commandedRate          = fabs(angleToGo) / dt; // Rate required to complete in this timestep
               commandedRate                 = std::min(commandedRate, mAngleRate[axis]);
               aCommands.mAngleRateCmd[axis] = commandedRate * UtMath::Sign(angleToGo);
               continuousUpdateNeeded        = true;
            }
         }
         else if (mAngleRate[axis] != WsfGuidanceComputer::cUNDEFINED_DOUBLE) // User specified only a rate
         {
            aCommands.mAngleRateCmd[axis] = mAngleRate[axis];
            continuousUpdateNeeded        = true;
         }
      }
      // cCOMPLETE is returned when all explicit angles have been reached and there are no other things that require
      // continuous update.
      if ((!continuousUpdateNeeded) && (anglesCompleted == anglesTested))
      {
         status = cCOMPLETE;
      }
   }
   mLastTime = aState.mEndTime;
   return status;
}

// =============================================================================================
class FlightPathAngleProgram : public WsfGuidanceProgram
{
public:
   FlightPathAngleProgram();
   WsfGuidanceProgram* Clone() const override { return new FlightPathAngleProgram(*this); }
   bool                ProcessInput(UtInput& aInput) override;
   void                Start(double aSimTime, double aCurTime) override;
   Status              Execute(State& aState, Commands& aCommands) override;

private:
   bool PitchProgramComplete(const State& aState, double aCommandedFlightPathAngle) const;

   double mCommandedFlightPathAngle;
   double mPitchRate;
   double mTimeConstant;
   double mStartFlightPathAngle;
   //! @name Values from the last call
   //@{
   double mLastTime;            //!< Simulation time
   double mLastFlightPathAngle; //!< Flight path angle
   //@}
   //! @name Conditions at time when switching the commanded acceleration from going up to going down.
   //! These are used only only if time_constant is non-zero.
   //@(
   // The simulation time when the ramp up phase ended (only used when time_constant is provided.
   double mSwitchTime; //!< Simulation time when ramp up ended
   double mSwitchFlightPathAngle;
   double mSwitchLateralAcceleration;
   bool   mFirstExecuteCall;
   bool   mPitchDownProgram;
};

FlightPathAngleProgram::FlightPathAngleProgram()
   : mCommandedFlightPathAngle(WsfGuidanceComputer::cUNDEFINED_DOUBLE)
   , mPitchRate(0.15 * UtMath::cRAD_PER_DEG)
   , mTimeConstant(1.0)
   , mStartFlightPathAngle(0.0)
   , mLastTime(0.0)
   , mLastFlightPathAngle(0.0)
   , mSwitchTime(0.0)
   , mSwitchFlightPathAngle(0.0)
   , mSwitchLateralAcceleration(0.0)
   , mFirstExecuteCall(true)
   , mPitchDownProgram(true)
{
}

bool FlightPathAngleProgram::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "flight_path_angle")
   {
      aInput.ReadValueOfType(mCommandedFlightPathAngle, UtInput::cANGLE);
      aInput.ValueGreater(mCommandedFlightPathAngle, 0.0);
      aInput.ValueLess(mCommandedFlightPathAngle, UtMath::cPI_OVER_2);
   }
   else if (command == "pitch_rate")
   {
      aInput.ReadValueOfType(mPitchRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mPitchRate, 0.0);
   }
   else if (command == "time_constant")
   {
      aInput.ReadValueOfType(mTimeConstant, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mTimeConstant, 0.0); // zero is allowed to cancel the time constant
   }
   else
   {
      myCommand = WsfGuidanceProgram::ProcessInput(aInput);
   }
   return myCommand;
}

void FlightPathAngleProgram::Start(double aSimTime, double aCurTime)
{
   WsfGuidanceProgram::Start(aSimTime, aCurTime);
   mFirstExecuteCall = true;
   mSwitchTime       = -1.0;

   // If the script variable OLC_PITCH_RATE exist in the global simulation context, use it for the pitch rate instead
   // of the input pitch rate. This is provided by weapon_tools OrbitalLaunchComputerGenerator when tuning the pitch rate.
   UtScriptContext& context = GetPlatform()->GetSimulation()->GetScriptContext().GetContext();
   UtScriptData*    dataPtr = context.FindVar("OLC_PITCH_RATE");
   if (dataPtr != nullptr)
   {
      mPitchRate = dataPtr->GetDouble() * UtMath::cRAD_PER_DEG;
   }
}

WsfGuidanceProgram::Status FlightPathAngleProgram::Execute(State& aState, Commands& aCommands)
{
   // NOTE: This program assume the vehicle is aligned with the flight path and not rolled.

   // Determine the target flight path angle. The selected value will be the first of the following that
   // results in a valid value:
   //   1) The value of the 'flight_path_angle' for this program.
   //   2) The value of the 'commanded_flight_path_angle' from the guidance computer. This will be the first
   //      of the following that returns a valid value:
   //      a) The value specified by the 'SetCommandedFlightPathAngle' script method.
   //      b) The value specified by 'commanded_flight_path_angle' in the current phase.
   //      c) The value passed from the launch computer.

   double commandedFlightPathAngle = mCommandedFlightPathAngle;
   if (commandedFlightPathAngle == WsfGuidanceComputer::cUNDEFINED_DOUBLE)
   {
      commandedFlightPathAngle = mComputerPtr->GetCommandedFlightPathAngle();
      if ((commandedFlightPathAngle == WsfGuidanceComputer::cUNDEFINED_DOUBLE) ||
          (commandedFlightPathAngle == WsfGuidanceComputer::cFROM_LAUNCH_COMPUTER))
      {
         commandedFlightPathAngle = mComputerPtr->GetLaunchComputerFlightPathAngle();
      }
   }
   if ((commandedFlightPathAngle >= UtMath::cPI_OVER_2) || (commandedFlightPathAngle <= 0.0))
   {
      auto out = ut::log::error() << "The desired flight path angle is undefined.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Computer: " << mComputerPtr->GetName();
      return cCOMPLETE;
   }
   commandedFlightPathAngle = UtMath::Limit(commandedFlightPathAngle, 0.99 * UtMath::cPI_OVER_2);

   // Compute the amount of pitch change remaining.
   double pitchDelta = commandedFlightPathAngle - aState.mFlightPathAngle;

   // The angle is made very slightly larger to ensure the transition is made.
   pitchDelta += UtMath::Sign(WsfGuidanceComputer::cFLIGHT_PATH_ANGLE_TOLERANCE, pitchDelta);

   if (mFirstExecuteCall)
   {
      mStartFlightPathAngle = aState.mFlightPathAngle;
      mLastFlightPathAngle  = aState.mFlightPathAngle;
      mPitchDownProgram     = (pitchDelta <= 0.0);
   }

   // Compute the local acceleration due to gravity in the flight path relative frame (ECS).
   // Do not use ComputeGravitationAcceleration as it uses a spherical Earth as an approximation (and should be changed,
   // but won't because of regressions). The following is faster and more accurate.
   double ownLocWCS[3];
   aState.mWpnState.GetLocationWCS(ownLocWCS);
   double g        = UtEarth::cGRAVITATIONAL_PARAMETER / UtVec3d::MagnitudeSquared(ownLocWCS); // g = G / r^2
   double gravAclZ = g * cos(aState.mFlightPathAngle);

   double speed     = aState.mWpnState.GetSpeedWCS(); // Speed for uniform circular motion approximation of pitch rate
   double cmdAclZ   = 0.0;
   double pitchAclZ = 0.0;
   double pitchRateCmd = 0.0;

   if (mTimeConstant != 0.0)
   {
      // time_constant was provided.
      //
      // The 'pitch_rate' the user provides is an AVERAGE observable rate of change of the flight path angle. The total
      // pitch required divided by 'pitch_rate' should be the time duration required by this program. 'time_constant'
      // lets the user add in a little more fidelity by noting that in reality the full pitch rate doesn't happen
      // instantaneously (i.e.: due to inertia or physical movement of nozzles or control surfaces.) It gradually lets
      // the pitch rate BUILD UP until the pitch rate is very close to the needed value.

      // Pitch up and pitch down are handled seperately. Pitch down was implemented first to support the typical orbit
      // launch. Pitch up was added later to support Virgin Orbit Launcher One. They are different enough and I didn't
      // want to cause a regression with the existing pitch down code.

      if (mPitchDownProgram)
      {
         if (PitchProgramComplete(aState, commandedFlightPathAngle))
         {
            return cCOMPLETE;
         }

         if (mSwitchTime < 0.0)
         {
            double f  = 1.0;
            double dt = aState.mEndTime - mStartTime;
            if (dt < 10.0 * mTimeConstant)
            {
               f = std::max(1.0 - exp(-dt / mTimeConstant), 0.01);
            }
            pitchAclZ = f * (mPitchRate * speed) * UtMath::Sign(-pitchDelta);
            cmdAclZ   = pitchAclZ - gravAclZ; // Gravity is also pulling us down!

            // Switch from ramping the acceleration up to ramping it down if:
            // - The maximum lateral acceleration has been reached (98% is used for the critera)
            // - Half of the pitch over has been completed.

            double pitchFrac =
               (aState.mFlightPathAngle - mStartFlightPathAngle) / (commandedFlightPathAngle - mStartFlightPathAngle);
            if ((f >= 0.98) || (pitchFrac >= 0.5))
            {
               mSwitchTime                = aState.mCurTime;
               mSwitchFlightPathAngle     = aState.mFlightPathAngle;
               mSwitchLateralAcceleration = cmdAclZ;
            }
         }
         else
         {
            // Ramping down. Simply use a linear ramp.
            double f = 1.0 - ((aState.mFlightPathAngle - mSwitchFlightPathAngle) /
                              (commandedFlightPathAngle - mSwitchFlightPathAngle));
            cmdAclZ  = f * mSwitchLateralAcceleration;
         }

         // The OBSERVABLE pitch rate is the result of the commanded acceleration plus the gravitation acceleration
         // applied during the integration of the equations of motion.
         pitchAclZ    = cmdAclZ + gravAclZ;
         pitchRateCmd = -pitchAclZ / speed;
      }
      else
      {
         // Pitch up program.
         if (mSwitchTime < 0.0)
         {
            // Ramp-up phase. Build up the commanded acceleration until the desired observable pitch rate is achieved.
            // Hold that rate until the target flight path angle is reached, and then start to ramp down.
            double f  = 1.0;
            double dt = aState.mEndTime - mStartTime;
            if (dt < 10.0 * mTimeConstant)
            {
               f = std::max(1.0 - exp(-dt / mTimeConstant), 0.01);
            }
            pitchAclZ = f * (mPitchRate * speed) * UtMath::Sign(-pitchDelta);
            cmdAclZ   = pitchAclZ - gravAclZ; // We also have to overcome gravity!
            if (PitchProgramComplete(aState, commandedFlightPathAngle))
            {
               mSwitchTime                = aState.mCurTime;
               mSwitchFlightPathAngle     = aState.mFlightPathAngle;
               mSwitchLateralAcceleration = cmdAclZ;
            }
         }
         else
         {
            // Ramp-down phase. Decrease the commanded acceleration until it reaches zero, at which point the
            // program signals it is complete. The ramp-down occurs over one time-constant.
            double f = 1.0 - ((aState.mCurTime - mSwitchTime) / mTimeConstant);
            if (f <= 0.0)
            {
               return cCOMPLETE;
            }
            cmdAclZ   = f * mSwitchLateralAcceleration;
            pitchAclZ = cmdAclZ + gravAclZ;
         }
         pitchRateCmd = -pitchAclZ / speed;
      }
   }
   else
   {
      // time_constant was NOT provided, but pitch_rate was specified (or defaulted).
      // In this mode the program completes in a fixed period of time with the pitch rate held at a constant.
      // This is not the typical mode because there is no ramp up/down of acceleration. It does, however, work like the
      // flight path angle change in the legacy program.

      if (PitchProgramComplete(aState, commandedFlightPathAngle))
      {
         return cCOMPLETE;
      }

      // Compute the time remaining to complete the program
      double endTime  = mStartTime + (fabs(mStartFlightPathAngle - commandedFlightPathAngle) / mPitchRate);
      double timeToGo = endTime - aState.mCurTime;

      // In theory we should complete the pitch over in exactly the right time, but it isn't guaranteed.
      // If the time-to-go becomes negative then we just keep extending it out. This, of course, may cause
      // some excessive pitch rates, but so be it...
      if (timeToGo <= 0.0)
      {
         timeToGo = aState.mEndTime - aState.mCurTime;
      }

      // Compute the pitch rate that needs to be commanded to get to the target angle in the time remaining...
      pitchRateCmd = pitchDelta / timeToGo;

      // Compute the total acceleration to achieve the desired pitch rate assuming uniform circular motion.
      //
      // This isn't really isn't perfectly accurate because it is based on UNIFORM (constant velocity) circular motion.
      // We are generally accelerating and the acceleration is increasing because the thrust is the same but the mass in
      // decreasing.
      //
      // Also note that the pitch acceleration is opposite the pitch direction (+Z is down)
      pitchAclZ = -pitchRateCmd * speed; // accel = omega * speed

      // The pitch acceleration is that needed to achieve the desired OBSERVABLE pitch rate and includes gravitational
      // acceleration. The commanded acceleration must subtract out gravity because it is applied when integrating the
      // equations of motion.
      cmdAclZ = pitchAclZ - gravAclZ;
   }

   aCommands.mAccelCmd[2] = cmdAclZ;

   if (mDebugEnabled)
   {
      auto out = ut::log::debug() << "FlightPathAngleProgram:";
      out.AddNote() << "T = " << aState.mCurTime - mStartTime;
      out.AddNote() << "Flight Path Angle: " << aState.mFlightPathAngle * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Commanded Pitch Rate: " << pitchRateCmd * UtMath::cDEG_PER_RAD << " deg";
      auto cmdac = out.AddNote() << "Commanded Acceleration: " << cmdAclZ;
      cmdac.AddNote() << -cmdAclZ / speed * UtMath::cDEG_PER_RAD;
      auto grac = out.AddNote() << "Gravity Acceleration: " << gravAclZ;
      grac.AddNote() << -gravAclZ / speed * UtMath::cDEG_PER_RAD;
      ;
   }

   mLastTime            = aState.mCurTime;
   mLastFlightPathAngle = aState.mFlightPathAngle;
   mFirstExecuteCall    = false;
   return cCONTINUE;
}

bool FlightPathAngleProgram::PitchProgramComplete(const State& aState, double aCommandedFlightPathAngle) const
{
   bool isComplete = false;
   // Check to see if the program is complete... This is fundamentally the same as the one for
   // 'next_phase X on_commanded_flight_path_angle' in WsfGuidanceComputer::CheckForPhaseChange.
   if ((fabs(aState.mFlightPathAngle - aCommandedFlightPathAngle) <= WsfGuidanceComputer::cFLIGHT_PATH_ANGLE_TOLERANCE) ||
       ((aState.mFlightPathAngle <= aCommandedFlightPathAngle) && (mLastFlightPathAngle >= aCommandedFlightPathAngle)) ||
       ((aState.mFlightPathAngle >= aCommandedFlightPathAngle) && (mLastFlightPathAngle <= aCommandedFlightPathAngle)))
   {
      isComplete = true;
   }
   return isComplete;
}

// =============================================================================================

// =============================================================================================
class GravityBiasProgram : public WsfGuidanceProgram
{
public:
   GravityBiasProgram()
      : mGravityBiasFactor(cPHASE_DEFAULT_DOUBLE)
      , mDiagnostics()
   {
   }
   WsfGuidanceProgram* Clone() const override { return new GravityBiasProgram(*this); }
   bool                ProcessInput(UtInput& aInput) override;
   Status              Execute(State& aState, Commands& aCommands) override;

   double      mGravityBiasFactor;
   Diagnostics mDiagnostics;
};

bool GravityBiasProgram::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "gravity_bias") || (command == "g_bias"))
   {
      aInput.ReadValue(mGravityBiasFactor);
   }
   else
   {
      myCommand = WsfGuidanceProgram::ProcessInput(aInput);
   }
   return myCommand;
}

WsfGuidanceProgram::Status GravityBiasProgram::Execute(State& aState, Commands& aCommands)
{
   if (mComputerPtr->ShowDiagnostics())
   {
      mDiagnostics.Execute(*this, aState);
   }
   double gravityBiasFactor = mGravityBiasFactor;
   if (gravityBiasFactor == cPHASE_DEFAULT_DOUBLE)
   {
      WsfGuidanceComputer::Phase& phase = mComputerPtr->GetCurrentPhase();
      gravityBiasFactor                 = phase.mGeeBiasValue;
   }
   ApplyGravityBias(aState.mWpnState, gravityBiasFactor, aCommands.mAccelCmd[1], aCommands.mAccelCmd[2]);
   return cCONTINUE;
}

// =============================================================================================
class GravityTurnProgram : public WsfGuidanceProgram
{
public:
   GravityTurnProgram()
      : mDiagnostics()
   {
   }
   WsfGuidanceProgram* Clone() const override { return new GravityTurnProgram(*this); }
   Status              Execute(State& aState, Commands& aCommands) override
   {
      if (mComputerPtr->ShowDiagnostics())
      {
         mDiagnostics.Execute(*this, aState);
      }
      aCommands.mAccelCmd[2] = 0.0;
      return cCONTINUE;
   }

   Diagnostics mDiagnostics;
};

// =============================================================================================
class InterceptProgram : public WsfGuidanceProgram
{
public:
   InterceptProgram()
      : mProNavGain(cPHASE_DEFAULT_DOUBLE)
      , mCosSwitchAngle(cPHASE_DEFAULT_DOUBLE)
      , mPursuitNavGain(cPHASE_DEFAULT_DOUBLE)
      , mLastRange(-1.0)
      , mPN_Method(WsfGuidanceComputer::Phase::cPN_PHASE_DEFAULT)
      , mLOS_RateValid(false)
      , mRangeRateValid(false)
   {
      mLastLOS_UnitVecWCS[0] = 0.0;
      mLastLOS_UnitVecWCS[1] = 0.0;
      mLastLOS_UnitVecWCS[2] = 0.0;
   }

   WsfGuidanceProgram* Clone() const override { return new InterceptProgram(*this); }
   bool                ProcessInput(UtInput& aInput) override;
   void                Start(double aSimTime, double aCurTime) override;
   Status              Execute(State& aState, Commands& aCommands) override;

#ifdef TODO_IMPROVED_PRONAV
   // JAJ - see notes preceeding the implementation.
   void NewProNavGuidance(State& aState, double aProNavGain, bool aAugmented, double& aY_Accel, double& aZ_Accel);
#endif

   double                                mProNavGain;
   double                                mCosSwitchAngle;
   double                                mPursuitNavGain;
   double                                mLastLOS_UnitVecWCS[3];
   double                                mLastRange;
   WsfGuidanceComputer::Phase::PN_Method mPN_Method;
   bool                                  mLOS_RateValid;
   bool                                  mRangeRateValid;
};

bool InterceptProgram::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "proportional_navigation_gain")
   {
      aInput.ReadValue(mProNavGain);
      aInput.ValueGreaterOrEqual(mProNavGain, 0.0);
   }
   else if (command == "proportional_navigation_limit_angle")
   {
      double switchAngle;
      aInput.ReadValueOfType(switchAngle, UtInput::cANGLE);
      aInput.ValueInClosedRange(switchAngle, 0.0, UtMath::cPI);
      mCosSwitchAngle = cos(switchAngle);
   }
   else if (command == "proportional_navigation_method")
   {
      std::string method;
      aInput.ReadValue(method);
      if (method == "pure")
      {
         mPN_Method = WsfGuidanceComputer::Phase::cPN_PURE;
      }
      else if (method == "augmented")
      {
         mPN_Method = WsfGuidanceComputer::Phase::cPN_AUGMENTED;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if ((command == "pursuit_navigation_gain") || (command == "velocity_pursuit_gain"))
   {
      aInput.ReadValue(mPursuitNavGain);
      aInput.ValueGreaterOrEqual(mPursuitNavGain, 0.0);
   }
   else
   {
      myCommand = WsfGuidanceProgram::ProcessInput(aInput);
   }
   return myCommand;
}

void InterceptProgram::Start(double aSimTime, double aCurTime)
{
   WsfGuidanceProgram::Start(aSimTime, aCurTime);
   mLOS_RateValid  = false;
   mRangeRateValid = false;
}

WsfGuidanceProgram::Status InterceptProgram::Execute(State& aState, Commands& aCommands)
{
   WsfGuidanceComputer::Phase& phase = mComputerPtr->GetCurrentPhase();
   if (aState.mAimpointIsValid)
   {
      double pursuitNavGain = mPursuitNavGain;
      if (pursuitNavGain == cPHASE_DEFAULT_DOUBLE)
      {
         pursuitNavGain = phase.mVP_GainValue;
      }
      double proNavGain = mProNavGain;
      if (proNavGain == cPHASE_DEFAULT_DOUBLE)
      {
         proNavGain = phase.mPN_GainValue;
      }
      double cosSwitchAngle = mCosSwitchAngle;
      if (cosSwitchAngle == cPHASE_DEFAULT_DOUBLE)
      {
         cosSwitchAngle = phase.mCosLOS_Offset;
      }

      // Determine when to switch from velocity pursuit to proportional navigation:
      bool usePursuitGuidance = (pursuitNavGain > 0.0);
      if ((proNavGain > 0.0) && (pursuitNavGain > 0.0) && (aState.mAimUnitVecECS[0] > cosSwitchAngle))
      {
         usePursuitGuidance = false;
      }

      // Invoke the necessary guidance functions to compute the desired accelerations.
      if (usePursuitGuidance)
      {
         PursuitGuidance(aState, pursuitNavGain, aCommands.mAccelCmd[1], aCommands.mAccelCmd[2]);
      }
      else if (proNavGain > 0.0)
      {
         auto PN_Method = mPN_Method;
         if (PN_Method == WsfGuidanceComputer::Phase::cPN_PHASE_DEFAULT)
         {
            PN_Method = phase.mPN_Method;
         }
         bool augmented = (PN_Method == WsfGuidanceComputer::Phase::cPN_AUGMENTED) ? true : false;
         ProNavGuidance(aState, proNavGain, augmented, aCommands.mAccelCmd[1], aCommands.mAccelCmd[2]);
      }
   }
   return cCONTINUE;
}

#ifdef TODO_IMPROVED_PRONAV
// JAJ
// This is formulation of proportional navigation is more 'textbook' than that found
// in the legacy formulation, but it causes regressions versus the legacy program. Thus, it
// is not used for now. This formulation results in smaller and more stable accelerations
// when close to intercept
void InterceptProgram::NewProNavGuidance(State& aState, double aProNavGain, bool aAugmented, double& aY_Accel, double& aZ_Accel)
{
   double dt = aState.mEndTime - aState.mCurTime;
   if (dt <= 0.0)
   {
      return;
   }
   double thisLOS_UnitVecWCS[3];
   double thisRange = UtVec3d::Normalize(thisLOS_UnitVecWCS, aState.mAimRelLocWCS);
   if (thisRange < 1.0)
   {
      aY_Accel = 0.0;
      aZ_Accel = 0.0;
      return;
   }

   // If this is the first call, initialize the previous range and LOS vectors.
   if (!mRangeRateValid)
   {
      mLastRange = thisRange;
   }

   if (!mLOS_RateValid)
   {
      UtVec3d::Set(mLastLOS_UnitVecWCS, thisLOS_UnitVecWCS);
   }

   // Compute the closing velocity, V_c = - R(dot) (positive is closing)
   double closingSpeed = -(thisRange - mLastRange) / dt;

   // Compute the LOS rate, lambda(dot)
   //
   // The cross product will be used to determine the direction of the required force.
   // (The vector is normal to the plane defined by the last line of sight vector and the
   // current line-of-sight vector).
   //
   // The dot product will be used to determine the absolute line of sight rate.
   // The sine of the magnitude of the cross product could also be used, but it would only
   // capture an LOS delta of up to 90 degrees. While that is probably adequate I am currently
   // sticking using the definition of the dot product, which will capture the full 180 degrees.

   double omegaVecWCS[3];
   UtVec3d::CrossProduct(omegaVecWCS, mLastLOS_UnitVecWCS, thisLOS_UnitVecWCS);
   /* double sinLOS_Delta = */ UtVec3d::Normalize(omegaVecWCS);
   double cosLOS_Delta  = UtVec3d::DotProduct(mLastLOS_UnitVecWCS, thisLOS_UnitVecWCS);
   double thisLOS_Delta = acos(UtMath::Limit(cosLOS_Delta, 1.0));
   double thisLOS_Rate  = thisLOS_Delta / dt;

   // The acceleration commands are defined to be normal to the instantaneous line-of-sight.
   double accelCmdWCS[3];
   UtVec3d::CrossProduct(accelCmdWCS, omegaVecWCS, thisLOS_UnitVecWCS);
   UtVec3d::Normalize(accelCmdWCS);

   // The commanded acceleration n_c = N' * V_c * lambda(dot)
   // cout << "Vc=" << closingSpeed << " lambdaDot=" << thisLOS_Rate * UtMath::cDEG_PER_RAD << endl;
   UtVec3d::Multiply(accelCmdWCS, accelCmdWCS, aProNavGain * closingSpeed * thisLOS_Rate);

   // Separate the commands to vertical and horizontal components so augmentation can be performed.
   double accelCmdNED[3];
   aState.mTgtState.ConvertWCSVectorToNED(accelCmdNED, accelCmdWCS);

   // Convert the commands in NED format back to WCS so they can be converted to ECS (no method for NED->ECS)
   aState.mTgtState.ConvertNEDVectorToWCS(accelCmdWCS, accelCmdNED);

   // Transform the commanded acceleration into the body frame.
   double accelCmdECS[3];
   aState.mWpnState.ConvertWCSVectorToECS(accelCmdECS, accelCmdWCS);
   aY_Accel = accelCmdECS[1];
   aZ_Accel = accelCmdECS[2];

   // Save the current range and LOS for next pass.
   mLastRange = thisRange;
   UtVec3d::Set(mLastLOS_UnitVecWCS, thisLOS_UnitVecWCS);
   mRangeRateValid = true;
   mLOS_RateValid  = true;
}
#endif

// =============================================================================================
class LegacyFlightPathAngleProgram : public WsfGuidanceProgram
{
public:
   LegacyFlightPathAngleProgram()
      : mCommandedFlightPathAngle(cPHASE_DEFAULT_DOUBLE)
   {
   }

   WsfGuidanceProgram* Clone() const override { return new LegacyFlightPathAngleProgram(*this); }
   bool                ProcessInput(UtInput& aInput) override;
   Status              Execute(State& aState, Commands& aCommands) override;

   double mCommandedFlightPathAngle;
};

bool LegacyFlightPathAngleProgram::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "flight_path_angle")
   {
      aInput.ReadValueOfType(mCommandedFlightPathAngle, UtInput::cANGLE);
      aInput.ValueGreater(mCommandedFlightPathAngle, 0.0);
      aInput.ValueLess(mCommandedFlightPathAngle, UtMath::cPI_OVER_2);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

WsfGuidanceProgram::Status LegacyFlightPathAngleProgram::Execute(State& aState, Commands& aCommands)
{
   // Determine the target flight path angle. The selected value will be the first of the following that
   // results in a valid value:
   //   1) The value of the 'flight_path_angle' for this program.
   //   2) The value of the 'commanded_flight_path_angle' from the guidance computer. This will be the first
   //      of the following that returns a valid value:
   //      a) The value specified by the 'SetCommandedFlightPathAngle' script method.
   //      b) The value specified by 'commanded_flight_path_angle' in the current phase.
   //      c) The value passed from the launch computer.

   double commandedFlightPathAngle = mCommandedFlightPathAngle;
   if (commandedFlightPathAngle == WsfGuidanceComputer::cUNDEFINED_DOUBLE)
   {
      commandedFlightPathAngle = mComputerPtr->GetCommandedFlightPathAngle();
      if ((commandedFlightPathAngle == WsfGuidanceComputer::cUNDEFINED_DOUBLE) ||
          (commandedFlightPathAngle == WsfGuidanceComputer::cFROM_LAUNCH_COMPUTER))
      {
         commandedFlightPathAngle = mComputerPtr->GetLaunchComputerFlightPathAngle();
      }
   }
   if ((commandedFlightPathAngle >= UtMath::cPI_OVER_2) || (commandedFlightPathAngle <= 0.0))
   {
      auto out = ut::log::error() << "The desired flight path angle is undefined.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Computer: " << mComputerPtr->GetName();
      return cCOMPLETE;
   }
   commandedFlightPathAngle = UtMath::Limit(commandedFlightPathAngle, 0.99 * UtMath::cPI_OVER_2);

   if (FlightPathAngleAchieved(aState, commandedFlightPathAngle))
   {
      return cCOMPLETE;
   }
   FlightPathAngleGuidance(aState,
                           mComputerPtr->GetCurrentPhase(),
                           commandedFlightPathAngle,
                           aCommands.mAccelCmd[1],
                           aCommands.mAccelCmd[2]);
   return cCONTINUE;
}

// =============================================================================================
class OrbitInsertionProgram : public WsfGuidanceProgram
{
public:
   // Action to take on orbit insertion
   enum InsertionAction
   {
      cIA_CONTINUE,   //!< Do nothing except report the program complete. User should have next stage for this complete
      cIA_EJECT_STAGE //!< eject the stage - should cause a MoverBurnout observer event
   };

   OrbitInsertionProgram()
      : mAscentGravityBias(0.0)
      , mMaximumLateralAcceleration(0.1 * UtEarth::cACCEL_OF_GRAVITY)
      , mMinimumInsertionAltitude(100000.0)
      , mCoarseAdjustmentThreshold(0.5 * UtMath::cRAD_PER_DEG)
      , mFineAdjustmentThreshold(0.05 * UtMath::cRAD_PER_DEG)
      , mInsertionAction(cIA_EJECT_STAGE)
      , mShowOrbit(false)
      , mFineAdjustmentActive(false)
      , mOrbitDeclared(false)
      , mDiagnostics()
   {
   }

   WsfGuidanceProgram* Clone() const override { return new OrbitInsertionProgram(*this); }
   bool                ProcessInput(UtInput& aInput) override;
   void                Start(double aSimTime, double aCurTime) override;
   Status              Execute(State& aState, Commands& aCommands) override;

   //! Gravity bias during ascent.
   double mAscentGravityBias;
   //! Maximum lateral acceleration to be applied during fine adjustment for orbit insertion.
   double mMaximumLateralAcceleration;
   //! The minimum altitude to consider for an orbit. This is nominally somewhere past the 'top-of-the-atmosphere'
   //! where there is effectively no drag.
   double mMinimumInsertionAltitude;
   //! The minimum flight time at which insertion will be attempted. This is typically only needed when there are more
   //! than two stages. This should be greater than the ignition time for the stage responsible for achieving orbit velocity.
   double          mCoarseAdjustmentThreshold;
   double          mFineAdjustmentThreshold;
   InsertionAction mInsertionAction;
   bool            mShowOrbit;
   bool            mFineAdjustmentActive;
   bool            mOrbitDeclared;
   Diagnostics     mDiagnostics;
};

bool OrbitInsertionProgram::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "ascent_gravity_bias") || (command == "ascent_g_bias"))
   {
      aInput.ReadValue(mAscentGravityBias);
   }
   else if (command == "maximum_lateral_acceleration")
   {
      aInput.ReadValueOfType(mMaximumLateralAcceleration, UtInput::cACCELERATION);
      aInput.ValueGreater(mMaximumLateralAcceleration, 0.0);
   }
   else if (command == "minimum_insertion_altitude")
   {
      aInput.ReadValueOfType(mMinimumInsertionAltitude, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mMinimumInsertionAltitude, 0.0);
   }
   else if (command == "coarse_adjustment_threshold")
   {
      aInput.ReadValueOfType(mCoarseAdjustmentThreshold, UtInput::cANGLE);
      aInput.ValueGreater(mCoarseAdjustmentThreshold, 0.0);
   }
   else if (command == "fine_adjustment_threshold")
   {
      aInput.ReadValueOfType(mFineAdjustmentThreshold, UtInput::cANGLE);
      aInput.ValueGreater(mFineAdjustmentThreshold, 0.0);
   }
   else if (command == "on_insertion")
   {
      std::string action;
      aInput.ReadValue(action);
      if (action == "eject_stage")
      {
         mInsertionAction = cIA_EJECT_STAGE;
      }
      else if (action == "continue")
      {
         mInsertionAction = cIA_CONTINUE;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "show_orbit")
   {
      aInput.ReadValue(mShowOrbit);
   }
   else
   {
      myCommand = WsfGuidanceProgram::ProcessInput(aInput);
   }
   return myCommand;
}

void OrbitInsertionProgram::Start(double aSimTime, double aCurTime)
{
   WsfGuidanceProgram::Start(aSimTime, aCurTime);
   mFineAdjustmentActive = false;
   mOrbitDeclared        = false;
   mDiagnostics.Start();
}

WsfGuidanceProgram::Status OrbitInsertionProgram::Execute(State& aState, Commands& aCommands)
{
   // The launching of satellite into an orbit can generally be broken in to four phases:
   //
   // 1) Lift-off: The rocket lifts off vertically and accelerates to some nominal speed (~100 m/s)
   // 2) Pitch-over: A gentle turn is performed to the correct flight path angle that will allow the rocket to fly
   //    a gravity turn to the desired altitude.
   // 3) Ascent: The rocket follows a ballistic trajectory until apogee. The altitude is pretty much determined by
   //    the terminal angle in the pitch-over phase.
   // 4) Orbit Injection: The speed is increased until orbital velocity for the alitude is reached.

   if (mOrbitDeclared)
   {
      return cCONTINUE;
   }

   if (mComputerPtr->ShowDiagnostics())
   {
      mDiagnostics.Execute(*this, aState);
   }

   // Attempting to orbit within the atmosphere is pretty much useless due to drag.
   if (aState.mWpnState.GetAltitude() <= mMinimumInsertionAltitude)
   {
      ApplyGravityBias(aState.mWpnState, mAscentGravityBias, aCommands.mAccelCmd[1], aCommands.mAccelCmd[2]);
      return cCONTINUE;
   }

   double dt = aState.mEndTime - aState.mCurTime;

   double velECI[3];
   aState.mWpnState.GetVelocityECI(velECI);
   double v = UtVec3d::Magnitude(velECI);

   double locECI[3];
   aState.mWpnState.GetLocationECI(locECI);
   double r = UtVec3d::Magnitude(locECI);
   // Acceleration of gravity at the current altitude.
   double g = UtEarth::cGRAVITATIONAL_PARAMETER / (r * r);
   // Centrifugal acceleration at the current altitude, assuming circular motion.
   double a_c = (v * v) / r;

   double cosLocVel = UtVec3d::DotProduct(locECI, velECI) / (r * v);
   double fpaRad    = UtMath::cPI_OVER_2 - acos(UtMath::Limit(cosLocVel, 1.0));

   // Determine the orbital speed for the current altitude and ratio of the current speed to the orbital speed.
   double orbitalSpeed  = sqrt(UtEarth::cGRAVITATIONAL_PARAMETER / r);
   double speedFraction = v / orbitalSpeed;

   bool   orbitNowDeclared = false;
   double gravityBias      = 0.0;
   double gravityBiasMod   = 0.0;

   if (fpaRad > 0.0)
   {
      // The flight path angle is greater than zero. If the speed is greater than orbit speed just let it continue to
      // ascend. If, however, the speed is less than the orbital speed AND the flight path angle is approaching zero,
      // attempt to hold the altitude constant and let speed the speed build towards the orbital speed.

      gravityBias = mAscentGravityBias; // assume we are in 'ascent'
      if (fpaRad <= mCoarseAdjustmentThreshold)
      {
         // If within the 'coarse adjustment' window then we are no longer considered to be in 'ascent'. Disabling any
         // gravity bias will let us naturally fall into the 'fine adjustment window'.
         gravityBias = 0.0;
      }

      if (speedFraction < 1.0)
      {
         mFineAdjustmentActive = false;
         if (fpaRad <= mFineAdjustmentThreshold)
         {
            mFineAdjustmentActive = true;
            gravityBias           = (g - a_c) / g;

            // The gravity bias computed above will almost exactly maintain the exact same flight path angle.
            // We need to drive the angle to zero so apply more bias to turn a small amount.
            //
            // omega = (fpa/dt) is the turn rate to zero the flight path angle in one timestep.
            // The lateral acceleration to achieve the turn rate is (omega * v), and will be limited by maximum_lateral_acceleration.

            gravityBiasMod = -std::min((fpaRad / dt) * v, mMaximumLateralAcceleration) / g;
         }
      }
      else // Speed exceeds that required for orbit
      {
         // The speed exceeds the orbital speed. Don't apply any gravity bias, which should force it to a higher
         // altitude. (a_c >= g and fpa > 0).

         gravityBias = 0.0;
         if (mFineAdjustmentActive)
         {
            // We were trying to hone in the orbit. Once the speed becomes acceptable while fine adjustment is active we
            // declare we are 'in orbit'
            orbitNowDeclared = true;
         }
      }
   }
   else
   {
      // The flight path angle is negative (or zero). If the speed is sufficient for orbit at this point, declare as
      // being 'in orbit' (Note that we may be slightly high, but it should occur gradually). Apply some gravity bias to
      // bring the flight path angle up.
      //
      // Note: If the engines have stopped burning or if there is not enough available thrust vectoring force, the angle
      // will continue to decrease.

      if (speedFraction < 1.0)
      {
         mFineAdjustmentActive = false;
         if (fpaRad > -mFineAdjustmentThreshold)
         {
            // Mostly the same logic as when the positive flight path angle logic, but the gravity bias for turning will
            // be added instead of subtracted.
            mFineAdjustmentActive = true;
            gravityBias           = (g - a_c) / g;
            gravityBiasMod        = std::min((-fpaRad / dt) * v, mMaximumLateralAcceleration) / g;
         }
         else
         {
            // Try to bring the trajectory up.
            gravityBias = std::min((-fpaRad / dt) * v, mMaximumLateralAcceleration) / g;
         }
      }
      else
      {
         if (mFineAdjustmentActive)
         {
            // We were trying to hone in the orbit. Once the speed becomes acceptable while fine adjustment is active we
            // declare we are 'in orbit'
            orbitNowDeclared = true;
            gravityBias      = 0.0;
         }
         else
         {
            // The speed is in excess of the orbital speed. Try to bring it to a higher altitude where the orbital speed is greater.
            gravityBias = std::min((-fpaRad / dt) * v, mMaximumLateralAcceleration) / g;
         }
      }
   }

   gravityBias = std::max(gravityBias + gravityBiasMod, 0.0);
   ApplyGravityBias(aState.mWpnState, gravityBias, aCommands.mAccelCmd[1], aCommands.mAccelCmd[2]);
   if (mDebugEnabled && (gravityBias != mAscentGravityBias))
   {
      auto out = ut::log::debug() << "Gravity Bias:";
      out.AddNote() << "T = " << aState.mCurTime;
      out.AddNote() << "Alt: " << aState.mWpnState.GetAltitude();
      out.AddNote() << "FPA: " << std::setprecision(8) << fpaRad * UtMath::cDEG_PER_RAD;
      out.AddNote() << "V: " << std::setprecision(8) << v;
      out.AddNote() << "SF: " << std::setprecision(8) << speedFraction << std::setprecision(6);
      out.AddNote() << "FAA: " << mFineAdjustmentActive;
      out.AddNote() << "Gravity: " << g;
      out.AddNote() << "a_c: " << a_c;
      out.AddNote() << "Gravity bias: " << gravityBias;
      out.AddNote() << "Gravity bias mod: " << gravityBiasMod;
   }

   Status status = cCONTINUE;
   if (orbitNowDeclared && (!mOrbitDeclared))
   {
      mOrbitDeclared = true;

      // Get the data wanted by weapon_tools. This must be saved as aux_data because a staging event may occur prior to
      // it and the data in the mover would not be valid...

      double deltaV_Remaining = 0.0;
      GetPlatform()->GetMover()->GetPropertyDouble("delta_v_remaining", deltaV_Remaining);
      double burnTimeRemaining = 0.0;
      GetPlatform()->GetMover()->GetPropertyDouble("burn_time_remaining", burnTimeRemaining);
      double fuelRemaining = 0.0;
      GetPlatform()->GetMover()->GetPropertyDouble("fuel_remaining", fuelRemaining);

      // Set the aux data variables for access by the orbital launch computer generator...
      GetPlatform()->GetAuxData().Assign(cGC_ORBIT_DECLARED, true);
      GetPlatform()->GetAuxData().Assign(cGC_DELTA_V_REMAINING, deltaV_Remaining);
      GetPlatform()->GetAuxData().Assign(cGC_BURN_TIME_REMAINING, burnTimeRemaining);
      GetPlatform()->GetAuxData().Assign(cGC_FUEL_REMAINING, fuelRemaining);

      // If requested, show the resulting orbit or the smaller diagnostic output if requested.
      if (mComputerPtr->ShowStatus() || mShowOrbit || mDebugEnabled)
      {
         auto out = ut::log::debug() << "Orbit achieved.";
         mComputerPtr->PrintStatusHeader(aState.mSimTime, aState.mCurTime, out);
         double lat;
         double lon;
         double alt;
         aState.mWpnState.GetLocationLLA(lat, lon, alt);
         double hdg;
         double pitch;
         double roll;
         aState.mWpnState.GetOrientationNED(hdg, pitch, roll);
         double velECS[3];
         aState.mWpnState.GetVelocityECS(velECS);
         UtVec3d::Normalize(velECS);
         UtVec3d::Multiply(velECS, UtVec3d::Magnitude(velECI)); // ECS velocity with ECI magnitude
         double aclECS[3];
         double aclECI[3];
         aState.mWpnState.GetAccelerationECI(aclECI);
         aState.mWpnState.GetAccelerationECS(aclECS); // ECS acceleration with ECI magnitude
         UtVec3d::Normalize(aclECS);
         UtVec3d::Multiply(aclECS, UtVec3d::Magnitude(aclECI));

         out.AddNote() << "LLA: " << UtLLPos(lat, lon) << ' ' << std::setprecision(0) << alt << " m";

         { // RAII block
            auto orient = out.AddNote() << "Orientation:" << std::setprecision(4);
            orient.AddNote() << "Heading: " << hdg * UtMath::cDEG_PER_RAD << " deg";
            orient.AddNote() << "Pitch: " << pitch * UtMath::cDEG_PER_RAD << " deg";
            orient.AddNote() << "Roll: " << roll * UtMath::cDEG_PER_RAD << " deg";
         }
         { // RAII block
            auto speed = out.AddNote() << std::setprecision(2) << "Speed:";
            speed.AddNote() << UtVec3d::Magnitude(velECS) << " [ " << velECS[0] << ' ' << velECS[1] << ' ' << velECS[2]
                            << " ] m/s";
         }
         { // RAII block
            auto acc = out.AddNote() << "Acceleration:";
            acc.AddNote() << UtVec3d::Magnitude(aclECS) / g << " [ " << aclECS[0] / g << ' ' << aclECS[1] / g << ' '
                          << aclECS[2] / g << "] g";
         }

         UtCalendar c;
         GetPlatform()->GetSimulation()->GetDateTime().GetCurrentTime(aState.mCurTime, c);
         ut::OrbitalState         orbitalState(c,
                                       GetPlatform()->GetSimulation()->GetEnvironment().GetCentralBody(),
                                       ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                       ut::OrbitalState::ReferenceFrame::cECI,
                                       ut::OrbitalState::Vector(locECI, velECI));
         const UtOrbitalElements& oe = orbitalState.GetOrbitalElementsTOD();
         double                   a  = oe.GetSemiMajorAxis();
         double                   e  = oe.GetEccentricity();
         double                   b  = a * (1.0 - e);

         { // RAII block
            auto orb = out.AddNote() << " Orbital Elements:";
            orb.AddNote() << "Semi-major axis: " << a * 0.001 << " km";
            orb.AddNote() << "Semi-minor axis: " << b * 0.001 << " km";
            orb.AddNote() << "Eccentricity: " << e;
            orb.AddNote() << "Inclination: " << oe.GetInclination() * UtMath::cDEG_PER_RAD << " deg";
            orb.AddNote() << "RAAN: " << oe.GetRAAN() * UtMath::cDEG_PER_RAD << " deg";
            orb.AddNote() << "Mean Anomaly: " << oe.GetMeanAnomaly() * UtMath::cDEG_PER_RAD << " deg";
            orb.AddNote() << "Argument of Periapsis: " << oe.GetArgumentOfPeriapsis() * UtMath::cDEG_PER_RAD << " deg";
         }

         double r0 = UtSphericalEarth::cEARTH_RADIUS;
         out.AddNote() << "  " << (a - r0) * 0.001 << " x " << (b - r0) * 0.001 << " km";
      }
      if (mComputerPtr->ShowDiagnostics())
      {
         auto out = ut::log::info() << std::fixed << "Orbit achieved.";
         mComputerPtr->PrintStatusHeader(aState.mSimTime, aState.mCurTime, out);
         out.AddNote() << "Alt: " << std::setprecision(0) << GetPlatform()->GetAltitude() << " m";
         out.AddNote() << "Radius: " << std::setprecision(0) << r << " m";
         out.AddNote() << "Velocity: " << std::setprecision(2) << v << " m/s";
         out.AddNote() << "V_orbit: " << std::setprecision(2) << orbitalSpeed << " m/s";
         if (deltaV_Remaining > 0.0)
         {
            out.AddNote() << "Delta-V Left: " << std::setprecision(2) << deltaV_Remaining << " m/s";
            out.AddNote() << "Time Left: " << std::setprecision(2) << burnTimeRemaining << " s";
         }
         out.AddNote() << "FPA: " << std::setprecision(2) << fpaRad * UtMath::cDEG_PER_RAD << " deg";
      }
      if (mComputerPtr->ShowGraphics())
      {
         mComputerPtr->DrawPoint(1.0, 1.0, 1.0, 14);
      }

      // Process the insertion action
      status = cCOMPLETE;
      switch (mInsertionAction)
      {
      case cIA_EJECT_STAGE:
         mComputerPtr->EjectStage(-1.0, -1.0);
         break;
      default:
         break;
      }
   }
   return status;
}

// =============================================================================================
// End of the anonymous namespace containing the guidance programs
// =============================================================================================
} // namespace

// ================================================================================================
// Start of WsfGuidanceProgramTypes
// ================================================================================================

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfGuidanceProgramTypes& WsfGuidanceProgramTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetGuidanceProgramTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfGuidanceProgramTypes& WsfGuidanceProgramTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetGuidanceProgramTypes();
}

// ================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
void WsfGuidanceProgramTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   mObjectFactoryList.push_back(aFactoryPtr);
}

// ================================================================================================
WsfGuidanceProgram* WsfGuidanceProgramTypes::CreateInstance(const std::string& aTypeName) const
{
   // Invoke each object factory to see if it can create an instance of the requested type.
   WsfGuidanceProgram* instancePtr = nullptr;
   auto                fli         = mObjectFactoryList.begin();
   while ((fli != mObjectFactoryList.end()) && (instancePtr == nullptr))
   {
      FactoryPtr factoryPtr = *fli;
      instancePtr           = (*factoryPtr)(aTypeName);
      ++fli;
   }
   if (instancePtr != nullptr)
   {
      instancePtr->SetType(aTypeName);
   }
   return instancePtr;
}

// ================================================================================================
//! The object factory for predefined guidance program types.
// private
WsfGuidanceProgram* WsfGuidanceProgramTypes::PredefinedObjectFactory(const std::string& aTypeName)
{
   WsfGuidanceProgram* instancePtr = nullptr;
   if (aTypeName == "_LEGACY_PROGRAM_")
   {
      instancePtr = new LegacyProgram();
   }
   else if (aTypeName == "NULL_PROGRAM")
   {
      instancePtr = new NullProgram();
   }
   else if (aTypeName == "ALTITUDE_PROGRAM")
   {
      instancePtr = new AltitudeProgram();
   }
   else if (aTypeName == "ATTITUDE_PROGRAM")
   {
      instancePtr = new AttitudeProgram();
   }
   else if ((aTypeName == "FLIGHT_PATH_ANGLE_PROGRAM") || (aTypeName == "PITCH_OVER_PROGRAM"))
   {
      instancePtr = new FlightPathAngleProgram();
   }
   else if (aTypeName == "LEGACY_FLIGHT_PATH_ANGLE_PROGRAM")
   {
      instancePtr = new LegacyFlightPathAngleProgram();
   }
   else if (aTypeName == "GRAVITY_BIAS_PROGRAM")
   {
      instancePtr = new GravityBiasProgram();
   }
   else if (aTypeName == "GRAVITY_TURN_PROGRAM")
   {
      instancePtr = new GravityTurnProgram();
   }
   else if (aTypeName == "INTERCEPT_PROGRAM")
   {
      instancePtr = new InterceptProgram();
   }
   else if (aTypeName == "ORBIT_INSERTION_PROGRAM")
   {
      instancePtr = new OrbitInsertionProgram();
   }
   return instancePtr;
}
