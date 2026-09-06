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

#include "xio_sim/WsfXIO_Component.hpp"

WsfXIO_Component::WsfXIO_Component(int aRole, const std::string& aName)
   : WsfSimpleComponentParentT<WsfXIO_Simulation>(aRole, aName)
{
}
