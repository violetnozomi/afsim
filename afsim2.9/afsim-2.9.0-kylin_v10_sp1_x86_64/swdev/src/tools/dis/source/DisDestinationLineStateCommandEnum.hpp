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

#ifndef DISDESTINATIONLINESTATECOMMANDENUM_HPP
#define DISDESTINATIONLINESTATECOMMANDENUM_HPP

enum DisDestinationLineStateCommandEnum
{
   NONE                               = 0,
   SET_LINE_STATE_TRANSMITTING        = 1,
   SET_LINE_STATE_NOT_TRANSMITTING    = 2,
   RETURN_TO_LOCAL_LINE_STATE_CONTROL = 3
};

#endif
