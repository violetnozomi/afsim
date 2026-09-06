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

#ifndef WSFPARSER_HPP
#define WSFPARSER_HPP

#include <list>

#include "UtCallback.hpp"
#include "UtCircularBuffer.hpp"
#include "UtPath.hpp"
#include "UtTextDocument.hpp"
#include "WsfParseDefinitions.hpp"
class WsfParseError;
#include "wsf_parser_export.h"
class WsfParseIndex;
#include "WsfParseNode.hpp"
class WsfParseRule;
class WsfParseSourceProvider;
class WsfParseSourceInclude;

//! WsfParser acts as a lexer for wsf core files.  Tokens are produced
//! delimited by whitespace.  Limited backtracking is allowed using UndoRead().
class WSF_PARSER_EXPORT WsfParser
{
public:
   WsfParser(WsfParseSourceProvider* aSourceProviderPtr,
             const UtPath&           aWorkingDirectory,
             WsfParseDefinitions*    aDefinitionsPtr = nullptr);
   WsfParser(const WsfParser& aRhs);
   WsfParser& operator=(const WsfParser&) = delete;
   ~WsfParser();

   WsfParseNode* ParseFiles(const std::vector<UtPath>& aFileList, int aMaxErrors);

   void PushDeferredSource(const UtPath& aFilePath);

   UtTextDocument* PushSource(const std::string&   aFilePath,
                              bool                 aForceInclude,
                              UtTextDocumentRange* aIncludeLocation    = nullptr,
                              bool                 aSearchIncludePaths = true);

   UtTextDocument* FindSource(const std::string& aFilePath, bool aSearchIncludePaths = true, bool aReadFile = true);

   //! Returns the number of words that have been read since the last call to Reset()
   size_t GetWordsRead() const { return mWordsRead; }

   //! Reads the next white-space-delimited token from the input
   //! @return the white-space-delimited token read from the input if the read succeeded
   //! @retval "an empty or invalid range" if the read failed
   UtTextDocumentRange ReadWord();

   //! Reads the next white-space-delimited terminal token from the input
   //! @return the white-space-delimited terminal token read from the input if the read succeeded
   //! @retval "an empty or invalid range" if the read failed
   //! @note This function is a specialized form of ReadWord for terminal tokens
   UtTextDocumentRange ReadTerminator(const std::string& aTerminator);

   void                UndoRead(size_t aWordCount = 1);
   UtTextDocumentRange UndoReadAndGetRange(size_t aWordCount);
   void                PushToUndoStack(const UtTextDocumentRange& aRange);

   void PartialMatch(UtTextDocumentRange aRange) { mPartialMatches.push_back(aRange); }
   void AddError(WsfParseError* aErrorPtr) { mSharedPtr->mErrors.push_back(aErrorPtr); }
   void AddError(WsfParseRule* aReader, UtTextDocumentRange& aRange);
   void ClearPartialMatches() { mPartialMatches.clear(); }

   WsfParseSourceInclude* GetCurrentInclude() { return mCurrentSourcePtr; }
   WsfParseSourceInclude* GetInclude(UtTextDocument* aParseSourcePtr);

   WsfParseIndex*&      GetParseIndex() { return mParseData; }
   WsfParseDefinitions* GetDefinitions() { return mDefinitions; }
   void                 SetDefinitions(WsfParseDefinitions* aDefinitionsPtr);
   void                 SwapDefinitions(WsfParseDefinitions*& aDefinitionsPtr);

   void PopSource();
   void ProcessSpecialNode(WsfParseNode* aNodePtr);
   void Reset();

   void PushTerminator(const char* aTerminatorString)
   {
      ++mTerminatorPrefixCheck[(unsigned char)*aTerminatorString];
      mTerminatorList.push_back(aTerminatorString);
   }
   void PopTerminator()
   {
      char c = mTerminatorList.back()[0];
      --mTerminatorPrefixCheck[(unsigned char)c];
      mTerminatorList.pop_back();
   }
   void SetWorkingDirectory(const UtPath& aWorkingDirectory) { mSharedPtr->mWorkingDirectory = aWorkingDirectory; }
   const UtPath&                      GetWorkingDirectory() const { return mSharedPtr->mWorkingDirectory; }
   const std::vector<WsfParseError*>& GetErrors() const { return mSharedPtr->mErrors; }

   WsfParseSourceProvider* GetSourceProvider() { return mSourceProvider; }

   WsfParseNode* GetCurrentSequence() { return mCurrentSequenceStack.back(); }
   void          PushSequence(WsfParseNode* aNodePtr) { mCurrentSequenceStack.push_back(aNodePtr); }
   void          PopSequence() { mCurrentSequenceStack.pop_back(); }

   void DelayLoad(const std::string& aKind,
                  const std::string& aName,
                  WsfParseNode*      aNodePtr,
                  ptrdiff_t          aTextOffset,
                  WsfParseRule*      aSequencePtr);

   bool Requires(const std::string& aKind, const std::string& aName);

   void ResolveDelayLoad();

   bool IsDelayLoading() { return mIsDelayLoading; }

   //! Node pool functions

   void DeleteAllNodes() { mSharedPtr->DeleteAllNodes(); }

   void SwapPool(WsfParser& aRhs) { mSharedPtr->SwapPool(*aRhs.mSharedPtr); }
   void SwapPool(WsfParseNodePool& aPool) { mSharedPtr->SwapPool(aPool); }

   WsfParseNode* NewNode(WsfParseRule* aRule, const char* aType, UtTextDocumentRange aValue = UtTextDocumentRange())
   {
      return mSharedPtr->NewNode(aRule, aType, aValue);
   }

   WsfParseNode* NewNode() { return mSharedPtr->NewNode(); }

   void FreeNode(WsfParseNode* aNodePtr)
   {
      // Currently, there is no way to free a single node.  This is a stub to keep track of places
      // that nodes are no longer needed.
   }

   void AddAuxiliaryValue(WsfParseAuxData* aDataPtr) { mSharedPtr->AddAuxiliaryValue(aDataPtr); }

   void FinalizeParseTree(WsfParseNode* aRootNodePtr, std::vector<WsfParseNode*>& aFileTransitionNodes);

   bool FullyExpandNodes() const { return mFullyExpandNodes; }
   void SetFullyExpandNodes(bool aValue) { mFullyExpandNodes = aValue; }
   void SetIgnoreIncludes(bool aIngoreIncludes) { mIgnoreIncludes = aIngoreIncludes; }

private:
   void PushSourceP(UtTextDocument* aSource, UtTextDocumentRange* aIncludeLocation = nullptr, size_t aIncludeCount = 1);

   using DelayLoadId = std::pair<std::string, std::string>;
   class DelayLoader
   {
   public:
      DelayLoader()
         : mNodePtr(nullptr)
         , mRulePtr(nullptr)
         , mTextOffset(0)
         , mIncludePtr(nullptr)
      {
      }

      WsfParseNode* mNodePtr;
      // WsfParseSequence*          mSequencePtr;
      WsfParseRule*          mRulePtr;
      ptrdiff_t              mTextOffset;
      WsfParseSourceInclude* mIncludePtr;
   };

   using DelayLoaderMap = std::multimap<DelayLoadId, DelayLoader>;


   //! For delay loading, the parser must be instanced.  Cloned parser instances share this data:
   class Shared : public WsfParseNodePool
   {
   public:
      Shared()
         : mDelayLoadingCount(0)
      {
      }
      ~Shared();
      void ClearErrors();

      std::vector<WsfParseError*> mErrors;
      UtPath                      mWorkingDirectory;
      DelayLoaderMap              mDelayLoaders;
      std::set<DelayLoadId>       mCompletedDelayLoaders;
      std::set<DelayLoadId>       mLoadingDelayLoaders;
      //! List of delay loader keys in order they were found
      std::vector<DelayLoadId> mDelayLoadOrdering;
      size_t                   mDelayLoadingCount;
      UT_MEMORY_DEBUG_MARKER(cMDB_PARSE_PARSER_SHARED);
   };

   struct SourceData
   {
      UtTextDocument*          mSource;
      WsfParseSourceInclude*   mIncludePtr;
      UtTextDocument::iterator mPos;
   };

   static const int cRECALL_SIZE = 100;
   //! Maximum number of times to include the same file.  Required to avoid infinite loops.
   static const int cMAXIMUM_REINCLUDE_COUNT = 10;

   //! Helper method to read the next white-space-delimited token from the input
   //! @par details
   //!   Processes the input character-by-character to read the next white-space-delimited token from the input.
   //!   Possible tokens are
   //!   * comments (line and block)
   //!   * includes (processed in ReadWord)
   //!   * preprocessor defines
   //!   * preprocessor variables
   //!   * normal tokens
   //! @return the white-space-delimited token read from the input if the read succeeded
   //! @retval "an empty or invalid range" if the read failed
   UtTextDocumentRange ReadWordP();

   Shared*           mSharedPtr;
   UtReferenceCount* mSharedReferenceCountPtr;

   //! Deferred sources are processed one at a time after the source stack is emptied.
   //! This is done to mimic how wsf core applications handle
   //! multiple files from the command line.
   std::list<UtPath> mDeferredSources;

   WsfParseIndex* mParseData;

   //! A count of the number of times ReadWord() has been successfully called
   size_t mWordsRead;

   //! Maintains an index into the files currently being read
   std::vector<SourceData> mSourceStack;

   WsfParseDefinitions* mDefinitions;

   std::vector<UtTextDocumentRange> mPartialMatches;

   std::vector<std::pair<UtTextDocument*, WsfParseSourceInclude*>> mSourceIncludes;

   WsfParseSourceInclude*  mCurrentSourcePtr;
   WsfParseSourceInclude*  mRootSourcePtr;
   WsfParseSourceProvider* mSourceProvider;
   //! Stores the last cRECALL_SIZE words.
   UtFixedCircularBuffer<UtTextDocumentRange> mRecall;
   ptrdiff_t                                  mRecallPosition;
   //! List if terminators.  ReadWord() will stop progressing if one of these is reached.
   std::vector<const char*> mTerminatorList;
   unsigned char            mTerminatorPrefixCheck[256];
   bool                     mCheckForTerminator;

   // UtPath                              mWorkingDirectory;
   std::vector<WsfParseNode*> mCurrentSequenceStack;

   bool mIsDelayLoading;
   bool mIgnoreIncludes;
   bool mInsidePreprocessorVariable;
   //! 'true' if each rule is guaranteed to make a node.
   //! Using 'false' uses less memory skipping the creation of some nodes
   //! Using 'true' may simplify post-processing of the parse tree
   bool mFullyExpandNodes;
   UT_MEMORY_DEBUG_MARKER(cMDB_PARSE_PARSER);
};

class WsfParseError
{
public:
   WsfParseRule*       mReaderPtr;
   UtTextDocumentRange mRange;
};

#endif
