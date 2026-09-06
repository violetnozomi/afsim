// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

//#include "dis_export.h"
// DIS_EXPORT

#ifndef DISGROUPEDENTITYCATEGORYENUM_HPP
#define DISGROUPEDENTITYCATEGORYENUM_HPP

enum DisGroupEntityCategoryEnum
{
   cUNDEFINED                      = 0,
   cBASIC_GROUND_COMBAT_VEHICLE    = 1,
   cENHANCED_GROUND_COMBAT_VEHICLE = 2,
   cBASIC_GROUND_COMBAT_SOLDIER    = 3,
   cENHANCED_GROUND_COMBAT_SOLDIER = 4,
   cBASIC_ROTOR_WING_AIRCRAFT      = 5,
   cENHANCED_ROTOR_WING_AIRCRAFT   = 6,
   cBASIC_FIXED_WING_AIRCRAFT      = 7,
   cENHANCED_FIXED_WING_AIRCRAFT   = 8,
   cGROUND_LOGISTICS_VEHICLE       = 9
};
#endif
