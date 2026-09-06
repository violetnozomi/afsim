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

#include "VaModelViewer.hpp"

#include <osg/BlendFunc>
#include <osg/LineWidth>
#include <osg/PositionAttitudeTransform>

#include "UtoEntity.hpp"
#include "UtoEntityDB.hpp"
#include "UtoResourceDB.hpp"
#include "UtoShaderUtilities.hpp"
#include "UtoUtility.hpp"
#include "UtoViewerHighlight.hpp"
#include "VaAttachmentModel.hpp"
#include "VaCamera.hpp"
#include "VaModelViewerCamera.hpp"
#include "VaObserver.hpp"
#include "VaUtils.hpp"
#include "VaViewerManager.hpp"

namespace vespa
{

void VaAttachmentModelRaw::PrivateLoad()
{
   if (mShapeName.empty())
   {
      static unsigned int uid = 0;
      mShapeName              = "RawAttachment" + vespa::VaUtils::ToString<unsigned int>(++uid);
   }

   // Remove the previous model.
   RemoveShapeFromParent(mShapeName);
   mShapePtr = nullptr;

   // Load the shape.
   UtoRawShape shape;
   mShapePtr = dynamic_cast<UtoRawShape*>(AddShapeToParent(mShapeName, shape));
   mShapePtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY);
}

void VaAttachmentModelRaw::SetNode(osg::Node* aNode)
{
   mShapePtr->GetRoot()->addChild(aNode);
}

VaModelViewer::VaModelViewer()
   : VaViewer(VaViewer::HighlightViewer, 0)
   , mEntityPtr(nullptr)
   , mRawPtr(nullptr)
   , mGridPtr(nullptr)
{
   delete mCameraPtr;
   mCameraPtr = new VaModelViewerCamera(this);
   mCameraPtr->UsePerspectiveProjection();
   mCameraPtr->SetViewer(this);

   mOffset = new osg::PositionAttitudeTransform;
}

void VaModelViewer::Initialize(osgViewer::Viewer* aViewerPtr, bool aShaderable, bool aGeomShaderable)
{
   if (!mInitialized)
   {
      mInitialized = true;

      // Figure out if this computer supports shaders.
      UtoShaderUtilities::EnableShaders(aShaderable);
      UtoShaderUtilities::EnableGeometryShaders(aGeomShaderable);
      SetupViewer(aViewerPtr, aShaderable);

      SetActiveCamera(UtoViewer::PERSPECTIVE);

      mViewerPtr->SetClearColor(osg::Vec4(0.4F, 0.6F, 0.8F, 1.0F));

      mViewerPtr->Connect(UtoEntityDB::Instance(""));

      mEntityPtr = new VaEntity("solo");
      mEntityPtr->SetPosition(vespa::VaPosition(0, 0, 30000));
      mEntityPtr->Initialize();
      SetUniform("uto_LightSourceAmbient", 0.4F, 0.4F, 0.4F, 1.0F);
      SetUniform("uto_LightSourceDiffuse", 1.0F, 1.0F, 1.0F, 1.0F);
      SetUniform("uto_LightSourceSpecular", 1.0F, 1.0F, 1.0F, 1.0F);
      SetUniform("vtk_sunVec", 0.57F, -0.57F, -0.57F);
   }
}

VaModelViewer::~VaModelViewer()
{
   if (mEntityPtr != nullptr)
   {
      delete mEntityPtr;
   }
}

void VaModelViewer::SetupViewer(osgViewer::Viewer* aViewerPtr, bool aUseShaders)
{
   mViewerPtr = new UtoViewerHighlight(aViewerPtr, "Modelview Display");
   mViewerPtr->SetFilter(GetNewViewFilter());

   if (mOptions & SmallFeatureCulling)
   {
      mViewerPtr->EnableSmallFeatureCulling();
   }
   // default some uniform values
   SetTrueScale(true);
   SetIconScale(1.0);
   SuppressTeamColor(true);
}

void VaModelViewer::ClearRawModel()
{
   if (mRawPtr)
   {
      mEntityPtr->RemoveAttachment(mRawPtr->GetUniqueId());
      mRawPtr = nullptr;
   }
}

void VaModelViewer::ShowRawModel(osg::Node* aNodePtr)
{
   if (!mRawPtr)
   {
      mRawPtr = make_attachment<VaAttachmentModelRaw>(*mEntityPtr, this);
      mRawPtr->ShowShape(mRawPtr->GetName(), true);
      mRawPtr->Initialize();
      mRawPtr->Load();
      mRawPtr->SetNode(mOffset);
   }
   if (mRawPtr)
   {
      mOffset->removeChildren(0, mOffset->getNumChildren());
      mOffset->addChild(aNodePtr);
   }
}

void VaModelViewer::UseGrid(double aX, double aY, double aZ, GridMode aMode)
{
   if (mRawPtr)
   {
      mOffset->setPosition(osg::Vec3(-aX, -aY, -aZ));
   }
   HideGrid();
   mGridPtr = make_attachment<VaAttachmentModelRaw>(*mEntityPtr, this);
   mGridPtr->ShowShape(mGridPtr->GetName(), true);
   mGridPtr->Initialize();
   mGridPtr->Load();

   if (mGridPtr)
   {
      osg::Geode* geode = new osg::Geode;
      geode->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
      geode->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc());
      geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      geode->getOrCreateStateSet()->setNestRenderBins(false);
      geode->getOrCreateStateSet()->setRenderBinDetails(10, "RenderBin");

      osg::Geometry* grid = new osg::Geometry;
      geode->addDrawable(grid);
      osg::Vec3Array* g_vArray = new osg::Vec3Array;
      osg::Vec4Array* g_cArray = new osg::Vec4Array;
      double          x1       = 0.0;
      double          y1       = 0.0;
      double          z1       = 0.0;
      double          x2       = 0.0;
      double          y2       = 0.0;
      double          z2       = 0.0;
      double          x3       = 0.0;
      double          y3       = 0.0;
      double          z3       = 0.0;
      double          x4       = 0.0;
      double          y4       = 0.0;
      double          z4       = 0.0;
      for (int i = -10; i <= 10; ++i)
      {
         if (aMode == XY)
         {
            x1 = x2 = i / 10.0;
            y1      = -1;
            y2      = 1;
            z1 = z2 = z3 = z4 = 0;
            x3                = -1;
            x4                = 1;
            y3 = y4 = i / 10.0;
         }
         else if (aMode == XZ)
         {
            x1 = x2 = i / 10.0;
            z1      = -1;
            z2      = 1;
            y1 = y2 = y3 = y4 = 0;
            x3                = -1;
            x4                = 1;
            z3 = z4 = i / 10.0;
         }
         else if (aMode == YZ)
         {
            z1 = z2 = i / 10.0;
            y1      = -1;
            y2      = 1;
            x1 = x2 = x3 = x4 = 0;
            z3                = -1;
            z4                = 1;
            y3 = y4 = i / 10.0;
         }
         g_cArray->push_back(osg::Vec4(0, 0, 0, 0.6));
         g_vArray->push_back(osg::Vec3(x1, y1, z1));
         g_cArray->push_back(osg::Vec4(0, 0, 0, 0.6));
         g_vArray->push_back(osg::Vec3(x2, y2, z2));
         g_cArray->push_back(osg::Vec4(0, 0, 0, 0.6));
         g_vArray->push_back(osg::Vec3(x3, y3, z3));
         g_cArray->push_back(osg::Vec4(0, 0, 0, 0.6));
         g_vArray->push_back(osg::Vec3(x4, y4, z4));

         g_cArray->push_back(osg::Vec4(0, 0, 0, 0.6));
         g_vArray->push_back(osg::Vec3(x1, y1, z1) * 10.0);
         g_cArray->push_back(osg::Vec4(0, 0, 0, 0.6));
         g_vArray->push_back(osg::Vec3(x2, y2, z2) * 10.0);
         g_cArray->push_back(osg::Vec4(0, 0, 0, 0.6));
         g_vArray->push_back(osg::Vec3(x3, y3, z3) * 10.0);
         g_cArray->push_back(osg::Vec4(0, 0, 0, 0.6));
         g_vArray->push_back(osg::Vec3(x4, y4, z4) * 10.0);

         g_cArray->push_back(osg::Vec4(0, 0, 0, 0.6));
         g_vArray->push_back(osg::Vec3(x1, y1, z1) * 100.0);
         g_cArray->push_back(osg::Vec4(0, 0, 0, 0.6));
         g_vArray->push_back(osg::Vec3(x2, y2, z2) * 100.0);
         g_cArray->push_back(osg::Vec4(0, 0, 0, 0.6));
         g_vArray->push_back(osg::Vec3(x3, y3, z3) * 100.0);
         g_cArray->push_back(osg::Vec4(0, 0, 0, 0.6));
         g_vArray->push_back(osg::Vec3(x4, y4, z4) * 100.0);
      }
      grid->setVertexArray(g_vArray);
      grid->setColorArray(g_cArray);
      grid->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
      grid->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 252));

      osg::Geometry* redLine = new osg::Geometry;
      redLine->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(4));
      geode->addDrawable(redLine);
      osg::Vec3Array* rl_vArray = new osg::Vec3Array;
      osg::Vec4Array* rl_cArray = new osg::Vec4Array;
      rl_cArray->push_back(osg::Vec4(1, 0, 0, 0.8));
      rl_vArray->push_back(osg::Vec3(0, 0, 0));
      rl_cArray->push_back(osg::Vec4(1, 0, 0, 0.8));
      rl_vArray->push_back(osg::Vec3(1000, 0, 0));

      redLine->setVertexArray(rl_vArray);
      redLine->setColorArray(rl_cArray);
      redLine->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
      redLine->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));

      mGridPtr->SetNode(geode);
   }
}

void VaModelViewer::HideGrid()
{
   if (mGridPtr)
   {
      mEntityPtr->RemoveAttachment(mGridPtr->GetUniqueId());
      mGridPtr = nullptr;
   }
}

} // namespace vespa
