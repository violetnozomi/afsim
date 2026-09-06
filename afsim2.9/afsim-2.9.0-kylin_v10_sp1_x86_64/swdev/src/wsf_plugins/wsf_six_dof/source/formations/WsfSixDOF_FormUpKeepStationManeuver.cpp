// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_FormUpKeepStationManeuver.hpp"

#include "UtLog.hpp"
#include "WsfPlatform.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_FormationManager.hpp"
#include "WsfSixDOF_FormationSimulationExtension.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "WsfSixDOF_StationKeepingState.hpp"

wsf::six_dof::FormUpKeepStationManeuver::FormUpKeepStationManeuver()
   : RelativeManeuver{ut::make_unique<wsf::six_dof::FormUpState>(mData, *this)}
{
}

wsf::six_dof::FormUpKeepStationManeuver::FormUpKeepStationManeuver(const FormUpKeepStationManeuver& aOther)
   : RelativeManeuver(aOther)
   , mFormationName{aOther.mFormationName}
   , mSpeedRangeFactor{aOther.mSpeedRangeFactor}
{
   mStatePtr = ut::make_unique<wsf::six_dof::FormUpState>(mData, *this);
}

//! Update the relative maneuver's data.
bool wsf::six_dof::FormUpKeepStationManeuver::UpdateData()
{
   bool       retval{false};
   Formation* formationPtr = GetFormation();
   if (formationPtr)
   {
      // Get the offset kinematics.
      Formation* attachedRootPtr = formationPtr->GetKeepStationRoot();
      mData.mKinematics          = formationPtr->GetOffsetKinematicsFrom(attachedRootPtr);

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

wsf::six_dof::Formation* wsf::six_dof::FormUpKeepStationManeuver::GetFormation() const
{
   if (mFormationName.empty())
   {
      ut::log::error() << "Cannot execute FormUpKeepStationManeuver with no formation. No actions will be performed.";
      return nullptr;
   }
   return FormationSimulationExtension::Get(GetMover()->GetSimulation()).GetManager().GetFormation(mFormationName);
}

void wsf::six_dof::FormUpKeepStationManeuver::SetFormation(const std::string& aFormationName)
{
   mFormationName = aFormationName;
}
