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

#ifndef WKFHDD_BUTTON_HPP
#define WKFHDD_BUTTON_HPP

#include "wkf_air_combat_common_export.h"

#include <functional>
#include <memory>

#include <QObject>
#include <QRect>

namespace vespa
{
class VaViewer;
}

namespace HDD
{
class Overlay;
class WKF_AIR_COMBAT_COMMON_EXPORT Button
{
public:
   /** This is the base class for the MenuButton and ArrowButton. */
   explicit Button(const std::string& aOverlayName, vespa::VaViewer* aViewerPtr, const QRectF& aRect = QRect(0, 0, 0, 0));
   ~Button() = default;

   /** Bring the button overlay to the front (if it's visible). */
   void BringOverlayToFront();

   /** Remove this button's overlay from the viewer. */
   void RemoveOverlay();

   /** Draw the overlay. */
   virtual void Draw() = 0;

   /** Return if the button is currently visible. */
   const bool IsVisible() const;

   /** Set the function used for drawing.
    * @param aDrawFunction function to set.
    */
   void SetDrawFunction(const std::function<void()>& aDrawFunction);

   /** Set the function to run when Button 1 is clicked.
    * @param aFunction function to set.
    */
   void SetButton1ClickAction(const std::function<bool(int, int, int)>& aFunction);

   /** Set the position of this button.
    * @param aRect position to set.
    */
   void SetPosition(const QRectF& aRect);

   /** Set if this button should be shown.
    * @param aState true to show, false to hide.
    */
   void SetVisible(const bool aState);

protected:
   Overlay*         mOverlayPtr; ///< Pointer to the overlay.
   vespa::VaViewer* mViewerPtr;  ///< Pointer to the parent viewer.
};
} // namespace HDD
#endif // WKFHDD_BUTTON_HPP
