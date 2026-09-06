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
*   File: unclassifiedBattleManager.hpp
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

#ifndef IADSLIB_UNCLASSIFIED_BATTLE_MANAGER
#define IADSLIB_UNCLASSIFIED_BATTLE_MANAGER

namespace il
{
   class AssetManagerInterface;
}

#include <string>
#include <vector>
#include <map>
#include <iadsLib/BattleManagerIface.hpp>
#include <iadsLib/weaponTable.hpp>
#include <iadsLib/weaponPairing.hpp>
#include <iadsLib/weaponScoringMatrix.hpp>

namespace il
{

   class IADSLIB_EXPORT unclassifiedBattleManager : public BattleManagerInterface
   {
      // Abstract interface
   public:
      virtual ~unclassifiedBattleManager() = default;
      unclassifiedBattleManager(std::weak_ptr<logger::GlobalLogger> aGlobalLogger, const std::shared_ptr<terrainInterface> &terrain_interface = std::shared_ptr<terrainInterface> (new nullTerrainEngine()));

      virtual BattleManagerInterface *Clone ();

      bool run(il::moeLoggingInterface& aMOELogger) override;
      void runMonitorAssignments(il::moeLoggingInterface& aMOELogger, const double SimTime) override;

      // model-specific interface
   public:
      inline virtual void setStaleAssetTime (const double value) { m_stale_asset_time_s = value; }
      inline virtual double getStaleAssetTime () const { return m_stale_asset_time_s; }

      inline virtual void setShotDoctrine (const eShotDoctrine value) { m_shot_doctrine = value; }
      inline virtual eShotDoctrine getShotDoctrine () const { return m_shot_doctrine; }

      inline virtual void setMaxFiringTime (const double value) { m_max_firing_time_s = value; }
      inline virtual double getMaxFiringTime () const { return m_max_firing_time_s; }

      inline virtual void setPrintValidUnits (const bool value = true) { m_print_valid_units = value; }
      inline virtual bool getPrintValidUnits () const { return m_print_valid_units; }
      inline virtual void setPrintValidUnitDetails (const bool value = true) { m_print_valid_unit_details = value; }
      inline virtual bool getPrintValidUnitDetails() const { return m_print_valid_unit_details; }
      inline virtual void setPrintInvalidUnitDetails (const bool value = true) { m_print_invalid_unit_details = value; }
      inline virtual bool getPrintInvalidUnitDetails () const { return m_print_invalid_unit_details; }

      inline virtual void setWeaponTypeScoringWeight (const int value) { m_weapon_type_weight = value; }
      inline virtual void setWeaponSubtypeScoringWeight (const int value) { m_weapon_subtype_weight = value; }
      inline virtual void setInterceptRangeScoringWeight (const int value) { m_intercept_rng_weight = value; }
      inline virtual void setInterceptTimeScoringWeight (const int value) { m_intercept_time_weight = value; }
      inline virtual void setProbabilityKillScoringWeight (const int value) { m_pk_weight = value; }
      inline virtual void setWorkloadScoringWeight (const int value) { m_workload_weight = value; }

      inline virtual weaponTable &getWeaponTable () { return m_weapon_table; }

   protected:
      unclassifiedBattleManager (const unclassifiedBattleManager &from);

   protected:

      double m_stale_asset_time_s;
      eShotDoctrine m_shot_doctrine;
      double m_max_firing_time_s;

      bool m_print_valid_units;
      bool m_print_valid_unit_details;
      bool m_print_invalid_unit_details;

      weaponTable m_weapon_table;

      // scoring factor weights
      int m_weapon_type_weight;
      int m_weapon_subtype_weight;
      int m_intercept_rng_weight;
      int m_intercept_time_weight;
      int m_pk_weight;
      int m_workload_weight;

      enum ePasses { PASS_1 = 0, PASS_2 = 1, PASS_3 = 2, PASS_4 = 3, PASS_5 = 4, PASS_6 = 5, NUM_PASSES };
      virtual void buildWeaponArrays (tReadyWeapons (&weapon_pass_arrays)[NUM_PASSES]);
	   virtual bool pass1Succeeds (const tReadyWeapons::value_type &weapon);
	   virtual bool pass2Succeeds (const tReadyWeapons::value_type &weapon);
	   virtual bool pass3Succeeds (const tReadyWeapons::value_type &weapon);
	   virtual bool pass4Succeeds (const tReadyWeapons::value_type &weapon);
	   virtual bool pass5Succeeds (const tReadyWeapons::value_type &weapon);
	   virtual bool pass6Succeeds (const tReadyWeapons::value_type &weapon);

      virtual void PerformWeaponPairings (il::moeLoggingInterface& aMOELogger, const tReadyWeapons &weapons, evalTrackArray &eval_tracks, tPairingArray &pairings);
      virtual void AssessWeapon (WeaponPairing &pairing);

      virtual void AssignWeapons (tPairingArray &pairings, int pass_number);

      virtual void PrioritizePairingsGlobally (tPairingArray &pairings) const;
      virtual bool CheckWeaponCapableOfAssignment (const tReadyWeapons::value_type &weapon, const idRecord &track_id) const;

   private:
      unclassifiedBattleManager & operator=(const unclassifiedBattleManager &rhs);
   };

}

#endif

