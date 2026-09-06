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

#include "VaAttachmentTexture.hpp"

#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "VaUtils.hpp"
#include "VaViewerManager.hpp"
#include "VaViewerSubordinate.hpp"

namespace vespa
{
unsigned int VaAttachmentTexture::sId = 0;

//! Method: VaAttachmentTexture
//! Description: VaAttachmentTexture constructor.  VaAttachmentTexture is an attachment
//!              that will display a textured quad when attached to a VaEntity.
//!
//! @param aParent - This parameter is the parent entity. This should always be a valid entity.

// public

VaAttachmentTexture::VaAttachmentTexture(VaEntity& aParent, VaViewer* aViewerPtr)
   : VaAttachment(aParent, aViewerPtr, "textureAttachment")
   , mShapePtr(nullptr)
   , mShapeName()
{
   SetName(GetName() + "-" + VaUtils::ToString<unsigned int>(++sId));
   SetType<VaAttachmentTexture>();
}

//! Method: ~VaAttachmentTexture
//! Description: VaAttachmentTexture destructor.
//!

// public
VaAttachmentTexture::~VaAttachmentTexture()
{
   mCallbacks.Clear();
   if (mViewerPtr)
   {
      vespa::VaEnvironment::Instance().GetViewerManager()->RemoveViewer(mViewerPtr);
      // remove the subordinate
      delete mViewerPtr;
      mViewerPtr = nullptr;
   }
}

void VaAttachmentTexture::PrivateLoad()
{
   if (mShapeName == "")
   {
      static unsigned int uid = 0;
      mShapeName              = "VaAttachmentTexture" + VaUtils::ToString<unsigned int>(++uid);
   }

   // Remove the previous model.
   RemoveShapeFromParent(mShapeName);
   mShapePtr = nullptr;

   // Load the shape.
   UtoRawShape shape;
   mShapePtr = dynamic_cast<UtoRawShape*>(AddShapeToParent(mShapeName, shape));
   mShapePtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY);

   auto root = mShapePtr->GetRoot();

   auto geom = osg::createTexturedQuadGeometry(osg::Vec3{0.0f, 0.0f, 0.0f},
                                               osg::Vec3{0.0f, 1.0f, 0.0f},
                                               osg::Vec3{0.0f, 0.0f, 1.0f},
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


   mMatrixNode = new osg::MatrixTransform;
   mMatrixNode->setMatrix(osg::Matrix::scale(osg::Vec3(1, mWidth, -mHeight)) *
                          osg::Matrix::translate(osg::Vec3(mX, mY, mZ)));
   mMatrixNode->addChild(quad);

   root->addChild(mMatrixNode.get());
   root->getOrCreateStateSet()->setAttribute(new osg::Depth(osg::Depth::ALWAYS, 0.0, 1.0, false));
   root->getOrCreateStateSet()->setRenderBinDetails(INT_MAX, "RenderBin");
   root->getOrCreateStateSet()->setNestRenderBins(false);
}

void VaAttachmentTexture::SetPosition(float aX, float aY, float aZ)
{
   mX = aX;
   mY = aY;
   mZ = aZ;

   UpdateMatrix();
}

void VaAttachmentTexture::SetSize(float aW, float aH)
{
   mWidth  = aW;
   mHeight = aH;

   UpdateMatrix();
}

void VaAttachmentTexture::UpdateMatrix()
{
   if (mMatrixNode)
   {
      mMatrixNode->setMatrix(osg::Matrix::scale(osg::Vec3(1, mWidth, -mHeight)) *
                             osg::Matrix::translate(osg::Vec3(mX, mY, mZ)));
   }
}

void VaAttachmentTexture::LoadImageFile(const char* aFilename)
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

VaViewer* VaAttachmentTexture::CreateViewerTexture(VaViewer* aParentPtr, unsigned int aResX, unsigned int aResY)
{
   // note, we are not caching this off if we don't have a node
   if (mMatrixNode)
   {
      osg::StateSet* ss = mMatrixNode->getOrCreateStateSet();

      mViewerPtr = new vespa::VaViewerSubordinate(aParentPtr, aResX, aResY, vespa::VaViewer::SmallFeatureCulling);
      vespa::VaEnvironment::Instance().GetViewerManager()->AddViewer(mViewerPtr);
      mViewerPtr->Initialize();

      osg::Texture2D* texture = mViewerPtr->GetTexture();
      if (texture != nullptr)
      {
         ss->setTextureAttributeAndModes(0, texture);
      }
      return mViewerPtr;
   }
   return nullptr;
}

void VaAttachmentTexture::SetShader(const char* aVertexShaderName, const char* aFragmentShaderName)
{
   if (mMatrixNode)
   {
      osg::StateSet* ss   = mMatrixNode->getOrCreateStateSet();
      std::string    vert = std::string(aVertexShaderName) + ".vert";
      std::string    frag = std::string(aFragmentShaderName) + ".frag";
      ss->setAttributeAndModes(UtoShaders::UseProgram(vert, frag));
   }
}

void VaAttachmentTexture::SetUniform(const char* aName, float aValue)
{
   if (mMatrixNode)
   {
      osg::StateSet* ss = mMatrixNode->getOrCreateStateSet();
      ss->getOrCreateUniform(aName, osg::Uniform::FLOAT)->set(aValue);
   }
}

void VaAttachmentTexture::SetUniform4f(const char* aName, float aX, float aY, float aZ, float aW)
{
   if (mMatrixNode)
   {
      osg::StateSet* ss = mMatrixNode->getOrCreateStateSet();
      ss->getOrCreateUniform(aName, osg::Uniform::FLOAT_VEC4)->set(osg::Vec4(aX, aY, aZ, aW));
   }
}

} // end namespace vespa.
