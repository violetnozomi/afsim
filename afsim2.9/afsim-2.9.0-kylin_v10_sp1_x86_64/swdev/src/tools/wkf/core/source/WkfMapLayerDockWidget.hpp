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

#ifndef WKFMAPLAYERDOCKWIDGET_HPP
#define WKFMAPLAYERDOCKWIDGET_HPP

#include "wkf_export.h"

#include <set>

#include <QDockWidget>
#include <QDragEnterEvent>
#include <QFileInfo>
#include <QStyledItemDelegate>

#include "UtMemory.hpp"
#include "UtQtGradientMap.hpp"
#include "VaCallbackHolder.hpp"
#include "ui_WkfMapLayerDockWidget.h"

class UtoCmeTerrain;

namespace vespa
{
class VaWidget;
}

namespace wkf
{
class DeleteButtonDelegate : public QStyledItemDelegate
{
public:
   void paint(QPainter* aPainter, const QStyleOptionViewItem& aOption, const QModelIndex& aIndex) const override;
};

// this UI allows the user to add layers to their maps
// layers may be shape or image files

class WKF_EXPORT MapLayerDockWidget : public QDockWidget
{
public:
   MapLayerDockWidget(QWidget* aParent = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());
   ~MapLayerDockWidget() override = default;

protected:
   bool CreateLayerShape(const QFileInfo& aFilename, UtoCmeTerrain& aTerrain);
   bool CreateLayerTiff(const QFileInfo& aFilename, UtoCmeTerrain& aTerrain);
   void CreateLayerImage(const QFileInfo& aFilename, UtoCmeTerrain& aTerrain);

   // something has dragged into the widget, only accept if it is files,
   // and every file is usable
   virtual void DragEnter(QDragEnterEvent* aEvent);

   // something has dropped into the widget, if it is usable, process it
   virtual void DragDrop(QDropEvent* aEvent);

private:
   enum UserDataMap
   {
      cTYPE     = Qt::UserRole,
      cNAME     = Qt::UserRole + 1,
      cFILE     = Qt::UserRole + 2,
      cCOLOR    = Qt::UserRole + 3,
      cMINLAT   = Qt::UserRole + 4,
      cMAXLAT   = Qt::UserRole + 5,
      cMINLON   = Qt::UserRole + 6,
      cMAXLON   = Qt::UserRole + 7,
      cMINVALUE = Qt::UserRole + 8,
      cMAXVALUE = Qt::UserRole + 9,
      cSUBDATA  = Qt::UserRole + 10,
      cOPACITY  = Qt::UserRole + 11,
      cGRADIENT = Qt::UserRole + 12
   };
   enum LayerType
   {
      cSHAPE           = 0,
      cGEOIMAGE        = 1,
      cRAWIMAGE        = 2,
      cSENSORPLOTIMAGE = 3
   };
   void CreateLayer();
   void ItemChanged(QTreeWidgetItem* aItemPtr);
   void ItemClicked(QTreeWidgetItem* aItemPtr);
   void ReapplyLayers();
   void ContextMenu(const QPoint& aPoint);
   void RemoveItemsAssociatedWithFile(const QFileInfo& aFilename);
   void EraseItemsLayer(const QTreeWidgetItem& aItem);
   void PreviewDbfFields(const std::string& aFile, std::map<char, std::set<std::string>>& aFields);
   void AddSensorPlotLayerImage(const QTreeWidgetItem& aItem, UtoCmeTerrain& aTerrain);

   // something has dragged into the widget, pass it to the implementation
   void dragEnterEvent(QDragEnterEvent* aEvent) override;

   // something has dropped into the widget, pass it to the implementation
   void dropEvent(QDropEvent* aEvent) override;

   // a new standard widget has come into play, connect to its drag/drop events
   void StandardWidgetChangedCB(vespa::VaWidget* aWidgetPtr);

   Ui::WkfMapLayerDockWidget             mUI;
   vespa::VaCallbackHolder               mCallbacks;
   std::unique_ptr<DeleteButtonDelegate> mDelDelegate;
   UtQtGradientMap                       mGradientMap;
};
} // namespace wkf

#endif
