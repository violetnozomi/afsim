// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFATTACHMENTMODEL_HPP
#define WKFATTACHMENTMODEL_HPP

#include "wkf_export.h"

#include "VaAttachmentModel.hpp"

namespace wkf
{
class Platform;

class WKF_EXPORT AttachmentModel : public vespa::VaAttachmentModel
{
public:
   AttachmentModel(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr);
   ~AttachmentModel() override = default;

   void UpdateFrame(double aTime) override;
   void SetColor(const UtColor& aColor) override;

private:
   void PrivateInitializeCallbacks() override;

   void PlatformBrokenCB(Platform* aPlatformPtr);
   void PlatformPendingDeletionCB(Platform* aPlatformPtr);

   double    mDeletionTime;
   double    mSaturateTime;
   osg::Vec4 mTeamColorP;
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentModel)

#endif // !WKFATTACHMENTMODEL_HPP
