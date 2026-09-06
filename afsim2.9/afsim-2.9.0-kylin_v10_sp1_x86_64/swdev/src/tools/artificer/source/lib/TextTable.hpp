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
#ifndef TEXTTABLE_HPP
#define TEXTTABLE_HPP

#include "artificer_export.h"

#include <array>
#include <cstddef>
#include <string>
#include <vector>

namespace artificer
{
class ARTIFICER_EXPORT TextTable
{
public:
   using TableRow  = std::vector<std::string>;
   using TableData = std::vector<TableRow>;
   struct Column
   {
      Column(std::string aLabel, std::size_t aWidth)
         : label(aLabel)
         , width(aWidth)
      {
      }
      std::string label;
      std::size_t width = 0;
   };
   TextTable(std::vector<Column> aColumns, TableData aRows);

   ///@brief Renders this object as a string and returns it.
   std::string Render() const;

   ///@brief Return the number of rows contained in the TextTable
   std::size_t RowCount() const { return mRows.size(); };

private:
   std::string RenderRow(const TableRow& aRow) const;
   std::string RenderHeaderRow() const;
   std::string RenderHorizontalDivider() const;

   std::vector<Column> mColumns;
   TableData           mRows;
};

///@brief Renders the text table as a string and streams it.
std::ostream& operator<<(std::ostream&, const TextTable&);

} // namespace artificer
#endif
