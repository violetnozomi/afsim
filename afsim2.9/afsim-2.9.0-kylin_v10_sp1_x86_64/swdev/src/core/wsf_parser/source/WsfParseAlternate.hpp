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

#ifndef WSFPARSEALTERNATE_HPP
#define WSFPARSEALTERNATE_HPP
#include "WsfParseRule.hpp"
class WsfParseSequence;

// A list of alternates
class WsfParseAlternate : public WsfParseRule
{
public:
   WsfParseAlternate(WsfParseRuleMemory& aMem)
      : WsfParseRule(aMem, cALTERNATE)
      , mFixedLength(false)
      , mAccelerators()
      , mUnfiltered()
   {
   }
   ~WsfParseAlternate() override;
   WsfParseAlternate(const WsfParseAlternate&) = default; // copy constructor

   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;

   bool ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode) override;

   virtual void Initialize();

   const std::vector<WsfParseRule*>& GetReaders() const { return mAlternates; }

   std::vector<WsfParseRule*> GetAlternates() override { return mAlternates; }


   const std::vector<WsfParseRule*>& Alternates() const { return mAlternates; }
   std::vector<WsfParseRule*>&       Alternates() { return mAlternates; }
   std::vector<WsfParseRule*>&       ChangeAlternates() { return mAlternates; }
   void                              AddAlternate(WsfParseRule* aSequence);
   void                              Swap(WsfParseAlternate& aRhs) { std::swap(mAlternates, aRhs.mAlternates); }
   std::string                       GetRuleDescription() override;

protected:
   std::vector<WsfParseRule*> mAlternates;


   int  ReaderStartsWithChar(WsfParseRule* aReaderPtr, char aChar);
   void ClearAccelerators();

   struct CharacterAccelerator
   {
      std::vector<int> mChildIndices;
   };

   bool mFixedLength;
   // For each starting character, the list of possible children that can read
   std::vector<CharacterAccelerator*> mAccelerators;
   // The list of reader child indices that must be tested each time
   CharacterAccelerator mUnfiltered;
};

#endif
