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

#include "RvTimeControllerToolbarWidgets.hpp"

#include <functional>

#include <QAction>
#include <QBasicTimer>
#include <QInputDialog>
#include <QPainter>
#include <QStyleOptionSlider>
#include <QStylePainter>
#include <QTimer>
#include <QToolTip>

#include "RvEnvironment.hpp"
#include "RvTimeControllerInterface.hpp"
#include "UtMath.hpp"
#include "WkfAction.hpp"
#include "WkfMainWindow.hpp"
#include "WkfUtils.hpp"

RvTimeController::ClockRateSpinBox::ClockRateSpinBox(QWidget* parent)
   : QDoubleSpinBox(parent)
{
   mResetActionPtr = new wkf::Action(QIcon::fromTheme("reset-small"), "Reset to x1.00", this);
   connect(this,
           static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
           this,
           &RvTimeController::ClockRateSpinBox::ValueChanged);
   connect(mResetActionPtr, &QAction::triggered, this, &RvTimeController::ClockRateSpinBox::Reset);
}

void RvTimeController::ClockRateSpinBox::ValueChanged(double value)
{
   if (value != 1.0)
   {
      lineEdit()->addAction(mResetActionPtr, QLineEdit::TrailingPosition);
   }
   else
   {
      lineEdit()->removeAction(mResetActionPtr);
   }
}

void RvTimeController::ClockRateSpinBox::Reset()
{
   // KLUDGE: Don't call setValue here as it will invoke the ValueChanged slot, which will remove the reset action
   // from the QLineEdit. This causes a crash in Qt, so instead delay the call to setValue using a single-shot
   // QTimer.
   QTimer::singleShot(100, std::bind(&RvTimeController::ClockRateSpinBox::setValue, this, 1.0));
}

RvTimeController::ClockRateSlider::ClockRateSlider(QWidget* parent)
   : QSlider(parent)
{
}

void RvTimeController::ClockRateSlider::paintEvent(QPaintEvent* ev)
{
   QStylePainter      p(this);
   QStyleOptionSlider opt;
   initStyleOption(&opt);

   QRect handle = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

   // draw tick marks
   // do this manually because they are very badly behaved with style sheets
   int interval = tickInterval();
   if (interval == 0)
   {
      interval = pageStep();
   }
   if (tickPosition() != NoTicks)
   {
      for (int i = minimum(); i <= maximum(); i += interval)
      {
         int x = round((double)((double)((double)(i - minimum()) / (double)(maximum() - minimum())) *
                                   (double)(width() - handle.width()) +
                                (double)(handle.width() / 2.0))) -
                 1;
         int h = (i - minimum()) % 5 == 0 ? 4 : 2;

         if (tickPosition() == TicksBothSides || tickPosition() == TicksAbove)
         {
            int y = rect().top();
            p.drawLine(x, y, x, y + h);
         }
         if (tickPosition() == TicksBothSides || tickPosition() == TicksBelow)
         {
            int y = rect().bottom();
            p.drawLine(x, y, x, y - h);
         }
      }
   }

   // draw the slider (this is basically copy/pasted from QSlider::paintEvent)
   opt.subControls = QStyle::SC_SliderGroove;
   p.drawComplexControl(QStyle::CC_Slider, opt);

   // draw the slider handle
   opt.subControls = QStyle::SC_SliderHandle;
   p.drawComplexControl(QStyle::CC_Slider, opt);
}

RvTimeController::CachedSlider::CachedSlider(QWidget* aParent)
   : QSlider(Qt::Horizontal, aParent)
   , mCacheMin(0)
   , mCacheMax(0)
{
   setMouseTracking(true);
}

void RvTimeController::CachedSlider::paintEvent(QPaintEvent* event)
{
   QPainter painter(this);
   QRect    rect;
   rect.setLeft((float)mCacheMin / maximum() * width());
   rect.setRight((float)mCacheMax / maximum() * width());
   rect.setTop(1);
   rect.setBottom(height() - 2);
   painter.fillRect(rect, QBrush(Qt::darkGray));
   for (auto& it : mBookmarks)
   {
      QRect bookmark;
      float pos = (float)it.pos / maximum() * width();
      bookmark.setLeft(pos - 2);
      bookmark.setRight(pos + 2);
      bookmark.setBottom(height());
      bookmark.setTop(0);
      it.currentVisual = bookmark;
      painter.fillRect(bookmark, QBrush(wkf::util::GetColor(QString::fromStdString(it.type))));
   }
   QSlider::paintEvent(event);
}

void RvTimeController::CachedSlider::mouseMoveEvent(QMouseEvent* event)
{
   auto truncatedString = [](const std::string& aStr)
   { return (aStr.size() > 30) ? (aStr.substr(0, 30) + "...") : aStr; };

   QString combinedTip;
   for (const auto& it : mBookmarks)
   {
      if (it.currentVisual.contains(event->pos()))
      {
         combinedTip += QString::fromStdString("<div><b>" + truncatedString(it.type) + ": </b></div><div> " +
                                               truncatedString(it.message) + "</div>");
      }
   }
   if (!combinedTip.isEmpty())
   {
      QToolTip::showText(event->screenPos().toPoint(), combinedTip);
   }
   QSlider::mouseMoveEvent(event);
}

void RvTimeController::CachedSlider::setCache(int aMin, int aMax)
{
   mCacheMin = aMin;
   mCacheMax = aMax;
}
