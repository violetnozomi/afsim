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

#include "WsfPProxyBasicValue.hpp"

WsfPProxyBasicValue::WsfPProxyBasicValue(const WsfPProxyValue& aVal)
   : WsfPProxyValue(aVal)
{
   if (mTypePtr && !mTypePtr->IsBasicType())
   {
      mTypePtr = nullptr;
   }
   ValueChanged();
}

WsfPProxyBasicValue::WsfPProxyBasicValue(void* aDataPtr, WsfPProxyBasicType* aTypePtr)
   : WsfPProxyValue(aDataPtr, aTypePtr)
{
   ValueChanged();
}

std::string WsfPProxyBasicValue::ToString() const
{
   return mTypePtr ? GetType()->ToString(mDataPtr) : "";
}
bool WsfPProxyBasicValue::IsEqual(const WsfPProxyValue& aRhs) const
{
   const WsfPProxyType* rhsType = aRhs.GetType();
   if (!mTypePtr || !rhsType)
   {
      return false;
   }
   if (rhsType->mTypeStoredKind == mTypePtr->mTypeStoredKind)
   {
      return GetType()->Equal(mDataPtr, aRhs.GetDataPtr());
   }
   return false;
}

void WsfPProxyBasicValue::SetValue(const std::string& aText)
{
   GetType()->SetValue(mDataPtr, aText);
   ValueChanged();
}

void WsfPProxyBasicValue::SetValue(WsfParseNode* aNodePtr)
{
   GetType()->Read(mDataPtr, aNodePtr);
   ValueChanged();
}

void WsfPProxyBasicValue::SetUnset()
{
   GetType()->SetUnset(mDataPtr);
   ValueChanged();
}

void WsfPProxyBasicValue::Read(WsfParseNode* aNodePtr)
{
   GetType()->Read(mDataPtr, aNodePtr);
   ValueChanged();
}

void WsfPProxyBasicValue::Negate()
{
   GetType()->Negate(mDataPtr);
   ValueChanged();
}

int WsfPProxyBasicValue::ToInt() const
{
   if (mTypePtr)
   {
      if (mTypePtr->mTypeStoredKind == WsfProxy::cINT_VALUE)
      {
         return ((WsfProxy::Int*)mDataPtr)->GetValue();
      }
      else if (mTypePtr->mTypeStoredKind == WsfProxy::cDOUBLE_VALUE)
      {
         return static_cast<int>(((WsfProxy::Double*)mDataPtr)->GetValue());
      }
   }
   return 0;
}
