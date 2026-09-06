// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WeaponsCoordinationPart.hpp"

#include "ComputerProcessor.hpp"
#include "Message9_0.hpp"
#include "Message9_1.hpp"
#include "WsfGroundMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfSubsurfaceMover.hpp"
#include "WsfSurfaceMover.hpp"

namespace WsfL16
{

WeaponsCoordinationPart::WeaponsCoordinationPart(WsfScenario& aScenario)
   : ComputerPart(aScenario)
{
   mSendInterval.SetUpdateInterval(12.0);
}

WeaponsCoordinationPart::WeaponsCoordinationPart(const WeaponsCoordinationPart& aSrc)
   : ComputerPart(aSrc)
{
}

ComputerPart* WeaponsCoordinationPart::Clone() const
{
   return new WeaponsCoordinationPart(*this);
}

bool WeaponsCoordinationPart::Initialize(double aSimTime, ComputerProcessor& aComputer)
{
   bool ok = ComputerPart::Initialize(aSimTime, aComputer);
   return ok;
}

bool WeaponsCoordinationPart::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (ComputerPart::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

void WeaponsCoordinationPart::HandleReceive(double aSimTime, const Messages::InitialBase& aMessage)
{
   auto logInfo = ut::log::info();
   using namespace Messages::J9_0;
   if (aMessage.GetLabel() == 9 && aMessage.GetSubLabel() == 0)
   {
      Initial& msg       = (Initial&)aMessage;
      int      addressee = msg.Addressee;
      if (addressee == (int)GetComputer()->GetTrackNumber())
      {
         logInfo << "Received J-9.0:";
         aMessage.PrintMessage(logInfo);
      }
      else
      {
         logInfo << "Received J-9.0 - Addressee doesn't match " << addressee;
      }
   }
}

void WeaponsCoordinationPart::HandleSend(double aSimTime) {}

} // namespace WsfL16
