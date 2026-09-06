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
#ifndef GEN_UMP_IO_RUDP_STATS_HPP
#define GEN_UMP_IO_RUDP_STATS_HPP

#include "GenExport.hpp"
#include "ump.h"

// We have this goofy struct so that we can have a UMPRUDPStatsType struct
// and not bring in ump.h in GenUmpIO.hpp

struct GEN_EXPORT GenUmpIORudpStats : public UMPRUDPStatsType
{
   GenUmpIORudpStats() {}
   ~GenUmpIORudpStats() {}
};

#endif
