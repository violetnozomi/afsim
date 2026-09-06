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
#ifndef VaCameraMotionTethered_hpp
#define VaCameraMotionTethered_hpp

#include "vespatk_export.h"

#include <deque>

#include "VaCallbackHolder.hpp"
#include "VaCameraMotion.hpp"

namespace vespa
{

class VaEntity;

class VESPATK_EXPORT VaCameraMotionTetheredData : public VaCameraMotionData
{
public:
   VaCameraMotionTetheredData(const VaPosition   aLLA,
                              const VaPosition   aRefLLA,
                              const double       aAED[3],
                              const double       aYPR[3],
                              const osg::Matrix& aViewMatrix)
      : VaCameraMotionData(VaCameraMotion::cTETHERED, aLLA, aRefLLA, aAED, aYPR, aViewMatrix)
   {
   }

   ~VaCameraMotionTetheredData() override {}
};

class VESPATK_EXPORT VaCameraMotionTethered : public VaCameraMotion
{
public:
   VaCameraMotionTethered(VaCameraBase* aCameraPtr, VaEntity* aEntityPtr);

   ~VaCameraMotionTethered() override;

   MotionType GetMotionType() override { return VaCameraMotion::cTETHERED; }

   bool         GetAllowPicking() const override { return mAllowPicking; }
   virtual void SetAllowPicking(bool aAllow) { mAllowPicking = aAllow; }

   void Initialize(VaCameraMotionData* aDataPtr) override;

   void Reset() override;

   bool Update(double aTime) override;

   void AllowUserControl(bool aState) { mAllowUserControl = aState; }

   VaCameraMotionTetheredData* GetMotionData() const override;

   bool SupportsMotionData(MotionType aMotionType) const override;

   bool IsWeird() const override { return true; }

   static VaCameraMotionTetheredData* GetMotionDataDefault();

   void GetViewMatrixAndCameraPosition(osg::Matrix& aViewMatrix, double aXYZ[3]) const override;

   bool GetUseRefHeading() const { return mUseRefHeading; }
   void SetUseRefHeading(bool aUseRefHeading);

   VaPosition GetPosition() override { return mLLA; }
   void       SetPosition(const VaPosition& aPosition) override;

   VaPosition GetReferencePosition() override;
   void       SetReferencePosition(const VaPosition& aPosition) override;

   double GetAz() const override { return mAz; }
   void   SetAz(double aAz) override;

   double GetEl() const override { return mEl; }
   void   SetEl(double aEl) override;

   double GetDist() const override { return mDist; }
   void   SetDist(double aDist) override;
   void   RequestDist(double aDist) override;

   void GetAzElDist(double& aAz, double& aEl, double& aDist) const override;
   void SetAzElDist(double aAz, double aEl, double aDist) override;

   void ForceRefresh() override;

   void GetYawPitchRoll(double& aYaw, double& aPitch, double& aRoll) override;
   void SetYawPitchRoll(double aYaw, double aPitch, double aRoll) override;

   bool MouseScroll(int aDirection) override;

   bool MouseMotion(int aX, int aY, unsigned int aState) override;

   bool PassiveMouseMotion(int aX, int aY, unsigned int aState) override;

   bool MouseButtonPress(int aX, int aY, unsigned int aButton, unsigned int aState) override;

   bool MouseButtonRelease(int aX, int aY, unsigned int aButton, unsigned int aState) override;

   bool MouseButtonDoublePress(int aX, int aY, unsigned int aButton, unsigned int aState) override;

   bool GesturePinch(double aScale, double aAngle, int aCenterX, int aCenterY) override;

   double GetGeographicHeading() override;

   VaEntity* GetEntity() { return mEntityPtr; }

   // Lock the camera to the entities coordinate frame
   // !@ aState true - lock to entity's CF, false - 'float' around the entity
   void SetMotionLocked(bool aState) { mMotionLocked = aState; }

   // Offset the camera from the entity's CF when locked to the entity's CF
   // !@ aX The x-offset (out the nose)
   // !@ aY the y-offset (out the right wing)
   // !@ aZ the z-offset (down)
   void SetArticulatedOffset(double aX, double aY, double aZ);

   // Rotate the camera on an articulated part on the entity's CF when locked to the entity's CF
   // !@ aH The rotation about the z-axis
   // !@ aP the rotation about the y-axis
   // !@ aR the rotation about the x-axis
   void SetArticulatedRotate(double aH, double aP, double aR);

   bool RecommendNearFar(double& aNear, double& aFar) override;

   const osg::Matrix& GetViewMatrix() override { return mViewMatrix; }

protected:
   VaEntity* mEntityPtr;

   VaPosition mLLA;
   VaPosition mRefLLA;
   double     mRefOrientation[3];
   /// mAz, mEl, and mDist *position* the camera (polar coordinates relative to the
   /// tethered entity, essentially).  Changing mAz and mEl will "orbit" the camera
   /// around the tethered entity.
   double mAz;
   double mEl;
   double mDist;
   /// ordinarily, the camera is oriented to point at the tethered entity.
   /// mYaw and mPitch allow you to rotate the camera to point away from the entity.
   /// (note that this doesn't move the camera... these variables rotate the
   /// camera in-place)
   double mYaw;
   double mPitch;

   osg::Matrix mViewMatrix;
   bool        mMotionLocked;
   bool        mDirty;
   bool        mAllowPicking;

private:
   virtual void BuildViewMatrix(const VaPosition& aRefPosition, const double aRefOrientation[3]);

   void EntityMovedCB(VaEntity* aEntityPtr, bool aMoveInTime);

   virtual void EntityRemovedCB(VaEntity* aEntityPtr);

   void SetNewOrbitDistance(double aNewDistance);

   bool mLeftDown;
   bool mMiddleDown;
   bool mRightDown;
   bool mPanPossible;
   bool mPanActive;
   /// The previous mouse X position.  It is used for some "pan"
   /// (mAz/mEl, mYaw/mPitch) calculations.
   int mPanX;
   /// The previous mouse Y position.  It is used for some "pan"
   /// (mAz/mEl, mYaw/mPitch) calculations.
   int  mPanY;
   bool mZoomPossible;
   int  mZoomX;
   int  mZoomY;
   bool mUseRefHeading;

   VaCallbackHolder mCallbacks;

   double mArticulatedTranslation[3];
   double mArticulatedRotation[3];
   double mGeoHeading;
   bool   mAllowUserControl;
};
} // end namespace vespa
#endif
