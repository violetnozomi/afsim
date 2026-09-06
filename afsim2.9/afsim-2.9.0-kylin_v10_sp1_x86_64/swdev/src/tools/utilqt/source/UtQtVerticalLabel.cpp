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

#include "UtQtVerticalLabel.hpp"

#include <QPainter>
#include <QPalette>

UtQtVerticalLabel::UtQtVerticalLabel(QWidget* aParent)
   : QLabel(aParent)
{
}

//! Draws the vertical label. The label will be vertically and horizontally
//! centered.
void UtQtVerticalLabel::paintEvent(QPaintEvent*)
{
   QPainter           painter(this);
   const QColor&      textColor = palette().color(QPalette::WindowText);
   const QFontMetrics fontMetrics(font(), this);
   painter.setPen(textColor);
   painter.translate(sizeHint().width(), sizeHint().height());
   painter.rotate(-90.0);
   const qreal x = -((size().height() / 2) - (sizeHint().height() / 2));                       // v-center
   const qreal y = -((size().width() / 2) - (sizeHint().width() / 2)) - fontMetrics.descent(); // h-center
   painter.drawText(QPointF(x, y), text());
}

QSize UtQtVerticalLabel::sizeHint() const
{
   const QSize& size = QLabel::sizeHint();
   return QSize(size.height(), size.width());
}

QSize UtQtVerticalLabel::minimumSizeHint() const
{
   const QSize& size = QLabel::minimumSizeHint();
   return QSize(size.height(), size.width());
}
