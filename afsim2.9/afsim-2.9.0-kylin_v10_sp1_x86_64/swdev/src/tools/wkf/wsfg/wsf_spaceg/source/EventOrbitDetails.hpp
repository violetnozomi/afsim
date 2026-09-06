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

#ifndef EVENTORBITDETAILS_HPP
#define EVENTORBITDETAILS_HPP

#include "wsf_spaceg_export.h"

#include "UtCalendar.hpp"
#include "UtCentralBody.hpp"
#include "UtColor.hpp"
#include "UtEarth.hpp"
#include "UtOrbitalElements.hpp"

namespace wsfg
{

namespace spaceg
{

class WSF_SPACEG_EXPORT EventOrbitDetails
{
public:
   EventOrbitDetails()
      : mElementsBefore{ut::EarthWGS84()}
      , mElementsAfter{ut::EarthWGS84()}
      , mSimTimeBefore{-1.0}
      , mSimTimeAfter{-1.0}
      , mPreviewColor{1.0f, 1.0f, 1.0f}
      , mEventName{"<unknown>"}
   {
   }

   bool IsValid() const { return mElementsBefore.IsValid() && mElementsAfter.IsValid(); }

   bool IsNoDeltaEvent() const;

   UtOrbitalElements mElementsBefore;
   UtOrbitalElements mElementsAfter;
   double            mSimTimeBefore;
   double            mSimTimeAfter;
   UtColor           mPreviewColor;
   std::string       mEventName;
};

} // namespace spaceg

} // namespace wsfg

#endif // EVENTORBITDETAILS_HPP
