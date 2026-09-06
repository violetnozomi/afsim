// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfAttachmentHeadingVector.hpp"

#include "UtoVectorShape.hpp"
#include "VaAttachmentModel.hpp"
#include "VaEntity.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"

namespace
{
const double cVELOCITY_THRESHOLD = 0.001;
}

wkf::AttachmentHeadingVector::AttachmentHeadingVector(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr)
   : vespa::VaAttachment(aParent, aViewerPtr, "headingVector")
   , mShape(nullptr)
{
   SetType<AttachmentHeadingVector>();
   vespa::VaAttachmentModel* modelPtr = GetParent().FindFirstAttachmentOfType<vespa::VaAttachmentModel>();
   if (modelPtr)
   {
      mModelScalar = modelPtr->GetModelScale();
   }
}

void wkf::AttachmentHeadingVector::Update(double aTime)
{
   if (mVectorId >= 0)
   {
      UtVec3d wcsxyz = GetParent().GetVelocityWCS();
      if (wcsxyz.Magnitude() > cVELOCITY_THRESHOLD)
      {
         // currently not visible, but needs to become visible
         if (!mVisible && mIntendedVisible)
         {
            bool parentVisibility = GetParent().IsVisible(GetViewer());
            mShape->Show(parentVisibility, *(GetViewer()->GetViewer()));
            mVisible = parentVisibility;
         }
         float az = atan2(wcsxyz[1], wcsxyz[0]);
         float el = atan2(wcsxyz[2], sqrt(wcsxyz[0] * wcsxyz[0] + wcsxyz[1] * wcsxyz[1]));
         mShape->UpdateVector(mVectorId, az, el);
      }
      // currently visible, but needs to become invisible
      else if (mVisible) //&& magnitude == 0
      {
         mShape->Show(false, *(GetViewer()->GetViewer()));
         mVisible = false;
      }
   }
}

void wkf::AttachmentHeadingVector::UpdateVisibility()
{
   if (mVectorId >= 0)
   {
      bool visible = IsVisible() && mVisible && mIntendedVisible;
      mShape->Show(visible, *(GetViewer()->GetViewer()));
      mVisible = visible;
   }
}

void wkf::AttachmentHeadingVector::ShowVector(bool aState, const UtColor& aColor)
{
   float rgb[3];
   aColor.Get(rgb[0], rgb[1], rgb[2]);
   osg::Vec4 color(rgb[0], rgb[1], rgb[2], 1);
   UtVec3d   wcsxyz     = GetParent().GetVelocityWCS();
   bool      visibility = (wcsxyz.Magnitude() > cVELOCITY_THRESHOLD) ? GetParent().IsVisible(GetViewer()) : false;
   if (mVectorId >= 0 && aState)
   {
      mShape->UpdateVectorColor(mVectorId, color);
   }
   else if (aState) // shape not created yet
   {
      RemoveShapeFromParent("vector" + std::to_string(GetUniqueId()));
      mShape = nullptr;
      UtoVectorShape vector;
      mShape = dynamic_cast<UtoVectorShape*>(AddShapeToParent("vector" + std::to_string(GetUniqueId()), vector));

      mShape->SetScalar(20 * mModelScalar);

      float az  = atan2(wcsxyz[1], wcsxyz[0]);
      float el  = atan2(wcsxyz[2], sqrt(wcsxyz[0] * wcsxyz[0] + wcsxyz[1] * wcsxyz[1]));
      mVectorId = mShape->AddVector(az, el, color, false);
   }
   if (mVectorId >= 0)
   {
      mShape->Show(aState && visibility, *(GetViewer()->GetViewer()));
   }
   mVisible         = aState && visibility;
   mIntendedVisible = aState;
}
