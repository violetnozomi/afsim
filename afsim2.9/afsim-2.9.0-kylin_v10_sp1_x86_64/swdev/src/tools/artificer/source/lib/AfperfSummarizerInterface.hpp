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
#ifndef AFPERFSUMMARIZERINTERFACE_HPP
#define AFPERFSUMMARIZERINTERFACE_HPP

#include "artificer_export.h"

#include "parseAfperf.hpp"

namespace artificer
{
/// @brief The interface that a object that receives records from an Afperf input must implement
class ARTIFICER_EXPORT AfperfSummarizerInterface
{
public:
   virtual ~AfperfSummarizerInterface() = default;

   ///@brief Receives records from the input as they are parsed.
   virtual void Summarize(const AfperfRecord& aRecord) = 0;

   ///@brief Notifies this object that the input has ended and it should output any final summaries it's been computing.
   virtual void EndOfRecords() {}
};
} // namespace artificer

#endif
