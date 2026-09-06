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
#include "VaCameraMotionTethered.hpp"

#include <iomanip>
#include <string>

#include <osg/Matrix>
#include <osg/Vec3>

#include "UtEllipsoidalEarth.hpp"
#include "UtMath.hpp"
#include "VaCameraBase.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaPosition.hpp"
#include "VaViewer.hpp"

namespace
{
const double cMaxCameraDistance = 160000000.0;
}

namespace vespa
{
VaCameraMotionTethered::VaCameraMotionTethered(VaCameraBase* aCameraPtr, VaEntity* aEntityPtr)
   : VaCameraMotion(aCameraPtr)
   , mEntityPtr(aEntityPtr)
   , mLLA(0, 0, 0)
   , mRefLLA(0, 0, 0)
   , mAz(-30.0)
   , mEl(45.0)
   , mDist(250.0)
   , mYaw(0.0)
   , mPitch(90.0)
   , mMotionLocked(false)
   , mDirty(true)
   , mAllowPicking(false)
   , mLeftDown(false)
   , mMiddleDown(false)
   , mRightDown(false)
   , mPanPossible(false)
   , mPanActive(false)
   , mPanX(0)
   , mPanY(0)
   , mZoomPossible(false)
   , mZoomX(0)
   , mZoomY(0)
   , mUseRefHeading(true)
   , mCallbacks()
   , mGeoHeading(0.0)
   , mAllowUserControl(true)
{
   mArticulatedTranslation[0] = 0.0;
   mArticulatedTranslation[1] = 0.0;
   mArticulatedTranslation[2] = 0.0;
   mArticulatedRotation[0]    = 0.0;
   mArticulatedRotation[1]    = 0.0;
   mArticulatedRotation[2]    = 0.0;

   mViewMatrix.makeIdentity();

   mCallbacks.Add(VaObserver::EntityMoved.Connect(&VaCameraMotionTethered::EntityMovedCB, this, mEntityPtr->GetUniqueId()));
   mCallbacks.Add(
      VaObserver::EntityRemoved.Connect(&VaCameraMotionTethered::EntityRemovedCB, this, mEntityPtr->GetUniqueId()));
   mCallbacks.Add(
      VaObserver::EntityDeleted.Connect(&VaCameraMotionTethered::EntityRemovedCB, this, mEntityPtr->GetUniqueId()));

   EntityMovedCB(mEntityPtr, true);
}

VaCameraMotionTethered::~VaCameraMotionTethered()
{
   if (mEntityPtr != nullptr)
   {
      mEntityPtr->SetVisibilityContribution(true, VaEntity::CameraHidden, GetCamera()->GetViewer());
   }
   mCallbacks.Clear();
}

void VaCameraMotionTethered::Initialize(VaCameraMotionData* aDataPtr)
{
   VaCameraMotion::Initialize(aDataPtr);
   if (aDataPtr != nullptr)
   {
      if (SupportsMotionData(aDataPtr->mMotionType))
      {
         mLLA        = aDataPtr->mLLA;
         mAz         = aDataPtr->mAED[0];
         mEl         = aDataPtr->mAED[1];
         mDist       = aDataPtr->mAED[2];
         mYaw        = aDataPtr->mYPR[0];
         mPitch      = aDataPtr->mYPR[1];
         mViewMatrix = aDataPtr->mViewMatrix;
      }
      delete aDataPtr;
   }
   mDirty = true;
}

void VaCameraMotionTethered::Reset()
{
   mPanPossible   = false;
   mPanActive     = false;
   mPanX          = 0;
   mPanY          = 0;
   mZoomPossible  = false;
   mZoomX         = 0;
   mZoomY         = 0;
   mAz            = -30.0;
   mEl            = 45.0;
   mDist          = 250.0;
   mYaw           = 0.0;
   mPitch         = 90.0;
   mUseRefHeading = true;
   mDirty         = true;
}

// private
void VaCameraMotionTethered::EntityMovedCB(VaEntity* aEntityPtr, bool aMoveInTime)
{
   if (aEntityPtr->GetUniqueId() == mEntityPtr->GetUniqueId()) // if it is my entity
   {
      mDirty = true; // note that we will need to update ourselves

      if (mEntityPtr != nullptr)
      {
         mRefLLA = mEntityPtr->GetFocus();
         mEntityPtr->GetOrientation(mRefOrientation[0], mRefOrientation[1], mRefOrientation[2]);
      }

      GetCamera()->GetViewer()->RequestUpdate();
   }
}

void VaCameraMotionTethered::EntityRemovedCB(VaEntity* aEntityPtr)
{
   if ((aEntityPtr != nullptr) && (aEntityPtr == mEntityPtr))
   {
      mEntityPtr = nullptr;
      mCallbacks.Clear();

      mDirty = true; // note that we will need to update ourselves
      GetCamera()->GetViewer()->RequestUpdate();
   }
}

bool VaCameraMotionTethered::Update(double aTime)
{
   VaCameraMotion::Update(aTime);

   if (mDirty)
   {
      mDirty = false;

      BuildViewMatrix(mRefLLA, mRefOrientation);
   }
   return mDirty;
}

VaCameraMotionTetheredData* VaCameraMotionTethered::GetMotionData() const
{
   double aed[3] = {mAz, mEl, mDist};
   double ypr[3] = {mYaw, mPitch, 0.0};
   return new VaCameraMotionTetheredData(mLLA, mRefLLA, aed, ypr, mViewMatrix);
}

bool VaCameraMotionTethered::SupportsMotionData(MotionType aMotionType) const
{
   bool supports;

   switch (aMotionType)
   {
   case cGEOCENTRIC:
      supports = false;
      break;
   case cTETHERED:
      supports = true;
      break;
   case cLOOK_AT:
      supports = false;
      break;
   case cFLY_TO:
      supports = true;
      break;
   default:
      supports = false;
      break;
   }
   return supports;
}

VaCameraMotionTetheredData* VaCameraMotionTethered::GetMotionDataDefault()
{
   static const double aed[3] = {-30.0, 45.0, 250.0};
   static const double ypr[3] = {0.0, 90.0, 0.0};

   VaPosition lla(0, 0, 0);

   return new VaCameraMotionTetheredData(lla, lla, aed, ypr, osg::Matrix::identity());
}

void VaCameraMotionTethered::GetViewMatrixAndCameraPosition(osg::Matrix& aViewMatrix, double aXYZ[3]) const
{
   aViewMatrix = mViewMatrix;
   mLLA.GetSceneXYZ(aXYZ, GetCamera()->GetViewer()->GetUniqueId());
}

void VaCameraMotionTethered::SetUseRefHeading(bool aUseRefHeading)
{
   mUseRefHeading = aUseRefHeading;
   mDirty         = true;
}

void VaCameraMotionTethered::SetPosition(const VaPosition& aPosition)
{
   mLLA   = aPosition;
   mDirty = true;
}

VaPosition VaCameraMotionTethered::GetReferencePosition()
{
   return mRefLLA;
}

void VaCameraMotionTethered::SetReferencePosition(const VaPosition& aPosition)
{
   mRefLLA = aPosition;
}

void VaCameraMotionTethered::GetYawPitchRoll(double& aYaw, double& aPitch, double& aRoll)
{
   aYaw   = mYaw;
   aPitch = mPitch;
   aRoll  = 0.0;
}

void VaCameraMotionTethered::SetYawPitchRoll(double aYaw, double aPitch, double aRoll)
{
   mYaw   = aYaw;
   mPitch = aPitch;
   mDirty = true;
}

bool VaCameraMotionTethered::MouseScroll(int aDirection)
{
   bool handled = false;

   if (mAllowUserControl)
   {
      if (GetCamera()->GetViewer()->GetHeight() > 0)
      {
         if (HandleSoftLock(cZOOM))
         {
            handled = true;

            double deltaY = (aDirection) / double(GetCamera()->GetViewer()->GetHeight());

            double newDistance =
               mDist * exp(deltaY * 5); // this is a continuous form of the function described by the linear
            // approximation: mDistance * (1.0 + 5.0 * deltaY).  Meaning with an
            // unpredictable frame rate it will still behave smoothly.
            SetNewOrbitDistance(newDistance);
         }
      }
   }

   return handled;
}

bool VaCameraMotionTethered::MouseMotion(int aX, int aY, unsigned int aState)
{
   bool handled = false;

   if (mAllowUserControl)
   {
      const int state = aState & (~VaViewer::CapsNumLockStates);

      if (mLeftDown && mPanPossible)
      {
         handled       = true;
         mPanActive    = true;
         double deltaX = (aX - mPanX) / double(GetCamera()->GetViewer()->GetWidth());
         double deltaY = (aY - mPanY) / double(GetCamera()->GetViewer()->GetHeight());
         mPanX         = aX;
         mPanY         = aY;

         if ((aState & VaViewer::ShiftCtrlAltKeys) == 0)
         {
            if (HandleSoftLock(cPRIMARY_ROTATION))
            {
               // Azimuth change
               double deltaAz = deltaX * 360.0;
               double newAz   = WrapAngle0To360(mAz + deltaAz);

               // Elevation change
               double deltaEl = deltaY * 90.0;
               double newEl   = mEl - deltaEl;
               if (newEl < -90.0)
               {
                  newEl = -90.0;
               }
               else if (newEl > 90.0)
               {
                  newEl = 90.0;
               }

               if ((newAz != mAz) || (newEl != mEl))
               {
                  mAz    = newAz;
                  mEl    = newEl;
                  mDirty = true;
                  GetCamera()->GetViewer()->RequestUpdate();
               }
            }
         }
         else if (state == VaViewer::ShiftKey)
         {
            if (HandleSoftLock(cSECONDARY_ROTATION))
            {
               // Yaw change
               double deltaYaw = deltaX * 360.0;
               double newYaw   = WrapAngle0To360(mYaw + deltaYaw);

               // Pitch change
               double deltaPitch = deltaY * 90.0;
               double newPitch   = mPitch - deltaPitch;
               if (newPitch < 0.0)
               {
                  newPitch = 0.0;
               }
               else if (newPitch > 180.0)
               {
                  newPitch = 180.0;
               }

               if ((newYaw != mYaw) || (newPitch != mPitch))
               {
                  mYaw   = newYaw;
                  mPitch = newPitch;
                  mDirty = true;
                  GetCamera()->GetViewer()->RequestUpdate();
               }
            }
         }
      }

      if ((mMiddleDown) || ((mRightDown) && (state == VaViewer::CtrlKey)))
      {
         if (mZoomPossible && GetCamera()->GetViewer()->GetHeight() > 0)
         {
            if (HandleSoftLock(cZOOM))
            {
               handled = true;

               double deltaY = (aY - mZoomY) / double(GetCamera()->GetViewer()->GetHeight());
               mZoomX        = aX;
               mZoomY        = aY;

               if (state == 0)
               {
                  double newDistance =
                     mDist * exp(deltaY * 5); // this is a continuous form of the function described by the linear
                  // approximation: mDistance * (1.0 + 5.0 * deltaY).  Meaning with an
                  // unpredictable frame rate it will still behave smoothly.
                  SetNewOrbitDistance(newDistance);
               }
            }
         }
      }
      mDirty |= handled;
   }

   return handled;
}

bool VaCameraMotionTethered::PassiveMouseMotion(int aX, int aY, unsigned int aState)
{
   bool handled = false;
   return handled;
}

bool VaCameraMotionTethered::MouseButtonPress(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   bool handled = false;

   if (mAllowUserControl)
   {
      const int state = aState & (~VaViewer::CapsNumLockStates);

      switch (aButton)
      {
      case 1:
         handled      = true;
         mLeftDown    = true;
         mPanPossible = false;

         if ((state == 0) || (state == VaViewer::ShiftKey))
         {
            // Pan/Tilt/Rotate
            mPanPossible = true;
            mPanX        = aX;
            mPanY        = aY;
         }
         break;
      case 2:
         handled       = true;
         mMiddleDown   = true;
         mZoomPossible = false;

         if (state == 0)
         {
            mZoomPossible = true;
            mZoomX        = aX;
            mZoomY        = aY;
         }
         break;
      case 3:
         handled    = true;
         mRightDown = true;
         break;
      default:
         break;
      }

      mDirty |= handled;
   }
   return handled;
}

bool VaCameraMotionTethered::MouseButtonRelease(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   bool handled = false;

   if (mAllowUserControl)
   {
      if (aButton == 1)
      {
         handled      = true;
         mLeftDown    = false;
         mPanPossible = false;
         mPanActive   = false;
      }
      else if (aButton == 2)
      {
         handled       = true;
         mMiddleDown   = false;
         mZoomPossible = false;
      }
      else if (aButton == 3)
      {
         handled    = true;
         mRightDown = false;
      }
      mDirty |= handled;
   }

   return handled;
}

bool VaCameraMotionTethered::MouseButtonDoublePress(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   bool handled = false;

   if (mAllowUserControl)
   {
      const int state = aState & (~VaViewer::CapsNumLockStates);

      if (aButton == 1)
      {
         if (state == 0)
         {
            if (HandleSoftLock(cZOOM))
            {
               handled = true;

               double newDistance = std::max(mDist * exp(-1.0), 1.0);
               SetNewOrbitDistance(newDistance);
            }
         }
      }
      else if (aButton == 2)
      {
      }
      else if (aButton == 3)
      {
      }

      mDirty |= handled;
   }
   return handled;
}

bool VaCameraMotionTethered::GesturePinch(double aScale, double aAngle, int aCenterX, int aCenterY)
{
   bool handled = false;
   if (mAllowUserControl)
   {
      mDirty |= handled;
   }
   return handled;
}

void VaCameraMotionTethered::BuildViewMatrix(const VaPosition& aRefPosition, const double aRefOrientation[3])
{
   // Build the model-view matrix.
   if (mMotionLocked)
   {
      double xyz[3];
      aRefPosition.GetSceneXYZ(xyz, GetCamera()->GetViewer()->GetUniqueId());

      // there used to be a branch here for equirectangular tethering... just don't do that, it isn't a good idea
      mViewMatrix = osg::Matrixd(
         osg::Matrixd::translate(-xyz[0], -xyz[1], -xyz[2]) *
         osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(1, 0, 0)) *
         osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(0, 1, 0)) *

         osg::Matrixd::rotate(osg::DegreesToRadians(-aRefPosition.GetLon()), 0.0, 1.0, 0.0) *
         osg::Matrixd::rotate(osg::DegreesToRadians(aRefPosition.GetLat()), 1.0, 0.0, 0.0) *

         osg::Matrixd::rotate(aRefOrientation[0], 0.0, 0.0, 1.0) *  // heading
         osg::Matrixd::rotate(-aRefOrientation[1], 1.0, 0.0, 0.0) * // pitch
         osg::Matrixd::rotate(-aRefOrientation[2], 0.0, 1.0, 0.0) * // roll

         osg::Matrixd::translate(-mArticulatedTranslation[1], -mArticulatedTranslation[0], mArticulatedTranslation[2]) *
         osg::Matrix::rotate(mArticulatedRotation[0], 0.0, 0.0, 1.0) *
         osg::Matrix::rotate(-mArticulatedRotation[1], 1.0, 0.0, 0.0) *
         osg::Matrix::rotate(-mArticulatedRotation[2], 0.0, 1.0, 0.0) *

         osg::Matrixd::rotate(osg::DegreesToRadians(mAz), 0.0, 0.0, 1.0) *
         osg::Matrixd::rotate(osg::DegreesToRadians(mEl + -90.0), 1.0, 0.0, 0.0));

      mGeoHeading = mAz + osg::RadiansToDegrees(aRefOrientation[0]);
   }
   else
   {
      double xyz[3];
      aRefPosition.GetSceneXYZ(xyz, GetCamera()->GetViewer()->GetUniqueId());

      double refHeading = mUseRefHeading ? aRefOrientation[0] : 0.0;

      mViewMatrix = osg::Matrixd(osg::Matrixd::translate(-xyz[0], -xyz[1], -xyz[2]) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(1, 0, 0)) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(0, 1, 0)) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(-aRefPosition.GetLon()), 0.0, 1.0, 0.0) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(aRefPosition.GetLat()), 1.0, 0.0, 0.0) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(mAz) + refHeading, 0.0, 0.0, 1.0) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(mEl - 90.0), 1.0, 0.0, 0.0) *
                                 osg::Matrixd::translate(0.0, 0.0, -mDist) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(-mEl + 90.0), 1.0, 0.0, 0.0) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(mYaw), 0.0, 0.0, 1.0) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(mPitch - 90.0), 1.0, 0.0, 0.0) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(mEl - 90.0), 1.0, 0.0, 0.0));

      mGeoHeading = mAz + osg::RadiansToDegrees(refHeading) + mYaw;
   }

   osg::Vec3 camerapos = osg::Vec3(0, 0, 0) * osg::Matrix::inverse(mViewMatrix);
   double    xyz[3]    = {camerapos[0], camerapos[1], camerapos[2]};
   double    lla[3];
   UtEllipsoidalEarth::ConvertECEFToLLA(xyz, lla[0], lla[1], lla[2]);
   mLLA.SetLLA(lla[0], lla[1], lla[2]);
   VaObserver::ViewMatrixChanged(GetCamera()->GetViewer());
}

// virtual
double VaCameraMotionTethered::GetGeographicHeading()
{
   return -mGeoHeading;
}

void VaCameraMotionTethered::SetAz(double aAz)
{
   mAz    = aAz;
   mDirty = true;
}

void VaCameraMotionTethered::SetEl(double aEl)
{
   mEl    = aEl;
   mDirty = true;
}

void VaCameraMotionTethered::SetDist(double aDist)
{
   mDist  = std::min(cMaxCameraDistance, std::max(0.0, aDist));
   mDirty = true;
}

void VaCameraMotionTethered::GetAzElDist(double& aAz, double& aEl, double& aDist) const
{
   aAz   = mAz;
   aEl   = mEl;
   aDist = mDist;
}

void VaCameraMotionTethered::SetAzElDist(double aAz, double aEl, double aDist)
{
   mAz = aAz;
   mEl = aEl;
   SetDist(aDist);
   mDirty = true;
}

void VaCameraMotionTethered::RequestDist(double aDist)
{
   SetNewOrbitDistance(aDist);
   GetCamera()->GetViewer()->RequestUpdate();
}

void VaCameraMotionTethered::ForceRefresh()
{
   mDirty = true;
}

void VaCameraMotionTethered::SetArticulatedOffset(double aX, double aY, double aZ)
{
   mArticulatedTranslation[0] = aX;
   mArticulatedTranslation[1] = aY;
   mArticulatedTranslation[2] = aZ;
}

void VaCameraMotionTethered::SetArticulatedRotate(double aH, double aP, double aR)
{
   mArticulatedRotation[0] = aH;
   mArticulatedRotation[1] = aP;
   mArticulatedRotation[2] = aR;
}


bool VaCameraMotionTethered::RecommendNearFar(double& aNear, double& aFar)
{
   aNear = 3.0;
   aFar  = 70000000.0;
   return true;
}

void VaCameraMotionTethered::SetNewOrbitDistance(double aNewDistance)
{
   if (aNewDistance < 1.0)
   {
      // attaching into cockpit, disable visibility of entity and its attachments, as they should not be seen be seen
      // while in cockpit
      mEntityPtr->SetVisibilityContribution(false, VaEntity::CameraHidden, GetCamera()->GetViewer());
      aNewDistance  = 1.0;
      mAz           = 0;
      mEl           = 0;
      mMotionLocked = true;
   }
   else
   {
      // coming out of cockpit, re-enable camera visibility on tethered view
      mEntityPtr->SetVisibilityContribution(true, VaEntity::CameraHidden, GetCamera()->GetViewer());
      mMotionLocked = false;
   }

   if (aNewDistance != mDist)
   {
      SetDist(aNewDistance);

      GetCamera()->GetViewer()->RequestUpdate();
   }
}

} // end namespace vespa
