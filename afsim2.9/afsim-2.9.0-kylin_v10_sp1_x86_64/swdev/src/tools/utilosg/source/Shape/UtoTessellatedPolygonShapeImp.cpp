// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoTessellatedPolygonShapeImp.cpp : implementation file

#include "UtoTessellatedPolygonShapeImp.hpp"

#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/PrimitiveSet>
#include <osgUtil/Tessellator>

#include "UtEntity.hpp"
#include "UtSphericalEarth.hpp"
#include "UtoEntity.hpp"
#include "UtoGeometryUtil.hpp"
#include "UtoTessellatedPolygonShape.hpp"

UtoTessellatedPolygonShapeImp::UtoTessellatedPolygonShapeImp()
   : UtoShapeImp()
   , mRoot(new osg::Geode())
   , mGeometry(new osg::Geometry())
   , mVertexArray(new osg::Vec3Array())
   , mOffset()
   , mClosed(false)
   , mDepthTest(true)
{
   // Register the heading transform at 0 deg
   mOrientationTransform = new osg::MatrixTransform;
   mOrientationTransform->addChild(mRoot.get());
   AddShapeChildNode(mOrientationTransform);

   mRoot->addDrawable(mGeometry.get());
   mRoot->setName("UtoTessellatedPolygonShape");

   // setup the color
   osg::Vec4Array* color = new osg::Vec4Array;
   color->push_back(osg::Vec4(1, 1, 1, 1));
   mGeometry->setColorArray(color);
   mGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

   mGeometry->setVertexArray(mVertexArray.get());
}

UtoTessellatedPolygonShapeImp::UtoTessellatedPolygonShapeImp(const UtoPosition pts[], int num)
   : UtoShapeImp()
   , mRoot(new osg::Geode)
   , mGeometry(new osg::Geometry)
   , mVertexArray(new osg::Vec3Array())
   , mOffset()
   , mClosed(false)
   , mDepthTest(true)
{
   // Register the heading transform at 0 deg
   mOrientationTransform = new osg::MatrixTransform;
   mOrientationTransform->addChild(mRoot.get());
   AddShapeChildNode(mOrientationTransform);

   mRoot->addDrawable(mGeometry.get());
   mRoot->setName("UtoTessellatedPolygonShape");

   mVertexArray = new osg::Vec3Array;
   mGeometry->setVertexArray(mVertexArray.get());

   // setup the color
   osg::Vec4Array* color = new osg::Vec4Array;
   color->push_back(osg::Vec4(1, 1, 1, 1));
   mGeometry->setColorArray(color);
   mGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

   Insert(0, pts, num);
}

UtoTessellatedPolygonShapeImp::UtoTessellatedPolygonShapeImp(const UtoTessellatedPolygonShapeImp& rhs)
   : UtoShapeImp(rhs)
   , mRoot(new osg::Geode)
   , mGeometry(new osg::Geometry)
   , mVertexArray(new osg::Vec3Array())
   , mOffset(rhs.mOffset)
   , mClosed(rhs.mClosed)
   , mDepthTest(rhs.mDepthTest)
{
   // Register the heading transform at 0 deg
   mOrientationTransform = new osg::MatrixTransform;
   mOrientationTransform->addChild(mRoot.get());
   AddShapeChildNode(mOrientationTransform);

   mRoot->addDrawable(mGeometry.get());

   mVertexArray = new osg::Vec3Array;
   mGeometry->setVertexArray(mVertexArray.get());

   // setup the color
   osg::Vec4Array* color = new osg::Vec4Array;
   color->push_back(osg::Vec4(1, 1, 1, 1));
   mGeometry->setColorArray(color);
   mGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

   mGeometry->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,
                                             mDepthTest ? osg::StateAttribute::ON : osg::StateAttribute::OFF);

   mRoot->setName("UtoTessellatedPolygonShape");
}

UtoTessellatedPolygonShapeImp::~UtoTessellatedPolygonShapeImp() {}

int UtoTessellatedPolygonShapeImp::Insert(int pos, const UtoPosition pts[], int num)
{
   for (int i = 0; i < num; ++i)
   {
      mPositions.insert(mPositions.begin() + pos + i, 1, pts[i]);
   }
   return 0;
}

void UtoTessellatedPolygonShapeImp::SetColor(const UtoColor& rColor, unsigned int nClrMask, unsigned int nReserved)
{
   UtoShapeImp::SetColor(rColor, nClrMask, nReserved);

   // there is only one entry in the color array list and it is the first entry
   osg::Vec4Array::iterator entry = dynamic_cast<osg::Vec4Array*>(mGeometry->getColorArray())->begin();
   entry->x()                     = m_oBlendColor.RedF();
   entry->y()                     = m_oBlendColor.GreenF();
   entry->z()                     = m_oBlendColor.BlueF();
   entry->w()                     = m_oBlendColor.AlphaF();

   if (rColor.HasTransparency())
   {
      UtoGeometryUtil::EnableBlend(mRoot.get());
      mRoot->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 0.0, 1.0, false));
      mRoot->getOrCreateStateSet()->setRenderBinDetails(11, "DepthSortedBin");
      mRoot->getOrCreateStateSet()->setNestRenderBins(false);
   }
   else
   {
      UtoGeometryUtil::DisableBlend(mRoot.get());
      mRoot->getOrCreateStateSet()->removeAttribute(osg::StateAttribute::DEPTH);
   }
   // Apply a material to set fill color
   osg::Material* material = new osg::Material;
   material->setDiffuse(osg::Material::FRONT, *entry);
   material->setSpecular(osg::Material::FRONT, osg::Vec4(0.0, 0.0, 0.0, 1.0));
   material->setAmbient(osg::Material::FRONT, *entry * 0.6f);
   material->setEmission(osg::Material::FRONT, osg::Vec4(0.0, 0.0, 0.0, 1.0));
   material->setShininess(osg::Material::FRONT, 25.0);
   mRoot->getOrCreateStateSet()->setAttribute(material);

   mGeometry->dirtyDisplayList();
}

void UtoTessellatedPolygonShapeImp::Clear()
{
   mPositions.clear();
   mVertexArray->clear();
   int np = mGeometry->getNumPrimitiveSets();
   mGeometry->removePrimitiveSet(0, np);
}

void UtoTessellatedPolygonShapeImp::SetOffset(const UtoPosition& aOffset)
{
   mOffset = aOffset;
}

UtoPosition UtoTessellatedPolygonShapeImp::GetOffset() const
{
   return UtoPosition(mOffset[0], mOffset[1], mOffset[2]);
}

void UtoTessellatedPolygonShapeImp::SetOrientationTransform(const UtoOrientation& aOrientation)
{
   mOrientationTransform->setMatrix(osg::Matrix::rotate(aOrientation.yaw,
                                                        osg::Vec3d(0, 0, 1),
                                                        aOrientation.pitch,
                                                        osg::Vec3d(1, 0, 0),
                                                        aOrientation.roll,
                                                        osg::Vec3d(0, 1, 0)));
}

void UtoTessellatedPolygonShapeImp::EnableLighting(bool aState)
{
   mGeometry->getOrCreateStateSet()->setMode(GL_LIGHTING, aState ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
}

void UtoTessellatedPolygonShapeImp::Rebuild()
{
   unsigned int ii = 0U;
   unsigned int pp = 0U;

   if (mClosed)
   {
      osgUtil::Tessellator tess;

      tess.beginTessellation();
      tess.beginContour();
      for (std::vector<osg::Vec3d>::const_iterator it = mPositions.begin(); it != mPositions.end(); ++it)
      {
         tess.addVertex(new osg::Vec3(*it));
      }
      tess.endContour();
      tess.endTessellation();

      const osgUtil::Tessellator::PrimList& prims = tess.getPrimList();

      // Build the bottom face.
      for (osgUtil::Tessellator::PrimList::const_iterator pit = prims.begin(); pit != prims.end(); ++pit)
      {
         mGeometry->addPrimitiveSet(new osg::DrawElementsUInt((*pit)->_mode));

         const osgUtil::Tessellator::Prim::VecList& vecs = (*pit)->_vertices;

         osg::DrawElementsUInt* primSet = static_cast<osg::DrawElementsUInt*>(mGeometry->getPrimitiveSet(pp));
         primSet->resize(vecs.size());
         osg::DrawElementsUInt::iterator itPrim = primSet->begin();

         bool aboveGround   = true;
         bool heightChecked = false;
         for (osgUtil::Tessellator::Prim::VecList::const_iterator vit = vecs.begin(); vit != vecs.end(); ++vit)
         {
            if (!heightChecked)
            {
               auto      shapeU_Pos = m_Owner->GetLocationWCS();
               osg::Vec3 shapePos   = osg::Vec3(shapeU_Pos[0], shapeU_Pos[1], shapeU_Pos[2]);
               aboveGround          = shapePos.length() > UtSphericalEarth::cEARTH_MEAN_RADIUS;
               heightChecked        = true;
            }

            osg::Vec3 tempVec;
            if (GetReferenceFrame() == UtoShape::cFRAME_WORLD_AT_ENTITY && !aboveGround)
            {
               auto      shapeU_Pos = m_Owner->GetLocationWCS();
               osg::Vec3 shapePos   = osg::Vec3(shapeU_Pos[0], shapeU_Pos[1], shapeU_Pos[2]);
               tempVec              = **vit * shapePos.length() / UtSphericalEarth::cEARTH_MEAN_RADIUS;
               mVertexArray->push_back(tempVec);
            }
            else
            {
               mVertexArray->push_back(tempVec);
            }
            *itPrim = ii;
            ++itPrim;
            ++ii;
         }
         ++pp;
      }

      // Build the top face.
      for (osgUtil::Tessellator::PrimList::const_iterator pit = prims.begin(); pit != prims.end(); ++pit)
      {
         mGeometry->addPrimitiveSet(new osg::DrawElementsUInt((*pit)->_mode));

         const osgUtil::Tessellator::Prim::VecList& vecs = (*pit)->_vertices;

         osg::DrawElementsUInt* primSet = static_cast<osg::DrawElementsUInt*>(mGeometry->getPrimitiveSet(pp));
         primSet->resize(vecs.size());
         osg::DrawElementsUInt::iterator itPrim = primSet->begin();

         bool aboveGround   = true;
         bool heightChecked = false;
         for (osgUtil::Tessellator::Prim::VecList::const_iterator vit = vecs.begin(); vit != vecs.end(); ++vit)
         {
            osg::Vec3 tempVec;

            if (GetReferenceFrame() == UtoShape::cFRAME_WORLD_AT_ENTITY)
            {
               auto      shapeU_Pos = m_Owner->GetLocationWCS();
               osg::Vec3 shapePos   = osg::Vec3(shapeU_Pos[0], shapeU_Pos[1], shapeU_Pos[2]);
               double    vlen       = mOffset.length();

               if (!heightChecked)
               {
                  aboveGround   = shapePos.length() > UtSphericalEarth::cEARTH_MEAN_RADIUS;
                  heightChecked = true;
               }

               if (!aboveGround)
               {
                  osg::Vec3d adjVit = **vit * shapePos.length() / UtSphericalEarth::cEARTH_MEAN_RADIUS;
                  osg::Vec3  p      = shapePos + adjVit;
                  p.normalize();
                  tempVec = adjVit + (p * vlen);
               }
               else
               {
                  osg::Vec3 p = shapePos + **vit;
                  p.normalize();
                  tempVec = **vit + (p * vlen);
               }
            }
            else
            {
               tempVec = (**vit) + mOffset;
            }
            mVertexArray->push_back(tempVec);
            *itPrim = ii;
            ++itPrim;
            ++ii;
         }
         ++pp;
      }
   }

   // Build the side walls.
   mGeometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLE_STRIP));
   osg::DrawElementsUInt* primSet = static_cast<osg::DrawElementsUInt*>(mGeometry->getPrimitiveSet(pp));
   primSet->resize(mPositions.size() * 2);
   osg::DrawElementsUInt::iterator itPrim = primSet->begin();

   bool aboveGround   = true;
   bool heightChecked = false;
   for (std::vector<osg::Vec3d>::const_iterator vit = mPositions.begin(); vit != mPositions.end(); ++vit)
   {
      *itPrim = ii;
      ++itPrim;
      ++ii;

      if (GetReferenceFrame() == UtoShape::cFRAME_WORLD_AT_ENTITY)
      {
         auto      shapeU_Pos = m_Owner->GetLocationWCS();
         osg::Vec3 shapePos   = osg::Vec3(shapeU_Pos[0], shapeU_Pos[1], shapeU_Pos[2]);
         double    vlen       = mOffset.length();

         if (!heightChecked)
         {
            aboveGround   = shapePos.length() > UtSphericalEarth::cEARTH_MEAN_RADIUS;
            heightChecked = true;
         }

         osg::Vec3d tempVec;
         tempVec = aboveGround ? *vit : *vit * shapePos.length() / UtSphericalEarth::cEARTH_MEAN_RADIUS;
         mVertexArray->push_back(tempVec);

         osg::Vec3 p = shapePos + tempVec;
         p.normalize();

         mVertexArray->push_back(tempVec + (p * vlen));
      }
      else
      {
         mVertexArray->push_back(*vit);
         mVertexArray->push_back((*vit) + mOffset);
      }

      *itPrim = ii;
      ++itPrim;
      ++ii;

      ++pp;
   }
}

void UtoTessellatedPolygonShapeImp::SetDepthTesting(bool bDepthTest)
{
   if (bDepthTest != mDepthTest)
   {
      mDepthTest = bDepthTest;
      mGeometry->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,
                                                mDepthTest ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
   }
}
