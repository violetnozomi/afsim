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

#include "VaCameraBase.hpp"

#include <iostream>
#include <math.h>
#include <string.h>

#include <osg/Matrix>
#include <osg/Vec3>

#include "UtCast.hpp"
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

VaCameraBase::VaCameraBase(VaViewer* aViewerPtr)
   : mViewerPtr(aViewerPtr)
   , mHalfY(0.0)
   , mCameraProjection(cPERSPECTIVE_BIT | cMANAGED_BIT)
   , mCallbacks()
   , mCameraMotionPtr(nullptr)
   , mRadius(0.0)
   , mViewableAreaValid(false)
   , mMetersPerPixel(1.0)
   , mMetersPerDegreeLat(1.0)
   , mMetersPerDegreeLon(1.0)
   , mRefHeading(0.0)
   , mRefLat(0.0)
   , mRefLon(0.0)
   , mRefAlt(0.0)
   , mDistance(0.0)
   , mAzimuth(0.0)
   , mElevation(0.0)
   , mLat(0.0)
   , mLon(0.0)
   , mFovY(40.0)
   , mAspect(1.0)
   , mWidth(100)
   , mHeight(100)
{
   mCameraMotionPtr = new VaCameraMotionGeocentric(this, false);

   mRefXYZ[0]  = 0.0;
   mRefXYZ[1]  = 0.0;
   mRefXYZ[2]  = 0.0;
   mRefHeading = 0.0;

   // Determine radius of Earth at the equator
   double xyz[3];
   UtEllipsoidalEarth::ConvertLLAToECEF(0.0, 0.0, 0.0, xyz);
   mRadius = UtVec3d::Magnitude(xyz);

   mLookVector[0] = 0.0;
   mLookVector[1] = 0.0;
   mLookVector[2] = 1.0;

   mCallbacks.Add(VaObserver::CameraSetLatLon.Connect(&VaCameraBase::CameraSetPositionLatLonCB, this));
   mCallbacks.Add(VaObserver::CameraSetAzElDist.Connect(&VaCameraBase::CameraSetPositionAzElDistCB, this));
   mCallbacks.Add(
      VaObserver::CameraSetProjection.Connect(&VaCameraBase::CameraSetProjectionCB, this, aViewerPtr->GetUniqueId()));
   mCallbacks.Add(VaObserver::CameraSetYawPitchDist.Connect(&VaCameraBase::CameraSetYawPitchDistCB, this));
}

// virtual
VaCameraBase::~VaCameraBase()
{
   delete mCameraMotionPtr;
   mCallbacks.Clear();
}

void VaCameraBase::SetCameraMotion(VaCameraMotion* aCameraMotion)
{
   if (!aCameraMotion)
   {
      return;
   }

   if (mCameraMotionPtr)
   {
      delete mCameraMotionPtr;
   }
   mCameraMotionPtr = aCameraMotion;
}

// protected
// virtual
void VaCameraBase::CameraChanged()
{
   mViewableAreaValid = false;

   // Update the reference point...
   mRefXYZ[0] = 0.0;
   mRefXYZ[1] = 0.0;
   mRefXYZ[2] = 0.0;
   if ((mRefLat != 0.0) || (mRefLon != 0.0) || (mRefAlt != 0.0))
   {
      double xyz[3];
      ConvertLLAToXYZ(xyz, mRefLat, mRefLon, mRefAlt);
      mRefXYZ[0] = xyz[0];
      mRefXYZ[1] = xyz[1];
      mRefXYZ[2] = xyz[2];
   }
}

// private
int VaCameraBase::ComputeViewableArea(GeoRect& aRect) const
{
   double latMin;
   double latMax;
   double lonMin;
   double lonMax;
   // (x0, y0) is the location of the viewer
   double x0;
   double y0;

   // Determine the limits of view in the latitudinal direction with
   // consideration for the elevation angle.

   VaPosition lla(0.0, 0.0, 0.0);
   double     heading = 0.0;
   lla                = mCameraMotionPtr->GetReferencePosition();
   heading            = mCameraMotionPtr->GetGeographicHeading();

   double distance = mDistance;
   double alpha    = mElevation;
   if (mCameraMotionPtr->IsWeird())
   {
      alpha = 90 - mLat;

      double h = mDistance * cos(mLat * UtMath::cRAD_PER_DEG);
      double v = mDistance * sin(mLat * UtMath::cRAD_PER_DEG) + lla.GetAlt();
      distance = sqrt(h * h + v * v); // distance to position of target projected down to the surface
   }
   double sinAlpha = sin(alpha * UtMath::cRAD_PER_DEG);
   double cosAlpha = cos(alpha * UtMath::cRAD_PER_DEG);
   double halfFovY = 0.5 * mFovY;

   if (GetUsePerspectiveProjection()) // this may be unreliable if using an unmanaged projection!
   {
      x0 = -distance * cosAlpha;
      y0 = -distance * sinAlpha;

      if (!mCameraMotionPtr->IsWeird())
      {
         latMax = ComputeVisibleAngle(x0, y0, alpha + halfFovY);
         latMin = ComputeVisibleAngle(x0, y0, alpha - halfFovY);
      }
      else
      {
         x0 = -x0 + lla.GetAlt();

         latMax = ComputeVisibleAngleTethered(x0, alpha + halfFovY + 90 - mElevation);
         latMin = ComputeVisibleAngleTethered(x0, alpha - halfFovY + 90 - mElevation);
      }
      latMin = -latMin;

      // Determine the limits of view in the longitudinal direction.  This
      // will be symmetrical.
      double tanPhi  = mAspect * tan(halfFovY * UtMath::cRAD_PER_DEG);
      double phi     = atan(tanPhi);
      double temp    = (mRadius + distance) / mRadius;
      double sinBeta = sin(phi) * temp;
      if (sinBeta >= 1.0)
      {
         // The whole earth is visible; use the grazing angle.

         lonMax = (0.5 * UtMath::cPI) - asin(1.0 / temp);
      }
      else
      {
         // The value returned by 'asin()' is always in the range [-pi/2, pi/2]
         // and in our case is always in the first quadrant  The angle we are
         // solving for is always obtuse so we want the value in the second
         // quadrant.

         double beta = UtMath::cPI - asin(sinBeta);
         lonMax      = UtMath::cPI - beta - phi;
      }
      lonMax *= UtMath::cDEG_PER_RAD;

      // Compensate for perspective when not looking straight down...
      if (!mCameraMotionPtr->IsWeird())
      {
         temp = latMax * tanPhi * cos(mElevation * UtMath::cRAD_PER_DEG);
      }
      else
      {
         temp = latMax * tanPhi * cos((mElevation + mLat - 90) * UtMath::cRAD_PER_DEG);
      }
      lonMax += temp;
   }
   else // Orthographic projection
   {
      double halfY = distance * tan(halfFovY * UtMath::cRAD_PER_DEG);
      double x     = -distance;
      double y     = halfY;
      x0           = x * cosAlpha - y * sinAlpha;
      y0           = x * sinAlpha + y * cosAlpha;
      latMax       = ComputeVisibleAngle(x0, y0, alpha);
      y            = -halfY;
      x0           = x * cosAlpha - y * sinAlpha;
      y0           = x * sinAlpha + y * cosAlpha;
      latMin       = ComputeVisibleAngle(x0, y0, alpha);
      latMin       = -latMin;

      double halfX = mAspect * halfY;
      if (halfX > mRadius)
      {
         lonMax = 0.5 * UtMath::cPI;
      }
      else
      {
         lonMax = asin(halfX / mRadius);
      }
      lonMax *= UtMath::cDEG_PER_RAD;
   }
   lonMin = -lonMax;

   // Convert the limits to meters.

   double metersPerDegree = 2.0 * UtMath::cPI * mRadius / 360.0;
   double ymin            = latMin * metersPerDegree;
   double ymax            = latMax * metersPerDegree;
   double xmin            = lonMin * metersPerDegree;
   double xmax            = lonMax * metersPerDegree;

   // Rotate the limits by the azimuth angle.

   double theta;
   if (!mCameraMotionPtr->IsWeird())
   {
      theta = (360.0 - mAzimuth) * UtMath::cRAD_PER_DEG;
   }
   else
   {
      theta = (360.0 - (mAzimuth + mLon + heading)) * UtMath::cRAD_PER_DEG;
   }
   double cosTheta = cos(theta);
   double sinTheta = sin(theta);

   double xyll[] = {xmin * cosTheta - ymin * sinTheta, xmin * sinTheta + ymin * cosTheta};
   double xylr[] = {xmax * cosTheta - ymin * sinTheta, xmax * sinTheta + ymin * cosTheta};
   double xyul[] = {xmin * cosTheta - ymax * sinTheta, xmin * sinTheta + ymax * cosTheta};
   double xyur[] = {xmax * cosTheta - ymax * sinTheta, xmax * sinTheta + ymax * cosTheta};
   xmin          = xyll[0];
   xmax          = xyll[0];
   if (xylr[0] < xmin)
      xmin = xylr[0];
   if (xylr[0] > xmax)
      xmax = xylr[0];
   if (xyul[0] < xmin)
      xmin = xyul[0];
   if (xyul[0] > xmax)
      xmax = xyul[0];
   if (xyur[0] < xmin)
      xmin = xyur[0];
   if (xyur[0] > xmax)
      xmax = xyur[0];

   ymin = xyll[1];
   ymax = xyll[1];
   if (xylr[1] < ymin)
      ymin = xylr[1];
   if (xylr[1] > ymax)
      ymax = xylr[1];
   if (xyul[1] < ymin)
      ymin = xyul[1];
   if (xyul[1] > ymax)
      ymax = xyul[1];
   if (xyur[1] < ymin)
      ymin = xyur[1];
   if (xyur[1] > ymax)
      ymax = xyur[1];

   // Convert the meters to degrees and compute the min/max lat/lon.

   double minLat;
   double maxLat;
   double minLon;
   double maxLon;

   if (!mCameraMotionPtr->IsWeird())
   {
      minLat = mLat + ymin / mMetersPerDegreeLat;
      maxLat = mLat + ymax / mMetersPerDegreeLat;
      minLon = mLon + xmin / mMetersPerDegreeLon;
      maxLon = mLon + xmax / mMetersPerDegreeLon;
   }
   else
   {
      // don't use mMetersPerDegreeX because it is invalid while tethered.
      double mpdLat = UtEllipsoidalEarth::MetersPerDegreeLat(lla.GetLat());
      double mpdLon = UtEllipsoidalEarth::MetersPerDegreeLon(lla.GetLat());
      minLat        = lla.GetLat() + ymin / mpdLat;
      maxLat        = lla.GetLat() + ymax / mpdLat;
      minLon        = lla.GetLon() + xmin / mpdLon;
      maxLon        = lla.GetLon() + xmax / mpdLon;
   }

   if (maxLat >= 90.0)
   {
      maxLat = 90.0;
      minLon = -180.0;
      maxLon = 180.0;
   }
   if (minLat <= -90.0)
   {
      minLat = -90.0;
      minLon = -180.0;
      maxLon = 180.0;
   }
   aRect.SetSWLat(minLat);
   aRect.SetSWLon(minLon);
   aRect.SetNELat(maxLat);
   aRect.SetNELon(maxLon);
   aRect.Normalize();

   return 0;
}

// Determine the latitude of the intersection between the earth and the
// line with the specified angle that originates at (X0, Y0).  If the
// line does not intersect then the grazing angle is returned.

// private
double VaCameraBase::ComputeVisibleAngle(double aX0, double aY0, double aAngle) const
{
   double m = aAngle;
   if (fabs(aAngle) < 1.0E-4)
   {
      m = 1.0E-4;
   }
   if (fabs(aAngle) > 90 - 1.0E-4)
   {
      m = 90 - 1.0E-4;
   }
   m = tan(m * UtMath::cRAD_PER_DEG);

   double theta = 0.0;

   // Determine the Y intercept.

   double x0 = aX0;
   double y0 = aY0;
   double b  = y0 - (m * x0);

   // Compute the coefficients of the quadratic equation.

   double A = 1.0 + (m * m);
   double B = 2.0 * (m * b - mRadius);
   double C = b * b;

   // A negative discriminant indicates the line does not intersect the
   // circle (i.e.: the horizon is in view).  In that case we return the
   // grazing angle...

   double desc = (B * B) - (4.0 * A * C);
   if (desc >= 0.0)
   {
      double sdesc = sqrt(desc);
      double x1    = (-B - sdesc) / (2.0 * A);
      // double x2 = (-B + sdesc) / (2.0 * A);

      double cos1 = 1.0 - (x1 / mRadius);
      // double cos2 = 1.0 - (x2 / mRadius);
      cos1      = std::max(std::min(cos1, 1.0), -1.0);
      double a1 = acos(cos1) * UtMath::cDEG_PER_RAD;
      // double a2 = acos(cos2) * UtMath::cDEG_PER_RAD;

      theta = a1;
   }
   else
   {
      // Compute the slope of the grazing angle
      double xp       = mRadius - x0;
      double yp       = -y0;
      double h        = sqrt(xp * xp + yp * yp);
      double sinGamma = std::min(mRadius / h, 1.0);
      double gamma    = asin(sinGamma) * UtMath::cDEG_PER_RAD;
      double mu       = atan2(yp, xp) * UtMath::cDEG_PER_RAD;
      theta           = 90.0 - gamma - mu;
      if (theta < 0.0)
      {
         theta += 90.0;
      }
   }
   return theta;
}

// this could actually replace the other method for non-tethered angles, I actually think it is more accurate.
// Unfortunately, it accurately draws a lot more than the old method, so we'll keep the old one to maintain performance
// when untethered.

double VaCameraBase::ComputeVisibleAngleTethered(double aY0, double aAngle) const
{
   // our origin is the center of the earth
   double m = aAngle;
   if (fabs(aAngle) < 1.0E-4)
   {
      m = 1.0E-4;
   }
   if (fabs(aAngle) > 90 - 1.0E-4)
   {
      m = 90 - 1.0E-4;
   }
   m        = 1 / tan(m * UtMath::cRAD_PER_DEG);
   double b = aY0 + mRadius; // y = mx + b is a line at aAngle originating aY0 + mRadius units off the earth

   double A = m * m + 1;
   double B = 2 * m * b;
   double C =
      b * b -
      mRadius * mRadius; // this quadratic describes the intersection between the line and a circle of earth's radius

   double desc = (B * B) - (4.0 * A * C);
   if (desc >= 0.0) // there is an intersection
   {
      double sdesc = sqrt(desc);
      double x1    = (-B + sdesc) / (2.0 * A);
      double x2    = (-B - sdesc) / (2.0 * A);
      if (fabs(x2) < fabs(x1))
      {
         x1 = x2; // get the closest intersection
      }
      double y1    = m * x1 + b - mRadius;
      double d     = sqrt(x1 * x1 + y1 * y1);                            // distance between observer and intersection
      double angle = 2 * asin(0.5 * d / mRadius) * UtMath::cDEG_PER_RAD; // angular distance

      return angle;
   }
   else // return the grazing angle
   {
      return acos(mRadius / (mRadius + aY0)) * UtMath::cDEG_PER_RAD; // this is the grazing angle to the earth
   }
}

// virtual
void VaCameraBase::ConvertLLAToXYZ(double aXYZ[3], double aLat, double aLon, double aAlt)
{
   VaPosition temp(aLat, aLon, aAlt);
   temp.GetSceneXYZ(aXYZ, mViewerPtr->GetUniqueId());
}

// virtual
void VaCameraBase::ConvertXYZToLLA(const double aXYZ[3], double& aLat, double& aLon, double& aAlt) const
{
   VaPosition temp = VaPosition::GetPositionOfSceneXYZ(aXYZ, mViewerPtr->GetUniqueId());
   aLat            = temp.GetLat();
   aLon            = temp.GetLon();
   aAlt            = temp.GetAlt();
}

//#define DEBUG_CONVERT 1

// virtual
bool VaCameraBase::ConvertScreenToLLA(int aX, int aY, double& aLat, double& aLon, double& aAlt)
{
   return ConstConvertScreenToLLA(aX, aY, aLat, aLon, aAlt);
}

// virtual
bool VaCameraBase::ConstConvertScreenToLLA(int aX, int aY, double& aLat, double& aLon, double& aAlt) const
{
   VaPosition nearPosition;
   VaPosition farPosition;
   ScreenToWorld(aX, aY, 0.0, nearPosition); // where is the mouse cursor on the near plane?
   ScreenToWorld(aX, aY, 1.0, farPosition);  // where is the mouse cursor on the far plane?

   if (mViewerPtr->GetMapProjection().projection == UtoMapProjection::Equirectangular)
   {
      if (nearPosition.GetAlt() != farPosition.GetAlt())
      {
         double invAltDiff = 1 / (nearPosition.GetAlt() - farPosition.GetAlt());
         aLat =
            nearPosition.GetLat() - (nearPosition.GetLat() - farPosition.GetLat()) * invAltDiff * nearPosition.GetAlt();
         if (aLat > 90)
         {
            aLat = 90;
         }
         if (aLat < -90)
         {
            aLat = -90;
         }
         aLon =
            nearPosition.GetLon() - (nearPosition.GetLon() - farPosition.GetLon()) * invAltDiff * nearPosition.GetAlt();
         aAlt = 0;
      }
      else
      {
         aLat = nearPosition.GetLat();
         aLon = nearPosition.GetLon();
         aAlt = 0;
      }
   }
   else
   {
      double nearXYZ[3];
      double farXYZ[3];

      nearPosition.GetECEF(nearXYZ);
      farPosition.GetECEF(farXYZ);

      nearXYZ[0] /= UtEarth::cA;
      nearXYZ[1] /= UtEarth::cA;
      nearXYZ[2] /= UtEarth::cB;
      farXYZ[0] /= UtEarth::cA;
      farXYZ[1] /= UtEarth::cA;
      farXYZ[2] /= UtEarth::cB; // convert positions into unit sphere coordinates

      double delta[3] = {farXYZ[0] - nearXYZ[0], farXYZ[1] - nearXYZ[1], farXYZ[2] - nearXYZ[2]};

      // our line segment is winPosNear + delta * t
      // our sphere is centered at 0 with a 1 unit radius

      double A = delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2];
      double B = 2 * nearXYZ[0] * delta[0] + 2 * nearXYZ[1] * delta[1] + 2 * nearXYZ[2] * delta[2];
      double C = nearXYZ[0] * nearXYZ[0] + nearXYZ[1] * nearXYZ[1] + nearXYZ[2] * nearXYZ[2] - 1;

      double  test = B * B - 4 * A * C;
      double  xyz1[3];
      double  xyz2[3];
      double* xyz;
      if (test < 0) // there is no intersection for the current vector find the horizon!
      {
         double lenE  = sqrt(C + 1);
         double lenF  = sqrt(A);
         double l[3]  = {delta[0] / lenF, delta[1] / lenF, delta[2] / lenF};          // vector towards cursor
         double e[3]  = {-nearXYZ[0] / lenE, -nearXYZ[1] / lenE, -nearXYZ[2] / lenE}; // vector towards earths center
         double angEF = acos(l[0] * e[0] + l[1] * e[1] + l[2] * e[2]);                // angle earth->near->far
         double angH  = asin(1 / lenE);                                               // angle earth->near->horizon
         double t     = 1 - angH / angEF;
         double m[3]  = {t * e[0] + (1 - t) * l[0],
                        t * e[1] + (1 - t) * l[1],
                        t * e[2] + (1 - t) * l[2]}; // this vector should be on the horizon towards the cursor

         double t2 = (-nearXYZ[0] * m[0] - nearXYZ[1] * m[1] - nearXYZ[2] * m[2]) /
                     (m[0] * m[0] + m[1] * m[1] + m[2] * m[2]); // distance along vector to point nearest the unit sphere
         xyz1[0] = nearXYZ[0] + t2 * m[0];                      // unit sphere xyz nearest the sphere
         xyz1[1] = nearXYZ[1] + t2 * m[1];
         xyz1[2] = nearXYZ[2] + t2 * m[2];
         xyz     = xyz1;
      }
      else
      {
         double t1  = (-B + sqrt(test)) / (2 * A); // distance to first intersection
         double t2  = (-B - sqrt(test)) / (2 * A); // distance to second intersection
         xyz1[0]    = nearXYZ[0] + delta[0] * t1;
         xyz1[1]    = nearXYZ[1] + delta[1] * t1;
         xyz1[2]    = nearXYZ[2] + delta[2] * t1; // first intersection
         xyz2[0]    = nearXYZ[0] + delta[0] * t2;
         xyz2[1]    = nearXYZ[1] + delta[1] * t2;
         xyz2[2]    = nearXYZ[2] + delta[2] * t2; // second intersection
         xyz        = xyz2;
         double d21 = (nearXYZ[0] - xyz1[0]) * (nearXYZ[0] - xyz1[0]) + (nearXYZ[1] - xyz1[1]) * (nearXYZ[1] - xyz1[1]) +
                      (nearXYZ[2] - xyz1[2]) * (nearXYZ[2] - xyz1[2]); // dist squared from hit 1 to near
         double d22 = (nearXYZ[0] - xyz2[0]) * (nearXYZ[0] - xyz2[0]) + (nearXYZ[1] - xyz2[1]) * (nearXYZ[1] - xyz2[1]) +
                      (nearXYZ[2] - xyz2[2]) * (nearXYZ[2] - xyz2[2]); // dist squared from hit 2 to near
         if (d21 <= d22)                                               // pick the closer one
         {
            xyz = xyz1;
         }
      }
      xyz[0] *= UtEarth::cA;
      xyz[1] *= UtEarth::cA;
      xyz[2] *= UtEarth::cB;
      UtEllipsoidalEarth::ConvertECEFToLLA(xyz, aLat, aLon, aAlt);
      aAlt = 0.0;
   }

   return true;
}

// virtual
int VaCameraBase::GetViewableArea(GeoRect& aRect)
{
   if (!mViewableAreaValid)
   {
      ComputeViewableArea(mViewableArea);
      mViewableAreaValid = true;
   }
   aRect = mViewableArea;
   return 0;
}

// virtual
bool VaCameraBase::IsFlat() const
{
   if (mViewerPtr->GetMapProjection().projection == UtoMapProjection::Equirectangular)
   {
      return true;
   }
   else
   {
      return false;
   }
}

// virtual
void VaCameraBase::LoadMatrices(bool)
{
#if 0
   auto out = ut::log::info() << "Location:";
   out.AddNote() << "Lat: " << mLat;
   out.AddNote() << "Lon: " << mLon;
   out.AddNote() << "Distance: " << mDistance;
   out.AddNote() << "Azimuth: " << mAzimuth;
   out.AddNote() << "Elevation: " << mElevation;
#endif

   bool tethered = (mRefLat != 0.0) || (mRefLon != 0.0) || (mRefAlt != 0.0);

   CalculateMetersPerPixel();

   // Compute the geocentric latitude and effective radius

   double xyz[3];
   ConvertLLAToXYZ(xyz, mLat, 0.0, 0.0);
   double earthRadius = sqrt(xyz[0] * xyz[0] + xyz[2] * xyz[2]);
   double gcLat       = asin(xyz[2] / earthRadius) * UtMath::cDEG_PER_RAD;

   osg::Matrix vm;
   if (tethered)
   {
      // Build the model-view matrix.
      vm = osg::Matrixd(osg::Matrixd::translate(-mRefXYZ[0], -mRefXYZ[1], -mRefXYZ[2]) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(1, 0, 0)) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(0, 1, 0)) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(-mRefLon), 0.0, 1.0, 0.0) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(mRefLat), 1.0, 0.0, 0.0) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(mLon + mRefHeading), 0.0, 0.0, 1.0) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(mLat - 90.0), 1.0, 0.0, 0.0) *
                        osg::Matrixd::translate(0.0, 0.0, -mDistance) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(-mLat + 90.0), 1.0, 0.0, 0.0) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(mAzimuth), 0.0, 0.0, 1.0) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(mElevation - 90.0), 1.0, 0.0, 0.0) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(mLat - 90.0), 1.0, 0.0, 0.0));
   }
   else
   {
      // Build the model-view matrix.
      vm = osg::Matrixd(osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(1, 0, 0)) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(-90.0), osg::Vec3d(0, 1, 0)) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(-mLon), 0.0, 1.0, 0.0) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(gcLat), 1.0, 0.0, 0.0) *
                        osg::Matrixd::translate(0.0, 0.0, -earthRadius) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(mAzimuth), 0.0, 0.0, 1.0) *
                        osg::Matrixd::rotate(osg::DegreesToRadians(mElevation - 90.0), 1.0, 0.0, 0.0) *
                        osg::Matrixd::translate(0.0, 0.0, -mDistance));
   }

   mLookVector[0] = vm(0, 2);
   mLookVector[1] = vm(1, 2);
   mLookVector[2] = vm(2, 2);

   if (mCameraProjection & cPERSPECTIVE_BIT)
   {
      // Set the near plane distance.  When close to the surface we set it
      // right in front of the eye.

      double zNear = 0.1 * mDistance;
      double zFar  = 2.0 * mDistance + 2.0 * earthRadius;
      mCameraMotionPtr->RecommendNearFar(zNear, zFar);
      double xyz[3] = {0, 0, 0};

      mViewerPtr->SetPerspectiveCamera(vm, xyz, mFovY, zNear, zFar, (mCameraProjection & cMANAGED_BIT) ? true : false);
   }
   else
   {
      // Set the near plane distance.  When close to the surface we set it
      // right in front of the eye.

      double zNear = 0.1 * mDistance;
      if (mDistance < 100000.0)
      {
         zNear = 0.0;
      }
      if (mElevation < (0.5 * mFovY))
      {
         double elevation = mElevation;
         if (elevation == 0.0)
         {
            elevation = 0.1;
         }
         zNear = -mHalfY / sin(elevation * UtMath::cRAD_PER_DEG);
      }
      double zFar = 2.0 * mDistance + 2.0 * earthRadius;

      mViewerPtr->SetOrthographicCamera(vm, mDistance / mHeight, zNear, zFar, (mCameraProjection & cMANAGED_BIT) ? true : false);
   }
}

void VaCameraBase::LoadViewMatrixAndCameraPosition(const osg::Matrix& aViewMatrix, const double aXYZ[3])
{
   CalculateMetersPerPixel();

   // Compute the geocentric latitude and effective radius
   double xyz[3];
   ConvertLLAToXYZ(xyz, mLat, 0.0, 0.0);
   double earthRadius = sqrt(xyz[0] * xyz[0] + xyz[2] * xyz[2]);

   mLookVector[0] = aViewMatrix(0, 2);
   mLookVector[1] = aViewMatrix(1, 2);
   mLookVector[2] = aViewMatrix(2, 2);

   if (mCameraProjection & cPERSPECTIVE_BIT)
   {
      // Set the near plane distance.  When close to the surface we set it
      // right in front of the eye.

      double zNear  = 0.1 * mDistance;
      double zFar   = 2.0 * mDistance + 2.0 * earthRadius;
      double xyz[3] = {aXYZ[0], aXYZ[1], aXYZ[2]};
      mCameraMotionPtr->RecommendNearFar(zNear, zFar);

      mViewerPtr->SetPerspectiveCamera(aViewMatrix, xyz, mFovY, zNear, zFar, (mCameraProjection & cMANAGED_BIT) ? true : false);
   }
   else
   {
      // Set the near plane distance.  When close to the surface we set it
      // right in front of the eye.

      double zNear = 0.1 * mDistance;
      if (mDistance < 100000.0)
      {
         zNear = 0.0;
      }
      if (mPitch < (0.5 * mFovY))
      {
         double elevation = mPitch;
         if (elevation == 0.0)
         {
            elevation = 0.1;
         }
         zNear = -mHalfY / sin(elevation * UtMath::cRAD_PER_DEG);
      }
      double zFar = 2.0 * mDistance + 2.0 * earthRadius;
      mViewerPtr->SetOrthographicCamera(aViewMatrix,
                                        mDistance / mWidth,
                                        zNear,
                                        zFar,
                                        (mCameraProjection & cMANAGED_BIT) ? true : false);
   }
}

// virtual
void VaCameraBase::SetEntityCoordinateSystem(double aLat, double aLon, double aAlt) {}

void VaCameraBase::CalculateMetersPerPixel()
{
   double tDistance = mDistance;
   if (mCameraMotionPtr->IsWeird())
   {
      VaPosition lla;
      lla = mCameraMotionPtr->GetPosition();

      double                  h1  = mDistance * cos(mLat * UtMath::cRAD_PER_DEG);
      double                  v1  = mDistance * sin(mLat * UtMath::cRAD_PER_DEG) + lla.GetAlt();
      double                  v2  = 0; // ALTITUDE of the platform;
      VaCameraMotionTethered* cmt = dynamic_cast<VaCameraMotionTethered*>(mCameraMotionPtr);
      if (nullptr != cmt)
      {
         v2 = cmt->GetEntity()->GetPosition().GetAlt();
      }
      double h2 = (h1 / v1) * v2; // should be proportional
      tDistance = sqrt((h1 + h2) * (h1 + h2) +
                       (v1 + v2) * (v1 + v2)); // distance to position of target projected down to the surface
   }
   mHalfY          = tDistance * tan(0.5 * mFovY * UtMath::cRAD_PER_DEG);
   mMetersPerPixel = 2.0 * mHalfY / mHeight;
}

void VaCameraBase::UseOrthographicProjection()
{
   mCameraProjection &= ~cPERSPECTIVE_BIT;
}

void VaCameraBase::UsePerspectiveProjection()
{
   mCameraProjection |= cPERSPECTIVE_BIT;
}

void VaCameraBase::ManageProjection(bool aState)
{
   if (aState)
   {
      mCameraProjection |= cMANAGED_BIT;
   }
   else
   {
      mCameraProjection &= ~cMANAGED_BIT;
   }
}

#if 0
// dead code
int VaCameraBase::PoleInScreen() const
{
   double lat;
   double lon;
   double alt;
   VaPosition north(90.0, 0.0, 0.0);
   VaPosition south(-90.0, 0.0, 0.0);

   unsigned int northX, northY, northZ;
   mViewerPtr->WorldToScreen(north, northX, northY, northZ);

   unsigned int southX, southY, southZ;
   mViewerPtr->WorldToScreen(south, southX, southY, southZ);

   if ((northX > 0) && (northX < mWidth) && (northY > 0) && (northY < mHeight))
   {
      ConstConvertScreenToLLA((int)(northX), (int)(northY), lat, lon, alt);
      if (lat > 85.0) // an arbitrary tolerance
      {
         return 1;
      }
      auto out = ut::log::info() << "North Fail.";
      out.AddNote() << "Lat: " << lat;
      out.AddNote() << "Lon: " << lon;
      out.AddNote() << "Alt: " << alt;
   }
   if ((southX > 0) && (southX < mWidth) && (southY > 0) && (southY < mHeight))
   {
      ConstConvertScreenToLLA((int)(southX), (int)(southY), lat, lon, alt);
      if (lat < -85.0) // an arbitrary tolerance
      {
         return -1;
      }
   }
   return 0;
}
#endif

// virtual
void VaCameraBase::GetLatLon(double& aLat, double& aLon) const
{
   if (mCameraMotionPtr->IsWeird())
   {
      aLat = 0.0;
      aLon = 0.0;
      if ((mWidth > 0) || (mHeight > 0))
      {
         double alt;
         ConstConvertScreenToLLA((int)(mWidth * 0.5), (int)(mHeight * 0.5), aLat, aLon, alt);
      }
   }
   else
   {
      aLat = mLat;
      aLon = mLon;
   }
}

// virtual
double VaCameraBase::GetMetersPerDegreeLat() const
{
   double metersPerDegreeLat = mMetersPerDegreeLat;
   if (mCameraMotionPtr->IsWeird())
   {
      VaPosition lla = mCameraMotionPtr->GetPosition();
      return UtEllipsoidalEarth::MetersPerDegreeLat(lla.GetLat());
   }
   return metersPerDegreeLat;
}

// virtual
double VaCameraBase::GetMetersPerDegreeLon() const
{
   double metersPerDegreeLon = mMetersPerDegreeLat;
   if (mCameraMotionPtr->IsWeird())
   {
      VaPosition lla = mCameraMotionPtr->GetPosition();
      return UtEllipsoidalEarth::MetersPerDegreeLon(lla.GetLat());
   }
   return metersPerDegreeLon;
}

// virtual
bool VaCameraBase::Update(double aTime, int aWidth, int aHeight)
{
   return mCameraMotionPtr->Update(aTime);
}

void VaCameraBase::UpdateView(int aWidth, int aHeight)
{
   VaPosition  camRefPos;
   double      cameraAz;
   double      cameraEl;
   double      cameraRoll;
   double      cameraPitch;
   double      cameraYaw;
   double      cameraDist;
   osg::Matrix cameraMatrix;
   double      xyz[3];

   camRefPos = mCameraMotionPtr->GetReferencePosition();
   mCameraMotionPtr->GetAzElDist(cameraAz, cameraEl, cameraDist);
   mCameraMotionPtr->GetYawPitchRoll(cameraYaw, cameraPitch, cameraRoll);
   mCameraMotionPtr->GetViewMatrixAndCameraPosition(cameraMatrix, xyz);
   mWidth              = aWidth;
   mHeight             = aHeight;
   mAspect             = static_cast<double>(aWidth) / static_cast<double>(aHeight);
   mLat                = camRefPos.GetLat();
   mLon                = camRefPos.GetLon();
   mMetersPerDegreeLat = UtEllipsoidalEarth::MetersPerDegreeLat(mLat);
   mMetersPerDegreeLon = UtEllipsoidalEarth::MetersPerDegreeLon(mLat);
   mAzimuth            = cameraAz;
   mElevation          = cameraEl;
   mDistance           = cameraDist;
   SetPitchYaw(cameraPitch, cameraYaw);
   CameraChanged();

   LoadViewMatrixAndCameraPosition(cameraMatrix, xyz);
}

// virtual
VaPosition VaCameraBase::GetCameraPosition()
{
   return mCameraMotionPtr->GetPosition();
}

// virtual
VaCameraMotion::MotionType VaCameraBase::GetMotionType()
{
   return mCameraMotionPtr->GetMotionType();
}

// virtual
bool VaCameraBase::GetAllowPicking() const
{
   return mCameraMotionPtr->GetAllowPicking();
}

// virtual
bool VaCameraBase::MouseButtonDoublePress(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   return mCameraMotionPtr->MouseButtonDoublePress(aX, aY, aButton, aState);
}

bool VaCameraBase::MouseButtonPress(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   return mCameraMotionPtr->MouseButtonPress(aX, aY, aButton, aState);
}

bool VaCameraBase::MouseButtonRelease(int aX, int aY, unsigned int aButton, unsigned int aState)
{
   return mCameraMotionPtr->MouseButtonRelease(aX, aY, aButton, aState);
}

bool VaCameraBase::MouseMotion(int aX, int aY, unsigned int aState)
{
   return mCameraMotionPtr->MouseMotion(aX, aY, aState);
}

bool VaCameraBase::MouseScroll(int aDirection)
{
   return mCameraMotionPtr->MouseScroll(aDirection);
}

bool VaCameraBase::GesturePinch(double aScale, double aAngle, int aCenterX, int aCenterY)
{
   return mCameraMotionPtr->GesturePinch(aScale, aAngle, aCenterX, aCenterY);
}

// private
bool VaCameraBase::ScreenToWorld(unsigned int aX, unsigned int aY, double aDepth, VaPosition& aPosition) const
{
   bool ok = false;
   if (mViewerPtr != nullptr)
   {
      UtoPosition pos(aX, aY, aDepth);
      ok = mViewerPtr->WindowToWorld(pos);
      if (ok)
      {
         double lat, lon, alt;
         ConvertXYZToLLA(pos.ptr(), lat, lon, alt);
         aPosition = VaPosition(lat, lon, alt, pos.x(), pos.y(), pos.z());
      }
   }
   return ok;
}

void VaCameraBase::GetAzElDist(double& aAzimuth, double& aElevation, double& aDistance) const
{
   aAzimuth   = mAzimuth;
   aElevation = mElevation;
   aDistance  = mDistance;
}

void VaCameraBase::GetViewport(unsigned int& aWidth, unsigned int& aHeight) const
{
   aWidth  = ut::safe_cast<unsigned int, int>(mWidth);
   aHeight = ut::safe_cast<unsigned int, int>(mHeight);
}


void VaCameraBase::CameraSetPositionLatLonCB(VaViewer* aViewerPtr, double aLat, double aLon)
{
   if ((aViewerPtr != nullptr) && (aViewerPtr->GetUniqueId() == mViewerPtr->GetUniqueId()))
   {
      if (mCameraMotionPtr != nullptr)
      {
         double az;
         double el;
         double dist;
         mCameraMotionPtr->GetAzElDist(az, el, dist);
         VaPosition temp(aLat, aLon, dist);
         mCameraMotionPtr->SetReferencePosition(temp);
         mViewerPtr->RequestUpdate();
      }
   }
}

void VaCameraBase::CameraSetPositionAzElDistCB(VaViewer* aViewerPtr, double aAz, double aEl, double aDist)
{
   if ((aViewerPtr != nullptr) && (aViewerPtr->GetUniqueId() == mViewerPtr->GetUniqueId()))
   {
      if (mCameraMotionPtr != nullptr)
      {
         mCameraMotionPtr->SetAzElDist(aAz, aEl, aDist);
         mViewerPtr->RequestUpdate();
      }
   }
}

void VaCameraBase::CameraSetYawPitchDistCB(VaViewer* aViewerPtr, double aYawDeg, double aPitchDeg, double aDist)
{
   if ((aViewerPtr != nullptr) && (aViewerPtr->GetUniqueId() == mViewerPtr->GetUniqueId()))
   {
      if (mCameraMotionPtr != nullptr)
      {
         mPitch = aPitchDeg;
         mYaw   = aYawDeg;
         mViewerPtr->RequestUpdate();
      }
   }
}

void VaCameraBase::CameraSetProjectionCB(int aProjection)
{
   bool doredraw = false;
   if (aProjection == 0)
   {
      doredraw = mViewerPtr->SetProjectionMode(VaViewer::PmPerspective);
      UsePerspectiveProjection();
   }
   else if (aProjection == 1)
   {
      doredraw = mViewerPtr->SetProjectionMode(VaViewer::PmOrthographic3D);
      UseOrthographicProjection();
   }
   if (doredraw)
   {
      mViewerPtr->RequestUpdate();
   }
}

// virtual
double VaCameraBase::GetGeographicHeading()
{
   if (mCameraMotionPtr != nullptr)
   {
      return mCameraMotionPtr->GetGeographicHeading();
   }
   return 0;
}

void VaCameraBase::ForceRefresh()
{
   if (mCameraMotionPtr != nullptr)
   {
      mCameraMotionPtr->ForceRefresh();
   }
}

void VaCameraBase::Reset()
{
   if (mCameraMotionPtr != nullptr)
   {
      mCameraMotionPtr->Reset();
   }
}

// bool VaCameraBase::GetProperty(int idx, UtScriptData& aProperty)
// {
//    switch (idx)
//    {
//    case 0: aProperty = mCameraMotionPtr->GetPosition().GetLat(); break;
//    case 1: aProperty = mCameraMotionPtr->GetPosition().GetLon(); break;
//    case 2: case 3: case 4: {
//       double az, el, dist;
//       mCameraMotionPtr->GetAzElDist(az, el, dist);
//       if (idx == 2) aProperty = az;
//       if (idx == 3) aProperty = el;
//       if (idx == 4) aProperty = dist;
//       break;
//    }
//    case 5: case 6: case 7: {
//       double yaw, pitch, roll;
//       mCameraMotionPtr->GetYawPitchRoll(yaw, pitch, roll);
//       if (idx == 5) aProperty = yaw;
//       if (idx == 6) aProperty = pitch;
//       if (idx == 7) aProperty = roll;
//       break;
//    }
//    case 8:
//       aProperty = mCameraMotionPtr->GetFovY();
//       break;
//    default:
//       return false;
//    }
//    return true;
// }
//
// void VaCameraBase::GetState(VaCameraState& aState)
// {
//    aState.lat = mCameraMotionPtr->GetPosition().GetLat();
//    aState.lon = mCameraMotionPtr->GetPosition().GetLon();
//    mCameraMotionPtr->GetAzElDist(aState.az, aState.el, aState.dist);
//    mCameraMotionPtr->GetYawPitchRoll(aState.yaw, aState.pitch, aState.roll);
//    aState.fovy = mCameraMotionPtr->GetFovY();
// }
//
// void VaCameraBase::SetState(const VaCameraState& aState)
// {
//    VaPosition pos = mCameraMotionPtr->GetPosition();
//    pos.SetLat(aState.lat);
//    pos.SetLon(aState.lon);
//    mCameraMotionPtr->SetAzElDist(aState.az, aState.el, aState.dist);
//    mCameraMotionPtr->SetYawPitchRoll(aState.yaw, aState.pitch, aState.roll);
//    mCameraMotionPtr->SetFovY(aState.fovy);
// }
//
// bool VaCameraBase::SetProperty(int idx, const UtScriptData& aProperty)
// {
//    switch (idx)
//    {
//    case 0: case 1:{
//               VaPosition pos = mCameraMotionPtr->GetPosition();
//               if (idx == 0) pos.SetLat(aProperty.GetDouble());
//               if (idx == 1) pos.SetLon(aProperty.GetDouble());
//               mCameraMotionPtr->SetPosition(pos);
//               break;
//    }
//    case 2: mCameraMotionPtr->SetAz(aProperty.GetDouble()); break;
//    case 3: mCameraMotionPtr->SetEl(aProperty.GetDouble()); break;
//    case 4: mCameraMotionPtr->SetDist(aProperty.GetDouble()); break;
//    case 6: case 7: case 8:
//    {
//       double yaw, pitch, roll;
//       mCameraMotionPtr->GetYawPitchRoll(yaw, pitch, roll);
//       if (idx == 5) yaw = aProperty.GetDouble();
//       if (idx == 6) pitch = aProperty.GetDouble();
//       if (idx == 7) roll = aProperty.GetDouble();
//       mCameraMotionPtr->SetYawPitchRoll(yaw, pitch, roll);
//       break;
//    }
//    case 8:
//       mCameraMotionPtr->SetFovY(aProperty.GetDouble());
//       break;
//    default:
//       return false;
//    }
//    return true;
// }

void VaCameraBase::SetPerspectiveCamera(const osg::Matrix& aViewMatrix, double aXYZ[3], double aFovY, double aNear, double aFar)
{
   mViewerPtr->SetPerspectiveCamera(aViewMatrix, aXYZ, aFovY, aNear, aFar, true);
}

void VaCameraBase::SetOrthographicCamera(const osg::Matrix& aViewMatrix, double aZoom, double aNear, double aFar)
{
   mViewerPtr->SetOrthographicCamera(aViewMatrix, aZoom, aNear, aFar, true);
}


} // end namespace vespa
