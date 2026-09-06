// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SceneGenCreateScripts.hpp"

#include <fstream>
#include <map>

#include "SceneGenInputFileReader.hpp"
#include "SceneGenPlatformTeam.hpp"
#include "UtPath.hpp"

namespace sceneGen
{
namespace
{
constexpr const char cPLATFORM_FILE_POSTFIX[] = "_layout.txt";
constexpr const char cSTARTUP_FILE_POSTFIX[]  = "_main.txt";

void FailedToOpenMessage(const UtPath& aPath)
{
   auto err = ut::log::error() << "Could not open file: " << aPath.GetSystemPath();
}

void CreateScenarioStartupFile(const SceneGenProjectInfo&      aProjectInfo,
                               const std::set<std::string>&    aPlatformTypes,
                               const std::vector<std::string>& aGeneratedPlatformFiles)
{
   std::ofstream file;

   UtPath path = aProjectInfo.mGeneratedStartupPath;
   path += aProjectInfo.mScenario + cSTARTUP_FILE_POSTFIX;

   file.open(path.GetSystemPath());

   if (file.is_open())
   {
      file << R"(
# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
)";
      // SCRIPT_PROJECT_PATH - root of a script project following the structure found in
      // tools/scene_gen/projects/base_project.
      file << "define_path_variable SCRIPT_PROJECT_PATH " << aProjectInfo.mScriptProjectPath.GetSystemPath() << '\n';
      file << "define_path_variable SCENARIO " << aProjectInfo.mScenario << '\n';

      file << R"(
file_path $(SCRIPT_PROJECT_PATH)
file_path $(SCRIPT_PROJECT_PATH)/scripts/

log_file $(SCRIPT_PROJECT_PATH)/output/$(SCENARIO).log

event_output file $(SCRIPT_PROJECT_PATH)/output/$(SCENARIO).evt end_event_output

event_pipe
   file $(SCRIPT_PROJECT_PATH)/output/$(SCENARIO).aer
   use_preset default
end_event_pipe

)";

      for (const std::string& type : aPlatformTypes)
      {
         file << "include_once $(SCRIPT_PROJECT_PATH)/platforms_types/" << type << ".txt\n";
      }
      for (const std::string& file_name : aGeneratedPlatformFiles)
      {
         file << "include_once $(SCRIPT_PROJECT_PATH)/" << aProjectInfo.mGeneratedPlatformsOutputPath.GetFileName()
              << '/' << file_name << "\n";
      }

      file << R"(

end_time 1 hr
)";

      file.close();
   }
   else
   {
      FailedToOpenMessage(path);
   }
}
} // namespace

void CreateScripts(const SceneGenProjectInfo& aProjectInfo)
{
   SceneGenInputFileReader reader{aProjectInfo.mScenarioFilePath.GetSystemPath()};

   if (reader.IsOpen())
   {
      // A PlatformTeam is created for each color group <color, team>
      std::map<std::string, SceneGenPlatformTeam> platform_teams;
      std::set<std::string>                       platform_types;

      try
      {
         reader.GetGroupCount();

         while (reader.GroupsLeft() > 0)
         {
            SceneGenInputData input; // default values

            // parse an individual group
            reader.FetchData(input, platform_types);

            // Determine if all required data has been set
            ValidateInput(input);

            auto team = platform_teams.find(input.mSide);
            if (team != platform_teams.end())
            {
               AddGroup(team->second, input);
            }
            else
            {
               SceneGenPlatformTeam new_team;
               new_team.mSide = input.mSide;
               AddGroup(new_team, input);
               platform_teams.insert(std::make_pair(new_team.mSide, new_team));
            }
         }
         reader.Close();
      }
      catch (const std::exception& e)
      {
         auto err = ut::log::fatal() << e.what() << " - Line: " << reader.GetCurrentLine();
         err.AddNote() << "No files generated";

         reader.Close();
         return;
      }

      AddTrackData(platform_teams);

      std::vector<std::string> generated_platform_files;

      // create script files
      for (const auto& team : platform_teams)
      {
         std::ofstream file;

         UtPath      path              = aProjectInfo.mGeneratedPlatformsOutputPath;
         std::string platform_filename = aProjectInfo.mScenario + '_' + team.second.mSide + cPLATFORM_FILE_POSTFIX;
         path += platform_filename;

         file.open(path.GetSystemPath());

         if (file.is_open())
         {
            generated_platform_files.push_back(platform_filename);
            file << team.second;
            file.close();
         }
         else
         {
            FailedToOpenMessage(path);
            return;
         }
      }

      // [scenario]_main.txt
      CreateScenarioStartupFile(aProjectInfo, platform_types, generated_platform_files);
   }
   else
   {
      FailedToOpenMessage(aProjectInfo.mScenarioFilePath);
   }
}
} // namespace sceneGen
