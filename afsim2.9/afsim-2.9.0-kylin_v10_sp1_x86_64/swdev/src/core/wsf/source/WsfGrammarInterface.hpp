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

#ifndef WSFGRAMMARINTERFACE_HPP
#define WSFGRAMMARINTERFACE_HPP

#include "wsf_export.h"

#include <string>
#include <vector>

#include "WsfApplicationExtension.hpp"

//! Manages grammar files for the application.
//! Tests the WSF parser if it is included in the build.
class WSF_EXPORT WsfGrammarExtension : public WsfApplicationExtension
{
public:
   static WsfGrammarExtension* Find(WsfApplication& aApplication);

   WsfGrammarExtension();

   int  ProcessCommandLine(WsfStandardApplication::Options& aOptions, int aArgc, char* aArgv[]) override;
   void ProcessCommandLineCommands() override;

   void AddGrammarFile(const std::string& aGrammarFilePath);
   int  AddGrammarDirectory(const std::string& aGrammarDirectoryPath);
   void AddGrammarText(const std::string& aGrammarText);

   const std::string& GetGrammarText();

private:
   const std::vector<std::string>& GetGrammarFiles();

   // Below are temporary placeholders to handle integration with the IDE
   void OutputIDE_Data(const std::string& aFileName);

   bool                     mGrammarFilesLoaded;
   std::vector<std::string> mInputGrammarTextList;
   std::vector<std::string> mGrammarFilePaths;
   std::string              mGrammarText;

   // Below are temporary placeholders to handle integration with the IDE
   std::string mIDE_DataOutputFile;
};
#endif
