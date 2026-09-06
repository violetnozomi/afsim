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

#include <fstream>
#include <iostream>

#include "UtScanDir.hpp"
#include "UtUnitTypes.hpp"
#include "WsfPProxyDeserialize.hpp"
#include "WsfPProxyDiff.hpp"
#include "WsfPProxyRegistry.hpp"
#include "WsfPProxySerialize.hpp"
#include "WsfParseSourceProvider.hpp"
#include "WsfParser.hpp"

namespace
{
class ParseSourceProvider : public WsfParseSourceProvider
{
   std::map<UtPath, UtTextDocument*> mCache;
   //! Derived classes should override this method to return
   //! a source if it exists at aPath
   virtual UtTextDocument* FindSource(const UtPath& aPath, bool aReadAccess)
   {
      if (mCache.find(aPath) != mCache.end())
      {
         return mCache[aPath];
      }

      if (aPath.Stat() == UtPath::cFILE)
      {
         return mCache[aPath] = new UtTextDocument(aPath.GetSystemPath());
      }
      return 0;
   }
};

int GetLineNumber(UtTextDocumentRange& aRange)
{
   int line = 1;
   for (int i = 0; i < aRange.mBegin; ++i)
   {
      if ((*aRange.mSource)[i] == '\n')
      {
         ++line;
      }
   }
   return line;
}

} // namespace

struct ParserTest
{
   ParseSourceProvider* mSourceProviderPtr;
   WsfParser*           mParserPtr;
   WsfPProxyRegistry*   mProxyRegistry;
   bool                 mPrintOutput;
   bool                 mDumpProxy;
   std::ofstream        mOutputStream;

   ParserTest()
      : mSourceProviderPtr(nullptr)
      , mParserPtr(nullptr)
      , mProxyRegistry(nullptr)
      , mPrintOutput(false)
      , mDumpProxy(false)
      , mOutputStream()
   {
   }

   bool InitParser(const std::string& aGrammarFilesLocation, const std::string& aWorkingDirectory)
   {
      mSourceProviderPtr         = new ParseSourceProvider;
      WsfParseDefinitions*  defs = new WsfParseDefinitions;
      UtScanDir::DirEntries entries;
      int                   found = UtScanDir::ScanDir(aGrammarFilesLocation, entries, "*.ag");
      for (size_t i = 0; i < entries.size(); ++i)
      {
         if (!entries[i].mDirectory)
         {
            UtPath path(aGrammarFilesLocation);
            if (entries[i].mFilename != "script_data.ag")
            {
               path += entries[i].mFilename;
               ifstream infile(path.GetSystemPath().c_str());
               defs->AddGrammar(entries[i].mFilename, infile);
            }
         }
      }

      const std::list<std::string>& errors = defs->GetDefinitionErrors();
      bool                          ok     = errors.empty();
      if (ok)
      {
         ok = defs->Initialize();
      }
      ok = ok && errors.empty();
      if (!ok)
      {
         std::cout << "***** ERROR: Encountered problems parsing WSF grammar:\n";
         for (std::list<std::string>::const_iterator i = errors.begin(); i != errors.end(); ++i)
         {
            std::cout << "\t" << *i << '\n';
         }
         return false;
      }

      mParserPtr = new WsfParser(mSourceProviderPtr, aWorkingDirectory);
      mParserPtr->SetDefinitions(defs);


      mProxyRegistry = new WsfPProxyRegistry;
      mProxyRegistry->Setup(defs->GetRootStruct(), defs->mBasicTypes);
      return true;
   }

   bool ProxyTest(UtPath aTestFile)
   {
      UtPath ppFile, clippedFile, lisFile;
      assert(aTestFile.IsFile());
      // First, process the input test making a new file
      {
         std::string fn = aTestFile.GetFileName(true);
         ppFile         = aTestFile + ".." + (fn + ".full");
         clippedFile    = aTestFile + ".." + (fn + ".clip");
         lisFile        = aTestFile + ".." + (fn + ".lis");
         mOutputStream.close();
         mOutputStream.open(lisFile.GetSystemPath().c_str());
         ifstream    is(aTestFile.GetSystemPath().c_str());
         std::string textWithExtras, textWithoutExtras;
         char        line[1000];
         while (is.getline(line, 1000))
         {
            if (line[0] == '+')
            {
               textWithExtras += line + 1;
               textWithExtras += '\n';
            }
            else if (line[0] == '-')
            {
               textWithoutExtras += line + 1;
               textWithoutExtras += '\n';
            }
            else
            {
               textWithExtras += line;
               textWithoutExtras += line;
               textWithExtras += '\n';
               textWithoutExtras += '\n';
            }
         }
         ofstream os(ppFile.GetSystemPath().c_str());
         os.write(textWithExtras.c_str(), textWithExtras.size());
         ofstream clipOs(clippedFile.GetSystemPath().c_str());
         clipOs.write(textWithoutExtras.c_str(), textWithoutExtras.size());
      }

      // Parse and deserialize the processed file
      WsfPProxyValue desiredProxy;
      {
         WsfPProxyIndex*      proxyIndex = new WsfPProxyIndex;
         WsfPProxyDeserialize des(mProxyRegistry, proxyIndex);
         mParserPtr->Reset();
         std::vector<UtPath> fileList;
         fileList.push_back(ppFile);
         WsfParseNode* treePtr = mParserPtr->ParseFiles(fileList, 0);
         desiredProxy          = mProxyRegistry->GetType("root")->ConstructNew();
         WsfPProxyUndoValue desiredUndo(desiredProxy);
         desiredUndo.DisableUndo();
         des.Deserialize(desiredUndo, treePtr);
         //          if (mDumpProxy)
         //          {
         //             std::cout << "\n\n --------- DESIRED PROXY --------- \n\n";
         //             mProxyRegistry->OutputJSON(desiredProxy, desiredProxy, std::cout, 0);
         //             //WsfPProxyRegistry::cHIDE_INHERITED_VALUES);
         //          }
      }

      // Do the serialization
      WsfPProxySerialize  serializer;
      std::vector<UtPath> serializeInputFiles;
      serializeInputFiles.push_back(clippedFile);
      {
         serializer.Initialize(mProxyRegistry, mParserPtr, serializeInputFiles);
         serializer.Serialize(desiredProxy);
         serializer.Finalize();
      }
      bool pass = false;
      // Finally, deserialize again
      WsfPProxyValue     finalProxy = mProxyRegistry->GetType("root")->ConstructNew();
      WsfPProxyUndoValue finalUndo(finalProxy);
      {
         WsfParseNode* treePtr = mParserPtr->ParseFiles(serializeInputFiles, 10);

         finalUndo.DisableUndo();
         WsfPProxyDeserialize des(mProxyRegistry, 0);
         des.Deserialize(finalUndo, treePtr);
         WsfPProxyDiff diff;
         WsfPProxyPath emptyPath;
         diff.Diff(finalProxy, desiredProxy, emptyPath);
         if (diff.mDiffs.empty() && mParserPtr->GetErrors().empty())
         {
            pass = true;
            mOutputStream << "\n--PASS-- " << aTestFile.GetFileName() << '\n';
         }
         else
         {
            mOutputStream << "\n--FAIL-- " << aTestFile.GetFileName() << '\n';
            cerr << "\n--FAIL-- " << aTestFile.GetFileName() << '\n';
            for (size_t i = 0; i < diff.mDiffs.size(); ++i)
            {
               WsfPProxyDiff::DiffEntry& e = diff.mDiffs[i];
               switch (e.mType)
               {
               case WsfPProxyDiff::cADDED:
                  mOutputStream << "Failed to add " << e.mPath.ToString(desiredProxy) << '\n';
                  break;
               case WsfPProxyDiff::cCHANGED:
                  mOutputStream << "Failed to set " << e.mPath.ToString(desiredProxy) << '\n';
                  break;
               case WsfPProxyDiff::cREMOVED:
                  mOutputStream << "Failed to remove " << e.mPath.ToString(finalProxy) << '\n';
                  break;
               }
            }
            const std::vector<WsfParseError*>& errors = mParserPtr->GetErrors();
            for (size_t i = 0; i < errors.size(); ++i)
            {
               if (errors[i]->mRange.mSource)
               {
                  int line, col;
                  errors[i]->mRange.mSource->PositionToLineColumn(errors[i]->mRange.mBegin, line, col);
                  mOutputStream << "Parse Error in file output at Line " << line << " col " << col << '\n';
               }
            }
         }
      }

      UtTextDocument* doc = mParserPtr->FindSource(clippedFile.GetSystemPath(), false);
      mOutputStream << "~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
      mOutputStream << doc->GetText().GetPointer();

      if (mDumpProxy)
      {
         mOutputStream << "\n\n --------- DESIRED PROXY --------- \n\n";
         mProxyRegistry->OutputJSON(desiredProxy, desiredProxy, mOutputStream, 0); // WsfPProxyRegistry::cHIDE_INHERITED_VALUES);
         mOutputStream << "\n\n --------- FINAL PROXY --------- \n\n";
         mProxyRegistry->OutputJSON(finalUndo.Get(), finalUndo.Get(), mOutputStream, 0); // WsfPProxyRegistry::cHIDE_INHERITED_VALUES);
         mOutputStream << "\n\n --------- BASIC VALUES --------- \n\n";
         mProxyRegistry->OutputJSON(mProxyRegistry->mBasicRoot,
                                    mProxyRegistry->mBasicRoot,
                                    mOutputStream,
                                    0); // WsfPProxyRegistry::cHIDE_INHERITED_VALUES);
      }

      return pass;
   }
};


// template<typename CONTAINER, typename COMPARE_FN>
// void UtMinimumCommonSubsequence(const CONTAINER& aLHS, const CONTAINER& aRHS, const COMPARE_FN& aCompare)
// {
//    UtMinimumCommonSubsequence_Detail<CONTAINER, COMPARE_FN> detail(aLHS, aRHS, aCompare);
//    detail.sub(0,0);
//    detail.backtrack();
//    int stop=5;
// }

// match head
// match tail

int main(int argc, char* argv[])
{
   UtPath     dataRoot(argv[1]);
   ParserTest parseTest;
   bool       ok = parseTest.InitParser((dataRoot + "../../../wsf_ide/data").GetSystemPath(), dataRoot.GetSystemPath());
   if (!ok)
   {
      return 1;
   }

   std::string testFile;

   std::string prevArg, arg;
   for (int i = 2; i < argc; ++i)
   {
      prevArg = arg;
      arg     = argv[i];
      if (prevArg == "--test")
      {
         testFile = arg;
      }
      else if (arg == "--print-proxy")
      {
         parseTest.mDumpProxy = true;
      }
   }
   int failCount = 0;
   int passCount = 0;
   if (!testFile.empty())
   {
      if (!parseTest.ProxyTest(dataRoot + testFile))
      {
         ++failCount;
      }
      else
      {
         ++passCount;
      }
   }
   else
   {
      UtScanDir::DirEntries entries;
      UtScanDir::ScanDir(dataRoot.GetSystemPath(), entries, "test*.txt");
      for (size_t i = 0; i < entries.size(); ++i)
      {
         std::cout << "Processing " << entries[i].mFilename << '\n';
         std::vector<UtPath> paths;
         if (!parseTest.ProxyTest(dataRoot + entries[i].mFilename))
         {
            ++failCount;
         }
         else
         {
            ++passCount;
         }
      }
   }
   std::cout << failCount << " Failures\n";
   std::cout << passCount << " Passes\n";
   return 0;
}
