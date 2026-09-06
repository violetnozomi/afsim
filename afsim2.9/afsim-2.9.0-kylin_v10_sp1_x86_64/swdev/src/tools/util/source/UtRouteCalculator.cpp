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
#include "UtRouteCalculator.hpp"

#include <algorithm>
#include <cassert>

#include "UtCircleTangents.hpp"
#include "UtMath.hpp"
#include "UtVec2.hpp"
#include "UtVec3.hpp"

UtRouteCalculator::UtRouteCalculator()
{
   mEntity.SetLocationLLA(0, 0, 0);
   double velNED[3] = {1.0, 0.0, 0.0};
   mEntity.SetVelocityNED(velNED);
   ResetActions();
}

//! Construct UtRouteCalculator beginning in a specific state.
UtRouteCalculator::UtRouteCalculator(UtEntity& aState)
{
   SetState(aState);
}

void UtRouteCalculator::SetState(UtEntity& aState)
{
   mSpeed  = aState.GetSpeed();
   mEntity = aState;
   double velNED[3];
   mEntity.GetVelocityNED(velNED);
   mHeading = atan2(velNED[1], velNED[0]);
   ResetActions();
}

//! Clears any actions added to the action list.
void UtRouteCalculator::ResetActions()
{
   mActions.clear();
   Action init(Action::cINITIALIZE);
   init.SetState(*this);
   init.mActionType = Action::cINITIALIZE;
   init.mDistance   = 0;
   mActions.push_back(init);
}

//! Sets the location of the entity
void UtRouteCalculator::SetLocation(double aLat, double aLon, double aAlt)
{
   mEntity.SetLocationLLA(aLat, aLon, aAlt);
   ResetActions();
}

//! Sets the heading of the entity
void UtRouteCalculator::SetHeading(double aHeading)
{
   UpdateHeading(aHeading);
   ResetActions();
}

//! Sets the speed of the entity
void UtRouteCalculator::SetSpeed(double aSpeed)
{
   mSpeed = aSpeed;
   UpdateVelocity();
   if (mComputeTurnRadius)
   {
      mTurnRadius = mSpeed * mSpeed / mRadialAccel;
   }
   ResetActions();
}

//! Sets the default turn radius used in calculating turns
void UtRouteCalculator::SetTurnRadius(double aRadius)
{
   mComputeTurnRadius = false;
   mTurnRadius        = aRadius;
}

//! Sets the default turn radius used in calculating turns.
void UtRouteCalculator::SetRadialAcceleration(double aRadialAcceleration)
{
   mComputeTurnRadius = true;
   mRadialAccel       = aRadialAcceleration;
   mTurnRadius        = mSpeed * mSpeed / mRadialAccel;
}

//! Advances the entity to a location obeying the turn rate constraint
void UtRouteCalculator::AdvanceToPoint(double aLat, double aLon)
{
   double currentLat, currentLon, alt;
   mEntity.GetLocationLLA(currentLat, currentLon, alt);
   TurnCircles circles = CreateTurnCircles(mEntity, mTurnRadius, mHeading, currentLat, currentLon);

   double toTargetNED[3];
   mEntity.ConvertLLAToNED(aLat, aLon, alt, toTargetNED);
   UtVec2d toTarget(toTargetNED[0], toTargetNED[1]);
   double  cross     = circles.mTargetDirection.CrossProduct(toTarget);
   int     direction = cross > 0 ? 0 : 1;

   UtVec2d tangent;
   if (!UtCircleTangents::CirclePointTangent(circles.mCenter[direction],
                                             mTurnRadius,
                                             toTarget,
                                             (UtCircleTangents::TurnDirection)direction,
                                             tangent))
   {
      direction = 1 - direction;
      bool ok   = UtCircleTangents::CirclePointTangent(circles.mCenter[direction],
                                                     mTurnRadius,
                                                     toTarget,
                                                     (UtCircleTangents::TurnDirection)direction,
                                                     tangent);
      if (!ok) // Avoid compiler warnings
      {
         assert(ok);
      }
   }
   ExecuteTurn(mTurnRadius, direction, tangent, toTarget);
   AdvanceForward((toTarget - tangent).Magnitude());
}

//! Advances the entity to a location, arriving with a specific heading.
void UtRouteCalculator::AdvanceToPointHeading(double aLat, double aLon, double aHeading)
{
   double currentLat, currentLon, altitude;
   mEntity.GetLocationLLA(currentLat, currentLon, altitude);

   TurnCircles circles1 = CreateTurnCircles(mEntity, mTurnRadius, mHeading, currentLat, currentLon);
   TurnCircles circles2 = CreateTurnCircles(mEntity, mTurnRadius, aHeading, aLat, aLon);

   // double c2CrossRange = UtVec2d::CrossProduct(circles2.mTargetDirection, circles2.mTarget - circles1.mTarget);
   // double c1CrossRange = UtVec2d::CrossProduct(circles1.mTargetDirection, circles1.mTarget - circles2.mTarget);

   int c2Directions[2] = {cCW, cCCW};
   int c1Directions[2] = {cCW, cCCW};

   double  bestTurnLength = 1.0E20;
   int     bestc1dir(-1);
   int     bestc2dir(-1);
   UtVec2d bestT1;
   UtVec2d bestT2;
   for (int c1dir : c1Directions)
   {
      for (int c2dir : c2Directions)
      {
         UtVec2d t1, t2;
         bool    canTurn = UtCircleTangents::CircleCircleTangent(circles1.mCenter[c1dir],
                                                              mTurnRadius,
                                                              c1dir,
                                                              circles2.mCenter[c2dir],
                                                              mTurnRadius,
                                                              c2dir,
                                                              t1,
                                                              t2);
         if (canTurn)
         {
            double length = GetTurnLength(circles2, c2dir, t2, mTurnRadius, false) +
                            GetTurnLength(circles1, c1dir, t1, mTurnRadius) + (t2 - t1).Magnitude();
            if (length < bestTurnLength)
            {
               bestTurnLength = length;
               bestc1dir      = c1dir;
               bestc2dir      = c2dir;
               bestT1         = t1;
               bestT2         = t2;
            }
         }
      }
   }
   ExecuteTurn(mTurnRadius, bestc1dir, bestT1, bestT2);
   AdvanceForward((bestT1 - bestT2).Magnitude());
   TurnToHeading(aHeading, bestc2dir, mTurnRadius);
}

// Computes the turn length in meters
// @param aExitAtTangent 'true' if the turn is from the circle 'target' to the tangent
//                       'false' if the turn is from the tangent to the circle 'target'
double UtRouteCalculator::GetTurnLength(TurnCircles& aCircle, int aDirection, UtVec2d& aTangent, double aTurnRadius, bool aExitAtTangent)
{
   UtVec2d toTarget     = aCircle.mTarget - aCircle.mCenter[aDirection];
   UtVec2d toTangent    = aTangent - aCircle.mCenter[aDirection];
   double  angleTarget  = atan2(toTarget[1], toTarget[0]);
   double  angleTangent = atan2(toTangent[1], toTangent[0]);
   double  angleChange  = angleTangent - angleTarget;
   angleChange *= DirectionSign(aDirection) * (aExitAtTangent ? 1.0 : -1.0);
   if (angleChange < 0)
   {
      angleChange += UtMath::cTWO_PI;
   }
   return angleChange * aTurnRadius * 2;
}

//! Turns the entity to a heading given direction and turn radius constraints
void UtRouteCalculator::TurnToHeading(double aHeading, int aDirection, double aTurnRadius)
{
   double headingChange = UtMath::NormalizeAngleMinusPi_Pi(aHeading - mHeading);

   // Avoid 360 turns caused by floating point error
   if (headingChange < 1.0E-5 && headingChange > -1.0E-5)
   {
      aDirection = -1;
   }

   double turnAngle = headingChange;
   // double direction = headingChange < 0 ? -1.0 : 1.0;
   if (aDirection != -1)
   {
      double direction = (aDirection == cCW) ? 1.0 : -1.0;
      if (direction == -1.0 && headingChange > 0)
      {
         turnAngle = -UtMath::cTWO_PI + headingChange;
      }
      if (direction == 1.0 && headingChange < 0)
      {
         turnAngle = UtMath::cTWO_PI + headingChange;
      }
   }

   Turn(turnAngle, aTurnRadius);
}

//! Advances the entity to arrive tangent to circle
void UtRouteCalculator::AdvanceToOrbitPoint(double aLat, double aLon, double aRadius, int aDirection)
{
   double currentLat, currentLon, altitude;
   mEntity.GetLocationLLA(currentLat, currentLon, altitude);
   double orbitCenterNED[3];
   mEntity.ConvertLLAToNED(aLat, aLon, altitude, orbitCenterNED);
   UtVec2d toCenter(orbitCenterNED[0], orbitCenterNED[1]);

   TurnCircles circles1 = CreateTurnCircles(mEntity, mTurnRadius, mHeading, currentLat, currentLon);

   int c1Directions[2] = {cCW, cCCW};

   // bool complete(false);
   double  bestTurnLength = 1.0E20;
   int     bestc1dir(-1);
   UtVec2d bestT1;
   UtVec2d bestT2;
   for (int c1dir : c1Directions)
   {
      UtVec2d t1, t2;
      bool    canTurn =
         UtCircleTangents::CircleCircleTangent(circles1.mCenter[c1dir], mTurnRadius, c1dir, toCenter, aRadius, aDirection, t1, t2);
      if (canTurn)
      {
         double length = +GetTurnLength(circles1, c1dir, t1, mTurnRadius) + (t2 - t1).Magnitude();
         if (length < bestTurnLength)
         {
            bestTurnLength = length;
            bestc1dir      = c1dir;
            bestT1         = t1;
            bestT2         = t2;
         }
      }
   }
   if (bestTurnLength < 1.0E20)
   {
      ExecuteTurn(mTurnRadius, bestc1dir, bestT1, bestT2);
      AdvanceForward((bestT2 - bestT1).Magnitude());

      //  AdvanceForward((bestT2-bestT1).Magnitude());
      // AdvanceForwardToPoint();
   }
   else
   {
      double forwardDistance = std::max(mTurnRadius, aRadius - toCenter.Magnitude());
      AdvanceForward(forwardDistance);
      AdvanceToOrbitPoint(aLat, aLon, aRadius, aDirection);
   }
}

void UtRouteCalculator::ExecuteTurn(double aTurnRadius, int aTurnDirection, UtVec2d aTangentPoint, UtVec2d aEndPoint)
{
   UtVec2d tangentDirection(aEndPoint[0] - aTangentPoint[0], aEndPoint[1] - aTangentPoint[1]);
   double  newHeading = atan2(tangentDirection[1], tangentDirection[0]);
   TurnToHeading(newHeading, aTurnDirection, aTurnRadius);
}

//! Advances the entity to orbit a location
//! @param aTurns The number of times to orbit the point
void UtRouteCalculator::OrbitPoint(double aLat, double aLon, double aRadius, double aTurns, int aDirection)
{
   AdvanceToOrbitPoint(aLat, aLon, aRadius, aDirection);
   Turn(aTurns * UtMath::cTWO_PI * DirectionSign(aDirection), aRadius);
}

//! Advance the entity around a 'race track' pattern consisting of two semicircle turns
void UtRouteCalculator::OrbitRaceTrack(double aLat, double aLon, double aHeading, double aRadius, double aLength, int aDirection)
{
   AdvanceToPointHeading(aLat, aLon, aHeading);
   Turn(DirectionSign(aDirection) * UtMath::cPI);
   double straightLength = aLength - aRadius * 2;
   AdvanceForward(straightLength);
   Turn(DirectionSign(aDirection) * UtMath::cPI);
   AdvanceForward(straightLength);
}

void UtRouteCalculator::UpdateHeading(double aHeading)
{
   mHeading = aHeading;
   UpdateVelocity();
   mEntity.SetOrientationNED(mHeading, 0.0, 0.0);
}

void UtRouteCalculator::AdvanceForward(double aDistance)
{
   Action act(Action::cSTRAIGHT);
   mDistance += aDistance;
   UtVec3d velNED;
   mEntity.GetVelocityNED(velNED.GetData());
   velNED *= aDistance / mSpeed;
   mEntity.IncrementLocationNED(velNED.GetData());
   act.SetState(*this);
   act.mDistance = aDistance;
   mActions.push_back(act);
}

//! Advances the entity around a turn.
void UtRouteCalculator::Turn(double aTurnAngle, double aTurnRadius)
{
   double turnRadius = aTurnRadius != 0.0 ? aTurnRadius : mTurnRadius;

   double turnMagnitude = aTurnAngle < 0 ? -aTurnAngle : aTurnAngle;
   Action act(Action::cARC);

   double finalHeading        = mHeading + aTurnAngle;
   mDistance += act.mDistance = turnMagnitude * turnRadius;
   double  turnDirection      = aTurnAngle > 0 ? 1.0 : -1.0;
   double  angleToMe          = mHeading - turnDirection * UtMath::cPI_OVER_2;
   double  angleToFinal       = finalHeading - turnDirection * UtMath::cPI_OVER_2;
   UtVec2d currentPos(cos(angleToMe) * turnRadius, sin(angleToMe) * turnRadius);
   UtVec2d finalPos(cos(angleToFinal) * turnRadius, sin(angleToFinal) * turnRadius);
   UtVec2d vToFinal(finalPos - currentPos);

   double nedFinal[3] = {vToFinal[0], vToFinal[1], 0};
   mEntity.IncrementLocationNED(nedFinal);
   UpdateHeading(finalHeading);
   act.SetState(*this);
   act.mTurnRadius = turnRadius;
   act.mAngle      = aTurnAngle;
   act.mIsCCW      = aTurnAngle < 0;
   mActions.push_back(act);
}

void UtRouteCalculator::UpdateVelocity()
{
   double velNED[3] = {cos(mHeading) * mSpeed, sin(mHeading) * mSpeed, 0.0};
   mEntity.SetVelocityNED(velNED);
}

void UtRouteCalculator::Action::SetState(UtRouteCalculator& aRouteHelper)
{
   aRouteHelper.GetState().GetLocationLLA(mFinalState.mLat, mFinalState.mLon, mFinalState.mAlt);
   mFinalState.mHeading = aRouteHelper.GetHeading();
   mFinalState.mSpeed   = aRouteHelper.GetSpeed();
   mTurnRadius          = aRouteHelper.mTurnRadius;
}

void UtRouteCalculator::SaveState(State& aState)
{
   mEntity.GetLocationLLA(aState.mLat, aState.mLon, aState.mAlt);
   aState.mHeading = mHeading;
   aState.mSpeed   = mSpeed;
}

void UtRouteCalculator::LoadState(const State& aState)
{
   mEntity.SetLocationLLA(aState.mLat, aState.mLon, aState.mAlt);
   UpdateHeading(aState.mHeading);
   mSpeed = aState.mSpeed;
}

void UtRouteCalculator::PopAction()
{
   if (!mActions.empty())
   {
      mDistance -= mActions.back().mDistance;
   }
   mActions.pop_back();
}

UtRouteCalculator::TurnCircles UtRouteCalculator::CreateTurnCircles(UtEntity& aReferencePoint,
                                                                    double    aTurnRadius,
                                                                    double    aHeading,
                                                                    double    aLat,
                                                                    double    aLon)
{
   UtEntity tmp;
   tmp.SetLocationLLA(aLat, aLon, 0);
   tmp.SetOrientationNED(aHeading, 0, 0);

   double rightECS[3] = {0, aTurnRadius, 0};
   double rightWCS[3];
   double rightNED[3];
   tmp.ConvertECSToWCS(rightECS, rightWCS);
   aReferencePoint.ConvertWCSToNED(rightWCS, rightNED);

   double leftECS[3] = {0, -aTurnRadius, 0};
   double leftWCS[3];
   double leftNED[3];
   tmp.ConvertECSToWCS(leftECS, leftWCS);
   aReferencePoint.ConvertWCSToNED(leftWCS, leftNED);

   TurnCircles c;
   c.mHeading = aHeading;
   aReferencePoint.ConvertWCSToNED(rightWCS, rightNED);
   c.mCenter[0].Set(rightNED[0], rightNED[1]);
   c.mCenter[1].Set(leftNED[0], leftNED[1]);
   double targetNED[3];
   aReferencePoint.ConvertLLAToNED(aLat, aLon, 0, targetNED);
   c.mTarget[0] = targetNED[0];
   c.mTarget[1] = targetNED[1];
   c.mTargetDirection.Set(cos(aHeading), sin(aHeading));
   return c;
}
