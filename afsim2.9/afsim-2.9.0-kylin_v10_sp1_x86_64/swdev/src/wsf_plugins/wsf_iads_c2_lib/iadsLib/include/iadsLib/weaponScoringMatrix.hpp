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
*   File: weaponScoringMatrix.hpp
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

#ifndef IADSLIB_WEAPON_SCORING_ARRAY_HPP
#define IADSLIB_WEAPON_SCORING_ARRAY_HPP

#include <map>
#include <iadsLib/weaponPairing.hpp>


namespace il
{
   class weaponScoringMatrix
   {
   public:
      weaponScoringMatrix();
      weaponScoringMatrix(const tPairingArray &sorted_pairings);
      ~weaponScoringMatrix() = default;
      // adds pairing to correct track row and associates the rank
      // with the entry. Ranks are guaranteed to start at 1 and
      // increment by one for each new track added. Therefore, the
      // tracks should be added in ranked order
      void AddPairingInOrder (const std::shared_ptr<WeaponPairing> &pairing);

      std::size_t GetNumRankedTracks () const;
      const tPairingArray &GetPairingForTrackRank (int track_rank);

      void SortAllWeaponAssessmentsByScore ();
   protected:
      int m_next_track_rank;

      // this datastructure groups tracks by rank. Each track will have
      // all weapon pairings against it and each track will only have one rank
      // associated with it based on the global priority of that track pairing
      std::map<int, tPairingArray> m_ordered_pairings;
      // track id to rank map mappings
      std::map<idRecord, int> m_track_rank_mappings;

      const tPairingArray m_empty_pairing;
   };
}

#endif

