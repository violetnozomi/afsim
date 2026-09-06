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
#include "VaCameraMotionLookAt.hpp"

#include <iomanip>
#include <iostream>
#include <string>

#include <osg/Matrix>
#include <osg/Vec3>

#include "UtEllipsoidalEarth.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "VaCameraBase.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaPosition.hpp"
#include "VaTimeManager.hpp"
#include "VaViewer.hpp"

namespace vespa
{
osg::Vec3d GetEyePosition(const VaPosition& aRefPosition, double aAz, double aEl, double aDist)
{
   double xyz[3];
   aRefPosition.GetSceneXYZ(xyz, 0);

   osg::Matrixd vm = osg::Matrixd(osg::Matrixd::translate(-xyz[0], -xyz[1], -xyz[2]) *
                                  osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(1, 0, 0)) *
                                  osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(0, 1, 0)) *
                                  osg::Matrixd::rotate(osg::DegreesToRadians(-aRefPosition.GetLon()), 0.0, 1.0, 0.0) *
                                  osg::Matrixd::rotate(osg::DegreesToRadians(aRefPosition.GetLat()), 1.0, 0.0, 0.0) *
                                  osg::Matrixd::rotate(osg::DegreesToRadians(aAz), 0.0, 0.0, 1.0) *
                                  osg::Matrixd::rotate(osg::DegreesToRadians(aEl - 90.0), 1.0, 0.0, 0.0) *
                                  osg::Matrixd::translate(0.0, 0.0, -aDist));

   osg::Vec3 eye = osg::Vec3(0, 0, 0) * osg::Matrix::inverse(vm);
   return eye;
}

VaCameraMotionLookAt::VaCameraMotionLookAt(VaCameraBase* aCameraPtr, VaEntity* aReferenceEntityPtr)
   : VaCameraMotion(aCameraPtr)
   , mReferenceEntityPtr(aReferenceEntityPtr)
   , mLookAtEntityPtr(nullptr)
   , mDebug(false)
   , mLLA()
   , mRefPosition()
   , mLookAtPosition()
   , mDist(1.0)
   , mUseRefHeading(true)
   , mRefHeading(0.0)
   , mDirty(true)
   , mCallbacksRef()
   , mCallbacksLookAt()
{
   SetReferencePosition(mReferenceEntityPtr->GetPosition());

   mViewMatrix.makeIdentity();

   mCallbacksRef.Add(
      VaObserver::EntityMoved.Connect(&VaCameraMotionLookAt::EntityMovedCB, this, mReferenceEntityPtr->GetUniqueId()));
   mCallbacksRef.Add(
      VaObserver::EntityRemoved.Connect(&VaCameraMotionLookAt::EntityRemovedCB, this, mReferenceEntityPtr->GetUniqueId()));
   mCallbacksRef.Add(
      VaObserver::EntityDeleted.Connect(&VaCameraMotionLookAt::EntityRemovedCB, this, mReferenceEntityPtr->GetUniqueId()));
}

VaCameraMotionLookAt::~VaCameraMotionLookAt()
{
   mCallbacksRef.Clear();
   mCallbacksLookAt.Clear();
}

void VaCameraMotionLookAt::Initialize(VaCameraMotionData* aDataPtr)
{
   VaCameraMotion::Initialize(aDataPtr);

   if (aDataPtr != nullptr)
   {
      if (SupportsMotionData(aDataPtr->mMotionType))
      {
         mAz         = aDataPtr->mAED[0];
         mEl         = aDataPtr->mAED[1];
         mDist       = aDataPtr->mAED[2];
         mViewMatrix = aDataPtr->mViewMatrix;
      }
      delete aDataPtr;
   }
   mDirty = true;
}

void VaCameraMotionLookAt::Reset()
{
   mDist          = 1.0;
   mUseRefHeading = true;
   mRefHeading    = 0.0;
   mDirty         = true;
}

// private
void VaCameraMotionLookAt::EntityMovedCB(VaEntity* aEntityPtr, bool aMoveInTime)
{
   if ((aEntityPtr != nullptr) && (aEntityPtr == mLookAtEntityPtr || aEntityPtr == mReferenceEntityPtr))
   {
      mDirty = true; // note that we will need to update ourselves
      GetCamera()->GetViewer()->RequestUpdate();
   }
}

// private
void VaCameraMotionLookAt::EntityRemovedCB(VaEntity* aEntityPtr)
{
   if (aEntityPtr != nullptr)
   {
      if (aEntityPtr == mReferenceEntityPtr)
      {
         mReferenceEntityPtr = nullptr;
         mCallbacksRef.Clear();
         LookAtClear();

         mDirty = true; // note that we will need to update ourselves
         GetCamera()->GetViewer()->RequestUpdate();
      }
      else if (aEntityPtr == mLookAtEntityPtr)
      {
         LookAtClear();

         mDirty = true; // note that we will need to update ourselves
         GetCamera()->GetViewer()->RequestUpdate();
      }
   }
}

bool VaCameraMotionLookAt::Update(double aTime)
{
   VaCameraMotion::Update(aTime);

   if (mDirty)
   {
      mDirty = false;

      if (mReferenceEntityPtr != nullptr)
      {
         SetReferencePosition(mReferenceEntityPtr->GetPosition());
      }
      if (mLookAtEntityPtr != nullptr)
      {
         mLookAtPosition = mLookAtEntityPtr->GetPosition();
      }
      BuildViewMatrix(mRefPosition);
   }
   return mDirty;
}

VaCameraMotionLookAtData* VaCameraMotionLookAt::GetMotionData() const
{
   double aed[3] = {0.0, 0.0, 1.0};
   double ypr[3] = {0.0, 90.0, 0.0};
   return new VaCameraMotionLookAtData(mLLA, mRefPosition, aed, ypr, mViewMatrix);
}

bool VaCameraMotionLookAt::SupportsMotionData(MotionType aMotionType) const
{
   bool supports = false;

   switch (aMotionType)
   {
   case cGEOCENTRIC:
      supports = false;
      break;
   case cTETHERED:
      supports = false;
      break;
   case cLOOK_AT:
      supports = true;
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

VaCameraMotionLookAtData* VaCameraMotionLookAt::GetMotionDataDefault()
{
   static const double aed[3] = {0.0, 0.0, 1.0};
   static const double ypr[3] = {0.0, 90.0, 0.0};

   VaPosition lla(0, 0, 0);

   return new VaCameraMotionLookAtData(lla, lla, aed, ypr, osg::Matrix::identity());
}

void VaCameraMotionLookAt::GetViewMatrixAndCameraPosition(osg::Matrix& aViewMatrix, double aXYZ[3]) const
{
   aViewMatrix = mViewMatrix;
   mLLA.GetSceneXYZ(aXYZ, GetCamera()->GetViewer()->GetUniqueId());
}

VaPosition VaCameraMotionLookAt::GetPosition()
{
   return mLLA;
}

void VaCameraMotionLookAt::SetPosition(const VaPosition& aPosition)
{
   mLLA   = aPosition;
   mDirty = true;
}

VaPosition VaCameraMotionLookAt::GetReferencePosition()
{
   return mRefPosition;
}

void VaCameraMotionLookAt::SetReferencePosition(const VaPosition& aPosition)
{
   mRefPosition = aPosition;
}

void VaCameraMotionLookAt::GetAzElDist(double& aAz, double& aEl, double& aDist) const
{
   aAz   = 0.0;
   aEl   = 0.0;
   aDist = mDist;
}

void VaCameraMotionLookAt::SetAzElDist(double aAz, double aEl, double aDist)
{
   mAz    = aAz;
   mEl    = aEl;
   mDist  = aDist;
   mDirty = true;
}

void VaCameraMotionLookAt::SetEl(double aEl)
{
   mEl    = aEl;
   mDirty = true;
}

void VaCameraMotionLookAt::SetAz(double aAz)
{
   mAz    = aAz;
   mDirty = true;
}

void VaCameraMotionLookAt::SetDist(double aDist)
{
   mDist  = aDist;
   mDirty = true;
}

void VaCameraMotionLookAt::GetYawPitchRoll(double& aYaw, double& aPitch, double& aRoll)
{
   aYaw   = 0.0;
   aPitch = 0.0;
   aRoll  = 0.0;
}

void VaCameraMotionLookAt::SetYawPitchRoll(double aYaw, double aPitch, double aRoll) {}

void VaCameraMotionLookAt::ForceRefresh()
{
   mDirty = true;
}

void VaCameraMotionLookAt::LookAt(const VaPosition& aPosition)
{
   mLookAtPosition = aPosition;
   mDirty          = true;
}

void VaCameraMotionLookAt::LookAt(VaEntity* aLookAtEntityPtr)
{
   if (aLookAtEntityPtr != nullptr)
   {
      mLookAtEntityPtr = aLookAtEntityPtr;
      mLookAtPosition  = mLookAtEntityPtr->GetPosition();
      mCallbacksLookAt.Add(
         VaObserver::EntityMoved.Connect(&VaCameraMotionLookAt::EntityMovedCB, this, mLookAtEntityPtr->GetUniqueId()));
      mCallbacksLookAt.Add(
         VaObserver::EntityRemoved.Connect(&VaCameraMotionLookAt::EntityRemovedCB, this, mLookAtEntityPtr->GetUniqueId()));
      mCallbacksLookAt.Add(
         VaObserver::EntityDeleted.Connect(&VaCameraMotionLookAt::EntityRemovedCB, this, mLookAtEntityPtr->GetUniqueId()));
      mDirty = true;
   }
}

void VaCameraMotionLookAt::LookAtClear()
{
   mLookAtEntityPtr = nullptr;
   mCallbacksLookAt.Clear();
   mDirty = true;
}

bool VaCameraMotionLookAt::MouseScroll(int aDirection)
{
   bool handled = false;
   return handled;
}

bool VaCameraMotionLookAt::MouseMotion(int aX, int aY, unsigned int aState)
{
   bool handled = false;
   mDirty |= handled;
   return handled;
}

bool VaCameraMotionLookAt::PassiveMouseMotion(int aX, int aY, unsigned int aState)
{
   bool handled = false;
   return handled;
}

bool VaCameraMotionLookAt::MouseButtonPress(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   bool handled = false;
   mDirty |= handled;
   return handled;
}

bool VaCameraMotionLookAt::MouseButtonRelease(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   bool handled = false;
   mDirty |= handled;
   return handled;
}

bool VaCameraMotionLookAt::MouseButtonDoublePress(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   bool handled = false;
   mDirty |= handled;
   return handled;
}

bool VaCameraMotionLookAt::GesturePinch(double aScale, double aAngle, int aCenterX, int aCenterY)
{
   bool handled = false;

   mDirty |= handled;
   return handled;
}

void VaCameraMotionLookAt::BuildViewMatrix(const VaPosition& aRefPosition)
{
   double srcXYZ[3];
   aRefPosition.GetSceneXYZ(srcXYZ, 0);
   osg::Vec3d src(srcXYZ[0], srcXYZ[1], srcXYZ[2]);

   double dstXYZ[3];
   mLookAtPosition.GetSceneXYZ(dstXYZ, 0);
   osg::Vec3d dst(dstXYZ[0], dstXYZ[1], dstXYZ[2]);

   osg::Vec3d up(src);
   up.normalize();

   osg::Vec3d eye = GetEyePosition(aRefPosition, mAz, mEl, mDist);

   // Calculate the camera's current geodetic position.
   double eyeLat, eyeLon, eyeAlt;
   UtEllipsoidalEarth::ConvertECEFToLLA(eye.ptr(), eyeLat, eyeLon, eyeAlt);
   mLLA.SetLLA(eyeLat, eyeLon, eyeAlt);

   if (mDebug)
   {
      auto out = ut::log::debug() << "CameraMotionLookAt::BuildViewMatrix:";
      out.AddNote() << "SRC LLA: " << aRefPosition.GetLat() << ", " << aRefPosition.GetLon() << ", "
                    << aRefPosition.GetAlt();
      out.AddNote() << "EYE LLA: " << mLLA.GetLat() << ", " << mLLA.GetLon() << ", " << mLLA.GetAlt();
      out.AddNote() << "DST LLA: " << mLookAtPosition.GetLat() << ", " << mLookAtPosition.GetLon() << ", "
                    << mLookAtPosition.GetAlt();
      out.AddNote() << "Azimuth: " << mAz;
      out.AddNote() << "Elevation: " << mEl;
      out.AddNote() << "Distance: " << mDist;
   }

   // Compute the view-matrix using OSG's look-at utility.
   mViewMatrix.makeLookAt(eye, dst, up);
   VaObserver::ViewMatrixChanged(GetCamera()->GetViewer());
}

// virtual
double VaCameraMotionLookAt::GetGeographicHeading()
{
   double heading;
   double dist;
   UtSphericalEarth::GreatCircleHeadingAndDistance(mLLA.GetLat(),
                                                   mLLA.GetLon(),
                                                   mRefPosition.GetLat(),
                                                   mRefPosition.GetLon(),
                                                   heading,
                                                   dist);

   return -heading;
}
} // end namespace vespa
