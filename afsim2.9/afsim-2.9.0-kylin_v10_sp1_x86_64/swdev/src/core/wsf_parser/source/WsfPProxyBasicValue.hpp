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

#ifndef WSFPPROXYBASICVALUE_HPP
#define WSFPPROXYBASICVALUE_HPP

#include "wsf_parser_export.h"

// clang-format off
#include "WsfPProxyValue.hpp"
#include "WsfPProxyType.hpp"
// clang-format on

// When WSF_PROXY_VALUE_DEBUG is defined, we save the string representation of the value after
// each change.  This lets you easily see the value in the debugger, but requires a lot of overhead.
//
//#define WSF_PROXY_VALUE_DEBUG

// A Wrapper to hold / operate on a proxy value with a basic type
class WSF_PARSER_EXPORT WsfPProxyBasicValue : public WsfPProxyValue
{
public:
   WsfPProxyBasicValue()
      : WsfPProxyValue()
   {
   }

   WsfPProxyBasicValue(const WsfPProxyValue& aVal);
   WsfPProxyBasicValue(void* aDataPtr, WsfPProxyBasicType* aTypePtr);
   WsfPProxyBasicType* GetType() const { return (WsfPProxyBasicType*)mTypePtr; }
   std::string         ToString() const;
   int                 ToInt() const;
   void                Read(WsfParseNode* aNodePtr);
   void                SetValue(const std::string& aText);

   void SetValue(WsfParseNode* aNodePtr);

   void SetUnset();

   void Negate();
   bool IsEqual(const WsfPProxyValue& aRhs) const;
#ifdef WSF_PROXY_VALUE_DEBUG
   void        ValueChanged() { mStringValue = ToString(); }
   std::string mStringValue;
#else
   void ValueChanged() {} // no-op
#endif
};

#endif
