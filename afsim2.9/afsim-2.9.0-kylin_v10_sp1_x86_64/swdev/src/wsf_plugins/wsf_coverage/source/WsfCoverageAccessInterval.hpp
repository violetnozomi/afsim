// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOVERAGEACCESSINTERVAL_HPP
#define WSFCOVERAGEACCESSINTERVAL_HPP

#include "wsf_coverage_export.h"

namespace wsf
{
namespace coverage
{

//! Representation of the simulation times during which an access occurs.
//!
//! This object uses simulation time to mark the bounds of the interval, so
//! a negative start or end time is invalid, and is used to indicate that
//! those values have not been set.
class WSF_COVERAGE_EXPORT AccessInterval
{
public:
   AccessInterval() = default;

   AccessInterval(double aStart, double aEnd)
      : mStart{aStart}
      , mEnd{aEnd}
   {
   }

   AccessInterval(const AccessInterval& aOther) = default;

   AccessInterval& operator=(const AccessInterval& aOther) = default;

   //! Return the start time of the interval.
   double GetStart() const { return mStart; }

   //! Return the end time of the interval.
   double GetEnd() const { return mEnd; }

   //! Get the duration of the interval.
   double GetDuration() const { return mEnd - mStart; }

   //! Returns true if this interval have a non-negative duration.
   bool IsValid() const { return mEnd >= mStart; }

   //! Returns true if this interval has a non-negative start time.
   bool HasStart() const { return mStart >= 0.0; }

   //! Returns true if this interval has a non-negative end time.
   bool HasEnd() const { return mEnd >= 0.0; }

   //! Set the start time to the given @p aSimTime.
   void SetStart(double aSimTime) { mStart = aSimTime; }

   //! Set the end time to the given @p aSimTime.
   void SetEnd(double aSimTime) { mEnd = aSimTime; }

   //! Equality operator.
   //!
   //! Intervals are equal if their start and end points are the same.
   bool operator==(const AccessInterval& aOther) const;

   //! Negated equality operator.
   bool operator!=(const AccessInterval& aOther) const { return !(*this == aOther); }

   //! Predicate to determine if this interval overlaps the given @p aOther.
   bool HasOverlapWith(const AccessInterval& aOther) const;

   //! Return the union of this interval with the given @p aOther.
   //!
   //! If the given @p aOther interval does not overalap with this
   //! interval, then this will return an invalid interval.
   AccessInterval UnionWith(const AccessInterval& aOther) const;

   //! Return the intersection of this interval with the given @p aOther.
   //!
   //! If the given @p aOther interval does not overalap with this
   //! interval, then this will return an invalid interval.
   AccessInterval IntersectionWith(const AccessInterval& aOther) const;

   //! Return true if the given @p aSimTime is inside this interval.
   bool Contains(double aSimTime) const;

private:
   double mStart{-1.0}; //! Start sim time of the interval.
   double mEnd{-2.0};   //! End sim time of the interval.
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEACCESSINTERVAL_HPP
