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

#include "FiresTableLookup.hpp"

#include "FiresTable.hpp"

namespace Fires
{

FiresTableLookup::FiresTableLookup()
   : mTablePtr(nullptr)
{
}

FiresTableLookup::~FiresTableLookup() {}

//! Given a range, lookup both max ord and impact time in
//! a fires table.
void FiresTableLookup::Lookup(double aRange, double& aMaxOrd, double& aImpactTime)
{
   double range = aRange;
   mRangeLookup.Lookup(mTablePtr->mRangeValues, range);
   aMaxOrd     = TblEvaluate(mTablePtr->mMaxOrdValues, mRangeLookup);
   aImpactTime = TblEvaluate(mTablePtr->mTimeOfFlight, mRangeLookup);
}

//! Given a range, lookup both max ord and impact time in
//! a fires table.
void FiresTableLookup::LookupElevation(double aRange, double& aElevation, double& aImpactTime)
{
   double range = aRange;
   mRangeLookup.Lookup(mTablePtr->mRangeValues, range);
   aElevation  = TblEvaluate(mTablePtr->mElevationValues, mRangeLookup);
   aImpactTime = TblEvaluate(mTablePtr->mTimeOfFlight, mRangeLookup);
}

//! Set the table for this lookup.
//!@param aTablePtr the table to use for lookup.
void FiresTableLookup::SetTable(FiresTable* aTablePtr)
{
   mTablePtr = aTablePtr;
   mRangeLookup.Reset();
}

} // namespace Fires
