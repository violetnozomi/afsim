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

#include "DisEmissionEnums.hpp"

const std::string& DisEnum::emission::state_update::ToString(DisEnum8 aSUI)
{
   static const std::string STR[] = {"Heartbeat Update", "Changed Data Update"};
   size_t                   index = IsValid(aSUI) ? static_cast<size_t>(aSUI) : 0;
   return STR[index];
}
