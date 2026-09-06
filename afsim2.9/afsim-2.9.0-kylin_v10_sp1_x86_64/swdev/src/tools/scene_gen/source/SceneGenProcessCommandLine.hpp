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

#ifndef SCENEGENPROCESSCOMMANDLINE_HPP
#define SCENEGENPROCESSCOMMANDLINE_HPP

#include <string>

#include "UtPath.hpp"

/// @file SceneGenProcessCommandLine.hpp
/// @brief File contains the supporting command line parser for the scene_generator
///    executable.

namespace sceneGen
{
/// @brief The SceneGenProjectInfo struct contains the directory
///    paths of the AFSIM script project that is used in conjunction
///    with the scene_generator executable.
struct SceneGenProjectInfo
{
   //! Path to a "script project" templated off of the directory found in tools/scene_gen/projects/base_project
   UtPath mScriptProjectPath;
   //! The directory to store the generated platform script files.
   UtPath mGeneratedPlatformsOutputPath;
   //! The directory to store the generated scenario startup (entry point) file.
   UtPath mGeneratedStartupPath;
   //! The directory containing the scene_generator input file.
   UtPath mScenarioFilePath;
   //! The directory to store the simulation output files.
   UtPath mSimulationOutputPath;
   //! The name of the scenario file.
   std::string mScenario;
};

/// @brief Handles parsing the scene_generator command line arguments, which
///    requires defining the AFSIM script project (see @ SceneGenProjectInfo)
///    root directory path, and the name of the input file used to create scripts.
///
/// @returns SceneGenProjectInfo containing directory paths built using the parsed arguments.
SceneGenProjectInfo ProcessCommandLine(int aArgc, char* aArgv[]);

/// @brief Validates the directory paths of a SceneGenProjectInfo struct.
///
/// The scene_generator generated scripts are tightly coupled with other AFSIM scripts
/// contained within specific directories. The scene_generator execution will only work
/// with a project directory adhering to the required template structure. An example of
/// correct structure is found in tools/scene_gen/projects/base_project.
///
/// @param aProjectInfo an initialized SceneGenProjectInfo containing path information.
///
/// @returns false if the required project directories do not exist or the
///    command-line parsing failed to set the project path or input file.
bool ValidateProjectDirectory(const SceneGenProjectInfo& aProjectInfo);

} // namespace sceneGen

#endif // SCENEGENPROCESSCOMMANDLINE_HPP
