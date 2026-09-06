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

#include "WsfStraightLineMover.hpp"

#include <iostream>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfIntercept.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackManager.hpp"

// =================================================================================================
WsfStraightLineMover::WsfStraightLineMover(WsfScenario& aScenario)
   : WsfMover(aScenario)
   , mGuidanceMode(cPURE_PURSUIT)
   , mGuideToTruth(false)
   , mIVs()
   , mDVs()
   , mLU()
   , mCurrentSpeed(0.0)
   , mDistanceTrav(0.0)
   , mLaunchTime(0.0)
   , mAvgSpeed(0.0)
   , mMaxLatAccel(0.0)
   , // Default = not used.
   mProNavGain(3.0)
   , mMissileTimeConst(0.0)
   , mHOBSenabled(false)
   , mHOBSangle(30.0)
   , mHOBSdisableTime(1.E12)
{
   UtVec3d::Set(mUnitVelDirWCS, 0.0);
}

// =================================================================================================
WsfStraightLineMover::WsfStraightLineMover(const WsfStraightLineMover& aSrc)
   : WsfMover(aSrc)
   , mGuidanceMode(aSrc.mGuidanceMode)
   , mGuideToTruth(aSrc.mGuideToTruth)
   , mIVs(aSrc.mIVs)
   , mDVs(aSrc.mDVs)
   , mLU(aSrc.mLU)
   , mCurrentSpeed(aSrc.mCurrentSpeed)
   , mDistanceTrav(aSrc.mDistanceTrav)
   , mLaunchTime(aSrc.mLaunchTime)
   , mAvgSpeed(aSrc.mAvgSpeed)
   , mMaxLatAccel(aSrc.mMaxLatAccel)
   , mProNavGain(aSrc.mProNavGain)
   , mMissileTimeConst(aSrc.mMissileTimeConst)
   , mHOBSenabled(aSrc.mHOBSenabled)
   , mHOBSangle(aSrc.mHOBSangle)
   , mHOBSdisableTime(aSrc.mHOBSdisableTime)
{
   UtVec3d::Set(mUnitVelDirWCS, aSrc.mUnitVelDirWCS);
}

// =================================================================================================
// virtual
WsfMover* WsfStraightLineMover::Clone() const
{
   return new WsfStraightLineMover(*this);
}

// =================================================================================================
// virtual
bool WsfStraightLineMover::Initialize(double aSimTime)
{
   bool ok     = WsfMover::Initialize(aSimTime);
   mLaunchTime = aSimTime;

   if (ok)
   {
      // Assure that one and only one trajectory option has been specified.
      if ((mAvgSpeed == 0.0) && (mIVs.GetSize() == 0))
      {
         ut::log::error() << "No trajectory type was specified.";
         ok = false;
      }
      else if ((mAvgSpeed != 0.0) && (mIVs.GetSize() > 0))
      {
         ut::log::error() << "Cannot specify more than one trajectory type.";
         ok = false;
      }
      else if (mAvgSpeed != 0.0)
      {
         // Set the initial speed to the specified average speed.
         mCurrentSpeed = mAvgSpeed;
      }
      else
      {
         // Set the initial velocity using the tables. If it is zero then the handoff velocity is used.
         GetSpeed(aSimTime);
         if (mCurrentSpeed <= 0.0)
         {
            mCurrentSpeed = GetPlatform()->GetSpeed();
         }
      }

      // Initialize the velocity vector. The handoff velocity *SHOULD NOT* be zero...
      double velNED[3];
      GetPlatform()->GetVelocityNED(velNED);
      double speed = UtVec3d::Normalize(velNED); // Velocity vector -> velocity unit vector
      if (speed == 0.0)
      {
         ut::log::error() << "Ejection velocity is zero.";
         ok = false;
      }
      // Align the body with the velocity vector.
      double hdg   = atan2(velNED[1], velNED[0]);
      double pitch = asin(-velNED[2]);
      GetPlatform()->SetOrientationNED(hdg, pitch, 0.0);

      UtVec3d::Multiply(velNED, mCurrentSpeed); // Velocity unit vector -> velocity vector
      GetPlatform()->SetVelocityNED(velNED);

      // Set the initial WCS direction of the velocity vector.
      GetPlatform()->GetVelocityWCS(mUnitVelDirWCS);
      UtVec3d::Normalize(mUnitVelDirWCS);
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfStraightLineMover::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "average_speed")
   {
      aInput.ReadValueOfType(mAvgSpeed, UtInput::cSPEED);
      aInput.ValueGreater(mAvgSpeed, 0.0);
      // Deactivate any speed vs. time curve
      mIVs.Resize(0);
      mDVs.Resize(0);
   }
   else if (command == "tof_and_speed")
   {
      double              tof;
      double              speed;
      std::vector<double> times;
      std::vector<double> speeds;

      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         // The keyword we just read is NOT the "end_tof_vs_speed" keyword,
         // so it must be the time value.  Push it back into the stream,
         // and re-read it as a time value.
         UtInput&    input = inputBlock.GetInput();
         std::string time  = input.GetCommand();
         input.PushBack(time);

         aInput.ReadValueOfType(tof, UtInput::cTIME);
         aInput.ValueGreaterOrEqual(tof, 0.0);

         aInput.ReadValueOfType(speed, UtInput::cSPEED);
         aInput.ValueGreater(speed, 0.0);

         times.push_back(tof);
         speeds.push_back(speed);
      }
      mIVs.SetValues(times);
      size_t size = mIVs.GetSize();
      mDVs.Resize(mIVs.GetSize());
      for (size_t i = 0; i != size; ++i)
      {
         mDVs.Set(speeds[i], i);
         if ((i != 0) && (times[i - 1] >= times[i]))
         {
            throw UtInput::BadValue(inputBlock.GetInput(), "Time of Flight values must be in increasing order.");
         }
      }

      // Deactivate average_speed.
      mAvgSpeed = 0.0;
   }
   else if (command == "maximum_lateral_acceleration")
   {
      aInput.ReadValueOfType(mMaxLatAccel, UtInput::cACCELERATION);
      aInput.ValueGreater(mMaxLatAccel, 0.0);
   }
   else if (command == "guidance_mode")
   {
      std::string guidanceMode;
      aInput.ReadValue(guidanceMode);
      if (guidanceMode == "lead_pursuit")
      {
         mGuidanceMode = cLEAD_PURSUIT;
      }
      else if (guidanceMode == "pure_pursuit")
      {
         mGuidanceMode = cPURE_PURSUIT;
      }
      else if (guidanceMode == "pronav")
      {
         mGuidanceMode = cPRONAV;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "guide_to_truth")
   {
      aInput.ReadValue(mGuideToTruth);
   }
   else if (command == "pronav_gain")
   {
      aInput.ReadValue(mProNavGain);
      aInput.ValueGreater(mProNavGain, 0.0);
   }
   else if (command == "missile_time_constant")
   {
      aInput.ReadValueOfType(mMissileTimeConst, UtInput::cTIME);
      aInput.ValueGreater(mMissileTimeConst, 0.0);
   }
   else if (command == "HOBS_enabled")
   {
      aInput.ReadValue(mHOBSenabled);
   }
   else if (command == "HOBS_angle")
   {
      aInput.ReadValueOfType(mHOBSangle, UtInput::cANGLE);
      aInput.ValueGreater(mHOBSangle, 0.0);
   }
   else if (command == "HOBS_disable_time")
   {
      aInput.ReadValueOfType(mHOBSdisableTime, UtInput::cTIME);
      aInput.ValueGreater(mHOBSdisableTime, 0.0);
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================================================
// protected
void WsfStraightLineMover::GetSpeed(double aSimTime)
{
   double tof = aSimTime - mLaunchTime;
   mLU.Lookup(mIVs, tof);
   mCurrentSpeed = TblEvaluate(mDVs, mLU);
}

// =================================================================================================
// virtual
void WsfStraightLineMover::Update(double aSimTime)
{
   double dt = aSimTime - mLastUpdateTime;
   if (dt > mUpdateTimeTolerance)
   {
      if (mAvgSpeed == 0.0)
      {
         // Do a 1-dimensional table lookup for mCurrentSpeed.
         GetSpeed(aSimTime);
      }

      double deltaWCS[3] = {0.0, 0.0, 0.0};
      double velWCS[3]   = {0.0, 0.0, 0.0};

      double tgtLocWCS[3] = {0.0, 0.0, 0.0};
      double tgtVelWCS[3] = {0.0, 0.0, 0.0};

      double relPosWCS[3] = {0.0, 0.0, 0.0};

      double cmdAccWCS[3] = {0.0, 0.0, 0.0};
      double cmdAccMag(0.0);
      double rstAccWCS[3] = {0.0, 0.0, 0.0};

      double unitRelPosWCS[3];                     // aka an unit aim vector
      UtVec3d::Set(unitRelPosWCS, mUnitVelDirWCS); // Initialize to the current movement direction
      UtVec3d::Set(relPosWCS, unitRelPosWCS);      // For HOBS, in case no target track

      double myLocWCS[3];
      double myVelWCS[3];
      double myAccWCS[3];
      GetPlatform()->GetLocationWCS(myLocWCS);
      GetPlatform()->GetVelocityWCS(myVelWCS);
      GetPlatform()->GetAccelerationWCS(myAccWCS);

      const WsfTrack* trackPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
      if (trackPtr != nullptr)
      {
         double relVelWCS[3] = {0.0, 0.0, 0.0};

         bool tgtStateValid = false;
         if (mGuideToTruth)
         {
            WsfPlatform* truthPtr = GetSimulation()->GetPlatformByIndex(trackPtr->GetTargetIndex());
            if (truthPtr != nullptr)
            {
               tgtStateValid = true;
               truthPtr->GetLocationWCS(tgtLocWCS);
               truthPtr->GetVelocityWCS(tgtVelWCS);

               // Extrapolate position forward by the difference between the simulation time and the last platform update time.
               double deltaT = std::max(0.0, aSimTime - truthPtr->GetLastUpdateTime());
               UtVec3d::AddProduct(tgtLocWCS, tgtLocWCS, deltaT, tgtVelWCS);
               UtVec3d::Subtract(relPosWCS, tgtLocWCS, myLocWCS);
               UtVec3d::Subtract(relVelWCS, tgtVelWCS, myVelWCS);
            }
         }

         if ((!tgtStateValid) && trackPtr->GetExtrapolatedLocationWCS(aSimTime, tgtLocWCS))
         {
            UtVec3d::Subtract(relPosWCS, tgtLocWCS, myLocWCS);
            if (trackPtr->VelocityValid())
            {
               trackPtr->GetVelocityWCS(tgtVelWCS);
               UtVec3d::Subtract(relVelWCS, tgtVelWCS, myVelWCS);
            }
         }

         // Update the unit relative position
         UtVec3d::Normalize(unitRelPosWCS, relPosWCS);

         if ((mGuidanceMode == cPRONAV) && (!UtVec3d::Equals(tgtVelWCS, 0.0)))
         {
            double relPosMag2 = UtVec3d::MagnitudeSquared(relPosWCS);
            double LOSRate[3];
            UtVec3d::CrossProduct(LOSRate, relPosWCS, relVelWCS);
            UtVec3d::Divide(LOSRate, LOSRate, relPosMag2);
            UtVec3d::CrossProduct(cmdAccWCS, LOSRate, myVelWCS);
            UtVec3d::Multiply(cmdAccWCS, mProNavGain);
            cmdAccMag = UtVec3d::Magnitude(cmdAccWCS);
            if (cmdAccMag == 0.0)
            {
               cmdAccMag = 0.0001;
            }
         }
         else if ((mGuidanceMode == cLEAD_PURSUIT) && (!UtVec3d::Equals(tgtVelWCS, 0.0)))
         {
            double intLocWCS[3];
            /* double tti = */ WsfIntercept::Intercept(myLocWCS, GetPlatform()->GetSpeed(), tgtLocWCS, tgtVelWCS, intLocWCS);
            UtVec3d::Set(tgtLocWCS, intLocWCS);
            UtVec3d::Subtract(relPosWCS, tgtLocWCS, myLocWCS);
            UtVec3d::Normalize(unitRelPosWCS, relPosWCS);

            double orthogonalVector[3];
            UtVec3d::CrossProduct(orthogonalVector, mUnitVelDirWCS, unitRelPosWCS);
            double desiredAccDir[3];
            UtVec3d::CrossProduct(desiredAccDir, orthogonalVector, mUnitVelDirWCS);
            UtVec3d::Normalize(desiredAccDir);
            double cosTgtLOS = UtVec3d::DotProduct(unitRelPosWCS, mUnitVelDirWCS) /
                               (UtVec3d::Magnitude(unitRelPosWCS) * UtVec3d::Magnitude(mUnitVelDirWCS));
            cosTgtLOS = std::min(std::max(-1.0, cosTgtLOS), 1.0);
            cmdAccMag = acos(cosTgtLOS) * mCurrentSpeed / dt;
            if (cmdAccMag == 0.0)
            {
               cmdAccMag = 0.0001;
            }
            UtVec3d::Multiply(cmdAccWCS, desiredAccDir, cmdAccMag);
         }
         else // by default guidance is PURE_PURSUIT where the weapons wants to point at the target
         {
            double orthogonalVector[3];
            UtVec3d::CrossProduct(orthogonalVector, mUnitVelDirWCS, unitRelPosWCS);
            double desiredAccDir[3];
            UtVec3d::CrossProduct(desiredAccDir, orthogonalVector, mUnitVelDirWCS);
            UtVec3d::Normalize(desiredAccDir);
            double cosTgtLOS = UtVec3d::DotProduct(unitRelPosWCS, mUnitVelDirWCS) /
                               (UtVec3d::Magnitude(unitRelPosWCS) * UtVec3d::Magnitude(mUnitVelDirWCS));
            cosTgtLOS = std::min(std::max(-1.0, cosTgtLOS), 1.0);
            cmdAccMag = acos(cosTgtLOS) * mCurrentSpeed / dt;
            if (cmdAccMag == 0.0)
            {
               cmdAccMag = 0.0001;
            }
            UtVec3d::Multiply(cmdAccWCS, desiredAccDir, cmdAccMag);
         }
      }

      // This is a rudimentary HOBS capability.
      // The weapon makes a max g level turn until bearing limit is met or time runs out
      if (mHOBSenabled)
      {
         // determine relative bearing from weapon heading to LOS i.e. angle between the two vectors in a horizontal plane
         double verticalVector[3];
         double myVelHorizon[3];
         double relPosHorizon[3];
         double tempVector[3];
         UtVec3d::Set(verticalVector, myLocWCS);
         UtVec3d::Normalize(verticalVector);

         double tempDot = UtVec3d::DotProduct(myVelWCS, verticalVector);
         UtVec3d::Multiply(tempVector, verticalVector, tempDot);
         UtVec3d::Subtract(myVelHorizon, myVelWCS, tempVector);
         UtVec3d::Normalize(myVelHorizon);

         tempDot = UtVec3d::DotProduct(relPosWCS, verticalVector);
         UtVec3d::Multiply(tempVector, verticalVector, tempDot);
         UtVec3d::Subtract(relPosHorizon, relPosWCS, tempVector);
         UtVec3d::Normalize(relPosHorizon);

         double cosLOSHorizon = UtVec3d::DotProduct(relPosHorizon, myVelHorizon) /
                                (UtVec3d::Magnitude(relPosHorizon) * UtVec3d::Magnitude(myVelHorizon));
         if (cosLOSHorizon > 1.0)
         {
            cosLOSHorizon = 1.0;
         }
         if (cosLOSHorizon < -1.0)
         {
            cosLOSHorizon = -1.0;
         }

         if ((cosLOSHorizon < cos(mHOBSangle)) && (mHOBSdisableTime > (aSimTime - mLaunchTime)))
         {
            double orthogonalVector[3];
            double desiredAccDir[3];
            double rotAxisVector[3];

            UtVec3d::CrossProduct(orthogonalVector, mUnitVelDirWCS, unitRelPosWCS);
            double turnDirection = UtVec3d::DotProduct(orthogonalVector, verticalVector);
            UtVec3d::Multiply(rotAxisVector, verticalVector, turnDirection);
            UtVec3d::CrossProduct(desiredAccDir, rotAxisVector, mUnitVelDirWCS);
            UtVec3d::Normalize(desiredAccDir);
            UtVec3d::Multiply(cmdAccWCS, desiredAccDir, mMaxLatAccel);
            cmdAccMag = UtVec3d::Magnitude(cmdAccWCS);
         }
      }

      // If we allow unlimited lateral acceleration, then we could instantly put the
      // velocity vector right on the desired aim vector... but if not, we can only let
      // it "creep" in that direction.  The default is to allow unlimited acceleration.
      if (mMaxLatAccel != 0.0)
      {
         if (cmdAccMag > mMaxLatAccel)
         {
            UtVec3d::Multiply(cmdAccWCS, (mMaxLatAccel / cmdAccMag));
         }

         if (mMissileTimeConst != 0.0)
         {
            rstAccWCS[0] = (1 - exp(-2 * UtMath::cPI * dt / mMissileTimeConst)) * cmdAccWCS[0] +
                           exp(-2 * UtMath::cPI * dt / mMissileTimeConst) * myAccWCS[0];
            rstAccWCS[1] = (1 - exp(-2 * UtMath::cPI * dt / mMissileTimeConst)) * cmdAccWCS[1] +
                           exp(-2 * UtMath::cPI * dt / mMissileTimeConst) * myAccWCS[1];
            rstAccWCS[2] = (1 - exp(-2 * UtMath::cPI * dt / mMissileTimeConst)) * cmdAccWCS[2] +
                           exp(-2 * UtMath::cPI * dt / mMissileTimeConst) * myAccWCS[2];
         }
         else
         {
            UtVec3d::Set(rstAccWCS, cmdAccWCS);
         }
      }
      else
      {
         UtVec3d::Set(rstAccWCS, cmdAccWCS);
      }

      // If necessary, the deltaV is constrained by a lateral acceleration
      // limit, so now use it to alter the current direction of velocity.
      GetPlatform()->SetAccelerationWCS(rstAccWCS);
      UtVec3d::AddProduct(mUnitVelDirWCS, myVelWCS, dt, rstAccWCS);
      UtVec3d::Normalize(mUnitVelDirWCS);

      double dRange = dt * mCurrentSpeed;
      mDistanceTrav += dRange;

      UtVec3d::Multiply(deltaWCS, mUnitVelDirWCS, dRange);
      GetPlatform()->IncrementLocationWCS(deltaWCS);

      UtVec3d::Multiply(velWCS, mUnitVelDirWCS, mCurrentSpeed);
      GetPlatform()->SetVelocityWCS(velWCS);

      double velNED[3];
      GetPlatform()->GetVelocityNED(velNED);

      double hdg   = atan2(velNED[1], velNED[0]);
      double pitch = asin(-velNED[2] / mCurrentSpeed);
      GetPlatform()->SetOrientationNED(hdg, pitch, 0.0);

      mLastUpdateTime = aSimTime;
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Location Report:";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Launch Time: " << aSimTime - mLaunchTime;
         out.AddNote() << "WCS Coordinates: " << myLocWCS[0] << ", " << myLocWCS[1] << ", " << myLocWCS[2];
         out.AddNote() << "Velocity WCS: " << myVelWCS[0] << ", " << myVelWCS[1] << ", " << myVelWCS[2];
         out.AddNote() << "Target Location WCS: " << tgtLocWCS[0] << ", " << tgtLocWCS[1] << ", " << tgtLocWCS[2];
         out.AddNote() << "Target Velocity WCS: " << tgtVelWCS[0] << ", " << tgtVelWCS[1] << ", " << tgtVelWCS[2];
      }
   }
}
