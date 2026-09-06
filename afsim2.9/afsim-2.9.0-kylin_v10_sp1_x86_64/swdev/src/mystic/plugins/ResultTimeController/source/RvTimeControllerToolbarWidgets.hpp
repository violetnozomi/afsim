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
#ifndef RV_TIME_CONTROLLER_TOOLBAR_WIDGETS_HPP
#define RV_TIME_CONTROLLER_TOOLBAR_WIDGETS_HPP

class QBasicTimer;
#include <QDoubleSpinBox>
#include <QSlider>
#include <QToolBar>

#include "RvEnvironment.hpp"

namespace RvTimeController
{
class Interface;
class CachedSlider : public QSlider
{
   Q_OBJECT
public:
   CachedSlider(QWidget* aParent);
   void setCache(int aMin, int aMax);

   struct BookmarkInfo
   {
      BookmarkInfo(int aPos, const std::string& aType, const std::string& aMsg)
         : pos(aPos)
         , type(aType)
         , message(aMsg)
      {
      }
      int         pos;
      std::string type;
      std::string message;
      // TODO: Add originalTime double as an identifier. Try to "highlight" the bookmark on the bar when it is selected
      // in the browser widget
      QRect currentVisual;
   };
   void SetBookmarks(const std::vector<BookmarkInfo> aBookmarks) { mBookmarks = aBookmarks; }

private:
   void                      paintEvent(QPaintEvent* event) override;
   void                      mouseMoveEvent(QMouseEvent* event) override;
   int                       mCacheMin;
   int                       mCacheMax;
   std::vector<BookmarkInfo> mBookmarks;
};

class ClockRateSpinBox : public QDoubleSpinBox
{
   friend class Toolbar;

   Q_OBJECT

public:
   ClockRateSpinBox(QWidget* parent);

private slots:
   void ValueChanged(double value);
   void Reset();

private:
   QAction* mResetActionPtr;
};

// Subclass QSlider and re-implement paintEvent to draw tick marks correctly based on palette colors
class ClockRateSlider : public QSlider
{
public:
   ClockRateSlider(QWidget* parent);

protected:
   void paintEvent(QPaintEvent* ev) override;
};
} // namespace RvTimeController

#endif
