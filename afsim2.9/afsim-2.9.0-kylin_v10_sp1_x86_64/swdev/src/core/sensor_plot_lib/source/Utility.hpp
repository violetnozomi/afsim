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

#ifndef UTILITY_HPP
#define UTILITY_HPP

class WsfPlatform;
class WsfSensor;

namespace Utility
{
void ConvertRBA_ToWCS(WsfPlatform* aPlatformPtr,
                      double       aGroundRange,
                      double       aBearing,
                      double       aAltitude,
                      double       aEarthRadiusMultiplier,
                      double       aLocationWCS[3]);

void CueSensorToTarget(WsfSensor* aSensorPtr, WsfPlatform* aTargetPtr);

double GetDetectionReference(WsfSensor* aSensorPtr, double aDefaultThreshold);

} // namespace Utility

#endif
