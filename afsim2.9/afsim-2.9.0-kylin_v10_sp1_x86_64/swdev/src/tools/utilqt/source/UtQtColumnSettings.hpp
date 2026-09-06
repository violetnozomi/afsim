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
#ifndef UTQTCOLUMNSETTINGS_HPP
#define UTQTCOLUMNSETTINGS_HPP

#include "utilqt_export.h"

#include <vector>

class QTreeView;
//! If a model is cleared or a column is removed, the view loses information about column sizes.
//! This class saves and restores column sizes to preserve user changes.
class UTILQT_EXPORT UtQtColumnSettings
{
public:
   UtQtColumnSettings();

   void Save(QTreeView& aView);
   void Load(QTreeView& aView);

   std::vector<int> mColumnSizes;
};

#endif
