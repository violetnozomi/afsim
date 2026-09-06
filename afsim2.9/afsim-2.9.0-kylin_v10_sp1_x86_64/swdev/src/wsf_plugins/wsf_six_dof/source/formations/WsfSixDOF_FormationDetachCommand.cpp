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

#include "WsfSixDOF_FormationDetachCommand.hpp"

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfPlatform.hpp"
#include "WsfSixDOF_FormUpKeepStationManeuver.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_FormationUpdateStationKeeping.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::FormationDetachCommand::FormationDetachCommand()
   : FormationCommand()
{
}

wsf::six_dof::FormationDetachCommand* wsf::six_dof::FormationDetachCommand::Clone() const
{
   return new FormationDetachCommand{*this};
}

double wsf::six_dof::FormationDetachCommand::Execute(double aSimTime)
{
   if (GetDebug())
   {
      auto out = ut::log::debug() << "Executing command.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "Formation: " << GetFormation()->GetQualifiedName();
   }
   Formation* formPtr = GetFormation();
   formPtr->SetAttached(false);
   FormationUpdateStationKeeping::Update(formPtr, aSimTime);
   return -1.0;
}

bool wsf::six_dof::FormationDetachCommand::AcceptCommand(Formation* aFormationPtr)
{
   return true;
}

void wsf::six_dof::FormationDetachCommand::ComputeCommonTransformation(Formation* aFormationPtr) {}

std::unique_ptr<wsf::six_dof::FormationCommand>
wsf::six_dof::FormationDetachCommand::ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr)
{
   return nullptr;
}
