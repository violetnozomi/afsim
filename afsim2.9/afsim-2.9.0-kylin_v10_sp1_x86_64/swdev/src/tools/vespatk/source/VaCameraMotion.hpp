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
#ifndef VaCameraMotion_hpp
#define VaCameraMotion_hpp

#include "vespatk_export.h"

#include <cstring>
#include <functional>
#include <iosfwd>
#include <string>

#include <osg/Matrix>

#include "UtWallClock.hpp"
#include "VaCallbackHolder.hpp"
#include "VaPosition.hpp"

namespace vespa
{
class VaCameraBase;
class VaCameraMotionData;
class VaViewer;

//! Defines the interface that all camera motion classes will provide.

class VESPATK_EXPORT VaCameraMotion
{
public:
   enum MotionType
   {
      cGEOCENTRIC,
      cTETHERED,
      cMOUNTED,
      cLOOK_AT,
      cFLY_TO,
      cCUSTOM_MOTION,
      cINVALID_MOTION,
      cMAX_MOTION_TYPE
   };
   enum InputType
   {
      cNONE               = 0,
      cPRIMARY_ROTATION   = 1,
      cSECONDARY_ROTATION = 2,
      cZOOM               = 4,
      cALL                = 7
   };

   VaCameraMotion(VaCameraBase* aCameraPtr);

   virtual ~VaCameraMotion() = default;

   virtual MotionType GetMotionType() = 0;

   virtual bool GetAllowPicking() const = 0;

   virtual void Initialize(VaCameraMotionData* aDataPtr);

   virtual bool Update(double aTime);

   VaCameraBase* GetCamera() const { return mCameraPtr; }

   virtual VaCameraMotionData* GetMotionData() const = 0;

   virtual bool SupportsMotionData(MotionType aMotionType) const = 0;

   virtual bool IsWeird() const { return false; }

   virtual double GetFovY() const { return mFovY; }
   virtual void   SetFovY(double aFovY) { mFovY = aFovY; }

   virtual void GetViewMatrixAndCameraPosition(osg::Matrix& aViewMatrix, double aXYZ[3]) const = 0;

   virtual VaPosition GetPosition()                            = 0;
   virtual void       SetPosition(const VaPosition& aPosition) = 0;

   virtual VaPosition GetReferencePosition()                            = 0;
   virtual void       SetReferencePosition(const VaPosition& aPosition) = 0;

   virtual void AnimateToPosition(const VaPosition& aPosition, double aYaw, double aPitch, double aTime = 0.25)
   {
      SetReferencePosition(aPosition);
   }

   virtual double GetAz() const     = 0;
   virtual void   SetAz(double aAz) = 0;

   virtual double GetEl() const     = 0;
   virtual void   SetEl(double aEl) = 0;

   virtual double GetDist() const       = 0;
   virtual void   SetDist(double aDist) = 0;
   // this is a better thing to call than SetDist from the application.  This allows the class to do some interpretation
   // of the inputs.  For example, VaCameraMotionTethered will switch to motion locked when it receives a aDist < 1.0,
   // whereas calling SetDist will just set the dist to 0;
   virtual void RequestDist(double aDist) { SetDist(aDist); }


   virtual void GetAzElDist(double& aAz, double& aEl, double& aDist) const   = 0;
   virtual void SetAzElDist(double aAz, double aEl, double aDist)            = 0;
   virtual void GetYawPitchRoll(double& aYaw, double& aPitch, double& aRoll) = 0;
   virtual void SetYawPitchRoll(double aYaw, double aPitch, double aRoll)    = 0;

   virtual bool MouseScroll(int aDirection) = 0;

   virtual bool MouseMotion(int aX, int aY, unsigned int aState) = 0;

   virtual bool GesturePinch(double aScale, double aAngle, int aCenterX, int aCenterY) = 0;

   virtual bool PassiveMouseMotion(int aX, int aY, unsigned int aState) = 0;

   virtual bool MouseButtonPress(int aX, int aY, unsigned int aButton, unsigned int aState) = 0;

   virtual bool MouseButtonRelease(int aX, int aY, unsigned int aButton, unsigned int aState) = 0;

   virtual bool MouseButtonDoublePress(int aX, int aY, unsigned int aButton, unsigned int aState) = 0;

   virtual double GetGeographicHeading() = 0;

   virtual void ForceRefresh() = 0;

   virtual void Reset() = 0;

   virtual bool RecommendNearFar(double& aNear, double& aFar) { return false; }

   virtual const osg::Matrix& GetViewMatrix() = 0;

   // soft lock provides a signal to other classes
   // when soft lock is set to true, it will only switch to false on a user driven motion in condition mask
   virtual void SetSoftLock(bool aState, int aPermissiblies = cALL, int aBreakConditions = cALL);
   bool         SoftLock() const { return mSoftLock; }
   bool         HandleSoftLock(InputType aInput);

protected:
   void UpdateFovY(double aTime);

   /// Folds the input angle into the range -180 to 180.  If passed -181, returns 179.
   double WrapAngleNeg180To180(double aAngle) const;
   /// Folds the input angle into the range 0 to 360.  If passed -1, returns 359.
   double WrapAngle0To360(double aAngle) const;

   bool mSoftLock{false};
   int  mSoftLockPermissibles{cALL};
   int  mSoftLockBreakConditions{cALL};

private:
   static std::string cMotionTypeStrings[cMAX_MOTION_TYPE];

   VaCameraBase*    mCameraPtr;
   double           mFovY;
   double           mFovYMotion[3];
   UtWallClock      mFovYTimer;
   VaCallbackHolder mCallbacks;
};

class VESPATK_EXPORT VaCameraMotionData
{
public:
   VaCameraMotionData(VaCameraMotion::MotionType aMotionType,
                      const VaPosition           aLLA,
                      const VaPosition           aRefLLA,
                      const double               aAED[3],
                      const double               aYPR[3],
                      const osg::Matrix&         aViewMatrix)
      : mMotionType(aMotionType)
      , mLLA(aLLA)
      , mRefLLA(aRefLLA)
      , mFovY(-1.0)
   {
      UtVec3d::Set(mAED, aAED);
      UtVec3d::Set(mYPR, aYPR);
      UtVec3d::Set(mFovYMotion, -1.0);
      mViewMatrix = aViewMatrix;
   }

   virtual ~VaCameraMotionData() {}

   VaCameraMotion::MotionType mMotionType;    //! An enumeration that specifies the type of camera motion.
   VaPosition                 mLLA;           //! Camera's position
   VaPosition                 mRefLLA;        //! Reference's position
   double                     mAED[3];        //! Azimuth, elevation, and distance
   double                     mYPR[3];        //! Yaw, pitch, and roll
   double                     mFovY;          //! Camera's vertical field of view
   double                     mFovYMotion[3]; //! Begin FovY, End FovY, duration
   osg::Matrix                mViewMatrix;    //! The view matrix
};

} // end namespace vespa
#endif
