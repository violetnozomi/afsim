// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFPLATFORMCONTEXTTREEWIDGET_HPP
#define WKFPLATFORMCONTEXTTREEWIDGET_HPP

#include "wkf_export.h"

#include <QTreeWidget>

class QMouseEvent;

namespace wkf
{
class WKF_EXPORT DraggableTreeWidget : public QTreeWidget
{
   Q_OBJECT
public:
   DraggableTreeWidget(QWidget* aParent = nullptr)
      : QTreeWidget(aParent)
   {
   }
   ~DraggableTreeWidget() override = default;

   // Enables/Disables the processing of the mouse events to support Drag & Drop
   void SetDragEnabled(bool aEnable) { mDragEnabled = aEnable; }

signals:
   void DragEvent(QTreeWidgetItem* aItem);

private:
   void mousePressEvent(QMouseEvent* aEvent) override;
   void mouseMoveEvent(QMouseEvent* aEvent) override;

   bool   mDragEnabled{true};
   bool   mDragPossible{false};
   QPoint mDragStart;
};
} // namespace wkf

#endif
