// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfHUD_DockWidget.hpp"

#include "VaCamera.hpp"
#include "VaCameraMotionTethered.hpp"
#include "VaViewer.hpp"
#include "WkfHUD_Overlay.hpp"

wkf::HUD_DockWidget::HUD_DockWidget(const QString& aPlatformName, QMainWindow* aParent, Qt::WindowFlags aFlags)
   : ImmersiveViewDockWidget("HUD_DockWidget", aPlatformName, aParent, aFlags)
{
   setFocusPolicy(Qt::StrongFocus);
   mCallbacks.Add(vespa::VaObserver::ViewerResized.Connect(&HUD_DockWidget::ViewerResizedCB, this));
}

void wkf::HUD_DockWidget::SetDefaultWidth(double aWidth)
{
   mDefaultWidth = aWidth;
   setFixedWidth(aWidth);
}

void wkf::HUD_DockWidget::SetDefaultHeight(double aHeight)
{
   mDefaultHeight = aHeight;
   setFixedHeight(aHeight);
}

void wkf::HUD_DockWidget::OverridePerspective(double aVFOV)
{
   mVFOV          = aVFOV;
   int viewWidth  = mViewerPtr->GetWidth();
   int viewHeight = mViewerPtr->GetHeight();

   // Determine the ratio between horizontal/vertical
   double xFactor = (double)viewWidth / (double)viewHeight;

   // Set the target/desired vertical half FOV angle
   double tgtHalfFovY_deg = 0.5 * aVFOV;
   double tgtHalfFovY_rad = tgtHalfFovY_deg * UtMath::cRAD_PER_DEG;

   // Set near/far clipping planes, as desired
   double near_m = 0.01; // 1 cm in front
   double far_m  = 20.0; // 20 m distant

   // To find Y at the near distance, we have tan(tgtHalfFovY_rad) = y/near
   // Thus, we have y = near * tan(tgtHalfFovY_rad)
   double yMax_m = near_m * tan(tgtHalfFovY_rad);
   double yMin_m = -yMax_m;
   double xMin_m = -xFactor * yMax_m;
   double xMax_m = xFactor * yMax_m;

   // Re-adjust near/far clipping planes, as desired
   near_m = 1.0;      // 1 meter in front
   far_m  = 200000.0; // 200 km distant

   // Re-calculate x & y values
   yMax_m = near_m * tan(tgtHalfFovY_rad);
   yMin_m = -yMax_m;
   xMin_m = -xFactor * yMax_m;
   xMax_m = xFactor * yMax_m;

   mViewerPtr->OverridePerspective(xMin_m, xMax_m, yMin_m, yMax_m, near_m, far_m);

   if (mHudPtr)
   {
      mHudPtr->SetupHudProjection(aVFOV);
   }
}

void wkf::HUD_DockWidget::ClearDataPrivate()
{
   mHudPtr = nullptr;
}

void wkf::HUD_DockWidget::ViewerInitializedPrivate()
{
   CompleteConstruction();
}

bool wkf::HUD_DockWidget::HUD_Visible() const
{
   if (mHudPtr)
   {
      return mHudPtr->IsVisible();
   }
   else
   {
      return false;
   }
}

wkf::HUD* wkf::HUD_DockWidget::GetHUD() const
{
   return mHudPtr;
}

void wkf::HUD_DockWidget::ShowHUD(bool aState)
{
   if (aState && !mHudPtr)
   {
      mHudPtr = new wkf::HUD("hud", vespa::VaOverlay::cOVERLAY_WINDOW, mVFOV);
      AddOverlay(mHudPtr);
   }
   if (mHudPtr)
   {
      mHudPtr->SetVisible(aState);
   }
}

void wkf::HUD_DockWidget::ViewerResizedCB(vespa::VaViewer* aViewerPtr)
{
   if (IsMyViewer(aViewerPtr))
   {
      OverridePerspective(mVFOV);

      vespa::VaCameraMotionTethered* cam =
         dynamic_cast<vespa::VaCameraMotionTethered*>(aViewerPtr->GetCamera()->GetCameraMotion());
      if (cam != nullptr)
      {
         cam->AllowUserControl(false);
      }
   }
}
