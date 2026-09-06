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

#include "GenThreadControl.hpp"

GenThreadControl::GenThreadControl() {}

// Virtual
GenThreadControl::~GenThreadControl() {}

int GenThreadControl::Lock()
{
   // Return a positive number if successful
   return 1;
}

int GenThreadControl::Unlock()
{
   // Return a positive number if successful
   return 1;
}
