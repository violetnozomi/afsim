// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaCamera.hpp"

#include <iostream>
#include <math.h>

#include <osg/Matrix>
#include <osg/Vec3>

#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "Uto.hpp"
#include "VaCameraMotionGeocentric.hpp"
#include "VaCameraMotionLookAt.hpp"
#include "VaCameraMotionTethered.hpp"
#include "VaEntity.hpp"
#include "VaObserver.hpp"
#include "VaPosition.hpp"
#include "VaUtils.hpp"
#include "VaViewer.hpp"

namespace vespa
{

VaCamera::VaCamera(VaViewer* aViewerPtr)
   : VaCameraBase(aViewerPtr)
   , mEntityUID(0)
   , mEntityRemovedUID(0)
   , mEntityDeletedUID(0)
   , mLockRotations(false)
{
   dynamic_cast<VaCameraMotionGeocentric*>(GetCameraMotion())->FlatMapMode(mLockRotations);
}

// virtual
VaCamera::~VaCamera() {}

void VaCamera::EntityRemovedCB(VaEntity* aEntityPtr)
{
   if (aEntityPtr != nullptr && (aEntityPtr->GetUniqueId() == mEntityUID))
   {
      if (mCameraMotionPtr->GetMotionType() == VaCameraMotion::cTETHERED)
      {
         Untether();
      }
      else if (mCameraMotionPtr->GetMotionType() == VaCameraMotion::cLOOK_AT)
      {
         UnLookAt();
      }
   }
}

void VaCamera::EntityDeletedCB(VaEntity* aEntityPtr)
{
   EntityRemovedCB(aEntityPtr);
}

void VaCamera::Untether(bool aResumeLastUntetheredPosition)
{
   if (mCameraMotionPtr->GetMotionType() == VaCameraMotion::cTETHERED)
   {
      mEntityUID = 0;
      if (mEntityRemovedUID != 0)
      {
         mCallbacks.Disconnect(mEntityRemovedUID);
         mEntityRemovedUID = 0;
      }
      if (mEntityDeletedUID != 0)
      {
         mCallbacks.Disconnect(mEntityDeletedUID);
         mEntityDeletedUID = 0;
      }

      bool                hasCamPosition = false;
      VaPosition          camPosition;
      VaCameraMotionData* motionDataPtr = nullptr;
      double              az, el, dist;
      double              heading = 0.0;
      if (mCameraMotionPtr != nullptr)
      {
         camPosition = mCameraMotionPtr->GetPosition();
         heading     = GetGeographicHeading();
         GetAzElDist(az, el, dist);
         motionDataPtr  = mCameraMotionPtr->GetMotionData();
         hasCamPosition = true;
      }

      SetCameraMotion(new VaCameraMotionGeocentric(this, mLockRotations));

      // TODO - Total hack to maintain the camera's position and previous distance from the ground.
      // This should be handled completely through the motion data.
      if (hasCamPosition)
      {
         if (aResumeLastUntetheredPosition)
         {
            camPosition.SetAlt(1.0);
         }
         else
         {
            camPosition.SetAlt(100000.0); // distance from camera to target that was tethered before

            mCameraMotionPtr->SetYawPitchRoll(-heading, el, 0.0); // no roll
         }

         mCameraMotionPtr->SetReferencePosition(camPosition);
      }
      if (motionDataPtr != nullptr && aResumeLastUntetheredPosition)
      {
         mCameraMotionPtr->Initialize(motionDataPtr);
      }

      mCameraMotionPtr->ForceRefresh();

      mViewerPtr->RequestUpdate();
   }
}

//! Tethers the camera to the specified entity.]
//! @param aEntityPtr the entity to tether to.
//! Returns true if it is successful and the tether doesn't already exist.
bool VaCamera::Tether(VaEntity* aEntityPtr)
{
   if (aEntityPtr != nullptr)
   {
      if (mCameraMotionPtr->GetMotionType() == VaCameraMotion::cTETHERED)
      {
         if (dynamic_cast<VaCameraMotionTethered*>(mCameraMotionPtr)->GetEntity()->GetUniqueId() ==
             aEntityPtr->GetUniqueId())
         {
            return false;
         }
      }

      SetCameraMotion(new VaCameraMotionTethered(this, aEntityPtr));

      mCameraMotionPtr->ForceRefresh();

      mEntityUID        = aEntityPtr->GetUniqueId();
      mEntityRemovedUID = mCallbacks.Add(VaObserver::EntityDeleted.Connect(&VaCamera::EntityRemovedCB, this, mEntityUID));
      mEntityDeletedUID = mCallbacks.Add(VaObserver::EntityRemoved.Connect(&VaCamera::EntityDeletedCB, this, mEntityUID));

      mViewerPtr->RequestUpdate();

      return true;
   }
   return false;
}

void VaCamera::UnLookAt()
{
   if (mCameraMotionPtr->GetMotionType() == VaCameraMotion::cLOOK_AT)
   {
      mEntityUID = 0;
      if (mEntityRemovedUID != 0)
      {
         mCallbacks.Disconnect(mEntityRemovedUID);
         mEntityRemovedUID = 0;
      }
      if (mEntityDeletedUID != 0)
      {
         mCallbacks.Disconnect(mEntityDeletedUID);
         mEntityDeletedUID = 0;
      }

      bool                hasCamPosition = false;
      VaPosition          camPosition;
      VaCameraMotionData* motionDataPtr = nullptr;
      if (mCameraMotionPtr != nullptr)
      {
         camPosition    = mCameraMotionPtr->GetPosition();
         motionDataPtr  = mCameraMotionPtr->GetMotionData();
         hasCamPosition = true;
      }

      SetCameraMotion(new VaCameraMotionGeocentric(this, mLockRotations));

      // TODO - Total hack to maintain the camera's position and previous distance from the ground.
      // This should be handled completely through the motion data.
      if (hasCamPosition)
      {
         camPosition.SetAlt(1.0);
         mCameraMotionPtr->SetReferencePosition(camPosition);
      }
      if (motionDataPtr != nullptr)
      {
         mCameraMotionPtr->Initialize(motionDataPtr);
      }

      mCameraMotionPtr->ForceRefresh();

      mViewerPtr->RequestUpdate();
   }
}

bool VaCamera::LookAt(VaEntity* aReferenceEntityPtr, VaEntity* aLookAtEntityPtr, double aAz, double aEl, double aDist)
{
   if ((aReferenceEntityPtr != nullptr) && (aLookAtEntityPtr != nullptr))
   {
      if (mCameraMotionPtr->GetMotionType() == VaCameraMotion::cLOOK_AT)
      {
         VaCameraMotionLookAt* motionPtr = dynamic_cast<VaCameraMotionLookAt*>(mCameraMotionPtr);
         if ((motionPtr != nullptr) && (motionPtr->GetLookAtEntity() != nullptr) &&
             (motionPtr->GetLookAtEntity() == aLookAtEntityPtr) && (motionPtr->GetEntity() != nullptr) &&
             (motionPtr->GetEntity() == aReferenceEntityPtr))
         {
            return false;
         }
      }

      SetCameraMotion(new VaCameraMotionLookAt(this, aReferenceEntityPtr));

      mCameraMotionPtr->SetAzElDist(aAz, aEl, aDist);
      ((VaCameraMotionLookAt*)mCameraMotionPtr)->LookAt(aLookAtEntityPtr);
      mCameraMotionPtr->ForceRefresh();

      mEntityUID        = aReferenceEntityPtr->GetUniqueId();
      mEntityRemovedUID = mCallbacks.Add(VaObserver::EntityDeleted.Connect(&VaCamera::EntityRemovedCB, this, mEntityUID));
      mEntityDeletedUID = mCallbacks.Add(VaObserver::EntityRemoved.Connect(&VaCamera::EntityDeletedCB, this, mEntityUID));

      mViewerPtr->RequestUpdate();

      return true;
   }
   return false;
}

void VaCamera::FlatMapMode(bool aState)
{
   mLockRotations                   = aState;
   VaCameraMotionGeocentric* geocen = dynamic_cast<VaCameraMotionGeocentric*>(mCameraMotionPtr);
   if (nullptr != geocen)
   {
      geocen->FlatMapMode(mLockRotations);
   }
}


bool VaCamera::SetCustomCameraMotion(VaCameraMotion* aCameraMotion)
{
   Untether(false);
   UnLookAt();

   SetCameraMotion(aCameraMotion);

   return true;
}


} // end namespace vespa
