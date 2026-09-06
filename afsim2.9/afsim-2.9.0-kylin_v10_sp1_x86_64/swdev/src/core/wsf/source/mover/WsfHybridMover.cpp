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

#include "WsfHybridMover.hpp"

#include <string>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfFollower.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

WsfHybridMover::WsfHybridMover(WsfScenario& aScenario)
   : WsfMover(aScenario)
   , mCurrentMoverType(cINVALID)
   , mFollowerMoverPtr(nullptr)
   , mWaypointMoverPtr(nullptr)
   , mAutoSwitch(false)
   , mInitialized(false)
{
   mThreadSafe = false;
}

WsfHybridMover::WsfHybridMover(const WsfHybridMover& aSrc)
   : WsfMover(aSrc)
   , mCurrentMoverType(aSrc.mCurrentMoverType)
   , mFollowerMoverPtr(nullptr)
   , mWaypointMoverPtr(nullptr)
   , mAutoSwitch(aSrc.mAutoSwitch)
   , mInitialized(aSrc.mInitialized)
{
   if (aSrc.mFollowerMoverPtr != nullptr)
   {
      mFollowerMoverPtr = dynamic_cast<WsfFollower*>(aSrc.mFollowerMoverPtr->Clone());
   }

   if (aSrc.mWaypointMoverPtr != nullptr)
   {
      mWaypointMoverPtr = aSrc.mWaypointMoverPtr->Clone();
   }
}

// virtual
WsfHybridMover::~WsfHybridMover()
{
   delete mFollowerMoverPtr;
   delete mWaypointMoverPtr;
}

// virtual
WsfMover* WsfHybridMover::Clone() const
{
   return new WsfHybridMover(*this);
}

// virtual
WsfSpatialDomain WsfHybridMover::GetSpatialDomain() const
{
   if (mWaypointMoverPtr != nullptr)
   {
      return mWaypointMoverPtr->GetSpatialDomain();
   }
   return WSF_SPATIAL_DOMAIN_AIR;
}

// virtual
bool WsfHybridMover::Initialize(double aSimTime)
{
   if (!WsfMover::Initialize(aSimTime))
   {
      return false;
   }

   if (mFollowerMoverPtr == nullptr)
   {
      ut::log::error() << "Must specify a valid follower_mover inside the hybrid mover block.";
      return false;
   }
   mFollowerMoverPtr->SetPlatform(GetPlatform());

   if (mWaypointMoverPtr == nullptr)
   {
      ut::log::error() << "Must specify a valid waypoint_mover inside the hybrid mover block.";
      return false;
   }
   mWaypointMoverPtr->SetPlatform(GetPlatform());

   if (mCurrentMoverType == cINVALID)
   {
      ut::log::error() << "Must specify a current_mover type inside the hybrid mover block.";
      return false;
   }

   // The initialization order affects the mover, so initialize
   // the current mover type first.
   if (mCurrentMoverType == cFOLLOWER)
   {
      if (!mWaypointMoverPtr->Initialize(aSimTime))
      {
         return false;
      }

      if (!mFollowerMoverPtr->Initialize(aSimTime))
      {
         return false;
      }
   }
   else
   {
      if (!mFollowerMoverPtr->Initialize(aSimTime))
      {
         return false;
      }

      if (!mWaypointMoverPtr->Initialize(aSimTime))
      {
         return false;
      }
   }

   mInitialized = true;
   return true;
}

// virtual
bool WsfHybridMover::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command(aInput.GetCommand());
   if (command == "follower_mover")
   {
      delete mFollowerMoverPtr;
      mFollowerMoverPtr = nullptr;

      std::string followerMover;
      aInput.ReadValue(followerMover);
      WsfMover* clonedPtr = GetScenario().CloneTypeT<WsfMover>(followerMover);
      if (clonedPtr != nullptr)
      {
         mFollowerMoverPtr = dynamic_cast<WsfFollower*>(clonedPtr);
      }
      if (mFollowerMoverPtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "The specified mover " + followerMover + " is invalid");
      }
      mCurrentMoverType = cFOLLOWER;
   }
   else if (command == "waypoint_mover")
   {
      std::string waypointMover;
      aInput.ReadValue(waypointMover);
      mWaypointMoverPtr = GetScenario().CloneTypeT<WsfMover>(waypointMover);
      if (mWaypointMoverPtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "The specified mover " + waypointMover + " is invalid");
      }
      mCurrentMoverType = cWAYPOINT;
   }
   else if (command == "current_mover")
   {
      std::string currentMover;
      aInput.ReadValue(currentMover);
      if (currentMover == "follower_mover")
      {
         mCurrentMoverType = cFOLLOWER;
      }
      else if (currentMover == "waypoint_mover")
      {
         mCurrentMoverType = cWAYPOINT;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Valid choices are follower_mover and waypoint_mover");
      }
   }
   else if (command == "auto_switch")
   {
      mAutoSwitch = true;
   }
   else
   {
      if (mWaypointMoverPtr != nullptr)
      {
         myCommand = mWaypointMoverPtr->ProcessInput(aInput);
      }
      if ((mFollowerMoverPtr != nullptr) && (!myCommand))
      {
         myCommand = mFollowerMoverPtr->ProcessInput(aInput);
      }
      else if (!myCommand)
      {
         myCommand = WsfMover::ProcessInput(aInput);
      }
   }
   return myCommand;
}

// virtual
void WsfHybridMover::Update(double aSimTime)
{
   WsfMover* moverPtr = GetCurrent();
   if (moverPtr != nullptr)
   {
      // Update the current mover
      moverPtr->Update(aSimTime);

      // if the current mover is the follower
      // and the lead no longer exists
      // then switch back to the waypoint mover
      // and return to route at the closest point
      if (mAutoSwitch)
      {
         if ((mCurrentMoverType == cFOLLOWER) && !mFollowerMoverPtr->GetTargetIsValid())
         {
            SetType(cWAYPOINT);
            mWaypointMoverPtr->GoToClosestPoint(aSimTime);
         }
      }
   }
}

//! Sets the current mover type.
void WsfHybridMover::SetType(MoverType aMoverType)
{
   if (mInitialized)
   {
      double simTime = GetSimulation()->GetSimTime();
      switch (mCurrentMoverType)
      {
      case cFOLLOWER:
         mFollowerMoverPtr->Update(simTime);
         mWaypointMoverPtr->SetLastUpdateTime(simTime);
         break;
      case cWAYPOINT:
         mWaypointMoverPtr->Update(simTime);
         mFollowerMoverPtr->SetLastUpdateTime(simTime);
         break;
      case cINVALID:
         break;
      default:
         break;
      }
   }
   mCurrentMoverType = aMoverType;
}

//! Returns a pointer to either the follower or waypoint mover, or 0 if the type is not valid.
WsfMover* WsfHybridMover::Get(MoverType aMoverType) const
{
   WsfMover* moverPtr = nullptr;

   switch (aMoverType)
   {
   case cFOLLOWER:
      moverPtr = mFollowerMoverPtr;
      break;
   case cWAYPOINT:
      moverPtr = mWaypointMoverPtr;
      break;
   case cINVALID:
      break;
   default:
      break;
   }
   return moverPtr;
}

//! Returns a pointer to the current mover.
WsfMover* WsfHybridMover::GetCurrent() const
{
   return Get(mCurrentMoverType);
}

// Implement navigation method defined in base class.
// virtual
const WsfRoute* WsfHybridMover::GetRoute() const
{
   return mWaypointMoverPtr->GetRoute();
}

// Implement navigation method defined in base class.
// virtual
const WsfRoute* WsfHybridMover::GetDefaultRoute() const
{
   return mWaypointMoverPtr->GetDefaultRoute();
}

// Implement navigation method defined in base class.
// virtual
bool WsfHybridMover::GoToAltitude(double aSimTime, double aAltitude, double aClimbDiveRate, bool aKeepRoute)
{
   bool ok = false;
   if (mCurrentMoverType == cWAYPOINT)
   {
      ok = mWaypointMoverPtr->GoToAltitude(aSimTime, aAltitude, aClimbDiveRate, aKeepRoute);
   }
   return ok;
}

// Implement navigation method defined in base class.
// virtual
bool WsfHybridMover::GoToClosestPoint(double aSimTime)
{
   return mWaypointMoverPtr->GoToClosestPoint(aSimTime);
}

// Implement navigation method defined in base class.
// virtual
bool WsfHybridMover::GoToLabel(double aSimTime, WsfStringId aLabelId)
{
   return mWaypointMoverPtr->GoToLabel(aSimTime, aLabelId);
}

// Implement navigation method defined in base class.
// virtual
bool WsfHybridMover::GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude)
{
   bool ok = false;
   if (mCurrentMoverType == cWAYPOINT)
   {
      ok = mWaypointMoverPtr->GoToLocation(aSimTime, aLatitude, aLongitude, aAltitude);
   }
   return ok;
}

// Implement navigation method defined in base class.
// virtual
bool WsfHybridMover::GoToSpeed(double aSimTime, double aSpeed, double aLinearAccel, bool aKeepRoute)
{
   bool ok = false;
   if (mCurrentMoverType == cWAYPOINT)
   {
      ok = mWaypointMoverPtr->GoToSpeed(aSimTime, aSpeed, aLinearAccel, aKeepRoute);
   }
   return ok;
}

// Implement navigation method defined in base class.
// virtual
bool WsfHybridMover::TurnToHeading(double aSimTime, double aHeading, double aRadialAccel, WsfPath::TurnDirection aTurnDirection)
{
   bool ok = false;
   if (mCurrentMoverType == cWAYPOINT)
   {
      ok = mWaypointMoverPtr->TurnToHeading(aSimTime, aHeading, aRadialAccel, aTurnDirection);
   }
   return ok;
}

// Implement navigation method defined in base class.
// virtual
bool WsfHybridMover::TurnToRelativeHeading(double                 aSimTime,
                                           double                 aHeadingChange,
                                           double                 aRadialAccel,
                                           WsfPath::TurnDirection aTurnDirection)
{
   bool ok = false;
   if (mCurrentMoverType == cWAYPOINT)
   {
      ok = mWaypointMoverPtr->TurnToRelativeHeading(aSimTime, aHeadingChange, aRadialAccel, aTurnDirection);
   }
   return ok;
}

// Implement navigation method defined in base class.
// virtual
bool WsfHybridMover::UpdateRoute(double aSimTime, const WsfRoute& aRoute)
{
   return mWaypointMoverPtr->UpdateRoute(aSimTime, aRoute);
}
