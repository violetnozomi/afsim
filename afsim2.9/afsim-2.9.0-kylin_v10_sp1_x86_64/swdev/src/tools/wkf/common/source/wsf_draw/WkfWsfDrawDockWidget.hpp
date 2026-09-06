// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFWSFDRAWDOCKWIDGET_HPP
#define WKFWSFDRAWDOCKWIDGET_HPP

#include <set>

#include <QDockWidget>
#include <QStyledItemDelegate>

class QStandardItemModel;

#include "wkf_common_export.h"

#include "ui_WkfWsfDrawDockWidget.h"

namespace wkf
{
// adds a right-justified color 'gem' to the items.  Color is bound to the Qt::UserData role by default.
class ColorDelegate : public QStyledItemDelegate
{
public:
   explicit ColorDelegate(QObject* aParent, int aColorRole = Qt::UserRole)
      : QStyledItemDelegate(aParent)
      , mColorRole(aColorRole)
   {
   }
   ~ColorDelegate() = default;

   void paint(QPainter* aPainter, const QStyleOptionViewItem& aOption, const QModelIndex& aIndex) const override;

private:
   int mColorRole;
};

class WKF_COMMON_EXPORT WsfDrawDockWidget : public QDockWidget
{
   Q_OBJECT
public:
   explicit WsfDrawDockWidget(QWidget* aParentPtr = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());
   virtual ~WsfDrawDockWidget() = default;

   void SetModel(QAbstractItemModel& aModel);

signals:

   void NetworkCheckboxChanged(bool aEnabled);

protected:
   Ui::WsfDrawDockWidget mUi;

private:
   void UpdateCheckState(const QAbstractItemModel& aModel);

   ColorDelegate* mColorDelegate;
};

} // namespace wkf
#endif // WKFWSFDRAWDOCKWIDGET_HPP
