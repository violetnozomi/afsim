// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfFormationDetachCommand.hpp"

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfFormUpKeepStationManeuver.hpp"
#include "WsfFormation.hpp"
#include "WsfFormationUpdateStationKeeping.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"

WsfFormationDetachCommand::WsfFormationDetachCommand()
   : WsfFormationCommand()
{
}

WsfFormationDetachCommand* WsfFormationDetachCommand::Clone() const
{
   return new WsfFormationDetachCommand{*this};
}

double WsfFormationDetachCommand::Execute(double aSimTime)
{
   if (GetDebug())
   {
      auto out = ut::log::debug() << "Executing command.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Formation: " << GetFormation()->GetQualifiedName();
   }
   WsfFormation* formPtr = GetFormation();
   formPtr->SetAttached(false);
   WsfFormationUpdateStationKeeping::Update(formPtr, aSimTime);
   return -1.0;
}

bool WsfFormationDetachCommand::AcceptCommand(WsfFormation* aFormationPtr)
{
   return true;
}

void WsfFormationDetachCommand::ComputeCommonTransformation(WsfFormation* aFormationPtr) {}

std::unique_ptr<WsfFormationCommand> WsfFormationDetachCommand::ComputeTransformation(WsfFormation* aParentPtr,
                                                                                      WsfFormation* aChildPtr)
{
   return nullptr;
}
