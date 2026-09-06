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

#ifndef MTT_MEASUREMENT_HPP
#define MTT_MEASUREMENT_HPP

#include "SupBlock.hpp"

//! MTT_Measurement corresponds with a suppressor block 359 structure
//! (but, it is not byte-equivalent).
class MTT_Measurement
{
public:
   // Base class for measurement "data"
   class Data
   {
   public:
      virtual Data* Clone() const = 0;
      virtual ~Data()             = default;
   };


public:
   MTT_Measurement() = default;

   MTT_Measurement(const MTT_Measurement& aSrc)
      : mDataPtr(nullptr)
      , mDataAvailable(aSrc.mDataAvailable)
   {
      if (aSrc.mDataPtr != nullptr)
      {
         mDataPtr = aSrc.mDataPtr->Clone();
      }
   }
   MTT_Measurement& operator=(const MTT_Measurement&) = delete;

   virtual ~MTT_Measurement() { delete mDataPtr; }

   virtual MTT_Measurement* Clone() const { return new MTT_Measurement(*this); }

   void ConvertFrom(const SupBlock& aBlock) { mDataAvailable = aBlock.mIntBlock[6]; }

   Data*    mDataPtr{nullptr}; // +4
   unsigned mDataAvailable{0}; // +6  (ijklmn bitfields for validity)
};

#endif
