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

#ifndef WKFATTACHMENTSATELLITETETHERTRACK_HPP
#define WKFATTACHMENTSATELLITETETHERTRACK_HPP

#include <QColor>
#include <osg/Transform>
#include <osgUtil/CullVisitor>

#include "UtVec3.hpp"
#include "UtoShape.hpp"
#include "VaAttachment.hpp"

namespace vespa
{
class VaDecoratorNode;
}

namespace wkf
{
class AttachmentSatelliteTetherTrack : public vespa::VaAttachment
{
public:
   AttachmentSatelliteTetherTrack(vespa::VaEntity& aParentEntity, vespa::VaViewer& aViewer, const QColor& aColor);

   ~AttachmentSatelliteTetherTrack() override = default;

   void SetColor(const QColor& aColor);

   void ShowNames(bool aState);

private:
   class ComputeInstancedBoundingBoxCallback : public osg::Drawable::ComputeBoundingBoxCallback
   {
   public:
      osg::BoundingBox computeBound(const osg::Drawable& drawable) const override
      {
         osg::BoundingBox bounds;
         bounds.expandBy(120000000, 120000000, 120000000);
         bounds.expandBy(-120000000, -120000000, -120000000);
         return bounds;
      }
   };
   void Rebuild();

   osg::Transform*         mTransformNodePtr{nullptr};
   QColor                  mColor;
   vespa::VaDecoratorNode* mLabelNodePtr{nullptr};
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentSatelliteTetherTrack)
#endif
