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
#ifndef L16_SLOTPARAMETERS_HPP
#define L16_SLOTPARAMETERS_HPP

class UtInput;
class WsfJTIDS_SlotGroup;

namespace WsfL16
{
class SlotParameters
{
public:
   SlotParameters();
   bool ProcessInput(UtInput& aInput);
   void Set(WsfJTIDS_SlotGroup& aSlotGroup, int aSlotNumber);

   int mNPG;
   int mSlotNumber;
   int mNetNumber;
   int mTSEC;
   int mMSEC;
};
} // namespace WsfL16
#endif
