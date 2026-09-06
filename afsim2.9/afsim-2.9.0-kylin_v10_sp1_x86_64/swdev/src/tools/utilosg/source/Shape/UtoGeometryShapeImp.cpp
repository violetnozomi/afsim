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
// UtoGeometryShapeImp.cpp : implementation file

#include "UtoGeometryShapeImp.hpp"

#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/PrimitiveSet>
#include <osg/ShadeModel>
#include <osgUtil/Tessellator>

#include "UtCast.hpp"
#include "UtSphericalEarth.hpp"
#include "UtoEntity.hpp"
#include "UtoGeometryUtil.hpp"
#include "UtoPriv.hpp"

/* UTO Function Block **********************************************************

Function: Constructor

Description:
   Creates a geometry node, vertex array, and primitive array, then inserts
   them into the tree.

*******************************************************************************/

UtoGeometryShapeImp::UtoGeometryShapeImp()
   : m_SmoothSides(false)
   , m_ComplexPoly(false)
   , mDrawTangential(false)
   , mLighting(false)
   , mUsesShader(false)
{
   m_Geometry = new osg::Geometry;
   m_Geode->addDrawable(m_Geometry.get());

   // setup an empty vertex array
   m_Vertex = new osg::Vec3Array;
   m_Geometry->setVertexArray(m_Vertex.get());

   // setup a normal
   m_Normal = new osg::Vec3Array;
   m_Normal->push_back(osg::Vec3(0, 0, 1));
   m_Geometry->setNormalArray(m_Normal.get());
   m_Geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

   // setup the color
   osg::Vec4Array* color = new osg::Vec4Array;
   color->push_back(osg::Vec4(1, 1, 1, 1));
   m_Geometry->setColorArray(color);
   m_Geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

   // setup the DrawArrays, derived classes will set the mode
   m_PrimitiveSet = new osg::DrawArrays();
   m_Geometry->addPrimitiveSet(m_PrimitiveSet.get());
}

/* UTO Function Block **********************************************************

Function: Copy Constructor

Description:
   Clone the geometry node,
   add it to the base class's drawable,
   copy the vertex array,
   replace the old vertex array with the new one

*******************************************************************************/

UtoGeometryShapeImp::UtoGeometryShapeImp(const UtoGeometryShapeImp& rhs)
   : UtoGeodeShapeImp(rhs)
   , m_SideOffset(rhs.m_SideOffset)
   , m_SmoothSides(rhs.m_SmoothSides)
   , m_ComplexPoly(rhs.m_ComplexPoly)
   , mDrawTangential(rhs.mDrawTangential)
   , mLighting(rhs.mLighting)
   , mUsesShader(rhs.mUsesShader)
{
   m_Geometry = new osg::Geometry;
   m_Geode->addDrawable(m_Geometry.get());

   // copy the vertex array
   m_Vertex = new osg::Vec3Array(*rhs.m_Vertex.get());
   m_Geometry->setVertexArray(m_Vertex.get());

   // copy the normal array
   m_Normal = new osg::Vec3Array(*rhs.m_Normal.get());
   m_Geometry->setNormalArray(m_Normal.get());
   m_Geometry->setNormalBinding(rhs.m_Geometry->getNormalBinding());

   // determine the use of display lists
   m_Geometry->setUseDisplayList(rhs.UsingDisplayLists());

   // copy the color
   if (const osg::Vec4Array* rhs_color = dynamic_cast<const osg::Vec4Array*>(rhs.m_Geometry->getColorArray()))
   {
      osg::Vec4Array* color = new osg::Vec4Array(*rhs_color);
      m_Geometry->setColorArray(color);
      m_Geometry->setColorBinding(rhs.m_Geometry->getColorBinding());
      if (color->size() > 0)
      {
         double transparency = (*color)[0].w();
         if (transparency < 1)
         {
            // Enable blending if it is somewhat transparent, and make sure to
            // call it with the Uto API's convention.
            UtoGeometryUtil::EnableBlend(m_Geode.get());
         }
      }
   }

   // Brand new Geometry on the left, clone primitives.
   m_PrimitiveSet = new osg::DrawArrays(*rhs.m_PrimitiveSet.get());
   if (int count = rhs.m_Geometry->getNumPrimitiveSets())
   {
      osg::Geometry::PrimitiveSetList &left  = m_Geometry->getPrimitiveSetList(),
                                      &right = rhs.m_Geometry->getPrimitiveSetList();
      // If rhs.m_PrimitiveSet is the first entry, do the same for this object
      // and start cloning at the next primitive set.
      int i = 0;
      if (rhs.m_PrimitiveSet.get() == right[0].get())
      {
         left.push_back(m_PrimitiveSet.get());
         i = 1;
      }
      // clone over the rest
      for (; i < count; ++i)
      {
         left.push_back((osg::PrimitiveSet*)right[i]->clone(osg::CopyOp::DEEP_COPY_ALL));
      }
   }

   if (rhs.m_Geometry->getUpdateCallback())
      NeedCallback();
}

UtoGeometryShapeImp::~UtoGeometryShapeImp() {}

/* UTO Function Block **********************************************************

Function: SetColor

Description:
   Sets the RGB components of the color retaining the alpha portion.

*******************************************************************************/

void UtoGeometryShapeImp::SetColor(const UtoColor& Color, unsigned int nClrMask, unsigned int nReserved)
{
   // call the UtoShapeImp base class
   UtoShapeImp::SetColor(Color, nClrMask, nReserved);

   // there is only one entry in the color array list and it is the first entry
   osg::Vec4Array::iterator entry = dynamic_cast<osg::Vec4Array*>(m_Geometry->getColorArray())->begin();
   entry->x()                     = m_oBlendColor.RedF();
   entry->y()                     = m_oBlendColor.GreenF();
   entry->z()                     = m_oBlendColor.BlueF();
   entry->w()                     = m_oBlendColor.AlphaF();

   if (Color.HasTransparency() || mUsesShader)
   {
      UtoGeometryUtil::EnableBlend(m_Geode.get());
      m_Geode->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 0.0, 1.0, false));
      m_Geode->getOrCreateStateSet()->setRenderBinDetails(11, "DepthSortedBin");
      m_Geode->getOrCreateStateSet()->setNestRenderBins(false);
   }
   else
   {
      UtoGeometryUtil::DisableBlend(m_Geode.get());
      m_Geode->getOrCreateStateSet()->removeAttribute(osg::StateAttribute::DEPTH);
   }
   // Apply a material to set fill color
   osg::Material* material = new osg::Material;
   material->setDiffuse(osg::Material::FRONT, *entry);
   material->setSpecular(osg::Material::FRONT, osg::Vec4(0.0, 0.0, 0.0, 1.0));
   material->setAmbient(osg::Material::FRONT, *entry * 0.6f);
   material->setEmission(osg::Material::FRONT, osg::Vec4(0.0, 0.0, 0.0, 1.0));
   material->setShininess(osg::Material::FRONT, 25.0);
   m_Geode->getOrCreateStateSet()->setAttribute(material);

   m_Geometry->dirtyDisplayList();
}


void UtoGeometryShapeImp::SetOwner(UtoEntity* owner)
{
   UtoShapeImp::SetOwner(owner);
}

void UtoGeometryShapeImp::SetUseDisplayLists(bool bUse)
{
   m_Geometry->setUseDisplayList(bUse);
}

bool UtoGeometryShapeImp::UsingDisplayLists() const
{
   return m_Geometry->getUseDisplayList();
}

void UtoGeometryShapeImp::EnableComplexPoly(bool enable)
{
   if (m_ComplexPoly && !enable)
   {
      // Reset to the original vertex count.
      osg::Geometry::PrimitiveSetList& pset = m_Geometry->getPrimitiveSetList();
      pset.resize(0);
      pset.push_back(m_PrimitiveSet.get());
   }
   m_ComplexPoly = enable;
   NeedCallback();
}

void UtoGeometryShapeImp::SetSides(const UtoPosition& offset)
{
   m_SideOffset = offset;
   NeedCallback();
}

UtoPosition UtoGeometryShapeImp::Sides() const
{
   return UtoPosition(m_SideOffset.x(), m_SideOffset.y(), m_SideOffset.z());
}

void UtoGeometryShapeImp::EnableSmoothSides(bool enable)
{
   m_SmoothSides = enable;
   NeedCallback();
}

void UtoGeometryShapeImp::EnableLighting(bool aState)
{
   mLighting = aState;
   m_Geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, aState ? 1 : 0);
   m_Geode->getOrCreateStateSet()->setMode(GL_LIGHTING, aState ? 1 : 0);
}

void UtoGeometryShapeImp::SetUsesShader(bool aUsesShader)
{
   if (mUsesShader != aUsesShader)
   {
      mUsesShader = aUsesShader;
      SetColor(m_oBlendColor, UtoColor::RGBA);
   }
}

void UtoGeometryShapeImp::Update(osg::NodeVisitor* nv, osg::Drawable* drawable)
{
   // Reset the original drawables and vertices
   osg::Geometry::PrimitiveSetList& pset = m_Geometry->getPrimitiveSetList();
   pset.resize(1);
   pset[0]         = m_PrimitiveSet.get();
   const int count = m_PrimitiveSet->getNumIndices();
   m_Vertex->resize(count);
   m_Geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
   m_Normal->resize(1);

   // Tessellation, Without Tessellation polygons are required to be
   // simple convex polygons.  Tessellation removes those restrictions.
   // The m_PrimitiveSet will be removed and replaced as part of the
   // Tessellation process.
   if (m_ComplexPoly)
   {
      // Tessellator setup
      osg::ref_ptr<osgUtil::Tessellator> tes = new osgUtil::Tessellator();
      tes->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);

      // Tessellator performance tip, give it a normal.
      tes->setTessellationNormal(m_Normal->front());

      /* Do tessellation.  It requires a vertex array.  The Color,
       * Normal, Fog arrays are reduce to the count of the first
       * primitive set for the default polygon tessellation type.  The
       * output will be a set of DrawElementsUShort, which can be
       * triangle fan, triangle strip, triangles, or line loop (if
       * boarder was enabled).  For self intersecting polygons
       * additional vertices will be created at the intersection
       * points.
       */
      tes->retessellatePolygons(*m_Geometry);

      m_Geometry->dirtyDisplayList();
      m_Geometry->dirtyBound();
   }

   /* Enable simple 3D sides.  The one normal is used for the top and
    * bottom face of the 3D object.  The order of Tessellation, add
    * sides, duplicate faces, and calculate normals is very important.
    *
    * The tessleation comes first with only the original primitive set
    * which identifies the polygon.  The primitive set is replaced by
    * what could be multiple primitive sets to define the geometry.
    * Everything else depends on the result of the Tessellation and
    * additional vertices if any.
    *
    * Add Sides comes next and will add primitive sets to create a
    * quad strip around the perimeter of the geometry connecting each
    * existing vertex to vertex index where the vertex will be offset
    * (but isn't yet in the list).  This must come before the
    * duplicate faces call so that only the top face has primitive
    * sets and the Tessellation process for self intersecting geometry
    * would throw away the additional vertices.
    *
    * DuplicateFace will duplicate each vertex and primitive set
    * except the quad strip produced by the AddSides routine.  The
    * vertices will be offset by m_SideOffset amount and each new
    * primitive set will have their index modified to use the new
    * vertices.
    *
    * CalculateNormals is the most complex of the steps.  The vertices
    * for the top face are assigned the first normal in the normal
    * array list and the negative of that normal for the bottom.  The
    * normals for the sides are calculated and assigned according to
    * if they should be flat shaded or smooth shaded.  Smooth shading
    * takes the average of adjacent faces with special cases for
    * connecting the first and last quads together.
    */
   if (HasSides())
   {
      // Close for all but the line strip.
      bool close = m_PrimitiveSet->getMode() != osg::PrimitiveSet::LINE_STRIP;
      AddSidesPreOffset(close);
      DuplicateFace(m_SideOffset);
      AddSidesPostOffset(close);
      CalculateNormals();

      // The lines in a polyline (or ellipse line loop)are redundant
      // with the top edge of the quad.  The lighting will cause them
      // to be a different color, if that is desired they can draw
      // another polyline without the sides.
      for (size_t i = 0; i < pset.size();)
      {
         GLenum mode = pset[i]->getMode();
         if (mode == GL_LINE_STRIP || mode == GL_LINE_LOOP)
            pset.erase(pset.begin() + i);
         else
            ++i;
      }

      m_Geometry->dirtyDisplayList();
      m_Geometry->dirtyBound();
   }
   else
   {
      // CalculateNormals enables lighting, disable it.
      m_Geometry->setStateSet(nullptr);
   }

   // One shot callback, disable it.
   m_Geometry->setUpdateCallback(nullptr);
}

void UtoGeometryShapeImp::NeedCallback()
{
   if (!m_Callback.get())
      m_Callback = new UtoDrawableCallback<UtoGeometryShapeImp>(this);

   m_Geometry->setUpdateCallback(m_Callback.get());
}

/* The simple version connects vertex 0 through count-1 with vertex
 * count through 2*count-2 and optionally closes the boundary.
 */
void UtoGeometryShapeImp::AddSidesSimple(bool close)
{
   auto                     count = m_Vertex->size();
   osg::DrawElementsUShort* elem  = new osg::DrawElementsUShort(GL_QUAD_STRIP, 2U * (count + (close ? 1U : 0U)));
   m_Geometry->addPrimitiveSet(elem);

   // If vertices represent a full circle, ignore first side, since it should be a closed shape.
   size_t startIndex = m_FullCircle ? 1u : 0u;
   if (m_FullCircle && elem->size() >= 4)
   {
      elem->resize(elem->size() - 4);
   }

   size_t                   o = 0U;
   osg::DrawElementsUShort& e = *elem;
   for (size_t i = startIndex; i < count; ++i)
   {
      e[o++] = ut::safe_cast<GLushort, size_t>(i);
      e[o++] = ut::safe_cast<GLushort, size_t>(i + count);
   }
   if (close && !m_FullCircle)
   {
      // back to the first set
      e[o++] = startIndex;
      e[o++] = ut::safe_cast<GLushort, size_t>(count);
   }
}

/* The close case is not handled.  The only reason it wouldn't be closed
 * is polylines and the Tessellator doesn't handle lines, so AddSidesComplex
 * will not be called in that case.
 */
void UtoGeometryShapeImp::AddSidesComplex(bool close)
{
   osgUtil::Tessellator tes;
   tes.setTessellationNormal(m_Normal->front());

   /* This Tessellation will produce a set of line loops.  I've
    * observed that any additional vertices will match the
    * set produced by the non-boundary Tessellation method.
    */
   tes.setBoundaryOnly(true);
   tes.setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
   // Start the quad primitives with the current set and
   // get a reference to the primitives.
   osg::Geometry::PrimitiveSetList quadset  = m_Geometry->getPrimitiveSetList(),
                                   &lineset = m_Geometry->getPrimitiveSetList();
   // reset to the original primitive set
   lineset.clear();
   lineset.push_back(m_PrimitiveSet.get());
   tes.retessellatePolygons(*m_Geometry);
   const size_t total = m_Vertex->size();
   for (size_t i = 0; i < lineset.size(); ++i)
   {
      osg::DrawElementsUShort* loop = dynamic_cast<osg::DrawElementsUShort*>(lineset[i].get());
      // Tessellation only returns lines, but to be safe...
      if (!loop || loop->getMode() != GL_LINE_LOOP)
         continue;
      osg::DrawElementsUShort* quad = new osg::DrawElementsUShort(GL_QUAD_STRIP);
      quadset.push_back(quad);
      // Connect the current top face with the vertices that
      // will be added for the bottom face.
      for (size_t j = 0; j < loop->size(); ++j)
      {
         quad->push_back((*loop)[j]);
         quad->push_back((*loop)[j] + total);
      }
      quad->push_back((*loop)[0]);
      quad->push_back((*loop)[0] + total);
   }
   lineset = quadset;
}

void UtoGeometryShapeImp::AddSidesPreOffset(bool close)
{
   // Self intersecting polygons have additional vertices.
   if (m_PrimitiveSet->getNumIndices() != m_Vertex->getNumElements())
      AddSidesComplex(close);
   else
      AddSidesSimple(close);
}

void UtoGeometryShapeImp::AddSidesPostOffset(bool close) {}

void UtoGeometryShapeImp::DuplicateFace(const osg::Vec3& offset)
{
   // duplicate the vertices with an offset
   osg::Vec3Array& v      = *m_Vertex;
   const int       pcount = m_Geometry->getNumPrimitiveSets();
   const int       num    = v.size();
   v.resize(num * 2);

   // duplicate vertices tangential if zone is polygonal and declared as lat_lon/mgrs
   // duplicate with normal offset otherwise
   if (GetReferenceFrame() == UtoShape::cFRAME_WORLD_AT_ENTITY && mDrawTangential)
   {
      bool aboveGround = true;
      for (int i = 0; i < num; ++i)
      {
         auto      shapeU_Pos = m_Owner->GetLocationWCS();
         osg::Vec3 shapePos   = osg::Vec3(shapeU_Pos[0], shapeU_Pos[1], shapeU_Pos[2]);
         double    vlen       = offset.length();

         if (i == 0)
         {
            aboveGround = shapePos.length() > UtSphericalEarth::cEARTH_MEAN_RADIUS;
         }

         if (!aboveGround)
         {
            v[i] *= shapePos.length() / UtSphericalEarth::cEARTH_MEAN_RADIUS;
         }

         osg::Vec3 p = shapePos + v[i];
         p.normalize();

         v[i + num] = v[i] + (p * vlen);
      }
   }
   else
   {
      for (int i = 0; i < num; ++i)
      {
         v[i + num] = v[i] + offset;
      }
   }

   // duplicate primitives with an offset
   osg::PrimitiveSet* pset;
   for (int i = 0; i < pcount; ++i)
   {
      // quad strips are the sides and don't get duplicated
      pset = m_Geometry->getPrimitiveSet(i);
      if (pset->getMode() == GL_QUAD_STRIP)
         continue;
      pset = (osg::PrimitiveSet*)pset->clone(osg::CopyOp::DEEP_COPY_ALL);
      pset->offsetIndices(num);
      m_Geometry->addPrimitiveSet(pset);
   }
}

void UtoGeometryShapeImp::AddNormal(osg::DrawElementsUShort& draw, int index, const osg::Vec3& n)
{
   // duplicate vertex, add the normal, update the draw elements index
   // This is required, because you can't assign multiple normals to a
   // given vertex in the array all vertices will be part of the edge
   // corner which will be part of the top surface and side, so if the
   // side has to be assigned a normal it must be duplicated to a new
   // array index first.
   m_Vertex->push_back((*m_Vertex)[draw[index]]);
   draw[index] = m_Vertex->size() - 1;
   m_Normal->push_back(n);
}

void UtoGeometryShapeImp::CalculateNormals()
{
   // Easier syntax for array accesses.
   osg::Vec3Array& vect   = *m_Vertex;
   osg::Vec3Array& normal = *m_Normal;

   // one state set for smooth and one for flat
   if (m_SmoothSides)
   {
      osg::StateSet*   ss = m_Geometry->getOrCreateStateSet();
      osg::ShadeModel* sm = new osg::ShadeModel;
      sm->setMode(osg::ShadeModel::SMOOTH);
      ss->setAttribute(sm);
      if (mLighting)
      {
         ss->setMode(GL_LIGHTING, osg::StateAttribute::ON);
      }
   }
   else
   {
      osg::StateSet*   ss = m_Geometry->getOrCreateStateSet();
      osg::ShadeModel* sm = new osg::ShadeModel;
      sm->setMode(osg::ShadeModel::FLAT);
      ss->setAttribute(sm);
      if (mLighting)
      {
         ss->setMode(GL_LIGHTING, osg::StateAttribute::ON);
      }
   }

   // up or first vertex for the polygon normal primitives
   osg::Vec3 pn(0, 0, 1);
   if (!normal.empty())
      pn = normal.front();

   m_Geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

   // one normal per vertex
   normal.resize(vect.size());

   size_t i;
   // assign up to first half
   for (i = 0; i < vect.size() / 2; ++i)
      normal[i] = pn;

   // next half down polygon normal
   for (; i < vect.size(); ++i)
      normal[i] = -pn;

   osg::Geometry::PrimitiveSetList& pset = m_Geometry->getPrimitiveSetList();
   osg::DrawElementsUShort*         drawElements;
   // assign normals
   for (i = 0; i < pset.size(); ++i)
   {
      // Calculate the normals for a quad strip or assign the
      // polygon normal from above to each.
      osg::PrimitiveSet* p = pset[i].get();
      if (p->getMode() != GL_QUAD_STRIP || !(drawElements = dynamic_cast<osg::DrawElementsUShort*>(p)))
         continue;

      osg::DrawElementsUShort& draw = *drawElements;
      if (m_SmoothSides)
      {
         // Four vertices to a quad, use the first three to
         // calculate the normal, for flag shading, assing
         // it to the last vertex in the surface.  For smooth
         // each vertex gets a normal that is the average between
         // the two faces.
         size_t jstop = draw.size();
         // The first face's normal and the last surface
         // calculated.
         osg::Vec3 nFirst, nLast, nAvg;
         for (size_t j = 0; j + 3 < jstop; j += 2)
         {
            const osg::Vec3 &v0 = vect[draw[j]], &v1 = vect[draw[j + 1]], &v2 = vect[draw[j + 2]];
            osg::Vec3        n = (v0 - v1) ^ (v1 - v2);
            n.normalize();
            if (!j)
            {
               nFirst = nLast = n;
               continue;
            }

            // Average and keep the normal around.
            nAvg = n + nLast;
            nAvg.normalize();
            nLast = n;

            AddNormal(draw, j, nAvg);
            AddNormal(draw, j + 1, nAvg);
         }
         // Fix up first and last vertices.  If the first and
         // last vertex index or vertex value matches smooth
         // shade.  Three cases, polygon where the indices
         // will match, polyline where the vertices match, and
         // polyline where the vertices don't.
         const unsigned short &fi = draw[0], &li = draw[jstop - 2];
         if (fi == li || vect[fi] == vect[li])
         {
            // closed: Blend between the first and last
            // normals.
            nAvg = nFirst + nLast;
            nAvg.normalize();

            AddNormal(draw, 0, nAvg);
            AddNormal(draw, 1, nAvg);

            // the last two are the same as the first two
            draw[jstop - 2] = draw[0];
            draw[jstop - 1] = draw[1];
         }
         else
         {
            // open: The first two and last two get their
            // surface normal.

            AddNormal(draw, 0, nFirst);
            AddNormal(draw, 1, nFirst);
            AddNormal(draw, jstop - 2, nLast);
            AddNormal(draw, jstop - 1, nLast);
         }
      }
      else
      {
         // Four vertices to a quad, use the first three to
         // calculate the normal, for flag shading, assing
         // it to the last vertex in the surface.
         size_t jstop = draw.size();
         for (size_t j = 0; j + 3 < jstop; j += 2)
         {
            const osg::Vec3 &v0 = vect[draw[j]], &v1 = vect[draw[j + 1]], &v2 = vect[draw[j + 2]];
            osg::Vec3        n = (v0 - v1) ^ (v1 - v2);
            n.normalize();
            AddNormal(draw, j + 3, n);
         }
      }
   }
}
