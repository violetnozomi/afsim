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
*   File: weaponScoringMatrix.cpp
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

#include <algorithm>
#include <iadsLib/weaponScoringMatrix.hpp>

namespace il
{

weaponScoringMatrix::weaponScoringMatrix()
   : m_next_track_rank (1)
{

}

weaponScoringMatrix::weaponScoringMatrix(const tPairingArray &sorted_pairings)
   : m_next_track_rank (1)
{
   for (auto it = sorted_pairings.begin(); it != sorted_pairings.end(); ++it)
   {
      const auto &pairing = *it;
      AddPairingInOrder(pairing);
   }

   // sort the weapon assessments by score
   SortAllWeaponAssessmentsByScore ();
}

void weaponScoringMatrix::AddPairingInOrder (const std::shared_ptr<WeaponPairing> &pairing)
{
   // find whether we have already seen this track
   const auto &track_id = pairing->getTrack().Track()->getID();

   auto track_iter = m_track_rank_mappings.find (track_id);
   if (track_iter == m_track_rank_mappings.end())
   {
      m_track_rank_mappings[pairing->getTrack().Track()->getID()] = m_next_track_rank;
      m_ordered_pairings[m_next_track_rank].push_back (pairing);

      m_next_track_rank++;
   }
   else
   {
      auto rank = track_iter->second;
      auto &pairings = m_ordered_pairings[rank];
      pairings.push_back (pairing);
   }
}

std::size_t weaponScoringMatrix::GetNumRankedTracks () const
{
   return m_ordered_pairings.size();
}

const tPairingArray &weaponScoringMatrix::GetPairingForTrackRank (int track_rank)
{
   // get rank mapping
   const auto &rank_entry = m_ordered_pairings.find (track_rank);
   if (rank_entry == m_ordered_pairings.end())
      return m_empty_pairing;

   // good until invalidated
   return rank_entry->second;
}

void weaponScoringMatrix::SortAllWeaponAssessmentsByScore ()
{
   for (auto track_rank = m_ordered_pairings.begin(); track_rank != m_ordered_pairings.end(); ++track_rank)
   {
      std::sort (track_rank->second.begin(), track_rank->second.end(), [](const tPairingArray::value_type lhs, tPairingArray::value_type rhs) -> bool
      {
         return lhs->getWeaponScore() > rhs->getWeaponScore();
      });
   }
}

} // namespace il
