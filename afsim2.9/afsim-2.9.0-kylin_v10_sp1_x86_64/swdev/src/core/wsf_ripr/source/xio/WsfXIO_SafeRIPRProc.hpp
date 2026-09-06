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
#ifndef WSFXIO_SAFE_RIPR_PROCESSOR_HPP_2009_12_09
#define WSFXIO_SAFE_RIPR_PROCESSOR_HPP_2009_12_09

#include "WsfStringId.hpp"
#include "WsfXIO_RIPRSerializeTypes.hpp"

class WsfRIPRProcessor;
class SafeRIPRProc;

//! The WsfXIO_SafeRiprProc is a SafeRiprProc designed for serialization, and
//! other purposes that break the intent of the SafeRiprProc.
class WsfXIO_SafeRIPRProc
{
public:
   WsfXIO_SafeRIPRProc(const SafeRIPRProc& aProc);
   WsfXIO_SafeRIPRProc(const WsfRIPRProcessor* aProc);
   WsfXIO_SafeRIPRProc()
      : mPlatformIndex(-1)
      , mProcessorName(0)
   {
   }
   WsfXIO_SafeRIPRProc(const WsfXIO_SafeRIPRProc& aRhs)
      : mPlatformIndex(aRhs.mPlatformIndex)
      , mProcessorName(aRhs.mProcessorName)
   {
   }

   WsfXIO_SafeRIPRProc& operator=(const WsfXIO_SafeRIPRProc& aRhs)
   {
      mPlatformIndex = aRhs.mPlatformIndex;
      mProcessorName = aRhs.mProcessorName;
      return *this;
   }
   int         GetPlatformIndex() const { return mPlatformIndex; }
   WsfStringId GetProcessorName() const { return mProcessorName; }

   //! Serialize implements the capability to pack and un-pack across the network.
   template<typename T>
   void Serialize(T& aBuff)
   {
      using namespace PakSerialization;
      aBuff& mPlatformIndex& mProcessorName;
   }

private:
   int         mPlatformIndex;
   WsfStringId mProcessorName;
};
#endif
