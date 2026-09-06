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

#include "WsfBinaryDetector.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

#include "UtInput.hpp"
#include "WsfSensorMode.hpp"

namespace wsf
{

BinaryDetector::BinaryDetector()
   : SensorDetector()
{
}

BinaryDetector* BinaryDetector::Clone() const
{
   return new BinaryDetector(*this);
}

//! Compute the probability of detection using binary detection threshold.
//! @param aSignalToNoise      [input] Absolute signal-to-noise ratio
//! @param aDetectionThreshold [input] Absolute detection threshold (optional)
//! @return The probability of detection [0 or 1]
double BinaryDetector::ComputeProbabilityOfDetection(double aSignalToNoise,
                                                     double aDetectionThreshold /* = UtMath::DB_ToLinear(3.0)*/)
{
   double pd = 1.0;
   if (aSignalToNoise < aDetectionThreshold)
   {
      pd = 0.0;
   }
   return pd;
}

// ================================================================================================
//! The object factory for the Binary Detector type.
// static
SensorDetector* BinaryDetector::ObjectFactory(const std::string& aTypeName)
{
   wsf::SensorDetector* instancePtr = nullptr;
   if (aTypeName == "marcum_swerling")
   {
      instancePtr = new BinaryDetector();
   }
   return instancePtr;
}

} // namespace wsf
