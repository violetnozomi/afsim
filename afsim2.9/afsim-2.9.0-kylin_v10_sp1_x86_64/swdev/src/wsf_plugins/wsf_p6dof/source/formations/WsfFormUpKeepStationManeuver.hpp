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

#ifndef WSFFORMUPKEEPSTATIONMANEUVER_HPP
#define WSFFORMUPKEEPSTATIONMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "maneuvers/WsfRelativeManeuver.hpp"

class WsfFormation;

//! A maneuver used by members of a formation to attain and keep station during formation flight.
//!
//! This relative maneuver operates in three states: Form Up, Keep Station and Pursue. During
//! Form Up, the formation member approaches their station. During Keep Station, the formation
//! member settles into its station, and maintains station relative to its leader. During Pursue,
//! the formation member will fly onto the turn circle of its leader in cases where the required
//! speed to keep station would be too large or too small. See the implementation of the various
//! states for more details.
class WSF_P6DOF_EXPORT WsfFormUpKeepStationManeuver : public WsfRelativeManeuver
{
public:
   WsfFormUpKeepStationManeuver();
   WsfFormUpKeepStationManeuver(const WsfFormUpKeepStationManeuver& aOther);
   WsfFormUpKeepStationManeuver(WsfFormUpKeepStationManeuver&& aOther) = delete;
   ~WsfFormUpKeepStationManeuver() override                            = default;

   WsfFormUpKeepStationManeuver& operator=(const WsfFormUpKeepStationManeuver& aOther) = delete;
   WsfFormUpKeepStationManeuver& operator=(WsfFormUpKeepStationManeuver&& aOther) = delete;

   WsfFormUpKeepStationManeuver* Clone() const override { return new WsfFormUpKeepStationManeuver{*this}; }

   // Note: There is no override of GetScriptClassName because this maneuver is not intended to be
   // used directly in script. Instead, these are assigned via formation commands.

   bool UpdateData() override;

   const char* Type() const override { return "FORM-UP-KEEP-STATION"; }

   WsfFormation* GetFormation() const;
   void          SetFormation(const std::string& aFormationName);

   double GetSpeedRangeFactor() const { return mSpeedRangeFactor; }
   void   SetSpeedRangeFactor(double aFactor) { mSpeedRangeFactor = aFactor; }

private:
   std::string mFormationName{};
   double      mSpeedRangeFactor{0.1}; //! Allowed factor above or below the leader's speed for this maneuver.
};

#endif // WSFFORMUPKEEPSTATIONMANEUVER_HPP
