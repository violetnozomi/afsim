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
// UtoVectorShapeImp.cpp : implementation file

#include "UtoVectorShapeImp.hpp"

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>

#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtoMemory.hpp"
#include "UtoShaders.hpp"
#include "UtoShape.hpp"

UtoVectorShapeImp::UtoVectorShapeImp()
   : UtoShapeImp()
{
   SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
   std::set<std::string> defset;
   m_RootNode->getOrCreateStateSet()->setAttribute(UtoShaders::UseProgram("vector.vert", "vector.frag", defset),
                                                   osg::StateAttribute::ON);
   m_RootNode->setCullingActive(false);
}


UtoVectorShapeImp::UtoVectorShapeImp(const UtoVectorShapeImp& aRHS)
   : UtoShapeImp(aRHS)
   , mScalar(aRHS.mScalar)
{
   SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
   std::set<std::string> defset;
   m_RootNode->getOrCreateStateSet()->setAttribute(UtoShaders::UseProgram("vector.vert", "vector.frag", defset),
                                                   osg::StateAttribute::ON);
   m_RootNode->setCullingActive(false);
}


UtoVectorShapeImp::~UtoVectorShapeImp() {}

void UtoVectorShapeImp::SetScalar(float aValue)
{
   mScalar = aValue;
   m_RootNode->getOrCreateStateSet()->getOrCreateUniform("modelScalar", osg::Uniform::FLOAT)->set(mScalar);
}

float UtoVectorShapeImp::GetScalar() const
{
   return mScalar;
}

int UtoVectorShapeImp::AddVector(float aAz, float aEl, const osg::Vec4& aColor, bool aHead)
{
   m_RootNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
   int retval               = mNextVectorId++;
   mTransformVector[retval] = new osg::PositionAttitudeTransform();
   mTransformVector[retval]->setAttitude(
      osg::Quat(-aEl, osg::Vec3d(0, 1, 0), -aAz, osg::Vec3d(1, 0, 0), UtMath::cPI_OVER_2, osg::Vec3d(0, 1, 0)));
   m_RootNode->addChild(mTransformVector[retval].get());
   osg::ref_ptr<osg::Geode> geode = new osg::Geode;
   mTransformVector[retval]->addChild(geode.get());

   if (aHead)
   {
      m_RootNode->getOrCreateStateSet()->getOrCreateUniform("uniformColor", osg::Uniform::BOOL)->set(false);
      osg::Material* material = new osg::Material;
      material->setDiffuse(osg::Material::FRONT, aColor);
      material->setSpecular(osg::Material::FRONT, osg::Vec4(0.0, 0.0, 0.0, 1.0));
      material->setAmbient(osg::Material::FRONT, aColor * 0.6f);
      material->setEmission(osg::Material::FRONT, osg::Vec4(0.0, 0.0, 0.0, 1.0));
      material->setShininess(osg::Material::FRONT, 25.0);

      // 1. shaft
      osg::ref_ptr<osg::ShapeDrawable> shaft =
         new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0F, 0.0F, 1.0F), 0.05F, 2.0F));
      shaft->getOrCreateStateSet()->setAttribute(material);
      geode->addDrawable(shaft.get());

      // 2. arrow head
      osg::ref_ptr<osg::ShapeDrawable> arrowhead =
         new osg::ShapeDrawable(new osg::Cone(osg::Vec3(0.0F, 0.0F, 1.96F), 0.08F, 0.12F));
      arrowhead->getOrCreateStateSet()->setAttribute(material);
      geode->addDrawable(arrowhead.get());
   }
   else
   {
      // simple, multisampled line segment
      geode->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
      geode->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::BlendFunc>(), osg::StateAttribute::ON);
      geode->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::LineWidth>(2), osg::StateAttribute::ON);
      auto lineGeom = ut::osg::make_ref<osg::Geometry>();
      auto vertices = ut::osg::make_ref<osg::Vec3Array>(2);
      (*vertices)[0].set(0, 0, 0);
      (*vertices)[1].set(0, 0, 2.0);
      lineGeom->setVertexArray(vertices);
      lineGeom->addPrimitiveSet(ut::osg::make_ref<osg::DrawArrays>(osg::PrimitiveSet::LINES, 0, 2));
      geode->addDrawable(lineGeom);

      m_RootNode->getOrCreateStateSet()->getOrCreateUniform("uniformColor", osg::Uniform::BOOL)->set(true);
      m_RootNode->getOrCreateStateSet()->getOrCreateUniform("uColor", osg::Uniform::FLOAT_VEC4)->set(aColor);
   }
   return retval;
}

void UtoVectorShapeImp::RemoveVector(int aVectorId)
{
   if (mTransformVector.find(aVectorId) != mTransformVector.end())
   {
      m_RootNode->removeChild(mTransformVector[aVectorId]);
      mTransformVector[aVectorId] = nullptr;
      mTransformVector.erase(aVectorId);
   }
}

void UtoVectorShapeImp::UpdateVector(int aVectorId, float aAz, float aEl)
{
   if (mTransformVector.find(aVectorId) != mTransformVector.end())
   {
      mTransformVector[aVectorId]->setAttitude(
         osg::Quat(-aEl, osg::Vec3d(0, 1, 0), -aAz, osg::Vec3d(1, 0, 0), UtMath::cPI_OVER_2, osg::Vec3d(0, 1, 0)));
   }
}

// This will only be effective when the "uniformColor" stateset is set to true (no material attribute used)
void UtoVectorShapeImp::UpdateVectorColor(int aVectorId, const osg::Vec4& aColor)
{
   m_RootNode->getOrCreateStateSet()->getOrCreateUniform("uColor", osg::Uniform::FLOAT_VEC4)->set(aColor);
}

void UtoVectorShapeImp::ShowVector(int aVectorId, bool aState)
{
   if (mTransformVector.find(aVectorId) != mTransformVector.end())
   {
      mTransformVector[aVectorId]->setNodeMask(aState ? ~0 : 0);
   }
}
