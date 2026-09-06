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

#ifndef VERTICALCOVERAGEFUNCTION_HPP
#define VERTICALCOVERAGEFUNCTION_HPP

#include <string>

#include "Function.hpp"
#include "Sensor.hpp"
#include "Target.hpp"

class VerticalCoverageFunction : public Function
{
public:
   static const double cUNDEFINED;

   explicit VerticalCoverageFunction(const WsfScenario& aScenario);

   bool Execute(WsfSimulation& aSimulation) override;
   bool ProcessInput(UtInput& aInput) override;

private:
   WsfSimulation* mSimulationPtr;
   bool           mOutputMaxHeight;
   bool           mOutputMaxRange;

   Sensor mSensor;
   Target mTarget;

   std::string mOutputBase;
   std::string mOutputFile;
   double      mMaximumRange;
   double      mLargeRangeStep;
   double      mSmallRangeStep;
   double      mMinElevation;
   double      mMaxElevation;
   double      mElevationStep;
   std::string mAltUnitsStr;
   std::string mRangeUnitsStr;
   std::string mX_UnitsStr;
   std::string mY_UnitsStr;
};

#endif
