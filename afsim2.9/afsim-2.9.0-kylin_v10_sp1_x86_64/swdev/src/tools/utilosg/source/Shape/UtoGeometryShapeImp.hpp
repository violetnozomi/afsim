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
// UtoGeodeShapeImp.hpp : header file

#if !defined(_UTOGEOMETRY_SHAPEIMP_HPP_)
#define _UTOGEOMETRY_SHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <osg/Array>

#include "UtoDrawableCallback.hpp"
#include "UtoGeodeShapeImp.hpp"
#include "UtoTypes.hpp"

namespace osg
{
class Geometry;
class DrawArrays;
} // namespace osg

class UtoGeometryShapeImp : public UtoGeodeShapeImp
{
public:
   UtoGeometryShapeImp();
   UtoGeometryShapeImp(const UtoGeometryShapeImp&);
   ~UtoGeometryShapeImp() override;
   void SetColor(const UtoColor& Color, unsigned int nClrMask, unsigned int nReserved = 0) override;
   void SetOwner(UtoEntity* owner) override;

   // allows for setting the use of display lists
   virtual void SetUseDisplayLists(bool bUse);
   // determines the state of display lists
   virtual bool UsingDisplayLists() const;

   void        EnableComplexPoly(bool enable);
   bool        ComplexPoly() const { return m_ComplexPoly; }
   void        SetSides(const UtoPosition& offset);
   UtoPosition Sides() const;
   void        EnableSmoothSides(bool enable);
   bool        SmoothSides() const { return m_SmoothSides; }
   void        EnableLighting(bool aState);
   void        SetUsesShader(bool aUsesShader);

   /* This is the update callback on the Geometry drawable.  It disables
    * the callback before returning.  If Update is overriden, be sure and
    * call this base class or the Update callback will not disabled and any
    * geometry updates at this level will not take place.
    */
   virtual void Update(osg::NodeVisitor* nv, osg::Drawable* drawable);

protected:
   osg::ref_ptr<osg::Geometry>  m_Geometry;
   osg::ref_ptr<osg::Vec3Array> m_Vertex;
   osg::ref_ptr<osg::Vec3Array> m_Normal;
   // If sides are enabled m_PrimitiveSet will be disconnected from the
   // geometry and replaced by possibly multiple primitives.  In that case
   // m_PrimitiveSet is used to hold the original number of verticies.
   osg::ref_ptr<osg::DrawArrays> m_PrimitiveSet;
   // Enable sides and specify the offset, 0,0,0 disable
   osg::Vec3 m_SideOffset;
   // Indicates if sides are enabled.
   bool HasSides() const { return (m_SideOffset.x() || m_SideOffset.y() || m_SideOffset.z()); }
   // true for smooth, false for flat shading
   bool m_SmoothSides;
   // Enable tesselation for complex polygons.
   bool m_ComplexPoly;
   // Skip displaying first slice side, if circle is closed
   bool m_FullCircle;
   // Enable drawing sides tangentially, converging at center of Earth
   bool mDrawTangential;

   // Call to enable the update callback.
   void NeedCallback();

   // Generate a quad strip to connect the first orig_count verticies to
   // a set at the offset cur_count optionally connecting the last vertex
   // to the first vertex.
   virtual void AddSidesPreOffset(bool close);
   virtual void AddSidesPostOffset(bool close);

private:
   // Add sides for simple shapes.
   void AddSidesSimple(bool close);
   // Add sides for self intersecting geometry.
   void AddSidesComplex(bool close);

   // Duplicate each vertex by the offset and duplicate non GL_QUAD_STRIP
   // primitives to use them.
   void DuplicateFace(const osg::Vec3& offset);

   // Add a normal for the vertex at the given index.
   void AddNormal(osg::DrawElementsUShort& draw, int index, const osg::Vec3& n);
   // Assign normals for all relevant vertices.
   void CalculateNormals();

   UtoGeometryShapeImp&                                   operator=(const UtoGeometryShapeImp&);
   osg::ref_ptr<UtoDrawableCallback<UtoGeometryShapeImp>> m_Callback;
   bool                                                   mLighting;
   bool                                                   mUsesShader;
};

#endif // !defined(_UTOGEOMETRY_SHAPEIMP_HPP_)
