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

#ifndef UtQtGLWidgetBase_HPP
#define UtQtGLWidgetBase_HPP

#include "utilqt_export.h"

#include <QColor>
#include <QTimer>
#include <qgl.h>

#include "UtQtOpenGLWidget.hpp"

class UTILQT_EXPORT UtQtGLWidgetBase : public QOpenGLWidget
{
   Q_OBJECT
   Q_PROPERTY(QColor bgColor READ GetBackgroundColor WRITE SetBackgroundColor)

public:
   UtQtGLWidgetBase(QWidget* aParent = nullptr);
   ~UtQtGLWidgetBase() override = default;

   // Set the QColor used for drawing the background of the display.
   //  Arguments of R,G,B instead of QColor to indicate that alpha is not used for background color
   virtual void SetBackgroundColor(int aR, int aG, int aB) { mBackgroundColor = QColor(aR, aG, aB); }
   // Created prototype with QColor argument for convenience, but note: Alpha is ignored.
   virtual void SetBackgroundColor(QColor aColor) { mBackgroundColor = aColor; }

   // Get the QColor used for drawing the Background of the Grid
   virtual QColor GetBackgroundColor() const { return mBackgroundColor; }

   // Update the rendering of the Widget
   virtual void Update();

protected:
   virtual void Draw() = 0;
   virtual void QtDraw() {} // if you want to do anything with a QPainter, do it here

   enum UtQtGlTextHorizontalAlignment
   {
      UtQtAlignLeft,
      UtQtAlignRight,
      UtQtAlignHCenter
   };

   enum UtQtGlTextVerticalAlignment
   {
      UtQtAlignTop,
      UtQtAlignBottom,
      UtQtAlignVCenter
   };

   // Qt reimplemented
   void initializeGL() override;
   void resizeGL(int aWidth, int aHeight) override;
   void paintGL() override;

   int mDisplayWidth;  // area width
   int mDisplayHeight; // area height

private:
   QColor mBackgroundColor;
};

#endif
