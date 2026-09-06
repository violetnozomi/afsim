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

#include "TextTable.hpp"

#include <sstream>


namespace artificer
{
TextTable::TextTable(std::vector<Column> aColumns, TableData aRows)
   : mColumns(std::move(aColumns))
   , mRows(std::move(aRows))
{
}

std::string TextTable::Render() const
{
   if (mColumns.size() < 1)
   {
      throw std::runtime_error("No columns in table.");
   }
   const std::string divider = RenderHorizontalDivider();
   std::stringstream stream;
   stream << divider << std::endl << RenderHeaderRow() << std::endl << divider << std::endl;
   for (const TableRow& row : mRows)
   {
      stream << RenderRow(row) << std::endl;
   }
   stream << divider << std::endl;
   return stream.str();
}

std::string TextTable::RenderHeaderRow() const
{
   std::stringstream render;
   render << "|";
   for (std::size_t col = 0; col < mColumns.size(); ++col)
   {
      std::size_t remaining = mColumns[col].width;
      render << mColumns[col].label.substr(0, remaining);
      remaining -= std::min(mColumns[col].label.size(), remaining);
      render << std::string(remaining, ' ') << "|";
   }
   return render.str();
}

std::string TextTable::RenderRow(const TableRow& aRow) const
{
   std::stringstream render;
   render << "|";
   for (std::size_t col = 0; col < mColumns.size(); ++col)
   {
      std::size_t remaining = mColumns[col].width;
      if (col < aRow.size())
      {
         render << aRow[col].substr(0, remaining);
         remaining -= std::min(aRow[col].size(), remaining);
      }
      render << std::string(remaining, ' ') << "|";
   }
   return render.str();
}

std::string TextTable::RenderHorizontalDivider() const
{
   std::stringstream divider;
   divider << "+";
   for (const auto& col : mColumns)
   {
      divider << std::string(col.width, '-') << "+";
   }
   return divider.str();
}


std::ostream& operator<<(std::ostream& stream, const TextTable& table)
{
   stream << table.Render() << std::endl;
   return stream;
}

} // namespace artificer
