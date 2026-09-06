// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef EVENTTYPE_HPP
#define EVENTTYPE_HPP

#include <parseAfperf.hpp>

namespace artificer
{
// Note: Duplicated from Afperf.cpp
enum class V1RecordType
{
   RESERVED,
   MeasurementType,
   PauseResume,
   RegionAggregate,
   RegionPoint,
   RegionStart,
   RegionStop,
   RunAggregate,
   RunInfo,
   RunPoint,
   SectionAggregate,
   SectionInfo,
   SectionPoint,
   SectionStart,
   SectionStop,
};

///@brief Returns the human-readable type string defined in the afperf spec for the given `aType`
const std::string& ToTypeString(artificer::V1RecordType aType);
///@brief Returns the minimum length of the CSV record for the given `aType`
std::size_t MinimumRecordFields(V1RecordType aType);
///@brief Returns the event type from the given `aRecord`
V1RecordType RecordTypeFrom(const artificer::AfperfRecord& aRecord);

} // namespace artificer

#endif
