// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtQtGradientBar.hpp"

#include <QColor>
#include <QPainter>
#include <QPen>

UtQtGradientBar::UtQtGradientBar(QWidget* aParent)
   : QWidget(aParent)
   , mHorizontal(false)
{
}

void UtQtGradientBar::SetSolidColor(const QColor& aColor)
{
   mGradientStops.clear();
   mGradientStops.append(qMakePair(0.0, aColor));
   update();
}

void UtQtGradientBar::SetGradientStops(const QGradientStops& aGradientStops)
{
   mGradientStops = aGradientStops;
   update();
}

//! @returns A gradient that is set to draw within the entire rectangle of this
//! widget.
QGradientStops UtQtGradientBar::GradientStops() const
{
   return mGradientStops;
}

//! Colors this entire widget with a gradient, and then colors a 1-pixel black
//! border "around" it. The border is key in distinguishing parts of the gradient
//! which may be white from a white background under or around the gradient bar.
void UtQtGradientBar::paintEvent(QPaintEvent* aEvent)
{
   QPainter        painter(this);
   const QRect     borderRect(rect().left(), rect().top(), rect().width() - 1, rect().height() - 1);
   QLinearGradient gradient;

   gradient.setStops(mGradientStops);

   if (mHorizontal)
   {
      gradient.setStart(rect().topLeft());
      gradient.setFinalStop(rect().topRight());
   }
   else
   {
      gradient.setStart(rect().bottomLeft());
      gradient.setFinalStop(rect().topLeft());
   }

   QWidget::paintEvent(aEvent);
   painter.fillRect(rect(), gradient);
   painter.drawRect(borderRect);
}

void UtQtGradientBar::SetHorizontal()
{
   mHorizontal = true;
   update();
}

UtQtGradientAction::UtQtGradientAction(QObject* aParent, const QGradientStops& aStops, const QString& aKey)
   : QWidgetAction(aParent)
   , mKey(aKey)
   , mGradientStops(aStops)
{
}

QWidget* UtQtGradientAction::createWidget(QWidget* aParent)
{
   UtQtGradientBar* bar = new UtQtGradientBar(aParent);
   bar->SetHorizontal();
   bar->SetGradientStops(mGradientStops);
   bar->setMinimumSize(64, 16);
   return bar;
}

void UtQtGradientAction::deleteWidget(QWidget* aWidget)
{
   aWidget->deleteLater();
}

void UtQtGradientAction::GetStandardStops(std::map<float, UtColor>& aStops)
{
   for (auto&& stop : mGradientStops)
   {
      aStops[stop.first] = UtColor(stop.second.red(), stop.second.green(), stop.second.blue(), stop.second.alpha());
   }
}
