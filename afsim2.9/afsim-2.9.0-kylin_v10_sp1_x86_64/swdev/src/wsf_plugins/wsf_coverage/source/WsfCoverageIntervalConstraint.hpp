// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOVERAGEINTERVALCONSTRAINT_HPP
#define WSFCOVERAGEINTERVALCONSTRAINT_HPP

#include <string>
#include <vector>

#include "UtCloneablePtr.hpp"
class UtInput;

#include "wsf_coverage_export.h"

#include "WsfCoverageRawDataOutput.hpp"

namespace wsf
{
namespace coverage
{

class AccessInterval;
class Coverage;
class FreeAsset;
class GridAsset;

//! Provides access interval duration constraints.
//!
//! This class implements constraints on the duration of access intervals.
//! The owning Coverage object will filter all complete access intervals through
//! this constraint to decide if the interval meets the specified duration limits.
//! The limits can be specified for platforms of a given name or type, or devices
//! of a given name or type. Additionally, instances can be configured to write
//! any filtered intervals out to file.
class WSF_COVERAGE_EXPORT IntervalConstraint
{
public:
   IntervalConstraint()                                 = default;
   IntervalConstraint(const IntervalConstraint& aOther) = default;
   ~IntervalConstraint()                                = default;

   IntervalConstraint* Clone() const { return new IntervalConstraint{*this}; }
   bool                ProcessInput(UtInput& aInput);

   //! Initialize this object.
   void Initialize(const Coverage& aCoverage, const std::string& aOutputDir);

   //! Finalize the constraint.
   //!
   //! This will write out any filtered intervals if this object was so configured.
   void Finalize(const UtCalendar& aSimStartEpoch) const;

   //! Determine if the given interval should be filtered.
   //!
   //! Determined if the given @p aInterval involving the given @p aGridAsset and given
   //! @p aFreeAsset should be ignored in the coverage computation.
   //!
   //! \param aGridAsset - The grid asset involved in the interval in question.
   //! \param aFreeAsset - The free asset involved in the interval in question.
   //! \param aInterval  - The interval in question.
   //! \returns          - true if the interval should be removed from the coverage computation.
   bool FilterInterval(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, const AccessInterval& aInterval) const;

   //! Return the filename into which the filtered intervals will be written.
   //!
   //! This will return either a default constructed string if this object was not
   //! configured to produce an output file with the filtered intervals, or the name
   //! of the file into which that data will be written.
   //!
   //! \returns - The name of the file into which the filtered intervals will be written,
   //!            or an empty string if not configured for output.
   std::string GetOutputFileName() const;

private:
   //! The Filter class implements filtering for a single specification.
   class Filter
   {
   public:
      bool ProcessInput(UtInput& aInput);

      bool FilterInterval(const IntervalConstraint& aConstraint,
                          const GridAsset&          aGridAsset,
                          const FreeAsset&          aFreeAsset,
                          const AccessInterval&     aInterval) const;

   private:
      enum class ConstraintType
      {
         cPLATFORM_NAME,
         cPLATFORM_TYPE,
         cDEVICE_NAME,
         cDEVICE_TYPE,
         cUNKNOWN
      };

      void ReadLimits(UtInput& aInput);
      bool MeetsConstraint(double aDuration) const;

      ConstraintType mType{ConstraintType::cUNKNOWN};
      std::string    mIdentifier{};
      double         mMinimum{-1.0};
      double         mMaximum{-2.0};
   };

   bool IsPlatformOfType(const std::string& aPlatformName, const std::string& aPlatformType) const;
   bool IsPlatformDeviceOfType(const std::string& aPlatformName,
                               const std::string& aDeviceName,
                               const std::string& aDeviceType) const;

   //! The owning Coverage object. This is only set afer the call to Initialize.
   const Coverage* mCoveragePtr{nullptr};

   //! The resource for performing output of filtered intervals to file.
   ut::CloneablePtr<RawDataOutput> mFilteredIntervalsPtr{nullptr};

   //! The set of Filters defined for this object.
   std::vector<Filter> mFilters{};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEINTERVALCONSTRAINT_HPP
