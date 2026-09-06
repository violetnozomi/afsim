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

#include "FiresTable.hpp"

#include <iostream>

#include "UtEarth.hpp"
#include "UtMath.hpp"

namespace Fires
{

FiresTable::FiresTable()
   : mUseMaxOrd(true)
{
}

// virtual
FiresTable::~FiresTable() {}

double FiresTable::GetMinimumRange() const
{
   return mRangeValues.GetValues()[0];
}

double FiresTable::GetMaximumRange() const
{
   return mRangeValues.GetValues()[mRangeValues.GetValues().size() - 1];
}

//! Return the "max. ord.," or maximum height of the trajectory, and time of flight, for a given weapon type
//! and horizontal range to target.
//! @param aRange The desired horizontal range of the flyout.
//! @param aMaxOrd (output) The max. ord. (max. height) for the trajectory.
//! @param aTOF (output) The time of flight of the round.
//! @return Whether an acceptable solution was computed.
void FiresTable::GetMaxOrdAndTOF(double aRange, double& aMaxOrd, double& aTOF)
{
   FiresTableLookup lookup;
   lookup.SetTable(this);
   lookup.Lookup(aRange, aMaxOrd, aTOF); // load max height and impact time from table.
}

void FiresTable::GetElevationAngleAndTOF(double aRange, double& aElevationAngle, double& aTOF)
{
   FiresTableLookup lookup;
   lookup.SetTable(this);
   lookup.LookupElevation(aRange, aElevationAngle, aTOF);
}

} // namespace Fires
