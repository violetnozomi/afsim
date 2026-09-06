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

#include "WkfAttachmentLabel.hpp"

#include <QColor>

#include "VaDecoratorNode.hpp"

wkf::AttachmentLabel::AttachmentLabel(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr)
   : vespa::VaAttachment(aParent, aViewerPtr, "label")
{
   SetType<AttachmentLabel>();
}

void wkf::AttachmentLabel::PrivateLoad()
{
   UtoRawShape rawShape;
   mShapePtr = dynamic_cast<UtoRawShape*>(AddShapeToParent(GetName() + std::to_string(GetUniqueId()), rawShape));

   mTextNodePtr = new vespa::VaDecoratorNode(vespa::VaDecoratorNode::cHideBehindEarth);
   mShapePtr->GetRoot()->addChild(mTextNodePtr);
}

void wkf::AttachmentLabel::SetString(const QString& aString)
{
   mString = aString;
   if (mTextNodePtr)
   {
      mTextNodePtr->SetText(mString, mColor, mFont, vespa::VaDecoratorNode::cOutline);
   }
}

void wkf::AttachmentLabel::SetColor(const QColor& aColor)
{
   mColor = aColor;
   if (mTextNodePtr)
   {
      mTextNodePtr->SetText(mString, mColor, mFont, vespa::VaDecoratorNode::cOutline);
   }
}

void wkf::AttachmentLabel::SetFont(const QFont& aFont)
{
   mFont = aFont;
   if (mTextNodePtr)
   {
      mTextNodePtr->SetText(mString, mColor, mFont, vespa::VaDecoratorNode::cOutline);
   }
}

void wkf::AttachmentLabel::Setup(const QString& aString, const QColor& aColor, const QFont& aFont)
{
   mString = aString;
   mColor  = aColor;
   mFont   = aFont;
   if (mTextNodePtr)
   {
      mTextNodePtr->SetText(mString, mColor, mFont, vespa::VaDecoratorNode::cOutline);
   }
}
