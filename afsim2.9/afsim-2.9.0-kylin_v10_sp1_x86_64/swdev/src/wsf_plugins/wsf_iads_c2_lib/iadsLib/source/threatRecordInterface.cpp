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
*   File: threatRecord.cpp
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

#include <iadsLib/threatRecordInterface.hpp>

namespace il
{

threatRecord::threatRecord(std::weak_ptr<logger::GlobalLogger> aGlobalLogger, const int unranked_value)
   : m_unranked_value (unranked_value)
   , m_eval_track(aGlobalLogger)
   , m_rank (unranked_value)
   , m_allocatedzonetypes (E_NO_ZONES_SET)
   , m_evaluation (-1)
   , m_zone_evaluation (-1)
{
}



bool threatRecord::SortByRank (const std::shared_ptr<threatRecord> &t1, const std::shared_ptr<threatRecord> &t2)
{
   return t1->m_rank < t2->m_rank;
}

bool threatRecord::SortByEval (const std::shared_ptr<threatRecord> &t1, const std::shared_ptr<threatRecord> &t2)
{
   return t1->getEvaluation() < t2->getEvaluation();
}

bool threatRecord::SortByZoneEval (const std::shared_ptr<threatRecord> &t1, const std::shared_ptr<threatRecord> &t2)
{
   return t1->getZoneEvaluation() < t2->getZoneEvaluation();
}

const std::shared_ptr<trackRecord> & threatRecord::getTrack() const
{
   return m_eval_track.Track();
}

const evaluationTrack & threatRecord::getEvaluationTrack() const
{
   return m_eval_track;
}

evaluationTrack &threatRecord::getEvaluationTrack()
{
   return m_eval_track;
}

bool threatRecord::isUnranked () const
{
   return m_unranked_value == m_rank;
}

void threatRecord::setRank (int Rank)
{
   m_rank = Rank;
}


int  threatRecord::getRank () const
{
   return m_rank;
}

void threatRecord::setAllocateZoneTypes (const tZoneTypeMask zones)
{
   m_allocatedzonetypes = zones;
}


tZoneTypeMask threatRecord::getAllocatedZoneTypes () const
{
   return m_allocatedzonetypes;
}

void threatRecord::setEvaluation (const double Value)
{
   m_evaluation = Value;
}


double threatRecord::getEvaluation () const
{
   return m_evaluation;
}

void threatRecord::setZoneEvaluation (const double Value)
{
   m_zone_evaluation = Value;
}


double threatRecord::getZoneEvaluation () const
{
   return m_zone_evaluation;
}

} // namespace il
