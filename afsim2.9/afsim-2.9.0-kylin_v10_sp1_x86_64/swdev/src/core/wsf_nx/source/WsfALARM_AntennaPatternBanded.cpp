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

#include "WsfALARM_AntennaPatternBanded.hpp"

#include <cfloat>

WsfALARM_AntennaPatternBanded::WsfALARM_AntennaPatternBanded()
   : WsfALARM_AntennaPattern(nullptr)
   , mMaxElevationRad(DBL_MAX)
{
}

// virtual
WsfAntennaPattern* WsfALARM_AntennaPatternBanded::Clone() const
{
   return new WsfALARM_AntennaPatternBanded(*this);
}

//! Factory method for WsfAntennaPatternTypes.
//! This is called by WsfAntennaPatternTypes::LoadType to determine if a pattern
//! represented by this class is being requested.
// static
WsfAntennaPattern* WsfALARM_AntennaPatternBanded::ObjectFactory(const std::string& aTypeName)
{
   WsfAntennaPattern* patternPtr = nullptr;
   if (aTypeName == "alarm_pattern_banded")
   {
      patternPtr = new WsfALARM_AntennaPatternBanded();
   }
   return patternPtr;
}

// virtual
bool WsfALARM_AntennaPatternBanded::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "elevation_maximum")
   {
      aInput.ReadValueOfType(mMaxElevationRad, UtInput::cANGLE);
   }
   else
   {
      // myCommand = WsfAntennaPattern::ProcessInput(aInput);
      myCommand = WsfALARM_AntennaPattern::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
double WsfALARM_AntennaPatternBanded::GetGain(double aFrequency, double aTargetAz, double aTargetEl, double aEBS_Az, double aEBS_El)
{
   return aTargetEl > mMaxElevationRad ?
             WsfALARM_AntennaPattern::GetGain(aFrequency, aTargetAz, aTargetEl, aEBS_Az, aEBS_El) :
             WsfALARM_AntennaPattern::GetGain(aFrequency, 0.0, 0.0, 0.0, 0.0);
   ////: WsfALARM_AntennaPattern::GetPeakGain(aFrequency);
}
