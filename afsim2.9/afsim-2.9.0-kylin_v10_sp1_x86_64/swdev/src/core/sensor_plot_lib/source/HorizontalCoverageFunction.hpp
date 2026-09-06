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

#ifndef HORIZONTALCOVERAGEFUNCTION_HPP
#define HORIZONTALCOVERAGEFUNCTION_HPP

#include <string>

#include "Function.hpp"
#include "Sensor.hpp"
#include "Target.hpp"

class HorizontalCoverageFunction : public Function
{
public:
   static const double cUNDEFINED;

   explicit HorizontalCoverageFunction(WsfScenario& aScenario);

   bool Execute(WsfSimulation& aSimulation) override;
   bool ProcessInput(UtInput& aInput) override;

private:
   WsfSimulation* mSimulationPtr;
   bool           mOutputMinRange;
   bool           mOutputMaxRange;

   Sensor mSensor;
   Target mTarget;

   std::string         mOutputBase;
   std::string         mOutputFile;
   double              mMinimumRange;
   double              mMaximumRange;
   double              mLargeRangeStep;
   double              mSmallRangeStep;
   double              mMinAzimuth;
   double              mMaxAzimuth;
   double              mAzimuthStep;
   std::vector<double> mAltValues;
   std::string         mAltUnitsStr;
   std::string         mRangeUnitsStr;
   std::string         mX_UnitsStr;
   std::string         mY_UnitsStr;
};

#endif
