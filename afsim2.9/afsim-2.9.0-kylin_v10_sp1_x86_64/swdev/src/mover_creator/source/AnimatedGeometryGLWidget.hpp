// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ANIMATED_GEOMETRY_GL_WIDGET_HPP
#define ANIMATED_GEOMETRY_GL_WIDGET_HPP

#include "GeometryGLWidget.hpp"
#include <QTimer>

namespace Designer
{
   class AnimatedGeometryGLWidget : public GeometryGLWidget
   {
      public:
         explicit AnimatedGeometryGLWidget(QWidget* aParent = nullptr);
         ~AnimatedGeometryGLWidget();
         void StartAnimationTimer();
         void PauseAnimationTimer();
         void UpdateAnimation();

      protected:
         void Draw() override;
         void DrawItems() override;

      private:
         QTimer*  mAnimationTimer = nullptr;
   };
}
#endif
