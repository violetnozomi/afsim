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

#ifndef DIS_CAPABILITIES_HPP
#define DIS_CAPABILITIES_HPP

#include "DisTypes.hpp"
//#include "DisPduEnums.hpp"
#include "dis_export.h"

//! This class is a utility to set and decipher DIS Entity Capabilities bits.
//! Capabilities Record is an attribute of DisEntityState.  Unlike the
//! DisAppearance class, one does not need to include a file containing
//! enumerated values, as (at least for now) all values set or returned are
//! a single bit.  Since this is true, the bool type is used.  If any multi-
//! bit fields are added to this record, will then have to adopt an approach
//! similar to that taken in DisAppearance.

class DIS_EXPORT DisCapabilities
{
public:
   static const DisUint32 cAMMO_SUPPLY_BIT;     // bit 0
   static const DisUint32 cFUEL_SUPPLY_BIT;     // bit 1
   static const DisUint32 cRECOVERY_SUPPLY_BIT; // bit 2
   static const DisUint32 cREPAIR_SUPPLY_BIT;   // bit 3
   static const DisUint32 cADS_B_SUPPLY_BIT;    // bit 4

   static const DisUint32 cAMMO_SUPPLY_MASK;
   static const DisUint32 cFUEL_SUPPLY_MASK;
   static const DisUint32 cRECOVERY_SUPPLY_MASK;
   static const DisUint32 cREPAIR_SUPPLY_MASK;
   static const DisUint32 cADS_B_SUPPLY_MASK;

   DisCapabilities();
   DisCapabilities(const DisCapabilities& aSrc);
   DisCapabilities(DisUint32 aValue);

   DisUint32 Value() const { return mCapabilityRecord; }
   void      Set(DisUint32 aValue) { mCapabilityRecord = aValue; }

   bool GetMaySupplyAmmunition() const;
   bool GetMaySupplyFuel() const;
   bool GetMaySupplyVehicleRecovery() const;
   bool GetMaySupplyVehicleRepair() const;
   bool GetMaySupplyADS_B() const; // Note: No further clarification on the meaning of 'ADS_B' is available at this time.

   void SetMaySupplyAmmunition(bool aValue);
   void SetMaySupplyFuel(bool aValue);
   void SetMaySupplyVehicleRecovery(bool aValue);
   void SetMaySupplyVehicleRepair(bool aValue);
   void SetMaySupplyADS_B(bool aValue);

private:
   DisUint32 mCapabilityRecord;
};

#endif
