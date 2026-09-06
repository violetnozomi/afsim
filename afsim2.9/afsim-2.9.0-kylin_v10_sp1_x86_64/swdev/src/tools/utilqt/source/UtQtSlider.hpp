// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTQTSLIDER_HPP
#define UTQTSLIDER_HPP

#include <QMouseEvent>
#include <QSlider>
#include <QStyle>

#include "UtCallback.hpp"
#include "UtQt.hpp"

class UTILQT_EXPORT UtQtSlider : public QSlider
{
public:
   UtQtSlider(QWidget* parent = nullptr)
      : QSlider(parent)
      , mProgramChange(0)
   {
      QObject::connect(this, &UtQtSlider::valueChanged, this, &UtQtSlider::ValueChangedCB);
      setMouseTracking(true);
   }
   void mousePressEvent(QMouseEvent* ev) override
   {
      if (ev->buttons() & Qt::LeftButton)
      {
         int slide = QStyle::sliderValueFromPosition(minimum(), maximum(), ev->x(), width());
         QSlider::setValue(slide);
      }
   }
   void mouseMoveEvent(QMouseEvent* ev) override
   {
      if (ev->buttons() & Qt::LeftButton)
      {
         int slide = QStyle::sliderValueFromPosition(minimum(), maximum(), ev->x(), width());
         QSlider::setValue(slide);
      }
   }

   void setValue(int x)
   {
      ++mProgramChange;
      QSlider::setValue(x);
      --mProgramChange;
   }

   UtCallbackListN<void(int)> userChangedValue;

private:
   void ValueChangedCB(int x)
   {
      if (mProgramChange == 0)
      {
         userChangedValue(x);
      }
   }
   int mProgramChange;
};
#endif
