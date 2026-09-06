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

#include "V1RecordType.hpp"

#include <stdexcept>

namespace
{
struct RecordMetadata
{
   RecordMetadata(std::string aLabel, std::size_t aMinLength)
      : label(aLabel)
      , minimumLength(aMinLength)
   {
   }
   std::string label;
   std::size_t minimumLength = 0;
};

static const std::vector<RecordMetadata> kRecordMetadata = {RecordMetadata("RESERVED", 0),
                                                            RecordMetadata("MeasurementType", 9),
                                                            RecordMetadata("PauseResume", 3),
                                                            RecordMetadata("RegionAggregate", 7),
                                                            RecordMetadata("RegionPoint", 5),
                                                            RecordMetadata("RegionStart", 6),
                                                            RecordMetadata("RegionStop", 3),
                                                            RecordMetadata("RunAggregate", 7),
                                                            RecordMetadata("RunInfo", 9),
                                                            RecordMetadata("RunPoint", 5),
                                                            RecordMetadata("SectionAggregate", 8),
                                                            RecordMetadata("SectionInfo", 6),
                                                            RecordMetadata("SectionPoint", 6),
                                                            RecordMetadata("SectionStart", 4),
                                                            RecordMetadata("SectionStop", 4)};
} // namespace

namespace artificer
{
const RecordMetadata& GetMetadata(const artificer::V1RecordType aType)
{
   const std::size_t typeAsInt = static_cast<std::size_t>(aType);
   if (typeAsInt < kRecordMetadata.size())
   {
      return kRecordMetadata[typeAsInt];
   }
   throw std::runtime_error("Invalid event type: " + std::to_string(static_cast<int>(aType)));
}

const std::string& ToTypeString(artificer::V1RecordType aType)
{
   return GetMetadata(aType).label;
}

std::size_t MinimumRecordFields(V1RecordType aType)
{
   return GetMetadata(aType).minimumLength;
}

V1RecordType RecordTypeFrom(const artificer::AfperfRecord& aRecord)
{
   if (aRecord.size() < 1)
   {
      throw std::runtime_error("Record does not contain a type value.");
   }
   for (std::size_t index = 0; index < kRecordMetadata.size(); ++index)
   {
      if (aRecord[0] == kRecordMetadata[index].label)
      {
         return static_cast<V1RecordType>(index);
      }
   }
   try
   {
      return static_cast<V1RecordType>(std::stol(aRecord[0]));
   }
   catch (std::invalid_argument&)
   {
      throw std::runtime_error("Invalid record type: " + aRecord[0]);
   }
}

} // namespace artificer
