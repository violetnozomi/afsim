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

#include "WkfAttachmentDataRing.hpp"

#include <QImage>
#include <QPainter>
#include <QPen>

#include "UtoRawShape.hpp"
#include "VaTimeManager.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::AttachmentDataRing::AttachmentDataRing(vespa::VaEntity& aParent, vespa::VaViewer* aViewer)
   : VaAttachment(aParent, aViewer, "Data Ring")
   , mShape(nullptr)
   , mDecoratorNode()
   , mUpdater(nullptr)
   , mMinColor(Qt::red)
   , mMaxColor(Qt::green)
   , mClampedToBase(false)
   , mLevel(1)
   , mMinValue(0)
   , mMaxValue(0)
   , mTargetValue(0)
   , mCurrentValue(0)
{
   SetType<AttachmentDataRing>();

   mDecoratorNode = new vespa::VaDecoratorNode(vespa::VaDecoratorNode::cDrawWhenOccluded, 0, 0);

   UtoRawShape raw;
   raw.SetReferenceFrame(UtoShape::cFRAME_ENTITY);

   mShape = AddShapeToParent(GetName() + std::to_string(GetUniqueId()), raw);
   mShape->SetName(GetName());
   mShape->GetTree()->addChild(mDecoratorNode.get());
   mShape->OverrideRenderBin(100, false);

   mDecoratorNode->SetImage(QImage(1, 1, QImage::Format::Format_ARGB32_Premultiplied), "blank");
}

void wkf::AttachmentDataRing::UpdateFrame(double aTime)
{
   // Grab the newest data value from our updater.
   UnitlessValueUpdater* updater = dynamic_cast<UnitlessValueUpdater*>(mUpdater.get());
   if (updater != nullptr)
   {
      if (updater->IsValid())
      {
         mTargetValue = updater->GetValueDouble();
      }
   }

   // To keep the baseline at a value of 0, make the minimum value always
   // equal the negative maximum value.
   if (mClampedToBase)
   {
      mMinValue = -1 * mMaxValue;
   }

   mCurrentValue          = mCurrentValue + (mTargetValue - mCurrentValue) * 0.3;
   double normalizedValue = 2 * ((mCurrentValue - mMinValue) / (mMaxValue - mMinValue)) - 1;

   // Lighten the color the further away the ring is from being filled.
   QColor color;
   normalizedValue >= 0 ? color = mMaxColor : color = mMinColor;
   double lightValue = ((1 - std::abs(normalizedValue)) * 25) + 100;
   color             = color.lighter(lightValue);

   double iconScale = vaEnv.GetStandardViewer()->GetIconScale();
   QImage image((128 * iconScale) + (15 * mLevel),
                (128 * iconScale) + (15 * mLevel),
                QImage::Format::Format_ARGB32_Premultiplied);
   image.fill(Qt::transparent);

   mDecoratorNode->SetXOffset(-(image.width() / 2) / 0.75);
   mDecoratorNode->SetYOffset(-(image.height() / 2) / 0.75);

   QPen     pen;
   QPainter painter(&image);
   painter.setRenderHints(QPainter::RenderHint::Antialiasing);

   double diameter = 30 + (15 * mLevel);
   diameter *= iconScale;
   QRect circleRect(0, 0, diameter, diameter);
   circleRect.moveCenter(QPoint(image.width() / 2, image.height() / 2));

   // Draw the empty portion of the meter.
   pen.setColor(Qt::darkGray);
   pen.setWidth(2.0);
   painter.setPen(pen);
   painter.drawEllipse(circleRect);

   pen.setColor(color);
   pen.setWidth(3.5);
   painter.setPen(pen);

   double startAngle = 180 * 16; // Qt expects angle values in 1/16th of a degree.
   double spanAngle  = -(2 * osg::PI * normalizedValue) * (180 / osg::PI) * 16;

   painter.drawArc(circleRect, startAngle, spanAngle);

   // Draw the baseline of the meter.
   pen.setColor(Qt::black);
   pen.setWidth(4.0);
   painter.setPen(pen);
   int x1 = circleRect.center().x() - (diameter / 3);
   int x2 = x1 - (diameter / 4);
   int y  = circleRect.center().y();
   painter.drawLine(x1, y, x2, y);

   mDecoratorNode->SetImage(image, mUpdater == nullptr ? "" : mUpdater->GetUpdaterName());
}
