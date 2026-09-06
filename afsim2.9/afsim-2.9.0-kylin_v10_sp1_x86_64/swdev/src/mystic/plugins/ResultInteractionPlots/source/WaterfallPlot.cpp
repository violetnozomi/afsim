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

#include "WaterfallPlot.hpp"

#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QScrollBar>
#include <QTextStream>

#include "RvInteractionDb.hpp"
#include "RvResultPlatform.hpp"
#include "WkfEnvironment.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"
#include "selectors/WkfGroupingSelector.hpp"

RvInteractionPlots::WaterfallPlot::WaterfallPlot(QWidget* aParent)
   : QTableWidget(aParent)
   , mDatabasePtr(nullptr)
   , mPlatformPtr(nullptr)
   , mFilter(wkf::PlatformGrouping())
   , mTime(0.0)
   , mZoom(3)
   , mEndTime(1000.0)
{
   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &QWidget::customContextMenuRequested, this, &WaterfallPlot::ContextMenu);
}

void RvInteractionPlots::WaterfallPlot::Setup(rv::ResultDb*                aDatabasePtr,
                                              rv::ResultPlatform*          aPlatformPtr,
                                              const QString&               aType,
                                              const wkf::PlatformGrouping& aFilter)
{
   mDatabasePtr = aDatabasePtr;
   mPlatformPtr = aPlatformPtr;
   mFilter      = aFilter;

   auto header = new WaterfallHeader(Qt::Horizontal, this);
   setHorizontalHeader(header);
   connect(this, &WaterfallPlot::Zoom, header, &WaterfallHeader::Zoom);
   horizontalHeader()->show();
   verticalHeader()->show();
   auto delegate = new WaterfallDelegate(this);
   setItemDelegateForColumn(0, delegate);
   connect(this, &WaterfallPlot::Zoom, delegate, &WaterfallDelegate::Zoom);
   connect(this, &WaterfallPlot::Time, delegate, &WaterfallDelegate::Time);
   SetType(aType);
   setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void RvInteractionPlots::WaterfallPlot::ContextMenu(const QPoint& aPoint)
{
   QMenu    menu;
   QAction* exportAction = new QAction("Export to CSV...", &menu);
   connect(exportAction, &QAction::triggered, [this]() { Export(); });
   menu.addAction(exportAction);
   menu.exec(mapToGlobal(aPoint));
}

void RvInteractionPlots::WaterfallPlot::SetTime(double aTime)
{
   mTime = aTime;

   emit Time(mTime);

   viewport()->update();
}

void RvInteractionPlots::WaterfallPlot::SetType(const QString& aType)
{
   mTypeString = aType;

   if (aType == "Detected")
   {
      mType   = "Detect";
      mSource = false;
   }
   else if (aType == "Detecting")
   {
      mType   = "Detect";
      mSource = true;
   }
   else if (aType == "Sensor Tracked")
   {
      mType   = "Track";
      mSource = false;
   }
   else if (aType == "Sensor Tracking")
   {
      mType   = "Track";
      mSource = true;
   }
   else if (aType == "Local Tracked")
   {
      mType   = "LocalTrack";
      mSource = false;
   }
   else if (aType == "Local Tracking")
   {
      mType   = "LocalTrack";
      mSource = true;
   }
   else if (aType == "Jammed")
   {
      mType   = "Jam";
      mSource = false;
   }
   else if (aType == "Jamming")
   {
      mType   = "Jam";
      mSource = true;
   }
   CollectData();
}

void RvInteractionPlots::WaterfallPlot::SetFilter(const wkf::PlatformGrouping& aGrouping)
{
   mFilter = aGrouping;
   CollectData();
}

QSize RvInteractionPlots::WaterfallPlot::sizeHint() const
{
   return QSize(800, 600);
}

void RvInteractionPlots::WaterfallPlot::Export()
{
   QString fname =
      QFileDialog::getSaveFileName(this, "Export to CSV", "", QString("CSV (*.csv);; Text (*.txt);; All Files(*.*)"));
   if (!fname.isEmpty())
   {
      QFile file(fname);
      if (file.open(QIODevice::WriteOnly))
      {
         QTextStream stream(&file);
         stream << QString("Ranges in which ") << mPlatformPtr->GetName().c_str() << " is " << mTypeString;

         for (auto&& label : mLabels)
         {
            stream << "\n" << label << ", ";
            bool start = true;
            bool first = true;
            for (auto&& time : mInteractions[label])
            {
               if (!first && start)
               {
                  stream << ", ";
               }
               if (start)
               {
                  stream << "[" << time.toString() << " - ";
               }
               else
               {
                  stream << time.toString() << "]";
               }
               start = !start;
               first = false;
            }
         }
         file.close();
      }
   }
}

void RvInteractionPlots::WaterfallPlot::SetZoom(int aValue, float aTime, int aX)
{
   mZoom = aValue;
   // -2 = 15 seconds per tick
   // 0 = 1 minute per tick
   // 3 = 4 minutes per tick
   mZoom       = std::max(-2, mZoom);
   int maxzoom = (int)(log2(mEndTime / (float)(width() - verticalHeader()->width())) + 1);
   mZoom       = std::min(maxzoom, mZoom);

   float mult = pow(2, mZoom);
   setColumnWidth(0, mEndTime / mult);

   mDesiredScroll = (aTime / mult) - aX;

   emit Zoom(mZoom);

   if (horizontalScrollBar())
   {
      mScrollConnection =
         connect(horizontalScrollBar(), &QScrollBar::rangeChanged, [this](int aMin, int aMax) { SetScrollbarPosition(); });
   }
   viewport()->update();
}

void RvInteractionPlots::WaterfallPlot::showEvent(QShowEvent* aEvent)
{
   QTableWidget::showEvent(aEvent);

   float w   = width() - verticalHeader()->width();
   float let = log2(mEndTime / w);
   SetZoom((int)(let + 1), 0, 0);
}

void RvInteractionPlots::WaterfallPlot::wheelEvent(QWheelEvent* aEvent)
{
   float mult              = pow(2, mZoom);
   int   scrollbarposition = 0;
   if (horizontalScrollBar())
   {
      scrollbarposition = horizontalScrollBar()->value();
   }
   float timeOfInterest = (aEvent->x() + scrollbarposition) * mult;
   int   numClicks      = aEvent->angleDelta().y() / 120;
   SetZoom(mZoom - numClicks, timeOfInterest, aEvent->x());
}

void RvInteractionPlots::WaterfallPlot::CollectData()
{
   float mult = pow(2, mZoom);
   mLabels.clear();
   mInteractions.clear();

   const rv::InteractionDb* interactionDB = mDatabasePtr->LockInteractionDb();
   bool                     valid;
   mEndTime = mDatabasePtr->GetMaxTime();
   rv::InteractionDb::InteractionArray::range_pair rangeData;
   // GetRangeData is exclusive on the endpoints, this can lead to bad things if an event starts at 0, so we start it at -1.
   rangeData = interactionDB->GetRangeData(mPlatformPtr->GetPlatformIndex(), -1, mEndTime, valid);
   std::map<QString, int>   counter;
   std::map<QString, float> endtimes;

   mPlatformPtr->BeginRead();
   float ownendtime = mPlatformPtr->GetEndTime();
   mPlatformPtr->EndRead();
   if (ownendtime < 0)
   {
      ownendtime = mEndTime;
   }

   if (valid)
   {
      for (auto it = rangeData.first; it <= rangeData.second; ++it)
      {
         if ((*it)->mType == mType)
         {
            auto* scenarioPtr = vaEnv.GetStandardScenario();
            if (scenarioPtr)
            {
               if (mSource && ((*it)->mSource == mPlatformPtr->GetPlatformIndex()))
               {
                  auto             tgtidx = (*it)->mTarget;
                  vespa::VaEntity* tgt    = scenarioPtr->FindPlatformByIndex(tgtidx);
                  if (tgt)
                  {
                     QString                         label = tgt->GetName().c_str();
                     std::set<wkf::PlatformGrouping> grps  = wkfEnv.GetGroupings(tgtidx);
                     if ((grps.find(mFilter) != grps.end()) ||
                         ((mFilter.IsIndividual()) && (mFilter.Name() == label.toStdString())))
                     {
                        if (endtimes.find(label) == endtimes.end())
                        {
                           rv::ResultPlatform* otherPlat = mDatabasePtr->FindPlatform(tgtidx);
                           if (otherPlat)
                           {
                              otherPlat->BeginRead();
                              float otherendtime = otherPlat->GetEndTime();
                              otherPlat->EndRead();
                              if (otherendtime < 0)
                              {
                                 otherendtime = ownendtime;
                              }
                              endtimes[label] = std::min(ownendtime, otherendtime);
                           }
                        }
                        if (!mLabels.contains(label))
                        {
                           mLabels.append(label);
                        }
                        bool  start = (*it)->mStart;
                        float time  = (*it)->mTime;
                        counter[label] += start ? 1 : -1;
                        if (start && (counter[label] == 1))
                        {
                           mInteractions[label].append(time);
                        }
                        else if (!start && (counter[label] == 0))
                        {
                           mInteractions[label].append(time);
                        }
                     }
                  }
               }
               else if (!mSource && ((*it)->mTarget == mPlatformPtr->GetPlatformIndex()))
               {
                  auto             srcidx = (*it)->mSource;
                  vespa::VaEntity* src    = scenarioPtr->FindPlatformByIndex(srcidx);
                  if (src)
                  {
                     QString                         label = src->GetName().c_str();
                     std::set<wkf::PlatformGrouping> grps  = wkfEnv.GetGroupings(srcidx);
                     if ((grps.find(mFilter) != grps.end()) ||
                         ((mFilter.IsIndividual()) && (mFilter.Name() == label.toStdString())))
                     {
                        if (endtimes.find(label) == endtimes.end())
                        {
                           rv::ResultPlatform* otherPlat = mDatabasePtr->FindPlatform(srcidx);
                           if (otherPlat)
                           {
                              otherPlat->BeginRead();
                              float otherendtime = otherPlat->GetEndTime();
                              otherPlat->EndRead();
                              if (otherendtime < 0)
                              {
                                 otherendtime = mEndTime;
                              }
                              endtimes[label] = std::min(ownendtime, otherendtime);
                           }
                        }
                        if (!mLabels.contains(label))
                        {
                           mLabels.append(label);
                        }
                        bool  start = (*it)->mStart;
                        float time  = (*it)->mTime;
                        counter[label] += start ? 1 : -1;
                        if (start && (counter[label] == 1))
                        {
                           mInteractions[label].append(time);
                        }
                        else if (!start && (counter[label] == 0))
                        {
                           mInteractions[label].append(time);
                        }
                     }
                  }
               }
            }
         }
      }
      for (auto&& it : counter)
      {
         // there is a track that should be drawn to the end of one of the platforms
         if (it.second > 0)
         {
            mInteractions[it.first].append(endtimes[it.first]);
         }
      }
   }

   setColumnCount(1);
   setColumnWidth(0, mEndTime / mult);
   setRowCount(mLabels.size());
   setVerticalHeaderLabels(mLabels);
   verticalHeader()->setDefaultSectionSize(verticalHeader()->minimumSectionSize());

   if (mLabels.empty())
   {
      setRowCount(1);
      QTableWidgetItem* titem = item(0, 0);
      if (!titem)
      {
         titem = new QTableWidgetItem();
         titem->setFlags(titem->flags() & ~Qt::ItemIsEditable);
         setItem(0, 0, titem);
      }
      titem->setData(Qt::DisplayRole, "NO DATA AVAILABLE");
   }
   else
   {
      int i = 0;
      for (auto&& label : mLabels)
      {
         QTableWidgetItem* titem = item(i, 0);
         if (!titem)
         {
            titem = new QTableWidgetItem();
            titem->setFlags(titem->flags() & ~Qt::ItemIsEditable);
            setItem(i, 0, titem);
         }
         titem->setData(Qt::DisplayRole, mInteractions[label]);
         ++i;
      }
   }

   mDatabasePtr->UnlockInteractionDb();
}

void RvInteractionPlots::WaterfallPlot::SetScrollbarPosition()
{
   int sb = std::max(mDesiredScroll, horizontalScrollBar()->minimum());
   sb     = std::min(sb, horizontalScrollBar()->maximum());
   horizontalScrollBar()->setValue(sb);
   disconnect(mScrollConnection);
}

RvInteractionPlots::WaterfallDelegate::WaterfallDelegate(QWidget* aParent /*= nullptr*/)
   : QStyledItemDelegate(aParent)
{
}

void RvInteractionPlots::WaterfallDelegate::paint(QPainter*                   aPainter,
                                                  const QStyleOptionViewItem& aOption,
                                                  const QModelIndex&          aIndex) const
{
   float mult = pow(2, mZoom);
   if (aIndex.data().canConvert<QList<QVariant>>())
   {
      QList<QVariant> ssdata = qvariant_cast<QList<QVariant>>(aIndex.data());

      float  x0    = aOption.rect.left();
      bool   first = true;
      QPoint p1;
      for (auto&& it : ssdata)
      {
         if (first)
         {
            p1 = QPoint(x0 + it.toFloat() / mult, aOption.rect.top() + 1);
         }
         else
         {
            QPoint p2(x0 + it.toFloat() / mult, aOption.rect.bottom() - 1);
            aPainter->fillRect(QRect(p1, p2), aPainter->pen().color());
         }
         first = !first;
      }

      int  time   = (mTime / mult) + x0;
      QPen oldpen = aPainter->pen();
      aPainter->setPen(Qt::red);
      aPainter->drawLine(time, aOption.rect.bottom(), time, aOption.rect.top());
      aPainter->setPen(oldpen);
   }
   else
   {
      QStyledItemDelegate::paint(aPainter, aOption, aIndex);
   }
}

void RvInteractionPlots::WaterfallDelegate::Zoom(int aValue)
{
   mZoom = aValue;
}

void RvInteractionPlots::WaterfallDelegate::Time(float aTime)
{
   mTime = aTime;
}

RvInteractionPlots::WaterfallHeader::WaterfallHeader(Qt::Orientation aOrientation, QWidget* aParent /*= nullptr*/)
   : QHeaderView(aOrientation, aParent)
   , mZoom(3)
{
}

void RvInteractionPlots::WaterfallHeader::paintSection(QPainter* aPainter, const QRect& aRect, int aLogicalIndex) const
{
   float mult = pow(2, mZoom);
   int   ic   = 0;
   for (int i = aRect.left(); i < aRect.right(); i += 60)
   {
      QPoint p1(i, aRect.bottom());
      QPoint p2(i, aRect.bottom() - 4);
      aPainter->drawLine(p1, p2);
      QString str = QString::number(i);
      int     w   = aPainter->fontMetrics().width(str);
      aPainter->drawText(QPoint(i - w / 2, aRect.bottom() - 6), QString::number(ic));
      ic += mult * 60;
   }
}

void RvInteractionPlots::WaterfallHeader::Zoom(int aValue)
{
   mZoom = aValue;
}
