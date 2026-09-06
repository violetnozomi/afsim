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

#include "WsfCoverageMeasureOutput.hpp"

#include <string>

#include "UtInput.hpp"

namespace wsf
{
namespace coverage
{

bool MeasureOutput::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command;
   aInput.GetCommand(command);
   if (command == "file")
   {
      aInput.ReadCommand(mFileName);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

} // namespace coverage
} // namespace wsf
