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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "TargetConfig.hpp"

#include <iostream> // TODO TEMP

#include "InputConfig.hpp"
#include "Platform.hpp"
#include "RunConfig.hpp"
#include "Simulation.hpp"
#include "Task.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfAirMover.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoute.hpp"
#include "WsfWaypoint.hpp"

namespace engage
{

// =================================================================================================
TargetConfig::TargetConfig()
   : mTargetType("TARGET_TYPE")
   , mGridDownRangeMin(0.0)
   , mGridDownRangeMax(0.0)
   , mGridDownRangeStep(0.0)
   , mGridDownRangeUnits("m")
   , mGridCrossRangeMin(0.0)
   , mGridCrossRangeMax(0.0)
   , mGridCrossRangeStep(0.0)
   , mGridCrossRangeUnits("m")
   , mGridAltitudeMin(0.0)
   , mGridAltitudeMax(0.0)
   , mGridAltitudeStep(0.0)
   , mGridAltitudeUnits("m")
   , mGridSpeed(134.0)
   , mGridMach(0.0)
   , mGridSpeedUnits("m/s")
   , mGridPointCount(0)
   , mGridDownRangeCount(0)
   , mGridCrossRangeCount(0)
   , mGridAltitudeCount(0)
   , mSimpleX(10000.0)
   , mSimpleY(0.0)
   , mSimpleZ(5000.0)
   , mSimpleSpeed(134.0)
   ,
   // NOTE: ESAMS used a value of 180 degrees counter clockwise from east. We use conventional heading
   //       (clockwise from north).
   mSimpleHeading(270.0 * UtMath::cRAD_PER_DEG)
   , mSimplePitch(0.0)
   , mSimpleRoll(0.0)
   , mFlightPath()
   , mPointCount(0)
   , mUseTargetPath(false)
{
}

// =================================================================================================
//! This is called when all of the input has been read.
bool TargetConfig::Complete(bool aTargetGridUsed)
{
   if (aTargetGridUsed)
   {
      mPointCount = std::max(1, mGridPointCount);
      return true;
   }
   else
   {
      double count;
      count                = (mGridAltitudeMax - mGridAltitudeMin + (0.5 * mGridAltitudeStep)) / mGridAltitudeStep;
      mGridAltitudeCount   = static_cast<int>(count + 1);
      mGridCrossRangeCount = 1;
      mGridDownRangeCount  = 1;

      mPointCount     = std::max(1, mGridAltitudeCount);
      mGridPointCount = mPointCount;
      return true;
   }
}

// =================================================================================================
bool TargetConfig::CreatePlatform(Simulation& aSimulation, const Task& aTask) const
{
   bool ok(false);

   double x(mSimpleX);
   double y(mSimpleY);
   double z(mSimpleZ);
   double speed(mSimpleSpeed);
   double heading(mSimpleHeading);
   double pitch(mSimplePitch);
   double roll(mSimpleRoll);
   if (mGridPointCount > 0)
   {
      int pointIndex    = aTask.GetTargetIndex();
      int altitudeIndex = pointIndex / (mGridDownRangeCount * mGridCrossRangeCount);
      pointIndex -= altitudeIndex * (mGridDownRangeCount * mGridCrossRangeCount);
      int downRangeIndex  = pointIndex / mGridCrossRangeCount;
      int crossRangeIndex = pointIndex - (downRangeIndex * mGridCrossRangeCount);

      x     = mGridDownRangeMin + (mGridDownRangeStep * downRangeIndex);
      y     = mGridCrossRangeMin + (mGridCrossRangeStep * crossRangeIndex);
      z     = mGridAltitudeMin + (mGridAltitudeStep * altitudeIndex);
      speed = mGridSpeed;
   }
   else if (!mFlightPath.empty())
   {
      x       = mFlightPath[0].mPosXYZ[0];
      y       = mFlightPath[0].mPosXYZ[1];
      z       = mFlightPath[0].mPosXYZ[2];
      heading = mFlightPath[0].mYaw;
      pitch   = mFlightPath[0].mPitch;
      roll    = mFlightPath[0].mRoll;
      speed   = UtVec3d::Magnitude(mFlightPath[0].mVelXYZ);
   }
   else if (!mFlightRoute.empty())
   {
      x = mFlightRoute[0].mPosXYZ[0];
      y = mFlightRoute[0].mPosXYZ[1];
      z = mFlightRoute[0].mPosXYZ[2];
      if (mFlightRoute.size() > 1)
      {
         double dX = mFlightRoute[1].mPosXYZ[0] - x;
         double dY = mFlightRoute[1].mPosXYZ[1] - y;
         heading   = atan2(dX, dY);
      }
      speed = mFlightRoute[0].mSpeed;
   }

   Platform* targetPtr = new Platform(aTask.GetRunConfig(), aSimulation, mTargetType, "target");
   aSimulation.SetTarget(targetPtr);
   if (targetPtr->IsValid())
   {
      if (!mUseTargetPath)
      {
         targetPtr->SetLocationXYZ(x, y, z);
         targetPtr->SetSpeedAndOrientationXYZ(speed, heading, pitch, roll);
      }

      ok = targetPtr->AddToSimulation(0.0, aSimulation);

      if (ok)
      {
         if (!mFlightPath.empty())
         {
            targetPtr->GetWsfPlatform()->SetMover(nullptr);
            targetPtr->SetFlightPath(mFlightPath);
         }
         else if (!mFlightRoute.empty())
         {
            WsfAirMover* moverPtr = dynamic_cast<WsfAirMover*>(targetPtr->GetWsfPlatform()->GetMover());
            if (moverPtr == nullptr)
            {
               moverPtr = new WsfAirMover(targetPtr->GetWsfPlatform()->GetScenario());
               targetPtr->GetWsfPlatform()->SetMover(moverPtr);
            }

            WsfRoute routePtr;
            for (size_t i = 0; i < mFlightRoute.size(); ++i)
            {
               double x     = mFlightRoute[i].mPosXYZ[0];
               double y     = mFlightRoute[i].mPosXYZ[1];
               double z     = mFlightRoute[i].mPosXYZ[2];
               double speed = mFlightRoute[i].mSpeed;

               double           lat;
               double           lon;
               double           alt(z);
               const Utilities& utilities(aTask.GetRunConfig().GetUtilities());
               utilities.ConvertXY_ToLL(x, y, lat, lon);

               WsfWaypoint waypoint(lat, lon, alt, speed);
               routePtr.Insert(static_cast<int>(i), waypoint);
            }
            moverPtr->SetRoute(0.0, routePtr);
         }
      }
   }
   return ok;
}

// =================================================================================================
bool TargetConfig::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "target_type")
   {
      aInput.ReadValue(mTargetType);
   }
   else if (command == "target_grid")
   {
      mUseTargetPath = false;
      mFlightPath.clear();
      mFlightRoute.clear();

      mGridDownRangeMin   = 0.0;
      mGridDownRangeMax   = 0.0;
      mGridDownRangeStep  = 0.0;
      mGridCrossRangeMin  = 0.0;
      mGridCrossRangeMax  = 0.0;
      mGridCrossRangeStep = 0.0;
      mGridAltitudeMin    = 0.0;
      mGridAltitudeMax    = 0.0;
      mGridAltitudeStep   = 0.0;

      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "down_range")
         {
            aInput.ReadValue(command);
            aInput.StringEqual(command, "from");
            aInput.ReadValueOfType(mGridDownRangeMin, UtInput::cLENGTH);
            aInput.ReadValue(command);
            aInput.StringEqual(command, "to");
            aInput.ReadValueOfType(mGridDownRangeMax, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(mGridDownRangeMax, mGridDownRangeMin);
            aInput.ReadValue(command);
            aInput.StringEqual(command, "by");
            aInput.ReadValue(mGridDownRangeStep);
            aInput.ReadValue(mGridDownRangeUnits);
            mGridDownRangeStep = aInput.ConvertValue(mGridDownRangeStep, mGridDownRangeUnits, UtInput::cLENGTH);
            aInput.ValueGreater(mGridDownRangeStep, 0.0);
         }
         else if (command == "cross_range")
         {
            aInput.ReadValue(command);
            aInput.StringEqual(command, "from");
            aInput.ReadValueOfType(mGridCrossRangeMin, UtInput::cLENGTH);
            aInput.ReadValue(command);
            aInput.StringEqual(command, "to");
            aInput.ReadValueOfType(mGridCrossRangeMax, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(mGridCrossRangeMax, mGridCrossRangeMin);
            aInput.ReadValue(command);
            aInput.StringEqual(command, "by");
            aInput.ReadValue(mGridCrossRangeStep);
            aInput.ReadValue(mGridCrossRangeUnits);
            mGridCrossRangeStep = aInput.ConvertValue(mGridCrossRangeStep, mGridCrossRangeUnits, UtInput::cLENGTH);
            aInput.ValueGreater(mGridCrossRangeStep, 0.0);
         }
         else if (command == "altitude")
         {
            aInput.ReadValue(command);
            aInput.StringEqual(command, "from");
            aInput.ReadValueOfType(mGridAltitudeMin, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(mGridAltitudeMin, 0.0);
            aInput.ReadValue(command);
            aInput.StringEqual(command, "to");
            aInput.ReadValueOfType(mGridAltitudeMax, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(mGridAltitudeMax, mGridAltitudeMin);
            aInput.ReadValue(command);
            aInput.StringEqual(command, "by");
            aInput.ReadValue(mGridAltitudeStep);
            aInput.ReadValue(mGridAltitudeUnits);
            mGridAltitudeStep = aInput.ConvertValue(mGridAltitudeStep, mGridAltitudeUnits, UtInput::cLENGTH);
            aInput.ValueGreater(mGridAltitudeStep, 0.0);
         }
         else if (command == "speed")
         {
            aInput.ReadValue(mGridSpeed);
            aInput.ValueGreaterOrEqual(mGridSpeed, 0.0);
            aInput.ReadValue(mGridSpeedUnits);
            mGridSpeed = aInput.ConvertValue(mGridSpeed, mGridSpeedUnits, UtInput::cSPEED);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // Compute the points in each direction and the total number of points.
      double count;
      count               = (mGridDownRangeMax - mGridDownRangeMin + (0.5 * mGridDownRangeStep)) / mGridDownRangeStep;
      mGridDownRangeCount = static_cast<int>(count + 1);
      count = (mGridCrossRangeMax - mGridCrossRangeMin + (0.5 * mGridCrossRangeStep)) / mGridCrossRangeStep;
      mGridCrossRangeCount = static_cast<int>(count + 1);
      count                = (mGridAltitudeMax - mGridAltitudeMin + (0.5 * mGridAltitudeStep)) / mGridAltitudeStep;
      mGridAltitudeCount   = static_cast<int>(count + 1);
      mGridPointCount      = mGridDownRangeCount * mGridCrossRangeCount * mGridAltitudeCount;
   }
   else if (command == "simple_path")
   {
      mUseTargetPath  = false;
      mGridPointCount = 0;
      mFlightPath.clear();
      mFlightRoute.clear();

      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "x")
         {
            aInput.ReadValueOfType(mSimpleX, UtInput::cLENGTH);
         }
         else if (command == "y")
         {
            aInput.ReadValueOfType(mSimpleY, UtInput::cLENGTH);
         }
         else if (command == "z")
         {
            aInput.ReadValueOfType(mSimpleZ, UtInput::cLENGTH);
         }
         else if (command == "speed")
         {
            aInput.ReadValueOfType(mSimpleSpeed, UtInput::cSPEED);
            aInput.ValueGreaterOrEqual(mSimpleSpeed, 0.0);
         }
         else if (command == "heading")
         {
            aInput.ReadValueOfType(mSimpleHeading, UtInput::cANGLE);
         }
         // else if (command == "pitch")
         //{
         //    aInput.ReadValueOfType(mSimplePitch, UtInput::cANGLE);
         // }
         // else if (command == "roll")
         //{
         //    aInput.ReadValueOfType(mSimpleRoll, UtInput::cANGLE);
         // }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "flight_path")
   {
      mUseTargetPath  = false;
      mGridPointCount = 0;
      mFlightPath.clear();
      mFlightRoute.clear();

      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         inputBlock.GetInput().PushBack(command);
         std::string units;
         PathPoint   point;
         double      multiplier;

         aInput.ReadValueOfType(point.mTime, UtInput::cTIME);

         aInput.ReadValue(point.mPosXYZ[0]);
         aInput.ReadValue(point.mPosXYZ[1]);
         aInput.ReadValue(point.mPosXYZ[2]);
         aInput.ReadValue(units);
         multiplier = aInput.ConvertValue(1.0, units, UtInput::cLENGTH);
         UtVec3d::Multiply(point.mPosXYZ, multiplier);

         aInput.ReadValue(point.mVelXYZ[0]);
         aInput.ReadValue(point.mVelXYZ[1]);
         aInput.ReadValue(point.mVelXYZ[2]);
         aInput.ReadValue(units);
         multiplier = aInput.ConvertValue(1.0, units, UtInput::cSPEED);
         UtVec3d::Multiply(point.mVelXYZ, multiplier);

         aInput.ReadValue(point.mPitch);
         aInput.ReadValue(point.mRoll);
         aInput.ReadValue(point.mYaw);
         aInput.ReadValue(units);
         multiplier = aInput.ConvertValue(1.0, units, UtInput::cANGLE);
         point.mPitch *= multiplier;
         point.mRoll *= multiplier;
         point.mYaw *= multiplier;

         mFlightPath.push_back(point);
      }
   }
   else if (command == "flight_route")
   {
      mUseTargetPath  = false;
      mGridPointCount = 0;
      mFlightPath.clear();
      mFlightRoute.clear();

      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         inputBlock.GetInput().PushBack(command);
         RoutePoint point;

         aInput.ReadValue(point.mPosXYZ[0]);
         aInput.ReadValue(point.mPosXYZ[1]);
         aInput.ReadValue(point.mPosXYZ[2]);
         std::string units;
         aInput.ReadValue(units);
         double multiplier = aInput.ConvertValue(1.0, units, UtInput::cLENGTH);
         UtVec3d::Multiply(point.mPosXYZ, multiplier);
         aInput.ReadValueOfType(point.mSpeed, UtInput::cSPEED);

         mFlightRoute.push_back(point);
      }
   }
   else if (command == "use_target_path")
   {
      mGridPointCount = 0;
      mUseTargetPath  = true;
      mFlightPath.clear();
      mFlightRoute.clear();
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
bool TargetConfig::ProcessSiteGridInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());

   if (command == "target_altitude")
   {
      aInput.ReadValue(command);
      if (command == "from")
      {
         aInput.ReadValueOfType(mGridAltitudeMin, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(mGridAltitudeMin, 0.0);
         aInput.ReadValue(command);
         aInput.StringEqual(command, "to");
         aInput.ReadValueOfType(mGridAltitudeMax, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(mGridAltitudeMax, mGridAltitudeMin);
         aInput.ReadValue(command);
         aInput.StringEqual(command, "by");
         aInput.ReadValue(mGridAltitudeStep);
         aInput.ReadValue(mGridAltitudeUnits);
         mGridAltitudeStep = aInput.ConvertValue(mGridAltitudeStep, mGridAltitudeUnits, UtInput::cLENGTH);
         aInput.ValueGreater(mGridAltitudeStep, 0.0);
      }
      else
      {
         aInput.ReadValue(mGridAltitudeMin);
         aInput.ReadValue(mGridAltitudeUnits);
         aInput.ConvertValue(mGridAltitudeMin, mGridAltitudeUnits, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(mGridAltitudeMin, 0.0);
         mGridAltitudeMax  = mGridAltitudeMin;
         mGridAltitudeStep = mGridAltitudeMin;
      }
   }
   else if (command == "target_speed")
   {
      aInput.ReadValue(mGridSpeed);
      aInput.ReadValue(mGridSpeedUnits);
      mGridSpeed = aInput.ConvertValue(mGridSpeed, mGridSpeedUnits, UtInput::cSPEED);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
void TargetConfig::GetRunPosition(int aTargetIndex, double& aX, double& aY, double& aZ) const
{
   int pointIndex    = aTargetIndex;
   int altitudeIndex = pointIndex / (mGridDownRangeCount * mGridCrossRangeCount);
   pointIndex -= altitudeIndex * (mGridDownRangeCount * mGridCrossRangeCount);
   int downRangeIndex  = pointIndex / mGridCrossRangeCount;
   int crossRangeIndex = pointIndex - (downRangeIndex * mGridCrossRangeCount);

   aX = mGridDownRangeMin + (mGridDownRangeStep * downRangeIndex);
   aY = mGridCrossRangeMin + (mGridCrossRangeStep * crossRangeIndex);
   aZ = mGridAltitudeMin + (mGridAltitudeStep * altitudeIndex);
}

} // namespace engage
