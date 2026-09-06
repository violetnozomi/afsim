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

#include "WkfAttachmentVariableInteraction.hpp"

#include <osg/Vec3>

#include "UtEllipsoidalEarth.hpp"
#include "UtoEntity.hpp"
#include "VaViewer.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::AttachmentVariableInteraction::AttachmentVariableInteraction(vespa::VaEntity&   aSrcEntity,
                                                                  vespa::VaEntity&   aTargetEntity,
                                                                  vespa::VaViewer*   aViewerPtr,
                                                                  const std::string& aDesc)
   : VaAttachment(aSrcEntity, aViewerPtr, aDesc)
   , mTargetEntity(&aTargetEntity)
   , mLineShapePtr(nullptr)
   , mColor(UtoColor(0, 0, 0, 0))
   , mOffset(0)
{
   SetType<AttachmentVariableInteraction>();

   unsigned int   tmapSize   = 1;
   unsigned char* textureMap = new unsigned char[1];
   textureMap[0]             = 255;

   UtoInteractionShape line;
   mLineShapePtr = dynamic_cast<UtoInteractionShape*>(AddShapeToParent(aDesc + GetUniqueIdString(), line));
   mLineShapePtr->SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
   mLineShapePtr->EnableAntiAliasLines(true);
   mLineShapePtr->ArchSegments(true);
   mLineShapePtr->OverrideRenderBin(100, false);
   mLineShapePtr->SetTexturePattern(textureMap, tmapSize);
}

void wkf::AttachmentVariableInteraction::UpdateFrame(double aTime)
{
   mLineShapePtr->Clear();
   mLineShapePtr->AddOutboundSegment(mTargetEntity->GetUtoEntity(), mOffset, mColor);
}

void wkf::AttachmentVariableInteraction::SetFillFactor(double aFactor)
{
   if (mLineShapePtr != nullptr)
   {
      mLineShapePtr->SetFillFactor(aFactor);
   }
}

void wkf::AttachmentVariableInteraction::SetColor(const UtoColor& aColor)
{
   if (mLineShapePtr != nullptr)
   {
      mColor = aColor;
   }
}

void wkf::AttachmentVariableInteraction::SetOffset(double aOffset)
{
   if (mLineShapePtr != nullptr)
   {
      mOffset = aOffset;
   }
}

void wkf::AttachmentVariableInteraction::SetWidth(double aWidth)
{
   if (mLineShapePtr != nullptr)
   {
      mLineShapePtr->SetWidth(aWidth);
   }
}
