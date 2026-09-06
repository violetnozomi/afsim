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

#ifndef WSFSA_PERCEIVE_HPP
#define WSFSA_PERCEIVE_HPP

#include "wsf_air_combat_export.h"

#include <random>
#include <set>

#include "WsfSA_GroupManager.hpp"
#include "WsfSA_Module.hpp"
class WsfExplicitWeapon;
class WsfSA_Processor;
#include <unordered_map>
#include <unordered_set>

#include "WsfTrackId.hpp"

/** This is the perceive module of the SA Processor */
class WSF_AIR_COMBAT_EXPORT WsfSA_Perceive : WsfSA_Module
{
public:
   friend class WsfSA_Processor;
   friend class WsfSA_ProcessorUpdateEvent;

   enum RecipientType
   {
      cCOMMANDER       = 1,
      cPEERS           = 2,
      cSUBORDINATES    = 4,
      cALLCOMMANDERS   = 8,
      cALLSUBORDINATES = 16
   };

   WsfSA_Perceive(WsfSA_Processor* aSA_ProcPtr, WsfSA_Processor::PerceiveData* aDataPtr);
   WsfSA_Perceive& operator=(const WsfSA_Perceive&) = delete;
   ~WsfSA_Perceive();

   /** Add a cognitive loading time (delay)
    * @param aTimeDelay time to delay
    */
   void AddExtraCognitiveLoading(double aTimeDelay);

   /** Mark an asset as important
    * @param aAsset platform to mark as important
    */
   void MarkAssetAsImportant(WsfPlatform& aAsset);

   /** Mark an asset as important
    * @param aIndex index of the platform to mark as important
    */
   void MarkAssetAsImportant(size_t aIndex);

   /** Mark an asset type as important
    * @param aType type to mark as important
    */
   void MarkAssetTypeAsImportant(const std::string& aType);

   /** Mark an asset type as unimportant
    * @param aType type to mark as unimportant
    */
   void MarkAssetTypeAsUnimportant(const std::string& aType);

   /** Mark an asset as unimportant
    * @param aAsset platform to mark as unimportant
    */
   void MarkAssetAsUnimportant(WsfPlatform& aAsset);

   /** Mark an asset as unimportant
    * @param aIndex index of the platform to mark as unimportant
    */
   void MarkAssetAsUnimportant(size_t aIndex);

   /** Mark a threat as important
    * @param aThreat track to mark as important
    */
   void MarkThreatAsImportant(WsfLocalTrack& aThreat);

   /** Mark a threat as unimportant
    * @param aThreat track to mark as unimportant
    */
   void MarkThreatAsUnimportant(WsfLocalTrack& aThreat);

   /** Mark a threat type as important
    * @param aType type to mark as important
    */
   void MarkThreatTypeAsImportant(const std::string& aType);

   /** Mark a threat type as unimportant
    * @param aType type to mark as unimportant
    */
   void MarkThreatTypeAsUnimportant(const std::string& aType);

   /** Return the max asset load the platform can handle */
   int MaxAssetLoad() { return mDataPtr->mMaxAssetLoad; }

   /** Return the max threat load the platform can handle */
   int MaxThreatLoad() { return mDataPtr->mMaxThreatLoad; }

   /** Find an asset in the perceived asset list by platform index.
    * If the asset isn't found, return nullptr.
    * @param aPlatformIndex index of the platform asset
    */
   const WsfSA_EntityPerception* PerceivedAsset(size_t aPlatformIndex) const;

   /** Return a reference to the list of perceived assets for the current
    * platform at a given time
    * @param aSimTime sim time at which to get list of perceived assets
    */
   const std::vector<WsfSA_EntityPerception*>& PerceivedAssets(double aSimTime);

   /** Return a reference to the list of perceived aircraft assets for the current
    * platform at a given time
    * @param aSimTime sim time at which to get list of perceived assets
    */
   const std::vector<WsfSA_EntityPerception*>& PerceivedAircraftAssets(double aSimTime);

   /** Return a reference to the list of missile perceived assets for the current
    * platform at a given time
    * @param aSimTime sim time at which to get list of perceived assets
    */
   const std::vector<WsfSA_EntityPerception*>& PerceivedMissileAssets(double aSimTime);

   /** Return a reference to the list of perceived bogies for the current
    * platform at a given time
    * @param aSimTime sim time at which to get list of perceived bogies
    */
   const std::vector<WsfSA_EntityPerception*>& PerceivedBogies(double aSimTime);

   /** Return a reference to the list of perceived aircraft bogies for the current
    * platform at a given time
    * @param aSimTime sim time at which to get list of perceived aircraft bogies
    */
   const std::vector<WsfSA_EntityPerception*>& PerceivedAircraftBogies(double aSimTime);

   /** Return a reference to the list of perceived missile bogies for the current
    * platform at a given time
    * @param aSimTime sim time at which to get list of perceived missile bogies
    */
   const std::vector<WsfSA_EntityPerception*>& PerceivedMissileBogies(double aSimTime);

   /** Return a reference to the list of perceived missile bandits for the current
    * platform at a given time
    * @param aSimTime sim time at which to get list of perceived bandits
    */
   const std::vector<WsfSA_EntityPerception*>& PerceivedBandits(double aSimTime);

   /** Return a reference to the list of perceived aircraft bandits for the current
    * platform at a given time
    * @param aSimTime sim time at which to get list of perceived aircraft bandits
    */
   const std::vector<WsfSA_EntityPerception*>& PerceivedAircraftBandits(double aSimTime);

   /** Return a reference to the list of perceived missile bandits for the current
    * platform at a given time
    * @param aSimTime sim time at which to get list of perceived missile bandits
    */
   const std::vector<WsfSA_EntityPerception*>& PerceivedMissileBandits(double aSimTime);

   /** Find a threat in the perceived threat list by string id.
    * If the threat isn't found, return nullptr.
    * @param aTargetNameId string id of the threat
    */
   WsfLocalTrack* PerceivedThreat(WsfStringId aTargetNameId);

   /** Find a threat in the perceived threat list by track id.
    * If the threat isn't found, return nullptr.
    * @param aTrackId track id of the threat
    */
   WsfLocalTrack* PerceivedThreat(const WsfTrackId& aTrackId);

   /** Return true if a track is a possible enemy, false otherwise
    * @param aTrackPtr track to check
    */
   bool PossibleEnemy(WsfLocalTrack* aTrackPtr);

   /** Process input from script
    * @param aInput input to parse
    */
   bool ProcessInput(UtInput& aInput);

   /** Process a WsfMessage
    * @param aSimTime sim time when processing
    * @param aMessage message to handle
    */
   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage);

   /** Performs initialization during creation. This should be called
    * from the SA Processor's Initialize() function.
    * @param aSimTime sim time when processing
    */
   void Initialize(double aSimTime);

   // Asset functions --------------------------------------------------------------------------------------

   /** Set the max asset load
    * @param aLoad number of assets a platform can cognitively handle (negative value means no limit)
    */
   void SetMaxAssetLoad(int aLoad) { mDataPtr->mMaxAssetLoad = aLoad; }

   /** Set the max asset load to the value provided during initialization (default is -1 where not provided) */
   void ResetMaxAssetLoad() { mDataPtr->mMaxAssetLoad = mDataPtr->mMaxAssetLoadInitial; }

   /** Score an asset.  Return true if scored, false otherwise
    * @param aSimTime time at which to calc score
    * @param aEntityPtr asset to score
    * @param aScore reference variable to hold the score
    */
   bool AssetScore(double aSimTime, WsfSA_EntityPerception* aEntityPtr, double& aScore);

   // Returns true if the entity is contained in the current asset list
   bool EntityIsInAssetGroup(const WsfSA_EntityPerception* aEntity) const;

   // Returns true if the entity index is contained in the current asset list
   bool EntityIsInAssetGroup(size_t aEntityIndex) const;

   // Purges old assets based on lifetime
   void PurgeOldAssets(double aSimTime);

   /** Return the nearest asset entity perception (WsfSA_EntityPerception).
    * If no such entity exists, return nullptr.
    * @param aSimTime time at which to get entity
    */
   WsfSA_EntityPerception* NearestAsset(double aSimTime);

   /** Return the nearest aircraft asset entity perception (WsfSA_EntityPerception).
    * If no such entity exists, return nullptr.
    * @param aSimTime time at which to get entity
    */
   WsfSA_EntityPerception* NearestAircraftAsset(double aSimTime);

   /** Return the nearest missile asset entity perception (WsfSA_EntityPerception).
    * If no such entity exists, return nullptr.
    * @param aSimTime time at which to get entity
    */
   WsfSA_EntityPerception* NearestMissileAsset(double aSimTime);

   // Threat (Bogie and Bandit) functions ------------------------------------------------------------------

   /** Set the max asset load.
    * @param aLoad number of threats a platform can cognitively handle (negative value means no limit)
    */
   void SetMaxThreatLoad(int aLoad) { mDataPtr->mMaxThreatLoad = aLoad; }

   /** Set the max threat load to the value provided during initialization (default is -1 where not provided) */
   void ResetMaxThreatLoad() { mDataPtr->mMaxThreatLoad = mDataPtr->mMaxThreatLoadInitial; }

   /** Score a threat. Return true if scored, false otherwise.
    * @param aSimTime time when calculated
    * @param aThreatPtr threat to score
    * @param aIsMissile indicates if track is perceived as a missile
    * @param aScore reference variable in which to hold the score
    */
   bool ThreatScore(double aSimTime, WsfLocalTrack* aThreatPtr, bool aIsMissile, double& aScore);

   /** If a threat scoring function isn't provided by script, this is what's used instead.
    * @param aSimTime time when calculated
    * @param aThreatPtr threat to score
    */
   double DefaultThreatScore(double aSimTime, WsfSA_PerceivedItem* aThreatPtr);

   /** Return the nearest bogie entity perception (WsfSA_EntityPerception).
    * If no such entity exists, return nullptr.
    */
   WsfSA_EntityPerception* NearestBogie() const;

   /** Return the nearest aircraft bogie entity perception (WsfSA_EntityPerception).
    * If no such entity exists, return nullptr.
    */
   WsfSA_EntityPerception* NearestAircraftBogie() const;

   /** Return the nearest missile bogie entity perception (WsfSA_EntityPerception).
    * If no such entity exists, return nullptr.
    */
   WsfSA_EntityPerception* NearestMissileBogie() const;

   /** Return the nearest bandit entity perception (WsfSA_EntityPerception).
    * If no such entity exists, return nullptr.
    */
   WsfSA_EntityPerception* NearestBandit() const;

   /** Return the nearest aircraft bandit entity perception (WsfSA_EntityPerception).
    * If no such entity exists, return nullptr.
    */
   WsfSA_EntityPerception* NearestAircraftBandit() const;

   /** Return the nearest missile bandit entity perception (WsfSA_EntityPerception).
    * If no such entity exists, return nullptr.
    */
   WsfSA_EntityPerception* NearestMissileBandit() const;

   // Miscellaneous functions ------------------------------------------------------------------------------

   /** Return a set of assets based on mTruthPerceptionMask.
    * This is a utility method for when asset knowledge is truth data. */
   std::set<WsfPlatform*> TruthAssets();

   /** These functions provide data related to the selected weapon --------------------------------------- */

   /** Returns the name of the selected weapon */
   std::string SelectedWeaponName() const;

   /** Returns a pointer to the selected weapon */
   const WsfExplicitWeapon* SelectedWeapon() const;

   /** Returns the available quantity of the selected weapon */
   int QuantityOfSelectedWeapon() const;

   /** Returns true if the weapon selection was successful, else false
    * @param aWeaponName name of the desired weapon
    */
   bool SetSelectedWeapon(const std::string& aWeaponName);

   /** Returns the current list of weapons */
   const std::vector<WsfExplicitWeapon*> GetWeaponList() const { return mDataPtr->mWeaponList; }

   /** Sets the master arm to the specified condition
    * @param aArmed set to true to arm, false to safe
    */
   void SetMasterArm(bool aArmed);

   /** Returns true if the master arm is active (armed), else false */
   bool GetMasterArm() const;

   /** Returns true if the master warning is active, else false */
   bool MasterWarningActive() const;

   /** Returns true if the master warning is active, else false */
   bool MasterCautionActive() const;

   /** Returns true if the weapon bay doors are open, else false */
   bool WeaponBayDoorsAreOpen() const;

   /** Returns true if the shoot cue is active, else false */
   bool ShootCueActive() const;

   /** Returns true if the shoot cue is blinking, else false */
   bool ShootCueBlinking() const;

   /** Returns true if radar jamming is detected by local radar sensors, else false */
   bool JammingDetected() const;

   /** Sets the master warning to the specified condition
    * @param aActive set to true if active, false if off
    */
   void SetMasterWarningActive(bool aActive);

   /** Sets the master caution to the specified condition
    * @param aActive set to true if active, false if off
    */
   void SetMasterCautionActive(bool aActive);

   /** Sets the weapon bay doors to the specified condition
    * @param aOpen set to true if doors are open, false if doors are closed
    */
   void SetWeaponBayDoorsAreOpen(bool aOpen);

   /** Sets the shoot cue to the specified condition
    * @param aActive set to true if active, false if off
    */
   void SetShootCueActive(bool aActive);

   /** Sets the shoot cue blinking to the specified condition
    * @param aBlinking set to true if blinking, false if not
    */
   void SetShootCueBlinking(bool aBlinking);

   /** Sets the jamming detected flag to the specified condition
    * @param aJammingDetected set to true if jamming is detected, false if no jamming is detected
    */
   void SetJammingDetected(bool aJammingDetected);

   /** These functions provide data related to the primary target being engaged */

   /** Returns true if there is an engaged target */
   bool EngagedTargetValid() const { return mDataPtr->mEngagedTarget.isValid; }

   /** Returns an EngagedTarget struct for the primary engaged target */
   WsfSA_Processor::SA_EngagedTarget GetEngagedTarget() const { return mDataPtr->mEngagedTarget; }

   /** Sets the engaged target using the specified engaged target data
    * @param aEngagedTarget engaged target data
    */
   void SetEngagedTarget(const WsfSA_Processor::SA_EngagedTarget& aEngagedTarget)
   {
      mDataPtr->mEngagedTarget = aEngagedTarget;
   }

   /** These functions provide data related to expendables */

   /** Returns the available quantity of the chaff cartridges */
   int QuantityOfChaff() const;

   /** Returns the available quantity of the flare cartridges */
   int QuantityOfFlares() const;

   /** Returns the available quantity of the expendable decoys */
   int QuantityOfDecoys() const;

   /** Returns true if there was chaff available and dispenses a deployment of chaff */
   bool DispenseChaff(double aSimTime);

   /** Returns true if there was a flare available and dispenses a deployment of flare(s) */
   bool DispenseFlare(double aSimTime);

   /** Returns true if there was a decoy available and dispenses a single decoy */
   bool DispenseDecoy(double aSimTime);

   // Group functions

   /** This allows a group to be renamed.
    * @param aGroup  The group to rename.
    * @param aName   The new name.
    * @return        True if success.
    */
   bool RenameGroup(WsfSA_Group* aGroup, const std::string& aName);

   /** Creates a new group and inserts all entities from the array argument into it.
    * As an exception to the general approach, if an entity in the array is already in another group,
    * it will be removed from that group, but that group will not be marked as protected.
    * @param aElements  The list of entities that should be contained within this new group.
    * @param aProtect   If this function was called from script, then this should be set to true
    *                   in order to protect the user's changes from being overridden by automatic merges or splits.
    *                   Otherwise, it should be false.
    * @return  False if an entity being removed from an unfocused group causes the PI limit to be exceeded, true otherwise.
    */
   WsfSA_Group* CreateGroup(const std::list<WsfSA_EntityPerception*>& aElements, bool aProtect = false);

   /** Disbands the group.
    * This does not delete the entities within that group.
    * The group is deleted, so it should be considered invalid after successfully calling the function.
    * @param aGroup    The group to disband.
    * @param aProtect  If this function was called from script, then this should be set to true
    *                  in order to protect the user's changes from being overridden by automatic merges or splits.
    *                  Otherwise, it should be false.
    * @return          False if disbanding an unfocused group causes the PI limit to be exceeded, true otherwise.
    */
   bool DisbandGroup(WsfSA_Group* aGroup, bool aProtect = false);

   /** Merges the groups.
    * All groups are deleted, and a new group is created, so the original groups should be considered invalid after
    * successfully calling this function.
    * @param aGroups   A list of all of the groups which will be merged.
    *                  The group pointers in this list will be invalid after this function successfuly completes.
    * @param aProtect  If this function was called from script, then this should be set to true
    *                  in order to protect the user's changes from being overridden by automatic merges or splits.
    *                  Otherwise, it should be false.
    * @return          False if the groups cannot be merged due to conflicting focus, true otherwise.
    */
   WsfSA_Group* MergeGroups(const std::list<WsfSA_Group*>& aGroups, bool aProtect = false);

   /** Splits a group according to the splitCriteria enumeration.
    * The number of entities specified by the number argument will be added to a new group which fits splitCriteria.
    * The remainder will be put into the other created group.
    * The input group is deleted, so the original group should be considered invalid after successfully calling this function.
    * @param aGroup        The group to split.
    * @param aNumElements  The number of elements that will appear in the new group.
    * @param aCriterion    An enumeration describing how the group should be split.
    * @param aProtect      If this function was called from script, then this should be set to true
    *                      in order to protect the user's changes from being overridden by automatic merges or splits.
    *                      Otherwise, it should be false.
    * @return              The new groups upon success, or a pair of null pointers upon failure.
    */
   std::pair<WsfSA_Group*, WsfSA_Group*>
   SplitGroup(WsfSA_Group* aGroup, int aNumElements, int aCriterion, bool aProtect = false);

   /** Transfers the entity into the group.
    * As an exception to the general approach, if the entity is already in another group,
    * it will be removed from that group, but that group will not be marked as protected.
    * @param aEntity   The entity to transfer.
    * @param aGroup    The group into which the entity will be transferred.
    * @param aProtect  If this function was called from script, then this should be set to true
    *                  in order to protect the user's changes from being overridden by automatic merges or splits.
    *                  Otherwise, it should be false.
    * @return          False if transferring an entity from an unfocused group to a focused one causes the PI limit to
    * be exceeded, true otherwise.
    */
   bool TransferEntity(WsfSA_EntityPerception* aEntity, WsfSA_Group* aGroup, bool aProtect = false);

   /** Removes an entity from a group, but does not delete the entity.
    * @param aEntity   The entity to remove from its group.
    * @param aProtect  If this function was called from script, then this should be set to true
    *                  in order to protect the user's changes from being overridden by automatic merges or splits.
    *                  Otherwise, it should be false.
    * @return          False if removing an entity from an unfocused group causes the PI limit to be exceeded, true otherwise.
    */
   bool RemoveEntityFromGroup(WsfSA_EntityPerception* aEntity, bool aProtect = false);

   /** Sets a group to focused or unfocused depending on whether focus is true or false, respectively.
    * @param aGroup    The group for which focus is set.
    * @param aFocus    Sets the group to focused if true, unfocused otherwise.
    * @param aProtect  If this function was called from script, then this should be set to true
    *                  in order to protect the user's changes from being overridden by automatic merges or splits.
    *                  Otherwise, it should be false.
    * @return          False if setting an unfocused group to focused causes the PI limit to be exceeded.
    */
   bool SetFocus(WsfSA_Group* aGroup, bool aFocus, bool aProtect = false);

   /** Sets an entity as important, meaning it should be favored over other entities.
    * A version of this for groups would be available as well, which for focused groups,
    * would set each entity within the group as important, and for unfocused groups, will flag the group itself as important.
    * @param aItem         The PI for which importance is set.
    * @param aIsImportant  True to set as important, false otherwise.
    */
   void SetImportant(WsfSA_PerceivedItem* aItem, bool aIsImportant);

   /** Gets whenther or not a group is focused.
    * @param aGroup  The group to query.
    * @return        The group's focus state.
    */
   bool GetFocus(WsfSA_Group* aGroup);

   /** Gets the importance of a PI as a boolean.
    * @param aItem  The PI to query.
    * @return       True if the PI is set to WsfSA_PerceivedItem::IMPORTANT, false otherwize.
    */
   bool GetImportant(WsfSA_PerceivedItem* aItem);

protected:
   WsfSA_Perceive(const WsfSA_Perceive& aSrc);

   /** Return the nearest bogie/bandit entity perception (WsfSA_EntityPerception) in the list.
    * If no such entity exists, return nullptr.
    */
   WsfSA_EntityPerception* NearestEntity(const std::vector<WsfSA_EntityPerception*>& aDataList) const;

   /** Return true if the threat given is important, false otherwise
    * @param aThreatPtr threat to evaluate
    */
   bool Important(WsfLocalTrack* aThreatPtr);

   /** Return true if the asset given is important, false otherwise
    * @param aAssetPtr asset to evaluate
    */
   bool Important(WsfPlatform* aAssetPtr);

   /** Return true if the platform given is important, false otherwise
    * @param aPlatformIndex index of platform to evaluate
    */
   bool Important(size_t aPlatformIndex);

   /** Return true if the threat given is unimportant, false otherwise
    * @param aThreatPtr threat to evaluate
    */
   bool Unimportant(WsfLocalTrack* aThreatPtr);

   /** Return true if the asset given is unimportant, false otherwise
    * @param aAssetPtr asset to evaluate
    */
   bool Unimportant(WsfPlatform* aAssetPtr);

   /** Return true if the platform given is unimportant, false otherwise
    * @param aPlatformIndex index of platform to evaluate
    */
   bool Unimportant(size_t aPlatformIndex);

   /** Handle a track dropping.
    * @param aSimTime sim time at which track dropped
    * @param aLocalTrackPtr pointer to the dropped track
    */
   void LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);

   /** Attempt to update status at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to send status
    */
   double TryUpdateStatus(double aSimTime);

   /** Attempt to update assets at a sim time, and return an appropriate time for the next event update.
    * A cognitive delay is factored in.
    * @param aSimTime sim time at which to attempt updating
    */
   double TryUpdateAssets(double aSimTime);

   /** Attempt to update the data for perceived items, which include bogies and bandits as well as groups.
    * A cognitive delay is factored in.
    * @param aSimTime sim time at which to attempt updating
    */
   double TryUpdatePerceivedItemData(double aSimTime);

   /** Attempt to update the perceived items calculations, which include bogies and bandits as
    * well as groups, at a sim time, and return an appropriate time for the next event update.
    * A cognitive delay is factored in.
    * @param aSimTime sim time at which to attempt updating
    */
   double TryUpdatePerceivedItemCalculation(double aSimTime);

   /** If this platform has external links, send a status message to those links
    * @param aSimTime sim time at which to send status
    */
   double UpdateStatus(double aSimTime);

   /** Update assets
    * @param aSimTime sim time at which to update
    */
   void UpdateAssets(double aSimTime);

   /** Update assets
    * @param aSimTime sim time at which to update
    */
   void UpdateAssetsData(double aSimTime);

   /** Update group data at a sim time, and return an appropriate time for the next event update.
    * A cognitive delay is factored in.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdateGroupsData(double aSimTime);

   /** Update the perceived items calculations, which include bogies and bandits as well as groups.
    * @param aSimTime sim time at which to update
    */
   void UpdatePerceivedItemsCalculation(double aSimTime);

   /** Remove any invalid (perceived index not valid) in the bogies and bandits lists */
   void RemoveAnyInvalidEntitiesInBogiesAndBandits();

   /** Unfocus or remove groups in order to stay under the PI limit. If PIs are unlimited, then immediately return.
    * @param aSimTime  Sim time at which to prune.
    */
   void Prune(double aSimTime);

   /** This will attempt to find expendable countermeasures for chaff, flare, and decoys */
   void FindExpenableCountermeasures();

   /** This will attempt to find weapons, excluding countermeasure "weapons" */
   void FindWeapons();

   /** This creates the mPerceivedAssets list, using the specified temporary asset list and flag.
    * @param aSimTime time at which to calc score
    * @param aTempAssetsList list of temporary assets to evaluate
    * @param aUnlimitedAssets true if there is no limit to the number of assets in mPerceivedAssets
    *
    */
   void CreateAssetsList(double aSimTime, std::list<WsfSA_EntityPerception*>& aTempAssetsList, bool aUnlimitedAssets);

   /** This creates the mPerceivedBandits list, using the specified temporary tracked entities list and flag.
    * Note: CreateBanditsList should be called before calling CreateBogiesList.
    * @param aSimTime time when created
    * @param aTrackedEntitiesList list of tracked entities to evaluate
    * @param aUnlimitedBandits true if there is no limit to the number of assets in mPerceivedThreats
    *
    */
   void CreateBanditsList(double aSimTime, std::list<WsfSA_EntityPerception*>& aTrackedEntitiesList, bool aUnlimitedBandits);

   /** This creates the mPerceivedBogies list, using the specified temporary tracked entities list and flag.
    * Note: CreateBogiesList should be called after calling CreateThreatsList.
    * @param aSimTime time when created
    * @param aTrackedEntitiesList list of tracked entities to evaluate
    * @param aUnlimitedBogies true if there is no limit to the number of assets in mPerceivedBogies
    *
    */
   void CreateBogiesList(double aSimTime, std::list<WsfSA_EntityPerception*>& aTrackedEntitiesList, bool aUnlimitedBogies);

   /** Score a threat. Return the score. Prefer a user-defined scoring method, but failing that, use the default.
    * @param aSimTime time when calculated
    * @param aThreatPtr threat to score
    * @param aIsMissile indicates if track is perceived as a missile
    */
   double GetThreatScore(double aSimTime, WsfSA_EntityPerception* aThreatPtr, bool aIsMissile);

   /** This creates sublists for aircraft and missiles for assets */
   void CreateAssetSublists();

   /** This creates sublists for aircraft and missiles for bogies and bandits */
   void CreateSublists();

   /** This function removes any Wsf_EntityPerceptions from the given list that have outlived the given coast time.
    *
    * @param aPerceptionList A vector containing entity perceptions.
    * @param aCoastTime The time each perception is allowed to persist without any update (in sec).
    * @param aSimTime The current simulation time.
    */
   void RemoveExpiredCoastingPerceptions(std::vector<WsfSA_EntityPerception*>& aPerceptionList,
                                         double                                aCoastTime,
                                         double                                aSimTime);

   void SetOldPerceptionsAsCoasting(const std::vector<WsfSA_EntityPerception*>& aPerceptionList, double aSimTime);

   /**
    * This function sets any currently perceived entities to coasting if they aren't present in the new data.
    * Additionally, any perceived entities that are already coasting will have their positions manually updated
    * based on their last known velocity and heading.
    *
    * @param aPerceptionList A vector containing entity perceptions to potentially coast.
    * @param aNewPerceptionList A list containing new entity perceptions to update aPerceptionList with.
    * @param aSimTime The current simulation time.
    */
   void UpdateCoastingPerceptions(const std::vector<WsfSA_EntityPerception*>& aPerceptionList,
                                  const std::list<WsfSA_EntityPerception*>&   aNewPerceptionList,
                                  double                                      aSimTime);

   /** Either adds an entity perception to the given vector or updates a matching perception in the list.
    *
    * @param aPerceptionList A vector containing entity perceptions.
    * @param aPerception The entity perception to add (or the perception containing data to update an existing
    * perception with).
    * @param aSimTime The current simulation time.
    */
   void AddOrUpdatePerception(std::vector<WsfSA_EntityPerception*>& aPerceptionList,
                              WsfSA_EntityPerception&               aPerception,
                              double                                aSimTime);

   /** All entities in aPerceptionList that do not have a parent group will
    * either join an existing one, or form a degenerate group.
    *
    * @param aPerceptionList  A vector containing entity perceptions.
    * @param aGroupList       A list of possible groups that perceptions can join.
    * @param aSimTime         The current simulation time.
    */
   void JoinInitialGroups(std::vector<WsfSA_EntityPerception*>& aPerceptionList,
                          std::list<WsfSA_Group*>&              aGroupList,
                          double                                aSimTime);

   /** Helper function for Prune() which drops an entity from current perception.
    * In addition to deleting the entity, it removes it from all relevant entity lists.
    * @param aOldEntity  The entity to be dropped.
    */
   void DropEntity(WsfSA_EntityPerception* aOldEntity);

   /** Prepare an entity for later deletion. This is used to allow a pointer to remain
    * valid after an entity is no longer needed until all other references to it are gone.
    * @param aOldEntity  The entity to be deleted.
    */
   void DeleteLater(WsfSA_EntityPerception* aOldEntity);

   /** Deletes all entities that have been passed to DeleteLater(). */
   void DeferredDelete();

private:
   std::list<WsfSA_EntityPerception*> mDeferredForDeletion;

   double mConsiderationRandomness{0.0};

   std::default_random_engine mRandom{};
   /**
    * @brief Container that contains track delay data
    */
   struct TrackDelayData
   {
      double mTimeAdded;
      bool   mIsVisual;
      bool   mIsDone;
   };
   /**
    * @brief Track ID's and SimTime that the track was added into mTrackDelayQueue to wait the track perception delay.
    * Note: This will be empty if GetSAP()->GetTrackPerceptionDelay() <= 0.
    */
   std::unordered_map<WsfTrackId, TrackDelayData, WsfTrackId> mTrackDelayQueue;

   // Groups data
   // ......................................................................................................................

   WsfSA_GroupManager mGroupManager; /// Group manager

   double mMaxGroupingDistanceCentroid{8.0 * UtMath::cM_PER_NM};      /// Meters  - Default is 8 nm
   double mMaxGroupingDistanceNeighbor{4.0 * UtMath::cM_PER_NM};      /// Meters  - Default is 4 nm
   double mMaxGroupingHeadingDifference{10.0 * UtMath::cRAD_PER_DEG}; /// Radians
   double mMaxGroupingSpeedDifference{100.0};                         /// Knots
   double mMinGroupRadius{1.0 * UtMath::cM_PER_NM};                   /// Meters  - Default minimum radius is 1 nm
   bool   mUseCentroidGrouping{true};                                 /// Utilize centroid distance grouping constraints
   bool   mUseNeighborGrouping{true};                                 /// Utilize neighbor distance grouping constraints
   bool   mUseHeadingGrouping{true};                                  /// Utilize heading grouping constraints*/
   bool   mUseSpeedGrouping{true};                                    /// Utilize speed grouping constraints*/
   bool   mUseTypeGrouping{false};                                    /// Utilize type grouping constraints*/
   bool   mIgnoreBogiesWhenGrouping{false};                           /// Used to ignore bogies when forming groups
};

#endif
