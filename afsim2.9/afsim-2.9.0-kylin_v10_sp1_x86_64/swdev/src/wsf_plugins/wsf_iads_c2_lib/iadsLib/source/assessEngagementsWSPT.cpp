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
*   File: assessEngagementsWSPT.cpp
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

#include <cfloat>

#include <logger.hpp>
#include <iadsLib/util.hpp>
#include <iadsLib/assessEngagementsWSPT.hpp>
#include <iadsLib/BattleManagerIface.hpp>
#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/weaponsManagerInterface.hpp>

namespace il
{

assessEngagementsWSPT::assessEngagementsWSPT (std::shared_ptr<InterceptCalculatorIface> intercept_calc,
                                              std::weak_ptr<logger::GlobalLogger> aGlobalLogger,
                                              std::shared_ptr<terrainInterface> terrain_interface)
   : m_WeaponTable(aGlobalLogger)
   , m_report_zone_allocation (true)
   , m_report_go_summary (true)
   , m_report_go_details (true)
   , m_report_nogo_summary (false)
   , m_allocate_to_mez (true)
   , m_allocate_to_fez (true)
   , m_allocate_to_jez (false)
   , m_threat_level_threshold (4.5)
   , m_intercept_calc (intercept_calc)
   , m_terrain_interface (terrain_interface)
   , mGlobalLogger(aGlobalLogger)
{

}


tAssessments assessEngagementsWSPT::Process (il::moeLoggingInterface& aMOELogger, const double SimTime, BattleManagerInterface *bm_ref, tRankedThreats &Threats)
{
   tAssessments assessments;

   auto my_asset = bm_ref->getAssetManagerRef()->getThisAsset();
   const auto &assets = bm_ref->getAssetManagerRef()->getAssets();

   // Build Weapon Array
   HCL_DEBUG_LOGGER(mGlobalLogger) << "\t Building weapon ready list...";
   tReadyWeapons weapons;
   std::tie(weapons, std::ignore) = my_asset->getReadyWeaponList(assets);

   if (!weapons.size())
   {
      // CANTCO unassigned assignments
      CantcoUnassignedAssignments (aMOELogger, bm_ref, Threats, "No Subordinate Weapons");
   }
   else
   {
      Assess (SimTime, bm_ref, assets, Threats, assessments, weapons);

      // Allocate?
      if (ShouldAllocateMEZ() || ShouldAllocateFEZ() || ShouldAllocateJEZ())
         Allocate (aMOELogger, SimTime, bm_ref, Threats, assessments, my_asset, assets);
   }

   return assessments;
}

// Assess basically creates all threat to weapon pairings where all weapons get paired with a threat via an assessment. These assessments get put into the
// assessment array.
void assessEngagementsWSPT::Assess (const double SimTime, const BattleManagerInterface *bm_ref, const assetMap &assets, tRankedThreats &Threats, tAssessments &Assessments, const tReadyWeapons &Weapons)
{
   // Loop through the threat array and assess the engagements
   for (auto it_threat = Threats.cbegin(); it_threat != Threats.cend(); ++it_threat)
   {
      const auto &Threat = *it_threat;

      // Not a ranked threat?
      if (Threat->isUnranked())
         continue;

      // Threat Level below use specified threshold for engagement?
      if (Threat->getEvaluation() > GetThreatLevelThreshold())
      {
         HCL_TRACE_LOGGER(mGlobalLogger) << "assessEngagementsWSPT::Assess(): Threat evaluation (" << Threat->getEvaluation() << ") did not pass weapon table threshold: " << GetThreatLevelThreshold();
         continue;
      }

      // Find track for a given threat
      auto currentTrack = Threat->getTrack();
      if (!currentTrack)
      {
         HCL_WARN_LOGGER(mGlobalLogger)  << "assessEngagementsWSPT::Assess(): Warning: Invalid track in threat.";
         continue;
      }


      // Loop through the weapons array and assess each weapon vs this threat
      for (auto it_weapon = Weapons.cbegin(); it_weapon != Weapons.cend(); ++it_weapon)
      {
         const auto &Weapon = *it_weapon;

         // Retrieve the weapon asset record
         auto currentWeapon = assets.GetAsset (Weapon->GetParentAssetID());
         if (!currentWeapon)
         {
            HCL_WARN_LOGGER(mGlobalLogger)  << "assessEngagementsWSPT::Assess(): Warning: Attempted to assess Weapon ID "
                               << Weapon->GetParentAssetID()
                               << " from the weapons array but the weapon not found within the assets array\n";
            continue;
         }

         // Create a new assessment record
         std::shared_ptr<assessmentRecord> newAssessment (new assessmentRecord (m_intercept_calc, mGlobalLogger, m_terrain_interface));

         // determine if we should attempt to counteract assignment delay
         auto assignment_delay = il::assetRecord::calculateAssignmentDelays (assets, bm_ref->getAssetManagerRef()->getThisAsset()->getID(), GetAssignedIDFromWeaponRef (Weapon));

         // Execution delay to account for delays such as sensor acquisition time
         double expected_execution_delay = 0;
         auto wm = Weapon->GetWeaponsManager();
         if (wm)
            expected_execution_delay = wm->getExpectedExecutionDelay();

         // Populate assessment record based on current weapon/track
         newAssessment->AssessWeapon (SimTime, Threat, Weapon, m_WeaponTable, bm_ref->getProjectTracksByDelays(), assignment_delay, expected_execution_delay, bm_ref->getTargetProjectionTime(), bm_ref->getTargetProjectionTimeIncrement());

         // If the weapon is not excluded, add assessment to the list
         if (!newAssessment->isWeaponExcluded())
         {
            // set the priority based on the rank
            newAssessment->setPriority (Threat->getRank()); // JLK- TTD this may be too naiive from the VCL BM since rank is only relatable within the current processing cycle. This could be an apples/oranges comparison between Processing loops.
            Assessments.push_back (newAssessment);

            // Weapon can intercept this threat?
            if (newAssessment->canInterceptTrack())
            {
               auto intercept_point = newAssessment->getPredictedInterceptPt();
               HCL_DEBUG_LOGGER(mGlobalLogger) << "... ASSESSMENT: Weapon " << currentWeapon->getName()
                                  << " CAN intercept Threat " << currentTrack->getTargetTruthName() << std::endl

                                    // Print Assessment Details
                                  << "... ... Intercept (w/i MaxR): " << BoolToString (newAssessment->isInterceptInsideMaxRange())
                                  << "  Intercept (outside MinR): " << BoolToString (newAssessment->isInterceptOutsideMinRange()) << std::endl

                                  << "... ... Intercept (w/i Zone): " << BoolToString (newAssessment->isInterceptInsideZone())
                                  << "  Intercept (w/i C2 Zone):  " << BoolToString (newAssessment->isInterceptInsideC2Zone()) << std::endl

                                  << "... ... Intercept Time: " << newAssessment->getInterceptTime()
                                  << "  Distance: " << newAssessment->getInterceptDistance()
                                  << "  PCA Distance: " << newAssessment->getDistanceToPCA() << std::endl

                                  << "... ... Potential Shots " << newAssessment->getPotentialShots() << std::endl

                                  << "... ... Intercept Point (Lat: " << intercept_point.getLatDegs()
                                  << " Lon: " << intercept_point.getLonDegs()
                                  << " Alt: " << intercept_point.getAltM() << ")\n";
            }
            else
            {
               HCL_DEBUG_LOGGER(mGlobalLogger) << "... ASSESSMENT: Weapon " << currentWeapon->getName()
                                  << " CANNOT intercept Threat " << currentTrack->getTargetTruthName() << std::endl;
            }
         }
      } // each weapon
   } // each threat
}


// allocate simply takes the highest assessment for each threat and marks the best zone type for that threat based on the lowest time
// to intercept.
void assessEngagementsWSPT::Allocate (il::moeLoggingInterface& aMOELogger, const double SimTime, const BattleManagerInterface *bm_ref, tRankedThreats &Threats, const tAssessments &Assessments, const std::shared_ptr<assetRecord> &my_asset, const assetMap &assets)
{
   // by definition of the ranked threats array, all threats here are ranked
   for (auto it_threat = Threats.cbegin(); it_threat != Threats.cend(); ++it_threat)
   {
      const auto &threat = *it_threat;

      tAssessments::value_type best_assessment;
      double best_assessment_time = DBL_MAX;

      for (auto it_assessment = Assessments.cbegin(); it_assessment != Assessments.cend(); ++it_assessment)
      {
         const auto &assessment = *it_assessment;

         if (assessment->getTrack()->getID() == threat->getTrack()->getID() &&
             IsAssessmentValid (*assessment, assets, my_asset, mGlobalLogger) &&
             assessment->getInterceptTime() < best_assessment_time)
         {
            best_assessment_time = assessment->getInterceptTime();
            best_assessment = assessment;
         }
      }

      if (best_assessment)
      {
         threat->setAllocateZoneTypes (best_assessment->GetAssessedZoneTypes());
         aMOELogger.Allocate (SimTime, &bm_ref->getAssetManagerRef()->getThisAsset()->getID(), best_assessment.get());
      }
   }
}

void assessEngagementsWSPT::CantcoUnassignedAssignments (il::moeLoggingInterface& aMOELogger, BattleManagerInterface *bm_ref, tRankedThreats &Threats, const std::string &reason)
{
   // CANTCO unassigned assignments
   HCL_DEBUG_LOGGER(mGlobalLogger) << bm_ref->getAssetManagerRef()->getThisAsset()->getName() << " ...  " << reason;

   for (auto it_threat = Threats.cbegin(); it_threat != Threats.cend(); ++it_threat)
   {
      const auto &threat = *it_threat;
      auto delegate_assignment = GetDelegatedAssignment (threat);
      if (delegate_assignment)
        bm_ref->GetAssetManager()->cantcoAssignment (aMOELogger, delegate_assignment, reason); // cantco to false because not active assignment yet since it was a delegation
   }
}

} // namespace il
