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

#include "VaViewerSubordinate.hpp"

#include "UtoEntityDB.hpp"
#include "UtoViewer.hpp"
#include "UtoViewerRTTComposite.hpp"
#include "VaCamera.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaUtils.hpp"
#include "VaViewerManager.hpp"

namespace vespa
{

VaViewerSubordinate::VaViewerSubordinate(VaViewer* aParentViewerPtr, unsigned int aResX, unsigned int aResY, int aViewerOptions)
   : VaViewer(RenderToTexture, aViewerOptions)
   , mParentViewerPtr(aParentViewerPtr)
   , mResX(aResX)
   , mResY(aResY)
{
}

VaViewerSubordinate::~VaViewerSubordinate()
{
   if (mInitialized)
   {
      mParentViewerPtr->RemoveSubordinate(this);
   }
}

void VaViewerSubordinate::Initialize()
{
   if (!mInitialized)
   {
      mInitialized = true;

      SetupViewer(true);

      // Configure the camera.
      UtoViewer::CameraMode cameraMode =
         GetCamera()->GetUsePerspectiveProjection() ? UtoViewer::PERSPECTIVE : UtoViewer::ORTHOGONAL;
      SetActiveCamera(cameraMode);

      mParentViewerPtr->AddSubordinate(this);

      mViewerPtr->SetClearColor(osg::Vec4(0.0F, 0.0F, 0.0F, 1.0F));

      mViewerPtr->Connect(UtoEntityDB::Instance(""));

      VaEnvironment::Instance().Load(this);

      VaObserver::ViewerInitialized(this);
   }
   CenterCamera();
}

void VaViewerSubordinate::SetupViewer(bool aUseShaders)
{
   mViewerPtr = new UtoViewerRTTComposite("VESPA Display", mResX, mResY);
   mViewerPtr->SetFilter(GetNewViewFilter());
   Resize(0, 0, mResX, mResY);

   if (mOptions & SmallFeatureCulling)
   {
      mViewerPtr->EnableSmallFeatureCulling();
   }
   // default some uniform values
   SetTrueScale(true);
   SetIconScale(1.0);
   SuppressTeamColor(true);
}

osg::Texture2D* VaViewerSubordinate::GetTexture()
{
   return mViewerPtr->GetTexture();
}

void VaViewerSubordinate::PrepareToRedraw()
{
   VaViewer::PrepareToRedraw();
}

} // end namespace vespa
