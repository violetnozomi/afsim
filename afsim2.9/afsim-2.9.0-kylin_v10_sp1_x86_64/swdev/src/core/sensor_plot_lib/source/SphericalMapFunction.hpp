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

#ifndef SPHERICALMAPFUNCTION_HPP
#define SPHERICALMAPFUNCTION_HPP

#include <string>
#include <vector>

#include "MapPlotFunction.hpp"
#include "Sensor.hpp"
#include "Target.hpp"

class SphericalMapFunction : public MapPlotFunction
{
public:
   //! The list of values at a specified sample point
   using Point = std::vector<double>;

   SphericalMapFunction(const WsfScenario& aScenario);

   bool Execute(WsfSimulation& aSimulation) override;
   bool ProcessInput(UtInput& aInput) override;

private:
   void WriteGnuPlotFile(const std::vector<double>& aRowValues,
                         const std::vector<double>& aColValues,
                         const std::vector<Point>&  aVarValues);

   void WritePdMapFile(const std::vector<double>& aRowValues,
                       const std::vector<double>& aColValues,
                       const std::vector<Point>&  aVarValues,
                       size_t                     aVarIndex);

   std::string GetPdMapFileJsonMetadata() const;

   WsfSimulation* mSimulationPtr;

   Sensor mSensor;
   double mSensorPlatformYaw;
   double mSensorPlatformPitch;
   double mSensorPlatformRoll;
   double mSensorPlatformAltitude;
   Target mTarget;

   double mAzMin;
   double mAzMax;
   double mAzStep;
   double mElMin;
   double mElMax;
   double mElStep;
   double mRange;
   double mAltitude;

   std::string mPdMapFile;
   std::string mHeaderLine1;
   std::string mHeaderLine2;
   std::string mHeaderLine3;
   size_t      mOutputColumnLimit;
   std::string mGnuPlotFile;

   //! If 'true', the sensor position is fixed and the target is moved over the grid.
   //! If 'false', the target position is fixed and the sensor is moved over the grid
   bool mFixedSensorPosition;
};

#endif
