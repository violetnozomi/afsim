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

#ifndef VWC_DIS_DISAISPOSITIONDATA_H
#define VWC_DIS_DISAISPOSITIONDATA_H

#include "DisTypes.hpp"

class GenI;
class GenO;

// This class defines a custom IFF data record for use in Layer 5 of a DIS IFF
// PDU. It contains positional information that is sent at regular intervals.
// See the AIS wiki page for more details.
struct DisAisPositionData
{
public:
   DisAisPositionData();

   // IFF Data Record header.
   static DisEnum32 GetRecordType() { return 1; }
   static DisUint16 GetRecordLength() { return 40; } // NOTE: update this if fields change

   // Record-specific fields.
   DisEnum8   navStatus;
   DisFloat64 latitude_deg;
   DisFloat64 longitude_deg;
   DisUint32  mmsi;
   DisFloat32 speed_kts;
   DisFloat32 course_deg;
   DisFloat32 turnRate_deg_sec;

   enum NavStatus
   {
      UNDER_WAY_USING_ENGINE = 0,
      AT_ANCHOR              = 1,
      NOT_UNDER_COMMAND      = 2,
      RESTRICTED_MANEUVER    = 3,
      CONSTRAINED_BY_DRAFT   = 4,
      MOORED                 = 5,
      AGROUND                = 6,
      FISHING                = 7,
      UNDER_WAY_SAILING      = 8
   };

   // Read packet from the data stream. Assumes caller has already
   // read the Record Type and Record Length fields (to verify correct
   // data format and size).
   void Get(GenI& aGenI);

   // Write packet to data stream.
   void Put(GenO& aGenO) const;

   bool IsValid() const;
};

#endif
