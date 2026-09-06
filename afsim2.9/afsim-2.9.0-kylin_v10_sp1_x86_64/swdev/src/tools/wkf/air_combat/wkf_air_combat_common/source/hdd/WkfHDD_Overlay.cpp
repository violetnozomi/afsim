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

#include "hdd/WkfHDD_Overlay.hpp"

#include <osg/Depth>
#include <osg/LineStipple>
#include <osg/LineWidth>
#include <osg/PositionAttitudeTransform>
#include <osg/PrimitiveSet>
#include <osg/Scissor>
#include <osg/StateSet>
#include <osg/Stencil>

#include "UtLog.hpp"
#include "UtPath.hpp"
#include "Uto.hpp"
#include "VaUtils.hpp"

HDD::Overlay::Overlay(const char* aName, int aScreenWidth, int aScreenHeight)
   : vespa::VaOverlay(aName, vespa::VaOverlay::cOVERLAY_WINDOW)
   , mHandleEventMode(vespa::VaOverlay::cHANDLE_EVENTS_NEVER)
   , mScreenWidth(aScreenWidth)
   , mScreenHeight(aScreenHeight)
   , mFontSizeMult(1.0f)
{
   SetType<HDD::Overlay>();
}

HDD::Overlay* HDD::Overlay::Clone() const
{
   return new HDD::Overlay(*this);
}

bool HDD::Overlay::Initialize()
{
   bool initialized = false;

   UtoRawShape shapePtr;
   mRawShapePtr = dynamic_cast<UtoRawShape*>(BindShape("hdd_gl", shapePtr));
   if (mRawShapePtr == nullptr)
   {
      auto out = ut::log::warning() << "Couldn't bind shape: hdd_gl";
   }
   else
   {
      SetVisible(true);
      initialized = true;

      // Create our geode if we don't have one.
      if (!mOverlayGeode)
      {
         mOverlayGeode = new osg::Geode;
      }

      // Create our state set if we don't have one.
      // This will set a 2D state, making sure it renders in the front.
      if (!mOverlayState)
      {
         mOverlayState = new osg::StateSet;
         // Disable depth testing so geometry is draw regardless of depth values
         // of geometry already draw.
         mOverlayState->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
         mOverlayState->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

         // Need to make sure this geometry is draw last. RenderBins are handled
         // in numerical order so set bin number to 11
         mOverlayState->setRenderBinDetails(11, "RenderBin");

         mOverlayGeode->setStateSet(mOverlayState);
      }

      // Set some more state set vars.  We don't need lighting, and we'd like smooth lines.
      mOverlayModelViewMatrix = new osg::MatrixTransform; // screen location
      mOverlayModelViewMatrix->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      mOverlayModelViewMatrix->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
      mRawShapePtr->GetRoot()->setCullingActive(false);
      mRawShapePtr->GetRoot()->addChild(mOverlayModelViewMatrix.get());

      mOverlayModelViewMatrix->addChild(mOverlayGeode.get());
   }

   return initialized;
}

void HDD::Overlay::Update()
{
   VaOverlay::Update();

   // Clear out the geodes drawables,
   mOverlayGeode->removeDrawables(0, mOverlayGeode->getNumDrawables());

   // Then redraw.
   if (mDrawFunction)
   {
      mDrawFunction();
   }

   if (mConnectionDestroyed)
   {
      DrawConnectionDestroyed();
   }
}

bool HDD::Overlay::Btn1Click(int aMouseX, int aMouseY, int aState)
{
   // If we handle events, call the function set for clicking.
   if (mHandleEventMode != vespa::VaOverlay::cHANDLE_EVENTS_NEVER && mBtn1Function)
   {
      return mBtn1Function(aMouseX, aMouseY, aState);
   }
   else
   {
      return false;
   }
}

bool HDD::Overlay::MouseMove(int aMouseX, int aMouseY)
{
   // If we handle events, call the function for a mouse movement event.
   if (mHandleEventMode != vespa::VaOverlay::cHANDLE_EVENTS_NEVER && mMouseMoveFunction)
   {
      return mMouseMoveFunction(aMouseX, aMouseY);
   }
   else
   {
      return false;
   }
}

bool HDD::Overlay::SelectableAt(int aMouseX, int aMouseY) const
{
   // If the overlay is visible and our mouse is inside it's rect, return true.
   if (IsVisible() && (aMouseX > mRect.x()) && (aMouseX < mRect.x() + mRect.width()) && (aMouseY > mRect.y()) &&
       (aMouseY < mRect.y() + mRect.height()))
   {
      return true;
   }
   return false;
}

void HDD::Overlay::SetButton1ClickAction(const std::function<bool(int, int, int)>& aFunction)
{
   mHandleEventMode = vespa::VaOverlay::cHANDLE_EVENTS_FOCUS;
   mBtn1Function    = aFunction;
}

void HDD::Overlay::SetMouseMoveAction(const std::function<bool(int, int)>& aFunction)
{
   mHandleEventMode   = vespa::VaOverlay::cHANDLE_EVENTS_FOCUS;
   mMouseMoveFunction = aFunction;
}

osg::ref_ptr<osg::Geode> HDD::Overlay::CreateGeometry(osg::ref_ptr<osg::Vec2Array> aArray,
                                                      int                          aDrawMode,
                                                      const osg::Vec4&             aColor)
{
   // Create a geode and geometry node.
   osg::ref_ptr<osg::Geometry> geom(new osg::Geometry);
   osg::ref_ptr<osg::Geode>    retGeode(new osg::Geode);

   // Plug in our vertex list and set the draw mode for the geometry.
   osg::ref_ptr<osg::DrawArrays> geomArray(new osg::DrawArrays(aDrawMode, 0, aArray->size()));
   geom->setVertexArray(aArray.get());
   geom->addPrimitiveSet(geomArray.get());

   // Populate a color array and bind it to all vertices.
   osg::ref_ptr<osg::Vec4Array> colorArray(new osg::Vec4Array);
   colorArray->push_back(aColor);

   geom->setColorArray(colorArray);
   geom->setColorBinding(osg::Geometry::BIND_OVERALL);

   // Set a state set for blended, smooth lines.
   osg::ref_ptr<osg::StateSet>  lineState(new osg::StateSet);
   osg::ref_ptr<osg::LineWidth> lineWidthAttr(new osg::LineWidth);
   lineWidthAttr->setWidth(1.0f * mLineWidthMult);
   lineState->setAttribute(lineWidthAttr.get());
   lineState->setMode(GL_LINE_SMOOTH, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   lineState->setMode(GL_BLEND, osg::StateAttribute::ON);
   retGeode->setStateSet(lineState.get());

   // Add the drawable and return.
   retGeode->addDrawable(geom.get());
   return retGeode;
}

osg::ref_ptr<osg::Geode> HDD::Overlay::AddTextItem(const std::string&                 aStr,
                                                   const double                       aXPos,
                                                   const double                       aYPos,
                                                   const float                        aFontSize,
                                                   const osg::Vec4&                   aColor,
                                                   const osgText::Text::AlignmentType aAlignment)
{
   // Create a text item and set the font size.
   osg::ref_ptr<osgText::Text> textItem(new osgText::Text());
   textItem->setCharacterSize(aFontSize);

   // Set alignment
   textItem->setAlignment(aAlignment);

   // If we don't have a font, load it.
   if (mFont == nullptr)
   {
      std::string resourceDir = "";
      vespa::VaUtils::GetPathPrioritizedFilename("fonts/DejaVuSansMono.ttf", resourceDir);
      UtPath path(resourceDir);
      path.Up();

      // Find the font in the available resource directories
      // If an empty string is returned, OSG will find some font to use

      UtPath wd = UtPath::WorkingDirectory();
      path.SetWorkingDirectory();
      mFont = osgText::readFontFile("DejaVuSansMono.ttf");
      wd.SetWorkingDirectory();
   }

   // Set our text options.
   textItem->setFont(mFont);
   textItem->setText(aStr);
   textItem->setAxisAlignment(osgText::Text::SCREEN);
   textItem->setPosition(osg::Vec3d(aXPos, aYPos, 0));
   textItem->setColor(aColor);

   // Add the finished drawable.
   osg::ref_ptr<osg::Geode> geoNode(new osg::Geode);
   geoNode->addDrawable(textItem);
   return geoNode;
}

void HDD::Overlay::DrawBackground(const osg::Vec4& aColor)
{
   // Background rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d(mRect.width() * 0.5, mRect.height() * 0.5));   // Top right
   boxArray->push_back(osg::Vec2d(mRect.width() * 0.5, -mRect.height() * 0.5));  // Bottom right
   boxArray->push_back(osg::Vec2d(-mRect.width() * 0.5, -mRect.height() * 0.5)); // Bottom left
   boxArray->push_back(osg::Vec2d(-mRect.width() * 0.5, mRect.height() * 0.5));  // Top left
   mOverlayGeode->addChild(CreateGeometry(boxArray, GL_POLYGON, aColor).get());
}

void HDD::Overlay::DrawOutline(const osg::Vec4& aColor)
{
   // This draws a box around the edge of the overlay, giving it an outline.
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d(mRect.width() * 0.4995, mRect.height() * 0.4995));   // Top right
   boxArray->push_back(osg::Vec2d(mRect.width() * 0.4995, -mRect.height() * 0.4995));  // Bottom right
   boxArray->push_back(osg::Vec2d(-mRect.width() * 0.4995, -mRect.height() * 0.4995)); // Bottom left
   boxArray->push_back(osg::Vec2d(-mRect.width() * 0.4995, mRect.height() * 0.4995));  // Top left

   mLineWidthMult = 2.0f;
   mOverlayGeode->addChild(CreateGeometry(boxArray, GL_LINE_LOOP, aColor).get());
   mLineWidthMult = 1.0f;
}

void HDD::Overlay::DrawLabel(const double                 aXPos,
                             const double                 aYPos,
                             const std::string&           aText,
                             const float                  aFontSize,
                             const osg::Vec4&             aColor,
                             osgText::Text::AlignmentType aAlignment)
{
   // Positioning transform
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aXPos, aYPos, 0.0));

   // Text label
   vectorXform->addChild(AddTextItem(aText, 0.0, 0.0, aFontSize, aColor, aAlignment).get());

   // Add the label to the geode
   mOverlayGeode->addChild(vectorXform.get());
}

void HDD::Overlay::SetPosition(float aX, float aY)
{
   // Set the position of this overlay and update the modelview matrix to match.
   mRect.setX(aX);
   mRect.setY(aY);
   mOverlayModelViewMatrix->setMatrix(
      osg::Matrix::translate(osg::Vec3(mRect.x() + mRect.width() * 0.5, mRect.y() + mRect.height() * 0.5, 0)));
}

void HDD::Overlay::SetSize(float aW, float aH)
{
   // Set the size of this overlay and update the modelview matrix and scissor state set to match.
   mRect.setWidth(aW);
   mRect.setHeight(aH);
   mOverlayModelViewMatrix->setMatrix(
      osg::Matrix::translate(osg::Vec3(mRect.x() + mRect.width() * 0.5, mRect.y() + mRect.height() * 0.5, 0)));
   osg::StateSet* ss = mRawShapePtr->GetRoot()->getOrCreateStateSet();
   ss->setAttributeAndModes(new osg::Scissor(mRect.x(), mRect.y(), mRect.width(), mRect.height()));
}

void HDD::Overlay::DrawConnectionDestroyed()
{
   int h = mRect.height();
   int w = mRect.width();

   // Black color
   const osg::Vec4 blackColor = osg::Vec4(0.0, 0.0, 0.0, 0.5);

   // Red color
   const osg::Vec4 redColor = osg::Vec4(0.9, 0.0, 0.0, 1.0);

   // Fade background
   osg::ref_ptr<osg::Vec2Array> bgArray(new osg::Vec2Array);
   bgArray->push_back(osg::Vec2d(-0.5 * w, 0.5 * h));
   bgArray->push_back(osg::Vec2d(0.5 * w, 0.5 * h));
   bgArray->push_back(osg::Vec2d(0.5 * w, -0.5 * h));
   bgArray->push_back(osg::Vec2d(-0.5 * w, -0.5 * h));
   mOverlayGeode->addChild(CreateGeometry(bgArray, GL_POLYGON, blackColor).get());

   // Top left to bottom right slash
   osg::ref_ptr<osg::Vec2Array> x1array(new osg::Vec2Array);
   x1array->push_back(osg::Vec2d(-0.5 * w, 0.5 * h));
   x1array->push_back(osg::Vec2d(-0.42 * w, 0.5 * h));
   x1array->push_back(osg::Vec2d(0.5 * w, -0.5 * h));
   x1array->push_back(osg::Vec2d(0.42 * w, -0.5 * h));
   mOverlayGeode->addChild(CreateGeometry(x1array, GL_POLYGON, redColor).get());

   // Bottom left to top right slash
   osg::ref_ptr<osg::Vec2Array> x2array(new osg::Vec2Array);
   x2array->push_back(osg::Vec2d(-0.42 * w, -0.5 * h));
   x2array->push_back(osg::Vec2d(-0.5 * w, -0.5 * h));
   x2array->push_back(osg::Vec2d(0.42 * w, 0.5 * h));
   x2array->push_back(osg::Vec2d(0.5 * w, 0.5 * h));
   mOverlayGeode->addChild(CreateGeometry(x2array, GL_POLYGON, redColor).get());
}
