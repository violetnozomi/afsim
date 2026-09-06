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
#include "SceneGenProcessCommandLine.hpp"

int main(int argc, char** argv)
{
   sceneGen::SceneGenProjectInfo project_info = sceneGen::ProcessCommandLine(argc, argv);

   if (sceneGen::ValidateProjectDirectory(project_info))
   {
      sceneGen::CreateScripts(project_info);
   }

   return 0;
}
