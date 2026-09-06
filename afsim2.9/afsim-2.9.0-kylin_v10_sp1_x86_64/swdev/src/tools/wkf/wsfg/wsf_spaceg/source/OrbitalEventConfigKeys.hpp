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

#ifndef ORBITALEVENTCONFIGKEYS_HPP
#define ORBITALEVENTCONFIGKEYS_HPP

#include <map>

#include <QString>

#include "WsfOrbitalEvent.hpp"

namespace wsfg
{
namespace spaceg
{
namespace EventConfigKeys
{
const QString cCONDITION          = QString{"condition"};
const QString cOPTION             = QString{"option"};
const QString cORBIT              = QString{"orbit"};
const QString cRELATIVE_TIME      = QString{"relative_time"};
const QString cRADIUS             = QString{"radius"};
const QString cFINITE             = QString{"finite"};
const QString cDURATION           = QString{"duration"};
const QString cUPDATE_INTERVAL    = QString{"update_interval"};
const QString cECCENTRICITY       = QString{"eccentricity"};
const QString cINCLINATION        = QString{"inclination"};
const QString cRAAN               = QString{"raan"};
const QString cPLATFORM           = QString{"platform"};
const QString cSEMI_MAJOR_AXIS    = QString{"semi_major_axis"};
const QString cAPOAPSIS_ALTITUDE  = QString{"apoapsis_altitude"};
const QString cPERIAPSIS_ALTITUDE = QString{"periapsis_altitude"};
const QString cFINAL_SMA          = QString{"final_semi_major_axis"};
const QString cFINAL_RADIUS       = QString{"final_radius"};
const QString cDELTA_V            = QString{"delta_v"};
const QString cDELTA_T            = QString{"delta_t"};
const QString cOFFSET_X           = QString{"offset_x"};
const QString cOFFSET_Y           = QString{"offset_y"};
const QString cOFFSET_Z           = QString{"offset_z"};
const QString cPOSITION_OFFSET    = QString{"position_offset"};
const QString cVELOCITY_OFFSET    = QString{"velocity_offset"};
const QString cFRAME              = QString{"frame"};
const QString cIS_COMPLETE        = QString{"is_complete"};
const QString cON_COMPLETE        = QString{"on_complete"};
const QString cON_UPDATE          = QString{"on_update"};
const QString cON_INITIALIZE      = QString{"on_initialize"};
const QString cDELTA_TIME         = QString{"delta_time"};
const QString cMAXIMUM_DELTA_TIME = QString{"maximum_delta_time"};
const QString cMAXIMUM_DELTA_V    = QString{"maximum_delta_v"};
const QString cOPTIMIZE           = QString{"optimize"};
const QString cDV_X               = QString{"dv_x"};
const QString cDV_Y               = QString{"dv_y"};
const QString cDV_Z               = QString{"dv_z"};
const QString cTARGET             = QString{"target"};
const QString cTRACK              = QString{"track"};
const QString cORIENTATION        = QString{"orientation"};
const QString cGEO_POINT          = QString{"geo_point"};
const QString cTOLERANCE          = QString{"tolerance"};
const QString cLIBRATION_SYSTEM   = QString{"libration_system"};
const QString cLIBRATION_POINT    = QString{"libration_point"};
const QString cORBIT_SIZE         = QString{"orbit_size"};
const QString cORBIT_PHASE        = QString{"orbit_phase"};
const QString cOOP_AMPLITUDE      = QString{"oop_amplitude"};
const QString cOOP_PHASE          = QString{"oop_phase"};
const QString cALLOW_LP_TARGET    = QString{"allow_libration_point"};
const QString cDRIFT_RATE         = QString{"drift_rate"};
const QString cOFFSET_TIME        = QString{"offset_time"};
const QString cLAG_TIME           = QString{"lag_time"};
const QString cR_AT_POCA          = QString{"r_at_poca"};
const QString cPERIOD             = QString{"period"};
const QString cTIME_TO_POCA       = QString{"time_to_poca"};
const QString cREPETITIONS        = QString{"repetitions"};
const QString cKINEMATIC_STATE    = QString{"kinematic_state"};
const QString cSTATE_POSITION     = QString{"state_position"};
const QString cSTATE_VELOCITY     = QString{"state_velocity"};
const QString cX_COMPONENT        = QString{"x_component"};
const QString cY_COMPONENT        = QString{"y_component"};
const QString cZ_COMPONENT        = QString{"z_component"};
const QString cCOST_OPTIMIZATION  = QString{"cost_optimization"};
const QString cCOST_SELECTION     = QString{"cost_selection"};
const QString cBLENDED_COST       = QString{"blended_cost"};
const QString cA_PARAMETER        = QString{"a_parameter"};
const QString cB_PARAMETER        = QString{"b_parameter"};
const QString cC_PARAMETER        = QString{"c_parameter"};
const QString cNONE               = QString{"none"};

const std::map<QString, std::string> cCONDITION_STRING_TO_TYPE = {
   {QString{"NONE"}, wsf::space::NoneCondition::cTYPE},
   {QString{"AT_PERIAPSIS"}, wsf::space::PeriapsisCondition::cTYPE},
   {QString{"AT_APOAPSIS"}, wsf::space::ApoapsisCondition::cTYPE},
   {QString{"AT_ASCENDING_NODE"}, wsf::space::AscendingNodeCondition::cTYPE},
   {QString{"AT_DESCENDING_NODE"}, wsf::space::DescendingNodeCondition::cTYPE},
   {QString{"AT_RELATIVE_TIME"}, wsf::space::RelativeTimeCondition::cTYPE},
   {QString{"AT_ASCENDING_RADIUS"}, wsf::space::AscendingRadiusCondition::cTYPE},
   {QString{"AT_DESCENDING_RADIUS"}, wsf::space::DescendingRadiusCondition::cTYPE},
   {QString{"AT_NORTHERN_INTERSECTION"}, wsf::space::NorthernIntersectionCondition::cTYPE},
   {QString{"AT_SOUTHERN_INTERSECTION"}, wsf::space::SouthernIntersectionCondition::cTYPE},
   {QString{"AT_ECLIPSE_ENTRY"}, wsf::space::EclipseEntryCondition::cTYPE},
   {QString{"AT_ECLIPSE_EXIT"}, wsf::space::EclipseExitCondition::cTYPE}};

} // namespace EventConfigKeys

} // namespace spaceg

} // namespace wsfg

#endif // ORBITALEVENTCONFIGKEYS_HPP
