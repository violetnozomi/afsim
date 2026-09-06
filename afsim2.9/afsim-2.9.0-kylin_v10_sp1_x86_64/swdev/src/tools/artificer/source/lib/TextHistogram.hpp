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
#ifndef TEXTHISTOGRAM_HPP
#define TEXTHISTOGRAM_HPP

#include <cstdint>
#include <queue>
#include <string>
#include <vector>

namespace artificer
{
using HistogramRow = std::pair<int64_t, std::string>;

class TextHistogram
{
public:
   TextHistogram(std::vector<HistogramRow> aRows);

   ///@brief Renders this object as a string and returns it.
   std::string Render(const size_t aMaxRows = 10) const;

private:
   std::vector<HistogramRow> mRows;
};

///@brief Renders the data as a text histgram and streams it.
std::ostream& operator<<(std::ostream&, const TextHistogram&);

} // namespace artificer
#endif
