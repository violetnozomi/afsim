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

#ifndef SUPBLOCK_HPP
#define SUPBLOCK_HPP

#include <map>

class GenI;
class GenO;

class SupBlock
{
public:
   SupBlock(unsigned aBlockNumber); // Creates a new block.
   SupBlock(long*    aBlockPtr,
            unsigned aBlockNumber); // Shares a block.
   SupBlock();
   void        Initialize();
   static void ResetState();

   ~SupBlock();

   friend GenO& operator<<(GenO& aOut, const SupBlock& aBlock);

   friend GenI& operator>>(GenI& aIn, SupBlock& aBlock);

   //   private:
   // Allow easy access to representation

   unsigned mBlockNumber;
   unsigned mSize;
   union
   {
      char*   mCharBlock;
      long*   mIntBlock;
      float*  mFloatBlock;
      double* mDoubleBlock;
   };

   bool mShared;

   static std::map<unsigned, unsigned> mBlockNumberToSizeMap;
   static bool                         mInitializeOnce;
};

// typedefs

#endif
