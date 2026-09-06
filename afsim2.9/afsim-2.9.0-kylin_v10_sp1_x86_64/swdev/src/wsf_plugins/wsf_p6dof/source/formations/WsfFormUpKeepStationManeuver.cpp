// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfFormUpKeepStationManeuver.hpp"

#include "StationKeepingState.hpp"
#include "UtLog.hpp"
#include "WsfFormation.hpp"
#include "WsfFormationManager.hpp"
#include "WsfFormationSimulationExtension.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"

WsfFormUpKeepStationManeuver::WsfFormUpKeepStationManeuver()
   : WsfRelativeManeuver{ut::make_unique<FormUpState>(mData, *this)}
{
}

WsfFormUpKeepStationManeuver::WsfFormUpKeepStationManeuver(const WsfFormUpKeepStationManeuver& aOther)
   : WsfRelativeManeuver(aOther)
   , mFormationName{aOther.mFormationName}
   , mSpeedRangeFactor{aOther.mSpeedRangeFactor}
{
   mStatePtr = ut::make_unique<FormUpState>(mData, *this);
}

//! Update the relative maneuver's data.
bool WsfFormUpKeepStationManeuver::UpdateData()
{
   bool          retval{false};
   WsfFormation* formationPtr = GetFormation();
   if (formationPtr)
   {
      // Get the offset kinematics.
      WsfFormation* attachedRootPtr = formationPtr->GetKeepStationRoot();
      mData.mKinematics             = formationPtr->GetOffsetKinematicsFrom(attachedRootPtr);

      // Set the speed limits for the maneuver.
      auto   leaderState = attachedRootPtr->GetFormationLeader()->GetMemberKinematicState();
      double leaderSpeed = leaderState.mVelWCS.Magnitude();
      SetSpeedMpsMax(leaderSpeed * (1.0 + mSpeedRangeFactor));
      SetSpeedMpsMin(leaderSpeed * (1.0 - mSpeedRangeFactor));

      // Determine the separation vector from the target location to the chaser.
      mData.mChaserMoverPtr    = formationPtr->GetMemberMover();
      mData.mChaserPlatformPtr = mData.mChaserMoverPtr->GetPlatform();
      mData.mChaserPlatformPtr->GetLocationWCS(mData.mChaserLocWCS.GetData());
      mData.mSeparationWCS.Subtract(mData.mChaserLocWCS, mData.mKinematics.mLocWCS);

      retval = true;
   }
   return retval;
}

WsfFormation* WsfFormUpKeepStationManeuver::GetFormation() const
{
   if (mFormationName.empty())
   {
      ut::log::error()
         << "Cannot execute WsfFormUpKeepStationManeuver with no formation. No actions will be performed.";
      return nullptr;
   }
   return WsfFormationSimulationExtension::Get(GetMover()->GetSimulation()).GetManager().GetFormation(mFormationName);
}

void WsfFormUpKeepStationManeuver::SetFormation(const std::string& aFormationName)
{
   mFormationName = aFormationName;
}
