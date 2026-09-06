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
*   File: threatRecord.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details: Base class for specialized threatRecord classes to inherit from.
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#ifndef IADSLIB_THREAT_RECORD_HPP
#define IADSLIB_THREAT_RECORD_HPP

#include <vector>
#include <map>
#include <string>
#include <memory>

#include <iadsLib/enum.hpp>
#include <iadsLib/idRecord.hpp>
#include <iadsLib/trackContainers.hpp>


namespace il
{
   class threatRecord;

   typedef std::pair<idRecord, std::shared_ptr<threatRecord>> threatPair;
   typedef std::multimap<idRecord, std::shared_ptr<threatRecord>> threatMap;
   typedef std::multimap<idRecord, std::shared_ptr<threatRecord>>::iterator threatMapIterator;
   typedef std::multimap<idRecord, std::shared_ptr<threatRecord>>::iterator threatMapIteratorConst;
   typedef std::vector<std::shared_ptr<threatRecord>> tRankedThreats;

   class IADSLIB_EXPORT threatRecord
   {
   public:
      threatRecord(std::weak_ptr<logger::GlobalLogger> aGlobalLogger, const int unranked_value = -1);
      ~threatRecord() = default;

      static bool SortByRank (const std::shared_ptr<threatRecord> &t1, const std::shared_ptr<threatRecord> &t2);
      static bool SortByEval (const std::shared_ptr<threatRecord> &t1, const std::shared_ptr<threatRecord> &t2);
      static bool SortByZoneEval (const std::shared_ptr<threatRecord> &t1, const std::shared_ptr<threatRecord> &t2);

      // users of this class will need to create an evaluate function that sets the track
      virtual const std::shared_ptr<trackRecord> & getTrack() const;
      virtual const evaluationTrack &getEvaluationTrack() const;
      virtual evaluationTrack &getEvaluationTrack();

      virtual bool isUnranked () const;
      virtual void setRank (int Rank);
      virtual int  getRank () const;

      virtual void setAllocateZoneTypes (const tZoneTypeMask zone);
      virtual tZoneTypeMask getAllocatedZoneTypes () const;

      virtual void setEvaluation (const double Value);
      virtual double getEvaluation () const;

      virtual void setZoneEvaluation (const double Value);
      virtual double getZoneEvaluation () const;

   protected:
      const int m_unranked_value;
      evaluationTrack m_eval_track;
      int m_rank;
      tZoneTypeMask m_allocatedzonetypes;

      double m_evaluation;
      double m_zone_evaluation;

   };
}


#endif

