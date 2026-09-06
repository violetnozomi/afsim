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

#include "UtQtGLWidgetBase.hpp"

#include <qmath.h>

#include "UtMath.hpp"
#include "UtQtInitResources.hpp"

// defining MULTISAMPLE for anti-aliasing in case it is not already defined in the GL lib being used
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

UtQtGLWidgetBase::UtQtGLWidgetBase(QWidget* aParent)
   : QOpenGLWidget(aParent)
   , mBackgroundColor(Qt::white)
{
   setMouseTracking(false);
}

void UtQtGLWidgetBase::initializeGL()
{
   // glEnable(GL_DEPTH_TEST);
   glDisable(GL_DEPTH_TEST);

   // glEnable(GL_TEXTURE_2D);
   glDisable(GL_TEXTURE_2D);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glDisable(GL_STENCIL_TEST);

   // enable anti-aliasing
   glEnable(GL_MULTISAMPLE);

   setAutoFillBackground(false);

   // This is required to initialize the resources when statically linking utilqt
   initUtQtResources();
}

void UtQtGLWidgetBase::resizeGL(int aWidth, int aHeight)
{
   mDisplayWidth  = aWidth;
   mDisplayHeight = aHeight;

   glViewport(0, 0, (GLint)mDisplayWidth, (GLint)mDisplayHeight);

   update();
}

void UtQtGLWidgetBase::Update()
{
   update();
}

void UtQtGLWidgetBase::paintGL()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, mDisplayWidth, 0, mDisplayHeight, -1000.0, 1000.0);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_SMOOTH);

   glClearColor(mBackgroundColor.redF(), mBackgroundColor.greenF(), mBackgroundColor.blueF(), 0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // If not visible dont bother calling Draw()
   if (isVisible())
   {
      Draw();
   }

   QtDraw();

   makeCurrent();
}
