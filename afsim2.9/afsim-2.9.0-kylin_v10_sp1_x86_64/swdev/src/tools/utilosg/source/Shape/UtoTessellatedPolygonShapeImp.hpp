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
// UtoTessellatedPolygonShapeImp.hpp : header file

#if !defined(_UTOTESSELLATEDPOLYGONSHAPEIMP_HPP_)
#define _UTOTESSELLATEDPOLYGONSHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "UtAngle.hpp"
#include "UtoGeometryShapeImp.hpp"
#include "UtoTypes.hpp"

class UtoTessellatedPolygonShapeImp;

class UtoTessellatedPolygonShapeImp : public UtoShapeImp
{
public:
   UtoTessellatedPolygonShapeImp();
   UtoTessellatedPolygonShapeImp(const UtoPosition pts[], int num);
   UtoTessellatedPolygonShapeImp(const UtoTessellatedPolygonShapeImp&);
   ~UtoTessellatedPolygonShapeImp() override;

   virtual int  Insert(int pos, const UtoPosition pts[], int num);
   virtual void Clear();

   void        SetOffset(const UtoPosition& aOffset);
   UtoPosition GetOffset() const;

   void SetOrientationTransform(const UtoOrientation& aOrientation);

   void SetClosed(bool aClosed) { mClosed = aClosed; }
   bool GetClosed() const { return mClosed; }

   void SetColor(const UtoColor& Color, unsigned int nClrMask, unsigned int nReserved = 0) override;

   // determines if the shape is allowed to be depth tested
   void SetDepthTesting(bool bDepthTest);

   void         EnableLighting(bool aState);
   virtual void Rebuild();

private:
   UtoTessellatedPolygonShapeImp& operator=(const UtoTessellatedPolygonShapeImp&);

   osg::ref_ptr<osg::Geode>     mRoot;
   osg::ref_ptr<osg::Geometry>  mGeometry;
   osg::ref_ptr<osg::Vec3Array> mVertexArray;

   std::vector<osg::Vec3d> mPositions;
   osg::Vec3d              mOffset;
   osg::MatrixTransform*   mOrientationTransform;
   bool                    mClosed;
   bool                    mDepthTest;
};

#endif // !defined(_UTOTESSELLATEDPOLYGONSHAPEIMP_HPP_)
