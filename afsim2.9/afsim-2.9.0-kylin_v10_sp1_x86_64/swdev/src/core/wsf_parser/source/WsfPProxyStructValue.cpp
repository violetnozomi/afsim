// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfPProxyStructValue.hpp"

#include "WsfPProxyStructType.hpp"

void* WsfPProxyStructValue::GetMember(size_t aMemberIndex)
{
   return (void*)(GetStruct()->mAttrData[aMemberIndex].mOffset + (char*)GetMemberData());
}

void* WsfPProxyStructValue::GetMember(const std::string& aName)
{
   return GetMember(GetStruct()->GetMemberIndex(aName));
}

size_t WsfPProxyStructValue::GetMemberCount()
{
   return GetStruct()->GetMemberCount();
}

std::string WsfPProxyStructValue::GetMemberName(size_t aMemberIndex) const
{
   if (aMemberIndex < GetStruct()->mAllNames.size())
   {
      return GetStruct()->mAllNames[aMemberIndex];
   }
   return "";
}

WsfPProxyStructValue::WsfPProxyStructValue(WsfPProxyStructHeader* aPtr)
   : WsfPProxyValue(aPtr, aPtr->mStructPtr)
{
}

size_t WsfPProxyStructValue::GetMemberIndex(const std::string& aName) const
{
   return dynamic_cast<const WsfPProxyStructType*>(mTypePtr)->GetMemberIndex(aName);
}

WsfPProxyType* WsfPProxyStructValue::GetMemberType(const std::string& aName) const
{
   auto*  structPtr   = dynamic_cast<const WsfPProxyStructType*>(mTypePtr);
   size_t memberIndex = structPtr->GetMemberIndex(aName);
   if (memberIndex < structPtr->mAttrData.size())
   {
      return structPtr->mAttrData[memberIndex].mTypePtr;
   }
   return nullptr;
}

WsfPProxyType* WsfPProxyStructValue::GetMemberType(size_t aMemberIndex) const
{
   auto* structPtr = dynamic_cast<const WsfPProxyStructType*>(mTypePtr);
   if (aMemberIndex < structPtr->mAttrData.size())
   {
      return structPtr->mAttrData[aMemberIndex].mTypePtr;
   }
   return nullptr;
}
