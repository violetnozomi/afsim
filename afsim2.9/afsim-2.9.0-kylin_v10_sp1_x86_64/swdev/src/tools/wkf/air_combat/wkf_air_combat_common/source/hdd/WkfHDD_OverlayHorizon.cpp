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

#include "WkfHDD_OverlayHorizon.hpp"

#include <QString>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Scissor>
#include <osgText/Text>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtoRawShape.hpp"

HDD::OverlayHorizon::OverlayHorizon(const std::string& aOverlayName)
   : vespa::VaOverlay(aOverlayName, VaOverlay::cOVERLAY_WINDOW)
   , mHandleEventMode(vespa::VaOverlay::cHANDLE_EVENTS_NEVER)
   , mShapePtr(nullptr)
   , mRect()
{
   SetType<OverlayHorizon>();
}

bool HDD::OverlayHorizon::Btn1Click(int aMouseX, int aMouseY, int aState)
{
   if (mHandleEventMode != vespa::VaOverlay::cHANDLE_EVENTS_NEVER)
   {
      return mBtn1Function(aMouseX, aMouseY, aState);
   }
   else
   {
      return false;
   }
}

HDD::OverlayHorizon* HDD::OverlayHorizon::Clone() const
{
   return new HDD::OverlayHorizon(*this);
}

bool HDD::OverlayHorizon::Initialize()
{
   bool initialized = false;

   UtoRawShape shapePtr;
   mShapePtr = dynamic_cast<UtoRawShape*>(BindShape("hdd_horz", shapePtr));
   if (mShapePtr == nullptr)
   {
      auto out = ut::log::warning() << "Couldn't bind shape: hdd_horz";

      mPosMatrixNode = new osg::MatrixTransform; // screen location
      mPosMatrixNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      mPosMatrixNode->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
   }
   else
   {
      SetVisible(true);
      initialized = true;

      mPosMatrixNode = new osg::MatrixTransform; // screen location
      mPosMatrixNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      mPosMatrixNode->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);

      mShapePtr->GetRoot()->setCullingActive(false);
      mShapePtr->GetRoot()->addChild(mPosMatrixNode.get());
   }

   mHorzMatrixNode = new osg::MatrixTransform; // horizon rotation / translation
   mPosMatrixNode->addChild(mHorzMatrixNode.get());

   osg::ref_ptr<osg::Geode> horzGeode  = new osg::Geode;
   osg::ref_ptr<osg::Geode> horzGeode2 = new osg::Geode;
   mHorzMatrixNode->addChild(horzGeode);
   mHorzMatrixNode->addChild(horzGeode2);
   osg::ref_ptr<osg::Geometry> geom1 = new osg::Geometry;
   osg::Vec3Array*             v1    = new osg::Vec3Array;
   osg::Vec4Array*             c1    = new osg::Vec4Array;
   osg::Vec4                   brown(0.294f, 0.192f, 0.196f, 1.0f);
   osg::Vec4                   dkbrown(brown.x() * 0.5f, brown.y() * 0.5f, brown.z() * 0.5f, 1.0f);
   osg::Vec4                   blue(0.133f, 0.353f, 0.553f, 1.0f);
   osg::Vec4                   white(1.0f, 1.0f, 1.0f, 1.0f);
   v1->push_back(osg::Vec3(-500, -1000, -1));
   v1->push_back(osg::Vec3(500, -1000, -1));
   v1->push_back(osg::Vec3(500, 0, -1));
   v1->push_back(osg::Vec3(-500, 0, -1));
   v1->push_back(osg::Vec3(-500, 0, -1));
   v1->push_back(osg::Vec3(500, 0, -1));
   v1->push_back(osg::Vec3(500, 1000, -1));
   v1->push_back(osg::Vec3(-500, 1000, -1));
   c1->push_back(dkbrown);
   c1->push_back(dkbrown);
   c1->push_back(brown);
   c1->push_back(brown);
   c1->push_back(blue);
   c1->push_back(blue);
   c1->push_back(blue);
   c1->push_back(blue);

   v1->push_back(osg::Vec3(-80, 0, 0));
   c1->push_back(white);
   v1->push_back(osg::Vec3(-20, 0, 0));
   c1->push_back(white);
   v1->push_back(osg::Vec3(20, 0, 0));
   c1->push_back(white);
   v1->push_back(osg::Vec3(80, 0, 0));
   c1->push_back(white);
   osgText::Font* font = osgText::readFontFile("arial.ttf");
   for (int i = 10; i < 90; i += 10)
   {
      int y = i * 6;
      v1->push_back(osg::Vec3(-60, y - 6, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(-60, y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(-60, y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(-20, y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(60, y - 6, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(60, y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(60, y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(20, y, 0));
      c1->push_back(white);

      v1->push_back(osg::Vec3(-60, -y + 6, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(-60, -y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(-60, -y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(-20, -y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(60, -y + 6, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(60, -y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(60, -y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(20, -y, 0));
      c1->push_back(white);
      osgText::Text* tp = new osgText::Text();
      osgText::Text* tn = new osgText::Text();
      tp->setFont(font);
      tp->setAlignment(osgText::Text::LEFT_BOTTOM);
      tp->setCharacterSize(12);
      tp->setPosition(osg::Vec3(-55, y - 5, 0.1));
      tp->setText(QString::number(i).toStdString());
      tn->setFont(font);
      tp->setAlignment(osgText::Text::LEFT_TOP);
      tn->setCharacterSize(12);
      tn->setPosition(osg::Vec3(-55, -y + 5, 0.1));
      tn->setText(QString::number(-i).toStdString());

      horzGeode2->addDrawable(tp);
      horzGeode2->addDrawable(tn);
   }

   geom1->setVertexArray(v1);
   geom1->setColorArray(c1);
   geom1->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
   geom1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 8));
   geom1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 8, 132));
   horzGeode->addDrawable(geom1);

   osg::ref_ptr<osg::Geode>    centerNode = new osg::Geode;
   osg::ref_ptr<osg::Geometry> geom2      = new osg::Geometry;
   centerNode->addDrawable(geom2);
   mPosMatrixNode->addChild(centerNode.get());
   osg::Vec3Array* v2 = new osg::Vec3Array;
   v2->push_back(osg::Vec3(-15, 0, 0));
   v2->push_back(osg::Vec3(-10, 0, 0));
   v2->push_back(osg::Vec3(-5, -7, 0));
   v2->push_back(osg::Vec3(0, 0, 0));
   v2->push_back(osg::Vec3(5, -7, 0));
   v2->push_back(osg::Vec3(10, 0, 0));
   v2->push_back(osg::Vec3(15, 0, 0));
   osg::Vec4Array* c2 = new osg::Vec4Array;
   c2->push_back(white);
   c2->push_back(white);
   c2->push_back(white);
   c2->push_back(white);
   c2->push_back(white);
   c2->push_back(white);
   c2->push_back(white);
   geom2->setVertexArray(v2);
   geom2->setColorArray(c2);
   geom2->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
   geom2->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, 7));

   return initialized;
}

bool HDD::OverlayHorizon::SelectableAt(int aMouseX, int aMouseY) const
{
   if ((aMouseX > mRect.x()) && (aMouseX < mRect.x() + mRect.width()) && (aMouseY > mRect.y()) &&
       (aMouseY < mRect.y() + mRect.height()))
   {
      return true;
   }
   return false;
}

void HDD::OverlayHorizon::SetButton1ClickAction(const std::function<bool(int, int, int)>& aFunction)
{
   mHandleEventMode = vespa::VaOverlay::cHANDLE_EVENTS_FOCUS;
   mBtn1Function    = aFunction;
}

void HDD::OverlayHorizon::SetOwnshipPitchAndRoll_deg(const float aPitch_deg, const float aRoll_deg)
{
   osg::Matrix mat;
   float       poffset = aPitch_deg * 6;
   mat.preMultRotate(osg::Quat(aRoll_deg * UtMath::cRAD_PER_DEG, osg::Vec3(0, 0, 1)));
   mat.preMultTranslate(osg::Vec3(0, -poffset, 0));
   mHorzMatrixNode->setMatrix(mat);
}

void HDD::OverlayHorizon::SetPosition(const float aX, const float aY)
{
   mRect.setX(aX);
   mRect.setY(aY);
   mPosMatrixNode->setMatrix(
      osg::Matrix::translate(osg::Vec3(mRect.x() + mRect.width() * 0.5, mRect.y() + mRect.height() * 0.5, 0)));
}

void HDD::OverlayHorizon::SetSize(float aW, float aH)
{
   mRect.setWidth(aW);
   mRect.setHeight(aH);
   mPosMatrixNode->setMatrix(
      osg::Matrix::translate(osg::Vec3(mRect.x() + mRect.width() * 0.5, mRect.y() + mRect.height() * 0.5, 0)));
   osg::StateSet* ss = mShapePtr->GetRoot()->getOrCreateStateSet();
   ss->setAttributeAndModes(new osg::Scissor(mRect.x(), mRect.y(), mRect.width(), mRect.height()));
}
