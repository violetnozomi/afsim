// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfUniqueId.hpp"

#include "WsfSimulation.hpp"

//! Initialize the unique ID with the next unique ID from the simulation.
//! This must be called by the Initialize method in the inheriting class.
//! @param aSimulationPtr The simulation in which this object exists.
void WsfUniqueId::AssignUniqueId(WsfSimulation* aSimulationPtr)
{
   mUniqueId = aSimulationPtr->AssignUniqueId();
}
