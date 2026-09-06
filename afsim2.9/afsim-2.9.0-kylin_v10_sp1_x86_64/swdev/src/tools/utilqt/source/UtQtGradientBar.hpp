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

#ifndef UTQTGRADIENTBAR_HPP
#define UTQTGRADIENTBAR_HPP

#include "utilqt_export.h"

#include <QGradientStops>
#include <QWidget>
#include <QWidgetAction>

#include "UtColor.hpp"

class QPaintEvent;

//! A vertically-oriented bar that will be filled (starting from bottom to the
//! top) with a linear gradient. Includes a one pixel black border.
class UTILQT_EXPORT UtQtGradientBar : public QWidget
{
   Q_OBJECT

public:
   explicit UtQtGradientBar(QWidget* aParent = nullptr);

   void           SetSolidColor(const QColor& aColor);
   void           SetGradientStops(const QGradientStops& aGradient);
   QGradientStops GradientStops() const;

   void SetHorizontal();

protected:
   void paintEvent(QPaintEvent* aEvent) override;

private:
   QGradientStops mGradientStops;
   bool           mHorizontal;
};

class UTILQT_EXPORT UtQtGradientAction : public QWidgetAction
{
public:
   UtQtGradientAction(QObject* aParent, const QGradientStops& aStops, const QString& aKey);
   ~UtQtGradientAction() override = default;

   QWidget*       createWidget(QWidget* aParent) override;
   void           deleteWidget(QWidget* aWidget) override;
   void           GetStandardStops(std::map<float, UtColor>& aStops);
   const QString& GetKey() const { return mKey; }

private:
   QString        mKey;
   QGradientStops mGradientStops;
};

#endif // UTQTGRADIENTBAR_HPP
