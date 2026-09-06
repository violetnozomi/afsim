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

#include "AngleEntry.hpp"

#include <osg/BlendFunc>
#include <osg/MatrixTransform>

#include "UtEllipsoidalEarth.hpp"


namespace vespa
{
AngleEntry::AngleEntry(const UtVec3f& aFirst, const UtVec3f& aSecond, const QColor& aColor, const std::string& aName, float aRadius)
   : mRadius{aRadius}
   , mVectorOne{aFirst}
   , mVectorTwo{aSecond}
   , mName{aName}
   , mColorOfEntry{aColor}
{
   CalculateAngle();
   mSector.SetReferenceFrame(UtoShape::cFRAME_ENTITY);
}

double AngleEntry::CalculateAngle()
{
   // we could use angleWith if we prefer, but we need to convert to degrees manually.
   mAngleInRadians = mVectorOne.AngleWith(mVectorTwo);
   return mAngleInRadians;
}

void AngleEntry::Update()
{
   UpdateGeometry();
}

UtoRawShape& AngleEntry::GeSectorForTree()
{
   return mSector;
}

void AngleEntry::SetSectorFromParent(UtoShape* aSectorInParent)
{
   mSectorInGraph = dynamic_cast<UtoRawShape*>(aSectorInParent);
   LoadShape();
}

void AngleEntry::UpdateStartVector(const UtVec3f& aVector)
{
   mVectorOne            = aVector;
   mGeometryUpdateNeeded = true;
}

void AngleEntry::UpdateEndVector(const UtVec3f& aVector)
{
   mVectorTwo            = aVector;
   mGeometryUpdateNeeded = true;
}

void AngleEntry::SetRadius(float aRadius)
{
   mRadius = aRadius;
}

bool AngleEntry::IsAngleVisible() const
{
   return mIsVisible;
}

void AngleEntry::SetVisibility(bool aIsVisible)
{
   mIsVisible = aIsVisible;
   if (!aIsVisible)
   {
      mTransform->setNodeMask(0);
   }
   else
   {
      mTransform->setNodeMask(1);
   }
}

void AngleEntry::ShouldFill(bool aShouldFill)
{
   mIsModeFill = aShouldFill;
   if (aShouldFill)
   {
      mGeom->getPrimitiveSet(0)->setMode(osg::PrimitiveSet::TRIANGLE_FAN);
      mGeom->getPrimitiveSet(0)->dirty();
   }
   else
   {
      mGeom->getPrimitiveSet(0)->setMode(osg::PrimitiveSet::LINE_LOOP);
      mGeom->getPrimitiveSet(0)->dirty();
   }
}

bool AngleEntry::IsModeFill() const
{
   return mIsModeFill;
}

const QColor& AngleEntry::GetColor() const
{
   return mColorOfEntry;
}

double AngleEntry::GetAngle() const
{
   return mAngleInRadians;
}

const std::string& AngleEntry::GetName() const
{
   return mName;
}

void AngleEntry::LoadShape()
{
   LoadGeometry();

   // If this does not makes sense please ask me and we can talk about how osg handles rendering.
   mGeom->setVertexArray(mVertices.get());
   mGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN, 0, mVertices->size()));
   mGeom->setUseDisplayList(false);        // Don't use display lists data structure
   mGeom->setUseVertexBufferObjects(true); // VBOs are the modern openGL way and are more performant.
   mGeode->addDrawable(mGeom.get());
   mTransform->addChild(mGeode.get());
   mSectorInGraph->GetRoot()->addChild(mTransform.get());
   float red   = mColorOfEntry.redF();
   float green = mColorOfEntry.greenF();
   float blue  = mColorOfEntry.blueF();
   float alpha = mColorOfEntry.alphaF();
   mSectorInGraph->GetRoot()->getOrCreateStateSet()->getOrCreateUniform("uRadius", osg::Uniform::FLOAT)->set(mRadius);
   mSectorInGraph->GetRoot()
      ->getOrCreateStateSet()
      ->getOrCreateUniform("uColor", osg::Uniform::FLOAT_VEC4)
      ->set(osg::Vec4(red, green, blue, alpha));
   mSectorInGraph->GetRoot()
      ->getOrCreateStateSet()
      ->getOrCreateUniform("uVectorStart", osg::Uniform::FLOAT_VEC3)
      ->set(osg::Vec3(mVectorOne.Get(0), mVectorOne.Get(1), mVectorOne.Get(2)));
   mSectorInGraph->GetRoot()
      ->getOrCreateStateSet()
      ->getOrCreateUniform("uVectorEnd", osg::Uniform::FLOAT_VEC3)
      ->set(osg::Vec3(mVectorTwo.Get(0), mVectorTwo.Get(1), mVectorTwo.Get(2)));
   mSectorInGraph->GetRoot()->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc()); // allow alpha channel
   mSectorInGraph->GetRoot()->getOrCreateStateSet()->setRenderBinDetails(11, "DepthSortedBin");

   if (!mShaderProgram)
   {
      mShaderProgram = UtoShaders::UseProgram("angleAttachment.vert", "angleAttachment.frag", {});
   }
   mSectorInGraph->SetProgram(mShaderProgram.get(), true);
}

void AngleEntry::UpdateGeometry()
{
   if (mGeometryUpdateNeeded && IsAngleVisible())
   {
      mSectorInGraph->GetRoot()
         ->getOrCreateStateSet()
         ->getOrCreateUniform("uVectorStart", osg::Uniform::FLOAT_VEC3)
         ->set(osg::Vec3(mVectorOne.Get(0), mVectorOne.Get(1), mVectorOne.Get(2)));
      mSectorInGraph->GetRoot()
         ->getOrCreateStateSet()
         ->getOrCreateUniform("uVectorEnd", osg::Uniform::FLOAT_VEC3)
         ->set(osg::Vec3(mVectorTwo.Get(0), mVectorTwo.Get(1), mVectorTwo.Get(2)));
      CalculateAngle();
      mGeometryUpdateNeeded = false;
   }
}

void AngleEntry::LoadGeometry()
{
   mVertices->clear();
   mVertices->push_back(osg::Vec3(0.0F, 0.0F, 0.0F));
   for (double v = 0; v <= 1.0; v += 0.020)
   {
      osg::Vec3 offset(v, 1.0F - v, 0.0F);
      offset.normalize();
      mVertices->push_back(offset * 20);
   }
   CalculateAngle();
   mGeometryUpdateNeeded = false;
}

osg::ref_ptr<osg::Program> AngleEntry::mShaderProgram = nullptr;
} // namespace vespa
