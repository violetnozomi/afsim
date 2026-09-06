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

#ifndef WSFSA_GROUP_HPP
#define WSFSA_GROUP_HPP

#include "wsf_air_combat_export.h"

#include <algorithm>
#include <list>

#include "WsfSA_EntityPerception.hpp"
#include "WsfSA_PerceivedItem.hpp"

class WSF_AIR_COMBAT_EXPORT WsfSA_Group : public WsfSA_PerceivedItem
{
public:
   WsfSA_Group() = default;
   explicit WsfSA_Group(const WsfPlatform* aOwningPlatform)
      : WsfSA_PerceivedItem(aOwningPlatform){};
   ~WsfSA_Group() override;
   WsfSA_Group(const WsfSA_Group& aSrc);

   using ElementList = std::list<WsfSA_EntityPerception*>;

   WsfSA_Group* Clone() const { return new WsfSA_Group(*this); }

   // This determines if the specified entity is the same as this entity
   bool IsSame(const WsfSA_PerceivedItem* aOther) const override;

   const char* GetScriptClassName() const override { return "WsfSA_Group"; }

   ItemType GetItemType() const override { return GROUP; }

   // Inherited from WsfSA_PerceivedItem
   double GetLat_deg() const override { return mCentroidLat_deg; }
   void   SetLat_deg(double aLat_deg) override { mCentroidLat_deg = aLat_deg; }

   double GetLon_deg() const override { return mCentroidLon_deg; }
   void   SetLon_deg(double aLon_deg) override { mCentroidLon_deg = aLon_deg; }

   double GetAltitude_ft() const override { return mCentroidAlt_ft; }
   void   SetAltitude_ft(double aAltitude_ft) override { mCentroidAlt_ft = static_cast<float>(aAltitude_ft); }

   double GetSpeed_kts() const override { return mCentroidSpeed_kts; }
   void   SetSpeed_kts(double aSpeed_kts) override { mCentroidSpeed_kts = static_cast<float>(aSpeed_kts); }

   double GetHeading_deg() const override { return mCentroidHeading_deg; }
   void   SetHeading_deg(double aHeading_deg) override { mCentroidHeading_deg = static_cast<float>(aHeading_deg); }

   Importance GetImportance() const override;
   void       SetImportance(Importance aImportance) override;

   Identification GetIdentification() const override;
   void           SetIdentification(Identification aIdent) override { mIdentification = aIdent; }

   std::string GetPerceivedName() const override { return mGroupName; }
   void        SetPerceivedName(const std::string& aIdString) override { mGroupName = aIdString; }

   bool GetAngleOnly() const override { return false; }
   void SetAngleOnly(bool aAngleOnly) override
   {
      ut::log::warning() << "SetAngleOnly not impolemented for WsfSA_Group.";
   } // groups cannot, at this time, be angle-only.
   // **********************************

   float GetRadiusCurrent_nm() const { return mRadiusCurrent_nm; }
   void  SetRadiusCurrent_nm(float aValue) { mRadiusCurrent_nm = aValue; }

   float GetRadiusMinimum_nm() const { return mRadiusMinimum_nm; }
   void  SetRadiusMinimum_nm(float aValue) { mRadiusMinimum_nm = aValue; }

   double GetLastUpdateTime_sec() const { return mLastUpdateTime_sec; }
   void   SetLastUpdateTime_sec(double aValue) { mLastUpdateTime_sec = aValue; }

   bool GetFocus() const { return mIsFocused; }
   void SetFocus(bool aFocus) { mIsFocused = aFocus; }

   bool GetElementProtection() const { return mProtectElements; }
   void SetElementProtection(bool aProtectElements) { mProtectElements = aProtectElements; }

   bool GetFocusProtection() const { return mProtectFocus; }
   void SetFocusProtection(bool aProtectFocus) { mProtectFocus = aProtectFocus; }

   size_t GetNumElements() const { return mElementList.size(); }

   const ElementList& GetElementList() const { return mElementList; }
   void               SetElementList(const ElementList& aList);

   ElementList* ModifiableElementList() { return &mElementList; }

   bool AddElement(WsfSA_EntityPerception* aElement);

   bool RemoveElement(WsfSA_EntityPerception* aElement);

   /** Returns an estimate of the time for this entity to intercept the specified platform.
    * If no intercept is possible, -1.0 will be returned.
    * If the entity is angle-only, or if it lacks speed data, no intercept will be
    * calculated and -1.0 will be returned.
    * @param aTarget pointer to the platform being considered
    */
   double EstimatedTimeToIntercept(WsfPlatform* aTarget) const override;

private:
   std::string mGroupName;
   double      mCentroidLat_deg     = {0.0};
   double      mCentroidLon_deg     = {0.0};
   float       mCentroidAlt_ft      = {0.0f};
   float       mCentroidHeading_deg = {0.0f};
   float       mCentroidSpeed_kts   = {0.0f};
   float       mRadiusCurrent_nm = {static_cast<float>(500.0 * UtMath::cNM_PER_FT)}; // Default current radius is 500 ft
   float       mRadiusMinimum_nm = {static_cast<float>(500.0 * UtMath::cNM_PER_FT)}; // Default minimum radius is 500 ft
   double      mLastUpdateTime_sec = {0.0};
   bool        mIsFocused          = {true};
   bool        mProtectElements    = {false};
   bool        mProtectFocus       = {false};
   Identification mIdentification  = UNKNOWN;
   Importance     mImportance      = {NOMINAL};

   ElementList mElementList;
};

//! WsfScriptSA_GroupClass is a UtScriptClass that defines the methods in
//! WsfSA_Group.  This makes the methods available for use in script.
class WSF_AIR_COMBAT_EXPORT WsfScriptSA_GroupClass : public WsfScriptSA_PerceivedItemClass
{
public:
   WsfScriptSA_GroupClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   ~WsfScriptSA_GroupClass() override = default;

   void Destroy(void* aObjectPtr) override;
   // std::string ToString(void* aObjectPtr) const override;

   UT_DECLARE_SCRIPT_METHOD(GroupName);
   UT_DECLARE_SCRIPT_METHOD(CentroidLat);
   UT_DECLARE_SCRIPT_METHOD(CentroidLon);
   UT_DECLARE_SCRIPT_METHOD(CentroidAlt);
   UT_DECLARE_SCRIPT_METHOD(CentroidHeading);
   UT_DECLARE_SCRIPT_METHOD(CentroidSpeed);
   UT_DECLARE_SCRIPT_METHOD(RadiusCurrent);
   UT_DECLARE_SCRIPT_METHOD(RadiusMinimum);
   UT_DECLARE_SCRIPT_METHOD(LastUpdateTime);
   UT_DECLARE_SCRIPT_METHOD(GetFocus);
   UT_DECLARE_SCRIPT_METHOD(NumElements);
   UT_DECLARE_SCRIPT_METHOD(ElementList);
   UT_DECLARE_SCRIPT_METHOD(SetElementProtection);
   UT_DECLARE_SCRIPT_METHOD(SetFocusProtection);
   UT_DECLARE_SCRIPT_METHOD(GetElementProtection);
   UT_DECLARE_SCRIPT_METHOD(GetFocusProtection);
};

#endif
