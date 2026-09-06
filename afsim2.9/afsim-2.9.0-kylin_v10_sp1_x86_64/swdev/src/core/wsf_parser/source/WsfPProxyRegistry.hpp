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

#ifndef VIPROXYREGISTRY_HPP
#define VIPROXYREGISTRY_HPP

#include "wsf_parser_export.h"

#include <map>
#include <memory>
#include <vector>

#include "WsfPProxyStructValue.hpp"
#include "WsfPProxyType.hpp"
#include "WsfParseRule.hpp"
#include "WsfParseType.hpp"

class WsfPProxyBasicType;
class WsfPProxyEnumType;
class WsfParseValueType;
class WsfPProxyStringType;
class WsfPProxyStructPointerType;

// Maintains all proxy types.
class WSF_PARSER_EXPORT WsfPProxyRegistry
{
public:
   WsfPProxyRegistry();
   WsfPProxyRegistry(const WsfPProxyRegistry&) = delete;
   WsfPProxyRegistry& operator=(const WsfPProxyRegistry&) = delete;
   ~WsfPProxyRegistry();

   void Setup(WsfParseStruct* aRootStructPtr, WsfParseType* aBasicTypes);
   // void Test(WsfParseNode* aRoot);
   void                        FindStructs(WsfParseNamedRule* aRootPtr, std::vector<WsfParseStruct*>& aStructs);
   WsfPProxyType*              GetType(const std::string& aType);
   WsfPProxyType*              GetType(WsfParseNamedRule* aRulePtr);
   WsfPProxyType*              GetType(WsfParseValueType* aTypePtr);
   WsfPProxyType*              FindBasicType(const WsfParseTypePath& aPath);
   WsfPProxyStructType*        GetStruct(WsfParseStruct* aStructPtr);
   WsfPProxyStructPointerType* GetPointerType(const WsfPProxyType* aTypePtr);
   WsfPProxyEnumType*          GetEnum(const std::string& aFullName, WsfParseEnumerationValues& aValues);
   void                        CreateDefaultStruct(WsfParseStruct* aStructPtr);
   WsfPProxyStructValue        GetBasicRoot() { return mBasicRoot; }
   void           PrintProxy(WsfPProxyValue aRoot, WsfPProxyValue aValue, ut::log::MessageStream& aStream) const;
   WsfPProxyType* GetStringType() const { return mDataTypes.at("String").get(); }

   void InitializeStruct(WsfPProxyStructType* aStructPtr);
   void BuildBasicRootFromBasicTypes(WsfPProxyValue& aOutput, WsfParseType* aInput);

   void OutputJSON(WsfPProxyValue aVal, WsfPProxyValue aRoot, std::ostream& aStream, int aOutputOptions);

   using DataTypeMap = std::map<std::string, std::unique_ptr<WsfPProxyType>>;

   void*                                           mApplicationPointer;
   WsfParseNamedRule*                              mRootRule;
   WsfParseNamedRule*                              mEntryPointRule;
   DataTypeMap                                     mDataTypes; // mBasicValueTypes;
   std::map<WsfParseStruct*, WsfPProxyStructType*> mStructs;

   WsfParseType* mBasicTypes;

   WsfPProxyStructValue mBasicRoot; // a mirror of the root containing base types.

   UT_MEMORY_DEBUG_MARKER(cMDB_PROXY_REGISTRY);

private:
   WsfPProxyType* GetAttributeType(WsfParseStruct* aStructPtr, WsfParseAttribute& aAttr);
};

#endif
