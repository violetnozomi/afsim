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

#ifndef WSF_SIXDOF_FORMUPKEEPSTATIONMANEUVER_HPP
#define WSF_SIXDOF_FORMUPKEEPSTATIONMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "maneuvers/WsfSixDOF_RelativeManeuver.hpp"

namespace wsf
{
namespace six_dof
{
class Formation;

//! A maneuver used by members of a formation to attain and keep station during formation flight.
//!
//! This relative maneuver operates in three states: Form Up, Keep Station and Pursue. During
//! Form Up, the formation member approaches their station. During Keep Station, the formation
//! member settles into its station, and maintains station relative to its leader. During Pursue,
//! the formation member will fly onto the turn circle of its leader in cases where the required
//! speed to keep station would be too large or too small. See the implementation of the various
//! states for more details.
class WSF_SIX_DOF_EXPORT FormUpKeepStationManeuver : public RelativeManeuver
{
public:
   FormUpKeepStationManeuver();
   FormUpKeepStationManeuver(const FormUpKeepStationManeuver& aOther);
   FormUpKeepStationManeuver(FormUpKeepStationManeuver&& aOther) = delete;
   ~FormUpKeepStationManeuver() override                         = default;

   FormUpKeepStationManeuver& operator=(const FormUpKeepStationManeuver& aOther) = delete;
   FormUpKeepStationManeuver& operator=(FormUpKeepStationManeuver&& aOther) = delete;

   FormUpKeepStationManeuver* Clone() const override { return new FormUpKeepStationManeuver{*this}; }

   // Note: There is no override of GetScriptClassName because this maneuver is not intended to be
   // used directly in script. Instead, these are assigned via formation commands.

   bool UpdateData() override;

   const char* Type() const override { return "FORM-UP-KEEP-STATION"; }

   Formation* GetFormation() const;
   void       SetFormation(const std::string& aFormationName);

   double GetSpeedRangeFactor() const { return mSpeedRangeFactor; }
   void   SetSpeedRangeFactor(double aFactor) { mSpeedRangeFactor = aFactor; }

private:
   std::string mFormationName{};
   double      mSpeedRangeFactor{0.1}; //! Allowed factor above or below the leader's speed for this maneuver.
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMUPKEEPSTATIONMANEUVER_HPP
