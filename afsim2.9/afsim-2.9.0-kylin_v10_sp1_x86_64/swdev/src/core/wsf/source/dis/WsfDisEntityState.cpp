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

#include "dis/WsfDisEntityState.hpp"

#include <algorithm>
#include <map>

#include "Dis.hpp"
#include "DisIO.hpp"
#include "UtCast.hpp"
#include "UtEarth.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"
#include "WsfDisObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfUtil.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisMover.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisPlaybackDevice.hpp"
#include "dis/WsfDisUtil.hpp"
#include "script/WsfScriptContext.hpp"

// ============================================================================
WsfDisEntityState::WsfDisEntityState(WsfDisInterface* aInterfacePtr)
   : DisEntityState()
   , mInterfacePtr(aInterfacePtr)
{
}

// ============================================================================
WsfDisEntityState::WsfDisEntityState(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisEntityState(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// ============================================================================
// virtual
WsfDisEntityState::~WsfDisEntityState() {}

WsfDisEntityState* WsfDisEntityState::Clone() const
{
   return new WsfDisEntityState(*this);
}

// ============================================================================
// virtual
int WsfDisEntityState::Process()
{
   double simTime = WsfDisUtil::GetTimeFromPdu(mInterfacePtr, *this);

   // We ignore PDU's where the site is 32767 (cSHADOW_SITE == 0x7FFF).  These are PDU's for shadow entities.
   // (See WsfDisInterface)

   if (GetEntityId().GetSite() == WsfDisInterface::cSHADOW_SITE)
   {
      return 1;
   }

   // Ignore the PDU if
   if (!GetEntityId().IsDefined())
   {
      mInterfacePtr->ReceivedBadEntityState();
      return 1;
   }

   // Determine if the specified entity already exists.

   int             deletePdu      = 1;
   WsfDisPlatform* disPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, GetEntityId());
   if (disPlatformPtr == nullptr)
   {
      // We haven't seen this entity ID before.  Attempt to create a platform for it.
      // If it fails then we simply ignore the PDU.

      if (ContentsAreValid())
      {
         // Only add the platform if the entity state says its active.
         bool isActive = (((GetAppearance() >> 23) & 1) == 0);
         if (isActive)
         {
            // Make a copy of "this" since AddExternalPlatform will take ownership of the WsfDisEntityState
            //  and it may be deleted before control is returned to this function.
            //  (This occurs when the platform fails initialization)
            disPlatformPtr = mInterfacePtr->AddExternalPlatform(simTime, ut::clone(*this));
         }
      }
   }
   else
   {
      // Platform already exists locally.  As long is it is marked externally controlled then we go ahead and update it.

      WsfPlatform* platformPtr = disPlatformPtr->GetPlatform();
      if (platformPtr->IsExternallyControlled() || platformPtr->IsExternallyMoved() ||
          WsfDisPlaybackDevice::IsPlaybackEntity(GetEntityId()))
      {
         if (ContentsAreValid())
         {
            disPlatformPtr->SetEntityState(simTime, this);
            // PROBLEM:
            // If no local objects are present in the simulation and/or if a panel is not connected to force a frame
            // rate update, then ExtrapolateEntityState does not get called. There are cases where the heading/alt/speed
            // changes are being missed and the DIS objects are lagging behind.  Could use a callback and allow the
            // application needing the update to subscribe, but this may potentially cause multiple updates at simTime
            // for the same platform if there are multiple subscribers. For that reason, the platform is simply going to
            // be updated when an ES PDU is received.
            platformPtr->Update(simTime);
            deletePdu = 0; // don't delete the PDU
         }
      }
      else
      {
         // We're receiving a PDU for something we own!
         // This is possible with DIS 7 and the Transfer Ownership Process (Push Transfer) where we are sending a
         // local entity to a remote simulation to be controlled from the remote simulation.  On receipt of an entity
         // state PDU from the remote simulation for a local platform, we should remove the local platform, as it
         // indicates the Transfer Ownership Process was completed successfully.
      }
   }

   // Notify any subscribers that an Entity State PDU had been received.
   WsfObserver::DisEntityStateReceived (&WsfDisUtil::GetSimulation(mInterfacePtr))(mInterfacePtr, *this);

   return deletePdu;
}

// ============================================================================
//! Check to see if the contents of the PDU are valid.
//! This method checks to see if the contents of the PDU are 'valid' (reasonable).
//! @returns 'true' if the PDU looks reasonable or 'false' if it does not.
// private
bool WsfDisEntityState::ContentsAreValid()
{
   // Warn if the magnitude of the position vector (distance from the center of the Earth) is less than
   // the (Earth's semi-major axis - 20000.0m) (Allow down to at least the bottom of the Marianas trench).
   static const double minimumRadius = UtEarth::cB - 20000.0;

   // Warn if we get a velocity of greater than about 100000 mi/hr.
   static const double maximumVelocity = 45000.0;

   // Warn about ridiculous accelerations (> 10000 G)
   static const double maximumAcceleration = 10000.0 * UtEarth::cACCEL_OF_GRAVITY;

   bool contentsAreValid = true;
   bool showWarning      = false;

   DisFloat64 location[3];
   DisFloat32 velocity[3];
   DisFloat32 acceleration[3];

   GetLocation(location[0], location[1], location[2]);
   GetVelocity(velocity[0], velocity[1], velocity[2]);
   GetAcceleration(acceleration[0], acceleration[1], acceleration[2]);

   if (UtVec3d::MagnitudeSquared(location) < (minimumRadius * minimumRadius))
   {
      contentsAreValid = false;
      showWarning      = true;
   }
   else if (GetDeadreckoningAlgorithm() != 1) // Not a static entity
   {
      if (UtVec3f::MagnitudeSquared(velocity) > (maximumVelocity * maximumVelocity))
      {
         showWarning = true;
      }
      else if (UtVec3f::MagnitudeSquared(acceleration) > (maximumAcceleration * maximumAcceleration))
      {
         showWarning = true;
      }
   }

   if (showWarning)
   {
      unsigned                             count = 0;
      std::map<DisEntityId, int>::iterator bdci  = mInterfacePtr->mBadDataCount.find(GetEntityId());
      if (bdci == mInterfacePtr->mBadDataCount.end())
      {
         count                                       = 1U;
         mInterfacePtr->mBadDataCount[GetEntityId()] = 1;
      }
      else
      {
         (*bdci).second += 1;
         count = ut::safe_cast<unsigned int, int>((*bdci).second);
      }
      if (count <= mInterfacePtr->mMaxBadPDU_Count)
      {
         auto out = ut::log::warning() << "WsfDisEntityState: Suspicious data in Entity State PDU.";
         out.AddNote() << "Entity: " << GetEntityId();
         out.AddNote() << "Location WCS: " << location[0] << ' ' << location[1] << ' ' << location[2];
         out.AddNote() << "Location Magnitude: " << UtVec3d::Magnitude(location) << " m";
         double lat;
         double lon;
         double alt;
         UtEntity::ConvertWCSToLLA(location, lat, lon, alt);
         out.AddNote() << "Location LLA: " << lat << ' ' << lon << ' ' << alt;
         out.AddNote() << "Velocity WCS: " << velocity[0] << ' ' << velocity[1] << ' ' << velocity[2];
         out.AddNote() << "Velocity Magnitude: " << UtVec3f::Magnitude(velocity) << " m/s";
         out.AddNote() << "Acceleration WCS: " << acceleration[0] << ' ' << acceleration[1] << ' ' << acceleration[2];
         out.AddNote() << "Acceleration Magnitude: " << UtVec3f::Magnitude(acceleration) << " m/s^2";
         if (!contentsAreValid)
         {
            out.AddNote() << "PDU will be ignored.";
         }
         if (count == mInterfacePtr->mMaxBadPDU_Count)
         {
            out.AddNote() << "Future messages for this entity will be suppressed.";
         }
      }
      else
      {
         // We have exceeded the maximum number of allowed PDUs with bad data.
         contentsAreValid = false;
      }
   }

   return contentsAreValid;
}
