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

#include "WsfGrammarInterface.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "UtScanDir.hpp"
#include "UtScriptTypes.hpp"
#include "WsfApplication.hpp"
#include "WsfExtensionList.hpp"
#include "WsfVersion.hpp"
#include "script/WsfScriptManager.hpp"

// =================================================================================================
void WSF_EXPORT Register_grammar_interface(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("grammar_interface"))
   {
      // This extension REQUIRES by the "event_pipe" extension.
      WSF_REGISTER_EXTENSION(aApplication, event_pipe);
      aApplication.RegisterExtension("grammar_interface", ut::make_unique<WsfGrammarExtension>());
      aApplication.ExtensionDepends("grammar_interface", "event_pipe", true);
   }
}

// =================================================================================================
WsfGrammarExtension* WsfGrammarExtension::Find(WsfApplication& aApplication)
{
   return static_cast<WsfGrammarExtension*>(aApplication.FindExtension("grammar_interface"));
}

WsfGrammarExtension::WsfGrammarExtension()
   : mGrammarFilesLoaded(false)
   , mInputGrammarTextList()
   , mGrammarFilePaths()
   , mGrammarText()
{
}

// virtual
int WsfGrammarExtension::ProcessCommandLine(WsfStandardApplication::Options& aOptions, int aArgc, char* aArgv[])
{
   mGrammarFilesLoaded = false;
   // Handle the --ide-output command line option.
   // This is a temporary mechanism the IDE uses to grab information about WSF.
   if (strcmp(aArgv[0], "--ide-output") == 0)
   {
      aOptions.mRunMode = WsfStandardApplication::cWRITE_GRAMMAR;
      if (aArgc > 1)
      {
         mIDE_DataOutputFile = aArgv[1];
      }
      else
      {
         ut::log::info("test_fail") << "-FAIL- An output file must be specified with the '--ide-output' flag.";
      }
      return 2;
   }
   return 0;
}

// virtual
void WsfGrammarExtension::ProcessCommandLineCommands()
{
   if (!mIDE_DataOutputFile.empty())
   {
      OutputIDE_Data(mIDE_DataOutputFile);
      exit(0xAF1de);
   }
}

//! Adds a file containing grammar text to the grammar input.
//! Applications and WSF libraries should add grammar for
//! processing input commands with either AddGrammarText() or AddGrammarFile()
void WsfGrammarExtension::AddGrammarFile(const std::string& aGrammarFilePath)
{
   if (std::find(mGrammarFilePaths.begin(), mGrammarFilePaths.end(), aGrammarFilePath) == mGrammarFilePaths.end())
   {
      mGrammarFilePaths.push_back(aGrammarFilePath);
   }
}

//! Adds a block of grammar text to the grammar input.
//! Applications and WSF libraries should add grammar for
//! processing input commands with either AddGrammarText() or AddGrammarFile()
void WsfGrammarExtension::AddGrammarText(const std::string& aGrammarText)
{
   mInputGrammarTextList.push_back(aGrammarText);
}

//! Adds all files with the .ag extension to the grammar text.
int WsfGrammarExtension::AddGrammarDirectory(const std::string& aGrammarDirectoryPath)
{
   int                   filesAdded = 0;
   UtScanDir::DirEntries entries;
   UtScanDir::ScanDir(aGrammarDirectoryPath, entries, "*.ag");
   for (size_t i = 0; i < entries.size(); ++i)
   {
      AddGrammarFile((UtPath(aGrammarDirectoryPath) + entries[i].mFilename).GetSystemPath());
      ++filesAdded;
   }
   return filesAdded;
}

const std::vector<std::string>& WsfGrammarExtension::GetGrammarFiles()
{
   int    argc;
   char** argv;
   GetApplication().GetCommandLineArguments(argc, argv);
   if ((!mGrammarFilesLoaded) && (argc > 0))
   {
      // search binary path
      UtPath exePath(argv[0]);
      exePath.Up();

      UtPath installedGrammarPath = exePath + (GetApplication().GetApplicationName() + "_grammar");
      if (installedGrammarPath.Stat() != UtPath::cDIRECTORY)
      {
         installedGrammarPath = exePath + "grammar";
      }
      bool hasInstalledGrammars = installedGrammarPath.Stat() == UtPath::cDIRECTORY;
      if (!hasInstalledGrammars)
      {
         installedGrammarPath = exePath + "../grammar";
         hasInstalledGrammars = installedGrammarPath.Stat() == UtPath::cDIRECTORY;
      }

      if (hasInstalledGrammars)
      {
         AddGrammarDirectory(installedGrammarPath.GetSystemPath());
      }

      mGrammarFilesLoaded = true;
      mGrammarText.clear();
      for (size_t i = 0; i < mGrammarFilePaths.size(); ++i)
      {
         std::ifstream grammarFile(mGrammarFilePaths[i].c_str());
         mGrammarText.insert(mGrammarText.end(),
                             (std::istreambuf_iterator<char>(grammarFile)),
                             std::istreambuf_iterator<char>());
      }

      // Ask extensions if they have grammar to report
      std::stringstream        extensionGrammarStream;
      std::vector<std::string> appExtensionNames = GetApplication().GetExtensions().GetExtensionOrder();
      for (size_t i = 0; i < appExtensionNames.size(); ++i)
      {
         WsfApplicationExtension* extPtr =
            static_cast<WsfApplicationExtension*>(GetApplication().GetExtensions().FindExtension(appExtensionNames[i]));
         extPtr->PrintGrammar(extensionGrammarStream);
      }
      mGrammarText += extensionGrammarStream.str();

      for (size_t i = 0; i < mInputGrammarTextList.size(); ++i)
      {
         mGrammarText += mInputGrammarTextList[i];
      }
      mInputGrammarTextList.clear();
   }
   return mGrammarFilePaths;
}

//! Returns the WSF grammar text.
const std::string& WsfGrammarExtension::GetGrammarText()
{
   // Ensure mGrammarText is set
   GetGrammarFiles();
   return mGrammarText;
}

void WsfGrammarExtension::OutputIDE_Data(const std::string& aFileName)
{
   std::ofstream outfile(aFileName.c_str());
   outfile << "^^^grammar\n";
   outfile << GetGrammarText();

   outfile << "# BEGIN_SCRIPT_DATA: Generated from source code...\n";
   UtScriptTypes& scriptTypes = WsfScriptManager::GetTypes();
   for (UtScriptTypes::ConstClassMapItr ti = scriptTypes.Begin(); ti != scriptTypes.End(); ++ti)
   {
      outfile << "(aux-script-class " << '"' << ti->first << '"';
      UtScriptClass* classPtr = ti->second.get();
      if (classPtr->IsConstructible())
      {
         outfile << " :constructible 1";
      }
      if (classPtr->IsCloneable())
      {
         outfile << " :cloneable 1";
      }
      if (classPtr->IsContainer())
      {
         outfile << " :container 1";
      }
      if (classPtr->IsLessThanComparable() || classPtr->IsBasicType())
      {
         outfile << " :less_compare 1";
      }
      if (classPtr->IsEqualityComparable() || classPtr->IsBasicType())
      {
         outfile << " :equal_compare 1";
      }

      const auto& templateArgs = classPtr->GetTemplateArgTypes();
      for (size_t i = 0; i < templateArgs.size(); ++i)
      {
         outfile << " :template_arg " << '"' << templateArgs[i].mTypeId << '"';
      }
      const std::vector<WsfStringId>& hierarchy = classPtr->GetClassNameHierachy();
      for (size_t i = 0; i < hierarchy.size(); ++i)
      {
         outfile << " :base_class " << '"' << hierarchy[i] << '"';
      }
      const std::vector<WsfStringId>& implicitCasts = classPtr->GetImplicitCastTypes();
      for (size_t i = 0; i < implicitCasts.size(); ++i)
      {
         outfile << " :implicit_cast " << '"' << implicitCasts[i] << '"';
      }
      const std::vector<WsfStringId>& explicitCasts = classPtr->GetExplicitCastTypes();
      for (size_t i = 0; i < explicitCasts.size(); ++i)
      {
         outfile << " :explicit_cast " << '"' << explicitCasts[i] << '"';
      }
      outfile << '\n';
      for (size_t mi = 0; mi < classPtr->GetMethodCount(); ++mi)
      {
         UtScriptClass::InterfaceMethod* methodPtr = classPtr->GetMethodEntry(mi);
         std::string                     name      = methodPtr->GetName();
         std::string                     rtype     = "void";
         if (methodPtr->GetHasReturnVal())
         {
            rtype = methodPtr->GetReturnTypeId();
         }
         outfile << "  (aux-script-method " << name << " :rtype \"" << rtype << '"';
         if (methodPtr->GetIsStatic())
         {
            outfile << " :static 1";
         }
         if (methodPtr->IsVarArg())
         {
            outfile << " :varargs 1";
         }
         UtScriptClass::ArgTypes args = methodPtr->GetArgTypes();
         for (size_t ai = 0; ai < args.size(); ++ai)
         {
            outfile << " :parameter " << '"' << args[ai] << '"';
         }
         outfile << ")\n";
      }
      outfile << ")\n";
   }
   outfile << "\n$$$grammar\n";
   outfile << "^^^version\n";
   outfile << wsf::version::GetString() << '\n';
   outfile << "$$$version\n";
   outfile << "^^^application\n";
   outfile << GetApplication().GetApplicationName() << '\n';
   outfile << "$$$application\n";
   outfile << "^^^features\n";
   if (GetApplication().ExtensionIsRegistered("profiling"))
   {
      outfile << "profiling\n";
   }
   outfile << "$$$features\n";

   outfile.close();
}
