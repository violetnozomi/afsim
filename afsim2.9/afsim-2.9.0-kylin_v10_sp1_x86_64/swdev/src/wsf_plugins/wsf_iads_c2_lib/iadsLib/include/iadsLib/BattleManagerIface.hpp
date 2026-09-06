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
*   File: BattleManagerIface.hpp
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
*   Abstract: Not a pure abstract base class but intended to be. The reason it
*    is not is because some simulation frameworks force base class type
*    registration independently from their inherited types (AFSIM) if you
*    want to be able to up/down cast.
*
*******************************************************************************/

#ifndef BATTLE_MANAGER_IFACE_HPP
#define BATTLE_MANAGER_IFACE_HPP

#include <list>
#include <map>
#include <string>
#include <memory>

#include <logger.hpp>
#include <iadsLib/idRecord.hpp>

#include "iadsLib/assetManagerInterface.hpp"
#include <iadsLib/baseMessage.hpp>
#include <iadsLib/airTargetMessage.hpp>
#include <iadsLib/plotMessage.hpp>
#include <iadsLib/bearingReportMessage.hpp>
#include <iadsLib/combatStatusMessage.hpp>
#include <iadsLib/assignmentMessage.hpp>
#include <iadsLib/assignmentACKMessage.hpp>
#include <iadsLib/weaponsControlMessage.hpp>

#include <iadsLib/trackRecord.hpp>
#include <iadsLib/threatRecordInterface.hpp>
#include <iadsLib/assessmentRecord.hpp>
#include <iadsLib/assetRecord.hpp>
#include <iadsLib/interceptCalculatorInterface.hpp>
#include <iadsLib/nullTerrainEngine.hpp>
#include <iadsLib/trackROE.hpp>
#include <iadsLib/zoneRecordInterface.hpp>
#include <iadsLib/referencesProcessor.hpp>

/**
*  @brief Encapsulate all IADSLIB functions and classes.
*/
namespace il
{
   /**
   @brief Class encapsulating a single iads component.
   */
   class IADSLIB_EXPORT BattleManagerInterface : public ReferencesAssetManager
   {
         // Abstract interface
      public:
         virtual ~BattleManagerInterface() = default;
         virtual bool run(il::moeLoggingInterface& aMOELogger)
         {
            HCL_FATAL_LOGGER(GetAssetManager()->getGlobalLogger()) << "Error. Cannot instantiate a raw BattleManagerInterface() class. It is meant to be used as a base type";
            throw std::runtime_error("Error. Cannot instantiate a raw BattleManagerInterface() class. It is meant to be used as a base type");
         }

         virtual void runMonitorAssignments(il::moeLoggingInterface& aMOELogger, const double SimTime)
         {
            HCL_FATAL_LOGGER(GetAssetManager()->getGlobalLogger()) << "MonitorAssignments() is meant to be implemented by the deriving class";
         }

         virtual BattleManagerInterface* Clone();

         BattleManagerInterface(const std::shared_ptr<terrainInterface>& terrain_interface = std::shared_ptr<terrainInterface>(new nullTerrainEngine()));

         virtual void SetInterceptCalculator(const std::shared_ptr<InterceptCalculatorIface>& xcept_iface);

         virtual void setCommitAuthority(const bool value);
         virtual bool getCommitAuthority() const;

         virtual void setProjectTracksByDelays(const bool value);
         virtual bool getProjectTracksByDelays() const;

         virtual void setTargetProjectionTime(const double time_s);
         virtual double getTargetProjectionTime() const;

         virtual void setTargetProjectionTimeIncrement(const double time_s);
         virtual double getTargetProjectionTimeIncrement() const;

         virtual void setTerrainEngine(const std::shared_ptr<terrainInterface>& terrain_interface);
         virtual void addTrackIFFROE(const eIFFType iff, const bool engage);


         // Assignment creation
         virtual void makeAssignment(const idRecord& initiating_unit_id,
                                     const idRecord& assigning_unit_id,
                                     const idRecord& weapon_unit_id,
                                     const idRecord& weapon_id,
                                     const evaluationTrack& evaluation_track,
                                     const eShotDoctrine shot_doctrine,
                                     const double max_shot_time);

         // what normally can happen is the battle manager is off looping queueing assignments in it's processing path.
         // sometimes it may need to ask if it has already made a tentative assignment, this is the interface for that
         virtual int getNumAssignmentsPending(const evaluationTrack& eval_track) const;

         // Assignment deletion must be orchestrated between monitor assignments and the holder of the assignment array although
         // there's some common actions regarding unallocated delegations that can be done in the BM via the AM
         virtual void CANTCOUnallocatedDelegations(il::moeLoggingInterface& aMOELogger, const std::string& reason);
         virtual void CANTCODelegatedAssignment(il::moeLoggingInterface& aMOELogger,
                                                std::shared_ptr<assignmentMessage>& assignment_message,
                                                const std::string& cantco_reason,
                                                const bool is_systemic = false, const bool is_global = false);


         /////////////////// Interfaces needed by Asset Manager Interface ///////////////////
         inline virtual bool isTrackEngageable(const std::shared_ptr<trackRecord>& track_record) const { return m_trk_engage_rules.IsEngageable(track_record); }
         inline virtual void clearEvaluationTracks() { m_EvaluationTracks.clear(); }
         inline virtual void addTrackForEvaluation(const evaluationTrack& track) { m_EvaluationTracks.push_back(track); }
         inline virtual const evalAssignmentArray& getEvaluatedAssignmentsArray() { return m_EvaluatedAssignments; }
         inline virtual void clearEvaluatedAssignmentsArray() { m_EvaluatedAssignments.clear(); }
         /////////////////// End Interfaces needed by Asset Manager Interface ///////////////////

         inline virtual std::shared_ptr<AssetManagerInterface>& getAssetManagerRef() { return m_asset_manager; }
         inline virtual const std::shared_ptr<AssetManagerInterface>& getAssetManagerRef() const { return m_asset_manager; }

      protected:
         BattleManagerInterface(const BattleManagerInterface& from);
         virtual size_t getNumThreats() const;

         // current tracks being submitted for the battle manager assignment pipeline
         evalTrackArray m_EvaluationTracks; // note: we separate out the master tracks array from the tracks to evaluate because the only time they will be the same is when
         // there are no assignments if this manager has commit authority, otherwise, we don't have any ability to engage new tracks, just previously assigned ones from
         // up-echelon (temporary structure)

         ///////////////////////////
         // Assignment Structures //
         ///////////////////////////
         // Assignments made during last invocation of run() - (temporary structure)
         evalAssignmentArray m_EvaluatedAssignments;


         /// Map of all threat calculations based on evaluated tracks.
         threatMap m_Threats;


         // Commit Authority
         bool  m_commit_authority;


         // track engagement rules
         trackROE m_trk_engage_rules;

         // forward projections based on assignment delays
         bool m_project_trks_by_assign_delays;

         // forward projection time/increment
         double m_target_projection_time_s;
         double m_target_projection_time_incr_s;

         // External interfaces
         std::shared_ptr<InterceptCalculatorIface> m_xcept_iface;
         std::shared_ptr<terrainInterface> m_terrain_interface;

      private:
         BattleManagerInterface& operator=(const BattleManagerInterface& rhs) = delete;
   };
}

#endif
