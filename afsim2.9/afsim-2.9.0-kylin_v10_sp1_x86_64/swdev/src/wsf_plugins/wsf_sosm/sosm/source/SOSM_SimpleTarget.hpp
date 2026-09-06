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

#ifndef SOSM_SIMPLETARGET_HPP
#define SOSM_SIMPLETARGET_HPP

#include "SOSM_ScalarTable2D.hpp"
#include "SOSM_Target.hpp"

class SOSM_SimpleTarget : public SOSM_Target
{
public:
   SOSM_SimpleTarget(SOSM_Manager* aManagerPtr);

   SOSM_Target* Clone() const override;

   void ComputeRadiantIntensity(const SOSM_SpectralObject& aRequestor,
                                SOSM_Interaction&          aInteraction,
                                std::vector<float>&        aBodyIntensity,
                                float&                     aBodyArea,
                                std::vector<float>&        aPlumeIntensity,
                                float&                     aPlumeArea) override;

   void GetSamplePoints(SOSM_Interaction& aInteraction, std::vector<float>& aAzValues, std::vector<float>& aElValues) override;

   void GetSampleTargetState(SOSM_Interaction& aInteraction, float& aAltitude, float& aSpeed, float& aThrottle) override;

   float BodyTemperature(SOSM_Interaction& aInteraction);

protected:
   StateBase* CreateState(const std::string& aStateName) const override;

   typedef SOSM_ScalarTable2D::TablePtr PolarTablePtr;

   class ST_State : public StateBase
   {
   public:
      ST_State(const std::string& aStateName);

      void InputComplete(SOSM_Target& aTarget) override;

      bool ProcessInput(UtInput& aInput, SOSM_Target& aTarget) override;

      void ReadRadiantIntensity(UtInput& aInput, float& aValue);

      //! The cold part area (or total structure area) as a function of az/el. (cm^2)
      PolarTablePtr mColdPartAreaTablePtr;

      //! The hot part area as a function of az/el. (cm^2)
      PolarTablePtr mHotPartAreaTablePtr;

      //! The area of the hot parts as a fraction of the total structure area.
      PolarTablePtr mHotPartAreaFractionTablePtr;

      //! The plume area as a function of az/el. (cm^2)
      PolarTablePtr mPlumeAreaTablePtr;

      //! The constant cold part area. (cm^2)
      float mConstantColdPartArea;

      //! The user defined recovery factor (for aero heating model)
      float mRecoveryFactor;

      //! Ratio of specific heats (for aero heating model)
      float mGamma;

      //! The constant cold part temperature
      //! (replaces aero heating temperature in blackbody model if > 0.0)
      float mColdPartTemperature;

      //! The constant radiant intensity (replaces blackbody model if > 0.0) (W/sr)
      float mColdPartRadiantIntensity;

      //! The constant hot part area. (cm^2)
      float mConstantHotPartArea;

      //! The constant hot part temperature (used for blackbody model if > 0.0)
      float mHotPartTemperature;

      //! The constant hot part radiant intensity (replaces blackbody model if > 0.0) (W/sr)
      float mHotPartRadiantIntensity;

      //! The constant plume area. (cm^2)
      float mConstantPlumeArea;

      //! The constant plume temperature (used for blackbody model if > 0.0)
      float mPlumeTemperature;

      //! The constant plume radiant intensity (replaces blackbody model if > 0.0) (W/sr)
      float mPlumeRadiantIntensity;

      //! The sample altitude used for fast detection mode table generation.
      float mSampleAltitude;

      //! The sample speed used for fast detection mode table generation.
      float mSampleSpeed;

      //! The sample throttle used for fast detection mode table generation.
      float mSampleThrottle;

      //! True if the sample speed is a mach number.
      bool mSampleSpeedIsMach;
   };
};

#endif
