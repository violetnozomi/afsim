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
#include "UtQtTableView.hpp"

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>

void UtQtTableView::keyPressEvent(QKeyEvent* e)
{
   if ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_C)
   {
      QModelIndexList indList = selectionModel()->selectedIndexes();
      QString         text;
      for (int i = 0; i < indList.size(); ++i)
      {
         const QModelIndex& ind = indList[i];
         if (i != 0)
            text += '\n';
         text += ind.data().toString();
      }
      QApplication::clipboard()->setText(text);
   }
   QTableView::keyPressEvent(e);
}
