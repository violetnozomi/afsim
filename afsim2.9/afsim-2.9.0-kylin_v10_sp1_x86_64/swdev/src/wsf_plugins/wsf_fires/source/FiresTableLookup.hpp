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

#ifndef FIRES_TABLE_LOOKUP_HPP
#define FIRES_TABLE_LOOKUP_HPP

#include "wsf_fires_export.h"

#include "TblLookup.hpp"

namespace Fires
{
class FiresTable;

//! This class uses a FiresTable to lookup both a max ord and
//! time of flight, given a range to target.
class WSF_FIRES_EXPORT FiresTableLookup
{
public:
   FiresTableLookup();
   ~FiresTableLookup();

   void Lookup(double aRange, double& aMaxOrd, double& aImpactTime);

   void LookupElevation(double aRange, double& aElevation, double& aImpactTime);

   void SetTable(FiresTable* aTablePtr);

private:
   FiresTable*          mTablePtr;
   TblLookupLUX<double> mRangeLookup;
};
}; // namespace Fires

#endif
