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

#include "OutputRate.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"

namespace engage
{

// =================================================================================================
//! Return the output period for the specified simulation time.
//! @param aSimTime The current simulation time.
//! @returns The output period for the current simulation time.
double OutputRate::GetPeriod(double aSimTime) const
{
   // The way ESAMS works, if one has a table defined by (T1,P1), (T2,P2) and (T3,P3),
   // if a time is supplied that exactly matches a boundary then the slot is used.

   double rate(0.5);
   if (aSimTime < mRateTable.front().mTime)
   {
      rate = mRateTable.front().mPeriod;
   }
   else if (aSimTime > mRateTable.back().mTime)
   {
      rate = mRateTable.back().mPeriod;
   }
   else
   {
      // The specified time is within the bounds of the table. Search the table for the correct interval.
      // It is assumed the table is well-formed (monotonically increasing).
      size_t i = 1;
      while (aSimTime > mRateTable[i].mTime)
      {
         ++i;
      }
      rate = mRateTable[i - 1].mPeriod;
   }
   return rate;
}

// =================================================================================================
void OutputRate::ProcessInputBlock(UtInputBlock& aInputBlock)
{
   std::string command;
   bool        periodValid(false);
   while (aInputBlock.ReadCommand(command))
   {
      UtInput& input(aInputBlock.GetInput());
      if (command == "time")
      {
         double time;
         input.ReadValueOfType(time, UtInput::cTIME);
         input.ValueGreaterOrEqual(time, 0.0);
         // If not the first entry, make sure the entries increase monotonically
         if (!mRateTable.empty())
         {
            input.ValueGreater(time, mRateTable.back().mTime);
         }
         mRateTable.push_back(Item(time, 0.0));
         periodValid = true;
      }
      else if (command == "period")
      {
         if (!periodValid)
         {
            throw UtInput::OutOfContext(input);
         }
         double period;
         input.ReadValueOfType(period, UtInput::cTIME);
         input.ValueGreaterOrEqual(period, 0.0);
         mRateTable.back().mPeriod = period;
         periodValid               = false;
      }
      else
      {
         throw UtInput::UnknownCommand(input);
      }
   }

   // Ensure the table is not empty.
   if (mRateTable.empty())
   {
      throw UtInput::BadValue(aInputBlock.GetInput(), "output rate table cannot be empty");
   }

   // Any entry with a period of less than or equal to zero is treated as 'infinite'.
   // We set such entries to 'infinite' here so we don't have to check at runtime.

   for (size_t i = 0; i < mRateTable.size(); ++i)
   {
      if (mRateTable[i].mPeriod <= 0.0)
      {
         mRateTable[i].mPeriod = 1.0E+30;
      }
   }
}

} // namespace engage
