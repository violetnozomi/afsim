// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef SOSM_SIMPLEINTERACTION_HPP
#define SOSM_SIMPLEINTERACTION_HPP

#include "SOSM_Interaction.hpp"

//! A concrete class that provides a minimal implementation of SOSM_Interaction.
//!
//! This class provides a minimal implementation of SOSM_Interaction for those uses where
//! the simulation may not be maintaining this data by itself. Methods are provides to
//! either set the values directly or by calling one of the ComputeGeometry methods to
//! compute the required data based on the supplied parameters.
class SOSM_SimpleInteraction : public SOSM_Interaction
{
public:
   SOSM_SimpleInteraction();

   //! @name Implementations of base class methods.
   //@{
   float         GetSlantRange() override { return mSlantRange; }
   float         GetSensorAltitude() override { return mSensorAltitude; }
   virtual float GetSensorSpeed() { return mSensorSpeed; }
   float         GetTargetAltitude() override { return mTargetAltitude; }
   float         GetTargetSpeed() override { return mTargetSpeed; }
   float         GetTargetThrottle() override { return mTargetThrottle; }
   float         GetAbsoluteTargetElevation() override { return mAbsoluteTargetElevation; }
   void          GetSensorToTargetAspect(float& aAzimuth, float& aElevation) override
   {
      aAzimuth   = mSensorToTargetAzimuth;
      aElevation = mSensorToTargetElevation;
   }
   void GetTargetToSensorAspect(float& aAzimuth, float& aElevation) override
   {
      aAzimuth   = mTargetToSensorAzimuth;
      aElevation = mTargetToSensorElevation;
   }
   //@}

   void ComputeGeometry(float aRange,
                        float aSensorAltitude,
                        float aTargetAltitude,
                        float aTargetHeading,
                        float aTargetPitch,
                        float aTargetRoll);

   void ComputeGeometry(double aSensorLatitude,
                        double aSensorLongitude,
                        float  aSensorAltitude,
                        float  aSensorHeading,
                        float  aSensorPitch,
                        float  aSensorRoll,
                        double aTargetLatitude,
                        double aTargetLongitude,
                        float  aTargetAltitude,
                        float  aTargetHeading,
                        float  aTargetPitch,
                        float  aTargetRoll);

   //! @name Methods to directly set the values returned by this class.
   //@{
   void SetSlantRange(float aSlantRange) { mSlantRange = aSlantRange; }
   void SetSensorAltitude(float aAltitude) { mSensorAltitude = aAltitude; }
   void SetSensorSpeed(float aSpeed) { mSensorSpeed = aSpeed; }
   void SetTargetAltitude(float aAltitude) { mTargetAltitude = aAltitude; }
   void SetTargetSpeed(float aSpeed) { mTargetSpeed = aSpeed; }
   void SetTargetThrottle(float aThrottle) { mTargetThrottle = aThrottle; }
   void SetAbsoluteTargetElevation(float aElevation) { mAbsoluteTargetElevation = aElevation; }
   void SetSensorToTargetAspect(float aAzimuth, float aElevation)
   {
      mSensorToTargetAzimuth   = aAzimuth;
      mSensorToTargetElevation = aElevation;
   }
   void SetTargetToSensorAspect(float aAzimuth, float aElevation)
   {
      mTargetToSensorAzimuth   = aAzimuth;
      mTargetToSensorElevation = aElevation;
   }
   //@}

private:
   float mSlantRange;
   float mSensorAltitude;
   float mSensorSpeed;
   float mTargetAltitude;
   float mTargetSpeed;
   float mTargetThrottle;
   float mAbsoluteTargetElevation;
   float mSensorToTargetAzimuth;
   float mSensorToTargetElevation;
   float mTargetToSensorAzimuth;
   float mTargetToSensorElevation;
};

#endif
