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
*   File: weaponPairing.hpp
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

#ifndef IADSLIB_WEAPON_PAIRING_HPP
#define IADSLIB_WEAPON_PAIRING_HPP

namespace il
{
   class InterceptCalculatorIface;
   class terrainInterface;
}


#include <climits>
#include <string>
#include <memory>
#include <iadsLib/assetRecord.hpp>
#include <iadsLib/trackContainers.hpp>
#include <iadsLib/trackRecord.hpp>
#include <iadsLib/weaponTable.hpp>

namespace il
{
   class WeaponsManagerInterface;

   class WeaponPairing
   {
      public:
         WeaponPairing(const tReadyWeapons::value_type& weapon,
                       const evalTrackArray::value_type& track,
                       const std::shared_ptr< weaponTableRow >& best_preference,
                       const bool use_projection_delays,
                       const double assignment_delay,
                       const double expected_execution_delay,
                       const double max_projection_time_s = 0,
                       const double max_projection_time_incr = 10);
         ~WeaponPairing() = default;
         bool isFailed() const;
         std::string getFailedReason() const;

         void setExternalFail(const std::string& reason, bool reset_intercept_ability = false);

         bool checkWeaponStatusStale(const double sim_time, const double stale_time);
         bool checkCanWeaponIntercept(const double sim_time,
                                      std::shared_ptr<InterceptCalculatorIface>& xcept_calc,
                                      const std::shared_ptr<terrainInterface>& terrain_interface,
                                      double& tti,
                                      positionRecord& pred_pt,
                                      std::weak_ptr<logger::GlobalLogger> aGlobalLogger);

         inline const evalTrackArray::value_type& getTrack() const { return m_track_ref; }
         inline const tReadyWeapons::value_type& getWeapon() const { return m_weapon_ref; }

         // only valid if check for intercept has been made
         inline const trackRecord& getProjectedTrackAtLaunchTime() const { return m_pred_track_at_launch; }

         double computeWeaponScore(const int weapon_type_weight,
                                   const int weapon_subtype_weight,
                                   const int intercept_rng_weight,
                                   const int intercept_time_weight,
                                   const int pk_weight,
                                   const int workload_weight);
         double getWeaponScore() const;
         inline int getTargetPriority() const { return m_best_preference ? m_best_preference->getTargetPriority() : INT_MAX; }
         inline double getComputedClosingTime() const { return m_intercept_closing_time; }

         inline std::string getTypePreference() const { return m_best_preference->getTargetType().getTypeStr(); }
         inline std::string getSubtypePreference() const { return m_best_preference->getTargetType().getSubTypeStr(); }

         bool getInterceptCalcs(double& est_pk,
                                double& est_tti,
                                positionRecord& pred_pt,
                                double& xcept_range,
                                double& max_range,
                                double& closing_time);

      protected:
         // protected interface because meant to be called within intercept call - doesn't reset intercept flags for instance
         // also restrictions on use of ground units, etc
         bool checkWeaponHasLOS(const std::shared_ptr<terrainInterface>& terrain_interface, const positionRecord& track_pos);

         bool canWeaponIntercept(const double sim_time,
                                 std::shared_ptr<InterceptCalculatorIface>& xcept_calc,
                                 const std::shared_ptr<terrainInterface>& terrain_interface,
                                 const trackRecord& projected_track,
                                 double& tti,
                                 positionRecord& pred_pt,
                                 double& est_pk,
                                 double& weap_max_range,
                                 std::weak_ptr<logger::GlobalLogger> aGlobalLogger);

         const tReadyWeapons::value_type m_weapon_ref;
         const evalTrackArray::value_type m_track_ref;
         const std::shared_ptr< weaponTableRow > m_best_preference;
         const bool m_use_projection_delays;
         const double m_assignment_delay;
         double m_expected_execution_delay;
         const double m_max_projection_time_s;
         const double m_max_projection_time_incr;

         trackRecord m_pred_track_at_launch;

         bool m_can_intercept;
         double m_estimated_pk;
         double m_estimated_tti;
         positionRecord m_pred_intercept_pt;
         double m_weapon_max_range; // max weapon range for these intercept params
         double m_intercept_closing_time;

         bool m_is_failed;
         std::string m_failed_reason;

         int m_weapon_type_priority;

         bool m_have_wsubtype_pref_match;
         weaponTableEntry m_wsys_match;

         double m_workload;

         bool m_weapon_score_computed;
         double m_weapon_score;
   };

   typedef std::vector<std::shared_ptr<WeaponPairing>> tPairingArray;
}

#endif
