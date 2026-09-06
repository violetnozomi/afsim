// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "MapPlotFunction.hpp"

#include "UtInput.hpp"

MapPlotFunction::MapPlotFunction(const WsfScenario& aScenario, const std::string& aScriptClassName)
   : Function(aScenario, aScriptClassName)
{
}

MapPlotFunction::~MapPlotFunction() = default;

bool MapPlotFunction::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "variable")
   {
      std::string varName;
      aInput.ReadValue(varName);
      try
      {
         mPlotVariables.AddVariable(varName);
      }
      catch (...)
      {
         throw UtInput::BadValue(aInput, "Unknown variable type: " + varName);
      }
   }
   else if (command == "script_variable")
   {
      std::string varName;
      double      initValue;
      aInput.ReadValue(varName);
      aInput.ReadValue(initValue);
      if (!mPlotVariables.ProcessScriptVariableInput(varName, initValue))
      {
         throw UtInput::BadValue(aInput, "Unknown script selected: " + varName);
      }
   }
   else if (mPlotVariables.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = Function::ProcessInput(aInput);
   }
   return myCommand;
}

void MapPlotFunction::RegisterMapPlotVariable(const std::string& aVarName, std::shared_ptr<MapPlotVariable> aVariablePtr)
{
   mPlotVariables.RegisterMapPlotVariable(aVarName, aVariablePtr);
}
