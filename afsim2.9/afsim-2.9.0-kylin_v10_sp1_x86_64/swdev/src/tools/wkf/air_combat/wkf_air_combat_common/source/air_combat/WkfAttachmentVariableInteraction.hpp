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

#ifndef ATTACHMENTVARIABLEINTERACTION_HPP
#define ATTACHMENTVARIABLEINTERACTION_HPP

#include "wkf_air_combat_common_export.h"

#include "UtoInteractionShape.hpp"
#include "VaAttachment.hpp"
#include "VaEntity.hpp"

namespace wkf
{
class WKF_AIR_COMBAT_COMMON_EXPORT AttachmentVariableInteraction : public vespa::VaAttachment
{
public:
   AttachmentVariableInteraction(vespa::VaEntity&   aSrcEntity,
                                 vespa::VaEntity&   aTargetEntity,
                                 vespa::VaViewer*   aViewerPtr,
                                 const std::string& aDesc);

   void UpdateFrame(double aTime) override;

   void SetFillFactor(double aFactor);
   void SetColor(const UtoColor& aColor);
   void SetOffset(double aOffset);
   void SetWidth(double aWidth);

private:
   vespa::VaEntity*     mTargetEntity;
   UtoInteractionShape* mLineShapePtr;
   UtoColor             mColor;

   double mOffset;
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentVariableInteraction)

#endif
