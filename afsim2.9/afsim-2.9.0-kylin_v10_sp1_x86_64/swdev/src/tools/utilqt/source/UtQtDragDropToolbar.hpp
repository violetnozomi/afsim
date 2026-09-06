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

#ifndef UTQTDRAGDROPTOOLBAR_HPP
#define UTQTDRAGDROPTOOLBAR_HPP

#include "utilqt_export.h"

#include <QToolBar>

// A QToolbar that will create QDrag objects when its tool buttons are clicked/dragged
// Some other widget must accept the drag enter/drop events and process them as needed
// The cursor hotspot for the QDrag will default to (16, 16), or mid-point of 32 x 32 icon,
// unless the action's data() contains a QPoint.
class UTILQT_EXPORT UtQtDragDropToolbar : public QToolBar
{
public:
   explicit UtQtDragDropToolbar(QWidget* parent = nullptr);

protected:
   void actionEvent(QActionEvent* event) override;
   bool eventFilter(QObject* watched, QEvent* event) override;
   void mouseMoveEvent(QMouseEvent* event) override;
   void mousePressEvent(QMouseEvent* event) override;

private:
   QPoint   mDragStartPos;
   QAction* mDragActionPtr;
};
#endif
