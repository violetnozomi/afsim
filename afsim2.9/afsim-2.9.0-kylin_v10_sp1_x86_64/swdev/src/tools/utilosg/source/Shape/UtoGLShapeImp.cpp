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
// UtoGLShapeImp.cpp : implementation file

#include "UtoGLShapeImp.hpp"

#include <algorithm>

#include <osg/CopyOp>
#include <osg/Geode>

#include "UtoGeometryUtil.hpp"
#include "UtoOptions.hpp"
#include "UtoPriv.hpp"

class GLDrawable : public osg::Drawable
{
public:
   GLDrawable(UtoGLShapeImp::DrawFn* fn = nullptr, const osg::Vec3 bound = osg::Vec3(1, 1, 1));
   GLDrawable(const GLDrawable& gld, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
   GLDrawable& operator=(const GLDrawable& aRHS) = delete;

   // define some functions for clone and string names
   META_Object(Boeing_uto, GLDrawable)

      void drawImplementation(osg::RenderInfo& aRenderInfo) const override;
   void SetFn(UtoGLShapeImp::DrawFn* fn) { m_DrawFn = fn; }
   void SetSize(double x, double y, double z);

   osg::Vec3  GetBound() const { return m_Bound; }
   osg::Vec4& Color() { return m_Color; }

   // render info is only valid during the drawImplementation
   osg::RenderInfo* GetRenderInfo() const { return mRenderInfoPtr; }

protected:
   osg::BoundingSphere computeBound() const override;

private:
   // Allocation and deallocation is completed in UtoGLShapeImp
   UtoGLShapeImp::DrawFn*   m_DrawFn;
   osg::Vec3                m_Bound;
   osg::Vec4                m_Color;
   mutable osg::RenderInfo* mRenderInfoPtr;
};

GLDrawable::GLDrawable(UtoGLShapeImp::DrawFn* fn, const osg::Vec3 bound)
   : m_DrawFn(fn)
   , m_Bound(bound)
   , m_Color(1, 1, 1, 1)
   , mRenderInfoPtr(nullptr)
{
   setUseDisplayList(false);
}

GLDrawable::GLDrawable(const GLDrawable& gld, const osg::CopyOp& copyop)
   : osg::Drawable(gld, copyop)
   , m_DrawFn(gld.m_DrawFn)
   , m_Bound(gld.m_Bound)
   , m_Color(gld.m_Color)
   , mRenderInfoPtr(nullptr)
{
}

void GLDrawable::drawImplementation(osg::RenderInfo& aRenderInfo) const
{
   mRenderInfoPtr = &aRenderInfo;
   if (UtoOptions::Instance()->GLCallbacksDisabled())
      return;

   glColor4fv(m_Color.ptr());

   if (m_DrawFn)
      (*m_DrawFn)();

   mRenderInfoPtr = nullptr;
}

void GLDrawable::SetSize(double x, double y, double z)
{
   m_Bound = osg::Vec3(x, y, z);
   dirtyBound();
}

osg::BoundingSphere GLDrawable::computeBound() const
{
   _boundingSphere.set(osg::Vec3f(0, 0, 0), std::max(m_Bound.x(), std::max(m_Bound.y(), m_Bound.z())));

   _boundingSphereComputed = true;
   return _boundingSphere;
}

UtoGLShapeImp::UtoGLShapeImp()
   : m_DrawFn(nullptr)
   , m_Drawable(new GLDrawable(nullptr))
{
   m_Geode->setName("GLShape");
   m_Geode->addDrawable(m_Drawable.get());
}

UtoGLShapeImp::UtoGLShapeImp(const UtoGLShapeImp& rhs, const osg::CopyOp& copyop)
   : UtoGeodeShapeImp(rhs)
   , m_DrawFn(rhs.m_DrawFn->clone())
   , m_Drawable(new GLDrawable(m_DrawFn, rhs.m_Drawable->GetBound()))
{
   m_Geode->setName("GLShape");
   m_Geode->addDrawable(m_Drawable.get());

   SetColor(rhs.m_oBlendColor, UtoColor::RGBA);
}

UtoGLShapeImp::~UtoGLShapeImp()
{
   delete m_DrawFn;
}

void UtoGLShapeImp::SetFn(DrawFn* fn)
{
   if (m_DrawFn)
      delete m_DrawFn;
   m_DrawFn = fn;
   m_Drawable->SetFn(m_DrawFn);
}

void UtoGLShapeImp::SetColor(const UtoColor& Color, unsigned int nClrMask, unsigned int nReserved)
{
   // call the base class
   UtoShapeImp::SetColor(Color, nClrMask, nReserved);

   // there is only one entry in the color array list and it is the first entry
   osg::Vec4& color = m_Drawable->Color();
   color.x()        = m_oBlendColor.RedF();
   color.y()        = m_oBlendColor.GreenF();
   color.z()        = m_oBlendColor.BlueF();
   color.w()        = m_oBlendColor.AlphaF();

   if (m_oBlendColor.HasTransparency())
   {
      UtoGeometryUtil::EnableBlend(m_RootNode.get());
      m_RootNode->getOrCreateStateSet()->setRenderBinDetails(11, "DepthSortedBin");
   }
   else
   {
      UtoGeometryUtil::DisableBlend(m_RootNode.get());
      m_RootNode->getOrCreateStateSet()->setRenderBinDetails(11, "DepthSortedBin");
      //      m_RootNode->getOrCreateStateSet()->setRenderBinToInherit();
   }

   m_Drawable->dirtyDisplayList();
}

void UtoGLShapeImp::SetSize(double radius)
{
   m_Drawable->SetSize(radius, radius, radius);
}

void UtoGLShapeImp::SetSize(double cx, double cy)
{
   double x, y, z;
   // cx and cy are the length on each side, we want half so we can
   // center the box at our current location.
   x = cx / 2;
   y = cy / 2;
   // until the api changes and the user can give us a real Z
   // base Z off the maximum of the X and Y value
   z = cx > cy ? x : y;
   m_Drawable->SetSize(x, y, z);
}

osg::RenderInfo* UtoGLShapeImp::GetRenderInfo() const
{
   if (m_Drawable.valid())
   {
      return m_Drawable->GetRenderInfo();
   }
   else
   {
      return nullptr;
   }
}
