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
#ifndef L16_HEADERTDL_HPP
#define L16_HEADERTDL_HPP

#include <cstdint>

#include "GenSwapEndian.hpp"

namespace WsfL16
{
namespace Messages
{
class MessageBitInput;
class MessageBitOutput;

#pragma pack(push, 1)
// Works with TRAP Tool
struct HeaderTDL100
{
   void SwapBigNativeEndian()
   {
      GenSwapEndian::SwapBigNative() & mNPG_Number& mSpare& mTimeSlotId& mTransmitInt& mTransmitFrac;
   }
   uint8_t  mTSec;
   uint8_t  mNetNumber;
   uint16_t mNPG_Number;
   uint16_t mSpare;
   uint8_t  mMessageType;
   uint8_t  mMSec;
   uint32_t mTimeSlotId;
   uint32_t mTransmitInt;
   uint32_t mTransmitFrac;
};
#pragma pack(pop)

#pragma pack(push, 1)
// Matches IWARS9.3.x and down
struct WSF_HeaderTDL100
{
   void SwapBigNativeEndian()
   {
      GenSwapEndian::SwapBigNative() & mNPG_Number& mSpare& mTimeSlotId& mTransmitInt& mTransmitFrac;
   }
   uint16_t mNPG_Number;
   uint8_t  mNetNumber;
   uint8_t  mTSec;
   uint8_t  mMSec;
   uint8_t  mMessageType;
   uint16_t mSpare;
   uint32_t mTimeSlotId;
   uint32_t mTransmitInt;
   uint32_t mTransmitFrac;
};
#pragma pack(pop)
} // namespace Messages
} // namespace WsfL16
#endif
