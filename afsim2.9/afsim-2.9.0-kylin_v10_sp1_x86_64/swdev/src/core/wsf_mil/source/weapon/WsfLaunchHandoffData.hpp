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

#ifndef WSFLAUNCHHANDOFFDATA_HPP
#define WSFLAUNCHHANDOFFDATA_HPP

#include "wsf_mil_export.h"

#include "UtVec3.hpp"
class WsfArticulatedPart;
#include "WsfGeoPoint.hpp"
class WsfLaunchComputer;
#include "WsfMilComponentRoles.hpp"
class WsfPlatform;
class WsfScenario;
#include "WsfSimpleComponent.hpp"
#include "WsfStringId.hpp"

//! Handoff data between a launching platform and the platform it launches.
//!
//! This object contains several types of data that is passed from the launching platform to
//! the launched platform. This primary includes the following, but may include other data:
//!
//! - Data about the state of the launching platform at the time of the launch (the launched
//!   platform cannot reliabily 'reach-back' to the launch platform because it may cease to
//!   exist.)
//! - Pre-launch data computed by the launch computer (e.g. loft angle, predicted intercept
//!    point, etc.)
//!
//! @note This largely parallels the data that is currently in WsfLaunchComputer.
//! It exists because there was no way to push launch computer results to the launched
//! object if it wasn't launched against a track.

// TODO_JAJ At some point some of there can probably be some incorporation of things between
//          this and WsfLaunchComputer. But I don't have time to do it right now.
class WSF_MIL_EXPORT WsfLaunchHandoffData : public WsfSimplePlatformComponent
{
protected:
   //! Validity flags for launch computer data.
   //! (Parallels what is in WsfLaunchComputer).
   enum
   {
      cVF_LAUNCH_TIME         = 0X0001,
      cVF_LAUNCHER_BEARING    = 0X0002,
      cVF_LAUNCHER_ELEVATION  = 0X0004,
      cVF_LOFT_ANGLE          = 0X0008,
      cVF_BURN_TIME           = 0X0010,
      cVF_TIME_OF_FLIGHT      = 0X0020,
      cVF_INTERCEPT_TIME      = 0X0040,
      cVF_INTERCEPT_POINT     = 0X0080,
      cVF_TARGET_IMPACT_TIME  = 0X0100,
      cVF_TARGET_IMPACT_POINT = 0X0200,
      cVF_END_TIME            = 0X0400,
      cVF_END_POINT           = 0X0800
   };

   bool ResultIsValidFor(unsigned int aDataMask) const { return (mValidityFlags & aDataMask) != 0; }

public:
   static WsfLaunchHandoffData& FindOrCreate(WsfPlatform& aPlatform);
   static WsfLaunchHandoffData* Find(const WsfPlatform& aPlatform);

   WsfLaunchHandoffData();
   WsfLaunchHandoffData(const WsfLaunchHandoffData& aSrc);
   ~WsfLaunchHandoffData() override = default;
   WsfLaunchHandoffData& operator=(const WsfLaunchHandoffData&) = delete;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override { return new WsfLaunchHandoffData(*this); }
   void*         QueryInterface(int aRole) override;
   //@}

   void SetLaunchPlatformData(WsfPlatform* aPlatformPtr, WsfArticulatedPart* aPartPtr);

   void SetLaunchComputerData(WsfLaunchComputer* aComputerPtr);

   //! @name Accessors for information about the launch platform and system.
   //@{
   size_t GetLaunchPlatformIndex() const { return mLaunchPlatformIndex; }
   double GetLaunchPlatformLat() const { return mLaunchPlatformLat; }
   double GetLaunchPlatformLon() const { return mLaunchPlatformLon; }
   double GetLaunchPlatformAlt() const { return mLaunchPlatformAlt; }
   void   GetLaunchPlatformVelNED(double aVelocityNED[3]) const { UtVec3d::Set(aVelocityNED, mLaunchPlatformVelNED); }
   double GetLaunchPlatformHeading() const { return mLaunchPlatformHeading; }
   double GetLaunchPlatformPitch() const { return mLaunchPlatformPitch; }
   double GetLaunchPlatformRoll() const { return mLaunchPlatformRoll; }

   void   GetLaunchSystemLocation(double aLocation[3]) const { UtVec3d::Set(aLocation, mLaunchSystemLocation); }
   double GetLaunchSystemYaw() const { return mLaunchSystemYaw; }
   double GetLaunchSystemPitch() const { return mLaunchSystemPitch; }
   double GetLaunchSystemRoll() const { return mLaunchSystemRoll; }
   double GetLaunchSystemTilt() const { return mLaunchSystemTilt; }

   void SetLaunchDeltaV(const double aLaunchDeltaV[3]) { UtVec3d::Set(mLaunchDeltaV, aLaunchDeltaV); }
   void GetLaunchDeltaV(double aLaunchDeltaV[3]) const { UtVec3d::Set(aLaunchDeltaV, mLaunchDeltaV); }
   //@}

   //! @name Accessors for optional data.
   //! The [XXX]IsValid method should be used prior to calling Get[XXX].
   //@{
   bool LoftAngleIsValid() const { return ResultIsValidFor(cVF_LOFT_ANGLE); }
   bool BurnTimeIsValid() const { return ResultIsValidFor(cVF_BURN_TIME); }
   bool TimeOfFlightIsValid() const { return ResultIsValidFor(cVF_TIME_OF_FLIGHT); }
   bool InterceptTimeIsValid() const { return ResultIsValidFor(cVF_INTERCEPT_TIME); }
   bool InterceptPointIsValid() const { return ResultIsValidFor(cVF_INTERCEPT_POINT); }

   double             GetLoftAngle() const { return mLoftAngle; }
   double             GetBurnTime() const { return mBurnTime; }
   double             GetTimeOfFlight() const { return mTimeOfFlight; }
   double             GetInterceptTime() const { return mInterceptTime; }
   const WsfGeoPoint& GetInterceptPoint() const { return mInterceptPoint; }
   //@}

protected:
   void SetLoftAngle(double aLoftAngle)
   {
      mLoftAngle = aLoftAngle;
      mValidityFlags |= cVF_LOFT_ANGLE;
   }

   void SetBurnTime(double aBurnTime)
   {
      mBurnTime = aBurnTime;
      mValidityFlags |= cVF_BURN_TIME;
   }

   void SetTimeOfFlight(double aTimeOfFlight)
   {
      mTimeOfFlight = aTimeOfFlight;
      mValidityFlags |= cVF_TIME_OF_FLIGHT;
   }

   void SetInterceptTime(double aInterceptTime)
   {
      mInterceptTime = aInterceptTime;
      mValidityFlags |= cVF_INTERCEPT_TIME;
   }

   void SetInterceptPoint(const WsfGeoPoint& aInterceptPoint)
   {
      mInterceptPoint = aInterceptPoint;
      mValidityFlags |= cVF_INTERCEPT_POINT;
   }

   //! Validity flags for optional data.
   unsigned int mValidityFlags;

   //! @name Launch platform data.
   //@{
   size_t mLaunchPlatformIndex; //!< Index of the launching platform.
   double mLaunchPlatformLat;
   double mLaunchPlatformLon;
   double mLaunchPlatformAlt;
   double mLaunchPlatformVelNED[3];
   double mLaunchPlatformHeading;
   double mLaunchPlatformPitch;
   double mLaunchPlatformRoll;
   //@}

   //! @name Launch system data (the articulated part).
   //@{
   double mLaunchSystemLocation[3];
   double mLaunchSystemYaw;
   double mLaunchSystemPitch;
   double mLaunchSystemRoll;
   double mLaunchSystemTilt;
   //@}

   //! @name Launch computer data (optional data)
   //@{
   double      mLoftAngle;
   double      mBurnTime;
   double      mTimeOfFlight;
   double      mInterceptTime;
   WsfGeoPoint mInterceptPoint;
   //@}

   //! @name Other handoff data (optional data)
   //@{
   double mLaunchDeltaV[3];
   double mEmptyMass;
   double mFuelMass;
   double mPayloadMass;
   //@}
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfLaunchHandoffData, cWSF_COMPONENT_LAUNCH_HANDOFF_DATA)

#endif
