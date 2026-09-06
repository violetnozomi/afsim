// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "EventOrbitDetails.hpp"

namespace wsfg
{

namespace spaceg
{

//! Return true if this event is one for which there is no change in the orbit.
bool EventOrbitDetails::IsNoDeltaEvent() const
{
   bool retval{false};
   if (IsValid())
   {
      retval = mElementsBefore.GetEccentricity() == mElementsAfter.GetEccentricity() &&
               mElementsBefore.GetInclination() == mElementsAfter.GetInclination() &&
               mElementsBefore.GetRAAN() == mElementsAfter.GetRAAN() &&
               mElementsBefore.GetSemiMajorAxis() == mElementsAfter.GetSemiMajorAxis() &&
               mElementsBefore.GetArgumentOfPeriapsis() == mElementsAfter.GetArgumentOfPeriapsis();
   }
   return retval;
}

} // namespace spaceg

} // namespace wsfg
