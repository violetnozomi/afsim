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

#include "WsfAtmosphere.hpp"

#include <string>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"

namespace wsf
{
namespace space
{

Atmosphere::Atmosphere()
   : mCentralBodyPtr{new ut::EarthEGM96{}}
{
}

bool Atmosphere::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command = aInput.GetCommand();
   if (command == "central_body")
   {
      UtInputBlock block{aInput};
      std::string  body;
      block.ReadCommand(body);
      mCentralBodyPtr = ut::CentralBody::Create(body);
      if (mCentralBodyPtr == nullptr)
      {
         throw UtInput::BadValue{aInput, "Unrecognized central body type."};
      }
      block.ProcessInput(mCentralBodyPtr.get());
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

} // namespace space
} // namespace wsf
