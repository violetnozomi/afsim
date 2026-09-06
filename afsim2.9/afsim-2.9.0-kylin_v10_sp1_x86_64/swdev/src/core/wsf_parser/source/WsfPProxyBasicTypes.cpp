// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfPProxyBasicTypes.hpp"

#include "UtLexicalCast.hpp"

std::string WsfPProxyEnumType::Write(void* p, WsfParseRule* aRulePtr) const
{
   return ToString(p); // note: this doesn't support multiple formats yet
}

std::string WsfPProxyEnumType::ToString(void* p) const
{
   if (!Get(p).IsUnset())
   {
      auto iter = mIntToString.find(Get(p).GetValue());
      if (iter != mIntToString.end())
      {
         return iter->second;
      }
   }
   return "BadValue";
}

bool WsfPProxyEnumType::IsTypeOf(const WsfPProxyType* aOtherType) const
{
   return aOtherType->mTypeKind == WsfProxy::cINT_VALUE || aOtherType->mTypeKind == WsfProxy::cENUMERATION_VALUE;
}

void WsfPProxyEnumType::Construct(void* p) const
{
   ProxyTypeAdapterT<WsfProxy::Int>::Construct(p);
   Get(p).GetValue() = 0;
}


void WsfPProxyEnumType::SetValue(void* p, const std::string& aText) const
{
   WsfPEnumStrMap::const_iterator iter = mStringToInt.find(aText);
   if (iter != mStringToInt.end())
   {
      Get(p).GetValue() = iter->second;
      Get(p).ClearUnset();
   }
   else
   {
      int intVal;
      if (ut::conversion::try_lexical_convert(aText, intVal))
      {
         if (mIntToString.find(intVal) != mIntToString.end())
         {
            Get(p).GetValue() = intVal;
            Get(p).ClearUnset();
         }
      }
   }
}

void WsfPProxyEnumType::Read(void* p, WsfParseNode* aNodePtr) const
{
   if (aNodePtr->mValue.Valid())
   {
      std::string                    text = WsfParseNode::GetNodeString(aNodePtr);
      WsfPEnumStrMap::const_iterator iter = mStringToInt.find(text);
      if (iter != mStringToInt.end())
      {
         Get(p).GetValue() = iter->second;
         Get(p).ClearUnset();
      }
   }
}

WsfPProxyEnumType::WsfPProxyEnumType()
   : ProxyTypeAdapterT<WsfProxy::Int>()
{
   // Usually, the mTypeKind and mTypeStoredKind match, but there are exceptions, such as enumerated types.
   // You need to handle the specific cases manually, like...
   mTypeName       = wsf::proxy::ValueKindToString(WsfProxy::cENUMERATION_VALUE);
   mTypeKind       = WsfProxy::cENUMERATION_VALUE;
   mTypeStoredKind = WsfProxy::cINT_VALUE; // indicate enumerations are stored as integers.
   mTypeFlags |= WsfProxy::cPOD;
}

WsfPProxyStringType::WsfPProxyStringType()
   : ProxyTypeAdapterT<WsfProxy::String>(0)
{
}

void WsfPProxyStringType::Construct(void* p) const
{
   new (p) WsfProxy::String();
}

WsfPProxyHash WsfPProxyStringType::Hash(void* aDataPtr) const
{
   const std::string& str = Get(aDataPtr).GetValue();
   return WsfPProxyHash(str.c_str(), str.size());
}

size_t WsfPProxyStringType::MemoryUsage(void* aDataPtr) const
{
   return mDataSize + Get(aDataPtr).GetValue().size();
}

std::string WsfPProxyStringType::Write(void* p, WsfParseRule* aRulePtr) const
{
   return ToString(p);
}

WsfPProxyQuotableStringType::WsfPProxyQuotableStringType()
   : ProxyTypeAdapterT<WsfProxy::QuotableString>(0)
{
}

void WsfPProxyQuotableStringType::Construct(void* p) const
{
   new (p) WsfProxy::QuotableString();
}

WsfPProxyHash WsfPProxyQuotableStringType::Hash(void* aDataPtr) const
{
   const std::string& str = Get(aDataPtr).GetValue();
   return WsfPProxyHash(str.c_str(), str.size());
}

size_t WsfPProxyQuotableStringType::MemoryUsage(void* aDataPtr) const
{
   return mDataSize + Get(aDataPtr).GetValue().size();
}

std::string WsfPProxyQuotableStringType::Write(void* p, WsfParseRule* aRulePtr) const
{
   return ToString(p);
}
