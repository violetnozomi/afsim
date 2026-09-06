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

#include "WsfParseDefinitions.hpp"
#include "WsfParseNode.hpp"
#include "WsfParseReader.hpp"
#include "WsfParseSourceInclude.hpp"
#include "WsfParseSourceProvider.hpp"
#include "WsfParser.hpp"

class ParseSourceProvider : public WsfParseSourceProvider
{
   //! Derived classes should override this method to return
   //! a source if it exists at aPath
   virtual WsfParseSource* FindSource(const UtPath& aPath)
   {
      if (aPath.Stat() == UtPath::cFILE)
         return new WsfParseSource(aPath.GetSystemPath());
      return 0;
   }
};
int GetLineNumber(WsfParseSourceRange& aRange)
{
   int line = 1;
   for (int i = 0; i < aRange.mBegin; ++i)
   {
      if (aRange.mSource->Get(i) == '\n')
         ++line;
   }
   return line;
}
int CheckFile(const char* aFileName, WsfParser& aParser)
{
   aParser.Reset();
   WsfParseSource* sourcePtr = aParser.PushSource(aFileName, true);
   if (!sourcePtr)
   {
      std::cout << "Error: Cant add initial file.\n";
   }
   std::vector<WsfParseSourceRange> errorList;
   while (1)
   {
      WsfParseNode* treePtr = 0;
      bool          ok      = aParser.GetDefinitions()->GetRoot()->Read(aParser, treePtr);
      // delete treePtr;
      if (!ok)
      {
         WsfParseSourceRange word = aParser.ReadWord();
         if (word.Valid())
         {
            errorList.push_back(word);
         }
         else
         {
            break;
         }
      }
   }
   const std::vector<WsfParseError*>& errors  = aParser.GetErrors();
   size_t                             errorCt = errors.size() + errorList.size();
   std::cout << errorCt << " errors.\n";
   const size_t cMAX_ERR = 10;
   for (size_t i = 0; i < errors.size() && i < cMAX_ERR; ++i)
   {
      WsfParseSourceRange range = errors[i]->mRange;
      int                 line  = GetLineNumber(range);
      std::cout << " error: " << range.mSource->mFilePath << "(" << line << "): " << range.Text() << '\n';
   }
   for (size_t i = 0; i < errorList.size() && i < cMAX_ERR; ++i)
   {
      WsfParseSourceRange range = errorList[i];
      int                 line  = GetLineNumber(range);
      std::cout << " error: " << range.mSource->mFilePath << "(" << line << "): " << range.Text() << '\n';
   }
   return (int)errorCt;
}
void PrintInclude(WsfParseSourceInclude* aIncludePtr)
{
   if (aIncludePtr->mSourcePtr)
   {
      std::cerr << "Enter: " << aIncludePtr->mSourcePtr->mFilePath << '\n';
   }
}
int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      std::cout << "Usage: wsf_core_parse_test  <grammar-file> <input-file(s)>\n";
   }
   else
   {
      std::ifstream        gfile(argv[1]);
      WsfParseDefinitions* defs = new WsfParseDefinitions();
      defs->AddGrammar(gfile);
      defs->Initialize();
      WsfParser parser(new ParseSourceProvider(), UtPath::WorkingDirectory().GetSystemPath(), defs);
      int       totalErrors = 0;
      for (int i = 2; i < argc; ++i)
      {
         std::string arg = argv[i];
         if (arg == "-v")
         {
            parser.IncludeEnter.Connect(&PrintInclude);
         }
         else
         {
            std::cout << "Checking: " << arg << std::endl;
            totalErrors += CheckFile(arg.c_str(), parser);
         }
      }
      std::cout << "Total Errors: " << totalErrors << '\n';
   }
   return 0;
}
