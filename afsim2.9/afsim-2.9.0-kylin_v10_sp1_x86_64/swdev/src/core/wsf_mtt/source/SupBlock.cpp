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

#include "SupBlock.hpp"

#include <iostream>

#include "GenI.hpp"
#include "GenO.hpp"
#include "UtLog.hpp"

// static
std::map<unsigned, unsigned> SupBlock::mBlockNumberToSizeMap;
bool                         SupBlock::mInitializeOnce = true;

SupBlock::SupBlock()
   : mIntBlock(nullptr)
   , mShared(false)
{
}

SupBlock::SupBlock(long* aBlockPtr, unsigned aBlockNumber)
   : mBlockNumber(aBlockNumber)
   , mIntBlock(aBlockPtr)
   , mShared(true)
{
   Initialize();
   std::map<unsigned, unsigned>::iterator iter = mBlockNumberToSizeMap.find(aBlockNumber);
   if (iter != mBlockNumberToSizeMap.end())
   {
      mSize = iter->second;
   }
   else
   {
      auto logger = ut::log::info() << "Size not found for block.";
      logger.AddNote() << "Block Number: " << aBlockNumber;
   }
}

void SupBlock::Initialize()
{
   if (mInitializeOnce)
   {
      mBlockNumberToSizeMap[17]  = 45;
      mBlockNumberToSizeMap[359] = 7;
      mBlockNumberToSizeMap[360] = 20;
      mBlockNumberToSizeMap[363] = 10;
      mBlockNumberToSizeMap[367] = 184;
      mBlockNumberToSizeMap[383] = 7;
      mBlockNumberToSizeMap[384] = 8;
      mBlockNumberToSizeMap[385] = 52;
      mBlockNumberToSizeMap[386] = 52;
      mBlockNumberToSizeMap[387] = 184;
      mBlockNumberToSizeMap[388] = 35;

      // Add relevant block entries:
      mInitializeOnce = false;
   }
}

SupBlock::SupBlock(unsigned aBlockNumber)
   : mBlockNumber(aBlockNumber)
   , mCharBlock(nullptr)
   , mShared(false)
{
   Initialize();

   std::map<unsigned, unsigned>::iterator iter = mBlockNumberToSizeMap.find(aBlockNumber);
   if (iter != mBlockNumberToSizeMap.end())
   {
      mSize      = iter->second;
      mCharBlock = new char[mSize * 4];
   }
   else
   {
      auto logger = ut::log::info() << "Size not found for block type.";
      logger.AddNote() << "Block Number: " << aBlockNumber;
   }
}

void SupBlock::ResetState()
{
   mBlockNumberToSizeMap.clear();
   mInitializeOnce = true;
}
SupBlock::~SupBlock()
{
   if (!mShared)
   {
      delete[] mCharBlock;
   }
}

// friend
GenO& operator<<(GenO& aOut, const SupBlock& aBlock)
{
   aOut << aBlock.mBlockNumber;
   aOut << aBlock.mSize;
   aOut.PutArray(aBlock.mCharBlock, aBlock.mSize * 4);
   return aOut;
}

// friend
GenI& operator>>(GenI& aIn, SupBlock& aBlock)
{
   if ((!aBlock.mShared) && (aBlock.mCharBlock != nullptr))
   {
      delete[] aBlock.mCharBlock;
      aBlock.mCharBlock = nullptr;
   }
   aIn >> aBlock.mBlockNumber;
   aIn >> aBlock.mSize;
   aBlock.mCharBlock = new char[aBlock.mSize * 4];
   aIn.GetArray(aBlock.mCharBlock, aBlock.mSize * 4);
   return aIn;
}
