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

#include "dis/WsfDisMover.hpp"

#include "DisEntityState.hpp"
#include "DisIO.hpp"
#include "UtLog.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisEntityState.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisPlaybackDevice.hpp"
#include "dis/WsfDisUtil.hpp"
#include "ext/WsfExtEntityDeadReckon.hpp"
#include "ext/WsfExtInterface.hpp"

// ============================================================================
WsfDisMover::WsfDisMover(WsfDisInterface* aInterfacePtr, WsfPlatform* aPlatformPtr, DisEntityState* aEntityStatePtr)
   : WsfMover(aInterfacePtr->GetScenario())
   , mInterfacePtr(aInterfacePtr)
{
   if (aEntityStatePtr != nullptr)
   {
      // Initialize the location of the platform. Technically this *SHOULD* be done in the Initialize
      // method, but the pointer to the entity state is not available at that point.

      DisFloat64 locX, locY, locZ;
      DisFloat32 velX, velY, velZ;
      DisFloat32 aclX, aclY, aclZ;
      DisFloat32 psi, theta, phi;

      aEntityStatePtr->GetLocation(locX, locY, locZ);
      aEntityStatePtr->GetVelocity(velX, velY, velZ);
      aEntityStatePtr->GetAcceleration(aclX, aclY, aclZ);
      aEntityStatePtr->GetOrientation(psi, theta, phi);

      double locWCS[3] = {locX, locY, locZ};
      double aclWCS[3] = {aclX, aclY, aclZ};
      double velWCS[3] = {velX, velY, velZ};
      aPlatformPtr->SetLocationWCS(locWCS);
      aPlatformPtr->SetVelocityWCS(velWCS);
      aPlatformPtr->SetAccelerationWCS(aclWCS);
      aPlatformPtr->SetOrientationWCS(psi, theta, phi);

      // Also initialize the spatial domain.

      mSpatialDomain = WSF_SPATIAL_DOMAIN_UNKNOWN;
      DisEnum8 kind  = aEntityStatePtr->GetEntityType().GetEntityKind();
      if ((kind == 1) || // Platform
          (kind == 3) || // Life form
          (kind == 4) || // Environmental
          (kind == 5) || // Cultural Feature
          (kind == 7) || // Radio
          (kind == 8) || // Expendable
          (kind == 9))   // Sensor/Emitter
      {
         switch (aEntityStatePtr->GetEntityType().GetDomain())
         {
         case 1:
            mSpatialDomain = WSF_SPATIAL_DOMAIN_LAND;
            break;
         case 2:
            mSpatialDomain = WSF_SPATIAL_DOMAIN_AIR;
            break;
         case 3:
            mSpatialDomain = WSF_SPATIAL_DOMAIN_SURFACE;
            break;
         case 4:
            mSpatialDomain = WSF_SPATIAL_DOMAIN_SUBSURFACE;
            break;
         case 5:
            mSpatialDomain = WSF_SPATIAL_DOMAIN_SPACE;
            break;
         default:
            mSpatialDomain = WSF_SPATIAL_DOMAIN_UNKNOWN;
            break;
         }
      }
      else if (kind == 2) // Payload
      {
         DisEnum8 domain = aEntityStatePtr->GetEntityType().GetDomain();
         if (domain != 12) // Not Directed energy
         {
            // We are making the assumption that this isn't a torpedo or land mine.
            // We could try to use altitude or speed, but either has its problems as the item
            // might be on a launch pad.
            mSpatialDomain = WSF_SPATIAL_DOMAIN_AIR;
         }
      }
   }
   // BAR DeadReckon
   // Attach to dead reckon updates to move the external platform along otherwise the only update came
   // on initializations and if weapons in air against the external platform, weapons miss external targets.
   if (mInterfacePtr->GetExtInterface() != nullptr)
   {
      WsfExtEntityDeadReckon* deadReckon = mInterfacePtr->GetExtInterface()->GetEntityDeadReckon();
      deadReckon->RequestService(true);
      mConditionalCallbacks.Add(deadReckon->DeadReckonChanged.Connect(&WsfDisMover::DeadReckonChanged, this));
   }
   // Since WsfDisMover is not a usable script type, its type name does not get set anywhere else. Set it here.
   SetType(UtStringIdLiteral("WSF_DIS_MOVER"));
}

// ============================================================================
WsfDisMover::~WsfDisMover() {}

// ============================================================================
// virtual
WsfMover* WsfDisMover::Clone() const
{
   return new WsfDisMover(*this);
}

// BAR DeadReckon
void WsfDisMover::DeadReckonChanged(double aSimTime, WsfExtEntityDeadReckon::PlatformDR& aDR)
{
   if (aDR.mPlatformPtr != nullptr)
   {
      if (aDR.mPlatformPtr->GetIndex() == GetPlatform()->GetIndex())
      {
         Update(aSimTime);
      }
   }
}

// ============================================================================
// When this method is called we update the kinematic state of the platform
// to by extrapolating from the received entity state data.  WsfDisEntityState
// has stored the data in the WsfDisPlatform object associated with this platform.
// virtual
void WsfDisMover::Update(double aSimTime)
{
   if (aSimTime >= mLastUpdateTime)
   {
      WsfDisPlatform* disPlatformPtr = mInterfacePtr->GetDisPlatform(GetPlatform()->GetIndex());
      if (disPlatformPtr != nullptr) // May not be true during platform startup/shutdown or transfer
      {
         // When transferring a platform from local to external control, the platform is locally controlled
         // until the Fire PDU is sent (which triggers the initial send of the entity state for the platform).
         // Extrapolation must be prevented while still locally controlled because:
         //
         // - The entity state will not have valid data until the first call to WsfDisPlatform::MoverUpdated.
         // - There isn't a received PDU from which to extrapolate.

         if ((GetPlatform()->IsExternallyControlled()) || (GetPlatform()->IsExternallyMoved()))
         {
            // If the simulation has not received an Entity State PDU for this platform in the required time,
            // remove it from the simulation.
            double ageOutTime = mInterfacePtr->GetHeartbeatMultiplier() * mInterfacePtr->GetHeartbeatTimer();
            if ((aSimTime - disPlatformPtr->GetEntityStateUpdateTime()) > ageOutTime)
            {
               if (!mInterfacePtr->IsTerseMode())
               {
                  auto out = ut::log::info() << "Entity is stale and will be removed.";
                  out.AddNote() << "T = " << aSimTime;
                  out.AddNote() << "Entity: " << disPlatformPtr->GetEntityId();
               }
               WsfDisUtil::GetSimulation(mInterfacePtr).DeletePlatform(aSimTime, disPlatformPtr->GetPlatform());
               return;
            }
            mInterfacePtr->ExtrapolatePlatformState(aSimTime, disPlatformPtr);
         }
         else if (WsfDisPlaybackDevice::IsPlaybackEntity(disPlatformPtr->GetEntityId()))
         {
            mInterfacePtr->ExtrapolatePlatformState(aSimTime, disPlatformPtr);
         }
      }
      mLastUpdateTime = aSimTime;
   }
}
