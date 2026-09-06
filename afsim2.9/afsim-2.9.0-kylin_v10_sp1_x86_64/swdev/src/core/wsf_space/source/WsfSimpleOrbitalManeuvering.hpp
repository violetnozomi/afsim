// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSIMPLEORBITALMANEUVERING_HPP
#define WSFSIMPLEORBITALMANEUVERING_HPP

#include "wsf_space_export.h"

#include "WsfOrbitalManeuvering.hpp"

//! Orbital maneuvering using a specified delta-V budget and constant acceleration.
//! This type of orbital maneuvering is applicable to an ion thruster.
class WSF_SPACE_EXPORT WsfSimpleOrbitalManeuvering : public WsfOrbitalManeuvering
{
public:
   WsfSimpleOrbitalManeuvering() = default;
   WsfSimpleOrbitalManeuvering(const WsfSimpleOrbitalManeuvering& aSrc);
   WsfSimpleOrbitalManeuvering& operator=(const WsfSimpleOrbitalManeuvering&) = delete;
   ~WsfSimpleOrbitalManeuvering() override;

   bool                   ProcessInput(UtInput& aInput) override;
   WsfOrbitalManeuvering* Clone() const override { return new WsfSimpleOrbitalManeuvering(*this); }

   double GetAvailableDeltaV() const override;
   double GetRequiredDuration(double aDeltaV) const override;
   double GetRequiredDeltaV(double aDuration) const override;
   bool   ReduceAvailableDeltaV_By(double aDeltaV) override;

private:
   static constexpr double cDEFAULT_DELTA_V_BUDGET = 1.0e12;
   double                  mDeltaV{cDEFAULT_DELTA_V_BUDGET};
   double                  mDeltaV_Rate{1000.0};
};

#endif
