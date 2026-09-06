// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef FILTERWIDGET_HPP
#define FILTERWIDGET_HPP

#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace Ui
{
class FilterWidget;
}

namespace PostProcessor
{
class FilterWidget : public QWidget
{
      Q_OBJECT
   public:
      explicit FilterWidget(QWidget* aParent = nullptr);
      ~FilterWidget() override;

      void SetColumnHeaders(const QStringList& aHeaders);
      void Clear();
      void AddItem(const QStringList& aItems,
                   QColor             aColor = QColor(255, 255, 255),
                   bool               aChecked = true);
      void RemoveItem(const QString& aText,
                      int            aColumn = 0);
      bool RemoveItem(const QVector<QPair<QString, int> >& aItems);

      QStringList GetCheckedItems(int aColumn);

      bool AllChecked();
      bool AllUnchecked();
      void SetChecked(const QString& aName,
                      bool           aChecked = true);

   private:
      void Connect();
      void FilterChanged();
      bool ItemPassesFilter(const QTreeWidgetItem* aItem);
      void CheckAll(bool aChecked);
      void SetBackgroundColor(QTreeWidgetItem* aItem,
                              QColor           aColor);
      void ItemChanged(QTreeWidgetItem* aItem,
                       int              aColumn);

      Ui::FilterWidget* mUIPtr;

};
}
#endif // !FILTERWIDGET_HPP
