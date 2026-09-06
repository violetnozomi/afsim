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

#ifndef VERTICALMAPFUNCTION_HPP
#define VERTICALMAPFUNCTION_HPP

#include <string>
#include <vector>

#include "MapPlotFunction.hpp"
#include "Sensor.hpp"
#include "Target.hpp"

class VerticalMapFunction : public MapPlotFunction
{
public:
   //! The list of values at a specified sample point
   using Point = std::vector<double>;

   static const double cUNDEFINED;

   explicit VerticalMapFunction(const WsfScenario& aScenario);

   bool Execute(WsfSimulation& aSimulation) override;
   bool ProcessInput(UtInput& aInput) override;

private:
   void WriteGnuPlotFile(WsfSimulation&             aSimulation,
                         const std::vector<double>& aRowValues,
                         const std::vector<double>& aColValues,
                         const std::vector<Point>&  aVarValues);

   void WriteGnuPlotPlayerFile(WsfSimulation& aSimulation);

   void WritePdMapFile(const std::vector<double>& aRowValues,
                       const std::vector<double>& aColValues,
                       const std::vector<Point>&  aVarValues,
                       size_t                     aVarIndex);

   std::string GetPdMapFileJsonMetadata() const;

   WsfSimulation* mSimulationPtr;

   Sensor mSensor;
   Target mTarget;

   double      mGroundRangeMin;
   double      mGroundRangeMax;
   double      mGroundRangeStep;
   std::string mGroundRangeUnits;
   double      mAltitudeMin;
   double      mAltitudeMax;
   double      mAltitudeStep;
   std::string mAltitudeUnits;

   std::string mPdMapFile;
   std::string mHeaderLine1;
   std::string mHeaderLine2;
   std::string mHeaderLine3;
   size_t      mOutputColumnLimit;
   std::string mGnuPlotFile;
   std::string mGnuPlotPlayerFile;

   //! If 'true', the sensor position is fixed and the target is moved over the grid.
   //! If 'false', the target position is fixed and the sensor is moved over the grid
   bool mFixedSensorPosition;
};

#endif
