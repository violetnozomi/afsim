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

#ifndef WATERFALLPLOT_HPP
#define WATERFALLPLOT_HPP

#include <QDialog>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QTableWidget>

#include "RvResultData.hpp"
#include "WkfDockWidget.hpp"
#include "WkfPlatformGrouping.hpp"

namespace RvInteractionPlots
{
class WaterfallPlot : public QTableWidget
{
   Q_OBJECT
public:
   WaterfallPlot(QWidget* aParent);
   ~WaterfallPlot() override = default;

   void  Setup(rv::ResultDb*                aDatabasePtr,
               rv::ResultPlatform*          aPlatformPtr,
               const QString&               aType,
               const wkf::PlatformGrouping& aFilter);
   void  ContextMenu(const QPoint& aPoint);
   void  SetTime(double aTime);
   void  SetType(const QString& aType);
   void  SetFilter(const wkf::PlatformGrouping& aGrouping);
   QSize sizeHint() const override;
   void  Export();
signals:
   void Zoom(int aValue);
   void Time(float aTime);

private:
   void SetZoom(int aValue, float aTime, int aX);
   void showEvent(QShowEvent* aEvent) override;
   void wheelEvent(QWheelEvent* aEvent) override;
   void CollectData();
   void SetScrollbarPosition();

   rv::ResultDb*         mDatabasePtr;
   rv::ResultPlatform*   mPlatformPtr;
   QString               mTypeString;
   std::string           mType;
   bool                  mSource;
   wkf::PlatformGrouping mFilter;
   double                mTime;
   int                   mZoom;
   double                mEndTime;

   QStringList                        mLabels;
   std::map<QString, QList<QVariant>> mInteractions;
   QMetaObject::Connection            mScrollConnection;
   int                                mDesiredScroll;
};

class WaterfallDelegate : public QStyledItemDelegate
{
   Q_OBJECT
public:
   WaterfallDelegate(QWidget* aParent = nullptr);
   void paint(QPainter* aPainter, const QStyleOptionViewItem& aOption, const QModelIndex& aIndex) const override;
   void Zoom(int aValue);
   void Time(float aTime);

private:
   int   mZoom{3};
   float mTime{0.0f};
};

class WaterfallHeader : public QHeaderView
{
   Q_OBJECT
public:
   WaterfallHeader(Qt::Orientation aOrientation, QWidget* aParent = nullptr);
   void Zoom(int aValue);

protected:
   void paintSection(QPainter* aPainter, const QRect& aRect, int aLogicalIndex) const override;

private:
   int mZoom;
};
} // namespace RvInteractionPlots

#endif
