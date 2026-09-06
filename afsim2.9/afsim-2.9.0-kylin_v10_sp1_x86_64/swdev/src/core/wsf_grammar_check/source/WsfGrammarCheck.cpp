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

#include "WsfGrammarCheck.hpp"

#include <cassert>
#include <fstream>
#include <sstream>

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfGrammarInterface.hpp"
#include "WsfParseDefinitions.hpp"
#include "WsfParseNode.hpp"
#include "WsfParseSourceInclude.hpp"
#include "WsfParseUtil.hpp"
#include "WsfParser.hpp"
#include "WsfScenario.hpp"

//
// An extension which checks that the grammar ( .ag files ) result in a parser that
// can parse the scenario.  Parse errors are written to the console.
//
void WSF_GRAMMAR_CHECK_EXPORT Register_wsf_grammar_check(WsfApplication& aApplication)
{
   aApplication.RegisterExtension("wsf_grammar_check",
                                  ut::make_unique<WsfDefaultApplicationExtension<WsfGrammarCheckExtension>>());
}

namespace
{
size_t GetLineNumber(UtTextDocumentRange& aRange)
{
   size_t line = 1;
   for (size_t i = 0; i < aRange.GetBegin(); ++i)
   {
      if ((*aRange.mSource)[i] == '\n')
      {
         ++line;
      }
   }
   return line;
}
} // namespace

// =================================================================================================
WsfGrammarCheckExtension::WsfGrammarCheckExtension()
   : mFileList()
   , mParserPtr(nullptr)
   , mSourceProviderPtr(nullptr)
   , mGrammarLoadFailed(false)
{
   WsfParseUtil::InitializeParseUtil();
}

// =================================================================================================
WsfGrammarCheckExtension::~WsfGrammarCheckExtension()
{
   delete mParserPtr;
   delete mSourceProviderPtr;
}

// =================================================================================================
void WsfGrammarCheckExtension::FileLoaded(const std::string& aFileName)
{
   // Note: WSF does not rely on the grammar or the wsf_parser library to parse input.
   //       This check is done to ensure the developer has added the grammar necessary
   //       to parse any new UtInput constructs.
   //
   // Skip this test if release build is specified

   if (GetScenario().GetApplication().IsTestingEnabled())
   {
      const std::string& grammar = WsfGrammarExtension::Find(GetScenario().GetApplication())->GetGrammarText();
      if (!grammar.empty()) // Only run if  grammar exists
      {
         // First time initialization
         if ((mParserPtr == nullptr) && (!mGrammarLoadFailed))
         {
            std::stringstream ss(grammar);
            InitializeGrammar(ss);
            mGrammarLoadFailed = (mParserPtr == nullptr);
         }
         if (mParserPtr != nullptr)
         {
            int errorCount = FileLoad(grammar, aFileName);
            if (errorCount > 0 && !GetScenario().ExpectInputError())
            {
               auto out = ut::log::info("test_fail") << "-FAIL- Grammar does not match input.";
               out.AddNote() << "Error count: " << errorCount;
            }
         }
      }
   }
}

// =================================================================================================
void WsfGrammarCheckExtension::InitializeGrammar(std::istream& aGrammarText)
{
   WsfParseDefinitions* defsPtr = new WsfParseDefinitions();
   defsPtr->AddGrammar("", aGrammarText);
   const std::list<std::string>& errors = defsPtr->GetDefinitionErrors();
   bool                          ok     = errors.empty();
   if (ok)
   {
      if (!defsPtr->Initialize())
      {
         ok = false;
         ut::log::info("test_fail") << "-FAIL- Failed to initialize WSF grammar.";
      }
   }
   if (!errors.empty())
   {
      ok       = false;
      auto out = ut::log::info("test_fail") << "-FAIL- Errors loading WSF grammar:";
      for (auto i = errors.begin(); i != errors.end(); ++i)
      {
         const std::string& error(*i);
         if (!error.empty())
         {
            out.AddNote() << "-FAIL- " << error;
         }
      }
   }
   if (ok)
   {
      mSourceProviderPtr = new ParseSourceProvider();
      mParserPtr         = new WsfParser(mSourceProviderPtr, UtPath::WorkingDirectory().GetSystemPath(), defsPtr);
   }
   else
   {
      delete defsPtr;
   }
}

// =================================================================================================
int WsfGrammarCheckExtension::FileLoad(const std::string& aGrammarText, const std::string& aFileName)
{
   if (mParserPtr == nullptr)
   {
      return 0;
   }

   mFileList.emplace_back(aFileName);
   mParserPtr->ParseFiles(mFileList, 10);
   // The parser collects errors internal to accepted blocks.
   // Top-level commands that are not parsed need to be collected manually here.
   std::vector<UtTextDocumentRange> errorList;
   WsfParseNode*                    rootNodePtr = mParserPtr->NewNode();
   while (true)
   {
      WsfParseNode* treePtr = nullptr;
      bool          ok      = mParserPtr->GetDefinitions()->GetRoot()->Read(*mParserPtr, treePtr);
      if (!ok)
      {
         UtTextDocumentRange word = mParserPtr->ReadWord();
         if (word.Valid())
         {
            errorList.push_back(word);
         }
         else
         {
            break;
         }
      }
      else if (treePtr != nullptr)
      {
         rootNodePtr->Add(treePtr);
      }
   }

   const std::vector<WsfParseError*>& errors   = mParserPtr->GetErrors();
   size_t                             errorCt  = errors.size() + errorList.size();
   const size_t                       cMAX_ERR = 2;
   for (size_t i = 0; i < errors.size() && i < cMAX_ERR; ++i)
   {
      UtTextDocumentRange range = errors[i]->mRange;
      size_t              line  = GetLineNumber(range);
      auto                out   = ut::log::error() << "WSF PARSE ERROR:";
      out.AddNote() << "Location: " << range.mSource->GetFilePath().GetSystemPath() << "(" << line << ")";
      out.AddNote() << "Range: " << range.Text();
   }
   for (size_t i = 0; i < errorList.size() && i < cMAX_ERR; ++i)
   {
      UtTextDocumentRange range = errorList[i];
      size_t              line  = GetLineNumber(range);
      auto                out   = ut::log::error() << "WSF PARSE ERROR:";
      out.AddNote() << "Location: " << range.mSource->GetFilePath().GetSystemPath() << "(" << line << ")";
      out.AddNote() << "Range: " << range.Text();
   }
   return (int)errorCt;
}

// =================================================================================================
ParseSourceProvider::~ParseSourceProvider()
{
   for (auto it = mSourcePtrs.begin(); it != mSourcePtrs.end(); ++it)
   {
      delete *it;
   }
   mSourcePtrs.clear();
}

UtTextDocument* ParseSourceProvider::FindSource(const UtPath& aPath, bool aReadAccess)
{
   if (aPath.Stat() == UtPath::cFILE)
   {
      if (aReadAccess)
      {
         UtTextDocument* sourcePtr = new UtTextDocument(aPath.GetSystemPath());
         mSourcePtrs.push_back(sourcePtr);
         return sourcePtr;
      }
      else
      {
         return nullptr;
      }
   }
   return nullptr;
}
