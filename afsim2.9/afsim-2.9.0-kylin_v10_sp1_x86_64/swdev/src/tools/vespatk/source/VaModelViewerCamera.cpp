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

#include "VaModelViewerCamera.hpp"

#include <osg/Matrix>
#include <osg/Vec3>

#include "UtEllipsoidalEarth.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"

namespace vespa
{

VaModelViewerCamera::VaModelViewerCamera(VaViewer* aViewerPtr)
   : VaCamera(aViewerPtr)
{
   delete mCameraMotionPtr;
   mCameraMotionPtr = new VaModelViewerCameraMotion(this);
}

VaModelViewerCameraMotion::VaModelViewerCameraMotion(VaCamera* aCameraPtr)
   : VaCameraMotion(aCameraPtr)
   , mAz(-30)
   , mEl(45)
   , mDist(50)
   , mLLA(0, 0, 30000)
   , mDirty(true)
   , mLeftDown(false)
   , mMiddleDown(false)
   , mRightDown(false)
   , mPanPossible(false)
   , mZoomPossible(false)
   , mPanActive(false)
   , mPanX(0)
   , mPanY(0)
   , mZoomX(0)
   , mZoomY(0)
{
   BuildViewMatrix();
}

void VaModelViewerCameraMotion::GetViewMatrixAndCameraPosition(osg::Matrix& aViewMatrix, double aXYZ[3]) const
{
   aViewMatrix = mViewMatrix;
   mLLA.GetSceneXYZ(aXYZ, GetCamera()->GetViewer()->GetUniqueId());
}

void VaModelViewerCameraMotion::GetAzElDist(double& aAz, double& aEl, double& aDist) const
{
   aAz   = mAz;
   aEl   = mEl;
   aDist = mDist;
}

void VaModelViewerCameraMotion::SetAzElDist(double aAz, double aEl, double aDist)
{
   mAz   = aAz;
   mEl   = aEl;
   mDist = aDist;
   BuildViewMatrix();
}

bool VaModelViewerCameraMotion::MouseScroll(int aDirection)
{
   bool handled = false;

   if (GetCamera()->GetViewer()->GetHeight() > 0)
   {
      handled = true;

      double deltaY = (aDirection) / double(GetCamera()->GetViewer()->GetHeight());

      double newDistance;
      newDistance = mDist * exp(deltaY * 5); // this is a continuous form of the function described by the linear
      // approximation: mDistance * (1.0 + 5.0 * deltaY).  Meaning with an
      // unpredictable frame rate it will still behave smoothly.

      if (newDistance != mDist)
      {
         mDist  = newDistance;
         mDirty = true;
         GetCamera()->GetViewer()->RequestUpdate();
      }
   }
   BuildViewMatrix();

   return handled;
}

bool VaModelViewerCameraMotion::MouseMotion(int aX, int aY, unsigned int aState)
{
   bool handled = false;

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
         // Azimuth change
         double deltaAz = deltaX * 360.0;
         double newAz   = mAz + deltaAz;
         while (newAz < 0.0)
         {
            newAz += 360.0;
         }
         while (newAz >= 360.0)
         {
            newAz -= 360.0;
         }

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

   if ((mMiddleDown) || ((mRightDown) && (state == VaViewer::CtrlKey)))
   {
      if (mZoomPossible && GetCamera()->GetViewer()->GetHeight() > 0)
      {
         handled = true;

         double deltaY = (aY - mZoomY) / double(GetCamera()->GetViewer()->GetHeight());
         mZoomX        = aX;
         mZoomY        = aY;

         if (state == 0)
         {
            double newDistance;
            newDistance = mDist * exp(deltaY * 5); // this is a continuous form of the function described by the linear
            // approximation: mDistance * (1.0 + 5.0 * deltaY).  Meaning with an
            // unpredictable frame rate it will still behave smoothly.

            if (newDistance != mDist)
            {
               mDist  = newDistance;
               mDirty = true;
               GetCamera()->GetViewer()->RequestUpdate();
            }
         }
      }
   }
   mDirty |= handled;
   BuildViewMatrix();

   return handled;
}

bool VaModelViewerCameraMotion::GesturePinch(double aScale, double aAngle, int aCenterX, int aCenterY)
{
   bool handled = false;

   mDirty |= handled;
   return handled;
}

bool VaModelViewerCameraMotion::PassiveMouseMotion(int aX, int aY, unsigned int aState)
{
   bool handled = false;
   return handled;
}

bool VaModelViewerCameraMotion::MouseButtonPress(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   bool handled = false;

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
   BuildViewMatrix();
   return handled;
}

bool VaModelViewerCameraMotion::MouseButtonRelease(int aX, int aY, unsigned int aButton, unsigned int aState)
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
   BuildViewMatrix();
   return handled;
}

bool VaModelViewerCameraMotion::MouseButtonDoublePress(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   bool handled = false;
   return handled;
}

void VaModelViewerCameraMotion::BuildViewMatrix()
{
   double     xyz[3];
   VaPosition foo(0, 0, 30000);
   foo.GetSceneXYZ(xyz, GetCamera()->GetViewer()->GetUniqueId());

   mViewMatrix = osg::Matrixd(osg::Matrixd::translate(-xyz[0], -xyz[1], -xyz[2]) *
                              osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(1, 0, 0)) *
                              osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(0, 1, 0)) *
                              osg::Matrixd::rotate(osg::DegreesToRadians(mAz), 0.0, 0.0, 1.0) *
                              osg::Matrixd::rotate(osg::DegreesToRadians(mEl - 90.0), 1.0, 0.0, 0.0) *
                              osg::Matrixd::translate(0.0, 0.0, -mDist));


   osg::Vec3 camerapos = osg::Vec3(0, 0, 0) * osg::Matrix::inverse(mViewMatrix);
   double    xyz2[3]   = {camerapos[0], camerapos[1], camerapos[2]};
   double    lla[3];
   UtEllipsoidalEarth::ConvertECEFToLLA(xyz2, lla[0], lla[1], lla[2]);
   mLLA.SetLLA(lla[0], lla[1], lla[2]);
   VaObserver::ViewMatrixChanged(GetCamera()->GetViewer());
}

} // namespace vespa
