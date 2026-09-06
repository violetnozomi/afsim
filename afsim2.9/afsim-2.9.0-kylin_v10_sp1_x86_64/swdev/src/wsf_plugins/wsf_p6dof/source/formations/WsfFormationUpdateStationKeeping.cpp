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

#include "WsfFormationUpdateStationKeeping.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfFormUpKeepStationManeuver.hpp"
#include "WsfFormation.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"

//! Update the station keeping maneuvers on formation members.
//!
//! This will traverse the formation tree starting at @p aFormationPtr and update
//! the existence or not of a FormUpKeepStation maneuver on the member platform.
//! This is called primarily when there are operations that change the organization
//! of the formation that might cause some members to no longer have to keep station,
//! or to have to start keeping station.
//!
//! \param aFormationPtr - the formation to consider.
//! \param aSimTime      - the time at which this update is occurring.
//! \param aJumpDetached - indicates if the update should ignore the attached state of the formation tree.
void WsfFormationUpdateStationKeeping::Update(WsfFormation* aFormationPtr, double aSimTime, bool aJumpDetached)
{
   if (aFormationPtr->IsLeaf())
   {
      WsfFormation*   leadPtr  = aFormationPtr->GetKeepStationRoot()->GetFormationLeader();
      WsfP6DOF_Mover* moverPtr = aFormationPtr->GetMemberMover();
      if (moverPtr)
      {
         if (aFormationPtr == leadPtr)
         {
            // So we only want to do this if there is nothing going on currently.
            WsfManeuver* mvrPtr = moverPtr->GetCurrentManeuver();
            if (mvrPtr && mvrPtr->Type() == std::string("FORM-UP-KEEP-STATION"))
            {
               moverPtr->CancelManeuvers();
               moverPtr->SetAutopilotFlightPathAngle(0.0);
               moverPtr->SetAutopilotRollAngle(0.0);
               moverPtr->SetAutopilotAltitude(moverPtr->GetPlatform()->GetAltitude() * UtMath::cFT_PER_M);
            }
         }
         else
         {
            auto mvrPtr = ut::make_unique<WsfFormUpKeepStationManeuver>();
            mvrPtr->SetFormation(aFormationPtr->GetQualifiedName());
            moverPtr->CancelManeuvers();
            moverPtr->ExecuteManeuver(aSimTime, std::move(mvrPtr));
         }
      }
   }
   else
   {
      auto func = [aJumpDetached, aSimTime](WsfFormation* aSubPtr)
      {
         if (aJumpDetached || aSubPtr->IsAttached())
         {
            Update(aSubPtr, aSimTime, aJumpDetached);
         }
      };
      aFormationPtr->InvokeOnSubFormations(func);
   }
}
