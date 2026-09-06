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
// UtoGLShape.hpp : header file

#if !defined(_UTOGLSHAPE_HPP_)
#define _UTOGLSHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include <osg/GL>
#include <osg/Group>

#include "UtoShape.hpp"
#include "UtoTypes.hpp"

namespace osg
{
class RenderInfo;
}

class UTILOSG_EXPORT UtoGLShapeBase : public UtoShape
{
public:
   typedef Utok::DrawFunctor DrawFunctor;

   ~UtoGLShapeBase() override;

   void SetSize(double radius);
   void SetSize(double cx, double cy);

   // this will only be valid within the Draw Function
   osg::RenderInfo* GetRenderInfo() const;

protected:
   UtoGLShapeBase();
   UtoGLShapeBase(const UtoGLShapeBase& rhs);
   void Set_i(DrawFunctor*);
   void Dump(std::ostream&) override;
};

class UtoGLShape : public UtoGLShapeBase
{
private:
   typedef Utok::DrawFunctor DrawFunctor;

public:
   template<class T, class Func>
   UtoGLShape(T* p, Func f)
   {
      Set_i(new Utok::DrawFunctorHolder<T, Func>(p, f));
   }

   UtoGLShape(const UtoGLShape& rhs)
      : UtoGLShapeBase(rhs)
   {
   }
   ~UtoGLShape() override {}
   UtoGLShape& operator=(const UtoGLShape& rhs)
   {
      UtoGLShape T(rhs);
      Swap(T);
      return *this;
   }

   template<class T, class Func>
   void SetFn(T* p, Func f)
   {
      Set_i(new Utok::DrawFunctorHolder<T, Func>(p, f));
   }

   UtoShape* Clone() const override { return new UtoGLShape(*this); }

   void EnableLineSmoothing(bool aState)
   {
      GetTree()->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH,
                                                aState ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
   }
   void EnableLighting(bool aState)
   {
      GetTree()->getOrCreateStateSet()->setMode(GL_LIGHTING, aState ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
   }

protected:
private:
   UtoGLShape();
};

#endif // !defined(_UTOGLSHAPE_HPP_)
