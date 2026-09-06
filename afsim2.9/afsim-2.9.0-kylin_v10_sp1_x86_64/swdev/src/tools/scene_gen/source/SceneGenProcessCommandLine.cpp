// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SceneGenProcessCommandLine.hpp"

#include <map>

#include "UtLog.hpp"

namespace sceneGen
{
namespace
{
//! Command-line argument identifiers
constexpr const char cSCRIPT_PROJECT_DIRECTORY_ARG[] = "-script-project-root";
constexpr const char cSCENARIO_ARG[]                 = "-scenario-file";

//! Test project subdirectory relative paths
const std::map<std::string, std::string> cRELATIVE_DIR{{"PLATFORMS_OUTPUT", "platforms_generated"},
                                                       {"INPUT", "scene_gen_input"},
                                                       {"SIM_OUTPUT", "output"},
                                                       {"STARTUP", "startup"}};

void EmptyArgumentMessage(const std::string& aArgument)
{
   ut::log::error() << aArgument << " not set.";
}

void InvalidPathMessage(const UtPath& aPath)
{
   ut::log::error() << aPath.GetSystemPath() << " does not exist and could not be generated.";
}

// Checks if a directory exists and attempts to create it if it doesn't
bool EnsureDirectoryExists(const UtPath& aDirectoryPath)
{
   if (!aDirectoryPath.Exists())
   {
      return aDirectoryPath.Mkdir();
   }
   return true;
}

// Helper to build the paths to the directories required in the templated script project
UtPath AppendIfDefined(const UtPath& aPath, const std::string& aKey)
{
   UtPath result = aPath;

   auto it = cRELATIVE_DIR.find(aKey);

   if (it != cRELATIVE_DIR.end())
   {
      result += it->second;
   }
   return result;
}
} // namespace

SceneGenProjectInfo ProcessCommandLine(int aArgc, char* aArgv[])
{
   int                 num_args_processed = 1; // skip over command name
   SceneGenProjectInfo result;

   while (num_args_processed < aArgc)
   {
      char* const current_arg = aArgv[num_args_processed];

      if (std::strcmp(current_arg, cSCRIPT_PROJECT_DIRECTORY_ARG) == 0)
      {
         result.mScriptProjectPath = {aArgv[++num_args_processed]};
      }
      else if (std::strcmp(current_arg, cSCENARIO_ARG) == 0)
      {
         result.mScenario = aArgv[++num_args_processed];
      }
      else
      {
         auto err = ut::log::error() << "Unrecognized argument: " << current_arg;
         err.AddNote() << "Valid arguments are: " << cSCRIPT_PROJECT_DIRECTORY_ARG << " <SCRIPT_PROJECT_PATH> and"
                       << cSCENARIO_ARG << " <INPUT_FILE_NAME.txt>";
         return result;
      }

      ++num_args_processed;
   }

   result.mGeneratedPlatformsOutputPath = AppendIfDefined(result.mScriptProjectPath, "PLATFORMS_OUTPUT");
   result.mGeneratedStartupPath         = AppendIfDefined(result.mScriptProjectPath, "STARTUP");
   result.mSimulationOutputPath         = AppendIfDefined(result.mScriptProjectPath, "SIM_OUTPUT");
   result.mScenarioFilePath             = AppendIfDefined(result.mScriptProjectPath, "INPUT");

   result.mScenarioFilePath += result.mScenario;

   // remove extension postfix; mScenario will be used to name generated files
   size_t pos = result.mScenario.find(".txt");
   if (pos != std::string::npos)
   {
      result.mScenario.erase(pos, std::string::npos);
   }

   return result;
}

bool ValidateProjectDirectory(const SceneGenProjectInfo& aProjectInfo)
{
   if (aProjectInfo.mScriptProjectPath.IsNull())
   {
      EmptyArgumentMessage(cSCRIPT_PROJECT_DIRECTORY_ARG);
      return false;
   }
   else if (!aProjectInfo.mScriptProjectPath.Exists())
   {
      InvalidPathMessage(aProjectInfo.mScriptProjectPath);
      return false;
   }

   if (aProjectInfo.mScenario.empty())
   {
      EmptyArgumentMessage(cSCENARIO_ARG);
      return false;
   }

   for (const auto& directory : cRELATIVE_DIR)
   {
      if (!EnsureDirectoryExists(aProjectInfo.mScriptProjectPath + directory.second))
      {
         InvalidPathMessage(aProjectInfo.mScriptProjectPath + directory.second);
         return false;
      }
   }

   return true;
}

} // namespace sceneGen
