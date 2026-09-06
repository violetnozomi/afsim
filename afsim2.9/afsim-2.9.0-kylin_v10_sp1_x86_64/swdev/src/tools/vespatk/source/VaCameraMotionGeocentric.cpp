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
#include "VaCameraMotionGeocentric.hpp"

#include <iomanip>
#include <iostream>
#include <math.h>
#include <string>

#include <osg/Matrix>
#include <osg/Vec3>

#include "UtEllipsoidalEarth.hpp"
#include "UtLLAPos.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtMoon.hpp"
#include "UtSphericalEarth.hpp"
#include "VaCameraBase.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaPosition.hpp"
#include "VaSelectionBox.hpp"
#include "VaTimeManager.hpp"
#include "VaViewer.hpp"

namespace
{
const double cApproxEarthRadius              = 6378137.0;
const double cApproxMetersPerDegreeAtEquator = 111319.4908;
const double cDefaultCameraDistance          = 12585018.0;
const double cMaxCameraDistanceFlatEarth     = 40000000.0;
const double cMaxCameraDistanceRoundEarth    = UtMoon::cA * 3.0;
} // namespace

namespace vespa
{
std::map<int, double> VaCameraMotionGeocentric::sDistCacheMap;

VaCameraMotionGeocentric::VaCameraMotionGeocentric(VaCameraBase* aCameraPtr, bool aFlatMapMode)
   : VaCameraMotion(aCameraPtr)
   , mDist(cDefaultCameraDistance)
   , mLeftDown(false)
   , mMiddleDown(false)
   , mRightDown(false)
   , mPanPossible(false)
   , mPanActive(false)
   , mPanX(0)
   , mPanY(0)
   , mAz(0.0)
   , mEl(0.0)
   , mYaw(0.0)
   , mPitch(90.0)
   , mECI_Offset(0)
   , mDirty(true)
   , mTime(0.0)
   , mTimeMouseEventStarted(0.0)
   , mTimeMouseEventDelay(0.1)
   , mCallbacks()
   , mEnableTouchScreenDelay(false)
   , mDebug(false)
   , mFlatMapMode(aFlatMapMode)
   , mPanDragMultiplier(1.0f)
   , mAnimationStruct(nullptr)
{
   mLLA.SetLLA(0.0, 0.0, 0.0);
   mRefLLA.SetLLA(0.0, 0.0, 0.0);

   mViewMatrix.makeIdentity();

   mCallbacks.Add(VaObserver::HandleSelectionBoxResult.Connect(&VaCameraMotionGeocentric::HandleSelectionBoxResultCB, this));
}

void VaCameraMotionGeocentric::Initialize(VaCameraMotionData* aDataPtr)
{
   VaCameraMotion::Initialize(aDataPtr);

   if (aDataPtr != nullptr)
   {
      double distCache = sDistCacheMap[0];
      if (mDist <= 0.0)
      {
         distCache = mDist;
      }
      VaPosition temp = mRefLLA;
      temp.SetAlt(distCache);
      SetPosition(temp);
      if (SupportsMotionData(aDataPtr->mMotionType))
      {
         SetYawPitchRoll(aDataPtr->mYPR[0], aDataPtr->mYPR[1], aDataPtr->mYPR[2]);
         mViewMatrix = aDataPtr->mViewMatrix;
      }
      delete aDataPtr;
   }
   mDirty = true;
}

void VaCameraMotionGeocentric::Reset()
{
   mPanPossible = false;
   mPanActive   = false;
   mPanX        = 0;
   mPanY        = 0;
   mAz          = 0.0;
   mEl          = 0.0;
   mDist        = cDefaultCameraDistance;
   mYaw         = 0.0;
   mPitch       = 90.0;
   mDirty       = true;
}

bool VaCameraMotionGeocentric::Update(double aTime)
{
   VaCameraMotion::Update(aTime);

   if (GetCamera()->GetViewer()->UseECI())
   {
      mECI_Offset += (aTime - mTime) / 3600.0 * 15.0 * UtMath::cRAD_PER_DEG; // convert seconds into radians
      mDirty = true;
   }
   else
   {
      mECI_Offset = 0.0;
   }

   mTime = aTime;
   if (mAnimationStruct)
   {
      mECI_Offset = 0;
      double time = vespa::VaEnvironment::Instance().GetTime()->GetWallClockTime();
      if (time - mAnimationStruct->mStartTime >= mAnimationStruct->mDuration)
      {
         // complete the animation
         SetReferencePosition(VaPosition(mAnimationStruct->mNewLat, mAnimationStruct->mNewLon, mAnimationStruct->mNewDist));
         CheckElLimits();
         SetYawPitchRoll(mAnimationStruct->mNewYaw, mAnimationStruct->mNewPitch, 0.0);

         sDistCacheMap[0] = mDist;
         BuildViewMatrix();
         mDirty = false;
         mAnimationStruct.reset();
      }
      else
      {
         double factor = (time - mAnimationStruct->mStartTime) / mAnimationStruct->mDuration;
         double lat    = (1 - factor) * mAnimationStruct->mOldLat + factor * mAnimationStruct->mNewLat;
         double lon    = (1 - factor) * mAnimationStruct->mOldLon + factor * mAnimationStruct->mNewLon;
         double yaw    = (1 - factor) * mAnimationStruct->mOldYaw + factor * mAnimationStruct->mNewYaw;
         if (fabs(mAnimationStruct->mNewYaw - mAnimationStruct->mOldYaw) > 180)
         {
            if (mAnimationStruct->mOldYaw > mAnimationStruct->mNewYaw)
            {
               yaw = (1 - factor) * (mAnimationStruct->mOldYaw - 360) + factor * mAnimationStruct->mNewYaw;
            }
            else
            {
               yaw = (1 - factor) * (mAnimationStruct->mOldYaw + 360) + factor * mAnimationStruct->mNewYaw;
            }
         }
         double pitch = (1 - factor) * mAnimationStruct->mOldPitch + factor * mAnimationStruct->mNewPitch;
         if (fabs(mAnimationStruct->mNewLon - mAnimationStruct->mOldLon) >
             180) // crossing the dateline, lets not go the long way around
         {
            if (mAnimationStruct->mOldLon > mAnimationStruct->mNewLon)
            {
               lon = (1 - factor) * (mAnimationStruct->mOldLon - 360) + factor * mAnimationStruct->mNewLon;
            }
            else
            {
               lon = (1 - factor) * (mAnimationStruct->mOldLon + 360) + factor * mAnimationStruct->mNewLon;
            }
         }
         double dist = (1 - factor) * mAnimationStruct->mOldDist + factor * mAnimationStruct->mNewDist;
         SetReferencePosition(VaPosition(lat, lon, dist));
         SetYawPitchRoll(yaw, pitch, 0.0);
         CheckElLimits();
         mRefLLA.SetLat(mEl);
         mRefLLA.SetLon(mAz);
         mRefLLA.SetAlt(0.0);

         sDistCacheMap[0] = mDist;
         BuildViewMatrix();
         // interpolate
         mDirty = true;
      }
      mDirty = true;
   }
   else if (mDirty)
   {
      mDirty = false;

      mRefLLA.SetLat(mEl);
      mRefLLA.SetLon(mAz);
      mRefLLA.SetAlt(0.0);

      sDistCacheMap[0] = mDist;
      BuildViewMatrix();
   }

   return mDirty;
}

VaCameraMotionData* VaCameraMotionGeocentric::GetMotionData() const
{
   double aed[3] = {mAz, mEl, mDist};
   double ypr[3] = {mYaw, mPitch, 0.0};
   return new VaCameraMotionGeocentricData(mLLA, mRefLLA, aed, ypr, mViewMatrix);
}

bool VaCameraMotionGeocentric::SupportsMotionData(MotionType aMotionType) const
{
   bool supports = false;
   switch (aMotionType)
   {
   case cGEOCENTRIC:
      supports = true;
      break;
   case cTETHERED:
      supports = false;
      break;
   case cLOOK_AT:
      supports = false;
      break;
   case cFLY_TO:
      supports = false;
      break;
   default:
      supports = false;
      break;
   }
   return supports;
}

VaCameraMotionData* VaCameraMotionGeocentric::GetMotionDataDefault()
{
   static const double aed[3] = {0.0, 0.0, 10270288.0};
   static const double ypr[3] = {0.0, 90.0, 0.0};

   VaPosition lla(0.0, 0.0, 10270288.0);
   VaPosition refLla(0.0, 0.0, 0.0);
   return new VaCameraMotionGeocentricData(lla, refLla, aed, ypr, osg::Matrix::identity());
}

void VaCameraMotionGeocentric::GetViewMatrixAndCameraPosition(osg::Matrix& aViewMatrix, double aXYZ[3]) const
{
   aViewMatrix = mViewMatrix;
   mLLA.GetSceneXYZ(aXYZ, GetCamera()->GetViewer()->GetUniqueId());
}

VaPosition VaCameraMotionGeocentric::GetReferencePosition()
{
   return mRefLLA;
}

void VaCameraMotionGeocentric::SetReferencePosition(const VaPosition& aRefLLA)
{
   mRefLLA = aRefLLA;
   SetAzElDist(mRefLLA.GetLon(), mRefLLA.GetLat(), mRefLLA.GetAlt());
   mDirty = true;
}


void VaCameraMotionGeocentric::SetPosition(const VaPosition& aLLA)
{
   mLLA = aLLA;
   SetAzElDist(mLLA.GetLon(), mLLA.GetLat(), mLLA.GetAlt());
   mDirty = true;
}

VaPosition VaCameraMotionGeocentric::GetPosition()
{
   return mLLA;
}

double VaCameraMotionGeocentric::GetAz() const
{
   return mAz;
}

void VaCameraMotionGeocentric::SetAz(double aAz)
{
   mAz    = aAz;
   mDirty = true;
}

void VaCameraMotionGeocentric::SetEl(double aEl)
{
   mEl    = aEl;
   mDirty = true;
}

void VaCameraMotionGeocentric::SetDist(double aDist)
{
   double           limit         = cMaxCameraDistanceRoundEarth;
   UtoMapProjection mapProjection = GetCamera()->GetViewer()->GetMapProjection();
   if (mapProjection.projection == UtoMapProjection::Equirectangular)
   {
      limit = cMaxCameraDistanceFlatEarth;
   }

   mDist  = std::min(limit, std::max(1.0, aDist));
   mDirty = true;
}

void VaCameraMotionGeocentric::GetAzElDist(double& aAz, double& aEl, double& aDist) const
{
   aAz   = mAz - UtMath::cDEG_PER_RAD * mECI_Offset;
   aEl   = mEl;
   aDist = GetDist();
}

void VaCameraMotionGeocentric::SetAzElDist(double aAz, double aEl, double aDist)
{
   mAz = aAz;
   mEl = aEl;
   SetDist(aDist);
   mDirty = true;
}

bool VaCameraMotionGeocentric::MouseScroll(int aDirection)
{
   bool handled = false;

   if (GetCamera()->GetViewer()->GetHeight() > 0)
   {
      if (HandleSoftLock(cZOOM))
      {
         handled = true;

         double deltaY = (aDirection * 0.0005);
         double lat    = mRefLLA.GetLat();
         double lon    = mRefLLA.GetLon();
         double dist   = mDist;
         if (mAnimationStruct)
         {
            lat  = mAnimationStruct->mNewLat;
            lon  = mAnimationStruct->mNewLon;
            dist = mAnimationStruct->mNewDist;
         }

         double newDistance;
         newDistance = dist * exp(deltaY * 5); // this is a continuous form of the function described by the linear
         // approximation: mDistance * (1.0 + 5.0 * deltaY).  Meaning with an
         // unpredictable frame rate it will still behave smoothly.
         if (newDistance < 1.0)
         {
            newDistance = 1.0;
         }
         if (newDistance != mDist)
         {
            // Change lat, lon to keep the mouse over the same location when zooming
            // Get lat/lon of mouse cursor
            VaPosition cursorPos(VaEnvironment::Instance().GetCurrentCursorPosition());
            // Get heading and distance along great circle from camera center to mouse cursor
            double cameraToCursorHeading, cameraToCursorDist;
            UtSphericalEarth::GreatCircleHeadingAndDistance(lat,
                                                            lon,
                                                            cursorPos.GetLat(),
                                                            cursorPos.GetLon() + mECI_Offset * UtMath::cDEG_PER_RAD,
                                                            cameraToCursorHeading,
                                                            cameraToCursorDist);
            // Extrapolate along line from camera center to mouse cursor in accordance
            // with how the camera distance changed, to get new camera position
            double newLat, newLon;
            UtSphericalEarth::ExtrapolateGreatCirclePosition(lat,
                                                             lon,
                                                             cameraToCursorHeading + (aDirection < 0 ? 0 : 180),
                                                             std::abs(1 - newDistance / dist) * cameraToCursorDist,
                                                             newLat,
                                                             newLon);
            mAnimationStruct =
               ut::make_unique<AnimationStruct>(mRefLLA.GetLat(),
                                                mRefLLA.GetLon(),
                                                mDist,
                                                mYaw,
                                                mPitch,
                                                newLat,
                                                newLon,
                                                newDistance,
                                                mYaw,
                                                mPitch,
                                                vespa::VaEnvironment::Instance().GetTime()->GetWallClockTime(),
                                                0.25);
            /*         // Set new camera reference position
                     SetReferencePosition(VaPosition(newLat, newLon, newDistance));
                     CheckElLimits();*/
            GetCamera()->GetViewer()->RequestUpdate();
         }
      }
   }

   return handled;
}

bool VaCameraMotionGeocentric::MouseMotion(int aX, int aY, unsigned int aState)
{
   bool handled = false;

   const int state = aState & (~VaViewer::CapsNumLockStates);

   if (mLeftDown && mPanPossible && GetCamera()->GetViewer()->GetHeight() > 0 &&
       (!mEnableTouchScreenDelay || (VaEnvironment::Instance().GetTime()->GetWallClockTime() - mTimeMouseEventStarted) >
                                       mTimeMouseEventDelay)) // time delay put in to account for touch devices
   {
      if (HandleSoftLock(cPRIMARY_ROTATION))
      {
         handled = true;

         mPanActive = true;
         double deltaX =
            (aX - mPanX) /
            double(GetCamera()->GetViewer()->GetHeight()); // changed this from mViewportWidth... scale is defined by
         double deltaY = (aY - mPanY) / double(GetCamera()->GetViewer()->GetHeight());
         mPanX         = aX;
         mPanY         = aY;

         if (state == 0)
         {
            // Determine the approximate size of a pixel in degrees...

            double factor = mDist / cApproxEarthRadius;
            factor *= 0.25;
            factor = std::min(1.0, factor); // limiting this allows for 1:1 motion close to the surface, and smooth
                                            // motion far out in space

            deltaX *= factor;
            deltaY *= factor;

            // vertical   drag translates along azimuth.
            // horizontal drag translates normal to azimuth.

            // latitudinal component of azimuth
            double cosPsi = cos(mYaw * UtMath::cRAD_PER_DEG);
            // longitudinal component of azimuth
            double sinPsi = sin(mYaw * UtMath::cRAD_PER_DEG);

            // first  term is for translation along azimuth.
            // second term is for translation normal to azimuth.

            double deltaAz = (deltaY * sinPsi) + (deltaX * cosPsi);
            double deltaEl = (deltaY * cosPsi) - (deltaX * sinPsi);

            deltaEl *= 180.0 * mPanDragMultiplier;
            deltaAz *= 180.0 * mPanDragMultiplier;

            double newEl = mEl - deltaEl;

            double newAz = WrapAngleNeg180To180(mAz - deltaAz);

            if ((newAz != mAz) || (newEl != mEl))
            {
               mAz = newAz;
               mEl = newEl;
               CheckElLimits();
               mDirty = true;
               GetCamera()->GetViewer()->RequestUpdate();
            }
         }
      }
   }

   if (!mFlatMapMode && mMiddleDown)
   {
      if (HandleSoftLock(cSECONDARY_ROTATION))
      {
         // handle pitch / yaw
         double deltaX =
            (aX - mPanX) /
            double(GetCamera()->GetViewer()->GetHeight()); // changed this from mViewportWidth... scale is defined by
         double deltaY = (aY - mPanY) / double(GetCamera()->GetViewer()->GetHeight());
         mPanX         = aX;
         mPanY         = aY;

         // Azimuth change
         double deltaYaw = deltaX * 360.0;
         double newYaw   = WrapAngle0To360(mYaw + deltaYaw);

         // Elevation change
         double deltaPitch = deltaY * 90.0;
         double newPitch   = mPitch - deltaPitch;
         if (newPitch < 0.0)
         {
            newPitch = 0.0;
         }
         else if (newPitch > 90.0)
         {
            newPitch = 90.0;
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

   mDirty |= handled;
   return handled;
}

bool VaCameraMotionGeocentric::PassiveMouseMotion(int aX, int aY, unsigned int aState)
{
   bool handled = false;
   return handled;
}

bool VaCameraMotionGeocentric::MouseButtonPress(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   bool handled = false;

   const int state = aState & (~VaViewer::CapsNumLockStates);

   if (mEnableTouchScreenDelay)
   {
      mTimeMouseEventStarted = VaEnvironment::Instance().GetTime()->GetWallClockTime();
   }

   switch (aButton)
   {
   case 1:
      handled      = true;
      mLeftDown    = true;
      mPanPossible = false;

      if (state == 0)
      {
         // Pan
         mPanPossible = true;
         mPanX        = aX;
         mPanY        = aY;
      }
      break;
   case 2:
      handled     = true;
      mMiddleDown = true;
      mPanX       = aX;
      mPanY       = aY;
      break;
   case 3:
      handled    = true;
      mRightDown = true;
      break;
   default:
      break;
   }

   mDirty |= handled;
   return handled;
}

bool VaCameraMotionGeocentric::MouseButtonRelease(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   bool handled = false;

   if (aButton == 1)
   {
      handled      = true;
      mLeftDown    = false;
      mPanPossible = false;
      mPanActive   = false;
   }
   else if (aButton == 2)
   {
      handled     = true;
      mMiddleDown = false;
   }
   else if (aButton == 3)
   {
      handled    = true;
      mRightDown = false;
   }

   mDirty |= handled;
   return handled;
}

bool VaCameraMotionGeocentric::MouseButtonDoublePress(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   bool handled = false;

   const int state = aState & (~VaViewer::CapsNumLockStates);

   if (aButton == 1)
   {
      if (state == 0)
      {
         if (HandleSoftLock(cZOOM))
         {
            handled = true;

            double lla[3];
            if (GetCamera()->ConvertScreenToLLA(aX, aY, lla[0], lla[1], lla[2]))
            {
               mEl    = lla[0];
               mAz    = lla[1] + mECI_Offset * UtMath::cDEG_PER_RAD;
               mDirty = true;
               GetCamera()->GetViewer()->RequestUpdate();
            }

            double newDistance = std::max(mDist * exp(-1.0), 1.0);
            if (newDistance != mDist)
            {
               SetDist(newDistance);
               GetCamera()->GetViewer()->RequestUpdate();
            }
            CheckElLimits();
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
   return handled;
}

bool VaCameraMotionGeocentric::GesturePinch(double aScale, double aAngle, int aCenterX, int aCenterY)
{
   bool handled = false;

   mPanPossible = false;

   if (GetCamera()->GetViewer()->GetHeight() > 0)
   {
      if (HandleSoftLock(cZOOM))
      {
         handled = true;

         double reverseScale =
            1 + (1 - aScale); // reverse so that it zooms in when opening fingers and zooms out when closing fingers

         double newDistance = mDist * reverseScale;
         // approximation: mDistance * (1.0 + 5.0 * deltaY).  Meaning with an
         // unpredictable frame rate it will still behave smoothly.
         if (newDistance < 1.0)
         {
            newDistance = 1.0;
         }

         if (newDistance != mDist)
         {
            SetDist(newDistance);
            GetCamera()->GetViewer()->RequestUpdate();
         }

         if (mDebug)
         {
            auto out = ut::log::debug() << "GesturePinch():";
            out.AddNote() << "Scale: " << aScale;
            out.AddNote() << "Angle: " << aAngle;
            out.AddNote() << "Center: " << aCenterX << ", " << aCenterY;
            out.AddNote() << "New Distance: " << newDistance;
         }
      }
   }

   mDirty |= handled;
   return handled;
}

// private
void VaCameraMotionGeocentric::BuildViewMatrix()
{
   VaPosition posXYZ;
   LLAToWorld(mEl, 0.0, 0.0, posXYZ);

   double xyz[3];
   posXYZ.GetECEF(xyz);

   double earthRadius = sqrt(xyz[0] * xyz[0] + xyz[2] * xyz[2]);
   double gcLat       = asin(xyz[2] / earthRadius) * UtMath::cDEG_PER_RAD;

   UtoMapProjection mapProjection = GetCamera()->GetViewer()->GetMapProjection();

   if (mapProjection.projection == UtoMapProjection::Equirectangular)
   {
      double dist = mDist;
      /*      if (mFlatMapMode)
            {
               dist = 48000000.0;
            }*/
      double newAz  = WrapAngleNeg180To180(mAz - mapProjection.longitudeOffset);
      double xyz[3] = {-newAz * cApproxMetersPerDegreeAtEquator, -mEl * cApproxMetersPerDegreeAtEquator, -dist};
      mViewMatrix =
         osg::Matrixd::translate(xyz[0], xyz[1], 0) * osg::Matrixd::rotate(osg::DegreesToRadians(mYaw), 0.0, 0.0, 1.0) *
         osg::Matrixd::rotate(osg::DegreesToRadians(mPitch - 90.0), 1.0, 0.0, 0.0) * osg::Matrixd::translate(0, 0, -dist);
   }
   else
   {
      // Build the view matrix.
      mViewMatrix = osg::Matrixd(osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(1, 0, 0)) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(0, 1, 0)) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(-mAz) + mECI_Offset, 0.0, 1.0, 0.0) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(gcLat), 1.0, 0.0, 0.0) *
                                 osg::Matrixd::translate(0.0, 0.0, -earthRadius) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(mYaw), 0.0, 0.0, 1.0) *
                                 osg::Matrixd::rotate(osg::DegreesToRadians(mPitch - 90.0), 1.0, 0.0, 0.0) *
                                 osg::Matrixd::translate(0.0, 0.0, -mDist));
   }

   osg::Vec3 camerapos = osg::Vec3(0, 0, 0) * osg::Matrix::inverse(mViewMatrix);
   double    ecef[3]   = {camerapos[0], camerapos[1], camerapos[2]};
   double    lla[3];
   GetCamera()->ConvertXYZToLLA(ecef, lla[0], lla[1], lla[2]);
   mLLA.SetLLA(lla[0], lla[1], lla[2]);
   VaObserver::ViewMatrixChanged(GetCamera()->GetViewer());
}

void VaCameraMotionGeocentric::BoxZoom(const VaSelectionBox& aBox)
{
   VaPosition firstCorner(aBox.GetLLA1()[0], aBox.GetLLA1()[1], 0.0);
   VaPosition secondCorner(aBox.GetLLA3()[0], aBox.GetLLA3()[1], 0.0);

   double x1, y1, z1;
   GetCamera()->GetViewer()->WorldToScreen(firstCorner, x1, y1, z1);

   double x2, y2, z2;
   GetCamera()->GetViewer()->WorldToScreen(secondCorner, x2, y2, z2);

   // Determine the new altitude
   double xscale = GetCamera()->GetViewer()->GetWidth() / (x2 - x1);
   double yscale = GetCamera()->GetViewer()->GetHeight() / (y2 - y1);
   if (xscale < yscale)
   {
      SetDist(mDist / xscale);
   }
   else
   {
      SetDist(mDist / yscale);
   }

   UtVec3d    boxLLA = aBox.GetCenterLLA();
   VaPosition pos(boxLLA[0], boxLLA[1], mDist);
   SetPosition(pos);
}

void VaCameraMotionGeocentric::LLAToWorld(double aLat, double aLon, double aAlt, VaPosition& aPosition)
{
   double alt = aAlt;
   double lat = aLat;
   double lon = aLon;
   if (aLat < -1.0e12)
   {
      lat = -90.0;
   }
   if (aLon < -1.0e12)
   {
      lon = -360.0;
   }
   aPosition.SetLLA(UtLLAPos(lat, lon, alt));
}

// virtual
double VaCameraMotionGeocentric::GetGeographicHeading()
{
   double heading;
   double distance;

   UtSphericalEarth::GreatCircleHeadingAndDistance(mLLA.GetLat(), mLLA.GetLon(), mEl, mAz, heading, distance);

   if (mPitch < 85) // I think there is a problem because we are using both spherical and ellipsoidal coordinates.
                    // This threshold is a hack to avoid discontinuity when transitioning between overhead to
                    // more acute viewing angles.
   {
      return -heading;
   }
   else
   {
      return -mYaw;
   }
}

void VaCameraMotionGeocentric::ForceRefresh()
{
   mDirty = true;
}

void VaCameraMotionGeocentric::HandleSelectionBoxResultCB(const std::string&    aBoxOperation,
                                                          VaViewer*             aViewerPtr,
                                                          const VaSelectionBox& aSelectionBox)
{
   if ((GetCamera()->GetViewer() == aViewerPtr) && (aBoxOperation == "zoom"))
   {
      BoxZoom(aSelectionBox);
   }
}

void VaCameraMotionGeocentric::FlatMapMode(bool aLock)
{
   mFlatMapMode = aLock;
   if (mFlatMapMode)
   {
      mYaw   = 0;
      mPitch = 90.0;
   }
   mDirty = true;
   GetCamera()->GetViewer()->RequestUpdate();
}

bool VaCameraMotionGeocentric::RecommendNearFar(double& aNear, double& aFar)
{
   aNear          = 0.1 * GetDist();
   double geosync = 42000000.0;
   aFar           = GetDist() * 2.0 + geosync;
   return true;
}

void VaCameraMotionGeocentric::AnimateToPosition(const VaPosition& aPosition, double aYaw, double aPitch, double aTime)
{
   // we apply the eci offset here to make sure we start at the right position, the eci offset will be cleared during
   // animation to make sure that we command the animation to the appropriate lla.
   mAnimationStruct = ut::make_unique<AnimationStruct>(mRefLLA.GetLat(),
                                                       mRefLLA.GetLon() - UtMath::cDEG_PER_RAD * mECI_Offset,
                                                       mDist,
                                                       mYaw,
                                                       mPitch,
                                                       aPosition.GetLat(),
                                                       aPosition.GetLon(),
                                                       aPosition.GetAlt(),
                                                       aYaw,
                                                       aPitch,
                                                       vespa::VaEnvironment::Instance().GetTime()->GetWallClockTime(),
                                                       aTime);
   GetCamera()->GetViewer()->RequestUpdate();
}

void VaCameraMotionGeocentric::CheckElLimits()
{
   if (mFlatMapMode)
   {
      float halfScreen = mDist / (UtMath::cM_PER_NM * 120.0) *
                         ((float)(GetCamera()->GetViewer()->GetHeight()) / (float)(GetCamera()->GetViewer()->GetWidth()));
      if (halfScreen > 90.0)
      {
         mEl = 0;
      }
      else if (mEl + halfScreen > 90.0)
      {
         mEl = 90 - halfScreen;
      }
      else if (mEl - halfScreen < -90)
      {
         mEl = -90 + halfScreen;
      }
   }
   else
   {
      if (mEl > 90.0)
      {
         mEl = 90.0;
      }
      else if (mEl < -90.0)
      {
         mEl = -90.0;
      }
   }
}

} // end namespace vespa
