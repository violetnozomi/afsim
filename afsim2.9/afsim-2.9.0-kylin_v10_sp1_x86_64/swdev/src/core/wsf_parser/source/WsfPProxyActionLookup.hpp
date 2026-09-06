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

#ifndef WSFPPROXYACTIONLOOKUP_HPP
#define WSFPPROXYACTIONLOOKUP_HPP
#include <map>
#include <set>
#include <vector>

#include "WsfParseAction.hpp"

class WsfPProxyType;
class WsfPProxyRegistry;
class WsfParseRule;
class WsfParseAlternate;
class WsfParseNamedRule;
class WsfParseActionAddress;
class WsfParseSequence;
class WsfParseTypeName;
class WsfPProxyActionLookup
{
public:
   WsfPProxyActionLookup(WsfPProxyRegistry* aRegistryPtr);

   void GetRulesThatModifyAttribute(WsfParseAlternate*          aAlternate,
                                    size_t                      aAttributeIndex,
                                    std::vector<WsfParseRule*>& aAlternateChoices);

private:
   void                  RecurseAllRules(WsfParseNamedRule* aNamedRulePtr);
   static WsfPProxyType* GetAddressType(WsfPProxyType* aCurrentTypePtr, WsfParseActionAddress& aAddr);
   void                  GetChangedAttributes_ApplyActions(WsfParseSequence* aSequencePtr,
                                                           WsfParseAction*   aAction,
                                                           std::set<size_t>& aAttributeIndices,
                                                           WsfPProxyType*&   aCurrentTypePtr);

   // Compute the set of attributes that might be changed when executing this rule
   // Indices changed by this rule are added to aAttributeIndices.
   void GetChangedAttributes(WsfParseRule* aRulePtr, std::set<size_t>& aAttributeIndices, WsfPProxyType* aCurrentTypePtr);

   WsfPProxyType* LookupType(WsfPProxyType* aTypePtr, const std::string& aSubType);
   WsfPProxyType* LookupType(WsfPProxyType* aBaseType, WsfParseTypeName& aPath);

   WsfPProxyType*                            mRootType;
   WsfPProxyRegistry*                        mRegistryPtr;
   std::set<WsfParseRule*>                   mRulesEvaluating;
   std::map<WsfParseRule*, std::set<size_t>> mRuleChangedAttributes;
};

#endif
