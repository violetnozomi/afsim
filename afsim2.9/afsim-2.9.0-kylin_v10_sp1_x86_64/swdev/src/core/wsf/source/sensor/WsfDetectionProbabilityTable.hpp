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

#ifndef WSPROBABILITYTABLE_HPP
#define WSPROBABILITYTABLE_HPP

#include "wsf_export.h"

#include "TblLookup.hpp"
class UtInput;
#include "UtMath.hpp"
#include "WsfSensorDetector.hpp"
class WsfSensorMode;

namespace wsf
{

//! Computation of probability of detection using a probability detection table.
class WSF_EXPORT DetectionProbabilityTable : public SensorDetector
{
public:
   DetectionProbabilityTable();
   DetectionProbabilityTable operator=(const DetectionProbabilityTable& aRhs) = delete;
   virtual ~DetectionProbabilityTable()                                       = default;

   DetectionProbabilityTable* Clone() const override;

   bool Initialize(double aSimTime, WsfSensorMode* aModePtr, size_t aBeamIndex = 0) override;
   bool ProcessInput(UtInput& aInput) override;

   void ReadTable(UtInput& aInput); // TODO-AWK needs to be public yet due to WsfRadarSensor

   double ComputeProbabilityOfDetection(double aSignalToNoise,
                                        double aDetectionThreshold = UtMath::DB_ToLinear(3.0)) override;

   static SensorDetector* ObjectFactory(const std::string& aTypeName);

private:
   TblIndVarU<double> mSignalTable;
   TblDepVar1<double> mPdTable;
};

} // namespace wsf

#endif
