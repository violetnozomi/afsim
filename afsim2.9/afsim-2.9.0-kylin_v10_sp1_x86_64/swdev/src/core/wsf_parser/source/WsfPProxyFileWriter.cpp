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

#include "WsfPProxyFileWriter.hpp"

#include "UtMemory.hpp"
#include "WsfPProxyRuleTrace.hpp"
#include "WsfParseNode.hpp"
#include "WsfParseSourceProvider.hpp"
#include "WsfParseUtil.hpp"

WsfPProxyFileWriter::WsfPProxyFileWriter()
   : mRootFilePtr(nullptr)
{
   mSourceProvider = nullptr;
}

void WsfPProxyFileWriter::AddChange(WsfPProxyFileWriterChange c)
{
   mChanges[c.mReplaceRange.mSource].push_back(c);
}

void WsfPProxyFileWriter::SetRootFile(UtTextDocument* aFilePtr)
{
   mRootFilePtr = aFilePtr;
   MoveCursor(UtTextDocumentLocation(mRootFilePtr, mRootFilePtr->Size() - 1));
}

UtTextDocument* WsfPProxyFileWriter::GetWritableDocument(UtTextDocument* aDocument)
{
   if (!aDocument)
   {
      aDocument = mRootFilePtr;
   }
   auto iter = mDocumentCopies.find(aDocument);
   if (iter != mDocumentCopies.end())
   {
      return iter->second.get();
   }
   else
   {
      mDocumentCopies[aDocument] = ut::make_unique<UtTextDocument>(*aDocument);
      return mDocumentCopies[aDocument].get();
   }
}

void WsfPProxyFileWriter::ApplyChanges()
{
   for (auto& change : mChanges)
   {
      std::vector<UtTextDocumentRange> newRanges;
      ChangeList&                      changes     = change.second;
      UtTextDocument*                  originalDoc = change.first;
      UtTextDocument*                  copyDoc     = GetWritableDocument(originalDoc);
      if (!copyDoc)
      {
         continue;
      }
      for (size_t j = 0; j < changes.size(); ++j)
      {
         WsfPProxyFileWriterChange& c = changes[j];
         if (c.mReplaceRange.Length() > 0)
         {
            ExpandCommandRangeForWhitespace(c.mReplaceRange, copyDoc);
            copyDoc->Erase(c.mReplaceRange.GetBegin(), c.mReplaceRange.Length());
         }
         copyDoc->Insert(c.mReplaceRange.GetBegin(), c.mReplaceText.c_str(), c.mReplaceText.size());
         for (size_t k = j + 1; k < changes.size(); ++k)
         {
            WsfPProxyFileWriterChange& ck = changes[k];
            c.ApplyChangeToRange(ck.mReplaceRange);
         }

         for (auto& newRange : newRanges)
         {
            c.ApplyChangeToRange(newRange);
         }
         // Store the range of the new text for later reformatting
         // Don't store removed text ranges
         UtTextDocumentRange newRange(c.mReplaceRange);
         newRange.SetEnd(newRange.GetBegin() + c.mReplaceText.length());
         if (newRange.GetEnd() != newRange.GetBegin())
         {
            newRanges.push_back(newRange);
         }
      }
      mNewRanges.insert(mNewRanges.end(), newRanges.begin(), newRanges.end());
      // std::cerr << copyDoc->GetPointer(0);
   }
}

void WsfPProxyFileWriter::MoveCursor(UtTextDocumentLocation aLocation)
{
   mCursor = aLocation;
}

void WsfPProxyFileWriter::MoveCursorToFile(const std::string& aFileName)
{
   UtTextDocument* docPtr = mSourceProvider->FindSource(UtPath(aFileName), true);

   if (!docPtr)
   {
      std::ofstream outfile(UtPath(aFileName).GetSystemPath().c_str(), std::ios::binary);
      outfile << "# File created by WSF\n";
      outfile.close();
      docPtr = mSourceProvider->FindSource(UtPath(aFileName), true);
      mNewFileList.push_back(aFileName);
   }

   if (docPtr)
   {
      size_t pos = docPtr->Size();
      // a document usually ends with a null character.  Insert before that
      if (pos > 0)
      {
         --pos;
      }
      mCursor = UtTextDocumentLocation(docPtr, pos);
   }
}

void WsfPProxyFileWriter::InsertText(const std::string& aText)
{
   WsfPProxyFileWriterChange c;
   c.mReplaceRange = UtTextDocumentRange(mCursor, 0);
   c.mReplaceText  = aText;
   AddChange(c);
   // mCursor.mPosition += aText.size();
}

void WsfPProxyFileWriter::WriteTraceP(WsfParseNode* aNodePtr)
{
   UtTextDocumentRange src = aNodePtr->mValue;
   if (src.Valid())
   {
      UtTextDocumentLocation loc = src.GetPosition();
      loc.mPosition += src.Length();
      MoveCursor(loc);
      //       aRange.mEnd = std::max(aRange.mEnd, loc.mPosition);
      //       aRange.mBegin = std::min(aRange.mBegin, loc.mPosition);
      //       assert(aRange.mSource == 0 || aRange.mSource == loc.mSource);     // Currently only work with one file
      //       aRange.mSource = loc.mSource;
   }
   WsfParseNode* c = aNodePtr->Down();
   while (c)
   {
      WriteTraceP(c);
      c = c->Right();
   }
}

void WsfPProxyFileWriter::WriteTrace(WsfPParseRuleTrace* aTracePtr)
{
   if (!mProxySaveLocations.empty())
   {
      UpdateSaveLocations(aTracePtr);
   }
   WriteTraceP(aTracePtr);

   // Make unique list of new files, and finalize them
   std::sort(mNewFileList.begin(), mNewFileList.end());
   mNewFileList.erase(std::unique(mNewFileList.begin(), mNewFileList.end()), mNewFileList.end());
   for (const std::string& newFilePath : mNewFileList)
   {
      // Make an 'include_once' command for that file
      UtTextDocument* rootFile = GetWritableDocument(mRootFilePtr);
      std::string     relPath  = BestIncludePath(newFilePath);
      if (!relPath.empty())
      {
         size_t      insertPos   = rootFile->GetText().Size() - 1;
         std::string includeLine = "\ninclude_once " + relPath;
         rootFile->Insert(insertPos, includeLine);
      }
      // Save the new file to disk so that our include system can find it
      // otherwise the parser won't see the file
      // UtTextDocument* newDocumentPtr = mSourceProvider->FindSource(UtPath(newFilePath), false);
      // if (newDocumentPtr)
      //{
      // std::ofstream outfile(newDocumentPtr->GetFilePath().GetSystemPath().c_str(), std::ios::binary);
      // newDocumentPtr->WriteFile(outfile);
      // outfile.close();
      //}
   }
}

void WsfPProxyFileWriter::WriteTraceP(WsfPParseRuleTrace* aTracePtr)
{
   if (aTracePtr->IsPruned())
   {
      WsfParseNode* pNode = aTracePtr->GetNearestNode();
      // TODO: Not sure this should happen, but it does
      if (!pNode)
      {
         return;
      }

      // TODO: We are deleting comments here;  May want to save comments
      WsfParseNode* lVal = pNode->GetLeftmostValue();
      if (lVal && lVal->mValue.Valid())
      {
         UtTextDocumentRange       r(lVal->mValue.mSource, pNode->SubtreeRange(lVal->mValue.mSource));
         WsfPProxyFileWriterChange c;
         c.mReplaceRange = r;
         AddChange(c);
      }
      return;
   }
   if (mSaveFiles.find(aTracePtr) != mSaveFiles.end())
   {
      MoveCursorToFile(mSaveFiles[aTracePtr]);
   }
   if (aTracePtr->HasInsertText())
   {
      WsfParseNode* pNode = aTracePtr->GetNearestNode();
      if (pNode)
      {
         WsfParseNode* lVal = pNode->GetLeftmostValue();
         if (lVal && lVal->mValue.Valid())
         {
            UtTextDocumentRange r(lVal->mValue.mSource, pNode->SubtreeRange(lVal->mValue.mSource));
            MoveCursor(UtTextDocumentLocation(r.mSource, r.GetBegin() + r.Length()));
            WsfPProxyFileWriterChange c;
            c.mReplaceRange = r;
            c.mReplaceText  = aTracePtr->GetInsertText();
            AddChange(c);
         }
      }
      else
      {
         InsertText(" " + aTracePtr->GetInsertText());
      }
      return;
   }
   if (aTracePtr->mNodePtr != nullptr && !aTracePtr->IsPruned())
   {
      // if (0 == (aTracePtr->mNodePtr->mFlags & WsfParseNode::cPRUNE_NODE))
      //{
      UtTextDocumentRange src = aTracePtr->mNodePtr->mValue;
      if (src.Valid())
      {
         UtTextDocumentLocation loc = src.GetPosition();
         loc.mPosition += src.Length();
         MoveCursor(loc);
      }
      //}
   }
   if (aTracePtr->mNodePtr == nullptr)
   {
      WsfParseRule* rulePtr = aTracePtr->ActualRule();
      if (rulePtr && rulePtr->Type() == WsfParseRule::cLITERAL)
      {
         WsfParseLiteral* lit = (WsfParseLiteral*)rulePtr;
         InsertText(std::string(" ") + lit->mText);
      }
   }
   const std::vector<WsfPParseRuleTrace*>& children = aTracePtr->Children();
   for (WsfPParseRuleTrace* c : children)
   {
      WriteTraceP(c);
   }
   // ODD BEHAVIOR:
   //       The trace may not be complete (TODO: Fix this)
   //       For now, if there are no children, trace the nodes to move the cursor to the correct position
   if (children.empty() && !aTracePtr->IsPruned() && aTracePtr->mNodePtr)
   {
      WsfParseNode* c = aTracePtr->mNodePtr->Down();
      while (c)
      {
         WriteTraceP(c);
         c = c->Right();
      }
      // TODO: Not sure how to deal with untraced nodes.
      //        Nodes internal to (Proxy) value nodes should be left alone
      //        but otherwise an untraced node _might_ be implicitly pruned due to the way the trace is built?
      //        if (range.mSource)
      //        {
      //           WsfPProxyFileWriterChange c;
      //           c.mReplaceRange = range;
      //           w.AddChange(c);
      //        }
   }
}

void WsfPProxyFileWriter::ExpandCommandRangeForWhitespace(UtTextDocumentRange& aRange, UtTextDocument* aDocPtr)
{
   size_t i = aRange.GetBegin() - 1;
   // try to remove whitespace before and after command on the same line

   bool startsAtLine = false;
   // scan backwards
   while (i > 0)
   {
      char c = (*aDocPtr)[i];
      if (!WsfParseUtil::IsWhitespace(c))
      {
         break;
      }
      if (c == '\n')
      {
         aRange.SetBegin(i + 1);
         startsAtLine = true;
         break;
      }
      --i;
   }

   // scan forward
   // don't erase newline if text remains before
   if (startsAtLine)
   {
      i = aRange.GetEnd() + 1;
      while (i + 1 < aDocPtr->Size())
      {
         char c = (*aDocPtr)[i];
         if (!WsfParseUtil::IsWhitespace(c))
         {
            break;
         }
         if (c == '\n')
         {
            aRange.SetEnd(i);
            break;
         }
         ++i;
      }
   }
}

void WsfPProxyFileWriter::UpdateSaveLocations(WsfPParseRuleTrace* aTracePtr)
{
   if (aTracePtr->mNodePtr && aTracePtr->mNodePtr->mValue.mSource)
   {
      return;
   } // can't change location if a node already exists

   if (aTracePtr->GetContext() &&
       (mProxySaveLocations.find(aTracePtr->GetContext()->GetAddr()) != mProxySaveLocations.end()))
   {
      std::string         fileName  = mProxySaveLocations[aTracePtr->GetContext()->GetAddr()];
      WsfPParseRuleTrace* rootTrace = aTracePtr->FindRootTrace();
      if (rootTrace)
      {
         if (nullptr == rootTrace->GetNearestNode()) // only proceed if no children have nodes
         {
            mSaveFiles[rootTrace] = fileName;
         }
      }
   }
   else
   {
      for (WsfPParseRuleTrace* child : aTracePtr->Children())
      {
         if (!child->IsPruned())
         {
            UpdateSaveLocations(child);
         }
      }
   }
}

void WsfPProxyFileWriter::SetSourceProvider(WsfParseSourceProvider* aSourceProvider)
{
   mSourceProvider = aSourceProvider;
}

void WsfPProxyFileWriter::SetIncludeDirectories(const UtPath&                   aWorkingDirectory,
                                                const std::vector<UtStringRef>& aIncludeDirectories)
{
   mIncludeDirectories = aIncludeDirectories;
   mWorkingDirectory   = aWorkingDirectory;
}

std::string WsfPProxyFileWriter::BestIncludePath(const UtPath& aFilePath)
{
   UtPath bestRelPath;
   for (size_t i = 0; i < mIncludeDirectories.size() + 1; ++i)
   {
      UtPath base;
      if (i == mIncludeDirectories.size())
      {
         base = mWorkingDirectory;
      }
      else
      {
         base = mWorkingDirectory + mIncludeDirectories[i].Get();
      }
      UtPath relPath = base.GetRelativePath(aFilePath);
      if (bestRelPath.GetNormalizedPath().empty() ||
          relPath.GetNormalizedPath().size() < bestRelPath.GetNormalizedPath().size())
      {
         bestRelPath = relPath;
      }
   }
   return bestRelPath.GetNormalizedPath();
}
