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

#ifndef SOSM_SENSOR_HPP
#define SOSM_SENSOR_HPP

#include "sosm_export.h"

#include <memory>
#include <string>
#include <vector>

class SOSM_Atmosphere;
#include "SOSM_CoreBase.hpp"
class SOSM_Interaction;
#include "SOSM_ScalarTable2D.hpp"
#include "SOSM_Selector.hpp"
class SOSM_Target;

//! A class that represents an optical sensor (IR or visible).
class SOSM_EXPORT SOSM_Sensor : public SOSM_CoreBase
{
public:
   SOSM_Sensor(SOSM_Manager* aManagerPtr);
   SOSM_Sensor(const SOSM_Sensor& aSrc);
   ~SOSM_Sensor() override;
   SOSM_Sensor& operator=(const SOSM_Sensor&) = delete;

   SOSM_Sensor* Clone() const;

   bool Initialize() override;

   void InputComplete() override;

   bool ProcessInput(UtInput& aInput) override;

   void ComputeIntegratedAtmosphericData(SOSM_Interaction& aInteraction,
                                         float&            aBackgroundRadiance_xR,
                                         float&            aForegroundRadiance_xR,
                                         float&            aTransmittance_xR);

   void ComputeIntegratedTargetData(SOSM_Target*      aTargetPtr,
                                    SOSM_Interaction& aInteraction,
                                    float&            aTargetIntensity_xR,
                                    float&            aProjectedArea);

   float ComputeProbabilityOfDetection(float aTargetIrradiance, SOSM_Interaction& aInteraction) const;

   float ComputeTargetIrradiance(SOSM_Target* aTargetPtr, SOSM_Interaction& aInteraction);

   //! Return the pointer to the atmosphere object.
   //! @note This is valid only after Initialize().
   SOSM_Atmosphere* GetAtmosphere() const { return mAtmospherePtr; }

   //! Return the average response.
   float GetAverageResponse() const { return mSharedDataPtr->mAverageResponse; }

   //! Return the detection threshold for targets above the horizon.
   //! @returns The detection threshold as a unitless signal-to-noise ratio.
   float GetDetectionThresholdAboveHorizon() const { return mSharedDataPtr->mDetectionThresholdAboveHorizon; }

   //! Return the detection threshold for targets below the horizon.
   //! @returns The detection threshold as a unitless signal-to-noise ratio.
   float GetDetectionThresholdBelowHorizon() const { return mSharedDataPtr->mDetectionThresholdBelowHorizon; }

   //! Get the integration factor (delta wavelength * response) used for spectral integration
   std::vector<float> GetIntegrationFactor() const { return mSharedDataPtr->mIntegFactor; }

   //! Return the 'Noise Equivalent Irradiance' (NEI).
   //! @returns The Noise Equivalent Irradiance (W/m^2).
   float GetNoiseEquivalentIrradiance() const { return mSharedDataPtr->mNoiseEquivalentIrradiance; }

   void GetResponseCurve(std::vector<float>& aWavelength, std::vector<float>& aResponse) const;

   //! Return the wavelength limits (in microns) of the sensor.
   void GetWavelengthLimits(float& aLowerWavelength, float& aUpperWavelength) const
   {
      aLowerWavelength = mSharedDataPtr->mLowerWavelength;
      aUpperWavelength = mSharedDataPtr->mUpperWavelength;
   }

   static double ComputeProbabilityOfDetection(double aSignal, double aNoise, double aThreshold);

   SOSM_Manager* GetManager() const { return mManagerPtr; }

private:
   class ResponsePoint
   {
   public:
      bool operator<(const ResponsePoint& aRhs) const { return (mWavelength < aRhs.mWavelength); }
      bool operator==(const ResponsePoint& aRhs) const { return (mWavelength == aRhs.mWavelength); }

      //! Wavelength (in microns) of the point.
      float mWavelength;
      //! The normalized response of the point.
      float mResponse;
   };

   //! Data that is shared amongst all instances of a given detector type
   class SharedData : public SOSM_SpectralObject
   {
   public:
      SharedData();

      float ComputeAverageResponse(const TblIndVarU<float>& aWavelengthVar,
                                   const TblDepVar1<float>& aResponseVar,
                                   float                    aLowerWavelength,
                                   float                    aUpperWavelength);
      void  InputComplete(SOSM_Sensor& aSensor);
      bool  ProcessInput(UtInput& aInput, SOSM_Sensor& aSensor);
      float ReadWavenumber(UtInput& aInput);
      void  ShowResponse(SOSM_Sensor& aSensor, const std::vector<float>& aResponse);

      //! The input response curve of the sensor as a function of wavelength.
      std::vector<ResponsePoint> mResponseCurve;

      //! The lower wavelength to which the sensor responds (um).
      float mLowerWavelength;

      //! The upper wavelength to which the sensor responds (um).
      float mUpperWavelength;

      //! Noise Equivalent Irradiance (NEI) (W/m^2)
      float mNoiseEquivalentIrradiance;

      //! Signal-To-Noise required for detection if the target is above the horizon
      float mDetectionThresholdAboveHorizon;

      //! Signal-To-Noise required for detection if the target is below the horizon
      float mDetectionThresholdBelowHorizon;

      //! Pointer to installation adjustment table.
      std::shared_ptr<SOSM_ScalarTable2D> mInstallationAdjustmentTablePtr;

      std::string mAtmosphereType;

      //! true if the response table should be shown during initialization.
      bool mShowResponse;

      //! Combines a factor used during the spectral integration loop.
      //! The i'th entry represents the interval between samples i and i+1,
      //! therefore there is one less entry than the number of samples.
      //!
      //! The i'th entry is:
      //!
      //!   R(i) * (lambda(i) - lambda(i+1)) / 2.0
      std::vector<float> mIntegFactor;

      //! Combines a factor used when forming the 'integrated' transmittance.
      //! The i'th entry represents the interval between samples i and i+1,
      //! therefore there is one less entry than the number of samples.
      //!
      //! The i'th entry is:
      //!
      //!   R(i) * / 2.0
      std::vector<float> mTransFactor;

      //! The portion of the first integration interval that is actually used.
      float mFirstIntervalFraction;

      //! The portion of the last  integration interval that is actually used.
      float mLastIntervalFraction;

      //! The sum of the interval sizes within the response of the sensor.
      //! This is:
      //!
      //!  (number of intervals - 2) + mFirstIntervalFraction + mLastIntervalFraction.
      float mSumIntervalSize;

      //! The 'average' response.
      float mAverageResponse;
   };

   SOSM_Manager* mManagerPtr;

   //! Pointer to the shared data.
   std::shared_ptr<SharedData> mSharedDataPtr;

   //! Pointer to the atmosphere instance
   SOSM_Atmosphere* mAtmospherePtr;

   //! The intersection between this (the sensor) and the atmosphere object.
   SOSM_Selector mAtmosphereSelector;
};

#endif
