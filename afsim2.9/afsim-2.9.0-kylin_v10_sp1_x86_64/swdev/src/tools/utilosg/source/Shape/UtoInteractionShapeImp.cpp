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
// UtoInteractionShapeImp.cpp : implementation file

#include "UtoInteractionShapeImp.hpp"

#include <string.h>

#include <osg/BlendFunc>
#include <osg/Geometry>
#include <osg/Image>
#include <osg/PrimitiveSet>
#include <osg/TexEnv>
#include <osg/Texture1D>

#include "UtEarth.hpp"
#include "UtoEntity.hpp"
#include "UtoInteractionShape.hpp"
#include "UtoViewer.hpp"
#include "UtoWallClock.hpp"

UtoInteractionShapeImp::UtoInteractionShapeImp()
   : UtoAttrPolyLineShapeImp()
   , mTexture(nullptr)
   , mFillFactor(1)
   , mArchSegments(false)
   , mMapProjection()
{
   m_TexCoord = new osg::FloatArray;
   m_Geometry->setName("CmeInteractionShape");
   m_Geometry->setTexCoordArray(0, m_TexCoord.get());

   m_Geometry->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
   m_Geometry->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(), osg::StateAttribute::ON);

   if (!m_Callback.get())
   {
      m_Callback = new UtoUpdateCallback<UtoInteractionShapeImp>(this);
   }
   m_RootNode->setUpdateCallback(m_Callback.get());
}

// osg::ref_ptr< osg::StateSet > g_pCmeInteractionShapeStateSet;

UtoInteractionShapeImp::UtoInteractionShapeImp(const UtoInteractionShapeImp& rhs)
   : UtoAttrPolyLineShapeImp(rhs)
   , mTexture(nullptr)
   , mFillFactor(rhs.mFillFactor)
   , mArchSegments(rhs.mArchSegments)
   , mMapProjection(rhs.mMapProjection)
{
   // Make the shading be flat.
   /*   if(!g_pCmeInteractionShapeStateSet.get())
      {
         g_pCmeInteractionShapeStateSet = new osg::StateSet;
         g_pCmeInteractionShapeStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);  // we want a seperate state
      set from polyline
      }
      m_Geometry->setStateSet(g_pCmeInteractionShapeStateSet.get());*/

   m_TexCoord = new osg::FloatArray(*rhs.m_TexCoord);
   m_Geometry->setName("CmeInteractionShape");
   m_Geometry->setTexCoordArray(0, m_TexCoord.get());

   m_Geometry->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
   m_Geometry->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(), osg::StateAttribute::ON);

   if (!m_Callback.get())
   {
      m_Callback = new UtoUpdateCallback<UtoInteractionShapeImp>(this);
   }
   m_RootNode->setUpdateCallback(m_Callback.get());
}

UtoInteractionShapeImp::~UtoInteractionShapeImp()
{
   if (mTexture != nullptr)
   {
      delete[] mTexture;
   }
   Clear();
}

int UtoInteractionShapeImp::Remove(int pos)
{
   int size = m_TexCoord->size();
   // check for user errors
   if (pos >= size || pos < 0)
      return -1;

   m_TexCoord->erase(m_TexCoord->begin() + pos);

   return UtoAttrPolyLineShapeImp::Remove(pos);
}

void UtoInteractionShapeImp::Clear()
{
   for (auto&& interaction : mInteractionList)
   {
      interaction.mTarget->disconnect(UtoEntity::DestroyedEvent(), this, &UtoInteractionShapeImp::OnRemoveEntityCB);
   }
   mInteractionList.clear();
}

int UtoInteractionShapeImp::Insert(int pos, const UtoPosition pts[], const UtoColor col[], const float texco[], int num)
{
   // insert the new points
   for (int i = 0; i < num; ++i)
   {
      m_TexCoord->insert(m_TexCoord->begin() + pos + i, texco[i]);
   }
   // the first element is already in place

   return UtoAttrPolyLineShapeImp::Insert(pos, pts, col, num);
}

// osg::ref_ptr< osg::StateSet > g_CmeInteractionShapeStateSet;

// this takes responsibility for the aPattern memory, use new to create the pattern
void UtoInteractionShapeImp::SetTexturePattern(unsigned char* aPattern, const int aSize)
{
   if (mTexture != nullptr)
   {
      delete[] mTexture;
   }
   mTexture = new unsigned char[aSize];
   memcpy(mTexture, aPattern, aSize);
   osg::ref_ptr<osg::Image> image = new osg::Image();
   image->setImage(aSize, 1, 1, GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE, mTexture, osg::Image::NO_DELETE);
   osg::ref_ptr<osg::Texture1D> texture = new osg::Texture1D();
   texture->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);
   texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
   texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
   texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
   texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
   texture->setImage(image.get());
   osg::ref_ptr<osg::TexEnv> texenv   = new osg::TexEnv(osg::TexEnv::BLEND);
   osg::StateSet*            stateset = m_Geometry->getOrCreateStateSet();
   stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   stateset->setTextureAttributeAndModes(0, texture.get(), osg::StateAttribute::ON);
   stateset->setTextureAttribute(0, texenv.get(), osg::StateAttribute::ON);
}

void UtoInteractionShapeImp::Update(osg::NodeVisitor* nv, osg::Drawable* drawable)
{
   // Reset the original drawables and vertices
   osg::Geometry::PrimitiveSetList& pset = m_Geometry->getPrimitiveSetList();
   pset.resize(1);
   pset[0]         = m_PrimitiveSet.get();
   const int count = m_PrimitiveSet->getNumIndices();
   m_Vertex->resize(count);
   m_Geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
   m_Normal->resize(1);

   // One shot callback, disable it.
   m_Geometry->setUpdateCallback(nullptr);
}

void UtoInteractionShapeImp::Update(UtoRenderInfo* info, osg::NodeVisitor* nv)
{
   int index = m_TexCoord->size();
   // Call remove until all elements have been removed.
   while (Remove(--index) != -1)
   {
   }

   float offset = UtoWallClock::GetClock() * 0.5; // * 0.033;

   if (info->m_Viewer->ActiveCamera() == UtoViewer::PERSPECTIVE)
   {
      // Reset the matrix just in case it was an orthographic camera
      // last frame.
      UtoCameraPerspective* cam = info->m_Viewer->GetPerspective();
      UtoPosition           eye = cam->Position();
      double                eposArray[3];
      m_Owner->GetLocationWCS(eposArray);
      mMapProjection.ConvertFromECEF(eposArray);
      osg::Vec3 epos(eposArray[0], eposArray[1], eposArray[2]);

      // vector shape position
      const osg::Vec3 vspos(m_Position);
      // vector eye (camera) position
      const osg::Vec3 veye(eye);
      // vector entity position
      const osg::Vec3 vepos(epos[0], epos[1], epos[2]);
      const double    height = info->m_Viewer->DisplaySize().cy;
      // vertical OpenGL Units to screen height scaling
      const double scale = 2 * tan(cam->FOV() * M_PI / 180 / 2) / height;

      // eye to shape distance, OpenGL units
      const double shape_depth = (veye - (vepos + vspos)).length();

      int index = m_TexCoord->size();
      // Call remove until all elements have been removed.
      while (Remove(--index) != -1)
      {
      }

      std::vector<Interaction>::iterator it;
      int                                p = 0;
      for (it = mInteractionList.begin(); it != mInteractionList.end(); ++it)
      {
         double epos2[3];
         it->mTarget->GetLocationWCS(epos2);
         mMapProjection.ConvertFromECEF(epos2);
         const osg::Vec3 ve2pos(epos2[0], epos2[1], epos2[2]);
         const double    shape_depth2 = (veye - (ve2pos + vspos)).length();
         // depth * scaling to GL Unit
         const double zoom = (shape_depth < shape_depth2) ? shape_depth * scale : shape_depth2 * scale;

         if (mArchSegments)
         {
            osg::Vec3 v1(vepos[0], vepos[1], vepos[2]);
            double    v2array[3];
            it->mTarget->GetLocationWCS(v2array);
            mMapProjection.ConvertFromECEF(v2array);
            osg::Vec3 v2(v2array[0], v2array[1], v2array[2]);

            osg::Vec3 dvec = v1 - v2;      // line direction
            osg::Vec3 evec = veye - vepos; // view direction

            osg::Vec3 ovec = dvec ^ evec; // perpendicular (offset direction)
            ovec.normalize();

            v1 += ovec * (it->mOffset + 0.5) * zoom * Width();
            v2 += ovec * (it->mOffset + 0.5) * zoom * Width();

            UtoPosition sv1(v1[0] - vepos[0], v1[1] - vepos[1], v1[2] - vepos[2]);
            UtoPosition sv2(v2[0] - vepos[0], v2[1] - vepos[1], v2[2] - vepos[2]);

            float tc1[2] = {offset, offset};
            float tc2[2] = {offset, offset};
            if (it->mDirection > 0)
            {
               tc1[0] += 0.05 * dvec.length() / zoom;
               tc1[1] += 0.05 * dvec.length() / zoom;
            }
            else
            {
               tc2[0] += 0.05 * dvec.length() / zoom;
               tc2[1] += 0.05 * dvec.length() / zoom;
            }

            double len1 = sqrt(v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2]);
            double len2 = sqrt(v2[0] * v2[0] + v2[1] * v2[1] + v2[2] * v2[2]);
            double len3sq =
               (v2[0] - v1[0]) * (v2[0] - v1[0]) + (v2[1] - v1[1]) * (v2[1] - v1[1]) + (v2[2] - v1[2]) * (v2[2] - v1[2]);

            double r = UtEarth::cA;

            // what follows is a heuristically found approach to curve the lines when necessary based on two
            // measurements, the "horizontal-ness" of the line, and the altitude of the line's nearest point
            float maxarchFactor =
               0.1 * (r - sqrt(std::max(0.0, r * r - len3sq * 0.25))); // adds a parabola to the curvature to make sure
                                                                       // it clears the ground

            double twcs[3];
            double swcs[3];
            it->mTarget->GetLocationWCS(twcs);
            m_Owner->GetLocationWCS(swcs);
            osg::Vec3 tgt(twcs[0], twcs[1], twcs[2]);
            osg::Vec3 src(swcs[0], swcs[1], swcs[2]);
            osg::Vec3 dir     = tgt - src;
            double    n       = std::min(1.0f, std::max(0.0f, -(src * dir) / (dir * dir)));
            osg::Vec3 nearest = src + dir * n;
            float     nlen    = nearest.normalize();
            float     proximityFactor =
               1.0f -
               std::max(0.0f, std::min(1.0f, (nlen - (float)UtEarth::cA) / (20000.0f))); // is 1 at sea-level, 0 at 20km

            float dot = fabs(dir * nearest); // this is the dot product, per osg
                                             // it will represent the verticalness of the line
                                             // 0.5 is a 30 degree angle, sign doesn't matter
                                             // because direction is irrelevant
            float dotcurvefactor =
               std::max(0.0f, std::min(1.0f, 1.0f - (dot * 11.5f))); // how much curvature does the dot product think we want
                                                                     // 0 curvature at 5 degrees from horizontal or more
                                                                     // 1 curvature at horizontal (11.5 is 1 / cos(85))
            float totalFactor = std::min(dotcurvefactor, proximityFactor); // essentially, if either factor tells me
                                                                           // straight line is good, it is good

            for (int i = 0; i <= 20; i++)
            {
               float fi = i / 20.0;
               fi *= mFillFactor;
               UtoPosition v  = sv2 * fi + sv1 * (1 - fi) + epos;
               UtoPosition vc = v;
               vc.normalize();
               vc = vc * ((len2 * fi + len1 * (1 - fi)) + fi * fi * -maxarchFactor + fi * maxarchFactor);
               v  = vc * totalFactor + v * (1.0f - totalFactor) - epos;

               float tc[2] = {tc2[0] * fi + tc1[0] * (1 - fi),
                              tc2[1] * fi + tc1[1] * (1 - fi)}; // interpolate for the texture coordinate

               if ((i != 0) && (i != 20))
               {
                  Insert(p++, &v, &(it->mColor), tc, 1);
               }
               Insert(p++, &v, &(it->mColor), tc, 1);
            }
         }
         else
         {
            osg::Vec3 v1(vepos[0], vepos[1], vepos[2]);
            double    v2Array[3];
            it->mTarget->GetLocationWCS(v2Array);
            mMapProjection.ConvertFromECEF(v2Array);
            osg::Vec3 v2(v2Array[0], v2Array[1], v2Array[2]);

            osg::Vec3 dvec = v1 - v2;      // line direction
            osg::Vec3 evec = veye - vepos; // view direction

            osg::Vec3 ovec = dvec ^ evec; // perpendicular (offset direction)
            ovec.normalize();

            v1 += ovec * (it->mOffset + 0.5) * zoom * Width();
            v2 += ovec * (it->mOffset + 0.5) * zoom * Width();

            UtoPosition sv1(v1[0] - vepos[0], v1[1] - vepos[1], v1[2] - vepos[2]);
            UtoPosition sv2(v2[0] - vepos[0], v2[1] - vepos[1], v2[2] - vepos[2]);

            float tc1[2] = {offset, offset};
            float tc2[2] = {offset, offset};
            if (it->mDirection > 0)
            {
               tc1[0] += 0.05 * dvec.length() / zoom;
               tc1[1] += 0.05 * dvec.length() / zoom;
            }
            else
            {
               tc2[0] += 0.05 * dvec.length() / zoom;
               tc2[1] += 0.05 * dvec.length() / zoom;
            }

            Insert(p++, &sv1, &(it->mColor), tc1, 1);
            Insert(p++, &sv2, &(it->mColor), tc2, 1);
         }
      }

      return;
   }
   else
   {
      if (mArchSegments)
      {
         // Reset the matrix just in case it was an orthographic camera
         // last frame.
         UtoCameraOrtho* cam = info->m_Viewer->GetOrtho();
         double          epos[3];
         m_Owner->GetLocationWCS(epos);
         mMapProjection.ConvertFromECEF(epos);
         // vector entity position
         const osg::Vec3 vepos(epos[0], epos[1], epos[2]);

         double zoom = cam->Zoom();
         double vmat[4][4];
         cam->ViewMatrix(vmat);

         int index = m_TexCoord->size();
         // Call remove until all elements have been removed.
         while (Remove(--index) != -1)
         {
         }

         std::vector<Interaction>::iterator it;
         int                                p = 0;
         for (it = mInteractionList.begin(); it != mInteractionList.end(); ++it)
         {
            osg::Vec3 v1(vepos[0], vepos[1], vepos[2]);
            double    v2Array[3];
            it->mTarget->GetLocationWCS(v2Array);
            mMapProjection.ConvertFromECEF(v2Array);
            osg::Vec3 v2(v2Array[0], v2Array[1], v2Array[2]);

            osg::Vec3 dvec = v1 - v2;
            osg::Vec3 evec(vmat[0][2], vmat[1][2], vmat[2][2]);

            osg::Vec3 ovec = dvec ^ evec;
            ovec.normalize();

            v1 += ovec * (it->mOffset + 0.5) * zoom * Width();
            v2 += ovec * (it->mOffset + 0.5) * zoom * Width();

            UtoPosition sv1(v1[0] - vepos[0], v1[1] - vepos[1], v1[2] - vepos[2]);
            UtoPosition sv2(v2[0] - vepos[0], v2[1] - vepos[1], v2[2] - vepos[2]);

            float tc1[2] = {offset, offset};
            float tc2[2] = {offset, offset};
            if (it->mDirection > 0)
            {
               tc1[0] += 0.05 * dvec.length() / zoom;
               tc1[1] += 0.05 * dvec.length() / zoom;
            }
            else
            {
               tc2[0] += 0.05 * dvec.length() / zoom;
               tc2[1] += 0.05 * dvec.length() / zoom;
            }

            for (int i = 0; i <= 20; ++i)
            {
               float fi = i / 20.0;
               fi *= mFillFactor;
               UtoPosition v     = sv2 * fi + sv1 * (1 - fi);
               float       tc[2] = {tc2[0] * fi + tc1[0] * (1 - fi), tc2[1] * fi + tc1[1] * (1 - fi)};

               if ((i != 0) && (i != 20))
               {
                  Insert(p++, &v, &(it->mColor), tc, 1);
               }
               Insert(p++, &v, &(it->mColor), tc, 1);
            }
         }
      }
      else
      {
         float offset = info->m_Viewer->GetFrameNumber() * 0.033;
         // Reset the matrix just in case it was an orthographic camera
         // last frame.
         UtoCameraOrtho* cam = info->m_Viewer->GetOrtho();
         double          epos[3];
         m_Owner->GetLocationWCS(epos);
         mMapProjection.ConvertFromECEF(epos);
         // vector entity position
         const osg::Vec3 vepos(epos[0], epos[1], epos[2]);

         double zoom = cam->Zoom();
         double vmat[4][4];
         cam->ViewMatrix(vmat);

         int index = m_TexCoord->size();
         // Call remove until all elements have been removed.
         while (Remove(--index) != -1)
         {
         }

         std::vector<Interaction>::iterator it;
         int                                p = 0;
         for (it = mInteractionList.begin(); it != mInteractionList.end(); ++it)
         {
            osg::Vec3 v1(vepos[0], vepos[1], vepos[2]);
            double    v2Array[3];
            it->mTarget->GetLocationWCS(v2Array);
            mMapProjection.ConvertFromECEF(v2Array);
            osg::Vec3 v2(v2Array[0], v2Array[1], v2Array[2]);

            osg::Vec3 dvec = v1 - v2;
            osg::Vec3 evec(vmat[0][2], vmat[1][2], vmat[2][2]);

            osg::Vec3 ovec = dvec ^ evec;
            ovec.normalize();

            v1 += ovec * (it->mOffset + 0.5) * zoom * Width();
            v2 += ovec * (it->mOffset + 0.5) * zoom * Width();

            UtoPosition sv1(v1[0] - vepos[0], v1[1] - vepos[1], v1[2] - vepos[2]);
            UtoPosition sv2(v2[0] - vepos[0], v2[1] - vepos[1], v2[2] - vepos[2]);

            float tc1[2] = {offset, offset};
            float tc2[2] = {offset, offset};
            if (it->mDirection > 0)
            {
               tc1[0] += 0.05 * dvec.length() / zoom;
               tc1[1] += 0.05 * dvec.length() / zoom;
            }
            else
            {
               tc2[0] += 0.05 * dvec.length() / zoom;
               tc2[1] += 0.05 * dvec.length() / zoom;
            }

            Insert(p++, &sv1, &(it->mColor), tc1, 1);
            Insert(p++, &sv2, &(it->mColor), tc2, 1);
         }
      }
   }
}

int UtoInteractionShapeImp::AddInboundSegment(UtoEntity* aSrc, int aOffset, const UtoColor& aColor)
{
   aSrc->connect(UtoEntity::DestroyedEvent(), this, &UtoInteractionShapeImp::OnRemoveEntityCB);
   mInteractionList.emplace_back(aSrc, -1, aOffset, aColor);

   return mInteractionList.size();
}

int UtoInteractionShapeImp::AddOutboundSegment(UtoEntity* aSrc, int aOffset, const UtoColor& aColor)
{
   aSrc->connect(UtoEntity::DestroyedEvent(), this, &UtoInteractionShapeImp::OnRemoveEntityCB);
   mInteractionList.emplace_back(aSrc, 1, aOffset, aColor);

   return mInteractionList.size();
}

void UtoInteractionShapeImp::OnRemoveEntityCB(UtoEntity* entity)
{
   auto it = std::find_if(std::begin(mInteractionList),
                          std::end(mInteractionList),
                          [entity](const Interaction& i) { return i.mTarget == entity; });
   if (it != std::end(mInteractionList))
   {
      mInteractionList.erase(it);
   }
}

void UtoInteractionShapeImp::ArchSegments(bool aState)
{
   mArchSegments = aState;
}

void UtoInteractionShapeImp::SetSceneProjection(const UtoMapProjection& aMapProjection)
{
   mMapProjection = aMapProjection;
}
