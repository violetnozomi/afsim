// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file assessmentRecord.hpp
 *  @author Kenneth R. Sewell III
 *          Kenneth.Sewell@wpafb.af.mil
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 *  Derived from gtibmifassessrec.spc 23 Aug 06 - Created. MDMcAllister
 */

#include <string>
#include <memory>

#include <vclMath/Vector3.hpp>

#include <iadsLib/enum.hpp>
#include <iadsLib/trackRecord.hpp>
#include <iadsLib/assetRecord.hpp>
#include <iadsLib/threatRecordInterface.hpp>
#include <iadsLib/weaponTable.hpp>
#include <iadsLib/interceptCalculatorInterface.hpp>
#include <iadsLib/weaponRecordInterface.hpp>
#include <iadsLib/nullTerrainEngine.hpp>

#ifndef IADSLIB_ASSESSMENTRECORD_HPP
#define IADSLIB_ASSESSMENTRECORD_HPP

/**
*  @brief Encapsulate all IADSLIB functions and classes.
*/
namespace il
{
// forward declarations
class assessmentRecord;
class assignmentMessage;
class threatRecord;

typedef std::vector<std::shared_ptr<assessmentRecord>> tAssessments;

/**
   @brief Class to hold weapon assessments.
*/
class IADSLIB_EXPORT assessmentRecord
{
   public:

      /**
         @brief Default constructor.
      */
      assessmentRecord(std::shared_ptr<InterceptCalculatorIface> xcept_iface, std::weak_ptr<logger::GlobalLogger> aGlobalLogger,
                        std::shared_ptr<terrainInterface> terrain_interface = std::shared_ptr<terrainInterface> (new nullTerrainEngine()));
      
      ~assessmentRecord() = default;

      // sets up engagement parameters (pretty much everything)
      void AssessSAMEngagement(const double simTime,
                                 const std::shared_ptr<weaponRecord>& weapon,
                                 const trackRecord& track,
                                 const bool use_delays_for_projection,
                                 const double assignment_delay,
                                 const double expected_execution_delay,
                                 const double projection_time_s,
                                 const double projection_incr_s);

      void AssessAIEngagement(const double simTime,
                              const std::shared_ptr<weaponRecord>& weapon,
                              const trackRecord& track,
                              const bool use_delays_for_projection,
                              const double assignment_delay,
                              const double projection_time_s,
                              const double projection_incr_s);

      // sets up interceptability estimates - does not set up threat or track variables or priorities
      void AssessWeapon(const double& simTime,
                        const std::shared_ptr<threatRecord>& threat,
                        const std::shared_ptr<weaponRecord>& weapon,
                        const weaponTable& weaponTable,
                        const bool use_delays_for_projection,
                        const double assignment_delay,
                        const double expected_execution_delay,
                        const double projected_time_s,
                        const double projected_time_incr_s);

      void setWeaponTypePriority(const int priority);
      int getWeaponTypePriority() const;

      void setWeaponSubtypePriority(const int priority);
      int getWeaponSubtypePriority() const;

      void setWeaponExcluded(const bool Excluded = true);
      bool isWeaponExcluded() const;

      inline double getInterceptTime() const
      {
         return m_InterceptTime;
      }

      inline double getProjectedTime() const
      {
         return m_ProjectedTime;
      }

      inline double getAssignmentTime() const
      {
         return m_AssignmentTime;
      }

      inline const std::shared_ptr<weaponRecord> getWeapon() const
      {
         return m_Weapon;
      }

      inline const std::shared_ptr<trackRecord>& getTrack() const
      {
         return m_Threat->getTrack();
      }

      inline idRecord getDirectC2ID() const
      {
         return m_DirectC2ID;
      }

      inline void setDirectC2ID(const idRecord& DirectC2ID)
      {
         m_DirectC2ID = DirectC2ID;
      }

      inline bool canInterceptTrack() const
      {
         return m_CanInterceptTrack;
      }

      inline bool isLOSMasked() const
      {
         return m_IsLOSMasked;
      }

      inline bool isInterceptInsideMaxRange() const
      {
         return m_InterceptInsideMaxRange;
      }

      inline bool isInterceptOutsideMinRange() const
      {
         return m_InterceptOutsideMinRange;
      }

      inline bool isInterceptInsideZone() const
      {
         return m_InterceptInsideZone;
      }

      inline bool isInterceptInsideC2Zone() const
      {
         return m_InterceptInsideC2Zone;
      }

      inline double getInterceptPK() const
      {
         return m_InterceptPK;
      }

      inline double getMaxRange() const
      {
         return m_MaxRange;
      }

      inline double getDistanceToPCA() const
      {
         return m_DistanceToPCA;
      }

      inline double getInterceptDistance() const
      {
         return m_InterceptDistance;
      }

      inline positionRecord getPredictedInterceptPt() const
      {
         return m_InterceptPoint;
      }

      inline double getDeflectionAngleRads() const
      {
         return m_DeflectionAngleRads;
      }

      inline int getPotentialShots() const
      {
         return m_PotentialShots;
      }

      void log() const;

      void setPriority(const tZonePriority& Priority);
      tZonePriority getPriority() const;

      const std::shared_ptr<assignmentMessage>& getExistingAssignment() const;
      const std::shared_ptr<threatRecord>& getAssessedThreatRecord() const;

      idRecord GetAssetID() const;
      tWeaponID GetWeaponID() const;

      inline tZoneTypeMask GetAssessedZoneTypes() const { return m_weapon_zone_types; }

   protected:
      bool GetEarliestSAMInterceptInZone(const double& simTime,
                                          const trackRecord& track,
                                          const std::shared_ptr<weaponRecord>& weapon,
                                          const double projected_time_s,
                                          const double projected_time_incr_s,
                                          double& interceptTime,
                                          double& interceptDistance,
                                          positionRecord& interceptPoint,
                                          double& projectedTime,
                                          double& estPK,
                                          double& max_range,
                                          bool   is_already_projected = false);

      bool GetEarliestAIInterceptInZone(const double& simTime,
                                          const trackRecord& track,
                                          const std::shared_ptr<weaponRecord>& weapon,
                                          const double projected_time_s,
                                          const double projected_time_incr_s,
                                          double& interceptTime,
                                          double& interceptDistance,
                                          positionRecord& interceptPoint,
                                          double& projectedTime,
                                          double& estPK,
                                          double& max_range);

      bool GetSAMIntercept(const trackRecord& track,
                           const std::shared_ptr<weaponRecord>& weapon,
                           double& interceptTime,
                           double& interceptDistance,
                           positionRecord& interceptPoint,
                           double& est_pk,
                           double& max_range);

      bool GetAIIntercept(const double sim_time_s,
                           const trackRecord& track,
                           const std::shared_ptr<weaponRecord>& weapon,
                           double& interceptTime,
                           double& interceptDistance,
                           positionRecord& interceptPoint,
                           double& est_pk,
                           double& max_range);

      il::eZoneType GetDefaultWeaponZoneType(const std::shared_ptr<weaponRecord>& weapon);

      // we dont' take ownership of weapon or threat so any copying or assignment of this class
      // will slice these members.
      std::shared_ptr<weaponRecord> m_Weapon;
      std::shared_ptr<threatRecord> m_Threat;
      idRecord m_DirectC2ID;

      int m_Summary;
      int m_WeaponTypePriority;
      int m_WeaponSubtypePriority;
      bool m_WeaponExcluded;
      bool m_ThreatInsideMaxRange;
      bool m_ThreatOutsideMinRange;
      bool m_CanInterceptTrack;
      bool m_IsLOSMasked;
      bool m_InterceptInsideMaxRange;
      bool m_InterceptOutsideMinRange;
      bool m_InterceptInsideZone;
      bool m_InterceptInsideC2Zone;
      double m_ProjectedTime;

      double m_InterceptAltitude;
      int    m_PotentialShots;

      // Heading from nose of threat to weapon.
      double m_DeflectionAngleRads;

      /// Calculated point of closest approach in geodetic.
      positionRecord m_ClosestApproach;

      double m_DistanceToPCA;
      double m_InterceptTime;
      double m_InterceptDistance;
      double m_InterceptPK;
      double m_MaxRange;

      // delays
      double m_AssignmentDelay;
      double m_TimeToFireDelay;
      double m_ExpectedExecutionDelay;

      /// Time when assignment must be made in order to hit at InterceptTime.
      double m_AssignmentTime;

      /// Calculated geodetic intercept point.
      positionRecord m_InterceptPoint;

      int m_KeepOutDA;

      // Priority of threat
      tZonePriority m_Priority;

      // zone types tied to the weapon that exists a valid assessment for (tied back to the base zone
      // type for the weapon if a weapon isn't explicitly tied to a zone (ex: AI => FEZ, SAM => MEZ)
      tZoneTypeMask  m_weapon_zone_types; // default to 0: no zones

      // we dont' take ownership of the intercept or terrain interfaces so any copying or assignment of this class
      // will slice these members.
      std::shared_ptr<InterceptCalculatorIface> m_xcept_iface;
      std::shared_ptr<terrainInterface>         m_terrain_interface;

      std::weak_ptr<logger::GlobalLogger> mGlobalLogger;

   private:

};
}
#endif
