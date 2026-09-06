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
// UtoGeometryUtil.h : header file

#if !defined(_UTOGEOMETRYUTIL_H_)
#define _UTOGEOMETRYUTIL_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <string>

#include <osg/LineSegment>
#include <osg/Matrix>
#include <osg/Node>
#include <osg/StateSet>
#include <osg/Vec3>

#include "UtoTypes.hpp"

namespace osg
{
class Material;
class TexEnv;
} // namespace osg

class UtoEntity;
class UtoShapeImp;
class UtoViewer;

struct UtoGeometryUtil
{
   class PolyIterator
   {
   public:
      virtual ~PolyIterator() {}
      virtual bool          First()              = 0;
      virtual bool          Next()               = 0;
      virtual int           Prim() const         = 0;
      virtual int           Poly() const         = 0;
      virtual int           PolysPerPrim() const = 0;
      virtual int           VertsPerPoly() const = 0;
      virtual int           NumPolys()           = 0;
      virtual osg::Vec3*    Vert(const int i)    = 0;
      virtual osg::Vec3*    Norm(const int i)    = 0;
      virtual float*        Tex(const int i)     = 0;
      virtual unsigned int* Color(const int i)   = 0;
      virtual osg::Vec3*    PrimNorm()           = 0;
      virtual int           PrimType() const     = 0;
      virtual int           NodeFlags() const    = 0;
      virtual int           Texture() const      = 0;

   protected:
      PolyIterator(const int type) {}
      PolyIterator(const PolyIterator& rhs) {}

   private:
      PolyIterator();
      PolyIterator& operator=(const PolyIterator& rhs);
   };

   static std::string DumpNode(osg::Node* node);

   static bool Intersect(const osg::LineSegment& ray, PolyIterator& PolyIter, const osg::Matrix& mat, osg::Vec3& hit);

   static bool IntersectTri(const osg::LineSegment& ray,
                            const osg::Vec3&        vert0,
                            const osg::Vec3&        vert1,
                            const osg::Vec3&        vert2,
                            osg::Vec3&              hit);

   // This routine looks at the path to find the entity imp and
   // returns a pointer to it if it was found.  The path is expected
   // to start at either the root node of the scene indicated by
   // passing SceneRoot as true or to start at the database node where
   // entities are added to the tree.
   static UtoEntity*   GetEntity(osg::NodePath& path, bool SceneRoot);
   static UtoShapeImp* GetShape(osg::NodePath& path, bool SceneRoot);

   static void EnableBlend(osg::Node* node);
   static void DisableBlend(osg::Node* node);
   static void SetMaterialColor(osg::Material* material, const UtoColor& color, double transparency);

   // enables the pixel shader operations for color blending and transparency
   // if this function returns false, then the pixel shader is either not
   // supported or the function encountered errors.
   static bool AttachColorTransparencyPixelShader(osg::Node*         pNode,
                                                  const UtoColor&    rColor,
                                                  const std::string& rVertexShader,
                                                  const std::string& rPixelShader);
   // removes a pixel shader from the node
   static void RemoveColorTransparencyPixelShader(osg::Node* pNode);
   // set the color and transparency of a pixel shader
   static void SetColorTransparencyPixelShader(osg::Node* pNode, const UtoColor& rColor);

   UTILOSG_EXPORT
   static void ComputeOffsetLine(UtoViewer* aViewerPtr, osg::Vec3& aSourceWCS, osg::Vec3& aTargetWCS, float aPixelOffset);

   // allocates a new StateSet and Material and use a common TexEnv, this
   // state set is used mostly for coloring models
   static osg::StateSet*            SetupColorMaterialState();
   static osg::ref_ptr<osg::TexEnv> m_TexEnv;
};

#endif // !defined(_UTOGEOMETRYUTIL_H_)
