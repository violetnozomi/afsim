// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFMILSTDTREEWIDGET_HPP
#define WKFMILSTDTREEWIDGET_HPP

#include "wkf_export.h"

#include <QTreeWidget>

namespace wkf
{
// This tree widget only allows for drops within the tree widget (no external drags)
// Drops are limited to the top level items (drops into children of the toplevel items are not allowed)
class WKF_EXPORT TopLevelDropTreeWidget : public QTreeWidget
{
   Q_OBJECT
public:
   TopLevelDropTreeWidget(QWidget* aParent = nullptr);
   ~TopLevelDropTreeWidget() override = default;

private:
   void dragMoveEvent(QDragMoveEvent* event) override;
   void dropEvent(QDropEvent* event) override;
};
} // namespace wkf

#endif
