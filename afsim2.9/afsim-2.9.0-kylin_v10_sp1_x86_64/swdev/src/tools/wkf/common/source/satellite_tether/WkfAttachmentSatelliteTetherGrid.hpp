// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFATTACHMENTSATELLITETETHERGRID_HPP
#define WKFATTACHMENTSATELLITETETHERGRID_HPP

#include <osg/Transform>

#include "UtoShape.hpp"
#include "VaAttachment.hpp"

namespace wkf
{
class AttachmentSatelliteTetherGrid : public vespa::VaAttachment
{
public:
   AttachmentSatelliteTetherGrid(vespa::VaEntity& aParentEntity, vespa::VaViewer& aViewer);

   ~AttachmentSatelliteTetherGrid() override = default;

   enum Alignment
   {
      NONE,
      IC,
      IR,
      RC
   };

   Alignment GetAlignment() const { return mAlignment; }

   void SetAlignment(const Alignment& aAlignment);
   void SetCartesian(const bool aState);
   void UpdateTransform(const osg::Matrix& aMat);

private:
   void Rebuild();
   void ViewMatrixChanged(vespa::VaViewer* aViewerPtr);

   bool      mCartesian{true};
   Alignment mAlignment{IC};

   osg::MatrixTransform* mTransformNodePtr{nullptr};
   osg::MatrixTransform* mAlignmentTransform{nullptr};
   int                   mFactor{2};
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentSatelliteTetherGrid)
#endif
