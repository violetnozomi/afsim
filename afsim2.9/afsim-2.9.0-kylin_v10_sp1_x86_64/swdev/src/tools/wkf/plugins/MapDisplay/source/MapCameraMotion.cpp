// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "MapCameraMotion.hpp"

#include <osg/Vec3>

#include "UtMath.hpp"
#include "VaCameraBase.hpp"
#include "VaEntity.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"

constexpr double Map::CameraMotion::cMinFollowRange;

Map::CameraMotion::CameraMotion(vespa::VaCameraBase* aCameraPtr)
   : VaCameraMotionGeocentric(aCameraPtr)
{
}

bool Map::CameraMotion::MouseScroll(int aDirection)
{
   if (mFollowCallback)
   {
      double deltaY = (aDirection * 0.0005);
      // allow the camera to come within cMinFollowRange m of the follow target
      SetDist(std::max(mDist * exp(deltaY * 5), cMinFollowRange));
      return true;
   }
   else if (!mZoomAtCursor)
   {
      vespa::VaPosition p      = GetReferencePosition();
      double            deltaY = (aDirection * 0.0005);
      p.SetAlt(mDist * exp(deltaY * 5));
      double ypr[3];
      GetYawPitchRoll(ypr[0], ypr[1], ypr[2]);
      AnimateToPosition(p, ypr[0], ypr[1], 0.25);
      return true;
   }
   else
   {
      return vespa::VaCameraMotionGeocentric::MouseScroll(aDirection);
   }
}

bool Map::CameraMotion::MouseMotion(int aX, int aY, unsigned int aState)
{
   if ((aState == 0) && (LeftDown()))
   {
      InteruptFollow();
   }
   return vespa::VaCameraMotionGeocentric::MouseMotion(aX, aY, aState);
}

bool Map::CameraMotion::MouseButtonDoublePress(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   InteruptFollow();
   return vespa::VaCameraMotionGeocentric::MouseButtonDoublePress(aX, aY, aButton, aState);
}

bool Map::CameraMotion::GesturePinch(double aScale, double aAngle, int aCenterX, int aCenterY)
{
   InteruptFollow();
   return vespa::VaCameraMotionGeocentric::GesturePinch(aScale, aAngle, aCenterX, aCenterY);
}

void Map::CameraMotion::InteruptFollow()
{
   if (mFollowCallback)
   {
      mDist = GetDist(); // add the entity distance back in to approximate the current location
      mPCallbacks.Disconnect(mFollowCallback);
      mFollowCallback = 0;
   }
}

void Map::CameraMotion::Follow(vespa::VaEntity* aEntityPtr)
{
   if (mFollowCallback)
   {
      InteruptFollow();
   }
   EntityMovedHandler(aEntityPtr, false);
   mFollowCallback = mPCallbacks.Add(
      vespa::VaObserver::EntityMoved.Connect(&CameraMotion::EntityMovedHandler, this, aEntityPtr->GetUniqueId()));
}

void Map::CameraMotion::AnimateToPosition(const vespa::VaPosition& aPosition, double aYaw, double aPitch, double aTime)
{
   InteruptFollow();
   vespa::VaCameraMotionGeocentric::AnimateToPosition(aPosition, aYaw, aPitch, aTime);
}

bool Map::CameraMotion::RecommendNearFar(double& aNear, double& aFar)
{
   if (mFollowCallback)
   {
      // try to keep the followed platform in view
      aNear = std::min(0.1 * mDist + mEntityAlt, 0.9 * (mDist));
      //      aNear = 0.1 * mDist;
      double geosync = 42000000.0;
      aFar           = mDist * 2.0 + geosync;
      return true;
   }
   else
   {
      return VaCameraMotionGeocentric::RecommendNearFar(aNear, aFar);
   }
}

double Map::CameraMotion::GetDist() const
{
   return mFollowCallback ? mDist + mEntityAlt : mDist;
}

void Map::CameraMotion::EntityMovedHandler(vespa::VaEntity* aEntityPtr, bool aMoving)
{
   GetCamera()->GetViewer()->RequestUpdate();
   // we won't let it drop under the platform while following
   // allowing the camera to come within cMinFollowRange m of the follow target
   double d = std::max(mDist, cMinFollowRange);
   SetReferencePosition(vespa::VaPosition(aEntityPtr->GetPosition().GetLat(), aEntityPtr->GetPosition().GetLon(), d));
   mEntityAlt = aEntityPtr->GetPosition().GetAlt();
}

void Map::CameraMotion::BuildViewMatrix()
{
   if (mFollowCallback)
   {
      UtoMapProjection mapProjection = GetCamera()->GetViewer()->GetMapProjection();
      if (mapProjection.projection == UtoMapProjection::Equirectangular)
      {
         double dist = mDist + mEntityAlt;

         double newAz = WrapAngleNeg180To180(GetAz() - mapProjection.longitudeOffset);
         double xyz[3] = {-newAz * 111319.4908, -GetEl() * 111319.4908, -dist}; // constant is appx. meters per pixel at equator
         double yaw;
         double pitch;
         double roll;
         GetYawPitchRoll(yaw, pitch, roll);
         osg::Matrixd mat = osg::Matrixd::translate(xyz[0], xyz[1], 0) *
                            osg::Matrixd::rotate(osg::DegreesToRadians(yaw), 0.0, 0.0, 1.0) *
                            osg::Matrixd::rotate(osg::DegreesToRadians(pitch - 90.0), 1.0, 0.0, 0.0) *
                            osg::Matrixd::translate(0, 0, -dist);
         SetViewMatrix(mat);
      }
      else
      {
         // Build the view matrix.
         vespa::VaPosition posXYZ;
         LLAToWorld(GetEl(), GetAz(), mEntityAlt, posXYZ);

         double xyz[3];
         posXYZ.GetECEF(xyz);

         double earthRadius = sqrt(xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2]);
         double gcLat       = asin(xyz[2] / earthRadius) * UtMath::cDEG_PER_RAD;

         double yaw;
         double pitch;
         double roll;
         GetYawPitchRoll(yaw, pitch, roll);
         osg::Matrixd mat = osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(1, 0, 0)) *
                            osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(0, 1, 0)) *
                            osg::Matrixd::rotate(osg::DegreesToRadians(-GetAz()), 0.0, 1.0, 0.0) *
                            osg::Matrixd::rotate(osg::DegreesToRadians(gcLat), 1.0, 0.0, 0.0) *
                            osg::Matrixd::translate(0.0, 0.0, -earthRadius) *
                            osg::Matrixd::rotate(osg::DegreesToRadians(yaw), 0.0, 0.0, 1.0) *
                            osg::Matrixd::rotate(osg::DegreesToRadians(pitch - 90.0), 1.0, 0.0, 0.0) *
                            osg::Matrixd::translate(0.0, 0.0, -mDist);
         SetViewMatrix(mat);
      }

      osg::Vec3 camerapos = osg::Vec3(0, 0, 0) * osg::Matrix::inverse(GetViewMatrix());
      double    ecef[3]   = {camerapos[0], camerapos[1], camerapos[2]};
      double    lla[3];
      GetCamera()->ConvertXYZToLLA(ecef, lla[0], lla[1], lla[2]);
      SetLLA(lla[0], lla[1], lla[2]);
      vespa::VaObserver::ViewMatrixChanged(GetCamera()->GetViewer());
   }
   else
   {
      vespa::VaCameraMotionGeocentric::BuildViewMatrix();
   }
}
