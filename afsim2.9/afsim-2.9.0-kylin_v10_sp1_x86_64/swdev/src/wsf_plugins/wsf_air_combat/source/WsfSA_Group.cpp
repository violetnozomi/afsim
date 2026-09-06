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

#include "WsfSA_Group.hpp"

#include "WsfIntercept.hpp"
#include "WsfPlatform.hpp"

WsfSA_Group::WsfSA_Group(const WsfSA_Group& aSrc)
   : WsfSA_PerceivedItem()
   , mGroupName(aSrc.mGroupName)
   , mCentroidLat_deg(aSrc.mCentroidLat_deg)
   , mCentroidLon_deg(aSrc.mCentroidLon_deg)
   , mCentroidAlt_ft(aSrc.mCentroidAlt_ft)
   , mCentroidHeading_deg(aSrc.mCentroidHeading_deg)
   , mCentroidSpeed_kts(aSrc.mCentroidSpeed_kts)
   , mRadiusCurrent_nm(aSrc.mRadiusCurrent_nm)
   , mRadiusMinimum_nm(aSrc.mRadiusMinimum_nm)
   , mIsFocused(aSrc.mIsFocused)
   , mProtectElements(aSrc.mProtectElements)
   , mProtectFocus(aSrc.mProtectFocus)
   , mIdentification(aSrc.mIdentification)
   , mImportance(aSrc.mImportance)
   , mElementList(aSrc.mElementList)
{
}

WsfSA_Group::~WsfSA_Group()
{
   for (auto element : mElementList)
   {
      element->SetParentGroup(nullptr);
   }
   FreeReference();
}

bool WsfSA_Group::IsSame(const WsfSA_PerceivedItem* aOther) const
{
   if (aOther->GetItemType() != WsfSA_PerceivedItem::GROUP)
   {
      return false;
   }
   const WsfSA_Group* group = dynamic_cast<const WsfSA_Group*>(aOther);
   if (GetPerceivedName() == group->GetPerceivedName())
   {
      return true;
   }

   return false;
}

WsfSA_PerceivedItem::Importance WsfSA_Group::GetImportance() const
{
   for (auto ent : mElementList)
   {
      if (ent->GetImportance() == IMPORTANT)
      {
         return IMPORTANT;
      }
   }
   return mImportance;
}

void WsfSA_Group::SetImportance(Importance aImportance)
{
   mImportance = aImportance;
   if (GetImportance() != mImportance)
   {
      // One of the elements must be important, so warn the user.
      ut::log::warning() << "Group " << GetPerceivedName()
                         << " will remain IMPORTANT until all important entities have been removed from it.";
   }
}

WsfSA_PerceivedItem::Identification WsfSA_Group::GetIdentification() const
{
   if (mIdentification == UNKNOWN)
   {
      // Decide based on elements
      // While there should never be a group that includes both friendlies and bandits,
      // some other combinations are possible, so it's best to account for all possibilities.
      Identification                             ret = UNKNOWN;
      static const std::map<Identification, int> precedence{{UNKNOWN, 0}, {BOGIE, 1}, {NEUTRAL, 2}, {FRIENDLY, 3}, {BANDIT, 4}};
      for (auto ent : mElementList)
      {
         if (precedence.at(ent->GetIdentification()) > precedence.at(ret))
         {
            ret = ent->GetIdentification();
         }
      }
      return ret;
   }
   else
   {
      return mIdentification;
   }
}

void WsfSA_Group::SetElementList(const ElementList& aList)
{
   for (auto element : mElementList)
   {
      if (element)
      {
         element->SetParentGroup(nullptr);
      }
   }
   mElementList = aList;
   if (aList.size() < 2)
   {
      mIsFocused = true; // This must be a degenerate or empty group, so focus it.
   }
   auto elementIter = mElementList.begin();
   while (elementIter != mElementList.end())
   {
      WsfSA_EntityPerception* element = *elementIter;
      if (element)
      {
         auto parent = element->GetParentGroup();
         if (parent && parent != this)
         {
            parent->RemoveElement(element);
         }
         element->SetParentGroup(this);
         ++elementIter;
      }
      else
      {
         elementIter = mElementList.erase(elementIter);
      }
   }
}

bool WsfSA_Group::AddElement(WsfSA_EntityPerception* aElement)
{
   if (aElement)
   {
      auto parent = aElement->GetParentGroup();
      if (parent)
      {
         parent->RemoveElement(aElement);
      }
      aElement->SetParentGroup(this);
      mElementList.push_back(aElement);
      if (mElementList.size() < 2)
      {
         mIsFocused = true; // This must be a degenerate or empty group, so focus it.
      }
      return true;
   }
   return false;
}

bool WsfSA_Group::RemoveElement(WsfSA_EntityPerception* aElement)
{
   if (aElement->GetParentGroup() == this)
   {
      mElementList.remove(aElement);
      aElement->SetParentGroup(nullptr);
      if (mElementList.size() < 2)
      {
         mIsFocused = true; // This must be a degenerate or empty group, so focus it.
      }
      return true;
   }
   return false;
}

double WsfSA_Group::EstimatedTimeToIntercept(WsfPlatform* aTarget) const
{
   if (aTarget != nullptr)
   {
      if (!GetSpeedValid())
      {
         // No estimate of estimated time to intercept if speed is not valid
         return -1.0;
      }

      // Get the target location in WCS
      double tgtLocWCS[3];
      aTarget->GetLocationWCS(tgtLocWCS);

      // Get target velocity in WCS
      double tgtVelWCS[3];
      aTarget->GetVelocityWCS(tgtVelWCS);

      // Get the entity altitude
      double altitude = GetAltitude_ft() * UtMath::cM_PER_FT;
      if (!GetAltitudeValid())
      {
         // If we lack altitude data for the entity, we will assume the same altitude as the target
         altitude = aTarget->GetAltitude();
      }

      // Get the entity location in WCS
      double entityLocWCS[3];
      UtEntity::ConvertLLAToWCS(GetLat_deg(), GetLon_deg(), altitude, entityLocWCS);

      // Get the entity speed
      double entitySpeed = GetSpeed_kts() * UtMath::cMPS_PER_NMPH;

      // Compute the intercept time and location using standard/common function
      double intLocWCS[3];
      double timeToIntercept = WsfIntercept::Intercept(entityLocWCS, entitySpeed, tgtLocWCS, tgtVelWCS, intLocWCS);

      return timeToIntercept;
   }
   else
   {
      // No target, so cannot intercept
      return -1.0;
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WsfScriptSA_GroupClass::WsfScriptSA_GroupClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptSA_PerceivedItemClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSA_Group");
   mConstructible = false;
   mCloneable     = false;
   AddImplicitCastType("WsfSA_PerceivedItem");

   AddMethod(ut::make_unique<GroupName>());
   AddMethod(ut::make_unique<CentroidLat>());
   AddMethod(ut::make_unique<CentroidLon>());
   AddMethod(ut::make_unique<CentroidAlt>());
   AddMethod(ut::make_unique<CentroidHeading>());
   AddMethod(ut::make_unique<CentroidSpeed>());
   AddMethod(ut::make_unique<RadiusCurrent>());
   AddMethod(ut::make_unique<RadiusMinimum>());
   AddMethod(ut::make_unique<LastUpdateTime>());
   AddMethod(ut::make_unique<GetFocus>());
   AddMethod(ut::make_unique<NumElements>());
   AddMethod(ut::make_unique<ElementList>());
   AddMethod(ut::make_unique<SetElementProtection>());
   AddMethod(ut::make_unique<SetFocusProtection>());
   AddMethod(ut::make_unique<GetElementProtection>());
   AddMethod(ut::make_unique<GetFocusProtection>());
}

void WsfScriptSA_GroupClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfSA_Group*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, GroupName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetPerceivedName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, CentroidLat, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLat_deg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, CentroidLon, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLon_deg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, CentroidAlt, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAltitude_ft() * UtMath::cM_PER_FT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, CentroidHeading, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHeading_deg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, CentroidSpeed, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSpeed_kts() * UtMath::cMPS_PER_NMPH);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, RadiusCurrent, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRadiusCurrent_nm() * UtMath::cM_PER_NM);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, RadiusMinimum, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRadiusMinimum_nm() * UtMath::cM_PER_NM);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, LastUpdateTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLastUpdateTime_sec());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, GetFocus, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetFocus());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, NumElements, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetElementList().size()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, ElementList, 0, "Array<WsfSA_EntityPerception>", "")
{
   auto           tempListPtr    = ut::make_unique<ut::script::DataList>();
   UtScriptClass* entityClassPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   // Only return the full list if the group is focused. Otherwise return an empty list.
   if (aObjectPtr->GetFocus())
   {
      for (auto& item : aObjectPtr->GetElementList())
      {
         tempListPtr->emplace_back(UtScriptRef::Ref(item, entityClassPtr));
      }
      aReturnVal.SetPointer(new UtScriptRef(tempListPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(new ut::script::DataList, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, SetElementProtection, 1, "void", "bool")
{
   bool isProtected = aVarArgs[0].GetBool();
   if (aObjectPtr)
   {
      aObjectPtr->SetElementProtection(isProtected);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, SetFocusProtection, 1, "void", "bool")
{
   bool isProtected = aVarArgs[0].GetBool();
   if (aObjectPtr)
   {
      aObjectPtr->SetFocusProtection(isProtected);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, GetElementProtection, 0, "bool", "")
{
   if (aObjectPtr)
   {
      aReturnVal.SetBool(aObjectPtr->GetElementProtection());
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_GroupClass, WsfSA_Group, GetFocusProtection, 0, "bool", "")
{
   if (aObjectPtr)
   {
      aReturnVal.SetBool(aObjectPtr->GetFocusProtection());
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}
