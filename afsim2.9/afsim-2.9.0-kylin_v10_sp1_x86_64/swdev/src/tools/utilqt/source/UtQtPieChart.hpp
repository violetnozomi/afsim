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
#ifndef UTQTPIECHART_HPP
#define UTQTPIECHART_HPP

#include "utilqt_export.h"

#include <QWidget>

class QAbstractItemModel;

class UTILQT_EXPORT UtQtPieChart : public QWidget
{
public:
   UtQtPieChart(QWidget* aParent);
   ~UtQtPieChart() override = default;
   void AddData(const QString& aLabel, float aValue);
   void Clear();
   void ShowKey(bool aState)
   {
      mShowKey = aState;
      update();
   }
   void AddModelSeries(QAbstractItemModel* aModel, int aColumn);

protected:
   void         paintEvent(QPaintEvent* event) override;
   QPainterPath DrawPiece(float mAmt, float aTotal);

private:
   void modelReset();
   void rowsInserted(const QModelIndex& aParent, int aStart, int aEnd);
   void rowsRemoved(const QModelIndex& aParent, int aStart, int aEnd);
   void dataChanged(const QModelIndex& aTL, const QModelIndex& BR, const QVector<int>& aRoles);
   void AbstractRedraw();


   std::map<QString, float> mData;
   float                    mTotal;
   bool                     mShowKey;
   QAbstractItemModel*      mModel;
   int                      mAbstractColumn;
};

#endif
