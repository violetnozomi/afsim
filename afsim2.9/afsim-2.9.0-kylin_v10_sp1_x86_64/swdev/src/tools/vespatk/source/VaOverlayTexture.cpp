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

#include "VaOverlayTexture.hpp"

#include <functional>

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osgDB/ReadFile>

#include "Uto.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "VaCamera.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaViewerManager.hpp"
#include "VaViewerSubordinate.hpp"

namespace vespa
{
VaOverlayTexture::VaOverlayTexture()
   : VaOverlay("texture_overlay")
   , mShape(nullptr)
   , mJustification(0)
   , mOwnedViewerPtr(nullptr)
   , mWheelFunction(nullptr)
   , mHandleEventMode(vespa::VaOverlay::cHANDLE_EVENTS_NEVER)
{
   SetType<VaOverlayTexture>();
}

VaOverlayTexture::VaOverlayTexture(const std::string& aName)
   : VaOverlay(aName)
   , mShape(nullptr)
   , mJustification(0)
   , mOwnedViewerPtr(nullptr)
   , mWheelFunction(nullptr)
   , mHandleEventMode(vespa::VaOverlay::cHANDLE_EVENTS_NEVER)
{
   SetType<VaOverlayTexture>();
}

VaOverlayTexture::VaOverlayTexture(const VaOverlayTexture& aSrc)
   : VaOverlay(aSrc)
   , mShape(nullptr)
   , mJustification(aSrc.mJustification)
   , mOwnedViewerPtr(nullptr)
   , mWheelFunction(nullptr)
   , mHandleEventMode(vespa::VaOverlay::cHANDLE_EVENTS_NEVER)
{
   SetType<VaOverlayTexture>();
}

VaOverlayTexture::~VaOverlayTexture()
{
   if (mOwnedViewerPtr)
   {
      vespa::VaEnvironment::Instance().GetViewerManager()->RemoveViewer(mOwnedViewerPtr);
      // remove the subordinate
      delete mOwnedViewerPtr;
      mOwnedViewerPtr = nullptr;
   }
   if (mShape)
   {
      UnbindShape("quad");
   }
}

VaOverlayTexture* VaOverlayTexture::Clone() const
{
   return new VaOverlayTexture(*this);
}

bool VaOverlayTexture::Initialize()
{
   mCallbacks.Add(VaObserver::ViewerResized.Connect(&VaOverlayTexture::ViewerResizedCB, this, GetViewer()->GetUniqueId()));

   UtoRawShape shape;
   mShape    = dynamic_cast<UtoRawShape*>(BindShape("overlayTexture", shape));
   auto root = mShape->GetRoot();

   auto geom = osg::createTexturedQuadGeometry(osg::Vec3{0.0f, 0.0f, 0.0f},
                                               osg::Vec3{1.0f, 0.0f, 0.0f},
                                               osg::Vec3{0.0f, 1.0f, 0.0f},
                                               0.0f,
                                               0.0f,
                                               1.0f,
                                               1.0f);
   geom->setCullingActive(false);
   auto quad = new osg::Geode{};
   quad->addDrawable(geom);

   auto off = osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE;

   auto stateSet = quad->getOrCreateStateSet();
   stateSet->setMode(GL_LIGHTING, off);

   // stateSet->setAttribute(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL), off);
   stateSet->setAttribute(new osg::Depth(osg::Depth::ALWAYS, 0.0, 1.0, false));

   mMatrixNode = new osg::MatrixTransform;
   mMatrixNode->setMatrix(osg::Matrix::scale(osg::Vec3(mWidth, mHeight, 1)) * osg::Matrix::translate(osg::Vec3(mX, mY, 0)));
   mMatrixNode->addChild(quad);

   root->addChild(mMatrixNode.get());
   return true;
}

void VaOverlayTexture::EnableTransparency(bool aState)
{
   if (mMatrixNode)
   {
      if (aState)
      {
         osg::StateSet* ss = mMatrixNode->getOrCreateStateSet();
         ss->setAttributeAndModes(new osg::BlendFunc());
      }
      else
      {
         osg::StateSet* ss = mMatrixNode->getOrCreateStateSet();
         ss->removeAttribute(osg::StateAttribute::BLENDFUNC);
      }
   }
}

void VaOverlayTexture::SetPosition(float aX, float aY)
{
   mX = aX;
   mY = aY;

   UpdateMatrix();
}

void VaOverlayTexture::SetJustification(bool aLeft, bool aBottom)
{
   mJustification = 0;
   if (!aLeft)
   {
      mJustification |= cRIGHT;
   }
   if (!aBottom)
   {
      mJustification |= cTOP;
   }
   UpdateMatrix();
}

void VaOverlayTexture::SetSize(float aW, float aH)
{
   mWidth  = aW;
   mHeight = aH;

   UpdateMatrix();
}

void VaOverlayTexture::LoadImageFile(const char* aFilename)
{
   // note, we are not caching this off if we don't have a node
   if (mMatrixNode)
   {
      osg::StateSet* ss = mMatrixNode->getOrCreateStateSet();

      osg::Image*   image   = osgDB::readImageFile(aFilename);
      osg::Texture* texture = new osg::Texture2D(image);
      ss->setTextureAttributeAndModes(0, texture);
   }
}

void VaOverlayTexture::LoadImageBits(unsigned int aW, unsigned int aH, const unsigned char* aBits)
{
   // note, we are not caching this off if we don't have a node
   if (mMatrixNode)
   {
      osg::StateSet* ss = mMatrixNode->getOrCreateStateSet();

      osg::ref_ptr<osg::Image> image   = new osg::Image();
      unsigned int             imgsize = aW * aH * 4;
      unsigned char*           data    = new unsigned char[imgsize];
      memcpy(data, aBits, imgsize);
      image->setImage(aW, aH, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data, osg::Image::USE_NEW_DELETE);
      osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
      texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
      texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
      ss->setTextureAttributeAndModes(0, texture);
   }
}

void VaOverlayTexture::LoadSubTextureFromImageFile(const char* aFilename, float afX, float afY, float afW, float afH)
{
   // note, we are not caching this off if we don't have a node
   if (mMatrixNode)
   {
      osg::StateSet*           ss           = mMatrixNode->getOrCreateStateSet();
      osg::ref_ptr<osg::Image> image        = osgDB::readRefImageFile(aFilename);
      int                      iX           = image->s() * afX;
      int                      iY           = image->t() * afY;
      int                      iW           = image->s() * afW;
      int                      iH           = image->t() * afH;
      int                      pxlSize      = image->getPixelSizeInBits() / 8 * iW;
      unsigned char*           subImageData = new unsigned char[iH * pxlSize];
      for (int j = 0; j < iH; ++j)
      {
         memcpy(&subImageData[j * pxlSize], image->data(iX, j + iY), pxlSize);
      }
      osg::Image* subImage = new osg::Image();
      subImage->setImage(iW,
                         iH,
                         1,
                         image->getInternalTextureFormat(),
                         image->getPixelFormat(),
                         image->getDataType(),
                         subImageData,
                         osg::Image::USE_NEW_DELETE);
      image                 = nullptr;
      osg::Texture* texture = new osg::Texture2D(subImage);
      ss->setTextureAttributeAndModes(0, texture);
   }
}

VaViewer* VaOverlayTexture::CreateViewerTexture(VaViewer* aParentPtr, unsigned int aResX, unsigned int aResY)
{
   // note, we are not caching this off if we don't have a node
   if (mMatrixNode)
   {
      osg::StateSet* ss = mMatrixNode->getOrCreateStateSet();

      mOwnedViewerPtr = new vespa::VaViewerSubordinate(aParentPtr, aResX, aResY, vespa::VaViewer::SmallFeatureCulling);
      vespa::VaEnvironment::Instance().GetViewerManager()->AddViewer(mOwnedViewerPtr);
      mOwnedViewerPtr->GetCamera()->UseOrthographicProjection();
      mOwnedViewerPtr->Initialize();

      osg::ref_ptr<osg::Texture> texture = mOwnedViewerPtr->GetTexture();
      texture->setBorderColor(osg::Vec4(0, 0, 0, 1));
      texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
      texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
      if (texture != nullptr)
      {
         ss->setTextureAttributeAndModes(0, texture);
      }
      return mOwnedViewerPtr;
   }
   return nullptr;
}

vespa::VaViewer* VaOverlayTexture::GetOwnedViewer()
{
   return mOwnedViewerPtr;
}

void VaOverlayTexture::RecalculateOrthographic(unsigned int aResX, unsigned int aResY, double aZoom)
{
   float x = aResX * aZoom;
   float y = aResY * aZoom;
   mOwnedViewerPtr->GetViewer()->SetProjectionMatrixAsOrtho(-x, x, -y, y, 0, 10000000);
}

void VaOverlayTexture::SetShader(const char* aVertexShaderName, const char* aFragmentShaderName)
{
   if (mMatrixNode)
   {
      osg::StateSet* ss   = mMatrixNode->getOrCreateStateSet();
      std::string    vert = std::string(aVertexShaderName) + ".vert";
      std::string    frag = std::string(aFragmentShaderName) + ".frag";
      ss->setAttributeAndModes(UtoShaders::UseProgram(vert, frag));
   }
}

void VaOverlayTexture::SetUniform(const char* aName, float aValue)
{
   if (mMatrixNode)
   {
      osg::StateSet* ss = mMatrixNode->getOrCreateStateSet();
      ss->getOrCreateUniform(aName, osg::Uniform::FLOAT)->set(aValue);
   }
}

void VaOverlayTexture::SetUniform4f(const char* aName, float aX, float aY, float aZ, float aW)
{
   if (mMatrixNode)
   {
      osg::StateSet* ss = mMatrixNode->getOrCreateStateSet();
      ss->getOrCreateUniform(aName, osg::Uniform::FLOAT_VEC4)->set(osg::Vec4(aX, aY, aZ, aW));
   }
}

void VaOverlayTexture::UpdateMatrix()
{
   if (mMatrixNode)
   {
      float x = mX;
      float y = mY;
      if (mJustification & cRIGHT)
      {
         x = GetViewer()->GetWidth() - x;
      }
      if (mJustification & cTOP)
      {
         y = GetViewer()->GetHeight() - y;
      }
      mMatrixNode->setMatrix(osg::Matrix::scale(osg::Vec3(mWidth, mHeight, 1)) *
                             osg::Matrix::translate(osg::Vec3(x, y, 0)));
   }
}


void VaOverlayTexture::ViewerResizedCB(VaViewer* aViewerPtr)
{
   UpdateMatrix();
}


void VaOverlayTexture::SetButton1ClickAction(std::function<bool(int, int, int)> aFunction)
{
   mHandleEventMode = vespa::VaOverlay::cHANDLE_EVENTS_FOCUS;
   mBtn1Function    = aFunction;
}

void VaOverlayTexture::SetMouseWheelAction(std::function<bool(int, int, int)> aFunction)
{
   mWheelFunction = aFunction;
}

bool VaOverlayTexture::Btn1Click(int aMouseX, int aMouseY, int aState)
{
   if (mHandleEventMode != vespa::VaOverlay::cHANDLE_EVENTS_NEVER)
   {
      return mBtn1Function(aMouseX, aMouseY, aState);
   }
   else
   {
      return false;
   }
}

bool VaOverlayTexture::MouseWheel(int aMouseX, int aMouseY, int aDelta)
{
   if (mWheelFunction)
   {
      return mWheelFunction(aMouseX, aMouseY, aDelta);
   }
   return false;
}

bool VaOverlayTexture::SelectableAt(int aMouseX, int aMouseY) const
{
   if ((aMouseX > mX) && (aMouseX < mX + mWidth) && (aMouseY > mY) && (aMouseY < mY + mHeight))
   {
      return true;
   }
   return false;
}

void VaOverlayTexture::AdjustTextureCoordinatesToAspectRatio(double aRatio)
{
   double activeRatio = (float)mWidth / (float)mHeight;
   if (activeRatio > aRatio)
   {
      osg::Geode* g = dynamic_cast<osg::Geode*>(mMatrixNode->getChild(0));
      if (g)
      {
         g->removeDrawables(0);
         float halfOffset = 0.5 * (activeRatio / aRatio - 1.0);
         auto  geom       = osg::createTexturedQuadGeometry(osg::Vec3{0.0f, 0.0f, 0.0f},
                                                     osg::Vec3{1.0f, 0.0f, 0.0f},
                                                     osg::Vec3{0.0f, 1.0f, 0.0f},
                                                     -halfOffset,
                                                     0.0f,
                                                     1 + halfOffset,
                                                     1.0f);
         geom->setCullingActive(false);
         g->addDrawable(geom);
      }
      // it is wider than we want, window box it
   }
   else
   {
      osg::Geode* g = dynamic_cast<osg::Geode*>(mMatrixNode->getChild(0));
      if (g)
      {
         g->removeDrawables(0);
         float halfOffset = 0.5 * (aRatio / activeRatio - 1.0);
         auto  geom       = osg::createTexturedQuadGeometry(osg::Vec3{0.0f, 0.0f, 0.0f},
                                                     osg::Vec3{1.0f, 0.0f, 0.0f},
                                                     osg::Vec3{0.0f, 1.0f, 0.0f},
                                                     0.0f,
                                                     -halfOffset,
                                                     1.0f,
                                                     1.0f + halfOffset);
         geom->setCullingActive(false);
         g->addDrawable(geom);
      }
      // it is taller than we want, letter box it
   }
}

} // namespace vespa
