// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfGuidedMoverTestObject.hpp"

#include "UtMath.hpp"
#include "WsfGuidedMover.hpp"

WsfGuidedMoverTestObject::WsfGuidedMoverTestObject(const WsfGuidedMover& aParentMover)
   : mParentMover(aParentMover)
{
}

double WsfGuidedMoverTestObject::GetLevelFlightThrust_lbs(double aSimTime, double aAltitude_ft, double aMach) const
{
   return UtMath::cLB_PER_NT * mParentMover.GetLevelFlightThrust(aSimTime, aAltitude_ft * UtMath::cM_PER_FT, aMach);
}

double WsfGuidedMoverTestObject::GetLevelFlightDrag_lbs(double aAltitude_ft, double aMach, double aGrossWeight_lbs) const
{
   return UtMath::cLB_PER_NT *
          mParentMover.GetLevelFlightDrag(aAltitude_ft * UtMath::cM_PER_FT, aMach, aGrossWeight_lbs * UtMath::cNT_PER_LB);
}

double WsfGuidedMoverTestObject::GetLevelFlightFuelBurnRate_lbsphr(double aSimTime,
                                                                   double aAltitude_ft,
                                                                   double aSpeed_kts,
                                                                   double aGrossWeight_lbs) const
{
   return 3600.0 * UtMath::cLB_PER_KG *
          mParentMover.GetLevelFlightFuelBurnRate(aSimTime,
                                                  aAltitude_ft * UtMath::cM_PER_FT,
                                                  aSpeed_kts * UtMath::cMPS_PER_NMPH,
                                                  aGrossWeight_lbs * UtMath::cNT_PER_LB);
}
