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

#ifndef WKFHDD_OVERLAY_HPP
#define WKFHDD_OVERLAY_HPP

#include "wkf_air_combat_common_export.h"

#include <functional>

#include <QRect>
#include <osg/Group>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Projection>
#include <osg/Transform>
#include <osg/ref_ptr>
#include <osgText/Font>
#include <osgText/Text>

#include "UtoRawShape.hpp"
#include "VaOverlay.hpp"

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT Overlay : public vespa::VaOverlay
{
public:
   Overlay(const char* aName, int aScreenWidth, int aScreenHeight);
   Overlay* Clone() const override;
   ~Overlay() override = default;

   /** Draw this overlay using the draw function. */
   void Update() override;

   /** Return a pointer to the geode for this overlay. */
   osg::ref_ptr<osg::Geode> GetGeode() { return mOverlayGeode; }

   /** Return a pointer to the state set for this overlay. */
   osg::ref_ptr<osg::StateSet> GetState() { return mOverlayState; }

   /** Return a pointer to the color for this overlay. */
   osg::ref_ptr<osg::Vec4Array> GetColor() { return mOverlayColor; }

   /** Return a pointer to the projection for this overlay. */
   osg::ref_ptr<osg::Projection> GetProjection() { return mOverlayProjection; }

   /** Return a pointer to the model view matrix for this overlay. */
   osg::ref_ptr<osg::MatrixTransform> GetModelViewMatrix() { return mOverlayModelViewMatrix; }

   /** Return the font size multiplier. */
   const float GetFontSizeMultiplier() const { return mFontSizeMult; }

   /** Set the font size multiplier.
    * @param aValue value to set.
    */
   void SetFontSizeMultiplier(const float aValue) { mFontSizeMult = aValue; }

   /** Return the line width multiplier. */
   const float GetLineWidthMultiplier() const { return mLineWidthMult; }

   /** Set the line width multiplier.
    * @param aValue value to set.
    */
   void SetLineWidthMultiplier(const float aValue) { mLineWidthMult = aValue; }

   /** Return the width of this overlay in pixels. */
   const int GetWidth() const { return mRect.width(); }

   /** Return the height of this overlay in pixels. */
   const int GetHeight() const { return mRect.height(); }

   /** Return the rect for this overlay. */
   const QRectF& GetRect() const { return mRect; }

   /** Initialize the overlay. */
   bool Initialize() override;

   /** Get the handle event mode for this overlay. */
   HandleEventMode GetHandleEventMode() const override { return mHandleEventMode; }

   /** Set the function to run when Button 1 is clicked.
    * @param aFunction function to set.
    */
   void SetButton1ClickAction(const std::function<bool(int, int, int)>& aFunction);

   /** Set the function to run when the mouse has been moved.
    * @param aFunction function to set.
    */
   void SetMouseMoveAction(const std::function<bool(int, int)>& aFunction);

   /** Set the function used for drawing.
    * @param aDrawFunction function to set.
    */
   void SetDrawFunction(const std::function<void()>& aDrawFunction) { mDrawFunction = aDrawFunction; }

   /** Create geometry using an opengl vec2 array.
    * @param aArray array of points to use to create the geometry.
    * @param aDrawMode draw mode to use.
    * @param aColor color to use.
    */
   osg::ref_ptr<osg::Geode> CreateGeometry(osg::ref_ptr<osg::Vec2Array> aArray, int aDrawMode, const osg::Vec4& aColor);

   // Alignment types include:  LEFT_TOP, LEFT_CENTER, LEFT_BOTTOM,
   //                           CENTER_TOP, CENTER_CENTER, CENTER_BOTTOM,
   //                           RIGHT_TOP, RIGHT_CENTER, RIGHT_BOTTOM,
   //                           LEFT_BASE_LINE, CENTER_BASE_LINE, RIGHT_BASE_LINE,
   //                           LEFT_BOTTOM_BASE_LINE, CENTER_BOTTOM_BASE_LINE, and RIGHT_BOTTOM_BASE_LINE
   /** Add a text item.
    * @param aStr text to add.
    * @param aXPos x position of the text.
    * @param aYPos y position of the text.
    * @param aFontSize size of the font.
    * @param aColor color to use.
    * @param aAlignment alignment of the text.
    */
   osg::ref_ptr<osg::Geode> AddTextItem(const std::string&                 aStr,
                                        const double                       aXPos,
                                        const double                       aYPos,
                                        const float                        aFontSize,
                                        const osg::Vec4&                   aColor,
                                        const osgText::Text::AlignmentType aAlignment = osgText::Text::LEFT_BASE_LINE);

   /** Fill the background
    * @param aColor color of the background.
    */
   void DrawBackground(const osg::Vec4& aColor);

   /** Draw an outline around the page.
    * @param aColor color of the outline.
    */
   void DrawOutline(const osg::Vec4& aColor);

   /** Draw text on the overlay.
    * @param aXPos x position.
    * @param aYPos y position.
    * @param aText text to draw.
    * @param aFontSize font size to use.
    * @param aColor color to use.
    * @param aAlignment text alignment to use.
    */
   void DrawLabel(const double                 aXPos,
                  const double                 aYPos,
                  const std::string&           aText,
                  const float                  aFontSize,
                  const osg::Vec4&             aColor,
                  osgText::Text::AlignmentType aAlignment);

   /** Set the position of this overlay, adjusting the modelview matrix transform to match.
    * @param aX x position to set.
    * @param aY y position to set.
    */
   void SetPosition(float aX, float aY);

   /** Set the size of this overlay, adjusting the modelview matrix
    * transform and scissor state set to match.
    * @param aW width to set.
    * @param aH height to set.
    */
   void SetSize(float aW, float aH);

   /** This sets if the connection to the HUD has been destroyed/ lost (draw x over screen).
    * @param aState state to set.
    */
   void SetConnectionDestroyed(const bool aState) { mConnectionDestroyed = aState; }

   /** Draw a black fade over the screen with a red x, indicating the connection to the platform has been lost/destroyed. */
   void DrawConnectionDestroyed();

private:
   /** Handle a Button 1 click.
    * @param aMouseX X mouse position.
    * @param aMouseY Y mouse position.
    * @param aState state of the button.
    */
   bool Btn1Click(int aMouseX, int aMouseY, int aState) override;

   /** Handle mouse movement.
    * @param aMouseX X mouse position.
    * @param aMouseY Y mouse position.
    */
   bool MouseMove(int aMouseX, int aMouseY) override;

   /** Set the selectable region for this overlay.
    * @param aMouseX X region.
    * @param aMouseY Y region.
    */
   bool SelectableAt(int aMouseX, int aMouseY) const override;

   HandleEventMode                    mHandleEventMode;        ///< How to handle events.
   std::function<bool(int, int)>      mMouseMoveFunction;      ///< Mouse move function.
   std::function<bool(int, int, int)> mBtn1Function;           ///< Button 1 click function.
   QRectF                             mRect;                   ///< Rectangle representing geometry.
   std::function<void()>              mDrawFunction;           ///< Draw function for this overlay.
   int                                mScreenWidth;            ///< Screen width.
   int                                mScreenHeight;           ///< Screen height.
   osgText::Font*                     mFont         = nullptr; ///< Pointer to the font object.
   osg::ref_ptr<osg::Geode>           mOverlayGeode = nullptr; ///< Pointer to the geode used by the overlay.
   osg::ref_ptr<osg::StateSet>        mOverlayState = nullptr; ///< Pointer to the state set for the overlay.
   osg::ref_ptr<osg::Vec4Array>       mOverlayColor = nullptr; ///< Pointer to the color used by the overlay.
   osg::ref_ptr<osg::Projection> mOverlayProjection = nullptr; ///< Pointer to the projection matrix used by the overlay.
   osg::ref_ptr<osg::MatrixTransform> mOverlayModelViewMatrix =
      nullptr; ///< Pointer to the model view matrix used by the overlay.  This positions the center of the page.
   UtoRawShape* mRawShapePtr         = nullptr; ///< Pointer to the raw shape used by the overlay.
   float        mLineWidthMult       = 1.0f;    ///< Multiplier for the line width.
   float        mFontSizeMult        = 1.0f;    ///< Multiplier for the font size.
   bool         mConnectionDestroyed = false;   ///< True to draw connection destroyed (x over screen), false otherwise.
};
} // namespace HDD

VA_DECLARE_OBJECT_TYPE(HDD::Overlay)

#endif // WKFHDD_OVERLAY_HPP
