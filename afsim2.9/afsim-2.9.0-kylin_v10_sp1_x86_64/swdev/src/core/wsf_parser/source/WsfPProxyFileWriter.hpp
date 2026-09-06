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

#ifndef WSFPPROXYFILEWRITER_HPP
#define WSFPPROXYFILEWRITER_HPP

#include <map>
#include <memory>
#include <string>

#include "UtTextDocument.hpp"
#include "WsfPProxyPath.hpp"
class WsfParseNode;
class WsfPParseRuleTrace;
class WsfParseSourceProvider;

struct WsfPProxyFileWriterChange
{
   UtTextDocumentRange mReplaceRange;
   std::string         mReplaceText;
   // Update a range of text after applying this change
   void ApplyChangeToRange(UtTextRange& aRange) const
   {
      if (mReplaceRange.GetBegin() > aRange.GetBegin())
      {
         return;
      }
      size_t delta = mReplaceText.size() - mReplaceRange.Length();
      aRange.SetRange(aRange.GetBegin() + delta, aRange.GetEnd() + delta);
   }
};

class WsfPProxyFileWriter
{
public:
   WsfPProxyFileWriter();
   WsfPProxyFileWriter(const WsfPProxyFileWriter&) = delete;
   WsfPProxyFileWriter& operator=(const WsfPProxyFileWriter&) = delete;
   WsfPProxyFileWriter(WsfPProxyFileWriter&&)                 = default;
   WsfPProxyFileWriter& operator=(WsfPProxyFileWriter&&) = default;
   ~WsfPProxyFileWriter()                                = default;

   void SetRootFile(UtTextDocument* aFilePtr);
   void AddChange(WsfPProxyFileWriterChange c);

   void ApplyChanges();
   void MoveCursor(UtTextDocumentLocation aLocation);
   void InsertText(const std::string& aText);
   void SetSourceProvider(WsfParseSourceProvider* aSourceProvider);
   void SetSaveFiles(const std::map<WsfPProxyPath, std::string>& aProxySaveLocations)
   {
      mProxySaveLocations = aProxySaveLocations;
   }
   void WriteTrace(WsfPParseRuleTrace* aTracePtr);
   void SetIncludeDirectories(const UtPath& aWorkingDirectory, const std::vector<UtStringRef>& aIncludeDirectories);
   const std::vector<UtTextDocumentRange>& GetNewRanges() { return mNewRanges; }

   const std::map<UtTextDocument*, std::unique_ptr<UtTextDocument>>& GetDocumentCopies() const
   {
      return mDocumentCopies;
   }
   using ChangeList = std::vector<WsfPProxyFileWriterChange>;
   using ChangeMap  = std::map<UtTextDocument*, ChangeList>;

private:
   std::string     BestIncludePath(const UtPath& aFilePath);
   void            UpdateSaveLocations(WsfPParseRuleTrace* aTracePtr);
   void            WriteTraceP(WsfParseNode* aNodePtr);
   void            WriteTraceP(WsfPParseRuleTrace* aTracePtr);
   void            ExpandCommandRangeForWhitespace(UtTextDocumentRange& aRange, UtTextDocument* aDocPtr);
   UtTextDocument* GetWritableDocument(UtTextDocument* aDocument);
   void            MoveCursorToFile(const std::string& aFileName);

   // Changes not associated with a file get added here
   UtTextDocument*                                            mRootFilePtr;
   ChangeMap                                                  mChanges;
   UtTextDocumentLocation                                     mCursor;
   std::vector<UtTextDocumentRange>                           mNewRanges;
   std::map<UtTextDocument*, std::unique_ptr<UtTextDocument>> mDocumentCopies;
   // for new constructs, the file may be selected
   std::map<WsfPParseRuleTrace*, std::string> mSaveFiles;
   std::map<WsfPProxyPath, std::string>       mProxySaveLocations;
   WsfParseSourceProvider*                    mSourceProvider;

   UtPath                   mWorkingDirectory;
   std::vector<std::string> mNewFileList;
   std::vector<UtStringRef> mIncludeDirectories;
};

#endif
