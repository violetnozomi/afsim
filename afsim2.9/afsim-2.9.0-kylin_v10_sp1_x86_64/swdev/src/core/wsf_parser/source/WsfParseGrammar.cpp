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

#include "WsfParseGrammar.hpp"

namespace WsfGrammar
{
namespace M
{
void Expr::Swap(Expr& aRhs)
{
   std::swap(mRuleType, aRhs.mRuleType);
   std::swap(mType, aRhs.mType);
   std::swap(mLabel, aRhs.mLabel);
   std::swap(mText, aRhs.mText);
   std::swap(mWords, aRhs.mWords);
   std::swap(mFlags, aRhs.mFlags);
}

Expr& Expr::operator=(const Expr& aRhs)
{
   if (this != &aRhs)
   {
      mRuleType = aRhs.mRuleType;
      mType     = aRhs.mType;
      mLabel    = aRhs.mLabel;
      mText     = aRhs.mText;
      // mWords = UtSharablePointer<ExprList>();
      mWords = aRhs.mWords;
      mFlags = aRhs.mFlags;
   }
   return *this;
}

Expr::Expr(const Expr& aSrc)
   : mRuleType(aSrc.mRuleType)
   , mType(aSrc.mType)
   , mLabel(aSrc.mLabel)
   , mText(aSrc.mText)
   , mWords(aSrc.mWords)
   , mFlags(aSrc.mFlags)
{
}

Expr& Expr::GetWord(size_t aIndex)
{
   return (*mWords.Write())[aIndex];
}

const Expr& Expr::GetWord(size_t aIndex) const
{
   return (*mWords.Read())[aIndex];
}

void Expr::NeedWords() const
{
   if (mWords.IsNull())
   {
      mWords = UtSharablePointer<ExprList>(new std::vector<Expr>);
   }
}
} // namespace M
} // namespace WsfGrammar
