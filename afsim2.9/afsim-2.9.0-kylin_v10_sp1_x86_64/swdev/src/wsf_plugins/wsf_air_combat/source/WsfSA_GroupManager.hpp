// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSA_GROUPMANAGER_HPP
#define WSFSA_GROUPMANAGER_HPP

#include "wsf_air_combat_export.h"

#include "WsfSA_EntityPerception.hpp"
#include "WsfScenario.hpp"

class WsfSA_EntityPerception;
class WsfSA_Group;

class WSF_AIR_COMBAT_EXPORT WsfSA_GroupManager
{
public:
   virtual ~WsfSA_GroupManager() = default;

   void InitializeData(WsfPlatform* aPlatform,
                       double       aMaxGroupingDistanceCentroid,
                       double       aMaxGroupingDistanceNeighbor,
                       double       aMaxGroupingHeadingDifference,
                       double       aMaxGroupingSpeedDifference,
                       double       aMaxGroupingAngle,
                       double       aMinGroupRadius,
                       bool         aUseCentroidGrouping,
                       bool         aUseNeighborGrouping,
                       bool         aUseHeadingGrouping,
                       bool         aUseSpeedGrouping,
                       bool         aUseTypeGrouping);

   void UpdateAndAssessGroups(double                                      aSimtime,
                              const WsfPlatform*                          aPlatform,
                              std::list<WsfSA_Group*>&                    aCurrentlyPerceivedGroups,
                              const std::vector<WsfSA_EntityPerception*>& aPerceivedBogies,
                              const std::vector<WsfSA_EntityPerception*>& aPerceivedBandits,
                              bool                                        aIgnoreBogies);

   enum GroupingAlgorithm
   {
      SIMPLE_POSITIONAL         = 0, //<! Simple, very simple positional clustering
      SIMPLE_ANGULAR            = 1, //<! Simple, very simple angular clustering
      K_MEANS                   = 2, //<! K-Means, very simple clustering
      HIERARCHICAL_TREE_AVERAGE = 3, //<! H-Tree average-linkage, cluster distance is mean location distance
      HIERARCHICAL_TREE_COMPLETE = 4, //<! H-Tree complete-linkage, cluster distance is max between any two members (default)
      HIERARCHICAL_TREE_SINGLE = 5 //<! H-Tree single-linkage, cluster distance is min between any two members
   };

   enum ScoringAlgorithm
   {
      SCORE_2D  = 0, //<! Clustered by horizontal/lateral distance apart
      SCORE_3D  = 1, //<! Clustered by slant distance apart (default)
      SCORE_VEL = 2, //<! Clustered by heading
      SCORE_ANG = 3  //<! Clustered by angle from viewing platform
   };

   enum GroupingValidity
   {
      VALID,
      INVALID,
      INCONCLUSIVE
   };

   void SetGroupingAlgorithms(GroupingAlgorithm aGroupingAlgorithm, ScoringAlgorithm aScoringAlgorithm);

   /** Generates a unique group name.
    * @return  A unique group name.
    */
   std::string GenerateNewGroupName();

   /** Attempt to make aEntity join one of the groups in aCurrentlyPerceivedGroups.
    * aEntity may not join any group if it isn't considered "close enough" to any of them.
    * @param aEntity                    The entity wich may be joining a group.
    * @param aCurrentlyPerceivedGroups  The list of groups which the entity may join.
    * @return                           True if the entity joined a group, false otherwise.
    */
   bool AttemptToJoinBestGroup(WsfSA_EntityPerception* aEntity, const std::list<WsfSA_Group*>& aCurrentlyPerceivedGroups);

   /** Form degenerate groups for each entity in aAvailableList which doesn't have a parent group.
    * If an entity in aAvailableList already has a parent group, it is skipped.
    * @param aAvailableList             A list of entities for which to form a degenerate group.
    * @param aCurrentlyPerceivedGroups  The list of currenty perceived groups.
    */
   void FormDegenerateGroups(std::list<WsfSA_EntityPerception*>& aAvailableList,
                             std::list<WsfSA_Group*>&            aCurrentlyPerceivedGroups);

   /** Form a degenerate group for aEntity.
    * If aEntity already has a parent group, it is skipped.
    * @param aEntity                    The entity for which to form a degenerate group.
    * @param aCurrentlyPerceivedGroups  The list of currenty perceived groups.
    */
   void FormDegenerateGroup(WsfSA_EntityPerception* aEntity, std::list<WsfSA_Group*>& aCurrentlyPerceivedGroups);

private:
   void PositionalAlgorithm(double                                      aSimtime,
                            const WsfPlatform*                          aPlatform,
                            std::list<WsfSA_Group*>&                    aCurrentlyPerceivedGroups,
                            const std::vector<WsfSA_EntityPerception*>& aPerceivedBogies,
                            const std::vector<WsfSA_EntityPerception*>& aPerceivedBandits,
                            bool                                        aIgnoreBogies);

   void FormAvailableList(const std::vector<WsfSA_EntityPerception*>& aPerceivedBogies,
                          const std::vector<WsfSA_EntityPerception*>& aPerceivedBandits,
                          std::list<WsfSA_EntityPerception*>&         aAvailableList,
                          bool                                        aIgnoreBogies);

   // Returns true if groups is still viable, false if group should be removed
   bool VerifyGroupEntitiesAreValid(WsfSA_Group*                        aGroup,
                                    std::list<WsfSA_EntityPerception*>& aAvailableList,
                                    bool                                aTestForAvailable);

   // Returns true if the entity would be a valid member of the specified group
   bool VerifyEntityIsValidForGroup(const WsfSA_EntityPerception* aEntity, const WsfSA_Group* aGroup);

   // Returns true if the entity is able to join the specified group
   bool VerifyEntityIsValidForJoiningGroup(const WsfSA_EntityPerception* aEntity, const WsfSA_Group* aGroup);

   void AttemptToFormGroup(WsfSA_EntityPerception*             aEntity,
                           std::list<WsfSA_EntityPerception*>& aConsiderationList,
                           std::list<WsfSA_Group*>&            aCurrentlyPerceivedGroups);

   void UpdateGroupEntityData(std::list<WsfSA_Group*>&                  aCurrentlyPerceivedGroups,
                              const std::list<WsfSA_EntityPerception*>& aAvailableList);

   // This returns false if the entity is not in the availableList
   static bool EntityDataAvailable(WsfSA_EntityPerception*                   aEntity,
                                   const std::list<WsfSA_EntityPerception*>& aAvailableList);

   void UpdateCurrentGroups(const std::list<WsfSA_Group*>& aCurrentlyPerceivedGroups);

   void RemoveOldGroups(std::list<WsfSA_Group*>& aCurrentlyPerceivedGroups, double aSimtime);

   void MergeGroups(std::list<WsfSA_Group*>& aCurrentlyPerceivedGroups);

   bool EntityIsInList(const WsfSA_EntityPerception* aEntity, const std::list<WsfSA_EntityPerception*>& aAvailableList) const;

   bool EntityIsInList(const WsfSA_EntityPerception* aEntity, const std::list<WsfSA_EntityPerception>& aAvailableList) const;

   void RemoveEntityFromList(const WsfSA_EntityPerception* aEntity, std::list<WsfSA_EntityPerception*>& aAvailableList);

   bool ItemMayBeAddedToGroup(const WsfSA_EntityPerception* aEntity) const;

   // Returns true if the entity is contained in mInSomeGroup map
   bool EntityHasBeenAddedToSomeGroup(const WsfSA_EntityPerception* aEntity) const;

   // Returns true if the entity is contained in the asset group of the specified platform's SA Processor
   bool EntityIsInAssetGroup(const WsfSA_EntityPerception* aEntity, const WsfPlatform* aPlatform) const;

   // Returns true if the entity meets top-level qualifiers for being in a group:
   // It will be invalid (and return false) if any of the following are true:
   //   1) Entity has the same platform index as the mPlatform of the group manager
   //   2) Entity is in the assets group of the mPlatform of the group manager
   //   3) Entity identification is FRIENDLY
   //   4) Entity identification is NEUTRAL
   //   5) Entity is an angle-only track (this may change in the future)
   bool MeetsTopLevelValidityGroupQualifiers(const WsfSA_EntityPerception* aEntity) const;

   // Returns true if there is only a single entity in the element list of the group and it is has
   // identification of FRIENDLY or NEUTRAL.
   bool IsLastItemFriendlyOrNeutral(const WsfSA_Group* aGroup) const;

   // Returns VALID if the entity is close enough to the centroid of the group.
   // If centroid grouping is not being used, it returns INCONCLUSIVE.
   // Else, return INVALID.
   GroupingValidity CloseEnoughToCentroid(const WsfSA_EntityPerception* aEntity, const WsfSA_Group* aGroup) const;

   // Returns VALID if the entity is close enough to any neighbor in the group.
   // If neighbor grouping is not being used or there are no neighbors, it returns INCONCLUSIVE.
   // Else, return INVALID.
   GroupingValidity CloseEnoughToNeighbor(const WsfSA_EntityPerception* aEntity, const WsfSA_Group* aGroup) const;

   // Returns VALID if the entity is close enough to the overall heading of the group.
   // If heading grouping is not being used or either the entity or group have invalid headings, it returns
   // INCONCLUSIVE. Else, return INVALID.
   GroupingValidity CloseEnoughToHeading(const WsfSA_EntityPerception* aEntity, const WsfSA_Group* aGroup) const;

   // Returns VALID if the entity is close enough to the overall speed of the group.
   // If heading grouping is not being used or either the entity or group have invalid speeds, it returns INCONCLUSIVE.
   // Else, return INVALID.
   GroupingValidity CloseEnoughToSpeed(const WsfSA_EntityPerception* aEntity, const WsfSA_Group* aGroup) const;

   // Returns VALID if the entity is the same type as the entities in the group.
   // If heading grouping is not being used or there are no other entities, it returns INCONCLUSIVE.
   // Else, return INVALID.
   GroupingValidity MatchesType(const WsfSA_EntityPerception* aEntity, const WsfSA_Group* aGroup) const;

   // Returns true if aEntity is close enough in all relevant parameters to those of the group to remain in it, else false.
   bool CloseEnoughToStay(const WsfSA_EntityPerception* aEntity, const WsfSA_Group* aGroup);

   // Returns true if aEntity is close enough in all relevant parameters to those of the group to join it, else false.
   bool CloseEnoughToJoin(const WsfSA_EntityPerception* aEntity, const WsfSA_Group* aGroup);

   const WsfPlatform* mPlatform                     = nullptr;
   GroupingAlgorithm  mGroupingAlgorithm            = SIMPLE_POSITIONAL;
   ScoringAlgorithm   mScoringAlgorithm             = SCORE_2D;
   double             mMaxGroupingDistanceCentroid  = 8.0 * UtMath::cM_PER_NM;     // Meters  - Default is 8 nm
   double             mMaxGroupingDistanceNeighbor  = 4.0 * UtMath::cM_PER_NM;     // Meters  - Default is 4 nm
   double             mMaxGroupingHeadingDifference = 10.0 * UtMath::cRAD_PER_DEG; // Radians
   double             mMaxGroupingSpeedDifference   = 100.0;                       // Knots
   double             mMinGroupRadius_nm            = 1.0; // Meters  - Default minimum radius is 1 nm

   bool mUseCentroidGrouping = true;  // Utilize centroid distance grouping constraints
   bool mUseNeighborGrouping = true;  // Utilize neighbor distance grouping constraints
   bool mUseHeadingGrouping  = true;  // Utilize heading grouping constraints
   bool mUseSpeedGrouping    = true;  // Utilize speed grouping constraints
   bool mUseTypeGrouping     = false; // Utilize type grouping constraints

   int mGroupNumber = 0; // Used to auto-gen group names

   double mLastSimtime = 0.0; // Seconds
};

#endif
