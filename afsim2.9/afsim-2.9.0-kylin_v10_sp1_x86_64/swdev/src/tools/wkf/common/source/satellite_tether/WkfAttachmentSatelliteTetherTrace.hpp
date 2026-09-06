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

#ifndef WKFATTACHMENTSATELLITETETHERTRACE_HPP
#define WKFATTACHMENTSATELLITETETHERTRACE_HPP

#include "wkf_common_export.h"

#include <QColor>
#include <osg/Drawable>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Transform>

#include "UtVec3.hpp"
#include "UtoShape.hpp"
#include "VaAttachment.hpp"

namespace rv
{
class ResultData;
}

namespace wkf
{
class WKF_COMMON_EXPORT AttachmentSatelliteTetherTrace : public vespa::VaAttachment
{
public:
   AttachmentSatelliteTetherTrace(vespa::VaEntity& aParentEntity,
                                  vespa::VaViewer& aViewer,
                                  vespa::VaEntity& aRelativeToEntity,
                                  const QColor&    aColor);

   ~AttachmentSatelliteTetherTrace() override = default;
   void         UpdateTransform(const osg::Matrix& aMat);
   unsigned int GetOriginPlatformId() const;
   std::string  GetOriginPlatformName() const;

   void         ShowFutureTracks(bool aState);
   virtual void UpdateData() = 0;

   void SetColor(const QColor& aColor) { mColor = aColor; }

protected:
   void ResetDrawing();

   static constexpr double cTOLERANCE_SQUARED{0.01};

   osg::ref_ptr<osg::Vec4Array> mColorArray;
   osg::ref_ptr<osg::Vec3Array> mVertexArray;
   vespa::VaEntity*             mOriginEntityPtr;
   QColor                       mColor;
   float                        mFutureAlpha{0.3f};
   osg::ref_ptr<osg::Geometry>  mGeometryPtr;
   UtVec3d                      mOldRic{0.0, 0.0, 0.0};

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
   void OriginDeletedCB(vespa::VaEntity* aEntityPtr);
   void Build();

   osg::ref_ptr<osg::MatrixTransform> mTransformNodePtr{nullptr};
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentSatelliteTetherTrace)
#endif
