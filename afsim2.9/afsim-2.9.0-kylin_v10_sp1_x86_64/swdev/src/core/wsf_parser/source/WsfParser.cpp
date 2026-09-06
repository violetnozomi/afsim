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

#include "WsfParser.hpp"

#include "UtCast.hpp"
#include "UtPath.hpp"
#include "UtStringUtil.hpp"

class WsfApplication;
#include "WsfParseIndex.hpp"
#include "WsfParseNode.hpp"
#include "WsfParseSequence.hpp"
#include "WsfParseSourceInclude.hpp"
#include "WsfParseSourceProvider.hpp"
#include "WsfParseUtil.hpp"

//
// An extension which parses the grammar in WSF.
//
void WSF_PARSER_EXPORT Register_wsf_parser(WsfApplication& aApplication)
{
   // aApplication.RegisterExtension("wsf_parser", new WsfDefaultApplicationExtension<WsfParserExtension>());
}

WsfParser::WsfParser(WsfParseSourceProvider* aSourceProviderPtr,
                     const UtPath&           aWorkingDirectory,
                     WsfParseDefinitions*    aDefinitionsPtr)
   : mWordsRead(0)
   , mDefinitions(aDefinitionsPtr)
   , mSourceProvider(aSourceProviderPtr)
   , mRecall(cRECALL_SIZE)
   , mTerminatorPrefixCheck{0}
   , mCheckForTerminator(true)
   , mIsDelayLoading(false)
   , mIgnoreIncludes(false)
   , mInsidePreprocessorVariable(false)
   , mFullyExpandNodes(false)
{
   mSharedPtr                    = new Shared;
   mSharedPtr->mWorkingDirectory = aWorkingDirectory;

   mSharedReferenceCountPtr = new UtReferenceCount(1);

   mCurrentSequenceStack.push_back(nullptr);
   mRecallPosition = -1;

   WsfParseUtil::InitializeParseUtil();
   if (mDefinitions == nullptr)
   {
      mDefinitions = new WsfParseDefinitions;
   }
   mCurrentSourcePtr = nullptr;
   mRootSourcePtr    = nullptr;
   mParseData        = new WsfParseIndex(mDefinitions);
}

WsfParser::WsfParser(const WsfParser& aRhs)
   : mSharedPtr(aRhs.mSharedPtr)
   , mSharedReferenceCountPtr(aRhs.mSharedReferenceCountPtr)
   , mDeferredSources()
   , mParseData(aRhs.mParseData)
   , mWordsRead(0)
   , mSourceStack()
   , mDefinitions(aRhs.mDefinitions)
   , mPartialMatches()
   , mSourceIncludes()
   , //???
   mCurrentSourcePtr(nullptr)
   , mRootSourcePtr(nullptr)
   , mSourceProvider(aRhs.mSourceProvider)
   , mRecall(cRECALL_SIZE)
   , mRecallPosition(-1)
   , mTerminatorList()
   , mTerminatorPrefixCheck{0}
   , mCheckForTerminator(true)
   , mCurrentSequenceStack()
   , mIsDelayLoading(true)
   , mIgnoreIncludes(aRhs.mIgnoreIncludes)
   , mInsidePreprocessorVariable(aRhs.mInsidePreprocessorVariable)
   , mFullyExpandNodes(aRhs.mFullyExpandNodes)
{
   mSharedReferenceCountPtr->AddStrongRef();
}

WsfParser::~WsfParser()
{
   if (mRootSourcePtr)
   {
      delete mRootSourcePtr;
   }
   if (mSharedReferenceCountPtr->RemoveStrongRef())
   {
      delete mSharedPtr;
   }
   if (!mIsDelayLoading)
   {
      delete mParseData;
      delete mDefinitions;
   }
}

//! Store a parsed word for later retrieval after UndoRead()
void WsfParser::PushToUndoStack(const UtTextDocumentRange& aRange)
{
   assert(mRecallPosition == -1);
   mRecall.Push(aRange);
}

UtTextDocumentRange WsfParser::ReadWord()
{
   bool                foundInclude;
   UtTextDocumentRange word;
   do
   {
      // Assume the command is not an include on each iteration
      foundInclude = false;

      word = ReadWordP();
      if (word.Valid())
      {
         // Check for the "inc" substring.
         // This substring may indicate the presence of an "include" or "include_once" command.
         const char* wordPtr = word.Pointer();
         if (wordPtr[0] == 'i' && wordPtr[1] == 'n' && wordPtr[2] == 'c')
         {
            bool isInclude     = word == "include";
            bool isIncludeOnce = word == "include_once";
            if (isInclude || isIncludeOnce)
            {
               // This is an "include" or "include_once" command.
               // Read the file name.
               UtTextDocumentRange fileWord = ReadWordP();
               if (fileWord.Valid())
               {
                  const char* fileNameBegin = fileWord.Pointer();
                  const char* fileNameEnd   = fileNameBegin + fileWord.Length();
                  // Trim any leading and trailing whitespace from the file name
                  WsfParseUtil::TrimIter<const char*>(fileNameBegin, fileNameEnd);

                  // Create a range containing the "include" or "include_once" command and the file name
                  UtTextDocumentRange range(word);
                  range.Extend(fileWord);
                  if (!mIgnoreIncludes)
                  {
                     PushSource(std::string(fileNameBegin, fileNameEnd), isInclude, &range);
                  }

                  // Remove any trace of the "include" or "include_once" from the recall
                  assert(mWordsRead >= 2);
                  mWordsRead -= 2;
                  mRecall.Pop(2);

                  // Found an include, so continue processing commands
                  foundInclude = true;
               }
            }
         }
         if (mCheckForTerminator && mTerminatorPrefixCheck[(unsigned char)*wordPtr])
         {
            for (const auto& terminator : mTerminatorList)
            {
               if (word == terminator)
               {
                  word = UtTextDocumentRange{};
               }
            }
         }
      }
   } while (foundInclude);

   // This return value is one of:
   // 1) a valid range from the first ReadWordP call indicating the ReadWord succeeded.
   // 2) an empty range indicating the ReadWord failed.
   return word;
}

UtTextDocumentRange WsfParser::ReadTerminator(const std::string& aTerminator)
{
   // Ensure that the parser is checking for a terminal token.
   // Then, reset the flag.
   assert(mCheckForTerminator);
   mCheckForTerminator = false;
   // Read the terminal token
   UtTextDocumentRange word = ReadWord();
   if (word.Valid())
   {
      if (!(word == aTerminator))
      {
         // Invalidate the range if the terminal token is not the passed-in terminal token
         word.mSource = nullptr;
      }
   }
   mCheckForTerminator = true;
   return word;
}

UtTextDocument* WsfParser::FindSource(const std::string& aFilePath, bool aSearchIncludePaths, bool aReadFile)
{
   // Default return value
   UtTextDocument* sourcePtr(nullptr);

   // Get the file path as a string
   std::string pathName = mParseData->SubstitutePathVariables(aFilePath);

   // First check relative to current input file
   if (!mSourceStack.empty())
   {
      UtPath currentFilePath(mSourceStack.back().mSource->GetFilePath());
      currentFilePath.Up();
      UtPath fullpath = currentFilePath + pathName;
      sourcePtr       = mSourceProvider->FindSource(fullpath, aReadFile);
   }

   // If source is not found
   if (sourcePtr == nullptr)
   {
      // Search include paths in order
      const std::vector<UtStringRef>& includePaths = mParseData->GetIncludePath();
      if (!includePaths.empty())
      {
         // Attempt to locate the file using the file_path list
         size_t maxIndex = includePaths.size();
         if (!aSearchIncludePaths)
         {
            maxIndex = 1;
         }
         for (size_t i = 0; i != maxIndex && !sourcePtr; ++i)
         {
            const std::string& includePath = includePaths[i];
            UtPath             path        = UtPath(includePath) + pathName;
            UtPath             fullpath    = mSharedPtr->mWorkingDirectory + path;
            sourcePtr                      = mSourceProvider->FindSource(fullpath, aReadFile);
         }
      }

      // If no file_path directives have been defined or
      // the source still has not been found look in current working directory
      if (includePaths.empty() || (sourcePtr == nullptr))
      {
         UtPath fullpath = mSharedPtr->mWorkingDirectory + pathName;
         sourcePtr       = mSourceProvider->FindSource(fullpath, aReadFile);
      }
   }

   return sourcePtr;
}

//! Add a source text to be parsed next.  This has the same effect of an 'include'.
//! @param aFilePath The path to the file, may be relative to any path variables
//! @param aForceInclude 'false' if the file should only be added once (like 'include_once')
//! @param aIncludeLocation Location input is being included from
//! @param aSearchIncludePaths 'true' if include paths should be searched for source
UtTextDocument* WsfParser::PushSource(const std::string&   aFilePath,
                                      bool                 aForceInclude,
                                      UtTextDocumentRange* aIncludeLocation,
                                      bool                 aSearchIncludePaths)
{
   UtTextDocument* sourcePtr = FindSource(aFilePath, aSearchIncludePaths);
   if (sourcePtr != nullptr)
   {
      bool included = false;
      if (aForceInclude || !mParseData->HasInclude(sourcePtr->GetFilePath().GetNormalizedPath()))
      {
         size_t includeCount = mParseData->AddInclude(sourcePtr->GetFilePath().GetNormalizedPath());
         if (includeCount < cMAXIMUM_REINCLUDE_COUNT)
         {
            included = true;
            PushSourceP(sourcePtr, aIncludeLocation, includeCount);
         }
      }
      if (!included && mCurrentSourcePtr && aIncludeLocation)
      {
         // note the include_once
         mCurrentSourcePtr->AddInclude(*aIncludeLocation, sourcePtr);
      }
   }
   else
   {
      if (mCurrentSourcePtr && aIncludeLocation)
      {
         // note the include failure
         mCurrentSourcePtr->AddInclude(*aIncludeLocation, (UtTextDocument*)nullptr);
      }
   }
   return sourcePtr;
}

void WsfParser::PushSourceP(UtTextDocument* aSource, UtTextDocumentRange* aIncludeLocation, size_t aIncludeCount)
{
   WsfParseSourceInclude* includePtr = new WsfParseSourceInclude;
   includePtr->mSourcePtr            = aSource;
   includePtr->mIncludeCount         = aIncludeCount;
   if (mCurrentSourcePtr != nullptr)
   {
      if (aIncludeLocation)
      {
         mCurrentSourcePtr->AddInclude(*aIncludeLocation, includePtr);
         includePtr->mLocation = *aIncludeLocation;
      }
      else
      {
         mCurrentSourcePtr->AddVirtualInclude(includePtr);
         includePtr->mLocation.SetRange(WsfParseSourceInclude::cVIRTUAL_LOCATION, WsfParseSourceInclude::cVIRTUAL_LOCATION);
      }
      includePtr->mParentPtr = mCurrentSourcePtr;
   }
   if (!mCurrentSourcePtr || !aIncludeLocation)
   {
      includePtr->mLocation =
         UtTextRange(WsfParseSourceInclude::cVIRTUAL_LOCATION, WsfParseSourceInclude::cVIRTUAL_LOCATION);
   }

   SourceData dat;
   dat.mSource     = aSource;
   dat.mPos        = aSource->Begin();
   dat.mIncludePtr = includePtr;
   mSourceStack.push_back(dat);
   mCurrentSourcePtr = includePtr;

   mSourceIncludes.emplace_back(dat.mSource, dat.mIncludePtr);
}

UtTextDocumentRange WsfParser::ReadWordP()
{
   if (mRecallPosition >= 0)
   {
      UtTextDocumentRange word = mRecall[mRecallPosition];
      --mRecallPosition;
      ++mWordsRead;
      return word;
   }
   enum State
   {
      cNORMAL_STATE        = 0,
      cLINE_COMMENT_STATE  = 1,
      cBLOCK_COMMENT_STATE = 2,
      cDEFINE_STATE        = 3,
      cVARIABLE_NAME       = 4,
      cVARIABLE_END        = 5
   };
   bool moreSource = false;
   do
   {
      moreSource = false;
      while (!mSourceStack.empty())
      {
         State                    parseState(cNORMAL_STATE);
         bool                     hasText = false;
         size_t                   commentStart(0);
         SourceData&              currentSource   = mSourceStack.back();
         UtTextDocument*          sourcePtr       = currentSource.mSource;
         UtTextDocument::iterator pos             = currentSource.mPos;
         UtTextDocument::iterator endIter         = currentSource.mSource->End();
         UtTextDocument::iterator endMinusOneIter = currentSource.mSource->End() - 1;
         if (pos == endIter)
         {
            PopSource();
            continue;
         }
         UtTextDocument::iterator startRange = pos;
         UtTextDocument::iterator endRange   = pos;
         while (endRange != endIter)
         {
            char c     = *endRange;
            char cNext = 0;
            if (endRange != endMinusOneIter)
            {
               cNext = *(endRange + 1);
            }
            switch (parseState)
            {
            case cNORMAL_STATE:
               if (hasText)
               {
                  bool hitComment  = (c == '#') || (c == '/' && cNext == '*');
                  bool hitVariable = false;
                  if (mInsidePreprocessorVariable && c == '>' && cNext == '$')
                  {
                     hitVariable = true;
                  }
                  if (c == '$' && cNext == '<')
                  {
                     hitVariable = *(endRange - 1) != '$';
                  }
                  if (hitComment || WsfParseUtil::IsWhitespace(c) || hitVariable)
                  {
                     ++mWordsRead;
                     currentSource.mPos = endRange;
                     --endRange;
                     UtTextDocumentRange range(sourcePtr, UtTextRange(startRange, endRange));
                     PushToUndoStack(range);
                     return range;
                  }
                  ++endRange;
               }
               else
               {
                  if (!WsfParseUtil::IsWhitespace(c))
                  {
                     if (c == '#' || (c == '/' && cNext == '/'))
                     {
                        commentStart = endRange.GetIndex();
                        parseState   = cLINE_COMMENT_STATE;
                     }
                     else if (c == '/' && cNext == '*')
                     {
                        commentStart = endRange.GetIndex();
                        parseState   = cBLOCK_COMMENT_STATE;
                     }
                     else if (c == '$' && cNext == '<' && (endRange.GetIndex() == 0 || *(endRange - 1) != '$'))
                     {
                        commentStart = endRange.GetIndex();
                        parseState   = cVARIABLE_NAME;
                        ++endRange;
                        ++endRange;
                     }
                     else if (c == '>' && cNext == '$' && mInsidePreprocessorVariable)
                     {
                        // end variable
                        mCurrentSourcePtr->AddPreprocessorEntry(WsfParseSourceInclude::cPREPROCESSOR_VARIABLE,
                                                                UtTextRange(endRange.GetIndex(), endRange.GetIndex() + 1));
                        ++endRange;
                        ++endRange;
                        mInsidePreprocessorVariable = false;
                     }
                     else if (c == '$' && cNext == 'd' && (endRange.GetIndex() == 0 || *(endRange - 1) == '\n'))
                     {
                        bool        matches = true;
                        const char* str     = "$define";
                        for (UtTextDocument::iterator tmpIter = endRange; tmpIter != endMinusOneIter && *str != 0;
                             ++tmpIter, ++str)
                        {
                           if (*tmpIter != *str)
                           {
                              matches = false;
                              break;
                           }
                        }
                        if (matches)
                        {
                           commentStart = endRange.GetIndex();
                           parseState   = cDEFINE_STATE;
                        }
                        else
                        {
                           startRange = endRange;
                           ++endRange;
                           hasText = true;
                        }
                     }
                     else
                     {
                        startRange = endRange;
                        ++endRange;
                        hasText = true;
                     }
                  }
                  else
                  {
                     ++endRange;
                  }
               }
               break;
            case cLINE_COMMENT_STATE:
               if (c == '\n')
               {
                  parseState = cNORMAL_STATE;
                  hasText    = false;
                  mCurrentSourcePtr->AddComment(UtTextRange(commentStart, endRange.GetIndex()), true);
               }
               else
               {
                  ++endRange;
               }
               break;
            case cBLOCK_COMMENT_STATE:
               if (c == '*' && cNext == '/')
               {
                  parseState = cNORMAL_STATE;
                  hasText    = false;
                  ++endRange;
                  mCurrentSourcePtr->AddComment(UtTextRange(commentStart, endRange.GetIndex()), false);
               }
               ++endRange;
               break;
            case cDEFINE_STATE:
               if (c == '\\' && cNext == '\n')
               {
                  ++endRange;
                  ++endRange;
               }
               else if (c == '\n')
               {
                  parseState = cNORMAL_STATE;
                  hasText    = false;
                  mCurrentSourcePtr->AddPreprocessorEntry(WsfParseSourceInclude::cPREPROCESSOR_DEFINE,
                                                          UtTextRange(commentStart, endRange.GetIndex()));
               }
               else
               {
                  ++endRange;
                  if (endRange == endIter)
                  {
                     mCurrentSourcePtr->AddPreprocessorEntry(WsfParseSourceInclude::cPREPROCESSOR_DEFINE,
                                                             UtTextRange(commentStart, endRange.GetIndex()));
                  }
               }
               break;
            case cVARIABLE_NAME:
               if (c == ':')
               {
                  mInsidePreprocessorVariable = true;
                  mCurrentSourcePtr->AddPreprocessorEntry(WsfParseSourceInclude::cPREPROCESSOR_VARIABLE,
                                                          UtTextRange(commentStart, endRange.GetIndex()));
                  ++endRange;
                  parseState = cNORMAL_STATE;
               }
               else if (c == '>' && cNext == '$')
               {
                  ++endRange;
                  mCurrentSourcePtr->AddPreprocessorEntry(WsfParseSourceInclude::cPREPROCESSOR_VARIABLE,
                                                          UtTextRange(commentStart, endRange.GetIndex()));
                  mInsidePreprocessorVariable = false;
                  ++endRange;
                  parseState = cNORMAL_STATE;
               }
               else
               {
                  ++endRange;
               }
               break;
            default:
               break;
            }
         }
         if (parseState == cNORMAL_STATE && hasText)
         {
            --endRange;
            UtTextDocumentRange range(sourcePtr, UtTextRange(startRange, endRange));
            PushToUndoStack(range);
            PopSource();
            ++mWordsRead;
            return range;
         }
         else if (parseState == cBLOCK_COMMENT_STATE || parseState == cLINE_COMMENT_STATE)
         {
            mCurrentSourcePtr->AddComment(UtTextRange(commentStart, endRange.GetIndex()),
                                          parseState == cLINE_COMMENT_STATE);
            PopSource();
         }
         else
         {
            PopSource();
         }
      }
      if (!mDeferredSources.empty())
      {
         PushSource(mDeferredSources.front().GetSystemPath(), false, nullptr, false);
         mDeferredSources.pop_front();
         moreSource = true;
      }
   } while (moreSource);
   ++mWordsRead;
   PushToUndoStack(UtTextDocumentRange());
   return UtTextDocumentRange();
}

//! Undo a previous call to ReadWord().
//! The number of words allowed to undone is limited to cRECALL_SIZE.
void WsfParser::UndoRead(size_t aWordCount)
{
   assert(mWordsRead >= aWordCount);
   mRecallPosition = mRecallPosition + aWordCount;
   mWordsRead -= aWordCount;
}
UtTextDocumentRange WsfParser::UndoReadAndGetRange(size_t aWordCount)
{
   UtTextDocumentRange range = mRecall[0];
   UndoRead(aWordCount);
   return range;
}

//! Reset the parser back to its original state.
void WsfParser::Reset()
{
   mCurrentSequenceStack.resize(1);
   mCurrentSequenceStack[0] = nullptr;
   mWordsRead               = 0;
   mSourceStack.clear();
   mRecall.Clear();
   mRecallPosition = -1;
   // mPartialMatches.clear();
   mParseData->Clear();
   mCurrentSourcePtr = nullptr;
   if (mRootSourcePtr)
   {
      delete mRootSourcePtr;
      mRootSourcePtr = nullptr;
   }
   mSourceIncludes.clear();
   mTerminatorList.clear();
   std::fill(mTerminatorPrefixCheck, mTerminatorPrefixCheck + 256, 0u);
   mSharedPtr->mDelayLoaders.clear();
   mSharedPtr->ClearErrors();
   mSharedPtr->mCompletedDelayLoaders.clear();
   mSharedPtr->mLoadingDelayLoaders.clear();
   mSharedPtr->mDelayLoadOrdering.clear();

   DeleteAllNodes();
}

void WsfParser::ProcessSpecialNode(WsfParseNode* aNodePtr)
{
   WsfParseNode* childPtr = aNodePtr->GetChild(0);
   if (childPtr != nullptr)
   {
      if (childPtr->mValue == "define_path_variable")
      {
         mParseData->DefinePathVariable(aNodePtr->GetChild(1)->mValue.Text(), aNodePtr->GetChild(2)->mValue.Text());
      }
      else if (childPtr->mValue == "undefine_path_variable")
      {
         mParseData->UndefinePathVariable(aNodePtr->GetChild(1)->mValue.Text());
      }
      else if (childPtr->mValue == "reset_file_path")
      {
         mParseData->ResetFilePath();
      }
      else if (childPtr->mValue == "file_path")
      {
         // Get the value of the file_path command
         WsfParseNode* nodePtr       = aNodePtr->GetChild(1);
         std::string   filePathValue = nodePtr->mValue.Text();
         UtStringUtil::Trim(filePathValue, "\""); // Remove any quotes

         // Get Path to the current file
         UtPath sourceFilePath = nodePtr->mValue.mSource->GetFilePath();
         sourceFilePath.Up(); // strip off file name

         // Construct the absolute path
         UtPath path(filePathValue);
         path.MakeFullPath(sourceFilePath);
         mParseData->AddFilePath(path.GetNormalizedPath());
      }
   }
}

void WsfParser::AddError(WsfParseRule* aRule, UtTextDocumentRange& aRange)
{
   WsfParseError* errorPtr = new WsfParseError;
   errorPtr->mRange        = aRange;
   errorPtr->mReaderPtr    = aRule;
   if (!mPartialMatches.empty())
   {
      for (const auto& match : mPartialMatches)
      {
         if (match.mSource == errorPtr->mRange.mSource)
         {
            errorPtr->mRange.SetBegin(std::max(errorPtr->mRange.GetBegin(), match.GetEnd()));
         }
      }
      if (errorPtr->mRange.GetEnd() == errorPtr->mRange.GetBegin())
      {
         errorPtr->mRange.SetBegin(errorPtr->mRange.GetEnd() - 2);
      }
   }
   mSharedPtr->mErrors.push_back(errorPtr);
}

void WsfParser::PopSource()
{
   if (!mSourceStack.empty())
   {
      mSourceStack.pop_back();
      if (!mSourceStack.empty())
      {
         mCurrentSourcePtr = mSourceStack.back().mIncludePtr;
      }
   }
}

WsfParseSourceInclude* WsfParser::GetInclude(UtTextDocument* aParseSourcePtr)
{
   auto s = std::find_if(mSourceIncludes.rbegin(),
                         mSourceIncludes.rend(),
                         [aParseSourcePtr](std::pair<UtTextDocument*, WsfParseSourceInclude*> sourceInclude)
                         { return sourceInclude.first == aParseSourcePtr; });

   return (s != mSourceIncludes.rend()) ? s->second : nullptr;
}

void WsfParser::SetDefinitions(WsfParseDefinitions* aDefinitionsPtr)
{
   delete mDefinitions;
   mDefinitions = aDefinitionsPtr;
   delete mParseData;
   mParseData = new WsfParseIndex(mDefinitions);
}

void WsfParser::SwapDefinitions(WsfParseDefinitions*& aDefinitionsPtr)
{
   std::swap(aDefinitionsPtr, mDefinitions);
   delete mParseData;
   mParseData = new WsfParseIndex(mDefinitions);
}

void WsfParser::PushDeferredSource(const UtPath& aFile)
{
   mDeferredSources.push_back(aFile);
}

void WsfParser::DelayLoad(const std::string& aKind,
                          const std::string& aName,
                          WsfParseNode*      aNodePtr,
                          ptrdiff_t          aTextOffset,
                          WsfParseRule*      aRulePtr)
{
   DelayLoader loader;
   loader.mNodePtr    = aNodePtr;
   loader.mRulePtr    = aRulePtr;
   loader.mIncludePtr = mCurrentSourcePtr;
   loader.mTextOffset = aTextOffset;
   DelayLoadId key(aKind, aName);
   auto        i = mSharedPtr->mDelayLoaders.upper_bound(key);
   // If this is the first delay loader with this key, Add it to the ordered list of delay loaders
   if (i == mSharedPtr->mDelayLoaders.end() || i->first != key)
   {
      mSharedPtr->mDelayLoadOrdering.push_back(key);
   }
   mSharedPtr->mDelayLoaders.insert(i, make_pair(key, loader));
}

bool WsfParser::Requires(const std::string& aKind, const std::string& aName)
{
   DelayLoadId id(aKind, aName);
   if (mSharedPtr->mCompletedDelayLoaders.find(id) != mSharedPtr->mCompletedDelayLoaders.end())
   {
      return true;
   }

   if (mSharedPtr->mLoadingDelayLoaders.find(id) != mSharedPtr->mLoadingDelayLoaders.end())
   {
      return false;
   }

   mSharedPtr->mLoadingDelayLoaders.insert(id);

   std::vector<DelayLoader> allLoaders;
   {
      auto i = mSharedPtr->mDelayLoaders.lower_bound(id);
      while (i != mSharedPtr->mDelayLoaders.end() && i->first == id)
      {
         allLoaders.push_back(i->second);
         ++i;
      }
   }
   if (allLoaders.empty())
   {
      return false;
   }
   else
   {
      for (const DelayLoader& loader : allLoaders)
      {
         WsfParser newParser(*this);
         newParser.mCurrentSourcePtr = loader.mIncludePtr;

         SourceData dat;
         dat.mSource     = loader.mIncludePtr->mSourcePtr;
         dat.mPos        = dat.mSource->Begin() + loader.mTextOffset;
         dat.mIncludePtr = loader.mIncludePtr;
         newParser.mSourceStack.push_back(dat);
         WsfParseNode* resultNode = nullptr;
         if (loader.mRulePtr->Read(newParser, resultNode) && resultNode)
         {
            loader.mNodePtr->Replace(resultNode);
            // Mark the node as delay loaded
            WsfParseDelayLoadData* delayLoadOrdering = new WsfParseDelayLoadData;
            AddAuxiliaryValue(delayLoadOrdering);
            delayLoadOrdering->mLoadOrder = mSharedPtr->mDelayLoadingCount++;
            resultNode->SetAuxiliaryValue(delayLoadOrdering);
            resultNode->SetFlags(resultNode->GetFlags() | WsfParseNode::cDELAY_LOAD);
         }
      }

      mSharedPtr->mLoadingDelayLoaders.erase(id);
      mSharedPtr->mCompletedDelayLoaders.insert(id);
      return true;
   }
}

void WsfParser::ResolveDelayLoad()
{
   // Visit the delay loaded objects in order they were found.
   for (const DelayLoadId& id : mSharedPtr->mDelayLoadOrdering)
   {
      Requires(id.first, id.second);
   }
}

WsfParseNode* WsfParser::ParseFiles(const std::vector<UtPath>& aFileList, int aMaxErrors)
{
   Reset();
   for (size_t i = 1; i < aFileList.size(); ++i)
   {
      PushDeferredSource(aFileList[i]);
   }

   // UtTextDocument* firstFilePtr = 0;
   if (!aFileList.empty())
   {
      /*firstFilePtr = */ PushSource(aFileList[0].GetSystemPath(), true);
   }
   mRootSourcePtr = GetCurrentInclude();

   // WsfParseSourceInclude* rootIncludePtr = GetCurrentInclude();
   WsfParseNode* rootNodePtr   = mSharedPtr->NewNode(nullptr, "root-commands");
   WsfParseRule* rootReaderPtr = GetDefinitions()->GetRoot();
   // Create a stub node for quick insertions to the linked list of siblings
   WsfParseNode* nextNodePtr = mSharedPtr->NewNode();
   rootNodePtr->Add(nextNodePtr);

   for (;;)
   {
      UtTextDocumentRange token = ReadWord();
      if (token.Valid())
      {
         UndoRead();
         WsfParseNode* commandTreePtr = nullptr;
         if (rootReaderPtr->Read(*this, commandTreePtr) && commandTreePtr != nullptr)
         {
            nextNodePtr->InsertAfter(commandTreePtr);
            nextNodePtr = commandTreePtr;
         }
         else
         {
            UtTextDocumentRange range = ReadWord();
            if (GetErrors().size() < ut::cast_to_size_t(aMaxErrors))
            {
               AddError(rootReaderPtr, range);
            }
         }
      }
      else
      {
         break;
      }
   }

   ResolveDelayLoad();
   return rootNodePtr;
}

//! Numbers parse nodes and creates the list of file transition nodes.
void WsfParser::FinalizeParseTree(WsfParseNode* aRootNodePtr, std::vector<WsfParseNode*>& aFileTransitionNodes)
{
   // Find file transition nodes and record node indices
   UtTextDocument* currentDoc = nullptr;
   size_t          nodeIndex  = 0;
   WsfParseNode*   n          = aRootNodePtr;
   while (n)
   {
      n->SetLexicalIndex(nodeIndex++);
      if (n->mValue.Valid())
      {
         UtTextDocument* nextDoc = n->mValue.mSource;
         if (nextDoc && currentDoc != nextDoc)
         {
            aFileTransitionNodes.push_back(n);
            currentDoc = nextDoc;
         }
      }
      n = n->Next();
   }
}


WsfParser::Shared::~Shared()
{
   ClearErrors();
}

void WsfParser::Shared::ClearErrors()
{
   for (auto& error : mErrors)
   {
      delete error;
   }
   mErrors.clear();
}
