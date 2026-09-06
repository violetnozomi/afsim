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
*   File: trackContainers.hpp
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

#ifndef IADSLIB_TRACK_CONTAINERS_HPP
#define IADSLIB_TRACK_CONTAINERS_HPP

#include <vector>
#include <map>
#include <memory>

#include <iadsLib/idRecord.hpp>
#include "logger.hpp"

namespace il
{
   // forward declarations
   class trackRecord;
   class assignmentMessage;

   ///////////////
   // Track Map //
   ///////////////

   // Track Map - Basic track database structure where each track is stored by track id.
   // This is used for maintaining track states of the individual tracks
   typedef std::map<idRecord, std::shared_ptr<trackRecord>> trackMap;
   IADSLIB_EXPORT std::shared_ptr<trackRecord> FindTrack (const idRecord &track_id, const trackMap &tracks);

   /////////////////
   // Track Array //
   /////////////////
   // Track Array - Basic track array structure where we want to do linear processing on the track list
   // but where duplicates may exist. This can occur in evaluations for instance where we have multiple delegate
   // assignments on the same track.
   class IADSLIB_EXPORT evaluationTrack
   {
   public:
      evaluationTrack(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);

      evaluationTrack (const std::shared_ptr<trackRecord> &track, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);

      evaluationTrack (const std::shared_ptr<trackRecord> &track,
                       const std::shared_ptr<assignmentMessage> &original_assignment,
                       std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
      ~evaluationTrack() = default;
     

      const std::shared_ptr<trackRecord> &Track() const;
      const std::shared_ptr<assignmentMessage> &DelegateAssignment() const;

      std::shared_ptr<trackRecord> &Track();
      std::shared_ptr<assignmentMessage> &DelegateAssignment();

      const idRecord GetReferenceTrackID() const;
   protected:
      std::shared_ptr<trackRecord> m_track;
      std::shared_ptr<assignmentMessage> m_original_assignment;
      std::weak_ptr<logger::GlobalLogger> mGlobalLogger;
   };

   typedef std::vector<evaluationTrack> evalTrackArray;

}


#endif

