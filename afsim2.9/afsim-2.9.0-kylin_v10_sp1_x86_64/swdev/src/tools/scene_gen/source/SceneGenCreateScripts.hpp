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

#ifndef SCENEGENCREATESCRIPTS_HPP
#define SCENEGENCREATESCRIPTS_HPP

/// @file SceneGenCreateScripts.hpp
/// @brief Declaration of the parser function to generate AFSIM scripts

#include <string>

#include <SceneGenProcessCommandLine.hpp>

namespace sceneGen
{
/// Processes an input file, located at SceneGenProjectInfo::mScenarioFilePath, to generate AFSIM scripts.
///
/// File lines are parsed by identifying description-value(s) entries, and data
/// validation will terminate execution if improperly formed data is detected.
/// If the input file parsing succeeds, the function generates the script(s) of the
/// platform group(s) defined in the input file as well as an entry point file
/// (main.txt) for the simulation.
///
/// @param aProjectInfo a validated SceneGenProjectInfo containing path information.
void CreateScripts(const SceneGenProjectInfo& aProjectInfo);

} // namespace sceneGen

#endif // SCENEGENCREATESCRIPTS_HPP
