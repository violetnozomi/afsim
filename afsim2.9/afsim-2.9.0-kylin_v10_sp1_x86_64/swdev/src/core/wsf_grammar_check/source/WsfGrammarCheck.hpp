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

#ifndef WSFGRAMMARCHECK_HPP
#define WSFGRAMMARCHECK_HPP

#include "wsf_grammar_check_export.h"

#include <iosfwd>
#include <vector>

#include "UtPath.hpp"
#include "WsfParseSourceProvider.hpp"
#include "WsfScenarioExtension.hpp"

class ParseSourceProvider;
class WsfParser;

class WSF_GRAMMAR_CHECK_EXPORT WsfGrammarCheckExtension : public WsfScenarioExtension
{
public:
   WsfGrammarCheckExtension();
   ~WsfGrammarCheckExtension() override;
   void FileLoaded(const std::string& aFileName) override;

private:
   void InitializeGrammar(std::istream& aGrammarText);

   int FileLoad(const std::string& aGrammarText, const std::string& aFileName);

   std::vector<UtPath>  mFileList;
   WsfParser*           mParserPtr;
   ParseSourceProvider* mSourceProviderPtr;
   bool                 mGrammarLoadFailed;
};

class ParseSourceProvider : public WsfParseSourceProvider
{
public:
   // Keeps track of the 'newed' UtTextDocuments so they can be cleaned up
   std::vector<UtTextDocument*> mSourcePtrs;

   ~ParseSourceProvider() override;

   UtTextDocument* FindSource(const UtPath& aPath, bool aReadAccess) override;
};

#endif
