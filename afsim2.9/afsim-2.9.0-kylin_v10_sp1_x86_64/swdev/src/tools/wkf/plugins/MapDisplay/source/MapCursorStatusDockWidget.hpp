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
#ifndef MAPCURSORSTATUSDOCKWIDGET_HPP
#define MAPCURSORSTATUSDOCKWIDGET_HPP

#include <QDockWidget>

#include "VaCallbackHolder.hpp"
#include "ui_CursorDockWidget.h"

namespace vespa
{
class VaViewer;
}

namespace Map
{
class CursorStatusDockWidget : public QDockWidget
{
public:
   CursorStatusDockWidget(const vespa::VaViewer& aViewer,
                          size_t                 aPluginUniqueId,
                          QWidget*               parent = nullptr,
                          Qt::WindowFlags        flags  = Qt::WindowFlags());

   void   Update();
   size_t GetDtedId() const { return mDtedRegistrationId; }

protected:
   void DragEvent(QTreeWidgetItem* aItem);

private:
   void UpdateCursorPositionCB(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY);

   vespa::VaCallbackHolder mCallbacks;
   const vespa::VaViewer&  mViewer;
   int                     mMouseX;
   int                     mMouseY;

   QTreeWidgetItem* mLatitudeItemPtr;
   QTreeWidgetItem* mLongitudeItemPtr;
   QTreeWidgetItem* mElevationItemPtr;
   QTreeWidgetItem* mRangeItemPtr;
   QTreeWidgetItem* mBearingItemPtr;
   QTreeWidgetItem* mBullseyeItemPtr;

   Ui::CursorDockWidget mUI;
   size_t               mDtedRegistrationId;
   size_t               mPluginUniqueId;
};
} // namespace Map
#endif
