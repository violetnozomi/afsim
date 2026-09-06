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

#ifndef WSFCOVERAGEMEASUREUTILS_HPP
#define WSFCOVERAGEMEASUREUTILS_HPP

#include <string>
#include <vector>

#include "WsfCoverageAccessInterval.hpp"

class UtCalendar;

namespace wsf
{
namespace coverage
{

class AccessInterval;

namespace MeasureUtils
{

//! Compute the gap intervals from the coverage intervals.
//!
//! The complement of the access @p aIntervals in the coverage interval are the
//! gap intervals. This routine will return the gaps given the access @p aIntervals.
//! The provided intervals should be non-overlapping: no interval should overlap any
//! other interval. The input @p aIntervals does not have to be time-ordered.
//!
//! \param aGaps              A reference to an empty vector that will be used to store the gap
//!                           intervals. This is an output parameter.
//! \param aIntervals         A set of access intervals.
//! \param aCoverageInterval  The overall coverage interval which specifies the beginning and end
//!                           of the relevant time span.
//! \param aMoE_Type          A string reference that describes the MoE type, i.e. "Simple Coverage MoE".
//! \param aMoE_ObjectName    The name of the MoE WsfObject. When in doubt, just use GetName().
void ComputeGaps(std::vector<AccessInterval>& aGaps,
                 std::vector<AccessInterval>& aIntervals,
                 const AccessInterval&        aCoverageInterval,
                 const std::string&           aMoE_Type,
                 const std::string&           aMoE_ObjectName);

//! Get the sample on which @p aInterval starts. The sample on which the coverage interval begins is always sample 0.
//! \param aInterval        The AccessInterval for which to find the start sample.
//! \param aStartEpoch      The date and time at which the coverage interval begins.
//! \param aCoverageStart   The simulation time at which the coverage interval begins. When in doubt, use
//! AccessInterval::GetStart(). \param aSampleDuration  The duration of the samples. \return                 The sample
//! on which @p aInterval begins.
long GetIntervalStartSample(const AccessInterval& aInterval,
                            const UtCalendar&     aStartEpoch,
                            double                aCoverageStart,
                            double                aSampleDuration);

//! Get the sample on which @p aInterval ends. The sample on which the coverage interval begins is always sample 0.
//! \param aInterval        The AccessInterval for which to find the end sample.
//! \param aStartEpoch      The date and time at which the coverage interval begins.
//! \param aCoverageStart   The simulation time at which the coverage interval begins. When in doubt, use
//! AccessInterval::GetStart(). \param aSampleDuration  The duration of the samples. \return                 The sample
//! on which @p aInterval ends. If it ends at midnight exactly, then it counts as the sample before.
long GetIntervalEndSample(const AccessInterval& aInterval,
                          const UtCalendar&     aStartEpoch,
                          double                aCoverageStart,
                          double                aSampleDuration);

} // namespace MeasureUtils
} // namespace coverage
} // namespace wsf
#endif // !WSFCOVERAGEMEASUREUTILS_HPP
