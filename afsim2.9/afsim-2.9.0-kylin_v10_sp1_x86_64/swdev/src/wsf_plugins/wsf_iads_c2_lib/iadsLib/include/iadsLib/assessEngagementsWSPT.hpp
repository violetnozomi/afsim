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
*   File: assessEngagementsWSPT.hpp
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

#ifndef IADSLIB_ASSESS_ENGAGEMENTS_WSPT_HPP
#define IADSLIB_ASSESS_ENGAGEMENTS_WSPT_HPP

// forward declarations
namespace il
{
   class BattleManagerInterface;
   class AssetManagerInterface;
}

#include <vector>

#include <iadsLib/assetRecord.hpp>
#include <iadsLib/weaponTable.hpp>
#include <iadsLib/threatRecordInterface.hpp>
#include <iadsLib/assessmentRecord.hpp>
#include <iadsLib/trackRecord.hpp>
#include <iadsLib/interceptCalculatorInterface.hpp>
#include <iadsLib/nullTerrainEngine.hpp>
#include <iadsLib/moeLoggingInterface.hpp>

namespace il
{

   class IADSLIB_EXPORT assessEngagementsWSPT
   {
   public:
      assessEngagementsWSPT (std::shared_ptr<InterceptCalculatorIface> intercept_calc, std::weak_ptr<logger::GlobalLogger> aGlobalLogger,
                             std::shared_ptr<terrainInterface> terrain_interface = std::shared_ptr<terrainInterface> (new nullTerrainEngine()));

      inline weaponTable &GetWeaponTable () { return m_WeaponTable; }
      inline void SetTerrainEngine (const std::shared_ptr<terrainInterface> &terrain_interface) { m_terrain_interface = terrain_interface; }
      inline void SetInterceptCalculator (const std::shared_ptr<InterceptCalculatorIface> &intercept_calc) { m_intercept_calc = intercept_calc; }

      tAssessments Process (il::moeLoggingInterface& aMOELogger, const double SimTime, BattleManagerInterface *bm_ref, tRankedThreats &Threats);

      inline void SetReportZoneAllocation (const bool zone_allocation) { m_report_zone_allocation = zone_allocation; }
      inline void SetReportGoSummary (const bool go_summary) { m_report_go_summary = go_summary; }
      inline void SetReportGoDetails (const bool go_details) { m_report_go_details = go_details; }
      inline void SetReportNoGoSummary (const bool nogo_summary) { m_report_nogo_summary = nogo_summary; }

      inline void SetZoneAllocationMEZ (const bool MEZ) { m_allocate_to_mez = MEZ; }
      inline bool ShouldAllocateMEZ () const { return m_allocate_to_mez; }
      inline void SetZoneAllocationFEZ (const bool FEZ) { m_allocate_to_fez = FEZ; }
      inline bool ShouldAllocateFEZ () const { return m_allocate_to_fez; }
      inline void SetZoneAllocationJEZ (const bool JEZ) { m_allocate_to_jez = JEZ; }
      inline bool ShouldAllocateJEZ () const { return m_allocate_to_jez; }

      inline void SetThreatLevelThreshold (const double threshold) { m_threat_level_threshold = threshold; }
      inline double GetThreatLevelThreshold () const { return m_threat_level_threshold; }


   protected:
      weaponTable    m_WeaponTable;

      // Report Settings
      bool m_report_zone_allocation;
      bool m_report_go_summary;
      bool m_report_go_details;
      bool m_report_nogo_summary;

      // Allocation Settings
      bool m_allocate_to_mez;
      bool m_allocate_to_fez;
      bool m_allocate_to_jez;

      // Threat Level
      double m_threat_level_threshold;

      // this class doesn't own these members therefore, on copy, they will get sliced. The new copy gets a reference to this ones
      // intercept and terrain engines.
      std::shared_ptr<InterceptCalculatorIface> m_intercept_calc;
      std::shared_ptr<terrainInterface>         m_terrain_interface;

      void Assess (const double SimTime, const BattleManagerInterface *bm_ref, const assetMap &assets, tRankedThreats &Threats, tAssessments &Assessments, const tReadyWeapons &Weapons);
      void Allocate (il::moeLoggingInterface& aMOELogger, const double SimTime, const BattleManagerInterface *bm_ref, tRankedThreats &Threats, const tAssessments &Assessments, const std::shared_ptr<assetRecord> &my_asset, const assetMap &assets);
      void CantcoUnassignedAssignments (il::moeLoggingInterface& aMOELogger, BattleManagerInterface *bm_ref, tRankedThreats &Threats, const std::string &reason);
      void GetBestAssessedZoneType (const tAssessments::value_type &assessment);

      std::weak_ptr<logger::GlobalLogger> mGlobalLogger;
   };
}

#endif


