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
#ifndef AFPERFPARSERINTERFACE_HPP
#define AFPERFPARSERINTERFACE_HPP

#include <istream>
#include <string>

namespace artificer
{
class AfperfSummarizerInterface;

/// @brief The interface that a parser of a Afperf file format version must implement
class AfperfParserInterface
{
public:
   virtual ~AfperfParserInterface() = default;

   /// @brief Returns true if this object can parse a data file described by the given header.  False otherwise.
   virtual bool CanHandle(const std::string& aHeader) = 0;

   /// @brief Gives the stream to parse and the summarizer object that this object should feed records into.
   virtual void Parse(std::istream& aData, AfperfSummarizerInterface& aSummarizer) = 0;
};

} // namespace artificer

#endif
