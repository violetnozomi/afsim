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
// UtoViewerHighlightImp.cpp : implementation file

#include "UtoViewerHighlight.hpp"

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
#include <osg/Stencil>
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

UtoViewerHighlight::UtoViewerHighlight(const std::string& name, int aEntitySelectionMask, bool aHideGlowBehindTerrain)
   : UtoViewer(name)
   , mEntitySelectionMask(aEntitySelectionMask)
   , mShowHighlightSelection(true)
   , mHideGlowBehindTerrain(aHideGlowBehindTerrain)
{
   Setup();
}

UtoViewerHighlight::UtoViewerHighlight(osgViewer::Viewer* aViewerPtr,
                                       const std::string& name,
                                       int                aEntitySelectionMask,
                                       bool               aHideGlowBehindTerrain)
   : UtoViewer(aViewerPtr, name)
   , mEntitySelectionMask(aEntitySelectionMask)
   , mShowHighlightSelection(true)
   , mHideGlowBehindTerrain(aHideGlowBehindTerrain)
{
   Setup();
}

void UtoViewerHighlight::Setup()
{
   mRawTexture = new osg::Texture2D;
   mRawTexture->setName("JEM_renderTexture");
   mRawTexture->setTextureSize(1024, 1024); // this will be corrected on window resize
   mRawTexture->setInternalFormat(GL_RGBA);
   mRawTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
   mRawTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

   osg::ref_ptr<osg::Geometry> screenQuad;
   screenQuad = osg::createTexturedQuadGeometry(osg::Vec3(), osg::Vec3(1, 0, 0), osg::Vec3(0, 1, 0));
   screenQuad->setName("JEM_screenQuad");
   screenQuad->setCullingActive(false);
   osg::ref_ptr<osg::Group> postrenderGroup = new osg::Group;
   mQuadGeode                               = new osg::Geode;
   mQuadGeode->addDrawable(screenQuad.get());
   mQuadGeode->setName("JEM_quadGeode");

   osg::StateSet* quadState = mQuadGeode->getOrCreateStateSet();
   quadState->setTextureAttributeAndModes(0, mRawTexture, osg::StateAttribute::ON);
   osg::BlendFunc* blendfunc = new osg::BlendFunc();
   quadState->setAttributeAndModes(blendfunc, osg::StateAttribute::ON);
   if (mHideGlowBehindTerrain)
   {
      quadState->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
   }
   quadState->setRenderBinDetails(10000000, "DepthSortedBin");
   postrenderGroup->getOrCreateStateSet()->setRenderBinDetails(15, "DepthSortedBin");
   osg::Stencil* stencilFunc = new osg::Stencil();
   stencilFunc->setFunction(osg::Stencil::EQUAL, 0, ~0);
   stencilFunc->setOperation(osg::Stencil::ZERO, osg::Stencil::ZERO, osg::Stencil::ZERO);
   quadState->setAttributeAndModes(stencilFunc, osg::StateAttribute::ON);
   mQuadGeode->setCullingActive(false);

   ApplyMyShaderTo(quadState);
   //////////////////////
   // This is the camera that looks at the scene and renders to mRawTexture
   //////////////////////
   mTextureCamera = new osg::Camera;
   mTextureCamera->setName("highlightEffectCamera");
   mTextureCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   mTextureCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   mTextureCamera->setClearColor(osg::Vec4(0, 0, 0, 0));
   mTextureCamera->setViewport(0, 0, 1024, 1024);

   // note: we used to use a FBO here with PRE_RENDER, but the switch to QOpenGlWidget seemed to break that
   // the options were to not use pre_render, which results in a one frame latency in highlights and alignment issues
   // or to switch to a FRAME_BUFFER which has potential performance costs.
   mTextureCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER);
   mTextureCamera->setRenderOrder(osg::Camera::PRE_RENDER);

   mTextureCamera->attach(osg::Camera::COLOR_BUFFER, mRawTexture.get());

   mTextureCamera->setCullMask(mEntitySelectionMask);
   /////////////////////

   ApplyObjectShader(mTextureCamera->getOrCreateStateSet());
   /////////////////////

   /////////////////////
   // This is the scene camera, it draws the scene, and then a full screen quad of our processed texture on top
   /////////////////////
   osg::ref_ptr<osg::Camera> orthoCamera = GetOSGViewer()->getCamera(); // new osg::Camera;

   orthoCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));

   orthoCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   orthoCamera->setViewMatrix(osg::Matrix::identity());
   orthoCamera->setName("sceneCamera");

   orthoCamera->setViewport(0, 0, 1, 1);

   mLastPassCamera = new osg::Camera;
   mLastPassCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   mLastPassCamera->setClearMask(GL_STENCIL_BUFFER_BIT);
   osg::Stencil* stencilFunc2 = new osg::Stencil();
   stencilFunc2->setFunction(osg::Stencil::ALWAYS, 0, ~0);
   stencilFunc2->setOperation(osg::Stencil::INCR, osg::Stencil::INCR, osg::Stencil::INCR);

   osg::Depth* depthFunc2 = new osg::Depth();
   depthFunc2->setFunction(osg::Depth::ALWAYS);
   depthFunc2->setWriteMask(0);
   mLastPassCamera->getOrCreateStateSet()->setAttributeAndModes(depthFunc2);

   osg::ColorMask* colormask = new osg::ColorMask(false, false, false, false);
   mLastPassCamera->getOrCreateStateSet()->setAttributeAndModes(colormask);
   mLastPassCamera->getOrCreateStateSet()->setAttributeAndModes(stencilFunc2);

   mLastPassCamera->setViewport(0, 0, 1024, 1024);
   mLastPassCamera->setRenderOrder(osg::Camera::NESTED_RENDER);
   mLastPassCamera->setCullMask(mEntitySelectionMask);
   ApplyObjectShader(mLastPassCamera->getOrCreateStateSet());

   GetRootNode()->insertChild(0, mTextureCamera.get());

   postrenderGroup->addChild(mLastPassCamera.get());
   postrenderGroup->addChild(mQuadGeode.get());
   orthoCamera->addChild(postrenderGroup.get());

   mSelectedEntityGroup = new osg::Group;
}

/* UTO Function Block **********************************************************

Function: UtoViewerImp

Description:
Destructor

*******************************************************************************/

UtoViewerHighlight::~UtoViewerHighlight() {}


bool UtoViewerHighlight::Connect(UtoEntityDB& edb)
{
   bool retval = UtoViewer::Connect(edb);
   if (mHideGlowBehindTerrain)
   {
      osg::ref_ptr<osg::Group> terrainGroup = new osg::Group;
      terrainGroup->addChild(UtoViewer::GetWorld()->GetTerrainTree());

      osg::BlendFunc* blendfunc = new osg::BlendFunc(GL_ONE, GL_ZERO);
      terrainGroup->getOrCreateStateSet()->setAttributeAndModes(blendfunc, osg::StateAttribute::ON);
      ApplyObjectShader(terrainGroup->getOrCreateStateSet());
      osg::Vec4                  blackvec4(0, 0, 0, 0);
      osg::ref_ptr<osg::Uniform> black = new osg::Uniform("highlightColor", blackvec4);
      terrainGroup->getOrCreateStateSet()->addUniform(black);

      mTextureCamera->addChild(terrainGroup.get());
   }

   mTextureCamera->addChild(mSelectedEntityGroup.get());
   mLastPassCamera->addChild(mSelectedEntityGroup.get());

   return retval;
}

//////////////////////////


void UtoViewerHighlight::SetViewport(int aX, int aY, int aW, int aH)
{
   mRawTexture->setTextureSize(aW, aH);
   mRawTexture->dirtyTextureObject();
   mTextureCamera->setRenderingCache(0);
   mTextureCamera->setViewport(aX, aY, aW, aH);

   UtoViewer::SetViewport(aX, aY, aW, aH);
   mLastPassCamera->setViewport(aX, aY, aW, aH);
}

void UtoViewerHighlight::SetViewMatrix(const osg::Matrix& aMatrix)
{
   mTextureCamera->setViewMatrix(aMatrix);
   mLastPassCamera->setViewMatrix(aMatrix);
   UtoViewer::SetViewMatrix(aMatrix);
}

void UtoViewerHighlight::SetViewMatrixAsLookAt(const osg::Vec3& aEye, const osg::Vec3& aCenter, const osg::Vec3& aUp)
{
   mTextureCamera->setViewMatrixAsLookAt(aEye, aCenter, aUp);
   mLastPassCamera->setViewMatrixAsLookAt(aEye, aCenter, aUp);
   UtoViewer::SetViewMatrixAsLookAt(aEye, aCenter, aUp);
}

void UtoViewerHighlight::SetProjectionMatrixAsPerspective(float aFOV, float aAspectRatio, float aHither, float aYon)
{
   UtoViewer::SetProjectionMatrixAsPerspective(aFOV, aAspectRatio, aHither, aYon);
   mTextureCamera->setProjectionMatrixAsPerspective(aFOV, aAspectRatio, aHither, aYon);
   mLastPassCamera->setProjectionMatrixAsPerspective(aFOV, aAspectRatio, aHither, aYon);
}

void UtoViewerHighlight::SetProjectionMatrixAsOrtho(float aMinX, float aMaxX, float aMinY, float aMaxY, float aMinZ, float aMaxZ)
{
   UtoViewer::SetProjectionMatrixAsOrtho(aMinX, aMaxX, aMinY, aMaxY, aMinZ, aMaxZ);
   mTextureCamera->setProjectionMatrixAsOrtho(aMinX, aMaxX, aMinY, aMaxY, aMinZ, aMaxZ);
   mLastPassCamera->setProjectionMatrixAsOrtho(aMinX, aMaxX, aMinY, aMaxY, aMinZ, aMaxZ);
}


//////////////////////////

void UtoViewerHighlight::ApplyMyShaderTo(osg::StateSet* ss)
{
   osg::ref_ptr<osg::Program> prog = UtoShaders::UseProgram("highlightBlur.vert", "highlightBlur.frag");

   ss->setAttribute(prog.get(), osg::StateAttribute::ON);
   ss->getOrCreateUniform("tex2D", osg::Uniform::INT)->set(0);
}

void UtoViewerHighlight::ApplyObjectShader(osg::StateSet* ss)
{
   osg::ref_ptr<osg::Program> prog = UtoShaders::UseProgram("highlightObject.vert", "highlightObject.frag");

   ss->setAttributeAndModes(prog.get(), osg::StateAttribute::OVERRIDE);
   osg::Vec4                  magentavec4(1, 0, 1, 1);
   osg::ref_ptr<osg::Uniform> magenta        = new osg::Uniform("highlightColor", magentavec4);
   osg::ref_ptr<osg::Uniform> screenScalable = new osg::Uniform("VA_TrueScale", 0.0f);
   ss->addUniform(magenta);
   ss->addUniform(screenScalable);
}

void UtoViewerHighlight::NewSymbolAdded(UtoShape* aShapePtr)
{
   aShapePtr->GetTree()->setNodeMask(aShapePtr->GetTree()->getNodeMask() & ~mEntitySelectionMask);
}

class nodePassthrough : public osg::Node
{
public:
   nodePassthrough(osg::Node* aNodePtr)
      : mRef(aNodePtr)
   {
   }
   ~nodePassthrough() override {}
   void accept(osg::NodeVisitor& nv) override
   {
      osg::ref_ptr<osg::Node> refNode;
      mRef.lock(refNode);
      if (refNode)
      {
         refNode->accept(nv);
      }
   }
   bool                         isGarbage() const { return !mRef.valid(); }
   osg::observer_ptr<osg::Node> mRef;
};

class UtoNodeReference : public osg::MatrixTransform
{
public:
   UtoNodeReference(osg::Node* aReference)
   {
      mPassthrough = new nodePassthrough(aReference);
      addChild(mPassthrough);
   }
   ~UtoNodeReference() override {}
   static osg::Group* up(osg::Node* aNode)
   {
      if (aNode->getNumParents() == 0)
         return nullptr;
      return aNode->getParent(0);
   }

   void accept(osg::NodeVisitor& nv) override
   {
      if (!mPassthrough)
         return;
      // find common ancestor
      std::set<const Group*> parentSet;
      Group*                 p = up(this);
      while (p)
      {
         parentSet.insert(p);
         p = up(p);
      }
      if (!mPassthrough->mRef)
         return;
      p = up(mPassthrough->mRef.get());
      std::vector<osg::Group*> ancestors;
      while (parentSet.find(p) == parentSet.end())
      {
         ancestors.push_back(p);
         p = up(p);
         if (p == nullptr) // we didn't find a common ancestor... maybe on another branch
         {
            Group* pup = (osg::Group*)(*(ancestors.rbegin()));
            ancestors.pop_back();
            Group* lastp = nullptr;
            while ((!ancestors.empty()) && (pup->getNumParents() <= 1))
            {
               lastp = pup;
               pup   = (osg::Group*)(*(ancestors.rbegin()));
               ancestors.pop_back();
            }
            if (ancestors.empty())
            {
               break;
            }
            else
            {
               unsigned int pi;
               for (pi = 0; pi < pup->getNumParents(); ++pi)
               {
                  if (pup->getParent(pi) == lastp)
                  {
                     pi++;
                     break;
                  }
               }
               if (pi >= pup->getNumParents()) // really shouldn't give up here.  keep checking the ancestors...
               {
                  break;
               }
               else
               {
                  p = pup->getParent(pi); // start down a different path;
               }
            }
         }
      }
      if (p)
      {
         osg::Matrix mat;
         for (size_t i = 0; i < ancestors.size(); ++i)
         {
            osg::Transform* trans = ancestors[ancestors.size() - i - 1]->asTransform();
            if (trans)
            {
               osg::MatrixTransform* matTrans = trans->asMatrixTransform();
               if (matTrans)
               {
                  mat.mult(matTrans->getMatrix(), mat);
               }
            }
         }
         setMatrix(mat);
      }
      osg::MatrixTransform::accept(nv);
      if (mPassthrough->isGarbage())
      {
         removeChild(mPassthrough);
         mPassthrough = nullptr;
      }
   }
   nodePassthrough* mPassthrough;
};

void UtoViewerHighlight::HighlightWithColor(osg::Node* aNodePtr, bool aState, const float aColor[4])
{
   if (aState)
   {
      if (mNodeToRef.find(aNodePtr) == mNodeToRef.end())
      {
         UtoNodeReference* nref = new UtoNodeReference(aNodePtr);
         mNodeToRef[aNodePtr]   = nref;
         osg::Vec4 color(aColor[0], aColor[1], aColor[2], aColor[3]);
         nref->getOrCreateStateSet()->addUniform(new osg::Uniform("highlightColor", color));
         mSelectedEntityGroup->addChild(nref);
      }
   }
   else if (mNodeToRef.find(aNodePtr) != mNodeToRef.end())
   {
      mSelectedEntityGroup->removeChild(mNodeToRef[aNodePtr]);
      mNodeToRef.erase(aNodePtr);
   }
}

void UtoViewerHighlight::HighlightNode(osg::Node* aNodePtr, bool aState, const float aColor[4])
{
   if (aState)
   {
      mSelectedEntityGroup->addChild(aNodePtr);
      osg::Vec4 color(aColor[0], aColor[1], aColor[2], aColor[3]);
      aNodePtr->getOrCreateStateSet()->addUniform(new osg::Uniform("highlightColor", color));
      aNodePtr->setNodeMask(aNodePtr->getNodeMask() | mEntitySelectionMask);
   }
   else if (mNodeToRef.find(aNodePtr) != mNodeToRef.end())
   {
      aNodePtr->setNodeMask(aNodePtr->getNodeMask() & ~mEntitySelectionMask);
      mSelectedEntityGroup->removeChild(aNodePtr);
   }
}

osg::Group* GetShapeRoot(UtoShape* aShapePtr, UtoViewer& aViewer)
{
   UtoShapeFrameTransform* frame = dynamic_cast<UtoShapeFrameTransform*>(aShapePtr->FindFrame(aViewer));
   if (frame)
   {
      return frame;
   }
   return nullptr;
}

void UtoViewerHighlight::HighlightSymbolsWithColor(UtoEntity*                      aEntityPtr,
                                                   const std::vector<std::string>& aShapeNameList,
                                                   bool                            aOn,
                                                   const float                     aColor[4],
                                                   bool                            aAutoScale)
{
   for (size_t i = 0; i < aShapeNameList.size(); ++i)
   {
      UtoShape* shapePtr = aEntityPtr->FindShape(aShapeNameList[i]);
      if (shapePtr)
      {
         if (shapePtr->IsVisible(this))
         {
            if (aOn)
            {
               osg::Vec4 color(aColor[0], aColor[1], aColor[2], aColor[3]);

               osg::Group* node = GetShapeRoot(shapePtr, *this);
               if (node)
               {
                  shapePtr->GetTree()->getOrCreateStateSet()->addUniform(
                     new osg::Uniform("VA_TrueScale", aAutoScale ? 0.0F : 1.0F));
                  shapePtr->GetTree()->getOrCreateStateSet()->addUniform(new osg::Uniform("highlightColor", color));
                  if (0 == (shapePtr->GetTree()->getNodeMask() & mEntitySelectionMask))
                  {
                     shapePtr->GetTree()->setNodeMask(shapePtr->GetTree()->getNodeMask() | mEntitySelectionMask);
                     mSelectedEntityGroup->addChild(node);
                  }
               }
            }
            else
            {
               shapePtr->GetTree()->setNodeMask(shapePtr->GetTree()->getNodeMask() & ~mEntitySelectionMask);
               osg::Group* node = GetShapeRoot(shapePtr, *this);
               if (node)
               {
                  mSelectedEntityGroup->removeChild(node);
               }
            }
         }
      }
   }
}

void UtoViewerHighlight::SuppressSelectionIndication(bool aState)
{
   if (aState)
   {
      mTextureCamera->setCullMask(0);
   }
   else
   {
      mTextureCamera->setCullMask(mEntitySelectionMask);
   }
}

void UtoViewerHighlight::ShapeRemoved(UtoShape* aShapePtr)
{
   if (aShapePtr->Owner())
   {
      std::vector<std::string> shapes;
      shapes.push_back(aShapePtr->GetName());
      float color[4] = {0.f, 0.f, 0.f, 0.f};
      HighlightSymbolsWithColor(aShapePtr->Owner(), shapes, false, color, false);
   }
   UtoViewer::ShapeRemoved(aShapePtr);
}

void UtoViewerHighlight::SetUniform(const char* aName, const float aValue)
{
   UtoViewer::SetUniform(aName, aValue);

   osg::StateSet* ss = mTextureCamera->getOrCreateStateSet();
   ss->getOrCreateUniform(aName, osg::Uniform::FLOAT)->set((float)aValue);
   osg::StateSet* ss2 = mLastPassCamera->getOrCreateStateSet();
   ss2->getOrCreateUniform(aName, osg::Uniform::FLOAT)->set((float)aValue);
}


void UtoViewerHighlight::SetUniform(const char* aName,
                                    const float aValue1,
                                    const float aValue2,
                                    const float aValue3,
                                    const float aValue4)
{
   UtoViewer::SetUniform(aName, aValue1, aValue2, aValue3, aValue4);

   osg::StateSet* ss = mTextureCamera->getOrCreateStateSet();
   ss->getOrCreateUniform(aName, osg::Uniform::FLOAT_VEC4)->set(osg::Vec4(aValue1, aValue2, aValue3, aValue4));
   osg::StateSet* ss2 = mLastPassCamera->getOrCreateStateSet();
   ss2->getOrCreateUniform(aName, osg::Uniform::FLOAT_VEC4)->set(osg::Vec4(aValue1, aValue2, aValue3, aValue4));
}
