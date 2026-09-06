// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "SlotParameters.hpp"

#include "UtInput.hpp"
#include "WsfJTIDS_SlotGroup.hpp"

namespace WsfL16
{

SlotParameters::SlotParameters()
   : mNPG(0)
   , mSlotNumber(0)
   , mNetNumber(0)
   , mTSEC(0)
   , mMSEC(0)
{
}

bool SlotParameters::ProcessInput(UtInput& aInput)
{
   if (aInput.GetCommand() == "npg")
   {
      aInput.ReadValue(mNPG);
      aInput.ValueInClosedRange(mNPG, 0, 512);
   }
   else if (aInput.GetCommand() == "slot_number")
   {
      aInput.ReadValue(mSlotNumber);
   }
   else if (aInput.GetCommand() == "network")
   {
      aInput.ReadValue(mNetNumber);
      aInput.ValueInClosedRange(mNetNumber, 0, 127);
   }
   else if (aInput.GetCommand() == "tsec")
   {
      aInput.ReadValue(mTSEC);
      aInput.ValueInClosedRange(mTSEC, 0, 127);
   }
   else if (aInput.GetCommand() == "msec")
   {
      aInput.ReadValue(mMSEC);
      aInput.ValueInClosedRange(mMSEC, 0, 127);
   }
   else
   {
      return false;
   }
   return true;
}

void SlotParameters::Set(WsfJTIDS_SlotGroup& aSlotGroup, int aSlotNumber)
{
   mSlotNumber = aSlotNumber;
   mNPG        = aSlotGroup.GetNPG();
   mTSEC       = aSlotGroup.GetTSEC();
   mMSEC       = aSlotGroup.GetMSEC();
   mNetNumber  = aSlotGroup.GetNetwork();
}

} // namespace WsfL16
