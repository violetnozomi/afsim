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

#ifndef WSFSA_PERCEIVEDITEM_HPP
#define WSFSA_PERCEIVEDITEM_HPP

#include "wsf_air_combat_export.h"

#include "UtReferenceTracked.hpp"
#include "UtScriptAccessible.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
#include "UtVec3.hpp"
#include "script/WsfScriptObjectClass.hpp"

class WsfLocalTrack;
class WsfPlatform;
class WsfProcessor;
class WsfSimulation;
class WsfSensor;
class WsfWeapon;

/** The WsfSA_PerceivedItem class is used as a base class for WsfSA_EntityPerception and WsfSA_Group,
 * which may be friendly (an asset), unknown (a bogie), or a threat (bandit or hostile).
 * Anything that contributes to an SAP's perceived item count is a WsfSA_PerceivedItem, and
 * contains information about its name, location, heading, etc. Perceived items will typically
 * live on the heap, so care hould be taken with managing their memory.
 */
class WSF_AIR_COMBAT_EXPORT WsfSA_PerceivedItem : public UtScriptAccessible, public UtReferenceTracked
{
public:
   enum Identification
   {
      UNKNOWN  = 0,
      FRIENDLY = 1,
      NEUTRAL  = 2,
      BOGIE    = 3,
      BANDIT   = 4
   };

   enum ItemType
   {
      ENTITY = 0,
      GROUP  = 1
   };

   enum Importance
   {
      IGNORED   = 0,
      NOMINAL   = 1,
      IMPORTANT = 2
   };

   // Static Functions
   static double GetAssumedRangeForAngleOnlyTargets() { return mAssumedRangeForAngleOnlyTargets; }
   static void   SetAssumedRangeForAngleOnlyTargets(double aRange) { mAssumedRangeForAngleOnlyTargets = aRange; }

   WsfSA_PerceivedItem()
      : UtScriptAccessible(){};

   WsfSA_PerceivedItem(const WsfPlatform* aOwningPlatform)
      : UtScriptAccessible()
      , mOwningPlatform(aOwningPlatform){};

   WsfSA_PerceivedItem(const WsfSA_PerceivedItem& aSrc);

   WsfSA_PerceivedItem& operator=(const WsfSA_PerceivedItem& aRhs);

   // NOTE: The operator== and operator!= are intentionally omitted.
   // To compare two entity perceptions, compare using IsSame() instead.
   bool operator==(const WsfSA_PerceivedItem& aRhs) = delete;
   bool operator!=(const WsfSA_PerceivedItem& aRhs) = delete;

   // This determines if the specified item is the same as this item
   virtual bool IsSame(const WsfSA_PerceivedItem* aOther) const = 0;

   const char* GetScriptClassName() const override { return "WsfSA_PerceivedItem"; }

   // Calculates the range and returns it. Also, it internally sets mSelectionScore to the range.
   double CalcRangeForPlatform(const WsfPlatform* aPlatform);

   /** Find the angle off heading from this PI's owning platform, in degrees -180 to 180.*/
   double AngleOffHeadingFromOwningPlatform_deg();

   const WsfPlatform* OwningPlatform() const { return mOwningPlatform; }
   void               OwningPlatform(const WsfPlatform* aPlatform) { mOwningPlatform = aPlatform; }

   bool PerceptionIsMalformed() const { return mMalformedPerception; }
   void SetMalformedCondition() { mMalformedPerception = true; }

   static bool CompareScoreLowGood(const WsfSA_PerceivedItem* aFirstItem, const WsfSA_PerceivedItem* aSecondItem);
   static bool CompareScoreHighGood(const WsfSA_PerceivedItem* aFirstItem, const WsfSA_PerceivedItem* aSecondItem);

   static bool CompareThreatLevelHighGood(const WsfSA_PerceivedItem* aFirstItem, const WsfSA_PerceivedItem* aSecondItem);
   static bool CompareTargetValueHighGood(const WsfSA_PerceivedItem* aFirstItem, const WsfSA_PerceivedItem* aSecondItem);

   virtual ItemType GetItemType() const = 0;

   virtual double GetLat_deg() const          = 0;
   virtual void   SetLat_deg(double aLat_deg) = 0;

   virtual double GetLon_deg() const          = 0;
   virtual void   SetLon_deg(double aLon_deg) = 0;

   virtual double GetAltitude_ft() const              = 0;
   virtual void   SetAltitude_ft(double aAltitude_ft) = 0;

   virtual double GetBearing_deg() const { return mBearing_deg; }
   virtual void   SetBearing_deg(double aBearing_deg) { mBearing_deg = aBearing_deg; }

   virtual double GetSpeed_kts() const            = 0;
   virtual void   SetSpeed_kts(double aSpeed_kts) = 0;

   virtual double GetHeading_deg() const              = 0;
   virtual void   SetHeading_deg(double aHeading_deg) = 0;

   virtual double GetThreatLevel() const { return mThreatLevel; }
   virtual void   SetThreatLevel(double aThreatLevel) { mThreatLevel = UtMath::Limit(aThreatLevel, 0.0, 1.0); }

   virtual double GetTargetValue() const { return mTargetValue; }
   virtual void   SetTargetValue(double aTargetValue) { mTargetValue = UtMath::Limit(aTargetValue, 0.0, 1.0); }

   virtual Identification GetIdentification() const                = 0;
   virtual void           SetIdentification(Identification aIdent) = 0;

   virtual Importance GetImportance() const                 = 0;
   virtual void       SetImportance(Importance aImportance) = 0;

   virtual std::string GetPerceivedName() const                       = 0;
   virtual void        SetPerceivedName(const std::string& aIdString) = 0;

   virtual bool GetAngleOnly() const        = 0;
   virtual void SetAngleOnly(bool aSetting) = 0;

   virtual bool GetAltitudeValid() const { return mAltitudeValid; }
   virtual void SetAltitudeValid(bool aSetting) { mAltitudeValid = aSetting; }

   virtual bool GetSpeedValid() const { return mSpeedValid; }
   virtual void SetSpeedValid(bool aSetting) { mSpeedValid = aSetting; }

   virtual bool GetHeadingValid() const { return mHeadingValid; }
   virtual void SetHeadingValid(bool aSetting) { mHeadingValid = aSetting; }

   virtual double GetSelectionScore() const { return mSelectionScore; }
   virtual void   SetSelectionScore(double aSelectionScore) { mSelectionScore = aSelectionScore; }

   /** Returns an estimate of the time for this entity to intercept the specified platform.
    * If no intercept is possible, -1.0 will be returned.
    * If the entity is angle-only, or if it lacks speed data, no intercept will be
    * calculated and -1.0 will be returned.
    * @param aTarget pointer to the platform being considered
    */
   virtual double EstimatedTimeToIntercept(WsfPlatform* aTarget) const = 0;

   // -----------------------------------------------------------------------------------------------------------------------------------------------
   // Static data

   static double mAssumedRangeForAngleOnlyTargets; // This is used for tracks lacking range or location data

private:
   const WsfPlatform* mOwningPlatform = nullptr;

   double      mBearing_deg    = {0.0};
   double      mThreatLevel    = {0.0}; // Normalized value (0.0 to 1.0)
   double      mTargetValue    = {0.0}; // Normalized value (0.0 to 1.0)
   std::string mPerceivedType  = {""};
   bool        mAltitudeValid  = {false};
   bool        mSpeedValid     = {false};
   bool        mHeadingValid   = {false};
   double      mSelectionScore = {0.0};

   bool mMalformedPerception = {
      false}; // This is used to denote a malformed perception, which can occur for a variety of reasons
   WsfSimulation* mSimulationPtr = {nullptr}; // Required for some calculations (such as to get the platform for target ids)
};

//! WsfScriptSA_PerceivedItemClass is a UtScriptClass that defines the methods in
//! WsfSA_PerceivedItem.  This makes the methods available for use in script.
class WSF_AIR_COMBAT_EXPORT WsfScriptSA_PerceivedItemClass : public WsfScriptObjectClass
{
public:
   WsfScriptSA_PerceivedItemClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   ~WsfScriptSA_PerceivedItemClass() override = default;

   void        Destroy(void* aObjectPtr) override;
   std::string ToString(void* aObjectPtr) const override;
   void        OnNewScriptRef(UtScriptRef& aReference) override;

   UT_DECLARE_SCRIPT_METHOD(ItemType);
   UT_DECLARE_SCRIPT_METHOD(Lat);
   UT_DECLARE_SCRIPT_METHOD(Lon);
   UT_DECLARE_SCRIPT_METHOD(Altitude);
   UT_DECLARE_SCRIPT_METHOD(Bearing);
   UT_DECLARE_SCRIPT_METHOD(Speed);
   UT_DECLARE_SCRIPT_METHOD(Heading);
   UT_DECLARE_SCRIPT_METHOD(ThreatLevel);
   UT_DECLARE_SCRIPT_METHOD(TargetValue);
   UT_DECLARE_SCRIPT_METHOD(PerceivedName);
   UT_DECLARE_SCRIPT_METHOD(Identification);
   UT_DECLARE_SCRIPT_METHOD(AngleOnly);
   UT_DECLARE_SCRIPT_METHOD(AltitudeValid);
   UT_DECLARE_SCRIPT_METHOD(SpeedValid);
   UT_DECLARE_SCRIPT_METHOD(HeadingValid);
   UT_DECLARE_SCRIPT_METHOD(SelectionScore);
   UT_DECLARE_SCRIPT_METHOD(EstimatedTimeToIntercept);
};

#endif // WSFSA_PERCEIVEDITEM_HPP
