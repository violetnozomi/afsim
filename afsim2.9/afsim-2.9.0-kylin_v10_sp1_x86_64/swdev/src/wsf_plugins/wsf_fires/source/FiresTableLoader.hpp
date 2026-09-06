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

#ifndef FIRESTABLELOADER_HPP
#define FIRESTABLELOADER_HPP

#include <string>
#include <vector>

class UtInput;

namespace Fires
{
class FiresTable;

//! A  utility class for loading a FiresTable from a UtInput object.
//!
//! This could have been put directly in FiresTable, but it was decided to
//! keep it separate as the FiresTable is a nice compact class that has
//! some general utility.  This class, on the other hand, could get messy if
//! the ability to read different signature tables is added.  In order
//! to use these tables, one must be able to look up the (launcher) system as well as
//! munition type.  In DIS, one would look up the ammunition enumeration and the
//! firing platform entity number.
class FiresTableLoader
{
public:
   static FiresTable* ProcessTable(UtInput& aInput);

private:
   static void LoadFiresTable(UtInput& aInput, FiresTable*& aTablePtr);

   static void LoadFiresElevationAngleTable(UtInput& aInput, FiresTable*& aTablePtr);

   static void LoadFiresMaximumOrdinateTable(UtInput& aInput, FiresTable*& aTablePtr);

   static void GetUnits(UtInput& aInput, std::string& aUnits);
};
}; // namespace Fires

#endif
