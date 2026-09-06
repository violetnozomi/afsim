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

#ifndef WKFATTACHMENTVISUALEFFECT_HPP
#define WKFATTACHMENTVISUALEFFECT_HPP

#include "wkf_common_export.h"

#include "UtoRawShape.hpp"
#include "VaAttachment.hpp"
#include "VaAttachmentModel.hpp"
#include "VaCallbackHolder.hpp"
#include "WkfPlatform.hpp"
#include "WkfVisualEffects.hpp"
#include "WkfVtkEnvironment.hpp"

namespace wkf
{
class WKF_COMMON_EXPORT AttachmentVisualEffect : public vespa::VaAttachment
{
public:
   AttachmentVisualEffect(vespa::VaEntity&   aEntity,
                          vespa::VaViewer*   aViewerPtr,
                          VisualEffect&      aEffect,
                          const std::string& aDesc);

   VisualEffect* GetEffect() const { return mEffect; }

private:
   std::string   mDesc;
   VisualEffect* mEffect;
   UtoRawShape*  mEffectShapePtr;

   vespa::VaCallbackHolder mCallbacks;
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentVisualEffect)

#endif // !WKFATTACHMENTVISUALEFFECT_HPP
