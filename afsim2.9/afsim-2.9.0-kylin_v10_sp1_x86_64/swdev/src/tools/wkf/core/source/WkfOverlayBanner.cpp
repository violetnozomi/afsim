// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfOverlayBanner.hpp"

#include "UtoRawShape.hpp"
#include "VaObserver.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::OverlayBanner::OverlayBanner()
   : vespa::VaOverlay("wkfOverlayBanner")
{
}

wkf::OverlayBanner::OverlayBanner(const std::string& aName)
   : vespa::VaOverlay(aName)
{
}

void wkf::OverlayBanner::Setup(const QString& aText, const QColor& aColor, const QFont& aFont)
{
   if (!mTextNode)
   {
      UtoRawShape raw;
      mTransform = new osg::MatrixTransform;
      // set up the position
      mShapePtr = VaOverlay::BindShape(GetName(), raw);
      mTextNode = new vespa::VaDecoratorNode;
      mShapePtr->GetTree()->addChild(mTransform);
      mTransform->addChild(mTextNode);
      mShapePtr->OverrideRenderBin(100, false);
      mCallbacks.Add(
         vespa::VaObserver::ViewerResized.Connect(&OverlayBanner::ViewerResizedCB, this, GetViewer()->GetUniqueId()));
   }
   mTextNode->SetText(aText, aColor, aFont, vespa::VaDecoratorNode::cRectangle);
   double vp[4];
   vaEnv.GetStandardViewer()->GetViewport(vp[0], vp[1], vp[2], vp[3]);
   mTransform->setMatrix(osg::Matrix::translate((vp[2] - mTextNode->GetWidth()) * 0.5, vp[3] - mTextNode->GetHeight(), 0.0));
}

void wkf::OverlayBanner::ViewerResizedCB(vespa::VaViewer* aViewerPtr)
{
   if (mTextNode)
   {
      double vp[4];
      vaEnv.GetStandardViewer()->GetViewport(vp[0], vp[1], vp[2], vp[3]);
      mTransform->setMatrix(
         osg::Matrix::translate((vp[2] - mTextNode->GetWidth()) * 0.5, vp[3] - mTextNode->GetHeight(), 0.0));
   }
}
