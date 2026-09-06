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
// UtoGLShapeImp.hpp : header file

#if !defined(_UTOGLSHAPEIMP_HPP_)
#define _UTOGLSHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <osg/Drawable>
#include <osg/Object>

#include "UtoGeodeShapeImp.hpp"

class GLDrawable;

class UtoGLShapeImp : public UtoGeodeShapeImp
{
public:
   typedef Utok::DrawFunctor DrawFn;

   UtoGLShapeImp();
   UtoGLShapeImp(const UtoGLShapeImp&, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
   ~UtoGLShapeImp() override;

   DrawFn* Get() const { return m_DrawFn; }
   void    SetFn(DrawFn* fn);

   void SetSize(double radius);
   void SetSize(double cx, double cy);

   void SetColor(const UtoColor& Color, unsigned int nClrMask, unsigned int nReserved = 0) override;
   // this will only be valid during the draw function
   osg::RenderInfo* GetRenderInfo() const;

private:
   UtoGLShapeImp& operator=(const UtoGLShapeImp&);

   DrawFn*                  m_DrawFn;
   osg::ref_ptr<GLDrawable> m_Drawable;
};

#endif // !defined(_UTOGLSHAPEIMP_HPP_)
