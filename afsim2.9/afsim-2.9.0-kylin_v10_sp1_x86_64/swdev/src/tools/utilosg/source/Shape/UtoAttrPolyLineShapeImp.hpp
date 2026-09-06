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
// UtoAttrPolyLineShapeImp.h : header file

#if !defined(_UtoAttrPolyLINESHAPEIMP_H_)
#define _UtoAttrPolyLINESHAPEIMP_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "UtoPolyLineShapeImp.hpp"
#include "UtoTypes.hpp"

class UtoAttrPolyLineShape;

class UtoAttrPolyLineShapeImp : public UtoPolyLineShapeImp
{
public:
   UtoAttrPolyLineShapeImp();
   UtoAttrPolyLineShapeImp(const UtoPosition pts[], const UtoColor cls[], int num);
   UtoAttrPolyLineShapeImp(const UtoAttrPolyLineShapeImp&);
   ~UtoAttrPolyLineShapeImp() override;

   int      Insert(int pos, const UtoPosition pts[], const UtoColor col[], int num);
   int      SetAt(int pos, const UtoPosition pts, const UtoColor col);
   UtoColor ColorAt(int pos);
   int      Remove(int pos);
   void     Clear();

   bool SetShaders(const char* aVShader, const char* aFShader, std::set<std::string>& aDefList);

private:
   void UnloadShaders();

   bool LoadProgram(osg::ref_ptr<osg::Shader>& aPixelPtr, osg::ref_ptr<osg::Shader>& aVertexPtr);

   osg::ref_ptr<osg::Vec4Array> m_Color;

   UtoAttrPolyLineShapeImp& operator=(const UtoAttrPolyLineShapeImp&);

   class PolyLineBoundingSphereCallback : public osg::Node::ComputeBoundingSphereCallback
   {
   public:
      PolyLineBoundingSphereCallback();
      virtual osg::BoundingSphere computeBound(const osg::Node&);

   private:
   };
   PolyLineBoundingSphereCallback* mBoundingSphereCallbackPtr;
};

#endif // !defined(_CMEPOLYLINESHAPEIMP_H_)
