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

#include "WsfParseSourceInclude.hpp"

// For some reason, this is necessary to avoid GCC linker error.
constexpr std::size_t WsfParseSourceInclude::cVIRTUAL_LOCATION;
const UtTextRange     WsfParseSourceInclude::cVIRTUAL_RANGE{WsfParseSourceInclude::cVIRTUAL_LOCATION,
                                                        WsfParseSourceInclude::cVIRTUAL_LOCATION};

WsfParseSourceInclude::~WsfParseSourceInclude()
{
   assert(mParentPtr == nullptr);
   for (size_t i = 0; i < mEntries.size(); ++i)
   {
      auto& childIncludePtr = mEntries[i].mIncludePtr;
      if (childIncludePtr)
      {
         assert(childIncludePtr->mParentPtr == this);
         childIncludePtr->mParentPtr = nullptr;
         mEntries[i].mIncludePtr     = nullptr;
      }
   }
}

void WsfParseSourceInclude::AddInclude(const UtTextRange& aRange, UtTextDocument* aSourcePtr)
{
   if (mEntries.empty())
   {
      AddEntryP(EntryType::cINCLUDE_COMMAND, false, aRange, aSourcePtr, nullptr);
   }
   else
   {
      const Entry& lastEntry = mEntries.back();
      if (!(lastEntry.mType == EntryType::cINCLUDE_COMMAND && lastEntry.mLocation == aRange &&
            lastEntry.mTargetSource == aSourcePtr))
      {
         AddEntryP(EntryType::cINCLUDE_COMMAND, false, aRange, aSourcePtr, nullptr);
      }
   }
}
void WsfParseSourceInclude::AddInclude(const UtTextRange& aRange, WsfParseSourceInclude* aIncludePtr)
{
   if (mEntries.empty())
   {
      AddEntryP(EntryType::cINCLUDE_COMMAND,
                false,
                aRange,
                aIncludePtr->mSourcePtr,
                std::unique_ptr<WsfParseSourceInclude>(aIncludePtr));
   }
   else
   {
      const Entry& lastEntry = mEntries.back();
      if (!(lastEntry.mType == EntryType::cINCLUDE_COMMAND && lastEntry.mLocation == aRange &&
            lastEntry.mIncludePtr.get() == aIncludePtr && lastEntry.mTargetSource == aIncludePtr->mSourcePtr))
      {
         AddEntryP(EntryType::cINCLUDE_COMMAND,
                   false,
                   aRange,
                   aIncludePtr->mSourcePtr,
                   std::unique_ptr<WsfParseSourceInclude>(aIncludePtr));
      }
   }
}
void WsfParseSourceInclude::AddVirtualInclude(WsfParseSourceInclude* aIncludePtr)
{
   if (mEntries.empty())
   {
      AddEntryP(EntryType::cINCLUDE_COMMAND,
                true,
                cVIRTUAL_RANGE,
                aIncludePtr->mSourcePtr,
                std::unique_ptr<WsfParseSourceInclude>(aIncludePtr));
   }
   else
   {
      const Entry& lastEntry = mEntries.back();
      if (!(lastEntry.mType == EntryType::cINCLUDE_COMMAND && lastEntry.mLocation == cVIRTUAL_RANGE &&
            lastEntry.mIncludePtr.get() == aIncludePtr && lastEntry.mTargetSource == aIncludePtr->mSourcePtr))
      {
         AddEntryP(EntryType::cINCLUDE_COMMAND,
                   true,
                   cVIRTUAL_RANGE,
                   aIncludePtr->mSourcePtr,
                   std::unique_ptr<WsfParseSourceInclude>(aIncludePtr));
      }
   }
}

void WsfParseSourceInclude::AddComment(const UtTextRange& aRange, bool aIsLineComment)
{
   if (mEntries.empty())
   {
      AddEntryP((aIsLineComment ? EntryType::cLINE_COMMENT : EntryType::cBLOCK_COMMENT), false, aRange, nullptr, nullptr);
   }
   else
   {
      const Entry& lastEntry = mEntries.back();
      if (!((lastEntry.mType == EntryType::cLINE_COMMENT || lastEntry.mType == EntryType::cBLOCK_COMMENT) &&
            lastEntry.mLocation == aRange))
      {
         AddEntryP((aIsLineComment ? EntryType::cLINE_COMMENT : EntryType::cBLOCK_COMMENT), false, aRange, nullptr, nullptr);
      }
   }
}

void WsfParseSourceInclude::AddPreprocessorEntry(EntryType aEntryType, const UtTextRange& aRange)
{
   if (mEntries.empty())
   {
      AddEntryP(aEntryType, false, aRange, nullptr, nullptr);
   }
   else
   {
      const Entry& lastEntry = mEntries.back();
      if (!(lastEntry.mType == aEntryType && lastEntry.mLocation == aRange))
      {
         AddEntryP(aEntryType, false, aRange, nullptr, nullptr);
      }
   }
}

WsfParseSourceInclude* WsfParseSourceInclude::GetRoot()
{
   WsfParseSourceInclude* root = this;
   while (root->mParentPtr != nullptr)
   {
      root = root->mParentPtr;
   }
   return root;
}

void WsfParseSourceInclude::AddEntryP(EntryType                              aType,
                                      bool                                   aIsVirtualInclude,
                                      UtTextRange                            aLocation,
                                      UtTextDocument*                        aTargetSource,
                                      std::unique_ptr<WsfParseSourceInclude> aIncludePtr)
{
   Entry e;
   e.mType             = aType;
   e.mIsVirtualInclude = aIsVirtualInclude;
   e.mLocation         = aLocation;
   e.mTargetSource     = aTargetSource;
   e.mIncludePtr       = std::move(aIncludePtr);
   if (e.mIncludePtr)
   {
      // Automatically set the child WsfParseSourceInclude's parent
      e.mIncludePtr->mParentPtr = this;
   }
   mEntries.emplace_back(std::move(e));
}
