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

#ifndef VAMODELVIEWERCAMERA_HPP
#define VAMODELVIEWERCAMERA_HPP

#include "vespatk_export.h"

#include "VaCamera.hpp"

namespace vespa
{

class VESPATK_EXPORT VaModelViewerCamera : public VaCamera
{
public:
   VaModelViewerCamera(VaViewer* aViewerPtr);
};

class VESPATK_EXPORT VaModelViewerCameraMotion : public VaCameraMotion
{
public:
   VaModelViewerCameraMotion(VaCamera* aCameraPtr);

   MotionType GetMotionType() override { return VaCameraMotion::cTETHERED; }
   bool       GetAllowPicking() const override { return false; }

   VaCameraMotionData* GetMotionData() const override { return nullptr; }

   bool SupportsMotionData(MotionType aMotionType) const override { return false; }

   void               GetViewMatrixAndCameraPosition(osg::Matrix& aViewMatrix, double aXYZ[3]) const override;
   const osg::Matrix& GetViewMatrix() override { return mViewMatrix; }

   VaPosition GetPosition() override { return VaPosition(0, 0, 30000); }
   void       SetPosition(const VaPosition& aPosition) override {}

   VaPosition GetReferencePosition() override { return VaPosition(0, 0, 30000); }
   void       SetReferencePosition(const VaPosition& aPosition) override {}

   double GetAz() const override { return mAz; }
   void   SetAz(double aAz) override { mAz = aAz; }

   double GetEl() const override { return mEl; }
   void   SetEl(double aEl) override { mEl = aEl; }

   double GetDist() const override { return mDist; }
   void   SetDist(double aDist) override { mDist = aDist; }

   void GetAzElDist(double& aAz, double& aEl, double& aDist) const override;
   void SetAzElDist(double aAz, double aEl, double aDist) override;

   void GetYawPitchRoll(double& aYaw, double& aPitch, double& aRoll) override {}
   void SetYawPitchRoll(double aYaw, double aPitch, double aRoll) override {}

   bool MouseScroll(int aDirection) override;

   bool MouseMotion(int aX, int aY, unsigned int aState) override;

   bool GesturePinch(double aScale, double aAngle, int aCenterX, int aCenterY) override;

   bool PassiveMouseMotion(int aX, int aY, unsigned int aState) override;

   bool MouseButtonPress(int aX, int aY, unsigned int aButton, unsigned int aState) override;

   bool MouseButtonRelease(int aX, int aY, unsigned int aButton, unsigned int aState) override;

   bool MouseButtonDoublePress(int aX, int aY, unsigned int aButton, unsigned int aState) override;

   double GetGeographicHeading() override { return 0.0; }

   void ForceRefresh() override {}

   void Reset() override {}

private:
   void BuildViewMatrix();

   double      mAz;
   double      mEl;
   double      mDist;
   osg::Matrix mViewMatrix;
   VaPosition  mLLA;
   bool        mDirty;
   bool        mLeftDown;
   bool        mMiddleDown;
   bool        mRightDown;
   bool        mPanPossible;
   bool        mZoomPossible;
   bool        mPanActive;
   int         mPanX;
   int         mPanY;
   int         mZoomX;
   int         mZoomY;
};

} // namespace vespa

#endif
