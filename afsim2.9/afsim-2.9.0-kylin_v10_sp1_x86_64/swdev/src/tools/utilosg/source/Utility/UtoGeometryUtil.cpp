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
// GeometryUtil.cpp : implementation file

#include "UtoGeometryUtil.hpp"

#include <assert.h>
#include <sstream>
#include <vector>

#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osg/StateAttribute>
#include <osg/TexEnv>
#include <osgUtil/CullVisitor>

#include "UtoEntity.hpp"
#include "UtoOptions.hpp"
#include "UtoShapeImp.hpp"
#include "UtoViewer.hpp"

static const double EPSILON = 0.000001;

UtoEntity* UtoGeometryUtil::GetEntity(osg::NodePath& path, bool SceneRoot)
{
   // To get access to the UtoEntityImp object we look at the path of nodes
   // to this one.  Entities have two group nodes before them in the tree,
   // are of type osg::MatrixTransform, and have a name that starts with
   // "UtoEntityImp: ".  A node path can either start at the scene graph
   // root placing the entity node to be node 2 in the path or the path can
   // start at the database node causing it to be node 1 the boolean
   // SceneRoot specifies which it is.

   UtoEntity* entity = nullptr;
   for (size_t index = 0, max = path.size(); index < max; ++index)
   {
      osg::Referenced*    ref = path[index]->getUserData();
      UtoViewerShapeData* ve  = dynamic_cast<UtoViewerShapeData*>(ref);
      if (ve)
      {
         entity = ve->shape->Owner();
         break;
      }
   }

   return entity;
}

// static
UtoShapeImp* UtoGeometryUtil::GetShape(osg::NodePath& path, bool SceneRoot)
{
   for (size_t index = 0, max = path.size(); index < max; ++index)
   {
      osg::Referenced*    ref = path[index]->getUserData();
      UtoViewerShapeData* vs  = dynamic_cast<UtoViewerShapeData*>(ref);
      if (vs)
      {
         return vs->shape;
      }
   }
   return nullptr;
}


void UtoGeometryUtil::EnableBlend(osg::Node* node)
{
   osg::StateSet* stateset = node->getOrCreateStateSet();
   stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
   stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
}

void UtoGeometryUtil::DisableBlend(osg::Node* node)
{
   osg::StateSet* stateset = node->getOrCreateStateSet();
   stateset->setMode(GL_BLEND, osg::StateAttribute::OFF);
   stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
}

void UtoGeometryUtil::SetMaterialColor(osg::Material* material, const UtoColor& color, double transparency)
{
   // See the osg::Material code for more information.  It shows that a color
   // mode of OFF means that the material is disabled and the color is
   // set by glColor using the diffuse color, but it seems like the
   // color is coming from the ambient and the alpha from the diffuse.
   osg::Vec4 v4color(color.RedF(), color.GreenF(), color.BlueF(), transparency);
   material->setDiffuse(osg::Material::FRONT_AND_BACK, v4color);
   material->setAmbient(osg::Material::FRONT_AND_BACK, v4color);
}

osg::ref_ptr<osg::TexEnv> UtoGeometryUtil::m_TexEnv = nullptr;

osg::StateSet* UtoGeometryUtil::SetupColorMaterialState()
{
   // Each node needs its own TexEnv to set the color.
   osg::StateSet* stateset = new osg::StateSet;
   if (!m_TexEnv.get())
   {
      m_TexEnv = new osg::TexEnv;
      m_TexEnv->setMode(osg::TexEnv::MODULATE);
   }
   stateset->setTextureAttribute(0, m_TexEnv.get(), osg::StateAttribute::ON);

   // The material is used in two ways.  It adds diffuse color to the
   // texture for the SetColor method and it picks up the light.
   // By default it just colors the texture, SetLightOn will enable
   // the lighting effects.
   osg::Material* material = new osg::Material;
   material->setColorMode(osg::Material::OFF);
   // material->setColorMode(osg::Material::DIFFUSE);

   // setup the light colors for lighting
   const osg::Material::Face fb = osg::Material::FRONT_AND_BACK;
   material->setAmbient(fb, osg::Vec4(1, 1, 1, 1.0));
   // setDiffuse is updated in the SetColor function
   material->setDiffuse(fb, osg::Vec4(1, 1, 1, 1));
   material->setSpecular(fb, osg::Vec4(0.4, 0.4, 0.4, 1.0));
   material->setEmission(fb, osg::Vec4(0.1, 0.1, 0.1, 1.0));
   material->setShininess(fb, 64.0);

   stateset->setAttribute(material, osg::StateAttribute::ON);

   // setup color blending
   /*osg::BlendColor* blend_clr = new osg::BlendColor;
   blend_clr->setConstantColor(osg::Vec4(1.0, 1.0, 1.0, 1.0));
   stateset->setAttribute(blend_clr, osg::StateAttribute::OFF);

   osg::BlendFunc* blend_fn = new osg::BlendFunc;
   //blend_fn->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_COLOR);
   blend_fn->setFunction(GL_CONSTANT_ALPHA, GL_DST_ALPHA);
   stateset->setAttribute(blend_fn, osg::StateAttribute::OFF);*/

   return stateset;
}

bool UtoGeometryUtil::AttachColorTransparencyPixelShader(osg::Node*         pNode,
                                                         const UtoColor&    rColor,
                                                         const std::string& rVertexShader,
                                                         const std::string& rPixelShader)
{
   // local(s)
   bool bShaderDisabled = UtoOptions::Instance()->DisableShaderSupport();
   ;

   // determine if a shader is to be used by the application
   if (!bShaderDisabled)
   {
      // local(s)
      osg::ref_ptr<osg::Shader> pPixelShader;
      osg::ref_ptr<osg::Shader> pVertexShader;

      if (!rVertexShader.empty())
      {
         // create a new vertex shader
         pVertexShader = new osg::Shader(osg::Shader::VERTEX);
         // load the source into the shader
         pVertexShader->setShaderSource(rVertexShader);
      }

      if (!rPixelShader.empty())
      {
         // create a new fragment shader to be used
         pPixelShader = new osg::Shader(osg::Shader::FRAGMENT);
         // load the source into the shader
         pPixelShader->setShaderSource(rPixelShader);
      }

      // create a new program to consume the shader
      osg::ref_ptr<osg::Program> pProgram = new osg::Program;

      bool bPixelShader  = true;
      bool bVertexShader = true;

      // attach the shaders to the program
      if (pPixelShader.get())
         bPixelShader = pProgram->addShader(pPixelShader.get());
      if (pVertexShader.get())
         bVertexShader = pProgram->addShader(pVertexShader.get());

      if (bPixelShader && bVertexShader)
      {
         // set up the attributes for the root node
         pNode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
         pNode->getOrCreateStateSet()->setAttribute(pProgram.get(), osg::StateAttribute::ON);
         // change the blend color
         SetColorTransparencyPixelShader(pNode, rColor);
         // indicate that the shader is not to be disabled
         bShaderDisabled = false;
      }
   }

   return !bShaderDisabled;
}

void UtoGeometryUtil::RemoveColorTransparencyPixelShader(osg::Node* pNode)
{
   pNode->getOrCreateStateSet()->removeAttribute(osg::Program::PROGRAM);
}

void UtoGeometryUtil::SetColorTransparencyPixelShader(osg::Node* pNode, const UtoColor& rColor)
{
   // create a color vector
   osg::Vec4 oColor((float)rColor.RedF(), (float)rColor.GreenF(), (float)rColor.BlueF(), (float)rColor.AlphaF());

   // change the blending color inside the fragment shader
   pNode->getOrCreateStateSet()->getOrCreateUniform("BlendValue", osg::Uniform::FLOAT_VEC4)->set(oColor);
}

void UtoGeometryUtil::ComputeOffsetLine(UtoViewer* aViewerPtr, osg::Vec3& aSourceWCS, osg::Vec3& aTargetWCS, float aPixelOffset)
{
   if (aViewerPtr->ActiveCamera() == UtoViewer::PERSPECTIVE)
   {
      // Reset the matrix just in case it was an orthographic camera
      // last frame.
      UtoCameraPerspective* cam = aViewerPtr->GetPerspective();
      UtoPosition           eye = cam->Position();

      // vector eye (camera) position
      const osg::Vec3 veye(eye);
      // vector entity position
      const osg::Vec3 vepos(aSourceWCS);
      const double    height = aViewerPtr->DisplaySize().cy;
      // vertical OpenGL Units to screen height scaling
      const double scale = 2 * tan(cam->FOV() * M_PI / 180 / 2) / height;

      // eye to shape distance, OpenGL units
      const double shape_depth  = (veye - vepos).length();
      const double shape_depth2 = (veye - aTargetWCS).length();

      // depth * scaling to GL Unit
      const double zoom = (shape_depth < shape_depth2) ? shape_depth * scale : shape_depth2 * scale;

      osg::Vec3 v1(aSourceWCS);
      osg::Vec3 v2(aTargetWCS);

      osg::Vec3 dvec = aSourceWCS - aTargetWCS; // line direction
      osg::Vec3 evec = veye - aSourceWCS;       // view direction

      osg::Vec3 ovec = dvec ^ evec; // perpendicular (offset direction)
      ovec.normalize();

      v1 += ovec * zoom * aPixelOffset;
      v2 += ovec * zoom * aPixelOffset;

      aSourceWCS = v1;
      aTargetWCS = v2;
   }
   else
   {
      // Reset the matrix just in case it was an orthographic camera
      // last frame.
      UtoCameraOrtho* cam = aViewerPtr->GetOrtho();
      UtoPosition     eye = cam->Position();

      // vector eye (camera) position
      const osg::Vec3 veye(eye);
      // vector entity position
      const osg::Vec3 vepos(aSourceWCS);

      double zoom = cam->Zoom();
      // double vmat[4][4];
      const osg::Matrixd& vmat = cam->GetViewMatrix();

      // cam->ViewMatrix(vmat);

      osg::Vec3 v1(vepos[0], vepos[1], vepos[2]);
      osg::Vec3 v2(aTargetWCS);

      osg::Vec3 dvec = v1 - v2;
      osg::Vec3 evec(vmat(0, 2), vmat(1, 2), vmat(2, 2));

      osg::Vec3 ovec = dvec ^ evec;
      ovec.normalize();

      v1 += ovec * zoom * aPixelOffset;
      v2 += ovec * zoom * aPixelOffset;

      aSourceWCS = v1;
      aTargetWCS = v2;
   }
}

#if 0
std::string UtoGeometryUtil::DumpNode (vtNode* node)
{
   std::stringstream sstr;
   sstr << node->Name () << " (";

   switch (node->ObjectType ())
   {
      case (vtNode::Node) : sstr << "Node"; break;
      case (vtNode::GNode) : sstr << "GNode"; break;
      case (vtNode::TNode) : sstr << "TNode"; break;
      case (vtNode::STNode) : sstr << "STNode"; break;
      case (vtNode::DTNode) : sstr << "DTNode"; break;
      case (vtNode::RSNode) : sstr << "RSNode"; break;
      case (vtNode::LODNode) : sstr << "LODNode"; break;
      case (vtNode::SNode) : sstr << "SNode"; break;
      case (vtNode::KNode) : sstr << "KNode"; break;
      case (vtNode::PNode) : sstr << "PNode"; break;
      case (vtNode::BNode) : sstr << "BNode"; break;
      case (vtNode::CBNode) : sstr << "CBNode"; break;
      case (vtNode::SONode) : sstr << "SONode"; break;
      case (vtNode::ASNode) : sstr << "ASNode"; break;
      case (vtNode::TextNode) : sstr << "TextNode"; break;
      case (vtNode::FTNode) : sstr << "FTNode"; break;
      case (vtNode::IGNode) : sstr << "IGNode"; break;
      case (vtNode::GrpNode) : sstr << "GrpNode"; break;
      case (vtNode::InterpGeomNode) : sstr << "InterpGeomNode"; break;
      case (vtNode::StrokeTextNode) : sstr << "StrokeTextNode"; break;
      case (vtNode::GaugeNode) : sstr << "GaugeNode"; break;
      case (vtNode::LightPointNode) : sstr << "LightPointNode"; break;
      case (vtNode::InstanceNode) : sstr << "InstanceNode"; break;
      case (vtNode::NilNode) : sstr << "NilNode"; break;
      default: sstr << "Unknown"; break;
   }

   sstr << "," << (int)node->ObjectType () << ")";

   return sstr.str ();
}

bool UtoGeometryUtil::Intersect (const vtLineSegment& ray, PolyIterator& PolyIter, const vtGMatrix& mat, vtVector& hit)
{
   int num_verts = PolyIter.VertsPerPoly ();
   ASSERT (num_verts >= 3);
   vtVector vert0, vert1, vert2;
   vtMultiply (vert0, mat, *PolyIter.Vert (0));
   vtMultiply (vert1, mat, *PolyIter.Vert (1));
   vtMultiply (vert2, mat, *PolyIter.Vert (2));

   if (num_verts == 3)
   {
      return IntersectTri (ray, vert0, vert1, vert2, hit);
   }
   else if (num_verts == 4)
   {
      if (!IntersectTri (ray, vert0, vert1, vert2, hit))
      {
         vtMultiply (vert1, mat, *PolyIter.Vert (3));
         return IntersectTri (ray, vert0, vert2, vert1, hit);
      }
      return true;
   }
   else
   {
      int index = 1;
      bool done = false;
      while (!done)
      {
         done = IntersectTri (ray, vert0, vert1, vert2, hit);
         if (++index == num_verts)
            break;

         vert1 = vert2;
         vtMultiply (vert2, mat, *PolyIter.Vert (index + 1));
      }
      return done;
   }
   return false;
}

bool UtoGeometryUtil::IntersectTri (const vtLineSegment& ray, const vtVector& vert0,
                                    const vtVector& vert1, const vtVector& vert2, vtVector& hit)
{
   /* Moller-Trumbore ray-triangle intersection algorithm (non-culling)

      int intersect_triangle(double orig[3], double dir[3],
                             double vert0[3], double vert1[3], double vert2[3],
                             double *t, double *u, double *v)

      <orig[3]> : origin of ray     = vtLineSegment
      <dir[3]> : direction of ray
      <vert0[3]> : vertex 1 of triangle  = PolyIterator
      <vert1[3]> : vertex 2 of triangle
      <vert2[3]> : vertex 3 of triangle
      <*t> : distance to triangle plane from ray origin (?)
      <*u, *v> : coordinate of point in triangle
   */

   vtVector edge1, edge2, tvec, pvec, qvec;
   double det,inv_det;

   /* find vectors for two edges sharing vert0 */
   //SUB(edge1, vert1, vert0);
   vtSub (edge1, vert1, vert0);
   //SUB(edge2, vert2, vert0);
   vtSub (edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   //CROSS(pvec, dir, edge2);
   vtCross (pvec, ray.Direction (), edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   //det = DOT(edge1, pvec);
   det = vtDot (edge1, pvec);

   if (det > -EPSILON && det < EPSILON)
     return false;
   inv_det = 1.0 / det;

   /* calculate distance from vert0 to ray origin */
   //SUB(tvec, orig, vert0);
   vtSub (tvec, ray.Position (), vert0);

   /* calculate U parameter and test bounds */
   //*u = DOT(tvec, pvec) * inv_det;
   double u = vtDot (tvec, pvec) * inv_det;
   if (u < 0.0 || u > 1.0)
     return false;

   /* prepare to test V parameter */
   //CROSS(qvec, tvec, edge1);
   vtCross (qvec, tvec, edge1);

   /* calculate V parameter and test bounds */
   //*v = DOT(dir, qvec) * inv_det;
   double v = vtDot (ray.Direction (), qvec) * inv_det;
   if (v < 0.0 || u + v > 1.0)
     return false;

   /* calculate t, ray intersects triangle */
   //*t = DOT(edge2, qvec) * inv_det;
   //double t = vtDot (edge2, qvec) * inv_det;  // is this necessary? jrp

   // find the coordinates of the hit point from the barycentrics coordinates:
   vtScale (edge1, u);
   vtScale (edge2, v);
   vtAdd (hit, vert0, edge1);
   vtAdd (hit, edge2);

   return true;
}
#endif
