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

#include "DisCapabilities.hpp"

// static
const DisUint32 DisCapabilities::cAMMO_SUPPLY_BIT     = 0; // bit 0
const DisUint32 DisCapabilities::cFUEL_SUPPLY_BIT     = 1; // bit 1
const DisUint32 DisCapabilities::cRECOVERY_SUPPLY_BIT = 2; // bit 2
const DisUint32 DisCapabilities::cREPAIR_SUPPLY_BIT   = 3; // bit 3
const DisUint32 DisCapabilities::cADS_B_SUPPLY_BIT    = 4; // bit 4

const DisUint32 DisCapabilities::cAMMO_SUPPLY_MASK     = 0x00000001; // bit 0
const DisUint32 DisCapabilities::cFUEL_SUPPLY_MASK     = 0x00000002; // bit 1
const DisUint32 DisCapabilities::cRECOVERY_SUPPLY_MASK = 0x00000004; // bit 2
const DisUint32 DisCapabilities::cREPAIR_SUPPLY_MASK   = 0x00000008; // bit 3
const DisUint32 DisCapabilities::cADS_B_SUPPLY_MASK    = 0x00000010; // bit 4

DisCapabilities::DisCapabilities()
   : mCapabilityRecord(0){};

DisCapabilities::DisCapabilities(const DisCapabilities& aSrc)
   : mCapabilityRecord(aSrc.mCapabilityRecord){};

DisCapabilities::DisCapabilities(DisUint32 aValue)
   : mCapabilityRecord(aValue){};

bool DisCapabilities::GetMaySupplyAmmunition() const
{
   return static_cast<bool>(mCapabilityRecord & cAMMO_SUPPLY_MASK >> cAMMO_SUPPLY_BIT);
}
bool DisCapabilities::GetMaySupplyFuel() const
{
   return static_cast<bool>(mCapabilityRecord & cFUEL_SUPPLY_MASK >> cFUEL_SUPPLY_BIT);
}
bool DisCapabilities::GetMaySupplyVehicleRecovery() const
{
   return static_cast<bool>(mCapabilityRecord & cRECOVERY_SUPPLY_MASK >> cRECOVERY_SUPPLY_BIT);
}
bool DisCapabilities::GetMaySupplyVehicleRepair() const
{
   return static_cast<bool>(mCapabilityRecord & cREPAIR_SUPPLY_MASK >> cREPAIR_SUPPLY_BIT);
}
bool DisCapabilities::GetMaySupplyADS_B() const
{
   return static_cast<bool>(mCapabilityRecord & cADS_B_SUPPLY_MASK >> cADS_B_SUPPLY_BIT);
}

void DisCapabilities::SetMaySupplyAmmunition(bool aValue)
{
   if (aValue)
   {
      mCapabilityRecord |= cAMMO_SUPPLY_MASK;
   }
   else
   {
      mCapabilityRecord &= ~cAMMO_SUPPLY_MASK;
   }
}
void DisCapabilities::SetMaySupplyFuel(bool aValue)
{
   if (aValue)
   {
      mCapabilityRecord |= cFUEL_SUPPLY_MASK;
   }
   else
   {
      mCapabilityRecord &= ~cFUEL_SUPPLY_MASK;
   }
}
void DisCapabilities::SetMaySupplyVehicleRecovery(bool aValue)
{
   if (aValue)
   {
      mCapabilityRecord |= cRECOVERY_SUPPLY_MASK;
   }
   else
   {
      mCapabilityRecord &= ~cRECOVERY_SUPPLY_MASK;
   }
}
void DisCapabilities::SetMaySupplyVehicleRepair(bool aValue)
{
   if (aValue)
   {
      mCapabilityRecord |= cREPAIR_SUPPLY_MASK;
   }
   else
   {
      mCapabilityRecord &= ~cREPAIR_SUPPLY_MASK;
   }
}
void DisCapabilities::SetMaySupplyADS_B(bool aValue)
{
   if (aValue)
   {
      mCapabilityRecord |= cADS_B_SUPPLY_MASK;
   }
   else
   {
      mCapabilityRecord &= ~cADS_B_SUPPLY_MASK;
   }
}
