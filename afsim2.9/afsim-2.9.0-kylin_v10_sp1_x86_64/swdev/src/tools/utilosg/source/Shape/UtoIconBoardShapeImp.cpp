// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoIconBoardShapeImp.cpp : implementation file

#include "UtoIconBoardShapeImp.hpp"

#include <osg/Depth>
#include <osg/Geode>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>

#include "UtoShaders.hpp"
#include "UtoShape.hpp"

UtoIconBoardShapeImp::UtoIconBoardShapeImp()
   : UtoShapeImp()
   , mCards(0)
{
   SetReferenceFrame(UtoShape::cFRAME_ENTITY);

   BuildNode();
}


UtoIconBoardShapeImp::UtoIconBoardShapeImp(const UtoIconBoardShapeImp& aRHS)
   : UtoShapeImp(aRHS)
   , mCards(0)
{
   SetReferenceFrame(UtoShape::cFRAME_ENTITY);

   BuildNode();
}

void UtoIconBoardShapeImp::BuildNode()
{
   SetProgram(UtoShaders::UseProgram("iconboard.vert", "iconboard.frag"), true);
}

UtoIconBoardShapeImp::~UtoIconBoardShapeImp() {}

void UtoIconBoardShapeImp::Clear()
{
   mCards       = 0;
   int children = m_RootNode->getNumChildren();
   while (children > 0)
   {
      osg::Node* child = m_RootNode->getChild(0);
      m_RootNode->removeChild(child);
      child    = nullptr;
      children = m_RootNode->getNumChildren();
   }
}

void UtoIconBoardShapeImp::AddTexture(const std::string& aCardId, unsigned int aWidth, unsigned int aHeight, unsigned char* aBits)
{
   osg::Image*    textureImagePtr = new osg::Image;
   unsigned char* newData         = new unsigned char[aWidth * aHeight * 4];
   memcpy(newData, aBits, aWidth * aHeight * 4);
   textureImagePtr->setImage(aWidth, aHeight, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, newData, osg::Image::USE_NEW_DELETE);

   mTextureMap[aCardId] = new osg::Texture2D(textureImagePtr);
   mTextureMap[aCardId]->setBorderColor(osg::Vec4d(1.0, 1.0, 1.0, 0));
   mTextureMap[aCardId]->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
   mTextureMap[aCardId]->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);
   mTextureMap[aCardId]->setWrap(osg::Texture2D::WRAP_R, osg::Texture2D::CLAMP_TO_BORDER);
   mTextureMap[aCardId]->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_BORDER);
}

void UtoIconBoardShapeImp::AddCard(const std::string& aCardId)
{
   osg::ref_ptr<osg::Geode> geode = new osg::Geode;
   m_RootNode->addChild(geode.get());
   osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
   geode->addDrawable(geom.get());

   int             v     = mCards / 4;
   int             h     = mCards % 4;
   osg::Vec3Array* verts = new osg::Vec3Array;
   verts->push_back(osg::Vec3(h * 16 - 16, -8 + v * 16, 0)); // 0 // 50 here should be multiplied by the model scalar
   verts->push_back(osg::Vec3(h * 16 - 16, 8 + v * 16, 0));  // 1
   verts->push_back(osg::Vec3(h * 16 - 32, -8 + v * 16, 0)); // 2
   verts->push_back(osg::Vec3(h * 16 - 32, 8 + v * 16, 0));  // 3
   geom->setVertexArray(verts);

   osg::Vec2Array* texco = new osg::Vec2Array;
   texco->push_back(osg::Vec2(1, 0));
   texco->push_back(osg::Vec2(1, 1));
   texco->push_back(osg::Vec2(0, 0));
   texco->push_back(osg::Vec2(0, 1));
   geom->setTexCoordArray(0, texco);

   osg::DrawElementsUInt* cube = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
   cube->push_back(0);
   cube->push_back(1);
   cube->push_back(3);
   cube->push_back(2);

   geom->addPrimitiveSet(cube);
   geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, mTextureMap[aCardId]);

   geom->getOrCreateStateSet()->getOrCreateUniform("texture0", osg::Uniform::INT)->set(0);
   mCards++;
}
