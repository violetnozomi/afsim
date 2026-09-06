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

#ifndef WSFACOUSTICSENSOR_HPP
#define WSFACOUSTICSENSOR_HPP

#include "wsf_mil_export.h"

#include <vector>

#include "UtAtmosphere.hpp"
class WsfAcousticSignature;
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Types.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"

namespace wsf
{
class Terrain;
}

//! A specialization of WsfSensor that implements a simple passive acoustic sensor representing human hearing.

class WSF_MIL_EXPORT WsfAcousticSensor : public WsfSensor
{
public:
   WsfAcousticSensor(WsfScenario& aScenario);
   WsfAcousticSensor(const WsfAcousticSensor& aSrc);
   WsfAcousticSensor& operator=(const WsfAcousticSensor&) = delete;
   ~WsfAcousticSensor() override;

   // acoustic sensor type
   enum AcousticTypes
   {
      cACOUSTIC_HUMAN  = 0,
      cACOUSTIC_NARROW = 1,
      cACOUSTIC_WIDE   = 2
   };

   WsfSensor* Clone() const override;
   bool       Initialize(double aSimTime) override;
   void       Update(double aSimTime) override;

   size_t      GetEM_RcvrCount() const override;
   WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const override;

   static int GetCenterFreqSize() { return 24; }

   static double GetCenterFreq(int aIndex) { return mCenterFrequency[aIndex]; }

protected:
   // Default copy constructor (for Clone())
   // WsfAcousticSensor(const WsfAcousticSensor& aSrc);

   // vector defining standard 1/3-octave band center frequency
   static const double mCenterFrequency[24];

   //! A mode of the sensor.
   class AcousticMode : public WsfSensorMode
   {
   public:
      AcousticMode(WsfScenario& aScenario);
      AcousticMode(const AcousticMode& aSrc);
      AcousticMode& operator=(const AcousticMode&) = delete;
      ~AcousticMode() override;

      WsfMode* Clone() const override;
      bool     Initialize(double aSimTime) override;
      bool     ProcessInput(UtInput& aInput) override;

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;
      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      double ApplyFilterWeighting(int          aBand,
                                  WsfPlatform* aTargetPtr = nullptr,
                                  const int    aFlag      = 0,
                                  const double aDoppler   = 0.0);

      double ComputeDopplerTerm(const WsfSensorResult& aResult);

      void ComputeIncidenceAngle(WsfSensorResult* aResultPtr, double& aAngle, double& aRange, double* aLoc);

      double AtmosphericAttenuation(const WsfSensorResult& aResult, double aFreq);

      double ComputeProbabilityOfDetection(double aSignal, double aNoise, double aThreshold);

      double GroundEffectAttenuation(WsfSensorResult* aResult, const double aFreq, const int aBandIndex);

      WsfEM_Antenna mAntenna;
      WsfEM_Rcvr    mRcvr;

      //! Signal-To-Noise required for detection
      double mDetectionThreshold;

      bool mVerbose;

      //! Atmosphere model.
      UtAtmosphere mAtmosphere;

      AcousticTypes mSensorType;

      //! Array of effective filter bandwidths for each sensor
      std::vector<double> mEffectiveFilterBandwidth;

      //! Pointer to the threshold table
      WsfAcousticSignature* mThresholdPtr;

      WsfStringId cDEFAULT_ID;

      //! Pointer to the background noise state
      WsfStringId mBackgroundNoiseStateId;

      //! Pointer to terrain
      wsf::Terrain* mTerrainPtr;
   };

   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<AcousticMode*> mAcousticModeList;

   struct SharedData;

   SharedData* mSharePtr;
};

#endif
