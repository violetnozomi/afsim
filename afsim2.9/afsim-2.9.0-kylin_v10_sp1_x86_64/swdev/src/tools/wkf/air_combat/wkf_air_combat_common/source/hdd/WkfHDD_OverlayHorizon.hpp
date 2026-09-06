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

#ifndef WKFHDD_OVERLAYHORIZON_HPP
#define WKFHDD_OVERLAYHORIZON_HPP

#include "wkf_air_combat_common_export.h"

#include <functional>

#include <QRect>
#include <osg/ref_ptr>

#include "UtoRawShape.hpp"
#include "VaOverlay.hpp"

namespace osg
{
class MatrixTransform;
}

namespace HDD
{
/** This type of overlay is used specifically for the PageHorizon page.  This is from the
 * old HDD code, and will probably be re-coded at a later point in time.
 */
class WKF_AIR_COMBAT_COMMON_EXPORT OverlayHorizon : public vespa::VaOverlay
{
public:
   OverlayHorizon(const std::string& aOverlayName);
   OverlayHorizon* Clone() const override;

   /** Initialize the overlay. */
   bool Initialize() override;

   /** Set the function triggered when Button 1 is clicked.
    * @param aFunction Function to use.
    */
   void SetButton1ClickAction(const std::function<bool(int, int, int)>& aFunction);

   /** Set the pitch and roll.
    * @param aPitch Pitch to set.
    * @param aRoll Roll to set.
    */
   void SetOwnshipPitchAndRoll_deg(const float aPitch_deg, const float aRoll_deg);

   /** Set the position of this overlay.
    * @param aX X position to set.
    * @param aY Y position to set.
    */
   void SetPosition(const float aX, const float aY);

   /** Set the size of the overlay.
    * @param aW Width to set.
    * @param aH Height to set.
    */
   void SetSize(const float aW, const float aH);

private:
   /** If events are handled, run mBtn1Function.
    * @param aMouseX X position.
    * @param aMouseY Y position.
    * @param aState button state.
    */
   bool Btn1Click(int aMouseX, int aMouseY, int aState) override;

   /** Return how events are handled for this page. */
   HandleEventMode GetHandleEventMode() const override { return mHandleEventMode; }

   /** Return if the mouse position selected is selectable.
    * @param aMouseX X position.
    * @param aMouseY Y position.
    */
   bool SelectableAt(int aMouseX, int aMouseY) const override;

   std::function<bool(int, int, int)> mBtn1Function;    ///< Button 1 function.
   HandleEventMode                    mHandleEventMode; ///< How to handle events?
   UtoRawShape*                       mShapePtr;        ///< Pointer to the horizon shape.
   osg::ref_ptr<osg::MatrixTransform> mPosMatrixNode;   ///< Transform matrix to position the horizon.
   osg::ref_ptr<osg::MatrixTransform> mHorzMatrixNode;  ///< Transform matrix to rotate/translate the horizon.
   QRectF                             mRect; ///< Rect for this overlay. Used for mouse interaction and geometry.
};
} // namespace HDD

VA_DECLARE_OBJECT_TYPE(HDD::OverlayHorizon)

#endif // WKFHDD_OVERLAYHORIZON_HPP
