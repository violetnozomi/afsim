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

// UtoTexturedPolygonImp.cpp: implementation of the UtoTexturedPolygonImp class.
//
//////////////////////////////////////////////////////////////////////

#include "UtoTexturedPolygonImp.hpp"

#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include "UtoObjectManager.hpp"
#include "UtoPriv.hpp"

osg::Geometry* UtoTexturedPolygonImp::CreateModel(const std::string& TextureFile,
                                                  bool&              o_TranslucentImage,
                                                  bool               AntiAliased,
                                                  bool               MipMapping,
                                                  WrapType           wt)
{
   return CreateModel(TextureFile, o_TranslucentImage, 0, 0, AntiAliased, MipMapping, wt);
}

osg::ref_ptr<osg::Vec2Array>  UtoTexturedPolygonImp::s_TexCoord  = nullptr;
osg::ref_ptr<osg::Vec3Array>  UtoTexturedPolygonImp::s_Normal    = nullptr;
osg::ref_ptr<osg::Vec4Array>  UtoTexturedPolygonImp::s_Color     = nullptr;
osg::ref_ptr<osg::DrawArrays> UtoTexturedPolygonImp::s_Primitive = nullptr;

osg::Texture2D* GetOrCreateTexture2D(const std::string& rName);

osg::Geometry* UtoTexturedPolygonImp::CreateModel(const std::string& TextureFile,
                                                  bool&              o_TranslucentImage,
                                                  double             Width,
                                                  double             Height,
                                                  bool               AntiAliased,
                                                  bool               MipMapping,
                                                  WrapType           wt)
{
   // obtain the 2d texture
   osg::Texture2D* pTexture2D = GetOrCreateTexture2D(TextureFile);
   if (!pTexture2D)
      return nullptr;
   // read in the image
   osg::Image* image = pTexture2D->getImage();
   if (!image)
      return nullptr;

   // set the return translucent flag
   o_TranslucentImage = image->isImageTranslucent();

   // create Geometry object to store all the vetices and lines primitives.
   osg::Geometry* polyGeom = new osg::Geometry();

   // use the width and height from the loaded image if the passed values
   // are zero
   if (!Width && !Height)
   {
      Width  = image->s();
      Height = image->t();
   }

   // note, counterclockwsie ordering.
   osg::Vec3 myCoords[] = {osg::Vec3(-Width / 2.0, Height / 2.0, 0.0f),
                           osg::Vec3(-Width / 2.0, -Height / 2.0, 0.0f),
                           osg::Vec3(Width / 2.0, -Height / 2.0, 0.0f),
                           osg::Vec3(Width / 2.0, Height / 2.0, 0.0f)};

   int numCoords = sizeof(myCoords) / sizeof(osg::Vec3);

   // pass the created vertex array to the points geometry object.
   polyGeom->setVertexArray(new osg::Vec3Array(numCoords, myCoords));

   osg::Vec4Array* clr = new osg::Vec4Array;
   clr->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
   polyGeom->setColorArray(clr);
   polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

   // set the normal in the same way color.
   if (!s_Normal.get())
   {
      s_Normal = new osg::Vec3Array;
      s_Normal->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
   }
   polyGeom->setNormalArray(s_Normal.get());
   polyGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

   if (!s_TexCoord)
   {
      osg::Vec2 myTexCoords[] = {osg::Vec2(0, 1), osg::Vec2(0, 0), osg::Vec2(1, 0), osg::Vec2(1, 1)};

      int numTexCoords = sizeof(myTexCoords) / sizeof(osg::Vec2);
      s_TexCoord       = new osg::Vec2Array(numTexCoords, myTexCoords);
   }

   // pass the created tex coord array to the points geometry object,
   // and use it to set texture unit 0.
   polyGeom->setTexCoordArray(0, s_TexCoord.get());

   if (!s_Primitive.get())
   {
      s_Primitive = new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4);
   }
   polyGeom->addPrimitiveSet(s_Primitive.get());

   // new we need to add the texture to the Drawable, we do so by creating
   // a StateSet to contain the Texture2D StateAttribute.
   osg::StateSet* stateset = new osg::StateSet;

   // DEBUG(("Skipping AntiAliased && MipMapping settings in "
   //    "Shape/TexturedPolygonImp.cpp\n"));

   switch (wt)
   {
   case WRAP_REPEAT:
      pTexture2D->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
      pTexture2D->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
      break;
   case WRAP_CLAMP:
      pTexture2D->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
      pTexture2D->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
   }

   stateset->setTextureAttributeAndModes(0, pTexture2D, osg::StateAttribute::ON);

   polyGeom->setStateSet(stateset);
   return polyGeom;
}

// creates the geometry for a textured subimage
osg::Geometry* UtoTexturedPolygonImp::CreateSubImage(const std::string& rTexture,
                                                     unsigned short     nX0,
                                                     unsigned short     nY0,
                                                     unsigned short     nX1,
                                                     unsigned short     nY1,
                                                     double             dWidth,
                                                     double             dHeight,
                                                     bool               bInvertY,
                                                     bool&              rTransImage)
{
   // create a local geometry drawable
   osg::Geometry* pGeometry = nullptr;

   // validate that there is a texture present
   // validate that the rect has been specified
   if (!rTexture.empty() && (nX0 || nX1 || nY0 || nY1))
   {
      // get or create the texture 2d implemenation
      osg::Texture2D* pTexture2D = GetOrCreateTexture2D(rTexture);
      if (!pTexture2D)
         return nullptr;

      // obtain the texture image
      osg::Image* pTexImage = pTexture2D->getImage();
      if (!pTexImage)
         return nullptr;

      // create the new geometry object
      pGeometry = new osg::Geometry;

      // determine if the image is transparent
      rTransImage = pTexImage->isImageTranslucent();

      // determine if the y needs to be inverted
      if (bInvertY)
      {
         nY0 = pTexImage->t() - nY0;
         nY1 = pTexImage->t() - nY1;
      }

      // if the width and height are zero, use the width and height
      // from the passed in texture coordinates
      if (!dWidth && !dHeight)
      {
         dWidth  = nX1 - nX0;
         dHeight = nY1 - nY0;
      }

      // create a new vertex array
      osg::Vec3Array* pVertices = new osg::Vec3Array;
      // push back all the vertices to satisfy the width and height
      pVertices->push_back(osg::Vec3(-dWidth / 2.0, dHeight / 2.0, 0.0f));
      pVertices->push_back(osg::Vec3(-dWidth / 2.0, -dHeight / 2.0, 0.0f));
      pVertices->push_back(osg::Vec3(dWidth / 2.0, -dHeight / 2.0, 0.0f));
      pVertices->push_back(osg::Vec3(dWidth / 2.0, dHeight / 2.0, 0.0f));

      // send the verts down to the card
      pGeometry->setVertexArray(pVertices);

      // create a new color value
      osg::Vec4Array* pClrIndex = new osg::Vec4Array;
      // push back the first color
      pClrIndex->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

      // send the color index values down and bind to each vert
      pGeometry->setColorArray(pClrIndex);
      pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

      // set the normal for the geometry
      if (!s_Normal.get())
      {
         // create the new vector for the normals
         s_Normal = new osg::Vec3Array;
         // push back the normal for the geometry
         s_Normal->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));
      }

      // attach the normal to the geometry and bind over all the verts
      pGeometry->setNormalArray(s_Normal.get());
      pGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

      // create the texture coordinates for the array
      osg::Vec2Array* pTexCoords = new osg::Vec2Array;

      // determine the image coordinates [0, 1] - wrapping not allowed
      float fX0 = (float)nX0 / (float)pTexImage->s();
      float fY0 = (float)nY0 / (float)pTexImage->t();
      float fX1 = (float)nX1 / (float)pTexImage->s();
      float fY1 = (float)nY1 / (float)pTexImage->t();

      // push back the new image coordinates
      pTexCoords->push_back(osg::Vec2(fX0, fY1));
      pTexCoords->push_back(osg::Vec2(fX0, fY0));
      pTexCoords->push_back(osg::Vec2(fX1, fY0));
      pTexCoords->push_back(osg::Vec2(fX1, fY1));

      // send the tex coords down to the card
      pGeometry->setTexCoordArray(0, pTexCoords);

      // create a primitive to draw the textured quad
      if (!s_Primitive.get())
      {
         s_Primitive = new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4);
      }

      // add the rendering primitive to the geometry
      pGeometry->addPrimitiveSet(s_Primitive.get());

      // attach the texture to the geometry
      pGeometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, pTexture2D);
   }

   return pGeometry;
}

// class to manage the creation of 2D textures
class TexManager2D
{
public:
   // access operator
   static TexManager2D& Instance()
   {
      if (!m_pThis)
         m_pThis = new TexManager2D;
      return *m_pThis;
   }

   // get or create the new texture
   osg::Texture2D* GetOrCreateTexture2D(const std::string& rFile)
   {
      // lock down this function
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_oMutex);

      // find the texture object within the map
      TextureMap::iterator itTex = g_oTextureMap.find(rFile);

      if (itTex == g_oTextureMap.end())
      {
         // define the image reading options to cache the images
         osg::ref_ptr<osgDB::ReaderWriter::Options> pOptions = new osgDB::ReaderWriter::Options;
         pOptions->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_IMAGES);

         // read in the image from the file
         osg::Image* pImage = osgDB::readImageFile(rFile.c_str(), pOptions.get());
         if (!pImage)
            return nullptr;

         // create a new texture object 2D
         itTex = g_oTextureMap.insert(TextureMap::value_type(rFile, new osg::Texture2D)).first;

         // bind the image to the texture
         itTex->second->setImage(pImage);
      }

      // increase the texture reference count so not to erase it in the next step
      osg::ref_ptr<osg::Texture2D> pTex2D = itTex->second.get();

      // search for released textures.  a released texture
      // will be a texture that has only 1 reference associated
      // to it: the texture manager map.  release the texture from memory

      // what would be nice would be to have some way to compare the image
      // to a texture so as to not have to delete and recreate a texture with
      // the same kind of formats.
      for (TextureMap::iterator itTexBeg = g_oTextureMap.begin(), itTexEnd = g_oTextureMap.end(); itTexBeg != itTexEnd;)
      {
         if (itTexBeg->second->referenceCount() == 1)
         {
            // remove the iterator from the managed map
            g_oTextureMap.erase(itTexBeg++);
         }
         else
         {
            ++itTexBeg;
         }
      }

      return pTex2D.get();
   }

private:
   // private typedefs
   typedef std::map<std::string, osg::ref_ptr<osg::Texture2D>> TextureMap;

   // private constructors
   TexManager2D() { UtoObjectManager::Instance()->Register("TexManager2D", this); }
   TexManager2D(const TexManager2D& rMgr);

   // private copy operator
   const TexManager2D& operator=(const TexManager2D& rMgr);

   // private member data
   TextureMap         g_oTextureMap;
   OpenThreads::Mutex m_oMutex;

   // private static member data
   static TexManager2D* m_pThis;
};

// init the static tex manager pointer
TexManager2D* TexManager2D::m_pThis = nullptr;

// create a texture 2d object that will automatically have the image bound to it
osg::Texture2D* GetOrCreateTexture2D(const std::string& rFile)
{
   return TexManager2D::Instance().GetOrCreateTexture2D(rFile);
}
