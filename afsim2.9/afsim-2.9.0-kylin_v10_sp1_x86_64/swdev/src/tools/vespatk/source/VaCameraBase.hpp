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

#ifndef VaCameraBase_hpp
#define VaCameraBase_hpp

#include "vespatk_export.h"

#include "GeoRect.hpp"
#include "VaCallbackHolder.hpp"
#include "VaCameraMotion.hpp"

namespace vespa
{
class VaCameraMotion;
class VaEntity;
class VaPosition;
class VaViewer;

class VESPATK_EXPORT VaCameraBase
{
public:
   // VaCameraBase may be constructed with aViewerPtr = 0, just please make sure to call VaCameraBase::SetViewer() with
   // a valid viewer as soon as possible.
   VaCameraBase(VaViewer* aViewerPtr);

   virtual ~VaCameraBase();

   VaCameraMotion* GetCameraMotion() const { return mCameraMotionPtr; }
   /// Takes ownership of the provided VaCameraMotion
   virtual void SetCameraMotion(VaCameraMotion* aCameraMotion);

   // See base class for description.
   virtual void ConvertLLAToXYZ(double aXYZ[3], double aLat, double aLon, double aAlt);

   virtual void ConvertXYZToLLA(const double aXYZ[3], double& aLat, double& aLon, double& aAlt) const;

   // See base class for description.
   virtual bool ConvertScreenToLLA(int aX, int aY, double& aLat, double& aLon, double& aAlt);

   virtual bool ConstConvertScreenToLLA(int aX, int aY, double& aLat, double& aLon, double& aAlt) const;

   // See base class for description.
   virtual int GetViewableArea(GeoRect& aRect);

   // See base class for description.
   virtual bool IsFlat() const;

   // See base class for description.
   virtual void LoadMatrices(bool aPicking);

   virtual void LoadViewMatrixAndCameraPosition(const osg::Matrix& aViewMatrix, const double aXYZ[3]);

   // See base class for description.
   virtual void SetEntityCoordinateSystem(double aLat, double aLon, double aAlt);

   void CalculateMetersPerPixel();

   bool GetUsePerspectiveProjection() const { return (mCameraProjection & cPERSPECTIVE_BIT); }

   void UseOrthographicProjection();

   void UsePerspectiveProjection();

   void ManageProjection(bool aState);

   float* GetLookVector() { return mLookVector; }

   virtual void GetLatLon(double& aLat, double& aLon) const;

   virtual double GetMetersPerDegreeLat() const;
   virtual double GetMetersPerDegreeLon() const;

   virtual bool Update(double aTime, int aWidth, int aHeight);

   virtual void UpdateView(int aWidth, int aHeight);

   virtual VaPosition GetCameraPosition();

   virtual VaCameraMotion::MotionType GetMotionType();

   virtual bool GetAllowPicking() const;

   virtual bool MouseButtonPress(int aX, int aY, unsigned int aButton, unsigned int aState);


   virtual bool MouseButtonRelease(int aX, int aY, unsigned int aButton, unsigned int aState);

   virtual bool MouseButtonDoublePress(int aX, int aY, unsigned int aButton, unsigned int aState);

   virtual bool MouseMotion(int aX, int aY, unsigned int aState);

   virtual bool MouseScroll(int aDirection);

   virtual bool GesturePinch(double aScale, double aAngle, int aCenterX, int aCenterY);

   virtual double GetGeographicHeading();

   VaViewer* GetViewer() { return mViewerPtr; }
   void      SetViewer(VaViewer* aViewerPtr) { mViewerPtr = aViewerPtr; }

   void ForceRefresh();

   void Reset();

   void SetPitchYaw(const double aPitch, const double aYaw)
   {
      mPitch = aPitch;
      mYaw   = aYaw;
   }

   double GetYaw() { return mYaw; }
   double GetPitch() { return mPitch; }

   bool ScreenToWorld(unsigned int aX, unsigned int aY, double aDepth, VaPosition& aPosition) const;

   virtual double GetMetersPerPixel() const { return mMetersPerPixel; }

   virtual void GetAzElDist(double& aAzimuth, double& aElevation, double& aDistance) const;

   void GetViewport(unsigned int& aWidth, unsigned int& aHeight) const;

   double GetEasternOffsetStart() const { return 180.0; }
   double GetEasternOffsetValue() const { return 0.0; }
   double GetWesternOffsetStart() const { return -180.0; }
   double GetWesternOffsetValue() const { return 0.0; }

   double GetFovY() const { return mFovY; }

protected:
   virtual void CameraChanged();

   virtual void SetPerspectiveCamera(const osg::Matrix& aViewMatrix, double aXYZ[3], double aFovY, double aNear, double aFar);
   virtual void SetOrthographicCamera(const osg::Matrix& aViewMatrix, double aZoom, double aNear, double aFar);

   VaViewer* mViewerPtr;

   /// Stores mRefLat, mRefLon, mRefAlt converted to geocentric coordinates
   double mRefXYZ[3];
   float  mLookVector[3];

   double mHalfY;

   // unmanaged projections will not have their matrices automatically built here, view matrices are unaffected by this
   enum
   {
      cPERSPECTIVE_BIT = 1,
      cMANAGED_BIT     = 2
   };
   int mCameraProjection;

   // weird, only used by ortho projection
   double mPitch;
   // unused, seemingly
   double mYaw;

   VaCallbackHolder mCallbacks;

   VaCameraMotion* mCameraMotionPtr;

   /// Radius of the Earth at the equator.
   double mRadius;

private:
   int ComputeViewableArea(GeoRect& aRect) const;

   double ComputeVisibleAngle(double aX0, double aY0, double aAngle) const;
   double ComputeVisibleAngleTethered(double aY0, double aAngle) const;

   // this method return -1 if the south pole is on screen, 1 if the north pole is on screen, and 0 if no poles are on
   // screen dead code   int PoleInScreen() const;

   void CameraSetPositionLatLonCB(VaViewer* aViewerPtr, double aLat, double aLon);

   void CameraSetPositionAzElDistCB(VaViewer* aViewerPtr, double aAz, double aEl, double aDist);

   void CameraSetYawPitchDistCB(VaViewer* aViewerPtr, double aYaw, double aPitch, double aDist);

   // aProjection is 0 if perspective projection is desired, 1 if orthographic
   void CameraSetProjectionCB(int aProjection);

   // Used to avoid excessive computation in GetViewableArea
   bool    mViewableAreaValid;
   GeoRect mViewableArea;

   double mMetersPerPixel;
   double mMetersPerDegreeLat;
   double mMetersPerDegreeLon;
   double mRefHeading;
   double mRefLat;
   double mRefLon;
   double mRefAlt;
   double mDistance;
   double mAzimuth;
   double mElevation;
   double mLat;
   double mLon;
   double mFovY;
   double mAspect;
   int    mWidth;
   int    mHeight;
};
} // end namespace vespa
#endif
