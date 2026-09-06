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
#ifndef UTROUTECALCULATOR_HPP
#define UTROUTECALCULATOR_HPP

#include "ut_export.h"

#include <vector>

#include "UtEntity.hpp"
#include "UtVec2.hpp"

//! UtRouteCalculator computes a path of an aircraft through the execution of specific movements.
//! Movements allowed include flying straight, turning to a heading, orbiting a point, flying to points, and flying to
//! point/heading targets. Movements given to UtRouteCalculator are decomposed into a simple action list. Distance is
//! also computed for each action.
class UT_EXPORT UtRouteCalculator
{
public:
   enum TurnDirection
   {
      cCW,
      cCCW
   };

   //! Represents the state of the entity at a certain point
   struct State
   {
      double mLat;
      double mLon;
      double mAlt;
      double mHeading;
      double mSpeed;
   };

   friend struct Action;

   //! The route calculator breaks actions down into straight lines and arcs
   //! This contains information to reconstruct the movement given to the route computer
   struct Action
   {
      enum ActionType
      {
         cINITIALIZE,
         cSTRAIGHT,
         cARC
      };

      Action(ActionType aType)
         : mActionType(aType)
      {
      }

      void       SetState(UtRouteCalculator& aRouteHelper);
      ActionType mActionType{cINITIALIZE};
      State      mFinalState{};
      double     mDistance{0.0};
      double     mAngle{0.0};
      double     mTurnRadius{0.0};
      bool       mIsCCW{false};
   };

   using ActionList = std::vector<Action>;

   UtRouteCalculator();

   UtRouteCalculator(UtEntity& aState);

   void SetState(UtEntity& aState);

   void SetLocation(double aLat, double aLon, double aAlt);

   void SetHeading(double aHeading);

   double GetHeading() { return mHeading; }

   void SetSpeed(double aSpeed);

   double GetSpeed() { return mSpeed; }

   void SetTurnRadius(double aRadius);

   void SetRadialAcceleration(double aRadialAcceleration);

   void AdvanceToPoint(double aLat, double aLon);

   void AdvanceToPointHeading(double aLat, double aLon, double aHeading);

   void AdvanceToOrbitPoint(double aLat, double aLon, double aRadius, int aDirection);

   void OrbitPoint(double aLat, double aLon, double aRadius, double aTurns, int aDirection);

   void OrbitRaceTrack(double aLat, double aLon, double aHeading, double aRadius, double aLength, int aDirection);

   void AdvanceForward(double aDistance);

   void Turn(double aTurnAngle, double aTurnRadius = 0);

   void TurnToHeading(double aHeading, int aDirection = -1, double aTurnRadius = 0);

   UtEntity& GetState() { return mEntity; }

   double GetDistance() { return mDistance; }

   const ActionList& GetActions() { return mActions; }

   void ResetActions();

private:
   void AdvanceForwardToPoint(double aLat, double aLon);

   //! Represents two options for a turn, clockwise or counterclockwise
   struct TurnCircles
   {
      UtVec2d mTarget;
      UtVec2d mTargetDirection;
      UtVec2d mCenter[2];
      double  mHeading;
   };

   double GetTurnLength(TurnCircles& aCircle, int aDirection, UtVec2d& aTangent, double aTurnRadius, bool aExitAtTangent = true);


   TurnCircles CreateTurnCircles(UtEntity& aReferencePoint, double aTurnRadius, double aHeading, double aLat, double aLon);


   void ExecuteTurn(double aTurnRadius, int aTurnDirection, UtVec2d aTangentPoint, UtVec2d aEndPoint);

   void SaveState(State& aState);

   void LoadState(const State& aState);

   void PopAction();

   double DirectionSign(int aDirection) const { return aDirection == cCW ? 1.0 : -1.0; }

   void UpdateHeading(double aHeading);

   void UpdateVelocity();


   bool       mComputeTurnRadius{false};
   double     mTurnRadius{100.0};
   double     mRadialAccel{0.0};
   double     mHeading{0.0};
   double     mSpeed{1.0};
   double     mDistance{0.0};
   UtEntity   mEntity;
   ActionList mActions;
};


#endif
