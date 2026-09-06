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
// ****************************************************************************

#ifndef RADARLOOKUPTABLEFUNCTION_HPP
#define RADARLOOKUPTABLEFUNCTION_HPP

#include <string>
#include <vector>

#include "Function.hpp"
#include "Sensor.hpp"
#include "Target.hpp"

class RadarLookupTableFunction : public Function
{
public:
   explicit RadarLookupTableFunction(const WsfScenario& aScenario);

   bool Execute(WsfSimulation& aSimulation) override;
   bool ProcessInput(UtInput& aInput) override;

private:
   void WriteFile(const std::string&         aFileName,
                  const std::vector<double>& aRowValues,
                  const std::vector<double>& aColValues,
                  const std::vector<double>& aDataValues);

   Sensor mSensor;
   Target mTarget;

   std::vector<double> mAltitudes;
   std::vector<double> mRanges;
   std::string         mAltUnitsStr;
   double              mAltUnitsScale;
   std::string         mRangeUnitsStr;
   double              mRangeUnitsScale;

   std::string mOutputName;
   std::string mHeaderLine1;
   std::string mHeaderLine2;
   std::string mHeaderLine3;
   size_t      mOutputColumnLimit;
};

#endif
