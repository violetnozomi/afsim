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

#ifndef WSFPARSERECURRENCE_HPP
#define WSFPARSERECURRENCE_HPP

#include "WsfParseRule.hpp"

class WsfParseRecurrence : public WsfParseRule
{
public:
   WsfParseRecurrence(WsfParseRuleMemory& aMem)
      : WsfParseRule(aMem, cRECURRENCE)
      , mCount(cZERO_TO_MANY)
      , mEndKeyword()
   {
   }
   ~WsfParseRecurrence() override;
   void Initialize(int aSequence) override;
   enum Count
   {
      cZERO_OR_ONE,
      cONE_TO_MANY,
      cZERO_TO_MANY
   };
   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;

   std::vector<WsfParseRule*> GetSequence() override { return std::vector<WsfParseRule*>(1, mSubordinateRulePtr); }

   void GetCountRange(size_t& aMin, size_t& aMax) const
   {
      aMin = 0;
      aMax = 0x7fffffff;
      if (mCount == cZERO_OR_ONE)
      {
         aMax = 1;
      }
      if (mCount == cONE_TO_MANY)
      {
         aMin = 1;
      }
   }

   std::string GetRuleDescription() override
   {
      char c = '*';
      if (mCount == cONE_TO_MANY)
      {
         c = '+';
      }
      if (mCount == cZERO_OR_ONE)
      {
         c = '?';
      }
      return mSubordinateRulePtr->GetRuleDescription() + c;
   }

   void SetRecurringRule(WsfParseRule* aRulePtr);

   Count       mCount;
   std::string mEndKeyword;
};

#endif
