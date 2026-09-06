// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFOPTICALSENSOR_HPP
#define WSFOPTICALSENSOR_HPP

#include "wsf_mil_export.h"

#include <map>
#include <memory>
#include <vector>

#include "UtAtmosphere.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
class WsfPlatform;
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"

//! A specialization of WsfSensor that implements a simple electro-optical sensor.

class WSF_MIL_EXPORT WsfOpticalSensor : public WsfSensor
{
public:
   WsfOpticalSensor(WsfScenario& aScenario);
   WsfOpticalSensor(const WsfOpticalSensor& aSrc);
   WsfOpticalSensor& operator=(const WsfOpticalSensor&) = delete;

   WsfSensor* Clone() const override;
   bool       Initialize(double aSimTime) override;
   void       Update(double aSimTime) override;

   size_t      GetEM_RcvrCount() const override;
   WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const override;

   void PlatformDeleted(WsfPlatform* aPlatformPtr) override;

   //! A mode of the sensor.
   class OpticalMode : public WsfSensorMode
   {
   public:
      OpticalMode(WsfScenario& aScenario);
      OpticalMode(const OpticalMode& aSrc);
      OpticalMode& operator=(const OpticalMode&) = delete;
      ~OpticalMode() override                    = default;

      WsfMode* Clone() const override;
      bool     Initialize(double aSimTime) override;
      bool     ProcessInput(UtInput& aInput) override;

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      virtual void PlatformDeleted(WsfPlatform* aPlatformPtr);

      double ComputeAtmosphericTransmittance(WsfSensorResult& aResult);

      double ComputeProbabilityOfDetection(double aSize, double aContrast, size_t aTgtIndex);

      double ComputeSimpleTransmittance(double aAttenuationPerMeter,
                                        double aHeightMSL_1,
                                        double aHeightMSL_2,
                                        double aGroundRange);

      std::unique_ptr<WsfEM_Antenna> mAntenna;
      std::unique_ptr<WsfEM_Rcvr>    mRcvr;

      //! observer glimpse time
      double mGlimpseTime;

      //! re-acquisition time
      double mReacquireTime;

      //! Fixed atmospheric attenuation (1/m)
      double mAtmosphericAttenuation;

      //! Fixed background radiance (W/m^2/sr)
      double mBackgroundRadiance;

      //! Fixed path radiance (W/m^/sr)
      double mPathRadiance;

      bool mVerbose;

      //! Atmosphere model.
      UtAtmosphere mAtmosphere;

      //! Air density at sea level (kg/m^3)
      double mRhoSeaLevel;

      //! a structure to store cumulative probabilities of detection
      struct CumPd
      {
         CumPd(double aSearchPd = 1.0, double aReacquirePd = 0.0, double aTrackPd = 0.0)
            : mSearchPd(aSearchPd)
            , mReacquirePd(aReacquirePd)
            , mTrackPd(aTrackPd)
         {
         }

         double mSearchPd;
         double mReacquirePd;
         double mTrackPd;
      };

      //! Glimpse probability data.
      class GlimpseProbability
      {
      public:
         GlimpseProbability();
         GlimpseProbability(const GlimpseProbability& aSrc) = default;
         GlimpseProbability& operator=(const GlimpseProbability&) = delete;
         virtual ~GlimpseProbability()                            = default;

         virtual GlimpseProbability* Clone() const;
         virtual bool                Initialize();
         virtual bool                ProcessInput(UtInput& aInput);
         virtual double              ProbabilityOfDetection(double aSize, double aContrast);

         //! Azimuth FOV
         double mAzimuthFOV;

         //! minimum elevation of FOV
         double mMinElevation;

         //! maximum elevation of FOV
         double mMaxElevation;

         //! number of iterations
         int mNumIterations;

         //! magnification
         double mMagnification;

         //! apparent half-angle FOV
         double mApparentHalfAngle;

         //! sensor resolution (stored steradians, entered as deg)
         double mResolution;

         //! sensor gain
         double mGain;

         //! ocular integration interval (deg)
         double mOcularIntegration;

      protected:
         virtual void   ComputeProbabilityDistribution();
         virtual double ContrastRatioToPd(double aRatio);
         virtual double InterpolateDistribution(double aThetaMin, double aThetaMax);
         virtual double ContrastThreshold(double aSolidAngle, double aAngle);
         virtual double ContrastThreshold0(double aAlpha);
         virtual double ContrastThreshold1(double aAlpha);

         //! angular separation cumulative probability distribution
         std::vector<double> mProbabilityDistribution;
      };

   protected:
      //! map to store cumulative probabilities
      std::map<size_t, CumPd> mPdMap;

      //! The mode-specific search, re-acquire and track glimpse probabilities
      GlimpseProbability mSearch;
      GlimpseProbability mReacquire;
      GlimpseProbability mTrack;
   };

protected:
   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<OpticalMode*> mOpticalModeList;
};

#endif
