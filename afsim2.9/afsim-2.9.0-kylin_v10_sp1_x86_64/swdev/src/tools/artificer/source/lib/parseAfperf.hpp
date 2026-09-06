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
#ifndef PARSEAFPERF_HPP
#define PARSEAFPERF_HPP

#include <functional>
#include <string>
#include <vector>

namespace artificer
{
///@brief Represents a general afperf file value, equivalent to a csv value.
using AfperfValue = std::string;

///@brief Represents a general afperf file record, equivalent to a csv row.
using AfperfRecord = std::vector<AfperfValue>;

///@brief Reads the header from an afperf file, exactly 16 bytes, and returns them.
std::string ReadAfperfHeader(std::istream& aData);

} // namespace artificer

#endif
