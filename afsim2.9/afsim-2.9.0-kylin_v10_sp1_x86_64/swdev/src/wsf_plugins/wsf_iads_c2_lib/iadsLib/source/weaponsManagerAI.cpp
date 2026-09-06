// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: weaponsManagerAI.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#include <iadsLib/weaponsManagerAI.hpp>

namespace il
{

weaponsManagerAI::weaponsManagerAI()
   : WeaponsManagerInterface()
   , returnToCAP(false)
   , isLowOnFuel(false)
{

}

WeaponsManagerInterface* weaponsManagerAI::Clone()
{
   return new weaponsManagerAI(*this);
}

weaponsManagerAI::weaponsManagerAI(const weaponsManagerAI& from)
   : WeaponsManagerInterface(from)
   , returnToCAP(from.returnToCAP)
   , isLowOnFuel(from.isLowOnFuel)
   , m_ai_scripting_params(from.m_ai_scripting_params)
{

}

void weaponsManagerAI::setAlertTime(const double alert_time_s)
{
   m_ai_scripting_params.m_alert_time_s = alert_time_s;
}

double weaponsManagerAI::getAlertTime() const
{
   return m_ai_scripting_params.m_alert_time_s;
}

void weaponsManagerAI::setAssessEngageVia(const il::eAssessEngage assess_engage_via)
{
   m_ai_scripting_params.m_assess_engage_via = assess_engage_via;
}

il::eAssessEngage weaponsManagerAI::getAssessEngageVia() const
{
   return m_ai_scripting_params.m_assess_engage_via;
}

void weaponsManagerAI::setEscort(const bool escort)
{
   m_ai_scripting_params.m_escort = escort;
}

bool weaponsManagerAI::getEscort() const
{
   return m_ai_scripting_params.m_escort;
}

void weaponsManagerAI::setInterceptSpeed(const double speed_m_per_s)
{
   m_ai_scripting_params.m_intercept_speed_m_per_s = speed_m_per_s;
}

double weaponsManagerAI::getInterceptSpeed() const
{
   return m_ai_scripting_params.m_intercept_speed_m_per_s;
}

void weaponsManagerAI::setPlaceInFormation(const unsigned int place_in_formation)
{
   m_ai_scripting_params.m_place_in_formation = place_in_formation;
}

unsigned int weaponsManagerAI::getPlaceInFormation()
{
   return m_ai_scripting_params.m_place_in_formation;
}

void weaponsManagerAI::setPursuitRange(const double range_m)
{
   m_ai_scripting_params.m_pursuit_range_m = range_m;
}

double weaponsManagerAI::getPursuitRange() const
{
   return m_ai_scripting_params.m_pursuit_range_m;
}

void weaponsManagerAI::setSalvoDelay(const double delay_s)
{
   m_ai_scripting_params.m_salvo_delay_s = delay_s;
}

double weaponsManagerAI::getSalvoDelay() const
{
   return m_ai_scripting_params.m_salvo_delay_s;
}

void weaponsManagerAI::setEvasionTakeAction(const bool enable)
{
   m_ai_scripting_params.m_evasion_take_action = enable;
}

double weaponsManagerAI::getEvasionTakeAction() const
{
   return m_ai_scripting_params.m_evasion_take_action;
}

void weaponsManagerAI::setEvasionTakeActionDistance(const double distance_m)
{
   m_ai_scripting_params.m_evasion_take_action_distance_m = distance_m;
}

double weaponsManagerAI::getEvasionTakeActionDistance() const
{
   return m_ai_scripting_params.m_evasion_take_action_distance_m;
}

void weaponsManagerAI::setEvasionAction1(const il::eTakeAction action)
{
   m_ai_scripting_params.m_evasion_actions[0] = action;
}

il::eTakeAction weaponsManagerAI::getEvasionAction1() const
{
   return m_ai_scripting_params.m_evasion_actions[0];
}

void weaponsManagerAI::setEvasionAction1Duration(const double duration_s)
{
   m_ai_scripting_params.m_evasion_take_action_durations_s[0] = duration_s;
}

double weaponsManagerAI::getEvasionAction1Duration() const
{
   return m_ai_scripting_params.m_evasion_take_action_durations_s[0];
}

void weaponsManagerAI::setEvasionAction2(const il::eTakeAction action)
{
   m_ai_scripting_params.m_evasion_actions[1] = action;
}

il::eTakeAction weaponsManagerAI::getEvasionAction2() const
{
   return m_ai_scripting_params.m_evasion_actions[1];
}

void weaponsManagerAI::setEvasionAction2Duration(const double duration_s)
{
   m_ai_scripting_params.m_evasion_take_action_durations_s[1] = duration_s;
}

double weaponsManagerAI::getEvasionAction2Duration() const
{
   return m_ai_scripting_params.m_evasion_take_action_durations_s[1];
}

void weaponsManagerAI::setEvasionAction3(const il::eTakeAction action)
{
   m_ai_scripting_params.m_evasion_actions[2] = action;
}

il::eTakeAction weaponsManagerAI::getEvasionAction3() const
{
   return m_ai_scripting_params.m_evasion_actions[2];
}

void weaponsManagerAI::setEvasionAction3Duration(const double duration_s)
{
   m_ai_scripting_params.m_evasion_take_action_durations_s[2] = duration_s;
}

double weaponsManagerAI::getEvasionAction3Duration() const
{
   return m_ai_scripting_params.m_evasion_take_action_durations_s[2];
}

void weaponsManagerAI::setEvasionAction4(const il::eTakeAction action)
{
   m_ai_scripting_params.m_evasion_actions[3] = action;
}

il::eTakeAction weaponsManagerAI::getEvasionAction4() const
{
   return m_ai_scripting_params.m_evasion_actions[3];
}

void weaponsManagerAI::setEvasionAction4Duration(const double duration_s)
{
   m_ai_scripting_params.m_evasion_take_action_durations_s[3] = duration_s;
}

double weaponsManagerAI::getEvasionAction4Duration() const
{
   return m_ai_scripting_params.m_evasion_take_action_durations_s[3];
}

void weaponsManagerAI::setEvasionRWRResponse(const bool enable)
{
   m_ai_scripting_params.m_evasion_rwr_response = enable;
}

bool weaponsManagerAI::getEvasionRWRResponse() const
{
   return m_ai_scripting_params.m_evasion_rwr_response;
}

void weaponsManagerAI::setEvasionRWRResponseBeamDuration(const double duration_s)
{
   m_ai_scripting_params.m_evasion_rwr_response_beam_duration_s = duration_s;
}

double weaponsManagerAI::getEvasionRWRResponseBeamDuration() const
{
   return m_ai_scripting_params.m_evasion_rwr_response_beam_duration_s;
}

void weaponsManagerAI::setEvasionRWRSAMResponseRange(const double range_m)
{
   m_ai_scripting_params.m_evasion_rwr_sam_response_range_m = range_m;
}

double weaponsManagerAI::getEvasionRWRSAMResponseRange() const
{
   return m_ai_scripting_params.m_evasion_rwr_sam_response_range_m;
}

void weaponsManagerAI::setEvasionRWRAIResponseRange(const double range_m)
{
   m_ai_scripting_params.m_evasion_rwr_ai_response_range_m = range_m;
}

double weaponsManagerAI::getEvasionRWRAIResponseRange() const
{
   return m_ai_scripting_params.m_evasion_rwr_ai_response_range_m;
}

void weaponsManagerAI::setEvasionRWRResponseRestoreAfterBreakLock(const double duration_s)
{
   m_ai_scripting_params.m_evasion_rwr_response_restore_after_break_lock_s = duration_s;
}

double weaponsManagerAI::getEvasionRWRResponseRestoreAfterBreakLock() const
{
   return m_ai_scripting_params.m_evasion_rwr_response_restore_after_break_lock_s;
}

void weaponsManagerAI::setEvasionRWRResponsePriority(il::eRWRResponsePriority priority)
{
   m_ai_scripting_params.m_evasion_rwr_response_priority = priority;
}

il::eRWRResponsePriority weaponsManagerAI::getEvasionRWRResponsePriority() const
{
   return m_ai_scripting_params.m_evasion_rwr_response_priority;
}


void weaponsManagerAI::setHomeBasePosition(const double lat, const double lon)
{
   m_ai_scripting_params.m_home_base_position.setLLA(lat, lon, 0.0);
}

il::positionRecord weaponsManagerAI::getHomeBasePosition() const
{
   return m_ai_scripting_params.m_home_base_position;
}

void weaponsManagerAI::setReturnToCAP(const bool return_to_CAP)
{
   returnToCAP = return_to_CAP;
}

bool weaponsManagerAI::getReturnToCAP() const
{
   return returnToCAP;
}

void weaponsManagerAI::setDebug(const bool debug)
{
   m_ai_scripting_params.m_debug = debug;
}

bool weaponsManagerAI::getDebug() const
{
   return m_ai_scripting_params.m_debug;
}

void weaponsManagerAI::setIsLowOnFuel(const bool low)
{
   isLowOnFuel = low;
}

bool weaponsManagerAI::getIsLowOnFuel() const
{
   return isLowOnFuel;
}

weaponsManagerAI::AIScriptingParams::AIScriptingParams()
   : m_debug(false)
   , m_alert_time_s(60)
   , m_assess_engage_via(MUNITION_INTERCEPT_PT_IGNORE_ZONE)
   , m_escort(false)
   , m_intercept_speed_m_per_s(257.222) // 500 knots = 257.222 m/s
   , m_place_in_formation(0)
   , m_pursuit_range_m(60000)
   , m_salvo_delay_s(1)
   , m_evasion_take_action(false)
   , m_evasion_take_action_distance_m(45000)
   , m_evasion_rwr_response(false)
   , m_evasion_rwr_response_beam_duration_s(3)
   , m_evasion_rwr_sam_response_range_m(150000)
   , m_evasion_rwr_ai_response_range_m(40000)
   , m_evasion_rwr_response_restore_after_break_lock_s(3)
   , m_evasion_rwr_response_priority(DEFAULT)
   , m_home_base_position()
{
   m_evasion_actions[0] = BREAK_LEFT;
   m_evasion_take_action_durations_s[0] = 3;
   m_evasion_actions[1] = BREAK_RIGHT;
   m_evasion_take_action_durations_s[1] = 3;
   m_evasion_actions[2] = STRAIGHT;
   m_evasion_take_action_durations_s[2] = 0;
   m_evasion_actions[3] = STRAIGHT;
   m_evasion_take_action_durations_s[3] = 10;
   m_home_base_position.setLLADegrees(0, 0, C_DEFAULT_UNSET_HOME_BASE_POSITION_ALTITUDE);
}

} // namespace il
