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
*   File: assignmentContainers.hpp
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

#ifndef IADSLIB_ASSIGNMENT_CONTAINERS_HPP
#define IADSLIB_ASSIGNMENT_CONTAINERS_HPP

#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <iadsLib/enum.hpp>
#include <iadsLib/idRecord.hpp>
#include <iadsLib/trackContainers.hpp>
#include <iadsLib/cueMessage.hpp>
#include <iadsLib/trackRecord.hpp>
#include <iadsLib/sensorRecordInterface.hpp>
#include <iadsLib/moeLoggingInterface.hpp>

namespace il
{
   // forward declarations
   class assignmentMessage;
   class threatRecord;
   class assessmentRecord;

   //////////////////////
   // Assignment Array //
   //////////////////////
   class IADSLIB_EXPORT evaluationAssignment
   {
      public:
         evaluationAssignment(const evaluationTrack& track,
                              const std::shared_ptr<assignmentMessage>& evaluated_assignment);
         ~evaluationAssignment() = default;
         const std::shared_ptr<trackRecord>& Track() const;
         const std::shared_ptr<assignmentMessage>& DelegateAssignment() const;
         const std::shared_ptr<assignmentMessage>& Assignment() const;

         std::shared_ptr<trackRecord>& Track();
         std::shared_ptr<assignmentMessage>& DelegateAssignment();
         std::shared_ptr<assignmentMessage>& Assignment();

         bool IsOverridden() const;
         bool IsShotDoctrineUpdated() const;

         // let dereference of an evaluationAssignment pointer refer to the evalauted assignment pointer for convenience
         const std::shared_ptr<assignmentMessage>& operator->() const;
         // create an implicit conversion to allow an evaluationAssignment to be converted directly to an assignment (using the evaluated assignment)
         operator const std::shared_ptr<assignmentMessage>& () const;
         operator std::shared_ptr<assignmentMessage>& ();

      protected:
         evaluationTrack                      m_evaluated_track;
         std::shared_ptr<assignmentMessage> m_evaluated_assignment; // guaranteed to be present
   };


   ////////////////////
   // Assignment Map //
   ////////////////////
   typedef std::vector<evaluationAssignment> evalAssignmentArray;

   typedef std::multimap<idRecord, evaluationAssignment> evalAssignmentMultimap;

   typedef std::map<idRecord, std::shared_ptr<assignmentMessage>> assignmentMap;
   typedef assignmentMap::iterator assignmentMapIterator;
   typedef assignmentMap::const_iterator assignmentMapConstIterator;

   // Assignment multimap iteration helper - this is to help get iteration of multi-maps correct since it is very
   // fraught with potential errors. The main thing the caller has to provide is a lambda or functor of the form:
   //
   //    void (evalAssignmentMultimap &assignments, evalAssignmentMultimap::iterator &assigned_track_itor, evalAssignmentMultimap::iterator &assignment_itor, <User Specified Ptr>) -> void
   //
   // the provided function must handle iteration of assignment iter (this is the inner iterator, in the current incarnation)
   //
   // By pushing iteration on the invoking function, this allows a lot of flexibility wrt destructive loops and simple loops.
   template <typename tFunctor, typename tUsrPtr>
   void MapAssignmentArrayFunc(il::moeLoggingInterface& aMOELogger, evalAssignmentMultimap& assignment_array, tFunctor func, tUsrPtr* usrPtr)
   {
      for (auto assigned_track_itor = assignment_array.begin(), assign_itor = assigned_track_itor;
           assigned_track_itor != assignment_array.end();
           assigned_track_itor = assign_itor)
      {
         auto curr_track_id = assigned_track_itor->first; // save value in case of a destructive inner lambda
         for (; assign_itor != assignment_array.end() && assign_itor->first == curr_track_id;)
         {
            func(aMOELogger, assignment_array, assigned_track_itor, assign_itor, usrPtr);
         }
      }
   }

   // Sensor-manager related bookkeeping
   class ResponsibleRadarCue
   {
      public:
         ResponsibleRadarCue(std::shared_ptr<sensorRecord> sensor, const bool skipped_previous = false);

         std::shared_ptr<sensorRecord> GetSensor() const;

         void SetCued(const double cue_time_s);
         bool IsCued() const;
         double GetCuedTime() const;

         void SetTracking(const double tracking_time_s);
         bool IsTracking() const;
         double GetTrackingTime() const;

         bool SkippedPreviousSensor() const;

      private:
         bool m_is_cued;
         double m_cue_time_s;

         bool m_is_tracking;
         double m_tracking_time;
         std::shared_ptr<sensorRecord> m_sensor;
         bool m_skipped_previous;
   };

   class IADSLIB_EXPORT ResponsibleRadarAssignment
   {
      public:
         ResponsibleRadarAssignment(std::shared_ptr<trackRecord> track, std::shared_ptr<assignmentMessage> assignment, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);

         void MasterTrackUpdated(const double sim_time, const idRecord& invoking_asset, const std::shared_ptr<const trackRecord>& track);
         inline std::shared_ptr<const trackRecord> GetTrackRecord() const { return m_assignment_master_track; }
         inline std::shared_ptr<assignmentMessage> GetAssignment() const { return m_assignment; }

         // responsibility and cued are two separate concepts. a radar can be responsible but not cued until the cue goes out

         /////////
         // TAR //
         /////////
         bool HasTARResponsibility() const;
         bool IsTARCued() const;

         void SetTARTracking(const double time_s);
         bool IsTARTracking() const;
         double GetTARTrackingTime() const;

         std::shared_ptr<sensorRecord> GetResponsibleTAR() const;

         double GetCuedTARTime() const;

         void SetTARCued(const double time_s);
         bool AddTARResponsibility(std::shared_ptr<sensorRecord>& sensor);
         void RemoveTARResponsibility();

         /////////
         // TTR //
         /////////
         bool HasTTRResponsibility() const;
         bool IsTTRCued() const;
         bool WasTARSkipped() const;

         void SetTTRTracking(const double time_s);
         bool IsTTRTracking() const;
         double GetTTRTrackingTime() const;

         std::shared_ptr<sensorRecord> GetResponsibleTTR() const;
         double GetCuedTTRTime() const;

         void SetTTRCued(const double time_s);
         bool AddTTRResponsibility(std::shared_ptr<sensorRecord>& sensor, const bool skipping_tar = false);
         void RemoveTTRResponsibility();

         // more generic interfaces
         bool IsRadarCued() const;

      private:
         std::shared_ptr<const trackRecord> m_assignment_master_track;
         std::shared_ptr<assignmentMessage> m_assignment;

         bool m_has_responsible_tar;
         ResponsibleRadarCue m_responsible_tar;

         bool m_has_responsible_ttr;
         ResponsibleRadarCue m_responsible_ttr;

         std::weak_ptr<logger::GlobalLogger> mGlobalLogger;
   };

   class ResponsibleAssignments
   {
      public:
         bool HasResponsibility(const std::shared_ptr<const trackRecord>& assignment_master_track) const;

         // will create a responsible assignment if doesn't exist
         ResponsibleRadarAssignment& GetResponsibleAssignment(std::shared_ptr<trackRecord> assignment_master_track, std::shared_ptr<assignmentMessage> assignment, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
         // will not create a responsible assignmen if doesn't exist, instead, will throw an exception.
         ResponsibleRadarAssignment& GetResponsibleAssignment(std::shared_ptr<trackRecord> assignment_master_track, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);

         IADSLIB_EXPORT void ForeachResponsibleAssignment(il::moeLoggingInterface& aMOELogger, std::function<void(il::moeLoggingInterface& aMOELogger, const tTrackID& master_track_id, ResponsibleRadarAssignment& responsible_assignment)> to_invoke);

         void Delete(const tTrackID& master_track_id);

      private:
         // responsible assignments are cues that are being sent by the sensor manager
         std::map <tTrackID, ResponsibleRadarAssignment> m_responsible_assignments;  // master track, ResponsibleAssignment
   };

   class ActiveCue
   {
      public:
         ActiveCue(const std::shared_ptr<const trackRecord>& master_track, const std::shared_ptr<il::cueMessage>& cue, const std::shared_ptr<sensorRecord>& sensor);

      private:
         std::shared_ptr<const trackRecord> m_assignment_master_track;
         std::shared_ptr<il::cueMessage> m_cue;
         std::shared_ptr<sensorRecord> m_sensor;
   };

   class ActiveCues
   {
      public:
         bool IsExistingCue(const tSensorID& sensor_id, const tTrackID& master_track_id) const;
         void HandleCue(const double curr_sim_time_s, const std::shared_ptr<const trackRecord>& master_track, const std::shared_ptr<il::cueMessage>& cue, const std::shared_ptr<sensorRecord>& sensor);

      private:
         std::map <tSensorID, std::map<tTrackID, ActiveCue>> m_active_cues; // sensorID, map<trackId, ActiveCue>
         std::map <tSensorID, bool> m_turn_off_if_no_cues; // keeps track of desire of cue sender to keep radar active or not after cue processing has finished
   };
}

#endif
