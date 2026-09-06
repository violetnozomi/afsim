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

#ifndef FIRES_TABLE_HPP
#define FIRES_TABLE_HPP

#include "wsf_fires_export.h"

#include <string>

#include "FiresTableLookup.hpp"
#include "TblLookup.hpp"

namespace Fires
{

//! A class used to store discrete range, max. ordinate (max height), and propagation time
//! for combinations of launchers and projectiles.  These tables are for unguided (ballistic)
//! projectiles only.
class WSF_FIRES_EXPORT FiresTable
{
public:
   FiresTable();
   virtual ~FiresTable();

   //! Get to the table lookup context for looking up values in the table.
   //!
   //! The returned table lookup context can be used to extract values from the table.
   //! The context has been optimized to choose the best method based on the size and
   //! spacing of the independent values.
   //!
   //! @param aContext [output] The context to be used for performing table lookups.
   //!
   //! @note This can be called only after Validate() has been called.
   void GetContext(FiresTableLookup& aContext) const { aContext = mContext; }

   void               SetName(const std::string& aName) { mName = aName; }
   const std::string& GetName() const { return mName; }
   double             GetMinimumRange() const;
   double             GetMaximumRange() const;

   void GetMaxOrdAndTOF(double aRange, double& aMaxOrd, double& aTOF);

   void GetElevationAngleAndTOF(double aRange, double& aElevationAngle, double& aTOF);

   TblIndVarU<double> mRangeValues;     //<! range independent values
   TblDepVar1<double> mMaxOrdValues;    //<! dependent value; max. ordinate (max height).
   TblDepVar1<double> mElevationValues; //<! Alternate dependent value; elevation angle
   TblDepVar1<double> mTimeOfFlight;    //<! dependent valud; time of flight
   bool               mUseMaxOrd;

private:
   FiresTableLookup mContext; //<! Prototype table lookup context
   std::string      mName;
};

}; // namespace Fires

#endif
