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
#ifndef VaCameraMotionGeocentric_hpp
#define VaCameraMotionGeocentric_hpp

#include "vespatk_export.h"

#include <memory>

#include "VaCallbackHolder.hpp"
#include "VaCameraMotion.hpp"
#include "VaPosition.hpp"

namespace vespa
{
class VaPosition;
class VaSelectionBox;

class VESPATK_EXPORT VaCameraMotionGeocentricData : public VaCameraMotionData
{
public:
   VaCameraMotionGeocentricData(const VaPosition   aLLA,
                                const VaPosition   aRefLLA,
                                const double       aAED[3],
                                const double       aYPR[3],
                                const osg::Matrix& aViewMatrix)
      : VaCameraMotionData(VaCameraMotion::cGEOCENTRIC, aLLA, aRefLLA, aAED, aYPR, aViewMatrix)
   {
   }

   ~VaCameraMotionGeocentricData() override = default;
};

class VESPATK_EXPORT VaCameraMotionGeocentric : public VaCameraMotion
{
public:
   VaCameraMotionGeocentric(VaCameraBase* aCameraPtr, bool aLockRotations = false);

   ~VaCameraMotionGeocentric() override = default;

   MotionType GetMotionType() override { return VaCameraMotion::cGEOCENTRIC; }

   bool GetAllowPicking() const override { return true; }

   void Initialize(VaCameraMotionData* aDataPtr) override;

   bool Update(double aTime) override;

   VaCameraMotionData* GetMotionData() const override;

   bool SupportsMotionData(MotionType aMotionType) const override;

   static VaCameraMotionData* GetMotionDataDefault();

   void GetViewMatrixAndCameraPosition(osg::Matrix& aViewMatrix, double aXYZ[3]) const override;

   VaPosition GetPosition() override;
   void       SetPosition(const VaPosition& aPosition) override;

   VaPosition GetReferencePosition() override;
   void       SetReferencePosition(const VaPosition& aRefPos) override;

   double GetAz() const override;
   void   SetAz(double aAz) override;
   double GetEl() const override { return mEl; }
   void   SetEl(double aEl) override;
   double GetDist() const override { return mDist; }
   void   SetDist(double aDist) override;
   void   GetAzElDist(double& aAz, double& aEl, double& aDist) const override;
   void   SetAzElDist(double aAz, double aEl, double aDist) override;

   void GetYawPitchRoll(double& aYaw, double& aPitch, double& aRoll) override
   {
      aYaw   = mYaw;
      aPitch = mPitch;
      aRoll  = 0.0;
   }
   void SetYawPitchRoll(double aYaw, double aPitch, double aRoll) override
   {
      mYaw   = aYaw;
      mPitch = aPitch;
   }

   bool MouseScroll(int aDirection) override;

   bool MouseMotion(int aX, int aY, unsigned int aState) override;

   bool PassiveMouseMotion(int aX, int aY, unsigned int aState) override;

   bool MouseButtonPress(int aX, int aY, unsigned int aButton, unsigned int aState) override;

   bool MouseButtonRelease(int aX, int aY, unsigned int aButton, unsigned int aState) override;

   bool MouseButtonDoublePress(int aX, int aY, unsigned int aButton, unsigned int aState) override;

   bool GesturePinch(double aScale, double aAngle, int aCenterX, int aCenterY) override;

   void EnableTouchScreenDelay(bool aEnabled) { mEnableTouchScreenDelay = aEnabled; }

   void BoxZoom(const VaSelectionBox& aBox);

   double GetGeographicHeading() override;

   void ForceRefresh() override;

   void Reset() override;

   virtual void FlatMapMode(bool aLock);

   virtual void  SetPanDragMultiplier(float aNewMultiplier) { mPanDragMultiplier = aNewMultiplier; }
   virtual float GetPanDragMultiplier() const { return mPanDragMultiplier; }

   bool RecommendNearFar(double& aNear, double& aFar) override;

   const osg::Matrix& GetViewMatrix() override { return mViewMatrix; }

   void AnimateToPosition(const VaPosition& aPosition, double aYaw, double aPitch, double aTime) override;
   bool LeftDown() const { return mLeftDown; }
   bool MiddleDown() const { return mMiddleDown; }
   bool RightDown() const { return mRightDown; }

protected:
   virtual void BuildViewMatrix();
   void         SetLLA(double aLat, double aLon, double aAlt) { mLLA.SetLLA(aLat, aLon, aAlt); }
   void         SetViewMatrix(const osg::Matrixd& aMatrix) { mViewMatrix = aMatrix; }

   void LLAToWorld(double aLat, double aLon, double aAlt, VaPosition& aPosition);

   double mDist{12585018.0}; // this was found to be a nice distance for viewing the flat map projection
private:
   void CheckElLimits();

   void HandleSelectionBoxResultCB(const std::string&    aBoxOperation,
                                   VaViewer*             aViewerPtr,
                                   const VaSelectionBox& aSelectionBox);

   struct AnimationStruct
   {
      double mOldLat;
      double mOldLon;
      double mOldDist;
      double mOldYaw;
      double mOldPitch;
      double mNewLat;
      double mNewLon;
      double mNewDist;
      double mNewYaw;
      double mNewPitch;
      double mStartTime;
      double mDuration;

      AnimationStruct(double aOldLat,
                      double aOldLon,
                      double aOldDist,
                      double aOldYaw,
                      double aOldPitch,
                      double aNewLat,
                      double aNewLon,
                      double aNewDist,
                      double aNewYaw,
                      double aNewPitch,
                      double aStartTime,
                      double aDuration)
         : mOldLat(aOldLat)
         , mOldLon(aOldLon)
         , mOldDist(aOldDist)
         , mOldYaw(aOldYaw)
         , mOldPitch(aOldPitch)
         , mNewLat(aNewLat)
         , mNewLon(aNewLon)
         , mNewDist(aNewDist)
         , mNewYaw(aNewYaw)
         , mNewPitch(aNewPitch)
         , mStartTime(aStartTime)
         , mDuration(aDuration)
      {
      }
   };

   int              mViewportWidth;
   int              mViewportHeight;
   bool             mLeftDown{false};
   bool             mMiddleDown{false};
   bool             mRightDown{false};
   bool             mPanPossible{false};
   bool             mPanActive{false};
   int              mPanX{0};
   int              mPanY{0};
   double           mScaleFactor;
   VaPosition       mLLA;
   VaPosition       mRefLLA;
   double           mAz{0.0};
   double           mEl{0.0};
   double           mYaw{0.0};
   double           mPitch{90.0};
   osg::Matrix      mViewMatrix;
   float            mECI_Offset{0.0F}; // this is received from the ephemeris for the ECI camera
   bool             mDirty{true};
   double           mTime{0.0};
   double           mTimeMouseEventStarted{0.0};
   double           mTimeMouseEventDelay{0.1};
   VaCallbackHolder mCallbacks;
   bool             mEnableTouchScreenDelay{
      false}; // this bool adds a delay on mouse press for touch screen devices so they can properly pinch to zoom

   bool mDebug{false};
   bool mFlatMapMode;

   /// Determines how much the map moves when the user click-drags.  A value
   /// of 1 results in motion that keeps a point on the earth under the cursor
   /// during a drag (more or less; this works best at the equator).  Higher
   /// values result in more map motion for a given drag distance.
   float mPanDragMultiplier{1.0F};

   static std::map<int, double>     sDistCacheMap;
   std::unique_ptr<AnimationStruct> mAnimationStruct;
};
} // end namespace vespa
#endif
