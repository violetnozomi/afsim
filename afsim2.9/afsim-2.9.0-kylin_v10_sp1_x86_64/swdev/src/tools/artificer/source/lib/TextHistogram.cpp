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
#include "TextHistogram.hpp"

#include <algorithm>
#include <sstream>

#include "TextTable.hpp"


namespace artificer
{
TextHistogram::TextHistogram(std::vector<HistogramRow> aRows)
{
   std::sort(aRows.begin(),
             aRows.end(),
             [](const HistogramRow& aVal1, const HistogramRow& aVal2) { return aVal1.first > aVal2.first; });
   mRows.swap(aRows);
}

std::string TextHistogram::Render(const size_t aMaxRows) const
{
   if (mRows.empty())
   {
      std::stringstream table;
      table << "No data." << std::endl;
      return table.str();
   }
   const size_t         maxBarLength = 39;
   const auto           maxValue     = mRows[0].first;
   TextTable::TableData tableData;

   for (size_t curRow = 0; curRow < aMaxRows && curRow < mRows.size(); ++curRow)
   {
      const size_t      barSize = maxValue == 0 ? 0 : maxBarLength * mRows[curRow].first / maxValue;
      const std::string bar(barSize, '#');
      const std::string label = "(" + std::to_string(mRows[curRow].first) + ")" + mRows[curRow].second;
      tableData.push_back({label, bar});
   }

   return TextTable({{"(Count)Label", 38}, {"Histogram", maxBarLength}}, tableData).Render();
}

std::ostream& operator<<(std::ostream& aStream, const TextHistogram& aHisto)
{
   aStream << aHisto.Render() << std::endl;
   return aStream;
}

} // namespace artificer
