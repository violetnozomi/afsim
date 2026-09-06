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
#ifndef WKFCOMBOBOXTREEWIDGETITEM_HPP
#define WKFCOMBOBOXTREEWIDGETITEM_HPP

#include "wkf_export.h"

#include <set>

#include <QApplication>
#include <QComboBox>
#include <QPointer>
#include <QStandardItemModel>
#include <QString>

#include "WkfPlatformDataTreeItem.hpp"

class QTreeWidget;
class QTreeWidgetItem;

namespace wkf
{
// PlatformDataTreeItem that contains a QComboBox in a given column
class WKF_EXPORT ComboBoxTreeWidgetItem : public PlatformDataTreeItem
{
private:
   // Overwrites popup display to not be animated, as it was causing flicker
   class UnanimatedComboBox : public QComboBox
   {
      void showPopup() override;
   };

public:
   ComboBoxTreeWidgetItem(QTreeWidgetItem*             aParentPtr,
                          int                          aBoxColumn,
                          QPointer<QStandardItemModel> aModelPtr,
                          bool                         aCategorized = false)
      : PlatformDataTreeItem(aParentPtr)
      , mBoxColumn(aBoxColumn)
      , mModelPtr(std::move(aModelPtr))
      , mCategorized(aCategorized)
   {
   }

   // Fills mBoxPtr with a new QComboBox populated with mItems, and puts it in column mBoxColumn
   void BuildWidget(QTreeWidget* aTreePtr);

   // Sets text of Combo Box if column matches, otherwise assumes no widget exists in that column
   void setText(int aColumn, const QString& aText);

   QPointer<wkf::ComboBoxTreeWidgetItem::UnanimatedComboBox> GetBox() const { return mBoxPtr; }
   const QString&                                            GetText() const { return mText; }
   bool                                                      IsCategorized() const { return mCategorized; }

private:
   QPointer<wkf::ComboBoxTreeWidgetItem::UnanimatedComboBox> mBoxPtr = nullptr;
   QString                                                   mText   = "";
   int                                                       mBoxColumn;
   QPointer<QStandardItemModel>                              mModelPtr;
   bool                                                      mCategorized;
};
} // namespace wkf
#endif
