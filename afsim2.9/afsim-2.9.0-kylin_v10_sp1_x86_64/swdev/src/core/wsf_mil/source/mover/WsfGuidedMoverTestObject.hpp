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

#ifndef WSFGUIDEDMOVERTESTOBJECT_HPP
#define WSFGUIDEDMOVERTESTOBJECT_HPP


#include "wsf_mil_export.h"

class WsfGuidedMover;

class WSF_MIL_EXPORT WsfGuidedMoverTestObject
{
public:
   //! Constructor
   explicit WsfGuidedMoverTestObject(const WsfGuidedMover& aParentMover);

   // This returns the horizontal thrust (in lbs) produced in level flight at the specified
   // altitude (ft) and speed (Mach)
   double GetLevelFlightThrust_lbs(double aSimTime, double aAltitude_ft, double aMach) const;

   // This returns the drag (in lbs) when in level flight at the specified altitude (ft) and
   // speed (Mach) with the specified gross weight.
   double GetLevelFlightDrag_lbs(double aAltitude_ft, double aMach, double aGrossWeight_lbs) const;

   // This returns the drag (in lbs) when in level flight at the specified altitude (ft) and
   // speed (Mach) with the specified gross weight.
   double GetLevelFlightFuelBurnRate_lbsphr(double aSimTime, double aAltitude_ft, double aMach, double aGrossWeight_lbs) const;

private:
   const WsfGuidedMover& mParentMover;
};


#endif
