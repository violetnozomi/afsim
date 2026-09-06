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
*   File: weaponsManagerInterface.hpp
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

#ifndef IADSLIB_WEAPONS_MANAGER_INTERFACE_HPP
#define IADSLIB_WEAPONS_MANAGER_INTERFACE_HPP

namespace il
{
   class AssetManagerInterface;
}


#include <vector>
#include <utility>
#include <map>
#include <memory>

#include <logger.hpp>
#include <iadsLib/baseMessage.hpp>
#include <iadsLib/referencesProcessor.hpp>
#include <iadsLib/enum.hpp>

namespace il
{
   class WeaponsManagerEngagementInterface;

   class IADSLIB_EXPORT WeaponsManagerInterface : public ReferencesAssetManager
   {
      public:
         WeaponsManagerInterface();
         virtual WeaponsManagerInterface* Clone();
         virtual ~WeaponsManagerInterface();

         virtual void Run(const double sim_time_s) {};

         void setWeaponEngagementModule(const std::shared_ptr<il::WeaponsManagerEngagementInterface>& eng_mod);
         std::shared_ptr<il::WeaponsManagerEngagementInterface> getWeaponEngagementModule();

         void setSelfDefense(const bool is_on);
         bool getSelfDefense() const;

         void setSelfDefenseRange(const double range_m);
         double getSelfDefenseRange() const;

         void setSelfDefenseShotDoctrine(const il::eShotDoctrine shot_doctrine);
         il::eShotDoctrine getSelfDefenseShotDoctrine() const;

         void setSelfDefenseShotExpiry(const double shot_expiry_time_s);
         double getSelfDefenseShotExpiry() const;

         void setEngageEWTargets(const bool set_on);
         bool getEngageEWTargets() const;

         void setEngageTARTargets(const bool set_on);
         bool getEngageTARTargets() const;

         void setEngageTTRTargets(const bool set_on);
         bool getEngageTTRTargets() const;

         void setEngageLocalTTRTargetsOnly(const bool set_on);
         bool getEngageLocalTTRTargetsOnly() const;

         void setEngageTrackQuality(const double quality);
         double getEngageTrackQuality() const;

         void setTimeBetweenEngagementsDelay(const double seconds);
         double getTimeBetweenEngagementsDelay() const;

         void setExpectedExecutionDelay(const double seconds);
         double getExpectedExecutionDelay() const;

         // WEZ calculator
         void setUseWEZCalculator(const bool use);
         bool getUseWEZCalculator() const;

         void setWEZLibraryName(const std::string& name);
         std::string getWEZLibraryName() const;

         enum WEZTacticalRangeEnum
         {
            E_AERO,
            E_MAX1,
            E_MAX2
         };
         void setWEZTacticalRange(const WEZTacticalRangeEnum& range);
         WEZTacticalRangeEnum getWEZTacticalRange() const;

         static WEZTacticalRangeEnum WEZTacticalRangeFromString(const std::string& range_string);

         // HAVECO reporting style
         enum eHavecoReportStyle
         {
            E_ON_LAUNCH,
            E_ON_DETONATION,
            E_ON_KILL
         };
         void setHavecoReportingStyle(const eHavecoReportStyle style);
         eHavecoReportStyle getHavecoReportingStyle() const;

         static eHavecoReportStyle HavecoReportStyleFromString(const std::string& style_string);


      protected:
         WeaponsManagerInterface(const WeaponsManagerInterface& from);

         class ScriptingParams
         {
            public:
               ScriptingParams();

               // self defense
               bool m_self_defense_flag;
               double m_self_defense_range_m;
               il::eShotDoctrine m_self_defense_shot_doctrine;
               double m_self_defense_shot_expiry_s;
               // engagement settings
               bool m_engage_ew_targets;
               bool m_engage_tar_targets;
               bool m_engage_ttr_targets;
               bool m_engage_local_ttr_targets_only;
               double m_engage_track_quality;
               // delays
               // note: time to launch and salvo delays are properties of the weapons
               // in the iadsLib as opposed to the weapons manager in HELIOS. This leads to cleaner
               // consistency in weapon implementations.
               double m_time_between_engagements_s;

               // This is to allow otherwise unaccounted delays (such as sensors establishing tracks and messages being passed between platforms)
               // to be considered for an assignment to improve shot timelines
               double m_expected_execution_delay;

               // WEZ calculator
               // Note: WEZ calculator scripting here is to be used within intercept calculators
               // we will make it available for querying to satisfy those inputs
               bool m_wez_calculator_scripted;
               std::string m_wez_library_name;
               WEZTacticalRangeEnum m_wez_tactical_range;

               // HAVECO reporting
               WeaponsManagerInterface::eHavecoReportStyle m_haveco_reporting_style;
         };

      private:
         WeaponsManagerInterface& operator=(const WeaponsManagerInterface& rhs) = delete;

         ScriptingParams m_scripting_params;

         std::shared_ptr<il::WeaponsManagerEngagementInterface> m_engagement_interface;
   };
}

#endif
