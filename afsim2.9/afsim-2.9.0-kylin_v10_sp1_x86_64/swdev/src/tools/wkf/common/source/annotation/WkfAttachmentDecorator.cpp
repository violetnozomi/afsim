// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfAttachmentDecorator.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QFontMetrics>
#include <QGLWidget>
#include <QImage>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/MatrixTransform>
#include <osg/PolygonStipple>
#include <osg/Texture2D>

#include "UtMath.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "VaViewer.hpp"

wkf::AttachmentDecorator::AttachmentDecorator(vespa::VaEntity& aParent,
                                              vespa::VaViewer* aViewer,
                                              const QImage&    aImage,
                                              const QString&   aImageDescription,
                                              bool             aStemmed,
                                              bool             aStippleWhenOccluded,
                                              float            aOffsetX,
                                              float            aOffsetY)
   : vespa::VaAttachment(aParent, aViewer, "decorator")
   , mDecoratorNode()
{
   SetType<AttachmentDecorator>();
   Build(aStemmed, aStippleWhenOccluded, aOffsetX, aOffsetY);
   SetImage(aImage, aImageDescription);
}

wkf::AttachmentDecorator::AttachmentDecorator(vespa::VaEntity& aParent,
                                              vespa::VaViewer* aViewer,
                                              const QString&   aText,
                                              bool             aStemmed,
                                              bool             aStippleWhenOccluded,
                                              float            aOffsetX,
                                              float            aOffsetY)
   : vespa::VaAttachment(aParent, aViewer, "decorator")
   , mDecoratorNode()
{
   SetType<AttachmentDecorator>();
   Build(aStemmed, aStippleWhenOccluded, aOffsetX, aOffsetY);
   SetText(aText);
}

void wkf::AttachmentDecorator::SetImage(const QImage& aImage, const QString& aImageDescription)
{
   mDecoratorNode->SetImage(aImage, aImageDescription);
}

void wkf::AttachmentDecorator::SetText(const QString& aText, const QColor& aBackgroundColor, const QColor aTextColor)
{
   mDecoratorNode->SetText(aText, aBackgroundColor, aTextColor);
}

void wkf::AttachmentDecorator::SetColor(float aRed, float aGreen, float aBlue, float aAlpha)
{
   mDecoratorNode->SetColor(aRed, aGreen, aBlue, aAlpha);
}

void wkf::AttachmentDecorator::SetOffset(float aOffsetX, float aOffsetY)
{
   mDecoratorNode->SetOffset(aOffsetX, aOffsetY);
}

void wkf::AttachmentDecorator::Build(bool aStemmed, bool aStippleWhenOccluded, float aOffsetX, float aOffsetY)
{
   int flags = vespa::VaDecoratorNode::cFramed | vespa::VaDecoratorNode::cDrawWhenOccluded;
   if (aStemmed)
   {
      flags = flags | vespa::VaDecoratorNode::cStemmed;
   }
   if (aStippleWhenOccluded)
   {
      flags = flags | vespa::VaDecoratorNode::cStippleWhenOccluded;
   }
   mDecoratorNode = new vespa::VaDecoratorNode(flags, aOffsetX, aOffsetY);

   UtoRawShape raw;
   raw.SetReferenceFrame(UtoShape::cFRAME_ENTITY);

   mShapePtr = AddShapeToParent(GetName() + GetUniqueIdString(), raw);
   mShapePtr->SetName(GetName() + GetUniqueIdString());
   mShapePtr->GetTree()->addChild(mDecoratorNode.get());
   mShapePtr->OverrideRenderBin(100, false);
}
