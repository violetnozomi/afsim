// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "UtQtColumnSettings.hpp"

#include <algorithm>

#include <QTreeView>

UtQtColumnSettings::UtQtColumnSettings() {}

void UtQtColumnSettings::Save(QTreeView& aView)
{
   if (!aView.model())
      return;
   int columns = aView.model()->columnCount();
   int newSize = std::max((int)mColumnSizes.size(), columns);
   mColumnSizes.resize(newSize);
   for (int i = 0; i < columns; ++i)
   {
      mColumnSizes[i] = aView.columnWidth(i);
   }
}

void UtQtColumnSettings::Load(QTreeView& aView)
{
   if (!aView.model())
      return;
   size_t modelColumns = (size_t)aView.model()->columnCount();
   for (size_t i = 0; i < mColumnSizes.size() && i < modelColumns; ++i)
   {
      aView.setColumnWidth((int)i, mColumnSizes[i]);
   }
}
