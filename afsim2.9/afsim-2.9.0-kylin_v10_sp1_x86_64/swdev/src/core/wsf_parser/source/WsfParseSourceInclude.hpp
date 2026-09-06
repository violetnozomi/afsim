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

#ifndef WSFPARSESOURCEINCLUDE_HPP
#define WSFPARSESOURCEINCLUDE_HPP

#include "UtMemory.hpp"
#include "UtTextDocument.hpp"
#include "WsfParseIndex.hpp"

class WsfParseRule;

using WsfParseSourceIncludeId = std::pair<UtTextDocument*, size_t>;

//! Represents an instance where a file is included.  include_once where the file
//! has already been used does not result in a WsfParseSourceInclude.  Most of the time
//! There is a 1-to-1 correlation between this class and UtTextDocument.  This is primarily
//! required for strange usage of include.
class WSF_PARSER_EXPORT WsfParseSourceInclude
{
public:
   static constexpr std::size_t cVIRTUAL_LOCATION = 0x0fffffff;
   static const UtTextRange     cVIRTUAL_RANGE;
   static bool                  IsValidLocation(int aLocation) { return aLocation < 0x0effffff; }
   enum EntryType
   {
      cINCLUDE_COMMAND,
      cLINE_COMMENT,
      cBLOCK_COMMENT,
      cPREPROCESSOR_DEFINE,
      cPREPROCESSOR_VARIABLE
   };

   WsfParseSourceInclude()
      : mParentPtr(nullptr)
      , mSourcePtr(nullptr)
      , mIncludeCount(1)
   {
   }
   WsfParseSourceInclude(const WsfParseSourceInclude&) = delete;
   WsfParseSourceInclude(WsfParseSourceInclude&&)      = default;
   WsfParseSourceInclude& operator=(const WsfParseSourceInclude&) = delete;
   WsfParseSourceInclude& operator=(WsfParseSourceInclude&&) = default;
   ~WsfParseSourceInclude();
   void AddInclude(const UtTextRange& aRange, UtTextDocument* aSourcePtr);
   void AddInclude(const UtTextRange& aRange, WsfParseSourceInclude* aIncludePtr);
   void AddVirtualInclude(WsfParseSourceInclude* aIncludePtr);

   //! Adds a comment entry
   //! @param aRange is the location of the comment in the input file
   //! @param aIsLineComment is whether the comment is a line (# or //) comment
   void AddComment(const UtTextRange& aRange, bool aIsLineComment);

   void AddPreprocessorEntry(EntryType aEntryType, const UtTextRange& aRange);

   WsfParseSourceIncludeId GetId() const { return WsfParseSourceIncludeId(mSourcePtr, mIncludeCount); }

   WsfParseSourceInclude* GetRoot();
   //! The location of the include directive.  This can be null for the 'main' input file.
   UtTextRange            mLocation;
   WsfParseSourceInclude* mParentPtr;
   UtTextDocument*        mSourcePtr;
   //! The number of times this file has been included
   size_t mIncludeCount;

   class Entry
   {
   public:
      Entry()
         : mType(cLINE_COMMENT)
         , mIsVirtualInclude(false)
         , mLocation()
         , mTargetSource(nullptr)
         , mIncludePtr(nullptr)
      {
      }
      Entry(const Entry&) = delete;
      Entry(Entry&&)      = default;
      Entry& operator=(const Entry&) = delete;
      Entry& operator=(Entry&&) = default;
      ~Entry()                  = default;

      EntryType   mType;
      bool        mIsVirtualInclude;
      UtTextRange mLocation;
      // valid for include commands
      UtTextDocument* mTargetSource;
      // valid for include commands if include_once does not prevent it
      std::unique_ptr<WsfParseSourceInclude> mIncludePtr;
   };
   std::vector<Entry> mEntries;

private:
   //! Adds an Entry to this include object's list
   //! @param aType is the EntryType
   //! @param aIsVirtualInclude is whether this Entry is an EntryType::cVIRTUAL_INCLUDE
   //! @param aLocation is the location of the Entry in the input file
   //! @param aTargetSource is the input file containing the Entry
   //! @param aIncludePtr is the WsfParseSourceInclude include object containing the EntryType::cINCLUDE Entry
   void AddEntryP(EntryType                              aType,
                  bool                                   aIsVirtualInclude,
                  UtTextRange                            aLocation,
                  UtTextDocument*                        aTargetSource,
                  std::unique_ptr<WsfParseSourceInclude> aIncludePtr);
};

#endif
