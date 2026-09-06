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
#include "WkfWsfDrawDockWidget.hpp"

#include <QPainter>

#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

wkf::WsfDrawDockWidget::WsfDrawDockWidget(QWidget*        aParentPtr /*= nullptr*/,
                                          Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QDockWidget(aParentPtr)
{
   mColorDelegate = new ColorDelegate(this);

   mUi.setupUi(this);

   wkfEnv.GetMainWindow()->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, this);

   mUi.treeView->setItemDelegate(mColorDelegate);

   mUi.networkCheckBox->hide();

   connect(mUi.networkCheckBox,
           &QCheckBox::stateChanged,
           [this](bool aChecked) { emit this->NetworkCheckboxChanged(aChecked); });

   connect(mUi.showAllCheckBox,
           &QCheckBox::clicked,
           [this](bool aChecked)
           {
              auto                checkState = aChecked ? Qt::Checked : Qt::Unchecked;
              QAbstractItemModel* model      = mUi.treeView->model();
              if (model)
              {
                 for (int i = 0; i < model->rowCount(); ++i)
                 {
                    model->setData(model->index(i, 0), checkState, Qt::CheckStateRole);
                 }
              }
           });
}

void wkf::WsfDrawDockWidget::SetModel(QAbstractItemModel& aModel)
{
   mUi.treeView->setModel(&aModel);
   UpdateCheckState(aModel);
   connect(&aModel,
           &QAbstractItemModel::dataChanged,
           [this](const QModelIndex& aBL, const QModelIndex& aTR, const QVector<int>& aRoles)
           {
              QAbstractItemModel* model = mUi.treeView->model();
              if (aRoles.contains(Qt::CheckStateRole) && model)
              {
                 UpdateCheckState(*model);
              }
           });
}

void wkf::WsfDrawDockWidget::UpdateCheckState(const QAbstractItemModel& aModel)
{
   // this isn't super efficient, but we don't anticipate a lot of entries in the model, and model/view design isn't
   // really made for aggregation
   Qt::CheckState checked = Qt::Checked;
   for (int i = 0; i < aModel.rowCount(); ++i)
   {
      Qt::CheckState itemCheckState =
         static_cast<Qt::CheckState>(aModel.data(aModel.index(i, 0), Qt::CheckStateRole).toUInt());
      if (i == 0)
      {
         checked = itemCheckState;
         if (itemCheckState == Qt::PartiallyChecked) // if the first item is partially checked, the overall state is partial
         {
            break;
         }
      }
      else
      {
         if (checked != itemCheckState) // if any check-state doesn't match, the overall state is partial
         {
            checked = Qt::PartiallyChecked;
            break;
         }
      }
   }
   mUi.showAllCheckBox->setCheckState(checked);
}

void wkf::ColorDelegate::paint(QPainter* aPainter, const QStyleOptionViewItem& aOption, const QModelIndex& aIndex) const
{
   QStyledItemDelegate::paint(aPainter, aOption, aIndex);

   QVariant v = aIndex.data(mColorRole);
   if (aIndex.data(mColorRole).isValid())
   {
      QColor c    = v.value<QColor>();
      QRect  rect = aOption.rect;
      rect.setLeft(rect.right() - rect.height() - 1);
      rect.setWidth(rect.width() - 2);
      rect.setHeight(rect.height() - 2);
      rect.setBottom(rect.bottom() + 1);

      aPainter->fillRect(rect, c);
      aPainter->drawRect(rect);
   }
}
