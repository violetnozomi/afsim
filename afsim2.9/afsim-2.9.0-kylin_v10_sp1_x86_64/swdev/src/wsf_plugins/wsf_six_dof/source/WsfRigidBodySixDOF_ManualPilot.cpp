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

#include "WsfRigidBodySixDOF_ManualPilot.hpp"

#include "WsfRigidBodySixDOF_CommonController.hpp"

wsf::six_dof::RigidBodyManualPilot::RigidBodyManualPilot(RigidBodyPilotObject* aPilotObject)
   : RigidBodyPilotObject(*aPilotObject)
{
}
