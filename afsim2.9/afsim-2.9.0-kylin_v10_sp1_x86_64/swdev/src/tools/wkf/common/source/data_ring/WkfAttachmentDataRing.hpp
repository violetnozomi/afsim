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

#ifndef WKFATTACHMENTDATARING_HPP
#define WKFATTACHMENTDATARING_HPP

#include "wkf_common_export.h"

#include <memory>

#include <QColor>

#include "UtoShape.hpp"
#include "VaAttachment.hpp"
#include "VaDecoratorNode.hpp"
#include "WkfUpdater.hpp"

namespace wkf
{
class WKF_COMMON_EXPORT AttachmentDataRing : public vespa::VaAttachment
{
public:
   AttachmentDataRing(vespa::VaEntity& aParent, vespa::VaViewer* aViewer);

   void UpdateFrame(double aTime) override;

   void     SetUpdater(std::unique_ptr<Updater> aUpdater) { mUpdater = std::move(aUpdater); }
   Updater* GetUpdater() const { return mUpdater.get(); }

   void SetLevel(int aLevel) { mLevel = aLevel; }
   int  GetLevel() const { return mLevel; }

   void SetMinValue(double aValue) { mMinValue = aValue; }
   void SetMaxValue(double aValue) { mMaxValue = aValue; }
   void SetTargetValue(double aValue) { mTargetValue = aValue; }
   void SetClampedToBase(bool aClamp) { mClampedToBase = aClamp; }
   void SetMinColor(const QColor& aColor) { mMinColor = aColor; }
   void SetMaxColor(const QColor& aColor) { mMaxColor = aColor; }

private:
   UtoShape*                            mShape;
   osg::ref_ptr<vespa::VaDecoratorNode> mDecoratorNode;

   std::unique_ptr<Updater> mUpdater;

   QColor mMinColor;
   QColor mMaxColor;

   bool   mClampedToBase; // If true, only colorizes the ring if the value is above the baseline.
   int    mLevel;         // The position of this ring relative to other rings (1 being innermost).
   double mMinValue;      // The minimum (full red) value.
   double mMaxValue;      // The maximum (full green) value.
   double mTargetValue;   // The value returned by the updater on each update (or manually set via SetTargetValue()).
   double mCurrentValue;  // The value currently displayed on the ring. Interpolates toward the target value on each
                          // update to 'animate' the value.
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentDataRing)

#endif // !WKFATTACHMENTDATARING_HPP
