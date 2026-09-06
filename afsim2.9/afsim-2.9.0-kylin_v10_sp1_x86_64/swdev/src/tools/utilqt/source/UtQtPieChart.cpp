// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtQtPieChart.hpp"

#include <QAbstractItemModel>
#include <QPainter>

#include "UtMath.hpp"


UtQtPieChart::UtQtPieChart(QWidget* aParent)
   : QWidget(aParent)
   , mTotal(0)
   , mShowKey(false)
{
   setMinimumSize(50, 50);
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void UtQtPieChart::AddData(const QString& aLabel, float aValue)
{
   mData[aLabel] += aValue;
   mTotal += aValue;
   update();
}

void UtQtPieChart::Clear()
{
   mData.clear();
   mTotal = 0.0f;
   update();
}

void UtQtPieChart::paintEvent(QPaintEvent* event)
{
   int      counter = 0;
   int      size    = (int)mData.size();
   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing, true);
   painter.setPen(QPen(palette().text().color(), 0.5));
   float  total = 0;
   QColor hsv;
   int    aX = 0;
   for (auto&& it : mData)
   {
      hsv.setHsvF(counter++ / (float)size, 1.0, 0.8);
      float        value = it.second / mTotal;
      QPainterPath path  = DrawPiece(value, total);
      total += value;

      painter.setBrush(QBrush(hsv));
      painter.drawPath(path);

      if (mShowKey)
      {
         if (it.second > 0)
         {
            painter.setBrush(QBrush(hsv));
            if (aX + 15 < height())
            {
               painter.drawRect(QRect(2 * width() / 3, aX + 5, 10, 10));
               painter.drawText(2 * width() / 3 + 15, aX + 15, it.first + "(" + QString::number(it.second) + ")");
            }
            aX += 20;
         }
      }
   }
}

QPainterPath UtQtPieChart::DrawPiece(float aAmt, float aTotal)
{
   QPainterPath retval;
   int          minwh;
   int          divider;
   if (mShowKey)
   {
      minwh   = (2 * width() / 3 < height()) ? 2 * width() / 3 : height();
      divider = 3;
   }
   else
   {
      minwh   = (width() < height()) ? width() : height();
      divider = 2;
   }
   float startX = minwh / 2 * cos(aTotal * UtMath::cTWO_PI);
   float startY = -minwh / 2 * sin(aTotal * UtMath::cTWO_PI);
   retval.moveTo(width() / divider, height() / 2);
   retval.lineTo(startX + width() / divider, startY + height() / 2);
   retval.arcTo(width() / divider - minwh / 2, (height() - minwh) / 2, minwh, minwh, aTotal * 360, aAmt * 360);
   retval.lineTo(width() / divider, height() / 2);
   return retval;
}

void UtQtPieChart::AddModelSeries(QAbstractItemModel* aModel, int aColumn)
{
   mModel          = aModel;
   mAbstractColumn = aColumn;

   connect(mModel, &QAbstractItemModel::rowsInserted, this, &UtQtPieChart::rowsInserted);
   connect(mModel, &QAbstractItemModel::rowsRemoved, this, &UtQtPieChart::rowsRemoved);
   connect(mModel, &QAbstractItemModel::modelReset, this, &UtQtPieChart::modelReset);
   connect(mModel, &QAbstractItemModel::dataChanged, this, &UtQtPieChart::dataChanged);

   AbstractRedraw();
}

void UtQtPieChart::modelReset()
{
   emit AbstractRedraw();
}

void UtQtPieChart::rowsInserted(const QModelIndex& aParent, int aStart, int aEnd)
{
   emit AbstractRedraw();
}

void UtQtPieChart::rowsRemoved(const QModelIndex& aParent, int aStart, int aEnd)
{
   emit AbstractRedraw();
}

void UtQtPieChart::dataChanged(const QModelIndex& aTL, const QModelIndex& BR, const QVector<int>& aRoles)
{
   emit AbstractRedraw();
}

void UtQtPieChart::AbstractRedraw()
{
   Clear();
   for (int i = 0; i < mModel->rowCount(); ++i)
   {
      AddData(mModel->data(mModel->index(i, mAbstractColumn), Qt::DisplayRole).toString(), 1);
   }
}
