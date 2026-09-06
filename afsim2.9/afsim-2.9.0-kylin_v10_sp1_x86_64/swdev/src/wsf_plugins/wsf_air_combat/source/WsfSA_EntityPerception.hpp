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

#ifndef WSFSA_ENTITYPERCEPTION_HPP
#define WSFSA_ENTITYPERCEPTION_HPP

#include "wsf_air_combat_export.h"

#include "UtScriptAccessible.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
#include "UtVec3.hpp"
#include "WsfSA_PerceivedItem.hpp"

class WsfLocalTrack;
class WsfPlatform;
class WsfProcessor;
class WsfSA_Group;
class WsfSimulation;
class WsfSensor;
class WsfWeapon;

//! The WsfSA_EntityPerception class is used for perception of an entity, which may be friendly (an asset), unknown
//! (a bogie), or a threat (bandit or hostile). An entity perception may include a platform, a track, or other
//! information. A future improvement may include sharing entity perceptions through the use of WsfSA_EntityMessages.
//! Entities include a perceived location, which may be estimated using an assumed range for angle-only tracks, for
//! example.
class WSF_AIR_COMBAT_EXPORT WsfSA_EntityPerception : public WsfSA_PerceivedItem
{
public:
   // Note:  This will be added in the future
   /*
   class SA_EntitySystem
   {
   public:
      SA_EntitySystem();
      SA_EntitySystem(WsfWeapon& aWeapon);
      SA_EntitySystem(WsfSensor& aSensor);
      SA_EntitySystem(WsfProcessor& aProcessor);
      SA_EntitySystem(SA_EntitySystem& aSrc, int ready, double quantity);

      enum SA_SystemKind
      {
         SA_SYSTEM_KIND_NONE      = 0,
         SA_SYSTEM_KIND_SENSOR    = 1,
         SA_SYSTEM_KIND_PROCESSOR = 2,
         SA_SYSTEM_KIND_WEAPON    = 3,
         SA_SYSTEM_KIND_JAMMER    = 4,
         SA_SYSTEM_KIND_UPLINK    = 5
      };

      SA_SystemKind mSystemKind = SA_SYSTEM_KIND_NONE; // Denotes the kind of system (sensor, processor,
                                                       // weapon, jammer, or uplink)
      std::string mSystemType{};                       // WSF type definition (like "RADAR_MISSILE")
      std::string mSystemName{};                       // WSF object name int
      mReadyAssignment = 0;                            // Similar to "channels" or "available request count",
                                                       // this represents the number of tasks this system can
                                                       // still accept
      double mQuantityRemaining = 0.0;
   };
   */

   WsfSA_EntityPerception() = default;

   // This constructor is typically used for assets, with aFriendlyAsset being set to true
   WsfSA_EntityPerception(WsfSimulation*     aSimulation,
                          double             aSimTime,
                          const WsfPlatform* aOwningPlatform,
                          WsfPlatform&       aPlatform,
                          bool               aFriendlyAsset);

   // This constructor is used for tracks
   WsfSA_EntityPerception(WsfSimulation*     aSimulation,
                          double             aSimTime,
                          const WsfPlatform* aOwningPlatform,
                          WsfLocalTrack*     aTrack);

   // This constructor is used with an existing entity perception
   WsfSA_EntityPerception(double aSimTime, const WsfPlatform* aOwningPlatform, const WsfSA_EntityPerception& aSrc);

   WsfSA_EntityPerception(const WsfSA_EntityPerception& aSrc);

   ~WsfSA_EntityPerception() override;

   WsfSA_EntityPerception* Clone() const { return new WsfSA_EntityPerception(*this); }

   WsfSA_EntityPerception& operator=(const WsfSA_EntityPerception& aRhs);

   // NOTE: The operator== and operator!= are intentionally omitted.
   // To compare two entity perceptions, compare using IsSame() instead.
   bool operator==(const WsfSA_EntityPerception& aRhs) = delete;
   bool operator!=(const WsfSA_EntityPerception& aRhs) = delete;

   // This determines if the specified entity is the same as this entity
   bool IsSame(const WsfSA_PerceivedItem* aOther) const override;

   const char* GetScriptClassName() const override { return "WsfSA_EntityPerception"; }
               operator int() { return (int)mEntityPlatformIndex; }
               operator const int() const { return (int)mEntityPlatformIndex; }
               operator size_t() { return mEntityPlatformIndex; }
               operator const size_t() const { return mEntityPlatformIndex; }

   void   SetTargetPlatformIndex(size_t aIndex) { mEntityPlatformIndex = aIndex; }
   size_t GetTargetPlatformIndex() const { return mEntityPlatformIndex; }

   // These functions are used to set (true) or clear (false) various Id settings,
   // without the need to pass a boolean argument. Some will automatically set/clear
   // other Id settings as appropriate.
   void SetIdIffFriend();
   void SetIdIffFoe();
   void SetIdIffNeutral();
   void SetIdIffUnknown();
   void ClearIdIff();
   void SetIdAuxFriend();
   void SetIdAuxFoe();
   void SetIdAuxNeutral();
   void SetIdAuxUnknown();
   void ClearIdAux();
   void SetIdSideFriend();
   void SetIdSideFoe();
   void SetIdSideNeutral();
   void SetIdSideUnknown();
   void ClearIdSide();
   void SetIdTypeFriend();
   void SetIdTypeFoe();
   void SetIdTypeNeutral();
   void SetIdTypeUnknown();
   void ClearIdType();

   bool PerceptionIsMalformed() const { return mMalformedPerception; }
   void SetMalformedCondition() { mMalformedPerception = true; }

   ItemType GetItemType() const override { return ENTITY; }

   double Time() const { return mTime; }
   void   Time(double aTime) { mTime = aTime; }

   size_t GetEntityPlatformIndex() const { return mEntityPlatformIndex; }
   void   SetEntityPlatformIndex(size_t aEntityPlatformIndex) { mEntityPlatformIndex = aEntityPlatformIndex; }

   WsfLocalTrack* GetTrackPtr() const { return mTrackPtr; }
   void           SetTrackPtr(WsfLocalTrack* aTrackPtr) { mTrackPtr = aTrackPtr; }

   double GetLat_deg() const override { return mLat_deg; }
   void   SetLat_deg(double aLat_deg) override { mLat_deg = aLat_deg; }

   double GetLon_deg() const override { return mLon_deg; }
   void   SetLon_deg(double aLon_deg) override { mLon_deg = aLon_deg; }

   double GetAltitude_ft() const override { return mAltitude_ft; }
   void   SetAltitude_ft(double aAltitude_ft) override { mAltitude_ft = aAltitude_ft; }

   double GetSpeed_kts() const override { return mSpeed_kts; }
   void   SetSpeed_kts(double aSpeed_kts) override { mSpeed_kts = aSpeed_kts; }

   double GetHeading_deg() const override { return mHeading_deg; }
   void   SetHeading_deg(double aHeading_deg) override { mHeading_deg = aHeading_deg; }

   Identification GetIdentification() const override { return mIdentification; }
   void           SetIdentification(Identification aIdent) override { mIdentification = aIdent; }

   double GetRisk() const { return mRisk; }
   void   SetRisk(double aRisk) { mRisk = UtMath::Limit(aRisk, 0.0, 1.0); }

   double GetDefensiveness() const { return mDefensiveness; }
   void   SetDefensiveness(double aDefensiveness) { mDefensiveness = UtMath::Limit(aDefensiveness, 0.0, 1.0); }

   double GetUrgency() const { return mUrgency; }
   void   SetUrgency(double aUrgency) { mUrgency = UtMath::Limit(aUrgency, 0.0, 1.0); }

   Importance GetImportance() const override { return mImportance; }
   void       SetImportance(Importance aImportance) override { mImportance = aImportance; }

   std::string GetIdString() const { return mIdString; }
   void        SetIdString(const std::string& aIdString) { mIdString = aIdString; }

   size_t GetPerceivedIndex() const { return mPerceivedIndex; }
   void   SetPerceivedIndex(size_t aIndex) { mPerceivedIndex = aIndex; }

   std::string GetPerceivedName() const override { return mPerceivedName; }
   void        SetPerceivedName(const std::string& aPerceivedName) override { mPerceivedName = aPerceivedName; }

   std::string GetPerceivedType() const { return mPerceivedType; }
   void        SetPerceivedType(const std::string& aPerceivedType) { mPerceivedType = aPerceivedType; }

   uint16_t GetFlightId() const { return mFlightId; }
   void     SetFlightId(uint16_t aFlightId) { mFlightId = aFlightId; }

   std::string GetIdFlag() const { return mIdFlag; }
   void        SetIdFlag(const std::string& aIdFlag) { mIdFlag = aIdFlag; }

   bool GetAngleOnly() const override { return mAngleOnly; }
   void SetAngleOnly(bool aSetting) override { mAngleOnly = aSetting; }

   bool GetIdIffFriend() const { return mIdIffFriend; }
   void SetIdIffFriend(bool aSetting) { mIdIffFriend = aSetting; }

   bool GetIdIffFoe() const { return mIdIffFoe; }
   void SetIdIffFoe(bool aSetting) { mIdIffFoe = aSetting; }

   bool GetIdIffNeutral() const { return mIdIffNeutral; }
   void SetIdIffNeutral(bool aSetting) { mIdIffNeutral = aSetting; }

   bool GetIdIffUnknown() const { return mIdIffUnknown; }
   void SetIdIffUnknown(bool aSetting) { mIdIffUnknown = aSetting; }

   bool GetIdAuxFriend() const { return mIdAuxFriend; }
   void SetIdAuxFriend(bool aSetting) { mIdAuxFriend = aSetting; }

   bool GetIdAuxFoe() const { return mIdAuxFoe; }
   void SetIdAuxFoe(bool aSetting) { mIdAuxFoe = aSetting; }

   bool GetIdAuxNeutral() const { return mIdAuxNeutral; }
   void SetIdAuxNeutral(bool aSetting) { mIdAuxNeutral = aSetting; }

   bool GetIdAuxUnknown() const { return mIdAuxUnknown; }
   void SetIdAuxUnknown(bool aSetting) { mIdAuxUnknown = aSetting; }

   bool GetIdSideFriend() const { return mIdSideFriend; }
   void SetIdSideFriend(bool aSetting) { mIdSideFriend = aSetting; }

   bool GetIdSideFoe() const { return mIdSideFoe; }
   void SetIdSideFoe(bool aSetting) { mIdSideFoe = aSetting; }

   bool GetIdSideNeutral() const { return mIdSideNeutral; }
   void SetIdSideNeutral(bool aSetting) { mIdSideNeutral = aSetting; }

   bool GetIdSideUnknown() const { return mIdSideUnknown; }
   void SetIdSideUnknown(bool aSetting) { mIdSideUnknown = aSetting; }

   bool GetIdTypeFriend() const { return mIdTypeFriend; }
   void SetIdTypeFriend(bool aSetting) { mIdTypeFriend = aSetting; }

   bool GetIdTypeFoe() const { return mIdTypeFoe; }
   void SetIdTypeFoe(bool aSetting) { mIdTypeFoe = aSetting; }

   bool GetIdTypeNeutral() const { return mIdTypeNeutral; }
   void SetIdTypeNeutral(bool aSetting) { mIdTypeNeutral = aSetting; }

   bool GetIdTypeUnknown() const { return mIdTypeUnknown; }
   void SetIdTypeUnknown(bool aSetting) { mIdTypeUnknown = aSetting; }

   bool GetIsMissile() const { return mIsMissile; }
   void SetIsMissile(bool aSetting) { mIsMissile = aSetting; }

   bool GetIsCoasting() const { return mIsCoasting; }
   void SetIsCoasting(bool aSetting) { mIsCoasting = aSetting; }

   bool GetIsNonTrackAsset() const { return mIsNonTrackAsset; }
   void SetIsNonTrackAsset(bool aSetting) { mIsNonTrackAsset = aSetting; }

   bool GetFriendlyAsset() const { return mFriendlyAsset; }
   void SetFriendlyAsset(bool aSetting) { mFriendlyAsset = aSetting; }

   bool GetIsHostile() const { return mIsHostile; }
   void SetIsHostile(bool aSetting) { mIsHostile = aSetting; }

   WsfSA_Group*       GetParentGroup();
   const WsfSA_Group* GetParentGroup() const;
   void               SetParentGroup(WsfSA_Group* aParentGroup);

   double GetLastUpdateTime() const { return mLastUpdateTime; }
   void   SetLastUpdateTime(double aTime) { mLastUpdateTime = aTime; }

   double GetLastExtrapolationTime() const { return mLastExtrapolationTime; }
   void   SetLastExtrapolationTime(double aTime) { mLastExtrapolationTime = aTime; }

   /** Returns an estimate of the time for this entity to intercept the specified platform.
    * If no intercept is possible, -1.0 will be returned.
    * If the entity is angle-only, or if it lacks speed data, no intercept will be
    * calculated and -1.0 will be returned.
    * @param aTarget pointer to the platform being considered
    */
   double EstimatedTimeToIntercept(WsfPlatform* aTarget) const override;

private:
   double         mTime                = 0.0;
   size_t         mEntityPlatformIndex = 0; // Zero is not valid (it will need to be initialized)
   WsfLocalTrack* mTrackPtr            = nullptr;

   // Note: This will be added in the future
   // std::vector<SA_EntitySystem> mSystems;

   double         mLat_deg               = {0.0};
   double         mLon_deg               = {0.0};
   double         mAltitude_ft           = {0.0};
   double         mSpeed_kts             = {0.0};
   double         mHeading_deg           = {0.0};
   double         mRisk                  = {0.0}; // Normalized value (0.0 to 1.0)
   double         mDefensiveness         = {0.0}; // Normalized value (0.0 to 1.0)
   double         mUrgency               = {0.0}; // Normalized value (0.0 to 1.0)
   std::string    mIdString              = {""};
   size_t         mPerceivedIndex        = {0};
   std::string    mPerceivedName         = {""};
   std::string    mPerceivedType         = {""};
   uint16_t       mFlightId              = {0}; // Zero indicates not a member of a flight
   std::string    mIdFlag                = {""};
   bool           mAngleOnly             = {false};
   bool           mIdIffFriend           = {false};
   bool           mIdIffFoe              = {false};
   bool           mIdIffNeutral          = {false};
   bool           mIdIffUnknown          = {false};
   bool           mIdAuxFriend           = {false};
   bool           mIdAuxFoe              = {false};
   bool           mIdAuxNeutral          = {false};
   bool           mIdAuxUnknown          = {false};
   bool           mIdSideFriend          = {false};
   bool           mIdSideFoe             = {false};
   bool           mIdSideNeutral         = {false};
   bool           mIdSideUnknown         = {false};
   bool           mIdTypeFriend          = {false};
   bool           mIdTypeFoe             = {false};
   bool           mIdTypeNeutral         = {false};
   bool           mIdTypeUnknown         = {false};
   bool           mIsMissile             = {false};
   bool           mIsCoasting            = {false};
   bool           mIsNonTrackAsset       = {false};
   bool           mFriendlyAsset         = {false};
   bool           mIsHostile             = {false};
   double         mLastUpdateTime        = {0.0};
   double         mLastExtrapolationTime = {0.0};
   Identification mIdentification        = {UNKNOWN};

   Importance mImportance = {NOMINAL};

   // Careful with this, as it creates a circular reference.
   // An entity perception's lifetime should be less than that of the parent group.
   WsfSA_Group*      mParentGroup = nullptr;
   UtReferenceCount* mParentRC    = nullptr;

   bool mMalformedPerception = {
      false}; // This is used to denote a malformed perception, which can occur for a variety of reasons
   WsfSimulation* mSimulationPtr = {nullptr}; // Required for some calculations (such as to get the platform for target ids)
};

//! WsfScriptSA_EntityPerceptionClass is a UtScriptClass that defines the methods in
//! WsfSA_EntityPerception.  This makes the methods available for use in script.
class WSF_AIR_COMBAT_EXPORT WsfScriptSA_EntityPerceptionClass : public WsfScriptSA_PerceivedItemClass
{
public:
   WsfScriptSA_EntityPerceptionClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   ~WsfScriptSA_EntityPerceptionClass() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;
   // std::string ToString(void* aObjectPtr) const override;

   UT_DECLARE_SCRIPT_METHOD(Index); // truth data - platform simulation index
   UT_DECLARE_SCRIPT_METHOD(Time);

   UT_DECLARE_SCRIPT_METHOD(EntityPlatformIndex);
   UT_DECLARE_SCRIPT_METHOD(Track);
   UT_DECLARE_SCRIPT_METHOD(Lat);
   UT_DECLARE_SCRIPT_METHOD(Lon);
   UT_DECLARE_SCRIPT_METHOD(Altitude);
   UT_DECLARE_SCRIPT_METHOD(Bearing);
   UT_DECLARE_SCRIPT_METHOD(Speed);
   UT_DECLARE_SCRIPT_METHOD(Heading);
   UT_DECLARE_SCRIPT_METHOD(ThreatLevel);
   UT_DECLARE_SCRIPT_METHOD(TargetValue);
   UT_DECLARE_SCRIPT_METHOD(Risk);
   UT_DECLARE_SCRIPT_METHOD(Defensiveness);
   UT_DECLARE_SCRIPT_METHOD(Urgency);
   UT_DECLARE_SCRIPT_METHOD(IdString);
   UT_DECLARE_SCRIPT_METHOD(PerceivedIndex);
   UT_DECLARE_SCRIPT_METHOD(PerceivedName);
   UT_DECLARE_SCRIPT_METHOD(PerceivedType);
   UT_DECLARE_SCRIPT_METHOD(FlightId);
   UT_DECLARE_SCRIPT_METHOD(IdFlag);
   UT_DECLARE_SCRIPT_METHOD(Identification);
   UT_DECLARE_SCRIPT_METHOD(FriendlyAsset);
   UT_DECLARE_SCRIPT_METHOD(AngleOnly);
   UT_DECLARE_SCRIPT_METHOD(IdIffFriend);
   UT_DECLARE_SCRIPT_METHOD(IdIffFoe);
   UT_DECLARE_SCRIPT_METHOD(IdIffNeutral);
   UT_DECLARE_SCRIPT_METHOD(IdIffUnknown);
   UT_DECLARE_SCRIPT_METHOD(IdAuxFriend);
   UT_DECLARE_SCRIPT_METHOD(IdAuxFoe);
   UT_DECLARE_SCRIPT_METHOD(IdAuxNeutral);
   UT_DECLARE_SCRIPT_METHOD(IdAuxUnknown);
   UT_DECLARE_SCRIPT_METHOD(IdSideFriend);
   UT_DECLARE_SCRIPT_METHOD(IdSideFoe);
   UT_DECLARE_SCRIPT_METHOD(IdSideNeutral);
   UT_DECLARE_SCRIPT_METHOD(IdSideUnknown);
   UT_DECLARE_SCRIPT_METHOD(IdTypeFriend);
   UT_DECLARE_SCRIPT_METHOD(IdTypeFoe);
   UT_DECLARE_SCRIPT_METHOD(IdTypeNeutral);
   UT_DECLARE_SCRIPT_METHOD(IdTypeUnknown);
   UT_DECLARE_SCRIPT_METHOD(IsHostile);
   UT_DECLARE_SCRIPT_METHOD(IsMissile);
   UT_DECLARE_SCRIPT_METHOD(AltitudeValid);
   UT_DECLARE_SCRIPT_METHOD(SpeedValid);
   UT_DECLARE_SCRIPT_METHOD(HeadingValid);
   UT_DECLARE_SCRIPT_METHOD(SelectionScore);
   UT_DECLARE_SCRIPT_METHOD(EstimatedTimeToIntercept);

   // Note:  These will be added in the future
   // UT_DECLARE_SCRIPT_METHOD(SystemCount);
   // UT_DECLARE_SCRIPT_METHOD(SystemKind);              //system at index
   // UT_DECLARE_SCRIPT_METHOD(SystemType);              //system at index
   // UT_DECLARE_SCRIPT_METHOD(SystemName);              //system at index
   // UT_DECLARE_SCRIPT_METHOD(SystemReadyAssignment);   //system at index
   // UT_DECLARE_SCRIPT_METHOD(SystemQuantityRemaining); //system at index
   // UT_DECLARE_SCRIPT_METHOD(InterceptLocation);
   // UT_DECLARE_SCRIPT_METHOD(TrueBearingTo);           //degrees
   // UT_DECLARE_SCRIPT_METHOD(RelativeBearingTo);       //degrees
   // UT_DECLARE_SCRIPT_METHOD(ClosestApproachOf);
   // UT_DECLARE_SCRIPT_METHOD(ClosingSpeedOf);
};

#endif
