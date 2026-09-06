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


#include "AnimatedGeometryGLWidget.hpp"

#include "GeometryBody.hpp"
#include "GeometryDish.hpp"
#include "GeometryFuselage.hpp"
#include "GeometryLandingGear.hpp"
#include "GeometryNacelle.hpp"
#include "GeometrySpeedBrake.hpp"
#include "GeometrySurface.hpp"
#include "GeometryWing.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "Vehicle.hpp"

namespace Designer
{

AnimatedGeometryGLWidget::AnimatedGeometryGLWidget(QWidget* aParent)
   : GeometryGLWidget(aParent)
{
   // Configure various settings for proper viewing of the animation
   mShowAxis                             = false;
   mShowSelectedObjectCG                 = false;
   mShowEngines                          = true;
   mShowThrustVectors                    = false;
   mShowPointMasses                      = false;
   mShowFuelTanks                        = false;
   mShowVehicleCG                        = false;
   mShowLandingGear                      = true;
   mShowSpeedBrakes                      = true;
   mShowAllAeroVectors                   = false;
   mShowSelectedAeroVectors              = false;
   mSaveOptions                          = false;
}

AnimatedGeometryGLWidget::~AnimatedGeometryGLWidget()
{
   delete mAnimationTimer;
}

void AnimatedGeometryGLWidget::Draw()
{
   Draw3dView();
}

void AnimatedGeometryGLWidget::DrawItems()
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();

   CalcViewRegionCharacteristics();
   SetDrawingNothingSelectedMode();
   DrawVehicle(vehicle, false); // Draw all objects
}

void AnimatedGeometryGLWidget::StartAnimationTimer()
{
   if (mAnimationTimer == nullptr)
   {
      mAnimationTimer = new QTimer(this);
      connect(mAnimationTimer, &QTimer::timeout, this, &AnimatedGeometryGLWidget::UpdateAnimation);
   }
   mAnimationTimer->start(10);
}

void AnimatedGeometryGLWidget::UpdateAnimation()
{
   mViewAz_deg -= 0.1;
   if (mViewAz_deg < 0.0) { mViewAz_deg += 360.0; }
   if (mViewAz_deg > 360.0) { mViewAz_deg -= 360.0; }
   ReCalcEyePoint();
   Update();
}

void AnimatedGeometryGLWidget::PauseAnimationTimer()
{
   if (mAnimationTimer != nullptr)
   {
      mAnimationTimer->stop();
   }
}

} // namespace Designer
