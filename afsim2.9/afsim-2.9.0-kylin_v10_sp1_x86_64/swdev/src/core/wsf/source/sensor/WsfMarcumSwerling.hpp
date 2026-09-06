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

#ifndef WSFMARCUMSWERLING_HPP
#define WSFMARCUMSWERLING_HPP

#include "wsf_export.h"

#include <string>

class UtInput;
#include "UtMath.hpp"
#include "WsfSensorDetector.hpp"
class WsfSensorMode;

namespace wsf
{

//! Computation of probability of detection using the Marcum-Swerling algorithm.
class WSF_EXPORT MarcumSwerling : public SensorDetector
{
public:
   enum DetectorLaw
   {
      cDL_LINEAR,
      cDL_SQUARE,
      cDL_LOG
   };

   MarcumSwerling();
   ~MarcumSwerling() override = default;

   MarcumSwerling* Clone() const override;

   bool Initialize(double aSimTime, WsfSensorMode* aModePtr, size_t aBeamIndex = 0) override;
   bool ProcessInput(UtInput& aInput) override;

   double ComputeProbabilityOfDetection(double aSignalToNoise,
                                        double aDetectionThreshold = UtMath::DB_ToLinear(3.0)) override;

   void SetCase(int aCase);
   int  GetCase() const { return mCase; }

   void        SetDetectorLaw(DetectorLaw aDetectorLaw);
   DetectorLaw GetDetectorLaw() const { return mDetectorLaw; }

   void SetNumberOfPulsesIntegrated(int aNumberOfPulsesIntegrated);
   int  GetNumberOfPulsesIntegrated() const { return mNumberOfPulsesIntegrated; }

   void   SetProbabilityOfFalseAlarm(double aProbabilityOfFalseAlarm);
   double GetProbabilityOfFalseAlarm() const { return mProbabilityOfFalseAlarm; }

   static SensorDetector* ObjectFactory(const std::string& aTypeName);

private:
   void ComputeConstants();

   // Input
   int         mCase;
   int         mNumberOfPulsesIntegrated;
   double      mProbabilityOfFalseAlarm;
   DetectorLaw mDetectorLaw;

   // Calculated / Flags
   bool   mComputeConstants;
   double mBase;
   double mExp;
};

} // namespace wsf

#endif
