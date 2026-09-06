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

#ifndef WSFSA_PROCESSOR_HPP
#define WSFSA_PROCESSOR_HPP

#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>

class UtInput;
class WsfAirMover;
class WsfBrawlerFuel;
class WsfBrawlerMover;
class WsfExplicitWeapon;
class WsfFuel;
class WsfLocalTrack;
class WsfP6DOF_Mover;

#include "WsfTrackList.hpp"

namespace wsf
{
namespace six_dof
{
class Mover;
}
} // namespace wsf

class WsfPlatform;
class WsfSA_Assess;
class WsfSA_EntityMessage;
class WsfSA_Perceive;
class WsfSA_Predict;
class WsfTrackManager;
class WsfTrackProcessor;

#include "wsf_air_combat_export.h"

#include "UtCallbackHolder.hpp"
#include "WsfEvent.hpp"
#include "WsfSA_EntityPerception.hpp"
#include "script/WsfScriptProcessor.hpp"
#include "script/WsfScriptProcessorClass.hpp"

// The WsfSA_Processor provides a collection of capabilities for situation awareness,
// including perception, assessment, and prediction modules.

class WSF_AIR_COMBAT_EXPORT WsfSA_Processor : public WsfScriptProcessor
{
public:
   enum EntityPerceptionType
   {
      TRUTH           = 0,
      STATUS_MESSAGES = 1
   };

   struct SA_EngagedTarget
   {
      bool   isValid          = false;
      size_t platformIndex    = 0;
      float  rangeMax         = 0.0;
      float  rangeNe          = 0.0;
      float  rangeMin         = 0.0;
      float  rangeCurrent     = 0.0;
      float  rangeClosureRate = 0.0;
      float  timeToIntercept  = 0.0;
      float  timeToActive     = 0.0;
      float  targetKTAS       = 0.0;
      float  targetAspect     = 0.0;
      float  targetAz         = 0.0;
      float  targetEl         = 0.0;
   };

   struct PerceiveData
   {
      // Assets
      std::vector<WsfSA_EntityPerception*> mPerceivedAssets;         ///< List of currently perceived assets
      std::vector<WsfSA_EntityPerception*> mPerceivedAircraftAssets; ///< List of currently perceived aircraft assets
      std::vector<WsfSA_EntityPerception*> mPerceivedMissileAssets;  ///< List of currently perceived missile assets

      EntityPerceptionType  mAssetPerception{TRUTH}; ///< Type of asset perception being used (TRUTH or STATUS_MESSAGES)
      std::map<size_t, int> mImportantAssets;        ///< Should be included in perception (asset's platform index = 1)
      std::map<WsfStringId, int> mImportantAssetTypes; ///< Map of important asset types
      std::map<size_t, int> mUnimportantAssets; ///< Should NOT be included in perception (asset's platform index = 1)
      std::map<WsfStringId, int> mUnimportantAssetTypes;     ///< Map of unimportant asset types
      bool                       mPerceiveSelfStatus{false}; ///< True if perceiving self, false otherwise
      std::map<size_t, WsfSA_EntityPerception*> mReceivedAssetStatus; ///< Map of asset status messages (platform index = message)
      bool         mReportingOthersStatus{false}; ///< True if reporting asset knowledge, false otherwise
      bool         mReportingSelfStatus{true};    ///< True if reporting self status, false otherwise
      unsigned int mAssetTruthPerceptionMask{
         0}; ///< Asset truth perception mask (WsfComm:: cCOMMANDER | cPEERS | cSUBORDINATES)
      WsfCategoryList mIgnoredAssetCategories; ///< Asset categories which we should ignore creating perceptions for

      // Bogies and Bandits
      std::vector<WsfSA_EntityPerception*> mPerceivedBogies;          ///< List of currently perceived bogies
      std::vector<WsfSA_EntityPerception*> mPerceivedAircraftBogies;  ///< List of currently perceived aircraft bogies
      std::vector<WsfSA_EntityPerception*> mPerceivedMissileBogies;   ///< List of currently perceived missile bogies
      std::vector<WsfSA_EntityPerception*> mPerceivedBandits;         ///< List of currently perceived bandits
      std::vector<WsfSA_EntityPerception*> mPerceivedAircraftBandits; ///< List of currently perceived aircraft bandits
      std::vector<WsfSA_EntityPerception*> mPerceivedMissileBandits;  ///< List of currently perceived missile bandits

      std::map<size_t, int> mImportantThreats; ///< Should be included in perception (track's target platform index = 1)
      std::map<WsfStringId, int> mImportantThreatTypes; ///< Map of important threat types
      std::map<size_t, int> mUnimportantThreats; ///< Should NOT be included in perception (track's target platform index = 1)
      std::map<WsfStringId, int> mUnimportantThreatTypes;       ///< Map of unimportant threat types
      bool                       mFilterAssetsFromBogies{true}; ///< True if assets should be filtered from bogie list

      std::list<WsfSA_Group*> mCurrentlyPerceivedGroups; ///< Perceived groups (these persist)

      // Custom Script Support
      UtScript* mAssetConsiderationScoringScriptPtr{nullptr}; ///< Pointer to the script being used to score assets
      UtScript* mBogieBanditConsiderationScoringScriptPtr{
         nullptr}; ///< Pointer to the script being used to score bogies and bandits
      UtScript* mMissileConsiderationScoringScriptPtr{nullptr}; ///< Pointer to the script being used to score missiles
      UtScript* mUnfocusedGroupConsiderationScoringScriptPtr{
         nullptr};                          ///< Pointer to the script being used to score unfocused groups
      UtScript* mPruningScriptPtr{nullptr}; ///< Pointer to the script being used to prune excess PIs.

      UtScriptClass* mSA_EntityPerceptionClassPtr{nullptr}; ///< Script class for WsfSA_EntityPerception
      UtScriptClass* mSA_GroupClassPtr{nullptr};            ///< Script class for WsfSA_Group
      UtScriptClass* mSA_PerceivedItemClassPtr{nullptr};    ///< Script class for WsfSA_PerceivedItem
      UtScriptClass* mTrackClassPtr{nullptr};               ///< Script class for WsfTrack

      // Cognitive Parameters
      std::vector<double> mExtraCognitiveDelaysAssets;  ///< List of delays to add to the next update time when updating
                                                        ///< asset perceptions
      std::vector<double> mExtraCognitiveDelaysThreats; ///< List of delays to add to the next update time when updating
                                                        ///< threat perceptions
      int    mMaxAssetLoad{-1};         ///< Max number of assets that can be perceived (negative value means no limit)
      int    mMaxAssetLoadInitial{-1};  ///< Max number of assets that can be perceived as initialized by user
      int    mMaxThreatLoad{-1}; ///< Max number of threats that can be perceived (negative value means no limit)
      int    mMaxThreatLoadInitial{-1}; ///< Max number of threats that can be perceived as initialized by user
      double mAssetCoastTime{0.0}; ///< The amount of time (in seconds) that an asset is allowed to coast before removal
      double mBanditCoastTime{0.0}; ///< The amount of time (in seconds) that a bandit is allowed to coast before removal
      double mBogeyCoastTime{0.0};  ///< The amount of time (in seconds) that a bogey is allowed to coast before removal

      // Weapons Data
      WsfSA_Processor::SA_EngagedTarget mEngagedTarget{};         ///< Current engaged target
      WsfExplicitWeapon*                mSelectedWeapon{nullptr}; ///< Pointer to the current selected weapon
      std::vector<WsfExplicitWeapon*>   mWeaponList;              ///< List of weapons
      bool mShootCueActive{false};   ///< Flag indicating if shoot cue is active (true=active, false=off)
      bool mShootCueBlinking{false}; ///< Flag indicating if shoot cue is blinking (true=blinking, false=not blinking)
      bool mWeaponBayDoorsAreOpen{false}; ///< Flag indicating if weapon bay doors are open (true=open, false=closed)
      bool mMasterArm{false};             ///< Flag indicating master arm state (true=armed, false=safe)

      // Warnings and Cautions
      bool mMasterWarningActive{false}; ///< Flag indicating master warning state (true=active, false=off)
      bool mMasterCautionActive{false}; ///< Flag indicating master caution state (true=active, false=off)
      bool mJammingDetected{
         false}; ///< Flag indicating if jamming is detected (true=jamming present, false=no jamming detected)

      // Expendable Countermeasures (CM)
      WsfExplicitWeapon* mChaffDispenser{nullptr}; ///< Pointer to the current chaff dispenser
      bool               mUseSimpleCM{false};      ///< Flag indicating if simple CM is used
      size_t             mNumSimpleChaff{0};       ///< Quantity of simple chaff remaining
      size_t             mNumSimpleFlare{0};       ///< Quantity of simple flares remaining
      size_t             mNumSimpleDecoy{0};       ///< Quantity of simple decoys remaining
   };

   // This enumeration identifies the various SA processor updates and is used as an argument
   // in methods and as an index to the mUpdateMgmtData vector.  Note that cUPDATE_TYPE_COUNT
   // is the number of UpdateType entries, but is outside the enumeration so that each UpdateType
   // is a valid vector index.
   enum UpdateType : size_t
   {
      cSTATUS                       = 0,
      cENGAGEMENT_DATA              = 1,
      cFLIGHT_DATA                  = 2,
      cFUEL_DATA                    = 3,
      cNAV_DATA                     = 4,
      cCONTROLS_DATA                = 5,
      cWEAPONS_DATA                 = 6,
      cTRACK_DATA                   = 7,
      cASSETS_DATA                  = 8,
      cPERCEIVED_ITEM_DATA          = 9,
      cPRIORITIZED_ITEM_DATA        = 10,
      cPERCEIVED_ITEM_CALCULATION   = 11,
      cPRIORITIZED_ITEM_CALCULATION = 12,
      cBEHAVIOR_CALCULATION         = 13
   };
   static const size_t cUPDATE_TYPE_COUNT{14}; // number of entries in WsfSA_Processor::UpdateType enum

   /** Get the UpdateTypes that are associated with the cognitive_update_interval group */
   static const std::vector<UpdateType> GetCognitiveUpdateGroup()
   {
      return std::vector<UpdateType>{cASSETS_DATA, cENGAGEMENT_DATA, cPERCEIVED_ITEM_DATA, cPRIORITIZED_ITEM_DATA};
   }

   /** Get the UpdateTypes that are associated with the platform_update_interval group */
   static const std::vector<UpdateType> GetPlatformUpdateGroup()
   {
      return std::vector<UpdateType>{cSTATUS, cFLIGHT_DATA, cFUEL_DATA, cNAV_DATA, cCONTROLS_DATA, cWEAPONS_DATA, cTRACK_DATA};
   }

   /** Get the UpdateTypes that are associated with the universal_update_interval group */
   static const std::vector<UpdateType> GetUniversalUpdateGroup()
   {
      return std::vector<UpdateType>{cSTATUS,
                                     cENGAGEMENT_DATA,
                                     cFLIGHT_DATA,
                                     cFUEL_DATA,
                                     cNAV_DATA,
                                     cCONTROLS_DATA,
                                     cWEAPONS_DATA,
                                     cTRACK_DATA,
                                     cASSETS_DATA,
                                     cPERCEIVED_ITEM_DATA,
                                     cPRIORITIZED_ITEM_DATA};
   }

   WsfSA_Processor(const WsfScenario& aScenario);
   WsfSA_Processor& operator=(const WsfSA_Processor&) = delete;
   ~WsfSA_Processor() override;
   WsfSA_Processor* Clone() const override { return new WsfSA_Processor(*this); }

   /** Create a new SA Processor for a platform.
    * @param aSimTime sim time to create the SAP at.
    * @param aPlatform platform to create the SAP for.
    */
   static WsfSA_Processor* CreateSA_Processor(double aSimTime, WsfPlatform& aPlatform);

   /** Get the pointer to the first attached SA processor.
    * @param aPlatform platform to get the processor for.
    */
   static WsfSA_Processor* GetSA_Processor(const WsfPlatform& aPlatform);

   /** Process script input.
    * @param aInput input to parse and handle.
    */
   bool ProcessInput(UtInput& aInput) override;

   /** Initialize this processor.
    * @param aSimTime sim time when the processor is initialized.
    */
   bool Initialize(double aSimTime) override;

   /** Initialize2 this processor.
    * @param aSimTime sim time when the processor is initialized (second pass).
    */
   bool Initialize2(double aSimTime) override;

   /** Update this processor.
    * @param aSimTime sim time of the update.
    */
   void Update(double aSimTime) override;

   /** This method is called by Update() at the base update rate to implement the
    * twelve SA Processor update types.
    * @param aSimTime Simulation time at update.
    */
   void HandleSA_ProcessorUpdates(double aSimTime);

   /** Return a pointer to the WsfSA_Perceive module. */
   WsfSA_Perceive* Perceive() const { return mPerceivePtr.get(); }

   /** Return a pointer to the WsfSA_Assess module. */
   WsfSA_Assess* Assess() const { return mAssessPtr.get(); }

   /** Return a pointer to the WsfSA_Predict module. */
   WsfSA_Predict* Predict() const { return mPredictPtr.get(); }

   /** Return a const reference to the perceive data */
   const WsfSA_Processor::PerceiveData* GetData() const { return &mData; }

   /** Get the P6DOF mover for the platform. This will be null if no
    * WsfP6DOF_Mover is on the platform.
    */
   WsfP6DOF_Mover* GetP6DofMover() { return mP6DofMover; }

   /** Get the SixDOF mover for the platform. This will be null if the platform lacks a WsfSixDOF_Mover. */
   wsf::six_dof::Mover* GetSixDOF_Mover() { return mSixDOF_Mover; }

   /** Get the Brawler mover for the platform. This will be null if the platform lacks a WsfBrawlerMover. */
   WsfBrawlerMover* GetBrawlerMover() { return mBrawlerMover; }

   /** Get the BrawlerFuel for the platform. This will be null if the platform lacks a WsfBrawlerFuel object. */
   WsfBrawlerFuel* GetBrawlerFuel() { return mBrawlerFuel; }

   /** Get the WsfAirMover for the platform. This will be null if the platform lacks a WsfAirMover. */
   WsfAirMover* GetAirMover() { return mAirMover; }

   /** Get the WsfFuel for the platform. This will be null if the platform lacks a WsfFuel object. */
   WsfFuel* GetAirMoverFuel() { return mAirMoverFuel; }

   /** Get the update interval (sec) for the requested data type
    * @param aUpdateType  one of WsfSA_Processor::UpdateType enumeration
    */
   double GetUpdateInterval(UpdateType aUpdateType) const { return mUpdateMgmtData[aUpdateType].activeInterval; }

   /** Get the startup interval (sec) parsed at startup for the requested data type
    * @param aUpdateType one of WsfSA_Processor::Updatetype enuermation
    */
   double GetStartupInterval(UpdateType aUpdateType) const { return mUpdateMgmtData[aUpdateType].startupInterval; }

   /** Set the update interval (sec) for the requested data type
    * @param aUpdateType      one of WsfSA_Processor::UpdateType enumeration
    * @param aUpdateInterval  new update interval
    */
   bool SetUpdateInterval(UpdateType aUpdateType, double aUpdateInterval);

   /** Reset the update interval (sec) for the requested data type to the startup interval
    * @param aUpdateType  one of WsfSA_Processor::UpdateType enumeration
    */
   void ResetUpdateInterval(UpdateType aUpdateType);

   /** Get the last update time (sec) for the requested data type
    * @param aUpdateType  one of WsfSA_Processor::UpdateType enumeration
    */
   double GetLastUpdateTime(UpdateType aUpdateType) const { return mUpdateMgmtData[aUpdateType].lastUpdateTime; }

   /** Set the last update time (sec) for the requested data type
    * @param aUpdateType  one of WsfSA_Processor::UpdateType enumeration
    * @param aSimTime     time at update.
    */
   bool SetLastUpdateTime(UpdateType aUpdateType, double aSimTime);

   /** Set the next update time (sec) for the requested data type
    * @param aUpdateType   one of the WsfSA_Processor::UpdateType enumeration
    */
   double GetNextUpdateTime(UpdateType aUpdateType) { return mUpdateMgmtData[aUpdateType].nextUpdateTime; }

   /** Set the next update time (sec) for the requested data type
    * @param aUpdateType  one of the WsfSA_Processor::UpdateType enumeration
    * @param aSimTime     time at update
    */
   bool SetNextUpdateTime(UpdateType aUpdateType, double aSimTime);

   /** Returns the name string associated with the specified UpdateType
    * @param aUpdateType  one of the WsfSA_Processor::UpdateType enumeration
    */
   std::string GetUpdateTypeString(const UpdateType aUpdateType);

   /** Get the purge lifetime limit (sec) for asset data */
   double GetAssetDataPurgeLifetime() const { return mAssetDataPurgeLifetime; }

   /** Set the purge lifetime limit (sec) for asset data
    * @param aPurgeLifetime  new purge lifetime setting
    */
   bool SetAssetDataPurgeLifetime(double aPurgeLifetime);

   /** Reset the purge lifetime limit (sec) for asset data */
   void ResetAssetDataPurgeLifetime() { mAssetDataPurgeLifetime = mStartupAssetDataPurgeLifetime; }

   /** This will return a double that is a multiple of the parameter provided
    * @param aNumToRound  double to be round up
    * @param aMultiple    double to be used as the multiple
    */
   static double RoundToBaseRateMultiple(double aNumToRound, double aMultiple);

   /** Returns the current value for visual_perception_delay (sec) */
   double GetVisualPerceptionDelay() { return mVisualPerceptionDelay; }

   /** Returns the current value for display_perception_delay (sec) */
   double GetDisplayPerceptionDelay() { return mDisplayPerceptionDelay; }

   /** Set the visual perception delay (sec)
    * @param aDelayTime  delay before a new visual track becomes perceived entity for processing/classification
    */
   bool SetVisualPerceptionDelay(double aDelayTime);

   /** Set the display perception delay (sec)
    * @param aDelayTime   delay before a new display track becomes perceived entity for processing/clasfficiation
    */
   bool SetDisplayPerceptionDelay(double aDelayTime);
   /** Returns the a pointer to the track processor of the provided name. A nullptr will be returned if
    * a processor cannot be found with the provided name.
    * @param aProcessorName  name of the processor to be sent
    */
   WsfTrackProcessor* GetTrackProcessor(const std::string& aProcessorName);

   /** Returns mTrackProcessorList that contains the pointers to the different track processors. */
   std::vector<WsfTrackProcessor*> GetTrackProcessors() { return mTrackProcessorList; }

   /** Returns true if the specified entity is in the list of assets at the present time.
    * @param aEntity entity to test.
    */
   bool EntityIsInAssetGroup(const WsfSA_EntityPerception* aEntity) const;

   /** Returns true if the specified entity index is in the list of assets at the present time.
    * @param aEntityIndex entity to test.
    */
   bool EntityIsInAssetGroup(size_t aEntityIndex) const;

   /** Get the perceived assets for a platform. This will be empty if no
    * SA Processor is on the platform.
    * @param aPlatform platform to get perceived assets for.
    * @param aSimTime sim time to get assets at.
    */
   static const std::vector<WsfSA_EntityPerception*>& GetPlatformPerceivedAssets(const WsfPlatform& aPlatform,
                                                                                 double             aSimTime);

   /** Get the perceived bogies for a platform. This will be empty if no
    * SA Processor is on the platform.
    * @param aPlatform platform to get perceived bogies for.
    * @param aSimTime sim time to get threats at.
    */
   static const std::vector<WsfSA_EntityPerception*>& GetPlatformPerceivedBogies(const WsfPlatform& aPlatform,
                                                                                 double             aSimTime);

   /** Get the perceived bandits for a platform. This will be empty if no
    * SA Processor is on the platform.
    * @param aPlatform platform to get perceived bandits for.
    * @param aSimTime sim time to get bandits at.
    */
   static const std::vector<WsfSA_EntityPerception*>& GetPlatformPerceivedBandits(const WsfPlatform& aPlatform,
                                                                                  double             aSimTime);

   /** Return the name of this script class. */
   const char* GetScriptClassName() const override { return "WsfSA_Processor"; }

   /** Create the 'class' object for the script system.
    * This is invoked once by WsfScriptManager to create the 'class' object that defines
    * the interface to instances of this class from the script system.
    * @param aClassName name for the new class.
    * @param aScriptTypesPtr pointer to the script types to store in the script class.
    */
   UtScriptClass* CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   /** Change the track manager.
    * @param aSimTime sim time when track manager changed.
    * @param aNewManagerPtr pointer to the new manager.
    */
   void TrackManagerChanged(double aSimTime, WsfTrackManager* aNewManagerPtr);

   /** Process a WsfMessage.
    * @param aSimTime sim time when processing.
    * @param aMessage message to handle.
    */
   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;

   /** Send a message to external links.
    * @param aSimTime sim time to send message.
    * @param aStatusMessage message to send.
    */
   void SendMessage(double aSimTime, const WsfSA_EntityMessage& aStatusMessage);

   /** Handle a track dropping.
    * @param aSimTime sim time when track dropped.
    * @param aLocalTrackPtr pointer to the dropped track.
    */
   void LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);

   /** True if the processor has external links (comms), false otherwise. */
   bool HasExternalLinks() const { return mExternalLinks.HasLinks(); }

   /** Check an input value for frequency units, and if needed, convert
    * it to seconds.
    * @param aValue value of the input.
    * @param aInput input to check units of.
    */
   static void CheckForFrequencyUnits(double& aValue, UtInput& aInput);

   /** These functions handle the addition or deletion of platforms in the simulation */

   /** This will (possibly) add a newly created platform to a local list of platforms, using various filters
    * @param aSimTime sim time when platform added
    * @param aPlatform pointer to the added platform
    */
   void PlatformAdded(double aSimTime, WsfPlatform* aPlatform) override;

   /** This will remove a newly deleted platform from a local list of platforms, if present
    * @param aPlatform pointer to the added platform
    */
   void PlatformDeleted(WsfPlatform* aPlatform) override;

   /** These functions provide data related to the selected weapon */

   /** Returns the name of the selected weapon. */
   std::string GetSelectedWeapon() const;

   /** Returns a pointer to the selected weapon. */
   const WsfExplicitWeapon* SelectedWeapon() const;

   /** Returns the available quantity of the selected weapon. */
   int QuantityOfSelectedWeapon() const;

   /** Returns true if the weapon selection was successful, else false.
    * @param aWeaponName name of the desired weapon.
    */
   bool SetSelectedWeapon(std::string aWeaponName);

   /** Returns the current list of weapons. */
   const std::vector<WsfExplicitWeapon*> GetWeaponList() const;

   /** Returns the initial number of weapons. */
   int GetInitialWeaponsQuantity() { return mInitialWeaponsQuantity; }

   /** Returns true if the weapon selection was successful, else false.
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

   /** Returns true if a weapon is flight is being supported, else false */
   bool SupportingWeapon() const;

   /** Returns true if the shoot cue is active, else false */
   bool ShootCueActive() const;

   /** Returns true if the shoot cue is blinking, else false */
   bool ShootCueBlinking() const;

   /** Returns true if radar jamming is detected by local radar sensors, else false */
   bool JammingDetected() const;

   /** These functions provide data related to the primary target being engaged */

   /** Returns true if there is an engaged target. */
   bool EngagedTargetValid() const;

   /** Returns an EngagedTarget struct for the primary engaged target. */
   SA_EngagedTarget GetEngagedTarget() const;

   /** Set the engaged target using the specified data */
   void SetEngagedTarget(SA_EngagedTarget aTarget);

   /** These functions provide data related to expendables */

   /** Returns the available quantity of the chaff cartridges. */
   int QuantityOfChaff() const;

   /** Returns the available quantity of the flare cartridges. */
   int QuantityOfFlares() const;

   /** Returns the available quantity of the expendable decoys. */
   int QuantityOfDecoys() const;

   /** Returns true if there was chaff available and dispenses a deployment of chaff */
   bool DispenseChaff(double aSimTime);

   /** Returns true if there was a flare available and dispenses a deployment of flare(s) */
   bool DispenseFlare(double aSimTime);

   /** Returns true if there was a decoy available and dispenses a single decoy */
   bool DispenseDecoy(double aSimTime);

   /** Other miscellaneous functions */

   /** This function will return true if an SA Processor exists on the specified platform */
   static bool PlatformContainsSA_Processor(WsfPlatform* aPlatform);

   /** This function will return a pointer to the first SA Processor on the specified platform */
   static const WsfSA_Processor* GetPlatformSA_Processor(const WsfPlatform* aPlatform);

   /** This function returns a list of filtered platform ids for platforms that will be evaluated  */
   const std::vector<size_t> GetFilteredPlatformIdList() const { return mFilteredPlatformIdList; }

   /** This function returns true if the specified side is in the enemy sides list  */
   bool IsEnemySide(const std::string& aSide) const;

   /** This function returns true if the specified side is in the friendly sides list  */
   bool IsFriendlySide(const std::string& aSide) const;

   /** This function returns true if the specified side is in the neutral sides list  */
   bool IsNeutralSide(const std::string& aSide) const;

   /** This function returns true if the specified type is in the enemy types list  */
   bool IsEnemyType(const std::string& aType) const;

   /** This function returns true if the specified type is in the friendly types list  */
   bool IsFriendlyType(const std::string& aType) const;

   /** This function returns true if the specified type is in the neutral types list  */
   bool IsNeutralType(const std::string& aType) const;

   /** This function returns true if the specified type is in the missile types list  */
   bool IsMissileType(const std::string& aType) const;

   /** Get the pointer to the Missile Warning System (MWS) track processor. If no such
    * track processor exists, return null. */
   const WsfTrackProcessor* GetMWS_Processor() const;

   /** This function will return true if the specified (truth) platform is in the track list
    * of the Missile Warning System (MWS) track processor. Otherwise, it will return false.
    * If there is no MWS track processor, it will return false. */
   bool PlatformInTrackListMWS(const WsfPlatform* aPlatform) const;

   /** This function returns true if IFF should be used for identification  */
   bool UseIFF() const { return mUseIFF; }

   /** This function returns true if the simple identification-by-type approach should be used  */
   bool UseSimpleIdByType() const { return mUseSimpleIdByType; }

   /** This function returns the max range (meters) for perceived assets */
   double MaxRangeForPerceivedAssets() const { return mMaxRangeForPerceivedAssets; }

   /** This function returns the max range (meters) for perceived bogies and bandits */
   double MaxRangeForPerceivedBogiesAndBandits() const { return mMaxRangeForPerceivedBogiesAndBandits; }

   /** This function returns true if the max range filter should be applied for assets */
   bool UseMaxRangeFilterForPerceivedAssets() const { return mUseMaxRangeFilterForPerceivedAssets; }

   /** This function returns true if the max range filter should be applied for bogies and bandits */
   bool UseMaxRangeFilterForPerceivedBogiesAndBandits() const { return mUseMaxRangeFilterForPerceivedBogiesAndBandits; }

   /** This function returns the max range (meters) for engagement data */
   double MaxRangeForEngagementData() const { return mMaxRangeForEngagementData; }

   /** This function returns true if the max range filter should be applied for engagements */
   bool UseMaxRangeForEngagementData() const { return mUseMaxRangeForEngagementData; }

   /** This function returns the assumed range of angle-only tracks */
   double AssumedRangeForAngleOnlyTargets() const { return mAssumedRangeForAngleOnlyTargets; }

   /** This function returns the flight id. Zero indicated not a member of any flight. */
   std::uint16_t FlightId() { return mFlightId; }

   /** This function returns the id flag string */
   std::string IdFlag() { return mIdFlag; }

   /** This function sets the flight id. Zero indicates not a member of any flight. */
   void SetFlightId(std::uint16_t aFlightId) { mFlightId = aFlightId; }

   /** This function sets the id flag string. */
   void SetIdFlag(const std::string& aIdFlag) { mIdFlag = aIdFlag; }

   /** This function returns the name of the optional ESM/RWR track processor.
    * If no such processor exists, it returns an empty string. */
   const std::string& ESM_TrackProcessor() const { return mESM_TrackProcessor; }

   /** This function returns the name of the optional missile warning system (MWS) track processor.
    * If no such processor exists, it returns an empty string. */
   const std::string& MWS_TrackProcessor() const { return mMWS_TrackProcessor; }

   /** This function returns the name of the optional radar track processor.
    * If no such processor exists, it returns an empty string. */
   const std::string& RadarTrackProcessor() const { return mRadarTrackProcessor; }

   /** This function returns the name of the optional IRST track processor.
    * If no such processor exists, it returns an empty string. */
   const std::string& IRST_TrackProcessor() const { return mIRST_TrackProcessor; }

   /** This function returns the name of the optional DAS track processor.
    * If no such processor exists, it returns an empty string. */
   const std::string& DAS_TrackProcessor() const { return mDAS_TrackProcessor; }

   /** This function returns the name of the optional FLIR track processor.
    * If no such processor exists, it returns an empty string. */
   const std::string& FLIR_TrackProcessor() const { return mFLIR_TrackProcessor; }

   /** This function returns the name of the optional eyes sensor track processor.
    * If no such processor exists, it returns an empty string. */
   const std::string& EyesTrackProcessor() const { return mEyesTrackProcessor; }

   /** This function returns the name of the optional perception master track processor.
    * If no such processor exists, it returns an empty string. */
   std::string PerceptionMasterTrackProcessor() const { return mPerceptionMasterTrackProcessorStr; }

   /** This function returns true if the optional perception master track processor is being used */
   bool UsingPerceptionMasterTrackProcessor() const { return mUsingPerceptionMasterTrackProcessor; }

   /** Get the pointer to the optional perception master track processor (PMTP).
    * If there is no PMTP, it returns null. */
   WsfTrackProcessor* GetPerceptionMasterTrackProcessor() const { return mPerceptionMasterTrackProcessorPtr; }

   /** This function returns the local track list for the optional perception master track processor (PMTP).
    * If there is no PMTP, it returns null. */
   WsfLocalTrackList* GetPerceptionMasterTrackProcessorTrackList() const;

   // Flight Kinematics Data
   struct SA_AircraftKinematics
   {
      double      lat_deg{0.0};
      double      lon_deg{0.0};
      float       kcas{0.0f};
      float       ktas{0.0f};
      float       mach{0.0f};
      float       altBaro_ft{0.0f};
      float       altRadar_ft{0.0f};
      float       vertSpd_fpm{0.0f};
      float       alpha_deg{0.0f};
      float       beta_deg{0.0f};
      float       gLoad{0.0f};
      float       gAvail{0.0f};
      float       heading_deg{0.0f};
      float       pitch_deg{0.0f};
      float       roll_deg{0.0f};
      float       gX{0.0f};
      float       gY{0.0f};
      std::string pilotType{""};
      bool        afterburnerOn{false};
      bool        contrailing{false};
      bool        stallWarning{false};
      bool        alphaValid{false};
      bool        betaValid{false};
      bool        kcasValid{false};
      bool        nxValid{false};
      bool        nyValid{false};
      bool        nzValid{false};
      bool        gAvailValid{false};
   };

   /** This function returns the aircraft kinematics data */
   const SA_AircraftKinematics& GetAircraftKinematics() const { return mAircraftKinematicsData; }

   /** This function sets the aircraft kinematics data */
   void SetAircraftKinematics(const SA_AircraftKinematics& aFlightData) { mAircraftKinematicsData = aFlightData; }

   // Fuel Data
   struct SA_FuelSystemData
   {
      float   fuelInternal_lbs{0.0f};
      float   fuelExternal_lbs{0.0f};
      float   fuelCapInt_lbs{0.0f};
      float   fuelCapExt_lbs{0.0f};
      float   grossWgt_lbs{0.0f};
      float   joker_lbs{0.0f};
      float   bingo_lbs{0.0f};
      float   fuelFlow_pph{0.0f};
      float   normalizedFuelQty{0.0f};
      uint8_t numExtTanks{0};
      bool    dropTanksPresent{false};
      bool    fuelValid{false};
   };

   /** This function returns the aircraft fuel data */
   const SA_FuelSystemData& GetFuelSystemData() const { return mFuelSystemData; }

   /** This function sets the aircraft fuel data */
   void SetFuelSystemData(const SA_FuelSystemData& aData) { mFuelSystemData = aData; }

   // Nav Data
   struct SA_NavigationLatLon
   {
      float lat_deg{0.0f};
      float lon_deg{0.0f};
   };

   struct SA_NavigationDataSummary
   {
      bool                             dataValid{false};
      uint8_t                          numWaypoints{0};
      uint8_t                          currentWaypoint{0};
      float                            headingBug_deg{0.0f};
      float                            distanceToWaypoint_nm{0.0f};
      float                            timeToWaypoint_sec{0.0f};
      std::vector<SA_NavigationLatLon> waypoints;
   };

   /** This function returns the aircraft nav data */
   const SA_NavigationDataSummary& GetNavigationDataSummary() const { return mNavigationDataSummary; }

   /** This function sets the aircraft nav data */
   void SetNavigationDataSummary(const SA_NavigationDataSummary& aData) { mNavigationDataSummary = aData; }

   // Flight Controls Data
   struct SA_FlightControlsDataSummary
   {
      float stickRgtInput{0.0f};
      float stickBackInput{0.0f};
      float rudderRgtInput{0.0f};
      float speedBrakeInput{0.0f};
      float throttleInput{0.0f};
      float aileronRgtNormalized{0.0f};
      float aileronLftNormalized{0.0f};
      float flapRgtNormalized{0.0f};
      float flapLftNormalized{0.0f};
      float stabilizerRgtNormalized{0.0f};
      float stabilizerLftNormalized{0.0f};
      float rudderRgtNormalized{0.0f};
      float rudderLftNormalized{0.0f};
      float rudderNormalized{0.0f};
      float speedBrakeNormalized{0.0f};
      float thrustNormalized{0.0f};
      float landingGear{0.0f};
      bool  throttleValid{false};
      bool  aileronsValid{false};
      bool  flapsValid{false};
      bool  twinRuddersValid{false};
      bool  singleRudderValid{false};
   };

   /** This function returns the aircraft flight controls data */
   const SA_FlightControlsDataSummary& GetFlightControlsDataSummary() const { return mFlightControlsDataSummary; }

   /** This function sets the aircraft flight controls data */
   void SetFlightControlsDataSummary(const SA_FlightControlsDataSummary& aData) { mFlightControlsDataSummary = aData; }

   struct SA_WeaponNameQtyPair
   {
      std::string weaponName{""};
      uint16_t    quantity{0};
   };

   struct SA_WeaponEngagedTarget
   {
      unsigned int targetIndex{0};
      float        rangeMax_nm{0.0f};
      float        rangeNe_nm{0.0f};
      float        rangeMin_nm{0.0f};
      float        rangeCurrent_nm{0.0f};
      float        rangeClosureRate_kts{0.0f};
      float        timeToIntercept_sec{0.0f};
      float        timeToActive_sec{0.0f};
      float        targetKTAS{0.0f};
      float        targetAspect_deg{0.0f};
      float        targetAz_deg{0.0f};
      float        targetEl_deg{0.0f};
   };

   struct SA_WeaponsDataSummary
   {
      unsigned int                        platformIndex{0};
      std::vector<SA_WeaponNameQtyPair>   weaponList{};
      std::string                         selectedWeapon{""};
      uint16_t                            numSelectedWeapon{0};
      uint16_t                            initialNumWeapons{0};
      std::vector<SA_WeaponEngagedTarget> engagedTargetList{};
      uint8_t                             numChaff{0};
      uint8_t                             numFlares{0};
      uint8_t                             numDecoys{0};

      bool weaponBayDoorsOpen{false};
      bool supportingWeapon{false};
      bool masterWarning{false};
      bool masterCaution{false};
      bool shootCueActive{false};
      bool shootCueBlink{false};
      bool masterArmActive{false};
      bool jammingDetected{false};
   };

   /** This function returns the weapons data */
   const SA_WeaponsDataSummary GetWeaponsDataSummary() const { return mWeaponsDataSummary; }

   /** This function sets the weapons data */
   void SetWeaponsDataSummary(const SA_WeaponsDataSummary& aData) { mWeaponsDataSummary = aData; }

   enum SA_Identification
   {
      ID_UNKNOWN  = 0,
      ID_FRIENDLY = 1,
      ID_NEUTRAL  = 2,
      ID_BOGIE    = 3,
      ID_BANDIT   = 4
   };

   // Track Data
   struct SA_TrackEntryData
   {
      unsigned int      targetPlatformIndex{0};
      double            lat_deg{0.0};
      double            lon_deg{0.0};
      float             altitude_ft{0.0f};
      float             bearing_deg{0.0f};
      float             elevation_deg{0.0f};
      float             range_nm{0.0f};
      float             rangeRate_kts{0.0f};
      float             targetSpeed_kts{0.0f};
      float             targetHdg_deg{0.0f};
      float             targetAspect_deg{0.0f};
      std::string       idString{""};
      std::string       tgtPlatformName{""};
      std::string       tgtTypeName{""};
      SA_Identification identification{ID_UNKNOWN};
      bool              latLonValid{false};
      bool              altitudeValid{false};
      bool              bearingValid{false};
      bool              elevationValid{false};
      bool              rangeValid{false};
      bool              rangeRateValid{false};
      bool              targetSpeedValid{false};
      bool              targetHdgValid{false};
      bool              targetAspectValid{false};
      bool              idIffValid{false};
      bool              idAuxValid{false};
      bool              idSideValid{false};
      bool              idTypeValid{false};
      bool              isMissile{false};
   };

   // Track manager Data
   struct SA_TrackManagerData
   {
      std::string                    name;
      bool                           isMasterProcessor = false;
      std::vector<SA_TrackEntryData> trackList;
   };

   /** This function returns the track data for all of the track processors/managers */
   const std::vector<SA_TrackManagerData>& GetTrackManagerList() const { return mTrackManagerDataList; }

   /** This function sets the track data */
   void SetTrackManagerList(const std::vector<SA_TrackManagerData>& aData) { mTrackManagerDataList = aData; }

   /** Attempt to update flight data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   double TryUpdateFlightKinematics(double aSimTime);

   /** Attempt to update fuel data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   double TryUpdateFuel(double aSimTime);

   /** Attempt to update nav data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   double TryUpdateNav(double aSimTime);

   /** Attempt to update flight controls data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   double TryUpdateFlightControls(double aSimTime);

   /** Attempt to update weapons data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   double TryUpdateWeapons(double aSimTime);

   /** Attempt to update track data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   double TryUpdateTracks(double aSimTime);

   /** Attempt to update advanced behavior trees and finite state machines, and return an appropriate time for the next
    * event update.
    * @param aSimTime sim time at which to attempt updating
    */
   double TryUpdateBehaviorCalculation(double aSimTime);

   /** Update flight data for the platform
    * @param aSimTime sim time at which to update
    */
   void UpdateFlightKinematics(double aSimTime);

   /** Update fuel data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdateFuel(double aSimTime);

   /** Update nav data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdateNav(double aSimTime);

   /** Update flight controls data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdateFlightControls(double aSimTime);

   /** Update weapons data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdateWeapons(double aSimTime);

   /** Update track data for the platform
    * @param aSimTime sim time at which to update
    */
   void UpdateTracks(double aSimTime);

   /** Update flight data for the platform
    * @param aSimTime sim time at which to update
    */
   void UpdateFlightKinematicsData(double aSimTime);

   /** Update fuel data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdateFuelData(double aSimTime);

   /** Update nav data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdateNavData(double aSimTime);

   /** Update flight controls data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdateFlightControlsData(double aSimTime);

   /** Update weapons data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdateWeaponsData(double aSimTime);

   /** Update track data for the platform
    * @param aSimTime sim time at which to update
    */
   void UpdateTrackData(double aSimTime);

   /** Update advanced behavior trees and finite state machines
    * @param aSimTime sim time at which to update
    */
   void UpdateBehaviorCalculation(double aSimTime);

   /** Returns true if afterburner is operating */
   bool AfterburnerOn() const { return mAircraftKinematicsData.afterburnerOn; }

   /** Returns true if producing contrails */
   bool IsContrailing() const { return mAircraftKinematicsData.contrailing; }

   /** Calculate the observables (afterburner and contrails) of the current platform */
   void CalculateObservables();

   /** Calculate the RF emissions for the current platform */
   void CalculateEmissions();

   // These functions support "RF emissions" related calculations

   /** This returns true if any radar is emitting. */
   bool RadarEmitting() const { return mRadarEmitting; }

   /** This returns true if any jammer is emitting. */
   bool JammerEmitting() const { return mJammerEmitting; }

   /** This returns true if any other systems are emitting. */
   bool OtherSystemsEmitting() const { return mOtherSystemsEmitting; }

   /** This returns true if the bingo fuel state has been reached */
   bool FuelStateBingoReached() const;

   /** This returns true if the joker fuel state has been reached */
   bool FuelStateJokerReached() const;

   /** This returns the normalized fuel state, in the range from 0 (empty) to 1 (full) */
   double FuelStateNormalized() const;

   /** This returns the time (sec) until reaching joker state at the current burn rate */
   double TimeToJoker() const;

   /** This returns the time (sec) until reaching bingo state at the current burn rate */
   double TimeToBingo() const;

   /** This returns the time (sec) until fuel tank(s) are empty at the current burn rate */
   double TimeToEmpty() const;

protected:
   WsfSA_Processor(const WsfSA_Processor& aSrc);

   /** This will call an AFSIM script function with the specified name.
    * If returnValueExpected is true, the function will be added to the
    * list of scripts awaiting a return value. Use false if no return value is needed.
    * @param aSimTime sim time to call script at.
    * @param aScriptName name of the script to call.
    */
   void CallScript(double aSimTime, const std::string& aScriptName);

   /** This will call an AFSIM script function with the specified name.
    * If returnValueExpected is true, the function will be added to the
    * list of scripts awaiting a return value. Use false if no return value is needed.
    * @param aSimTime sim time to call script at.
    * @param aScriptName name of the script to call.
    */
   double CallScriptReturnDouble(double aSimTime, const std::string& aScriptName);

   /** This will return true if an AFSIM script function with the specified name exists.
    * @param aScriptName name of the script to check for.
    */
   bool ScriptExists(const std::string& aScriptName);

   /** This outputs test data, useful for debugging */
   void DebugTestOutput(double aSimTime);

private:
   UtCallbackHolder mCallbacks; ///< Callback holder

   // The platform carrying this SA Processor
   WsfPlatform* mPlatform{nullptr}; ///< Platform for this processor

   // Possible mover (and fuel) objects
   bool                 mMoverPresent{false};   ///< True if platform contains a mover
   WsfP6DOF_Mover*      mP6DofMover{nullptr};   ///< Mover for this processor
   wsf::six_dof::Mover* mSixDOF_Mover{nullptr}; ///< Mover for this processor
   WsfBrawlerMover*     mBrawlerMover{nullptr}; ///< Mover for this processor
   WsfBrawlerFuel*      mBrawlerFuel{nullptr};  ///< Fuel for this processor
   WsfAirMover*         mAirMover{nullptr};     ///< Mover for this processor
   WsfFuel*             mAirMoverFuel{nullptr}; ///< Fuel for this processor

   // Master track processor (track manager) and track processors
   WsfTrackManager* mMasterTrackProcessor{nullptr}; ///< Pointer to the master track processor/manager for the current platform
   std::vector<WsfTrackProcessor*> mTrackProcessorList; ///< List of track processors for the current platform

   // Track Data - Includes data for each track processor on the platform
   std::vector<SA_TrackManagerData> mTrackManagerDataList; ///< List of track managers and their tracks
   bool mFilterAssetsFromTracks{true};                     ///< True if assets should be filtered from track lists

   // Weapons Data -- Includes the number/types of weapons, countermeasures, and warnings/flags
   SA_WeaponsDataSummary mWeaponsDataSummary;        ///< Summary of weapons data
   int                   mInitialWeaponsQuantity{0}; ///< The total number of explicit weapons at start

   // Modules - Perceive, Assess, and Predict
   std::unique_ptr<WsfSA_Perceive> mPerceivePtr; ///< Pointer to the Perceive module
   std::unique_ptr<WsfSA_Assess>   mAssessPtr;   ///< Pointer to the Assess module
   std::unique_ptr<WsfSA_Predict>  mPredictPtr;  ///< Pointer to the Predict module

   // Enemy/friendly sides/types
   std::vector<std::string> mEnemySidesList;    ///< A list of enemy side strings
   std::vector<std::string> mFriendlySidesList; ///< A list of friendly side strings
   std::vector<std::string> mNeutralSidesList;  ///< A list of neutral side strings
   std::vector<std::string> mEnemyTypesList;    ///< A list of enemy type strings
   std::vector<std::string> mFriendlyTypesList; ///< A list of friendly type strings
   std::vector<std::string> mNeutralTypesList;  ///< A list of neutral type strings
   std::vector<std::string> mMissileTypesList;  ///< A list of missile type strings

   // Identification
   bool mUseIFF{false};            ///< Allows the use of IFF in determining identification
   bool mUseSimpleIdByType{false}; ///< Allows the use of the simple means of type-based identification

   // Range settings
   double mMaxRangeForPerceivedAssets{0.0}; ///< Range limit (meters) to include an asset in the perceived asset list
   double mMaxRangeForPerceivedBogiesAndBandits{
      0.0}; ///< Range limit (meters) to include an bogie/bandit in the perceived lists
   bool mUseMaxRangeFilterForPerceivedAssets{false}; ///< Indicates if range limits should be utilized for assets
   bool mUseMaxRangeFilterForPerceivedBogiesAndBandits{
      false};                                    ///< Indicates if range limits should be utilized for bogies/bandits
   double mMaxRangeForEngagementData{0.0};       ///< Range limit (meters) to include an entity in the engagement data
   bool   mUseMaxRangeForEngagementData{false};  ///< Range limit (meters) to include an entity in the engagement data
   double mAssumedRangeForAngleOnlyTargets{0.0}; ///< Range assumed for angle-only tracks (meters)

   /** This is a list of all platform ids in the simulation, but applying filters to eliminate platforms that lack the
    * desired characteristics. */
   std::vector<size_t> mFilteredPlatformIdList;

   // Filters for monitored platforms, used for engagement assessment
   bool mFilterRequiresSameSide{false};            ///< Filter-out platforms that are not on the same side
   bool mFilterRequiresNotSameSide{false};         ///< Filter-out platforms that are on the same side
   bool mFilterRequiresAirDomain{false};           ///< Filter-out platforms that are not in the air domain
   bool mFilterRequiresNotAirDomain{false};        ///< Filter-out platforms that are in the air domain
   bool mFilterRequiresLandOrSurfaceDomain{false}; ///< Filter-out platforms that are not in the land or surface domains
   bool mFilterRequiresNotSubsurfaceDomain{false}; ///< Filter-out platforms that are in the land or surface domains
   bool mFilterRequiresNotSpaceDomain{false};      ///< Filter-out platforms that are in the space domain
   bool mFilterRequiresSA_Processor{false};        ///< Filter-out platforms that lack an SA Processor

   // Missile identification filters
   bool mMissileSpeedAnyAlt{false};     ///< Speed (any alt) greater than .......... missile_speed_any_alt
   bool mMissileAltAnySpeed{false};     ///< Altitude (any speed) greater than ..... missile_alt_any_speed
   bool mMissileSpeedWithAlt{false};    ///< Speed with alt greater than ........... missile_speed_with_alt
   bool mMissileAltWithSpeed{false};    ///< Altitude with speed greater than ...... missile_alt_with_speed
   bool mMissileNoseAngle{false};       ///< Angle off nose of target less than .... missile_nose_angle
   bool mMissileTimeToIntercept{false}; ///< Time to intercept less than ........... missile_time_to_intercept
   bool mMissileDistance{false};        ///< Distance less than .................... missile_distance

   double mMissileSpeedAnyAltValue{0.0};     ///< Speed (any alt) greater than (value) in meters/sec
   double mMissileAltAnySpeedValue{0.0};     ///< Altitude (any speed) greater than  (value) in meters
   double mMissileSpeedWithAltValue{0.0};    ///< Speed with alt greater than (value) in meters/sec
   double mMissileAltWithSpeedValue{0.0};    ///< Altitude with speed greater than (value) in meters
   double mMissileNoseAngleValue{0.0};       ///< Angle off nose of target less than (value) in radians
   double mMissileTimeToInterceptValue{0.0}; ///< Time to intercept less than  (value) in sec
   double mMissileDistanceValue{0.0};        ///< Distance less than (value) in meters

   // Flight ID, provides common means of identifying "flights" (typically two or four aircraft). Each flight should use
   // a unique number.
   std::uint16_t mFlightId{0}; ///< Optional flight id (used to denote aircraft in a particular flight )
   std::string   mIdFlag{""};  ///< Optional string that provides an id within a flight.

   // Flight, fuel, navigation, and controls data
   double mBingoFuel_lbs{0.0}; ///< Optional bingo fuel level -- if specified, is used instead of mover value
   double mJokerFuel_lbs{0.0}; ///< Optional joker fuel level -- if specified, is used instead of mover value
   SA_AircraftKinematics        mAircraftKinematicsData;    ///< Flight kinematics data for the current platform
   SA_FuelSystemData            mFuelSystemData;            ///< Fuel system data for the current platform
   SA_NavigationDataSummary     mNavigationDataSummary;     ///< Navigation/waypoint data for the current platform
   SA_FlightControlsDataSummary mFlightControlsDataSummary; ///< Flight controls data for the current platform

   // Optional Track Processor Names
   std::string mESM_TrackProcessor{""};  ///< Optional string for a track processor for ESM/RWR sensors
   std::string mMWS_TrackProcessor{""};  ///< Optional string for a track processor for missile warning system (MWS)
   std::string mRadarTrackProcessor{""}; ///< Optional string for a track processor for radar sensors
   std::string mIRST_TrackProcessor{""}; ///< Optional string for a track processor for IRST sensors
   std::string mDAS_TrackProcessor{""};  ///< Optional string for a track processor for DAS sensors
   std::string mFLIR_TrackProcessor{""}; ///< Optional string for a track processor for FLIR sensors
   std::string mEyesTrackProcessor{""};  ///< Optional string for a track processor for eyes sensors
   std::string mPerceptionMasterTrackProcessorStr{""}; ///< Optional string for an alternate perception master track processor
   WsfTrackProcessor* mPerceptionMasterTrackProcessorPtr{nullptr}; ///< Pointer to optional track processor
   bool               mUsingPerceptionMasterTrackProcessor{
      false}; ///< True if an optional alternate perception master track processor is being used

   // RF Emissions
   bool mRadarEmitting{false};        ///< True if the radar is emitting, false otherwise
   bool mJammerEmitting{false};       ///< True if the jammer is emitting, false otherwise
   bool mOtherSystemsEmitting{false}; ///< True if other systems are emitting, false otherwise

   double mAssetDataPurgeLifetime{0};        ///< Purge lifetime limit (sec) for asset data
   double mStartupAssetDataPurgeLifetime{0}; ///< Purge lifetime limit at scenario startup
   double mDisplayPerceptionDelay{0.0};
   double mVisualPerceptionDelay{0.0};

   // This structure holds the collection of data members that are used to manage a given type of SA processor update.
   // The vector that follows is used to manage this data for all the SA processor update types.
   struct UpdateMgmtRecord
   {
      double startupInterval{0.0};  // update interval established at startup (multiple of mUpdateInterval)
      double activeInterval{0.0};   // update interval that is currently active (multiple of mUpdateInterval)
      double lastUpdateTime{0.0};   // last simulation time at which this update was performed
      double nextUpdateTime{0.0};   // next simulation time at which this update is requested
      bool   manualModeLock{false}; // true = disallow automated/adaptive changes to this update rate
   };

   std::vector<UpdateMgmtRecord> mUpdateMgmtData; // used to manage data for all SA processor update types
   size_t                        mEventCount{0};  // total number of update events
   PerceiveData                  mData;           // perceived data container used by multiple SAP classes
   bool mUpdatedScriptContext{false}; // set to true if the script context was updated within HandleSA_ProcessorUpdates
   bool mUseDebugOutput{false};       // set to true to output debug information
   std::string mDebugPlatformName{"Talon_1_1"}; // set this to the name of the platform used in debugging
};

#endif
