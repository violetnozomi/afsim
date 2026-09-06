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
*   File: trackContainers.cpp
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

#include <iadsLib/trackContainers.hpp>
#include <iadsLib/assignmentMessage.hpp>

namespace il
{

evaluationTrack::evaluationTrack(std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : mGlobalLogger(aGlobalLogger)
{

}

evaluationTrack::evaluationTrack (const std::shared_ptr<trackRecord> &track, std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : m_track(track)
   , mGlobalLogger(aGlobalLogger)
{
}

evaluationTrack::evaluationTrack (const std::shared_ptr<trackRecord> &track,
                                  const std::shared_ptr<assignmentMessage> &original_assignment,
                                  std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : m_track(track)
   , m_original_assignment(original_assignment)
   , mGlobalLogger(aGlobalLogger)
{
}


const std::shared_ptr<trackRecord> & evaluationTrack::Track() const
{
   // the expectation is that the track must always be set, throw if it's being accessed and unset
   if (!m_track)
   {
      HCL_FATAL_LOGGER(mGlobalLogger) << "Error: Accessing evaluationTrack::Track() but track hasn't been set.";
      throw std::runtime_error ("Error: Accessing evaluationTrack::Track() but track hasn't been set.");
   }

   return m_track;
}

const std::shared_ptr<assignmentMessage> & evaluationTrack::DelegateAssignment() const
{
   return m_original_assignment;
}

std::shared_ptr<trackRecord> & evaluationTrack::Track()
{
   // the expectation is that the track must always be set, throw if it's being accessed and unset
   if (!m_track)
   {
      HCL_FATAL_LOGGER(mGlobalLogger) << "Error: Accessing evaluationTrack::Track() but track hasn't been set.";
      throw std::runtime_error ("Error: Accessing evaluationTrack::Track() but track hasn't been set.");
   }

   return m_track;
}

std::shared_ptr<assignmentMessage> & evaluationTrack::DelegateAssignment()
{
   return m_original_assignment;
}


const idRecord evaluationTrack::GetReferenceTrackID() const
{
   idRecord track_id;
   if (m_original_assignment)
      track_id = m_original_assignment->getReferenceTrackID();
   else if (m_track)
      track_id = m_track->getID();
   else
   {
      HCL_FATAL_LOGGER(mGlobalLogger) << "Error: Accessing evaluationTrack::GetReferenceTrackID() no track or delegation has been set.";
      throw std::runtime_error ("Error: Accessing evaluationTrack::GetReferenceTrackID() no track or delegation has been set.");
   }

   return track_id;
}

} // namespace il
