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

#ifndef ANTENNAPLOTFUNCTION_HPP
#define ANTENNAPLOTFUNCTION_HPP

#include <string>
#include <vector>

#include "Function.hpp"
#include "WsfEM_Antenna.hpp"
class WsfEM_Rcvr;
#include "WsfEM_Types.hpp"

class AntennaPlotFunction : public Function
{
public:
   explicit AntennaPlotFunction(WsfScenario& aScenario);

   bool Execute(WsfSimulation& aSimulation) override;
   bool ProcessInput(UtInput& aInput) override;

private:
   bool PlotBoth(WsfEM_Rcvr* rcvrPtr);
   bool PlotHorizontal(WsfEM_Rcvr* rcvrPtr);
   bool PlotVertical(WsfEM_Rcvr* rcvrPtr);

   void WritePlotFile(const std::string&         aFileName,
                      const std::vector<double>& aRowValues,
                      const std::vector<double>& aColValues,
                      const std::vector<double>& aDataValues);

   void WriteGnuPlotFile(const std::vector<double>& aRowValues,
                         const std::vector<double>& aColValues,
                         const std::vector<double>& aDataValues);

   WsfSimulation*            mSimulationPtr;
   std::string               mPatternName;
   std::string               mOutputFile;
   std::string               mGnuPlotFile;
   std::string               mHeaderLine1;
   std::string               mHeaderLine2;
   std::string               mHeaderLine3;
   size_t                    mOutputColumnLimit;
   char                      mPlotType;
   double                    mAzimuthMin;
   double                    mAzimuthMax;
   double                    mAzimuthStep;
   double                    mElevationMin;
   double                    mElevationMax;
   double                    mElevationStep;
   double                    mTiltAngle;
   double                    mFrequency;
   WsfEM_Types::Polarization mPolarization;
   WsfEM_Antenna::EBS_Mode   mEBS_Mode;
   double                    mEBS_AzCosSteeringLimit;
   double                    mEBS_ElCosSteeringLimit;
   double                    mEBS_AzLossExponent;
   double                    mEBS_ElLossExponent;
   double                    mEBS_Az;
   double                    mEBS_El;
};

#endif
