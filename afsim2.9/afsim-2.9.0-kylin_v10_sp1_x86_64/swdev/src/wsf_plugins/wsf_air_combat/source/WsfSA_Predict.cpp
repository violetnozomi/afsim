// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSA_Predict.hpp"

#include "UtDCM.hpp"
#include "UtRandom.hpp"
#include "UtScriptVec3.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfSimulation.hpp"

WsfSA_Predict::WsfSA_Predict(WsfSA_Processor* aSA_ProcPtr, WsfSA_Processor::PerceiveData* aDataPtr)
   : WsfSA_Module(aSA_ProcPtr, aDataPtr)
{
}

WsfSA_Predict::~WsfSA_Predict() {}

WsfSA_Predict::WsfSA_Predict(const WsfSA_Predict& aSrc)
   : WsfSA_Module(aSrc)
{
}

void WsfSA_Predict::Initialize(double aSimTime)
{
   // Generic position predictor
   {
      mGenericProjectionScriptPtr = GetSAP()->GetScriptContext().FindScript("ProjectPositionInTime");
      bool ok                     = GetSAP()->GetScriptContext().ValidateScript(mGenericProjectionScriptPtr,
                                                            "WsfGeoPoint",
                                                            "double, WsfSA_EntityPerception");
      if (!ok)
      {
         mGenericProjectionScriptPtr = nullptr;
      }
   }

   // Straightahead predictor
   {
      mGoForwardScriptPtr = GetSAP()->GetScriptContext().FindScript("ProjectPositionForward");
      bool ok =
         GetSAP()->GetScriptContext().ValidateScript(mGoForwardScriptPtr, "WsfGeoPoint", "double, WsfSA_EntityPerception");
      if (!ok)
      {
         mGoForwardScriptPtr = nullptr;
      }
   }

   // Level turn predictor (clockwise)
   {
      mLevelTurnLeftScriptPtr = GetSAP()->GetScriptContext().FindScript("ProjectPositionLevelTurnLeft");
      bool ok                 = GetSAP()->GetScriptContext().ValidateScript(mLevelTurnLeftScriptPtr,
                                                            "WsfGeoPoint",
                                                            "double, WsfSA_EntityPerception, double");
      if (!ok)
      {
         mLevelTurnLeftScriptPtr = nullptr;
      }
   }

   // Level turn predictor (counter-clockwise)
   {
      mLevelTurnRightScriptPtr = GetSAP()->GetScriptContext().FindScript("ProjectPositionLevelTurnRight");
      bool ok                  = GetSAP()->GetScriptContext().ValidateScript(mLevelTurnRightScriptPtr,
                                                            "WsfGeoPoint",
                                                            "double, WsfSA_EntityPerception, double");
      if (!ok)
      {
         mLevelTurnRightScriptPtr = nullptr;
      }
   }

   // Turn-to-heading predictor
   {
      mTurnToHeadingScriptPtr = GetSAP()->GetScriptContext().FindScript("ProjectPositionTurnToHeading");
      bool ok                 = GetSAP()->GetScriptContext().ValidateScript(mTurnToHeadingScriptPtr,
                                                            "WsfGeoPoint",
                                                            "double, WsfSA_EntityPerception, double, double");
      if (!ok)
      {
         mTurnToHeadingScriptPtr = nullptr;
      }
   }

   // Turn-to-point predictor
   {
      mGoToPointScriptPtr = GetSAP()->GetScriptContext().FindScript("ProjectPositionGoToPoint");
      bool ok             = GetSAP()->GetScriptContext().ValidateScript(mGoToPointScriptPtr,
                                                            "WsfGeoPoint",
                                                            "double, WsfSA_EntityPerception, WsfGeoPoint, double");
      if (!ok)
      {
         mGoToPointScriptPtr = nullptr;
      }
   }

   // Slice-to-run predictor
   {
      mRollAndPullScriptPtr = GetSAP()->GetScriptContext().FindScript("ProjectPositionSlice");
      bool ok               = GetSAP()->GetScriptContext().ValidateScript(mRollAndPullScriptPtr,
                                                            "WsfGeoPoint",
                                                            "double, WsfSA_EntityPerception, double, double");
      if (!ok)
      {
         mRollAndPullScriptPtr = nullptr;
      }
   }

   // Slice-to-heading predictor
   {
      mRollAndPullToHeadingScriptPtr = GetSAP()->GetScriptContext().FindScript("ProjectPositionSliceToHeading");
      bool ok                        = GetSAP()->GetScriptContext().ValidateScript(mRollAndPullToHeadingScriptPtr,
                                                            "WsfGeoPoint",
                                                            "double, WsfSA_EntityPerception, double, double, double");
      if (!ok)
      {
         mRollAndPullToHeadingScriptPtr = nullptr;
      }
   }

   // Split-S predictor
   {
      mSplitEssScriptPtr = GetSAP()->GetScriptContext().FindScript("ProjectPositionSplitS");
      bool ok            = GetSAP()->GetScriptContext().ValidateScript(mSplitEssScriptPtr,
                                                            "WsfGeoPoint",
                                                            "double, WsfSA_EntityPerception, double");
      if (!ok)
      {
         mSplitEssScriptPtr = nullptr;
      }
   }
}

bool WsfSA_Predict::GetPerceivedState(const WsfSA_EntityPerception& aEntity, UtVec3dX& aLocationWCS, UtVec3dX& aVelocityWCS)
{
   aLocationWCS.Null();
   aVelocityWCS.Null();

   // Both must be valid to return true from the function
   bool validLocation = false;
   bool validVelocity = false;

   if (!aEntity.GetAngleOnly())
   {
      UtEntity::ConvertLLAToWCS(aEntity.GetLat_deg(),
                                aEntity.GetLon_deg(),
                                aEntity.GetAltitude_ft() * UtMath::cM_PER_FT,
                                aLocationWCS.GetData());

      validLocation = true;
   }

   // Location needs to be valid to be able to convert bearing and speed to WCS vector
   if (validLocation)
   {
      if (aEntity.GetHeadingValid() && aEntity.GetSpeedValid())
      {
         double   elevation = 0.0;
         UtVec3dX velocityNED(aEntity.GetHeading_deg() * UtMath::cRAD_PER_DEG,
                              elevation,
                              aEntity.GetSpeed_kts() * UtMath::cMPS_PER_NMPH,
                              true);

         UtEntity entity;
         entity.SetLocationWCS(aLocationWCS.GetData());
         entity.ConvertNEDVectorToWCS(aVelocityWCS.GetData(), velocityNED.GetData());

         validVelocity |= true;
      }
   }

   return validLocation & validVelocity;
}

bool WsfSA_Predict::CheckRequestedTime(double aSimTime, const WsfSA_EntityPerception& aEntity)
{
   if (aSimTime < aEntity.Time())
   {
      if (mSA_ProcessorPtr->DebugEnabled())
      {
         auto log = ut::log::debug() << "Requested a prediction at a time earlier than the most recent perception.";
         log.AddNote() << "Entity: " << aEntity.GetPerceivedName();
         log.AddNote() << "Requested time: " << aSimTime << ", Latest: " << aEntity.Time();
      }
      return false;
   }
   return true;
}

const WsfGeoPoint WsfSA_Predict::ProjectPositionInTime(double aSimTime, const WsfSA_EntityPerception& aEntity)
{
   if (mGenericProjectionScriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aSimTime;
         scriptArgs << aEntity;

         GetScriptContext().ExecuteScript(aSimTime, mGenericProjectionScriptPtr, scriptRetVal, scriptArgs);
         auto valuePtr = static_cast<WsfGeoPoint*>(scriptRetVal.GetPointer()->GetAppObject());

         return *valuePtr;
      }
      catch (...)
      {
         auto logger = ut::log::error() << "SA Processor";
         logger.AddNote() << "Exception thrown in WsfSA_Predict::ProjectPositionInTime on platform: "
                          << GetPlatform()->GetName() << ".";
      }
   }

   WsfLocalTrack* perceivedTrack = aEntity.GetTrackPtr();
   if (perceivedTrack)
   {
      UtVec3dX newPosition;
      perceivedTrack->GetExtrapolatedLocationWCS(aSimTime, newPosition.GetData());
      return WsfGeoPoint(newPosition.GetData());
   }

   auto log =
      ut::log::error() << "Generic entity prediction requires a track, which was not found for the perceived entity.";
   log.AddNote() << "Entity: " << aEntity.GetPerceivedName();
   return WsfGeoPoint();
}

const WsfGeoPoint WsfSA_Predict::ProjectPositionForward(double aSimTime, const WsfSA_EntityPerception& aEntity)
{
   if (mGoForwardScriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aSimTime;
         scriptArgs << aEntity;

         GetScriptContext().ExecuteScript(aSimTime, mGoForwardScriptPtr, scriptRetVal, scriptArgs);
         auto valuePtr = static_cast<WsfGeoPoint*>(scriptRetVal.GetPointer()->GetAppObject());

         return *valuePtr;
      }
      catch (...)
      {
         auto logger = ut::log::error() << "SA Processor";
         logger.AddNote() << "Exception thrown in WsfSA_Predict::ProjectPositionForward on platform: "
                          << GetPlatform()->GetName() << ".";
      }
   }

   UtVec3dX perceivedLocationWCS;
   UtVec3dX perceivedVelocityWCS;
   if (GetPerceivedState(aEntity, perceivedLocationWCS, perceivedVelocityWCS))
   {
      if (!CheckRequestedTime(aSimTime, aEntity))
      {
         return WsfGeoPoint(perceivedLocationWCS.GetData());
      }

      UtEntity entityData;
      entityData.SetLocationWCS(perceivedLocationWCS.GetData());
      entityData.SetVelocityWCS(perceivedVelocityWCS.GetData());

      UtVec3dX velocityNED = entityData.GetVelocityNED().GetData();
      double   heading_rad = velocityNED.Azimuth();

      entityData.SetOrientationNED(heading_rad, 0.0, 0.0);

      // Straight and level flyout
      double distanceRemaining_m = (aSimTime - aEntity.Time()) * entityData.GetSpeed();

      double finalLat, finalLon;
      UtEllipsoidalEarth::ExtrapolateGreatEllipsePosition(aEntity.GetLat_deg(),
                                                          aEntity.GetLon_deg(),
                                                          aEntity.GetHeading_deg(),
                                                          distanceRemaining_m,
                                                          finalLat,
                                                          finalLon);

      return WsfGeoPoint(finalLat, finalLon, entityData.GetAltitude());
   }

   auto log =
      ut::log::error() << "Insufficient state data found for perceived entity. A valid prediction cannot be made.";
   log.AddNote() << "Entity: " << aEntity.GetPerceivedName();
   return WsfGeoPoint();
}

const WsfGeoPoint WsfSA_Predict::ProjectPositionLevelTurn(const double                  aSimTime,
                                                          const WsfSA_EntityPerception& aEntity,
                                                          const double                  aGLimit,
                                                          const TurnDirection           aDirection)
{
   UtScript* scriptPtr = (aDirection == TurnDirection::cRIGHT) ? mLevelTurnRightScriptPtr : mLevelTurnLeftScriptPtr;
   if (scriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aSimTime;
         scriptArgs << aEntity;
         scriptArgs << aGLimit;

         GetScriptContext().ExecuteScript(aSimTime, scriptPtr, scriptRetVal, scriptArgs);
         auto valuePtr = static_cast<WsfGeoPoint*>(scriptRetVal.GetPointer()->GetAppObject());

         return *valuePtr;
      }
      catch (...)
      {
         auto logger = ut::log::error() << "SA Processor";
         logger.AddNote() << "Exception thrown in WsfSA_Predict::ProjectPositionLevelTurn on platform: "
                          << GetPlatform()->GetName() << ".";
      }
   }

   UtVec3dX perceivedLocationWCS;
   UtVec3dX perceivedVelocityWCS;
   if (GetPerceivedState(aEntity, perceivedLocationWCS, perceivedVelocityWCS))
   {
      if (!CheckRequestedTime(aSimTime, aEntity))
      {
         return WsfGeoPoint(perceivedLocationWCS.GetData());
      }

      // 1. Solve for r in a = v^2 / r => r = v^2 / a
      // 2. Use r to solve for w in a = rw^2 => w = sqrt(a/r)
      // 3. Use w to solve for the angle where angle = w*t
      // 4. Use the angle to solve for the displacement
      // Rotate the entity around a center point (i, j) determined by extending a determined R
      // left or right of the entity (depending on turn direction)
      // Then calculate the displacement from the original point x1, y1 to x2, y2
      // using the alg found here https://math.stackexchange.com/questions/51034/recomputing-arc-center/51055#51055

      // Circular motion stuff
      double radialAcc = aGLimit * UtEarth::cACCEL_OF_GRAVITY;
      double R         = perceivedVelocityWCS.MagnitudeSquared() / radialAcc;

      double w = 0.0; // radians/t
      if (!UtMath::NearlyZero(R))
      {
         w = sqrt(radialAcc / R);
      }
      double angleAtTimeRad = w * (aSimTime - aEntity.Time());
      double angleAtTimeDeg = angleAtTimeRad * UtMath::cDEG_PER_RAD;

      // Populate some LLA and NED values
      double lat, lon, alt;
      UtEntity::ConvertWCSToLLA(perceivedLocationWCS.GetData(), lat, lon, alt);

      // Get the angle to the center point.  If turning right it will be 90 degrees
      // CW, if turning left it will be 90 degrees CCW
      double centerPointAngDeg       = (aDirection == TurnDirection::cRIGHT ? 90 : -90);
      double headingToCenterPointDeg = UtMath::NormalizeAngle0_360(aEntity.GetHeading_deg() + centerPointAngDeg);

      // This is the center point we pivot around
      double cpLat = 0.0;
      double cpLon = 0.0;
      UtSphericalEarth::ExtrapolateGreatCirclePosition(lat, lon, headingToCenterPointDeg, R, cpLat, cpLon);

      // Convert our LLA for the aircraft and center point to NED
      double   lned[3];
      double   cpned[3];
      UtEntity tmpEntity;
      tmpEntity.ConvertLLAToNED(lat, lon, alt, lned);
      tmpEntity.ConvertLLAToNED(cpLat, cpLon, alt, cpned);

      // Grab our x,y values for the rotating a point around a point algorithm
      double cX = cpned[0];
      double cY = cpned[1];
      double x1 = lned[0];
      double y1 = lned[1];

      // Get the angle into the turn in radians to use for the new point calculation
      double turnAngRad = (aDirection == TurnDirection::cRIGHT ? angleAtTimeDeg : -angleAtTimeDeg) * UtMath::cRAD_PER_DEG;
      double cosAng     = cos(turnAngRad);
      double sinAng     = sin(turnAngRad);

      // Calculate where the aircraft will end up
      double x2 = cosAng * (x1 - cX) - sinAng * (y1 - cY) + cX;
      double y2 = sinAng * (x1 - cX) + cosAng * (y1 - cY) + cY;

      // Create the new position and convert back and return the LLA
      double newNED[3];
      newNED[0] = x2;
      newNED[1] = y2;
      newNED[2] = lned[2];
      double newLat, newLon, newAlt;
      tmpEntity.ConvertNEDToLLA(newNED, newLat, newLon, newAlt);

      return WsfGeoPoint(newLat, newLon, newAlt);
   }

   auto log =
      ut::log::error() << "Insufficient state data found for perceived entity. A valid prediction cannot be made.";
   log.AddNote() << "Entity: " << aEntity.GetPerceivedName();
   return WsfGeoPoint();
}

const WsfGeoPoint WsfSA_Predict::ProjectPositionTurnToHeading(double                        aSimTime,
                                                              const WsfSA_EntityPerception& aEntity,
                                                              double                        aHeading_deg,
                                                              double                        aGLimit)
{
   if (mTurnToHeadingScriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aSimTime;
         scriptArgs << aEntity;
         scriptArgs << aHeading_deg;
         scriptArgs << aGLimit;

         GetScriptContext().ExecuteScript(aSimTime, mTurnToHeadingScriptPtr, scriptRetVal, scriptArgs);
         auto valuePtr = static_cast<WsfGeoPoint*>(scriptRetVal.GetPointer()->GetAppObject());

         return *valuePtr;
      }
      catch (...)
      {
         auto logger = ut::log::error() << "SA Processor";
         logger.AddNote() << "Exception thrown in WsfSA_Predict::ProjectPositionTurnToHeading on platform: "
                          << GetPlatform()->GetName() << ".";
      }
   }

   UtVec3dX perceivedLocationWCS;
   UtVec3dX perceivedVelocityWCS;
   if (GetPerceivedState(aEntity, perceivedLocationWCS, perceivedVelocityWCS))
   {
      if (!CheckRequestedTime(aSimTime, aEntity))
      {
         return WsfGeoPoint(perceivedLocationWCS.GetData());
      }

      UtEntity entityData;
      entityData.SetLocationWCS(perceivedLocationWCS.GetData());
      entityData.SetVelocityWCS(perceivedVelocityWCS.GetData());

      UtVec3dX velocityNED = entityData.GetVelocityNED().GetData();
      double   heading_rad = velocityNED.Azimuth();
      double   heading_deg = UtMath::cDEG_PER_RAD * heading_rad;

      entityData.SetOrientationNED(heading_rad, 0.0, 0.0);

      double        deltaHeading_deg = UtMath::NormalizeAngleMinus180_180(aHeading_deg - heading_deg);
      TurnDirection direction        = (deltaHeading_deg >= 0.0) ? TurnDirection::cRIGHT : TurnDirection::cLEFT;

      // 1. Solve for r in a = v^2 / r => r = v^2 / a
      // 2. Use r to solve for w in a = rw^2 => w = sqrt(a/r)
      // 3. Use w to solve for the angle where angle = w*t
      // 4. Use the angle to solve for the displacement

      double radialAccelerationLimit_mps2 = aGLimit * UtEarth::cACCEL_OF_GRAVITY;
      double R                            = entityData.GetSpeedSquared() / radialAccelerationLimit_mps2;

      double w = 0.0; // radians/t
      if (!UtMath::NearlyZero(R))
      {
         w = sqrt(radialAccelerationLimit_mps2 / R);
      }
      double angle = w * (aSimTime - aEntity.Time());

      if (direction == TurnDirection::cRIGHT)
      {
         angle = fabs(angle);
      }
      else
      {
         angle = -fabs(angle);
      }

      double angleLimited  = UtMath::Limit(angle, fabs(deltaHeading_deg) * UtMath::cRAD_PER_DEG);
      double timeRemaining = 0.0;
      if (!UtMath::NearlyZero(w))
      {
         timeRemaining = fabs(angle - angleLimited) / w;
      }

      // Lazy solution: create a UtEntity for the circle center
      UtVec3dX circleCenterECS, circleCenterWCS;

      if (direction == TurnDirection::cRIGHT)
      {
         circleCenterECS = UtVec3dX(0.0, R, 0.0);
      }
      else
      {
         circleCenterECS = UtVec3dX(0.0, -R, 0.0);
      }
      entityData.ConvertECSToWCS(circleCenterECS.GetData(), circleCenterWCS.GetData());

      UtEntity circleCenter;
      circleCenter.SetLocationWCS(circleCenterWCS.GetData());
      circleCenter.SetOrientationNED(heading_rad, 0.0, 0.0);

      // Identify the end of the arc
      double angleRotation_rad = angleLimited;
      angleRotation_rad += (direction == TurnDirection::cRIGHT) ? -UtMath::cPI_OVER_2 : UtMath::cPI_OVER_2;

      UtVec3dX arcEndpointECS = R * UtVec3dX(angleRotation_rad, 0.0, true);
      UtVec3dX arcEndpointWCS;
      circleCenter.ConvertECSToWCS(arcEndpointECS.GetData(), arcEndpointWCS.GetData());

      WsfGeoPoint arcEndpoint(arcEndpointWCS.GetData());

      // Straight and level flyout from arc endpoint to end of evaluation
      double distanceRemaining_m = timeRemaining * entityData.GetSpeed();

      double finalLat, finalLon;
      UtEllipsoidalEarth::ExtrapolateGreatEllipsePosition(arcEndpoint.GetLat(),
                                                          arcEndpoint.GetLon(),
                                                          aHeading_deg,
                                                          distanceRemaining_m,
                                                          finalLat,
                                                          finalLon);

      return WsfGeoPoint(finalLat, finalLon, arcEndpoint.GetAlt());
   }

   auto log =
      ut::log::error() << "Insufficient state data found for perceived entity. A valid prediction cannot be made.";
   log.AddNote() << "Entity: " << aEntity.GetPerceivedName();
   return WsfGeoPoint();
}

const WsfGeoPoint WsfSA_Predict::ProjectPositionGoToPoint(double                        aSimTime,
                                                          const WsfSA_EntityPerception& aEntity,
                                                          WsfGeoPoint                   aPointOfInterest,
                                                          double                        aGLimit)
{
   if (mGoToPointScriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aSimTime;
         scriptArgs << aEntity;
         scriptArgs.Push(&aPointOfInterest, "WsfGeoPoint");
         scriptArgs << aGLimit;

         GetScriptContext().ExecuteScript(aSimTime, mGoToPointScriptPtr, scriptRetVal, scriptArgs);
         auto valuePtr = static_cast<WsfGeoPoint*>(scriptRetVal.GetPointer()->GetAppObject());

         return *valuePtr;
      }
      catch (...)
      {
         auto logger = ut::log::error() << "SA Processor";
         logger.AddNote() << "Exception thrown in WsfSA_Predict::ProjectPositionGoToPoint on platform: "
                          << GetPlatform()->GetName() << ".";
      }
   }

   double startHeading_deg, endHeading_deg;

   UtEllipsoidalEarth::GetVincentyDistance(aEntity.GetLat_deg(),
                                           aEntity.GetLon_deg(),
                                           aPointOfInterest.GetLat(),
                                           aPointOfInterest.GetLon(),
                                           startHeading_deg,
                                           endHeading_deg);

   return ProjectPositionTurnToHeading(aSimTime, aEntity, startHeading_deg, aGLimit);
}

const WsfGeoPoint WsfSA_Predict::ProjectPositionRollAndPull(double                        aSimTime,
                                                            const WsfSA_EntityPerception& aEntity,
                                                            double                        aHeading_deg,
                                                            double                        aRollAngle_deg,
                                                            double                        aGLimit)
{
   if (mRollAndPullToHeadingScriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aSimTime;
         scriptArgs << aEntity;
         scriptArgs << aHeading_deg;
         scriptArgs << aRollAngle_deg;
         scriptArgs << aGLimit;

         GetScriptContext().ExecuteScript(aSimTime, mRollAndPullToHeadingScriptPtr, scriptRetVal, scriptArgs);
         auto valuePtr = static_cast<WsfGeoPoint*>(scriptRetVal.GetPointer()->GetAppObject());

         return *valuePtr;
      }
      catch (...)
      {
         auto logger = ut::log::error() << "SA Processor";
         logger.AddNote() << "Exception thrown in WsfSA_Predict::ProjectPositionRollAndPull on platform: "
                          << GetPlatform()->GetName() << ".";
      }
   }

   UtVec3dX perceivedLocationWCS;
   UtVec3dX perceivedVelocityWCS;
   if (GetPerceivedState(aEntity, perceivedLocationWCS, perceivedVelocityWCS))
   {
      if (!CheckRequestedTime(aSimTime, aEntity))
      {
         return WsfGeoPoint(perceivedLocationWCS.GetData());
      }

      UtEntity entityData;
      entityData.SetLocationWCS(perceivedLocationWCS.GetData());
      entityData.SetVelocityWCS(perceivedVelocityWCS.GetData());

      UtVec3dX velocityNED = entityData.GetVelocityNED().GetData();
      double   heading_rad = velocityNED.Azimuth();
      double   heading_deg = UtMath::cDEG_PER_RAD * heading_rad;

      entityData.SetOrientationNED(heading_rad, 0.0, 0.0);

      double        deltaHeading_deg = UtMath::NormalizeAngleMinus180_180(aHeading_deg - heading_deg);
      TurnDirection direction        = (deltaHeading_deg >= 0.0) ? TurnDirection::cRIGHT : TurnDirection::cLEFT;

      // 1. Solve for r in a = v^2 / r => r = v^2 / a
      // 2. Use r to solve for w in a = rw^2 => w = sqrt(a/r)
      // 3. Use w to solve for the angle where angle = w*t
      // 4. Use the angle to solve for the displacement

      double radialAccelerationLimit_mps2 = aGLimit * 9.80665;
      double R                            = entityData.GetSpeedSquared() / radialAccelerationLimit_mps2;

      double w = 0.0; // radians/t
      if (!UtMath::NearlyZero(R))
      {
         w = sqrt(radialAccelerationLimit_mps2 / R);
      }
      double angle = w * (aSimTime - aEntity.Time());

      if (direction == TurnDirection::cLEFT)
      {
         aRollAngle_deg = -fabs(aRollAngle_deg);
         angle          = -fabs(angle);
      }
      else
      {
         aRollAngle_deg = fabs(aRollAngle_deg);
         angle          = fabs(angle);
      }

      double angleLimited  = UtMath::Limit(angle, fabs(deltaHeading_deg) * UtMath::cRAD_PER_DEG);
      double timeRemaining = fabs(angle - angleLimited) / w;

      double rollAngle_rad = UtMath::cRAD_PER_DEG * aRollAngle_deg;

      UtVec3dX circleCenterECS, circleCenterWCS;
      circleCenterECS = R * UtVec3dX(0.0, sin(rollAngle_rad), -cos(rollAngle_rad));
      entityData.ConvertECSToWCS(circleCenterECS.GetData(), circleCenterWCS.GetData());

      // Lazy solution: create a UtEntity for the circle center
      UtEntity circleCenter;
      circleCenter.SetLocationWCS(circleCenterWCS.GetData());

      circleCenter.SetOrientationNED(heading_rad, 0.0, rollAngle_rad);

      // Identify the end of the arc
      double angleRotation_rad;
      if (direction == TurnDirection::cLEFT)
      {
         angleRotation_rad = -UtMath::cPI_OVER_2 - angleLimited;
      }
      else
      {
         angleRotation_rad = -UtMath::cPI_OVER_2 + angleLimited;
      }

      UtVec3dX arcEndpointECS = R * UtVec3dX(0.0, angleRotation_rad, true);
      UtVec3dX arcEndpointWCS;
      circleCenter.ConvertECSToWCS(arcEndpointECS.GetData(), arcEndpointWCS.GetData());

      WsfGeoPoint arcEndpoint(arcEndpointWCS.GetData());

      // Straight and level flyout from arc endpoint to end of evaluation
      double distanceRemaining_m = timeRemaining * entityData.GetSpeed();

      double finalLat, finalLon;
      UtEllipsoidalEarth::ExtrapolateGreatEllipsePosition(arcEndpoint.GetLat(),
                                                          arcEndpoint.GetLon(),
                                                          aHeading_deg,
                                                          distanceRemaining_m,
                                                          finalLat,
                                                          finalLon);

      return WsfGeoPoint(finalLat, finalLon, arcEndpoint.GetAlt());
   }

   auto log =
      ut::log::error() << "Insufficient state data found for perceived entity. A valid prediction cannot be made.";
   log.AddNote() << "Entity: " << aEntity.GetPerceivedName();
   return WsfGeoPoint();
}

const WsfGeoPoint WsfSA_Predict::ProjectPositionRollAndPull(double                        aSimTime,
                                                            const WsfSA_EntityPerception& aEntity,
                                                            double                        aRollAngle_deg,
                                                            double                        aGLimit)
{
   if (mRollAndPullScriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aSimTime;
         scriptArgs << aEntity;
         scriptArgs << aRollAngle_deg;
         scriptArgs << aGLimit;

         GetScriptContext().ExecuteScript(aSimTime, mRollAndPullScriptPtr, scriptRetVal, scriptArgs);
         auto valuePtr = static_cast<WsfGeoPoint*>(scriptRetVal.GetPointer()->GetAppObject());

         return *valuePtr;
      }
      catch (...)
      {
         auto logger = ut::log::error() << "SA Processor";
         logger.AddNote() << "Exception thrown in WsfSA_Predict::ProjectPositionRollAndPull on platform: "
                          << GetPlatform()->GetName() << ".";
      }
   }

   // Using 180 exactly results in a turn to a particular side every time (in testing, to the left), so choose a heading
   // delta slightly less
   double changeOfHeading_deg = UtMath::Sign(aRollAngle_deg) * 179.9;

   return ProjectPositionRollAndPull(aSimTime, aEntity, aEntity.GetHeading_deg() + changeOfHeading_deg, aRollAngle_deg, aGLimit);
}

const WsfGeoPoint WsfSA_Predict::ProjectPositionSplitS(double aSimTime, const WsfSA_EntityPerception& aEntity, double aGLimit)
{
   if (mSplitEssScriptPtr != nullptr)
   {
      try
      {
         UtScriptData       scriptRetVal;
         UtScriptDataPacker scriptArgs{GetScriptContext().GetTypes()};
         scriptArgs << aSimTime;
         scriptArgs << aEntity;
         scriptArgs << aGLimit;

         GetScriptContext().ExecuteScript(aSimTime, mSplitEssScriptPtr, scriptRetVal, scriptArgs);
         auto valuePtr = static_cast<WsfGeoPoint*>(scriptRetVal.GetPointer()->GetAppObject());

         return *valuePtr;
      }
      catch (...)
      {
         auto logger = ut::log::error() << "SA Processor";
         logger.AddNote() << "Exception thrown in WsfSA_Predict::ProjectPositionSplitS on platform: "
                          << GetPlatform()->GetName() << ".";
      }
   }

   return ProjectPositionRollAndPull(aSimTime, aEntity, aEntity.GetHeading_deg() + 180.0, 180.0, aGLimit);
}
