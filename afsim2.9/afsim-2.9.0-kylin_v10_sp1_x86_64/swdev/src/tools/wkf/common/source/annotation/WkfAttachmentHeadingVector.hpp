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

#ifndef WKFATTACHMENTHEADINGVECTOR_HPP
#define WKFATTACHMENTHEADINGVECTOR_HPP

#include "wkf_common_export.h"

#include <osg/Vec4>

#include "UtVec3.hpp"
#include "VaAttachment.hpp"

class UtoMapProjection;
class UtoVectorShape;
class UtoPolyLineShape;

namespace wkf
{
class WKF_COMMON_EXPORT AttachmentHeadingVector : public vespa::VaAttachment
{
public:
   AttachmentHeadingVector(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr);
   ~AttachmentHeadingVector() override = default;

   void Update(double aTime) override;
   void UpdateVisibility() override;
   void ShowVector(bool aState, const UtColor& aColor);

private:
   UtoVectorShape* mShape;
   bool            mIntendedVisible{false};
   bool            mVisible{false};
   int             mVectorId{-1};
   float           mModelScalar{1.0};
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentHeadingVector)

#endif
