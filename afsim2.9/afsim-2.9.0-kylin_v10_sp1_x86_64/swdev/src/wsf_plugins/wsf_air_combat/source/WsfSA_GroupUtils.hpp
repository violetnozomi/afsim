// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSA_GROUPUTILS_HPP
#define WSFSA_GROUPUTILS_HPP

#include "wsf_air_combat_export.h"

#include <functional>
#include <list>

#include "WsfSA_EntityPerception.hpp"
#include "WsfSA_Group.hpp"

class WsfPlatform;

/*This pure static class provides testable and prtable functions for manipulating groups.*/
class WSF_AIR_COMBAT_EXPORT WsfSA_GroupUtils
{
public:
   enum SplitCriteria
   {
      cMAX_THREAT             = 1,
      cMIN_THREAT             = 2,
      cMAX_TARGET             = 3,
      cMIN_TARGET             = 4,
      cMAX_ALTITUDE           = 5,
      cMIN_ALTITUDE           = 6,
      cMAX_DISTANCE           = 7,
      cMIN_DISTANCE           = 8,
      cRIGHTMOST_FROM_HEADING = 9,
      cLEFTMOST_FROM_HEADING  = 10
   };

   enum GroupSorting
   {
      cASCENDING_THREAT        = 1,
      cDESCENDING_THREAT       = 2,
      cASCENDING_TARGET        = 3,
      cDESCENDING_TARGET       = 4,
      cASCENDING_ALTITUDE      = 5,
      cDESCENDING_ALTITUDE     = 6,
      cASCENDING_DISTANCE      = 7,
      cDESCENDING_DISTANCE     = 8,
      cASCENDING_ENTITY_COUNT  = 9,
      cDESCENDING_ENTITY_COUNT = 10,
      cASCENDING_RADIUS        = 11,
      cDESCENDING_RADIUS       = 12
   };

   /** Get a split criteria enumeration from a string.
    * @param aEnumString  The string from which to determine the split criteria.
    * @return             The enumeration determined from the string.
    */
   static int StringToSplitCriteria(const std::string& aEnumString);

   /** Get a group sorting enumeration from a string.
    * @param aEnumString  The string from which to determine the group sorting.
    * @return             The enumeration determined from the string.
    */
   static int StringToGroupSorting(const std::string& aEnumString);

   /** This calculates the number of PIs represented within a list of groups and entity perceptions.
    * @param aItemList  The list of currently recognized groups.
    * @return           The number of PIs represented within the list of recognized groups.*/
   static int CalculateNumPIs(const std::list<WsfSA_PerceivedItem*>& aItemList);

   /** This calculates the number of PIs represented within the list of recognized groups.
    * @param aGroupList  The list of currently recognized groups.
    * @return            The number of PIs represented within the list of recognized groups.*/
   static int CalculateNumPIs(const std::list<WsfSA_Group*>& aGroupList);

   /** This allows a group to be renamed.
    * @param aGroupList  The list of currently recognized groups.
    * @param aGroup      The group to rename.
    * @param aName       The new name.
    * @return            True if success.
    */
   static bool RenameGroup(const std::list<WsfSA_Group*>& aGroupList, WsfSA_Group* aGroup, const std::string& aName);

   /** Creates a new group and inserts all entities from the array argument into it.
    * As an exception to the general approach, if an entity in the array is already in another group,
    * it will be removed from that group, but that group will not be marked as protected.
    * @param aPI_Limit           The maximum number of PIs which should be allowed. If set to 0 or a negative number, it
    * is assumed to be infinite.
    * @param aPI_Current         The current number of recognized PIs.
    * @param aGroupList          The mutable list of currently recognized groups.
    * @param aOwningPlatform     A pointer to the platform from which this group originates.
    * @param aElements           The list of entities that should be contained within this new group.
    * @param aName               The name of the new group.
    * @param aMinGroupRadius_nm  Default settings of new groups created by this function.
    * @param aProtect            If this function was called from script, then this should be set to true
    *                            in order to protect the user's changes from being overridden by automatic merges or
    * splits. Otherwise, it should be false.
    * @return                    False if an entity being removed from an unfocused group causes the PI limit to be
    * exceeded, true otherwise.
    */
   static WsfSA_Group* CreateGroup(int                                       aPI_Limit,
                                   int                                       aPI_Current,
                                   std::list<WsfSA_Group*>&                  aGroupList,
                                   const WsfPlatform*                        aOwningPlatform,
                                   const std::list<WsfSA_EntityPerception*>& aElements,
                                   const std::string&                        aName,
                                   double                                    aMinGroupRadius_nm,
                                   bool                                      aProtect = false);

   /** Disbands the group.
    * This does not delete the entities within that group.
    * The group is deleted, so it should be considered invalid after successfully calling the function.
    * @param aPI_Limit           The maximum number of PIs which should be allowed. If set to 0 or a negative number, it
    * is assumed to be infinite.
    * @param aPI_Current         The current number of recognized PIs.
    * @param aGroupList          The mutable list of currently recognized groups.
    * @param aGroup              The group to disband.
    * @param aNamingFunc         A callable object which takes no object and returns a string which will be used to name
    * the degenerate groups created upon successful completion of DisbandGroup.
    * @param aMinGroupRadius_nm  Default settings of new groups created by this function.
    * @param aProtect            If this function was called from script, then this should be set to true
    *                            in order to protect the user's changes from being overridden by automatic merges or
    * splits. Otherwise, it should be false.
    * @return                    False if disbanding an unfocused group causes the PI limit to be exceeded, true
    * otherwise.
    */
   static bool DisbandGroup(int                          aPI_Limit,
                            int                          aPI_Current,
                            std::list<WsfSA_Group*>&     aGroupList,
                            WsfSA_Group*                 aGroup,
                            std::function<std::string()> aNamingFunc,
                            double                       aMinGroupRadius_nm,
                            bool                         aProtect = false);

   /** Merges the groups.
    * All groups are deleted, and a new group is created, so the original groups should be considered invalid after
    * successfully calling this function.
    * @param aGroupList          The mutable list of currently recognized groups.
    * @param aGroups             The list of groups to merge together.
    * @param aOwningPlatform     A pointer to the platform from which this group originates.
    * @param aMinGroupRadius_nm  Default settings of new groups created by this function.
    * @param aProtect            If this function was called from script, then this should be set to true
    *                            in order to protect the user's changes from being overridden by automatic merges or
    * splits. Otherwise, it should be false.
    * @return                    False if the groups cannot be merged due to conflicting focus, true otherwise.
    */
   static WsfSA_Group* MergeGroups(std::list<WsfSA_Group*>&       aGroupList,
                                   const std::list<WsfSA_Group*>& aGroups,
                                   const WsfPlatform*             aOwningPlatform,
                                   double                         aMinGroupRadius_nm,
                                   bool                           aProtect = false);

   /** Splits a group according to the splitCriteria enumeration.
    * The number of entities specified by the number argument will be added to a new group which fits splitCriteria.
    * The remainder will be put into the other created group.
    * The input group is deleted, so the original group should be considered invalid after successfully calling this
    * function.
    * @param aPI_Limit           The maximum number of PIs which should be allowed. If set to 0 or a negative number, it
    * is assumed to be infinite.
    * @param aPI_Current         The current number of recognized PIs.
    * @param aGroupList          The mutable list of currently recognized groups.
    * @param aGroup              The group to split.
    * @param aNumElements        The number of elements that will appear in the new group.
    * @param aCriterion          An enumeration describing how the group should be split.
    * @param aPlatform           The platform which is calling this function, which is necessary for some split
    * criteria.
    * @param aNewGroupName       The name given to the group containing the entities which met aCriterion.
    * @param aMinGroupRadius_nm  Default settings of new groups created by this function.
    * @param aProtect            If this function was called from script, then this should be set to true
    *                            in order to protect the user's changes from being overridden by automatic merges or
    * splits. Otherwise, it should be false.
    * @return                    A pair containing the remainder of the original group, followed by the new group
    * described by the split criteria and number of elements given.
    */
   static std::pair<WsfSA_Group*, WsfSA_Group*> SplitGroup(int                      aPI_Limit,
                                                           int                      aPI_Current,
                                                           std::list<WsfSA_Group*>& aGroupList,
                                                           WsfSA_Group*             aGroup,
                                                           int                      aNumElements,
                                                           int                      aCriterion,
                                                           WsfPlatform*             aPlatform,
                                                           const std::string&       aNewGroupName,
                                                           double                   aMinGroupRadius_nm,
                                                           bool                     aProtect = false);

   /** Transfers the entity into the group.
    * As an exception to the general approach, if the entity is already in another group,
    * it will be removed from that group, but that group will not be marked as protected.
    * @param aPI_Limit           The maximum number of PIs which should be allowed. If set to 0 or a negative number, it
    * is assumed to be infinite.
    * @param aPI_Current         The current number of recognized PIs.
    * @param aGroupList          The mutable list of currently recognized groups.
    * @param aEntity             The entity to transfer.
    * @param aGroup              The group into which the entity will be transferred.
    * @param aMinGroupRadius_nm  Default settings of new groups created by this function.
    * @param aProtect            If this function was called from script, then this should be set to true
    *                            in order to protect the user's changes from being overridden by automatic merges or
    * splits. Otherwise, it should be false.
    * @return                    False if transferring an entity from an unfocused group to a focused one causes the PI
    * limit to be exceeded, true otherwise.
    */
   static bool TransferEntity(int                      aPI_Limit,
                              int                      aPI_Current,
                              std::list<WsfSA_Group*>& aGroupList,
                              WsfSA_EntityPerception*  aEntity,
                              WsfSA_Group*             aGroup,
                              double                   aMinGroupRadius_nm,
                              bool                     aProtect = false);

   /** Removes an entity from a group, but does not delete the entity.
    * @param aPI_Limit             The maximum number of PIs which should be allowed.
    *                              If set to 0 or a negative number, it is assumed to be infinite.
    * @param aPI_Current           The current number of recognized PIs.
    * @param aGroupList            The mutable list of currently recognized groups.
    * @param aEntity               The entity to remove from its group.
    * @param aDegenerateGroupName  The name given to the entity's new degenerate group.
    * @param aMinGroupRadius_nm    Default settings of new groups created by this function.
    * @param aProtect              If this function was called from script, then this should be set to true
    *                              in order to protect the user's changes from being overridden by automatic merges or
    * splits. Otherwise, it should be false.
    * @return                      False if removing an entity from an unfocused group causes the PI limit to be
    * exceeded, true otherwise.
    */
   static bool RemoveEntityFromGroup(int                      aPI_Limit,
                                     int                      aPI_Current,
                                     std::list<WsfSA_Group*>& aGroupList,
                                     WsfSA_EntityPerception*  aEntity,
                                     const std::string&       aDegenerateGroupName,
                                     double                   aMinGroupRadius_nm,
                                     bool                     aProtect = false);

   /** Sets a group to focused or unfocused depending on whether focus is true or false, respectively.
    * @param aPI_Limit    The maximum number of PIs which should be allowed.
    *                     If set to 0 or a negative number, it is assumed to be infinite.
    * @param aPI_Current  The current number of recognized PIs.
    * @param aGroup       The group for which focus is set.
    * @param aFocus       Sets the group to focused if true, unfocused otherwise.
    * @param aProtect     If this function was called from script, then this should be set to true
    *                     in order to protect the user's changes from being overridden by automatic merges or splits.
    *                     Otherwise, it should be false.
    * @return             False if setting an unfocused group to focused causes the PI limit to be exceeded,
    *                     or if the group cannot be unfocused due to having only one entity.
    */
   static bool SetFocus(int aPI_Limit, int aPI_Current, WsfSA_Group* aGroup, bool aFocus, bool aProtect = false);

   /** Removes PIs which are in excess of the PI limit by unfocusing or removing groups.
    * This calls PruneRecursive with aImportance = NOMINAL and aPreserve = true.
    * @param aPI_Limit    The maximum number of allowable PIs.
    * @param aGroupList   The list of currently recognized groups.
    * @param aToRemove    Groups which were removed from the group list are added to this list. The entities
    *                     within those groups should stop being perceived.
    */
   static void Prune(int aPI_Limit, std::list<WsfSA_PerceivedItem*>& aGroupList, std::list<WsfSA_PerceivedItem*>& aToRemove);

   /** Removes PIs which are in excess of the PI limit by unfocusing or removing groups.
    * This runs recursively with more permissive parameters until the number of PIs is within the PI limit.
    * @param aPI_Limit    The maximum number of allowable PIs.
    * @param aImportance  The maximum importance level at which groups are allowed to be demoted or removed.
    * @param aPreserve    True if user-modified groups should be preserved.
    * @param aGroupList   The list of currently recognized groups.
    * @param aToRemove    Groups which were removed from the group list are added to this list. The entities
    *                     within those groups should stop being perceived.
    */
   static void PruneRecursive(int                              aPI_Limit,
                              WsfSA_PerceivedItem::Importance  aImportance,
                              bool                             aPreserve,
                              std::list<WsfSA_PerceivedItem*>& aGroupList,
                              std::list<WsfSA_PerceivedItem*>& aToRemove);

   /** Returns the groups sorted according to a sorting enumeration.
    * @param aSortingEnum   An enumeration which corresponds to a sorting order.
    * @param aOutputGroups  The vector populated with the group pointers, in sorted order.
    *                       This should be passed in as an empty group.
    * @param aGroupList     The original list of groups.
    * @param aPlatform      The owning platform, which is required for some sortings, such as distance from the
    * platform. This can be nullptr only for sortings which do not require it, such as by group radius.
    */
   static void SortedGroups(int                            aSortingEnum,
                            std::vector<WsfSA_Group*>&     aOutputGroups,
                            const std::list<WsfSA_Group*>& aGroupList,
                            const WsfPlatform*             aPlatform);

   // =======================
   // From WsfSA_GroupManager
   // =======================

   /** This recalculates the centroid and radius the given group.
    * @param aGroup              A pointer to the group which has its centroid and radius recalculated.
    * @param aMinGroupRadius_nm  The minimum radius that a group can have.
    */
   static void RecalculateGroupCentroid(WsfSA_Group* aGroup, double aMinGroupRadius_nm);

   /** Calculate the radius for the given group.
    * @param aGroup              A pointer to the group which has its radius recalculated.
    * @param aMinGroupRadius_nm  The minimum radius that a group can have.
    */
   static double CalcRadiusForGroup_nm(const WsfSA_Group* aGroup, double aMinGroupRadius_nm);

   /** Calculate the distance from a group's centroid to and entity.
    * @param aGroup   A pointer to the group whose centroid is used.
    * @param aEntity  A pointer to the entity whose location is used.*/
   static double DistanceFromCentroid(const WsfSA_Group* aGroup, const WsfSA_EntityPerception* aEntity);
};

#endif // WSFSA_GROUPUTILS_HPP
