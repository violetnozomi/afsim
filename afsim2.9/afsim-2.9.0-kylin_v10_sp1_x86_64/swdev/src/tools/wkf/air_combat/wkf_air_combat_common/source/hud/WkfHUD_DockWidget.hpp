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
#ifndef HUD_DOCKWIDGET_HPP
#define HUD_DOCKWIDGET_HPP

#include "wkf_air_combat_common_export.h"

#include "misc/WkfImmersiveViewDockWidget.hpp"

namespace wkf
{
class HUD;
class WKF_AIR_COMBAT_COMMON_EXPORT HUD_DockWidget : public ImmersiveViewDockWidget
{
public:
   HUD_DockWidget(const QString& aPlatformName, QMainWindow* aParent = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());

   /** This returns true if the HUD is visible/shown, false otherwise. */
   bool HUD_Visible() const;

   /** Set the FOV when overriding perspective.
    * @param aVFOV vertical FOV to set.
    */
   void OverridePerspective(double aVFOV);

   /** Returns a pointer to the HUD object. */
   HUD* GetHUD() const;

   /** Show or hide the HUD.
    * @param aState true to show, false to hide.
    */
   void ShowHUD(bool aState);

   /** Set the default width for the widget.
    * @param aWidth width to set.
    */
   void SetDefaultWidth(double aWidth);

   /** Set the default height for the widget.
    * @param aHeight height to set.
    */
   void SetDefaultHeight(double aHeight);

protected:
   /** Clear the HUD. */
   void ClearDataPrivate() override;

private:
   /** Callback for the viewer being resized.
    * @param aViewerPtr pointer to the viewer being resized.
    */
   void ViewerResizedCB(vespa::VaViewer* aViewerPtr);

   /** Called when the viewer is initialized. */
   void ViewerInitializedPrivate() override;

   HUD*   mHudPtr        = nullptr;
   double mVFOV          = 44.0;
   double mDefaultWidth  = 0;
   double mDefaultHeight = 0;
};
} // namespace wkf
#endif // HUD_DOCKWIDGET_HPP
