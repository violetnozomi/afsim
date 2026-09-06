// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_Environment.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlugin.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

void wsf::six_dof::Environment::AddedToScenario()
{
   const WsfScenario& scenario(GetScenario());

   // Define pointers to simulation-common environment data
   mAtmosphere  = &(scenario.GetAtmosphere());
   mEnvironment = &(scenario.GetEnvironment());
   mTerrain     = ut::make_unique<Terrain>(scenario.GetTerrainInterface());
}

bool wsf::six_dof::Environment::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command   = aInput.GetCommand();
   if (command == "six_dof_environment")
   {
      myCommand = true;
      UtInputBlock block(aInput);

      std::string subCommand;
      while (block.ReadCommand(subCommand))
      {
         if (subCommand == "terrain")
         {
            std::string filename;
            aInput.ReadValue(filename);
            filename = aInput.LocateFile(filename);
            mTerrain->ReadTerrainFile(filename);
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within Environment::ProcessInput().";
            out.AddNote() << "Command: " << subCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return myCommand;
}

void wsf::six_dof::Environment::GetAeroParameters(double  aAltitude_ft,
                                                  double  aSpeed_fps,
                                                  double& aAirDensity_slug_ft3,
                                                  double& aDynamicPressure_psf,
                                                  double& aStaticPressure_psf) const
{
   double altitude_m = aAltitude_ft * UtMath::cM_PER_FT;

   aAirDensity_slug_ft3 = CalcAirDensity_slug_ft3(aAltitude_ft);

   // Calc dynamic pressure
   aDynamicPressure_psf = CalcDynamicPressure_psf(altitude_m, aSpeed_fps);

   // Calc static pressure
   aStaticPressure_psf = CalcAmbientPressure_psf(aAltitude_ft);
}
