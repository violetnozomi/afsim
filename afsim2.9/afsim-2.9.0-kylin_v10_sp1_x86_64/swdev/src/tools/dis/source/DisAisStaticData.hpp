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

#ifndef VWC_DIS_DISAISSTATICDATA_H
#define VWC_DIS_DISAISSTATICDATA_H

#include <string>

#include "DisTypes.hpp"

class GenI;
class GenO;

// This class defines a custom IFF data record for use in Layer 5 of a DIS IFF
// PDU. It contains descriptive information that isn't expected to change very
// often, if at all. See the AIS wiki page for more details.
struct DisAisStaticData
{
public:
   DisAisStaticData();

   // IFF Data Record header.
   static DisEnum32 GetRecordType() { return 5; }
   static DisUint16 GetRecordLength() { return 96; } // NOTE: update this if fields change

   // Record-specific fields.
   std::string shipFlag;
   DisUint32   mmsi;
   DisUint32   imo;
   std::string callSign;
   DisEnum8    vesselType;
   std::string shipName;
   DisUint16   length_m;
   DisUint16   width_m;
   DisFloat32  draft_m;
   std::string bodyOfWater;
   std::string destination;

   enum VesselType
   {
      WING_IN_GROUND    = 20,
      FISHING           = 30,
      TOWING            = 31,
      DREDGING          = 33,
      DIVING_OPS        = 34,
      MILITARY_OPS      = 35,
      SAILING           = 36,
      PLEASURE_CRAFT    = 37,
      HIGH_SPEED_CRAFT  = 40,
      PILOT_BOAT        = 50,
      SEARCH_AND_RESCUE = 51,
      TUG               = 52,
      PORT_TENDER       = 53,
      ANTI_POLLUTION    = 54,
      LAW_ENFORCEMENT   = 55,
      MEDICAL_TRANSPORT = 58,
      PASSENGER         = 60,
      CARGO             = 70,
      TANKER            = 80,
      OTHER             = 90
   };

   // Read packet from the data stream. Assumes caller has already
   // read the Record Type and Record Length fields (to verify correct
   // data format and size).
   void Get(GenI& aGenI);

   // Write packet to data stream.
   void Put(GenO& aGenO) const;

   //! Data Validation
   bool IsValid() const;
};

#endif
