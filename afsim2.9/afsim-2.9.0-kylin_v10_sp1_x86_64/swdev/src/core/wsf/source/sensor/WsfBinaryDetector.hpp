// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFBINARYDETECTOR_HPP
#define WSFBINARYDETECTOR_HPP

#include "wsf_export.h"

#include <string>

class UtInput;
#include "UtMath.hpp"
#include "WsfSensorDetector.hpp"
class WsfSensorMode;

namespace wsf
{
//! Computation of probability of detection using the binary detection algorithm.
class WSF_EXPORT BinaryDetector : public SensorDetector
{
public:
   BinaryDetector();
   ~BinaryDetector() override = default;

   BinaryDetector* Clone() const override;

   double ComputeProbabilityOfDetection(double aSignalToNoise,
                                        double aDetectionThreshold = UtMath::DB_ToLinear(3.0)) override;

   static wsf::SensorDetector* ObjectFactory(const std::string& aTypeName);
};

} // namespace wsf

#endif
