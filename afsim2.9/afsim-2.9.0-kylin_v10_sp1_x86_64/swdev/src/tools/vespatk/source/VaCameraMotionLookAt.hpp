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
#ifndef VaCameraMotionLookAt_hpp
#define VaCameraMotionLookAt_hpp

#include "vespatk_export.h"

#include "VaCallbackHolder.hpp"
#include "VaCameraMotion.hpp"

namespace vespa
{

class VaEntity;

class VESPATK_EXPORT VaCameraMotionLookAtData : public VaCameraMotionData
{
public:
   VaCameraMotionLookAtData(const VaPosition  aLLA,
                            const VaPosition  aRefLLA,
                            const double      aAED[3],
                            const double      aYPR[3],
                            const osg::Matrix aViewMatrix)
      : VaCameraMotionData(VaCameraMotion::cLOOK_AT, aLLA, aRefLLA, aAED, aYPR, aViewMatrix)
   {
   }

   ~VaCameraMotionLookAtData() override {}
};

class VaCameraMotionLookAt : public VaCameraMotion
{
public:
   VaCameraMotionLookAt(VaCameraBase* aCameraPtr, VaEntity* aReferenceEntityPtr);

   ~VaCameraMotionLookAt() override;

   MotionType GetMotionType() override { return VaCameraMotion::cLOOK_AT; }

   bool GetAllowPicking() const override { return false; }

   void Initialize(VaCameraMotionData* aDataPtr) override;

   void Reset() override;

   bool Update(double aTime) override;

   VaCameraMotionLookAtData* GetMotionData() const override;

   bool SupportsMotionData(MotionType aMotionType) const override;

   bool IsWeird() const override { return true; }

   static VaCameraMotionLookAtData* GetMotionDataDefault();

   void               GetViewMatrixAndCameraPosition(osg::Matrix& aViewMatrix, double aXYZ[3]) const override;
   const osg::Matrix& GetViewMatrix() override { return mViewMatrix; }

   VaPosition GetPosition() override;
   void       SetPosition(const VaPosition& aPosition) override;

   VaPosition GetReferencePosition() override;
   void       SetReferencePosition(const VaPosition& aPosition) override;

   double GetAz() const override { return mAz; }
   void   SetAz(double aAz) override;

   double GetEl() const override { return mEl; }
   void   SetEl(double aEl) override;

   double GetDist() const override { return mDist; }
   void   SetDist(double aDist) override;

   void GetAzElDist(double& aAz, double& aEl, double& aDist) const override;
   void SetAzElDist(double aAz, double aEl, double aDist) override;

   void GetYawPitchRoll(double& aYaw, double& aPitch, double& aRoll) override;
   void SetYawPitchRoll(double aYaw, double aPitch, double aRoll) override;

   void ForceRefresh() override;

   virtual void LookAt(const VaPosition& aPosition);

   virtual void LookAt(VaEntity* aEntityPtr);

   virtual void LookAtClear();

   bool MouseScroll(int aDirection) override;

   bool MouseMotion(int aX, int aY, unsigned int aState) override;

   bool PassiveMouseMotion(int aX, int aY, unsigned int aState) override;

   bool MouseButtonPress(int aX, int aY, unsigned int aButton, unsigned int aState) override;

   bool MouseButtonRelease(int aX, int aY, unsigned int aButton, unsigned int aState) override;

   bool MouseButtonDoublePress(int aX, int aY, unsigned int aButton, unsigned int aState) override;

   bool GesturePinch(double aScale, double aAngle, int aCenterX, int aCenterY) override;

   double GetGeographicHeading() override;

   virtual VaEntity* GetEntity() const { return mReferenceEntityPtr; }
   virtual VaEntity* GetLookAtEntity() const { return mLookAtEntityPtr; }

private:
   void BuildViewMatrix(const VaPosition& aRefPosition);

   void EntityMovedCB(VaEntity* aEntityPtr, bool aEntityMoving);

   void EntityRemovedCB(VaEntity* aEntityPtr);


   VaEntity*        mReferenceEntityPtr;
   VaEntity*        mLookAtEntityPtr;
   bool             mDebug;
   VaPosition       mLLA;
   VaPosition       mRefPosition;
   VaPosition       mLookAtPosition;
   double           mAz;
   double           mEl;
   double           mDist;
   bool             mUseRefHeading;
   double           mRefHeading;
   osg::Matrix      mViewMatrix;
   bool             mDirty;
   VaCallbackHolder mCallbacksRef;
   VaCallbackHolder mCallbacksLookAt;
};
} // end namespace vespa
#endif
