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

#ifndef WSFPARSEINDEX_HPP
#define WSFPARSEINDEX_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

#include "UtHistoryMap.hpp"
#include "UtMemoryDebug.hpp"
#include "UtReferenceCount.hpp"
#include "UtStringRef.hpp"
#include "UtTextDocument.hpp"
#include "WsfParseType.hpp"
class WsfParser;
class WsfParseRule;
class WsfParseNode;
class WsfParseDefinitions;

//! WsfParseIndex stores a minimal set of information required to continue
//! parsing a wsf core file.  Prior to parsing, this should be empty.
//!
//! Warning: Future code may utilize a hash of this data to do partial
//! re-parsing.  Try not to do anything which would prevent that.
class WSF_PARSER_EXPORT WsfParseIndex
{
public:
   WsfParseIndex(WsfParseDefinitions* aDefinitionsPtr);

   ~WsfParseIndex();

   void AddFilePath(const std::string& aPath);
   void DefinePathVariable(const std::string& aVariableName, const std::string& aValue);
   void UndefinePathVariable(const std::string& aVariableName);
   void ResetFilePath();

   void Clear();

   std::string SubstitutePathVariables(const std::string& aString) const;

   //! Returns 'true' if aFullPath has already been included
   bool HasInclude(const std::string& aFullPath) const
   {
      return (mIncludeCount.find(aFullPath) != mIncludeCount.end());
   }

   //! Add an included file.
   size_t AddInclude(const UtStringRef& aFullPath) { return ++mIncludeCount[aFullPath]; }

   const std::vector<UtStringRef>&      GetIncludePath() const { return mIncludePath; }
   const std::map<UtStringRef, size_t>& GetIncludes() const { return mIncludeCount; }

   void SetParseVariable(const std::string& aName, const UtTextDocumentRange& aValue)
   {
      mParseVariables[aName] = aValue;
   }
   const UtTextDocumentRange& GetParseVariable(const std::string& aName) { return mParseVariables[aName]; }
   void                       ClearParseVariable(const std::string& aName) { mParseVariables.erase(aName); }

   void LoadCurrentType(WsfParseType* aCurrentType)
   {
      assert(aCurrentType != nullptr);
      mCurrentTypeStack.push_back(aCurrentType);
   }
   void          UnloadCurrentType() { mCurrentTypeStack.pop_back(); }
   WsfParseType* GetCurrentType()
   {
      assert(!mCurrentTypeStack.empty());
      return mCurrentTypeStack.back();
   }
   // size_t GetCurrentTypeStackSize() const { return mCurrentTypeStack.size(); }

   // WsfParseTypePath
   // UtHistoryMap<WsfParseTypePath, WsfParseType>  mUserTypeTable;

   WsfParseType mUserTypes;
   int          GenerateName() { return mGeneratedNameCount++; }

private:
   // No implementation:
   WsfParseIndex&                             operator=(const WsfParseIndex& aRhs);
   int                                        mGeneratedNameCount;
   std::map<UtStringRef, UtTextDocumentRange> mParseVariables;

   // For each include path, store the number of times it has been included
   // (always 1 for include_once)
   std::map<UtStringRef, size_t>      mIncludeCount;
   std::vector<UtStringRef>           mIncludePath;
   std::map<UtStringRef, UtStringRef> mPathVariables;

   std::vector<WsfParseType*> mCurrentTypeStack;

   WsfParseDefinitions* mDefinitions;
   UT_MEMORY_DEBUG_MARKER(cMDB_PARSE_INDEX);
};
#endif
