// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFALARM_ANTENNAPATTERNBANDED_HPP
#define WSFALARM_ANTENNAPATTERNBANDED_HPP

#include "UtInput.hpp"
#include "WsfALARM_AntennaPattern.hpp"
#include "WsfAntennaPattern.hpp"

//! This class works with a propagation table to prevent double-counting,
//! or reapply any antenna effects that already rolled into the propagation table.
//! Simply said, when the propagation table is in its usable range and being applied,
//! this class returns its maximum gain for the given antenna.
//! Otherwise, the returned gain is the actual/normal gain.

class WsfALARM_AntennaPatternBanded : public WsfALARM_AntennaPattern
{
public:
   WsfALARM_AntennaPatternBanded();
   ~WsfALARM_AntennaPatternBanded() override = default;

   WsfAntennaPattern* Clone() const override;

   static WsfAntennaPattern* ObjectFactory(const std::string& aTypeName);

   bool ProcessInput(UtInput& aInput) override;

   double GetGain(double aFrequency, double aTargetAz, double aTargetEl, double aEBS_Az, double aEBS_El) override;

protected:
   WsfALARM_AntennaPatternBanded(const WsfALARM_AntennaPatternBanded& aSrc) = default;
   WsfALARM_AntennaPatternBanded& operator=(const WsfALARM_AntennaPatternBanded& aRhs) = delete;

   double mMaxElevationRad;
};

#endif
