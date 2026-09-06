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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef NIMAMAPS_HPP
#define NIMAMAPS_HPP

#include "geodata_export.h"

class GEODATA_EXPORT NimaMaps
{
public:
   // Data Series (Map/Chart) types from MIL-STD-2411-1, sec 5.1.4.
   // Note: Always use the constants for these.  New values may be
   // inserted at any time.
   enum DataSeries
   {
      DsUnknown = 0,
      DsGNC     = 1,  // 1:5,000,000 Global Navigation Chart
      DsJNC     = 2,  // 1:2,000,000 Jet Navigation Chart
      DsONC     = 3,  // 1:1,000,000 Operational Navigation Chart
      DsTPC     = 4,  // 1:500,000 Tactical Pilotage Chart
      DsJOG     = 5,  // 1:250,000 Joint Operations Graphic
      DsJOG_A   = 6,  // 1:250,000 Joint Operations Graphic - Air
      DsJOG_R   = 7,  // 1:250,000 Joint Operations Graphic - Radar
      DsTFC     = 8,  // 1:250,000 Transit Flying Chart (UK)
      DsATC     = 9,  // 1:200,000 Series 200 Air Target Chart
      DsTLM_100 = 10, // 1:100,000 Topographic Line Map
      DsTLM_50  = 11, // 1:50,000 Topographic Line Map
      DsCIB_10  = 12,
      DsCIB_5   = 13,
      DsCIB_1   = 14,
      DsCount   = 15 // Number of data series
   };

private:
};

#endif
