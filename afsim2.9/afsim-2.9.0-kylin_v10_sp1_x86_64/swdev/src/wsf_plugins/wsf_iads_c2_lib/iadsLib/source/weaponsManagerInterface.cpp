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
*   File: weaponsManagerInterface.cpp
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
*   Abstract: Weapons Manager Base class.
*
*******************************************************************************/

#include <iadsLib/weaponsManagerInterface.hpp>
#include <iadsLib/weaponsManagerEngageInterface.hpp>

namespace il
{

WeaponsManagerInterface::WeaponsManagerInterface()
   : ReferencesAssetManager()
{

}


WeaponsManagerInterface::WeaponsManagerInterface(const WeaponsManagerInterface& from)
   : ReferencesAssetManager(from)
   , m_scripting_params(from.m_scripting_params)
   , m_engagement_interface(nullptr) // must explicitly re-set this after a clone/copy
{

}

WeaponsManagerInterface* WeaponsManagerInterface::Clone()
{
   return new WeaponsManagerInterface(*this);
}

WeaponsManagerInterface::~WeaponsManagerInterface()
{

}

void WeaponsManagerInterface::setWeaponEngagementModule(const std::shared_ptr<il::WeaponsManagerEngagementInterface>& eng_mod)
{
   m_engagement_interface = eng_mod;
}

std::shared_ptr<il::WeaponsManagerEngagementInterface> WeaponsManagerInterface::getWeaponEngagementModule()
{
   return m_engagement_interface;
}

void WeaponsManagerInterface::setSelfDefense(const bool is_on)
{
   m_scripting_params.m_self_defense_flag = is_on;
}

bool WeaponsManagerInterface::getSelfDefense() const
{
   return m_scripting_params.m_self_defense_flag;
}

void WeaponsManagerInterface::setSelfDefenseRange(const double range_m)
{
   m_scripting_params.m_self_defense_range_m = range_m;
}

double WeaponsManagerInterface::getSelfDefenseRange() const
{
   return m_scripting_params.m_self_defense_range_m;
}

void WeaponsManagerInterface::setSelfDefenseShotDoctrine(const il::eShotDoctrine shot_doctrine)
{
   m_scripting_params.m_self_defense_shot_doctrine = shot_doctrine;
}

il::eShotDoctrine WeaponsManagerInterface::getSelfDefenseShotDoctrine() const
{
   return m_scripting_params.m_self_defense_shot_doctrine;
}

void WeaponsManagerInterface::setSelfDefenseShotExpiry(const double shot_expiry_time_s)
{
   m_scripting_params.m_self_defense_shot_expiry_s = shot_expiry_time_s;
}

double WeaponsManagerInterface::getSelfDefenseShotExpiry() const
{
   return m_scripting_params.m_self_defense_shot_expiry_s;
}

void WeaponsManagerInterface::setEngageEWTargets(const bool set_on)
{
   m_scripting_params.m_engage_ew_targets = set_on;
}

bool WeaponsManagerInterface::getEngageEWTargets() const
{
   return m_scripting_params.m_engage_ew_targets;
}

void WeaponsManagerInterface::setEngageTARTargets(const bool set_on)
{
   m_scripting_params.m_engage_tar_targets = set_on;
}

bool WeaponsManagerInterface::getEngageTARTargets() const
{
   return m_scripting_params.m_engage_tar_targets;
}

void WeaponsManagerInterface::setEngageTTRTargets(const bool set_on)
{
   m_scripting_params.m_engage_ttr_targets = set_on;
}

bool WeaponsManagerInterface::getEngageTTRTargets() const
{
   return m_scripting_params.m_engage_ttr_targets;
}

void WeaponsManagerInterface::setEngageLocalTTRTargetsOnly(const bool set_on)
{
   m_scripting_params.m_engage_local_ttr_targets_only = set_on;
}

bool WeaponsManagerInterface::getEngageLocalTTRTargetsOnly() const
{
   return m_scripting_params.m_engage_local_ttr_targets_only;
}

void WeaponsManagerInterface::setEngageTrackQuality(const double quality)
{
   m_scripting_params.m_engage_track_quality = quality;
}

double WeaponsManagerInterface::getEngageTrackQuality() const
{
   return m_scripting_params.m_engage_track_quality;
}

void WeaponsManagerInterface::setTimeBetweenEngagementsDelay(const double seconds)
{
   m_scripting_params.m_time_between_engagements_s = seconds;
}

double WeaponsManagerInterface::getTimeBetweenEngagementsDelay() const
{
   return m_scripting_params.m_time_between_engagements_s;
}

void WeaponsManagerInterface::setExpectedExecutionDelay(const double seconds)
{
   m_scripting_params.m_expected_execution_delay = seconds;
}

double WeaponsManagerInterface::getExpectedExecutionDelay() const
{
   return m_scripting_params.m_expected_execution_delay;
}

void WeaponsManagerInterface::setUseWEZCalculator(const bool use)
{
   m_scripting_params.m_wez_calculator_scripted = use;
}

bool WeaponsManagerInterface::getUseWEZCalculator() const
{
   return m_scripting_params.m_wez_calculator_scripted;
}


void WeaponsManagerInterface::setWEZLibraryName(const std::string& name)
{
   m_scripting_params.m_wez_library_name = name;
}

std::string WeaponsManagerInterface::getWEZLibraryName() const
{
   return m_scripting_params.m_wez_library_name;
}

void WeaponsManagerInterface::setWEZTacticalRange(const WEZTacticalRangeEnum& range)
{
   m_scripting_params.m_wez_tactical_range = range;
}

WeaponsManagerInterface::WEZTacticalRangeEnum WeaponsManagerInterface::getWEZTacticalRange() const
{
   return m_scripting_params.m_wez_tactical_range;
}

WeaponsManagerInterface::WEZTacticalRangeEnum WeaponsManagerInterface::WEZTacticalRangeFromString(const std::string& range_string)
{
   if (range_string == "aero")
   {
      return WeaponsManagerInterface::E_AERO;
   }
   else if (range_string == "max1")
   {
      return WeaponsManagerInterface::E_MAX1;
   }
   else if (range_string == "max2")
   {
      return WeaponsManagerInterface::E_MAX2;
   }

   throw std::runtime_error("WEZTacticalRangeFromString(): Unrecognized WEZ tactical range style: " + range_string);

   return WeaponsManagerInterface::E_AERO;
}

void WeaponsManagerInterface::setHavecoReportingStyle(const WeaponsManagerInterface::eHavecoReportStyle style)
{
   m_scripting_params.m_haveco_reporting_style = style;
}

WeaponsManagerInterface::eHavecoReportStyle WeaponsManagerInterface::getHavecoReportingStyle() const
{
   return m_scripting_params.m_haveco_reporting_style;
}


WeaponsManagerInterface::eHavecoReportStyle WeaponsManagerInterface::HavecoReportStyleFromString(const std::string& style_string)
{
   if (style_string == "on_launch")
   {
      return WeaponsManagerInterface::E_ON_LAUNCH;
   }
   else if (style_string == "on_detonate")
   {
      return WeaponsManagerInterface::E_ON_DETONATION;
   }
   else if (style_string == "on_kill")
   {
      return WeaponsManagerInterface::E_ON_KILL;
   }

   throw std::runtime_error("HavecoReportStyleFromString(): Unrecognized haveco scripting style: " + style_string);

   return WeaponsManagerInterface::E_ON_LAUNCH;
}

WeaponsManagerInterface::ScriptingParams::ScriptingParams()
   : m_self_defense_flag(false)
   , m_self_defense_range_m(0)
   , m_self_defense_shot_doctrine(il::SHOOT_1)
   , m_self_defense_shot_expiry_s(10)
   , m_engage_ew_targets(false)
   , m_engage_tar_targets(false)
   , m_engage_ttr_targets(true)
   , m_engage_local_ttr_targets_only(true)
   , m_engage_track_quality(0.0)
   , m_time_between_engagements_s(0)
   , m_expected_execution_delay(0.0)  // TODO auto
   , m_wez_calculator_scripted(false)
   , m_wez_library_name()
   , m_wez_tactical_range(WeaponsManagerInterface::E_AERO)
   , m_haveco_reporting_style(WeaponsManagerInterface::E_ON_KILL)
{
}

} // namespace il
