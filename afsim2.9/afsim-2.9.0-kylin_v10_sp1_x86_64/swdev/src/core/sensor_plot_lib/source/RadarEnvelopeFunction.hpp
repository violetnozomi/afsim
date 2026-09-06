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

#ifndef RADARENVELOPEFUNCTION_HPP
#define RADARENVELOPEFUNCTION_HPP

#include <string>
#include <vector>

#include "Function.hpp"
#include "Sensor.hpp"
#include "Target.hpp"
class WsfSensor;

class RadarEnvelopeFunction : public Function
{
public:
   explicit RadarEnvelopeFunction(const WsfScenario& aScenario);

   bool Execute(WsfSimulation& aSimulation) override;
   bool ProcessInput(UtInput& aInput) override;

private:
   double ComputeDetectionRange(double aTargetAlt);

   Sensor mSensor;
   Target mTarget;

   std::vector<double> mAltValues;
   std::vector<double> mRCS_Values;
   std::string         mAltUnitsStr;
   std::string         mRangeUnitsStr;
   std::string         mOutputName;

   double mMaximumRange;
   double mLargeRangeStep;
   double mSmallRangeStep;
};

#endif
