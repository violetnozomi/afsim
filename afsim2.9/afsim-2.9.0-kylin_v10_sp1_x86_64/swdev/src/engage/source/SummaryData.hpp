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

#ifndef SUMMARYDATA_HPP
#define SUMMARYDATA_HPP

#include <sstream>
#include <vector>


class WsfWeaponEngagement;

namespace engage
{

class InputConfig;

//! This class holds summary data from the runs.
class SummaryData
{
public:
   SummaryData();

   void EngagementResult(double aSimtime, const WsfWeaponEngagement* aEngagementPtr);

   int    mShotCount;          //!< The total number of shots taken
   int    mNonZeroPkShotCount; //!< The number of shots with a non-zero Pk.
   double mNonZeroPkSum;       //!< The sum of the non-zero Pk's
};

class RunSummary
{
public:
   typedef SummaryData SiteSummary;

   std::vector<SiteSummary> mSiteSummary;
};

} // namespace engage

#endif
