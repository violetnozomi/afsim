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

#ifndef OUTPUTCONFIG_HPP
#define OUTPUTCONFIG_HPP

#include <list>
#include <string>

class UtInput;

#include "Constants.hpp"
#include "EventItem.hpp"
#include "OutputItem.hpp"
#include "OutputRate.hpp"

namespace engage
{

//! This holds the input data for the 'output' block.
class OutputConfig
{
public:
   typedef std::list<OutputItem> ItemList;
   typedef std::list<EventItem>  EventItems;

   //! Output format for the slot
   enum Format
   {
      cMINIMAL, //!< ESAMS minimal format
      cBLOCK,   //!< ESAMS block format
      cCOLUMN   //!< ESAMS column format
   };

   OutputConfig();

   bool ProcessInput(UtInput& aInput);

   void SetFileName(const std::string& aFileName) { mFileName = aFileName; }
   void SetFormat(Format aFormat) { mFormat = aFormat; }
   void SetPhase(Phase aPhase) { mPhase = aPhase; }
   void SetEventOutput(bool aEventOutput) { mEventOutput = aEventOutput; }
   void SetSummaryOutput(bool aSummaryOutput) { mSummaryOutput = aSummaryOutput; }
   void SetRateTableName(const std::string& aRateTableName) { mRateTableName = aRateTableName; }
   void SetRateTable(const OutputRate& aRateTable) { mRateTable = aRateTable; }
   void AddOutputItem(const OutputItem& aItem) { mOutputItems.push_back(aItem); }

   std::string       GetFileName() const { return mFileName; }
   Format            GetFormat() const { return mFormat; }
   Phase             GetPhase() const { return mPhase; }
   bool              DoEventOutput() const { return mEventOutput; }
   bool              DoSummaryOutput() const { return mSummaryOutput; }
   std::string       GetRateTableName() const { return mRateTableName; }
   const ItemList&   GetOutputItems() const { return mOutputItems; }
   const EventItems& GetEventItems() const { return mEventItems; }

   bool DoPeriodicOutput(Phase aPhase, double aSimTime, double aLastOutputTime) const;

private:
   std::string mFileName;
   Format      mFormat;
   Phase       mPhase;
   bool        mEventOutput;
   bool        mSummaryOutput;
   std::string mRateTableName;
   OutputRate  mRateTable;
   ItemList    mOutputItems;
   EventItems  mEventItems;
};

} // namespace engage

#endif
