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

#ifndef ATTACHMENTDECORATOR_HPP
#define ATTACHMENTDECORATOR_HPP

#include "wkf_common_export.h"

#include "UtoShape.hpp"
#include "VaAttachment.hpp"
#include "VaDecoratorNode.hpp"

class QImage;
#include <QColor>
#include <QString>
class UtoShape;

namespace wkf
{
class WKF_COMMON_EXPORT AttachmentDecorator : public vespa::VaAttachment
{
public:
   AttachmentDecorator(vespa::VaEntity& aParent,
                       vespa::VaViewer* aViewer,
                       const QImage&    aImage,
                       const QString&   aImageDescription,
                       bool             aStemmed             = true,
                       bool             aStippleWhenOccluded = true,
                       float            aOffsetX             = 20.0f,
                       float            aOffsetY             = 20.0f);

   AttachmentDecorator(vespa::VaEntity& aParent,
                       vespa::VaViewer* aViewer,
                       const QString&   aText,
                       bool             aStemmed             = true,
                       bool             aStippleWhenOccluded = true,
                       float            aOffsetX             = 20.0f,
                       float            aOffsetY             = 20.0f);

   unsigned int GetPickMode() const override { return cPICK_NOTHING; }

   void SetImage(const QImage& aImage, const QString& aImageDescription);
   void SetText(const QString& aText, const QColor& aBackgroundColor = Qt::white, const QColor aTextColor = Qt::black);
   const QString& GetText() { return mDecoratorNode->GetText(); }
   const QString& GetImageDescription() { return mDecoratorNode->GetImageDescription(); }

   void SetColor(float aRed, float aGreen, float aBlue, float aAlpha);
   void SetOffset(float aOffsetX, float aOffsetY);

private:
   void Build(bool aStemmed, bool aStippleWhenOccluded, float aOffsetX, float aOffsetY);

   UtoShape*                            mShapePtr;
   osg::ref_ptr<vespa::VaDecoratorNode> mDecoratorNode;
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentDecorator)

#endif
