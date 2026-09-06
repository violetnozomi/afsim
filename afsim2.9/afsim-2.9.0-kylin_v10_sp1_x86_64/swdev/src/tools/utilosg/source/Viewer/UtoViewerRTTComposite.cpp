// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoViewerRTTComposite.cpp : implementation file

#include "UtoViewerRTTComposite.hpp"

#include <algorithm>
#include <assert.h>
#include <iostream>

#include "UtoShaders.hpp"
//#include "UtoSymbol.hpp"
#include <osg/BlendFunc>
#include <osg/ColorMask>
#include <osg/Depth>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Vec3>
#include <osgDB/ReadFile>
#include <osgUtil/SceneView>
#include <osgUtil/UpdateVisitor>
#include <osgViewer/Viewer>

#include "UtoEntity.hpp"
#include "UtoEntityDB.hpp"
#include "UtoPriv.hpp"
#include "UtoRenderInfo.hpp"
#include "UtoShape.hpp"
#include "UtoShapeFrame.hpp"
#include "UtoShapeImp.hpp"
#include "UtoViewFilter.hpp"

#ifdef _WIN32
#pragma warning(disable : 4355)
#endif

/* UTO Function Block **********************************************************

Function: UtoViewerImp

Description:
Constructor

*******************************************************************************/

UtoViewerRTTComposite::UtoViewerRTTComposite(const std::string& name, unsigned int aResolutionX, unsigned int aResolutionY)
   : UtoViewer(nullptr, name)
   , mResolutionX(aResolutionX)
   , mResolutionY(aResolutionY)
{
   Setup();
}

void UtoViewerRTTComposite::Setup()
{
   mRawTexture = new osg::Texture2D;
   mRawTexture->setName("JEM_renderTexture");
   mRawTexture->setTextureSize(mResolutionX, mResolutionY);
   mRawTexture->setInternalFormat(GL_RGBA);
   mRawTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
   mRawTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);

   //////////////////////
   // This is the camera that looks at the scene and renders to mRawTexture
   //////////////////////
   mTextureCamera = new osg::Camera;
   mTextureCamera->setName("toTextureCamera");
   mTextureCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   mTextureCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   mTextureCamera->setClearColor(osg::Vec4(0, 0, 0, 1));
   mTextureCamera->setViewport(0, 0, mResolutionX, mResolutionY);

   mTextureCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
   mTextureCamera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

   //   mTextureCamera->setRenderOrder(osg::Camera::PRE_RENDER); // this fails on new VaWidget
   mTextureCamera->attach(osg::Camera::COLOR_BUFFER, mRawTexture.get());
   // mTextureCamera->attach(osg::Camera::DEPTH_BUFFER, mDepthTexture.get());
   mTextureCamera->addChild(GetWorld()->GetTerrainTree());
   mTextureCamera->addChild(GetWorld()->GetOverlayTree());

   // add the window overlays to the scene
   mTextureCamera->addChild(GetScreenOverlayManager()->GetTree());

   mTextureCamera->addChild(mScene.get());
}

/* UTO Function Block **********************************************************

Function: UtoViewerImp

Description:
Destructor

*******************************************************************************/

UtoViewerRTTComposite::~UtoViewerRTTComposite() {}

bool UtoViewerRTTComposite::Connect(UtoEntityDB& edb)
{
   bool retval = UtoViewer::Connect(edb);

   return retval;
}

//////////////////////////

const osg::Viewport* UtoViewerRTTComposite::GetViewport() const
{
   return mTextureCamera->getViewport();
}

void UtoViewerRTTComposite::SetViewport(int aX, int aY, int aW, int aH)
{
   mTextureCamera->setViewport(aX, aY, aW, aH);
   SetUniform("UTO_Viewport", aX, aY, aW, aH);
}

void UtoViewerRTTComposite::SetViewMatrix(const osg::Matrix& aMatrix)
{
   mTextureCamera->setViewMatrix(aMatrix);
}

void UtoViewerRTTComposite::SetViewMatrixAsLookAt(const osg::Vec3& aEye, const osg::Vec3& aCenter, const osg::Vec3& aUp)
{
   mTextureCamera->setViewMatrixAsLookAt(aEye, aCenter, aUp);
}

void UtoViewerRTTComposite::SetProjectionMatrixAsPerspective(float aFOV, float aAspectRatio, float aHither, float aYon)
{
   mTextureCamera->setProjectionMatrixAsPerspective(aFOV, aAspectRatio, aHither, aYon);
}

void UtoViewerRTTComposite::SetProjectionMatrixAsOrtho(float aMinX, float aMaxX, float aMinY, float aMaxY, float aMinZ, float aMaxZ)
{
   mTextureCamera->setProjectionMatrixAsOrtho(aMinX, aMaxX, aMinY, aMaxY, aMinZ, aMaxZ);
}

void UtoViewerRTTComposite::SetProjectionMatrixAsFrustum(double aLeft,
                                                         double aRight,
                                                         double aBottom,
                                                         double aTop,
                                                         double aNear,
                                                         double aFar)
{
   mTextureCamera->setProjectionMatrixAsFrustum(aLeft, aRight, aBottom, aTop, aNear, aFar);
}


const osg::Matrix* UtoViewerRTTComposite::GetProjectionMatrix() const
{
   if (nullptr != mTextureCamera)
   {
      return &mTextureCamera->getProjectionMatrix();
   }
   else
   {
      return nullptr;
   }
}

void UtoViewerRTTComposite::SetUniform(const char* aName, const float aValue)
{
   if (nullptr != mTextureCamera)
   {
      osg::StateSet* ss = mTextureCamera->getOrCreateStateSet();
      ss->getOrCreateUniform(aName, osg::Uniform::FLOAT)->set((float)aValue);
   }
}

void UtoViewerRTTComposite::SetUniform(const char* aName, const float aValue1, const float aValue2, const float aValue3)
{
   if (nullptr != mTextureCamera)
   {
      osg::StateSet* ss = mTextureCamera->getOrCreateStateSet();
      ss->getOrCreateUniform(aName, osg::Uniform::FLOAT_VEC3)->set(osg::Vec3(aValue1, aValue2, aValue3));
   }
}

void UtoViewerRTTComposite::SetUniform(const char* aName,
                                       const float aValue1,
                                       const float aValue2,
                                       const float aValue3,
                                       const float aValue4)
{
   if (nullptr != mTextureCamera)
   {
      osg::StateSet* ss = mTextureCamera->getOrCreateStateSet();
      ss->getOrCreateUniform(aName, osg::Uniform::FLOAT_VEC4)->set(osg::Vec4(aValue1, aValue2, aValue3, aValue4));
   }
}

void UtoViewerRTTComposite::SetUniform(const char* aName, const unsigned int aIndex, const char* aFilename)
{
   if (nullptr != mTextureCamera)
   {
      osg::StateSet* ss = mTextureCamera->getOrCreateStateSet();


      osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
      osg::ref_ptr<osg::Image>     image   = osgDB::readImageFile(aFilename);
      if (image.valid())
      {
         texture->setImage(image.get());
         texture->setInternalFormat(GL_RGBA);
         texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
         texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

         texture->setWrap(osg::Texture2D::WRAP_R, osg::Texture2D::CLAMP_TO_EDGE);
         texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
         texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);

         ss->setTextureAttribute(aIndex, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
         //         ss->setTextureAttributeAndModes(aIndex, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
         ss->getOrCreateUniform(aName, osg::Uniform::SAMPLER_2D)->set((int)aIndex);
      }
   }
}

void UtoViewerRTTComposite::SetUniform(const char*          aName,
                                       const unsigned int   aIndex,
                                       const unsigned char* aData,
                                       const unsigned int   aWidth,
                                       const unsigned int   aHeight)
{
   if (nullptr != mTextureCamera)
   {
      osg::StateSet* ss = mTextureCamera->getOrCreateStateSet();

      unsigned char* datamem = new unsigned char[aWidth * aHeight * 3];
      memcpy(datamem, aData, aWidth * aHeight * 3);
      osg::ref_ptr<osg::Image> hmimage = new osg::Image();
      hmimage->setImage(aWidth, aHeight, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, datamem, osg::Image::USE_NEW_DELETE);
      osg::ref_ptr<osg::Texture2D> hmtexture = new osg::Texture2D();
      hmtexture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
      hmtexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
      hmtexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
      hmtexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
      hmtexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
      hmtexture->setImage(hmimage.get());

      ss->setTextureAttribute(aIndex, hmtexture.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
      ss->getOrCreateUniform(aName, osg::Uniform::SAMPLER_2D)->set((int)aIndex);
   }
}

void UtoViewerRTTComposite::SetCullingMode(int aMode)
{
   mTextureCamera->setCullingMode(aMode);
}

void UtoViewerRTTComposite::SetClearColor(const osg::Vec4& aInput)
{
   mTextureCamera->setClearColor(aInput);
}

const osg::Vec4& UtoViewerRTTComposite::GetClearColor()
{
   return mTextureCamera->getClearColor();
}
