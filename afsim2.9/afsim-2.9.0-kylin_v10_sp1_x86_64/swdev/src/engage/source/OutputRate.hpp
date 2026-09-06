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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef OUTPUTRATE_HPP
#define OUTPUTRATE_HPP

#include <vector>

class UtInput;
class UtInputBlock;

namespace engage
{

//! Defines a table of simulation output rate as a function of simulation time.
class OutputRate
{
public:
   struct Item
   {
      Item(double aTime, double aPeriod)
         : mTime(aTime)
         , mPeriod(aPeriod)
      {
      }

      double mTime;
      double mPeriod;
   };

   //! Append an entry to the rate table.
   //! The caller is responsible for ensuring the resulting table is valid.
   void Append(double aTime, double aPeriod) { mRateTable.push_back(Item(aTime, aPeriod)); }

   double GetPeriod(double aSimTime) const;

   void ProcessInputBlock(UtInputBlock& aInputBlock);

private:
   std::vector<Item> mRateTable;
};

} // namespace engage

#endif
