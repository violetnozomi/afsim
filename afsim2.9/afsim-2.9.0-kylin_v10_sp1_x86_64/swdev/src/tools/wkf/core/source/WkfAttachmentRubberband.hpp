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

#ifndef WKFATTACHMENTRUBBERBAND_HPP
#define WKFATTACHMENTRUBBERBAND_HPP

#include "wkf_export.h"

#include <functional>

#include <osg/Geometry>

#include "UtoRawShape.hpp"
#include "VaAttachment.hpp"
#include "VaLocationReference.hpp"

class UtoPolyLineShape;

namespace wkf
{
class WKF_EXPORT AttachmentRubberband : public vespa::VaAttachment
{
public:
   // the rubberband attachment is used to draw a line between two entities while selecting a pair
   // this is temporary, and will be destroyed on completion or cancel (in WkfViewer)
   // the update function is called as the mouse cursor is dragged
   // the finish function is called on a selection
   AttachmentRubberband(vespa::VaEntity&                                        aParent,
                        vespa::VaViewer*                                        aViewerPtr,
                        const std::function<void(AttachmentRubberband&)>&       aUpdateFunc,
                        const std::function<void(const AttachmentRubberband&)>& aFinishFunc,
                        const vespa::VaEntity&                                  aSource);
   ~AttachmentRubberband() override = default;

   void                     SetSrcPosition(const vespa::VaPosition& aPosition);
   void                     SetSrcPosition(const vespa::VaEntity& aEntity);
   vespa::VaEntity*         GetHoverEntity() const { return mHoverEntityPtr; }
   const vespa::VaPosition& GetHoverLocation() const { return mHoverLocation; }
   void                     SetLineColor(const UtoColor& aColor);
   void                     End();
   bool                     IsHighlightable() override { return false; }

private:
   void PrivateInitializeCallbacks() override;
   void PrivateLoad() override;

   void Build(bool aForce = false);

   void EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving);

   void UpdateCursorPositionCB(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY);
   void GetRelativePosition(const vespa::VaPosition& aLLA, double aRelXYZ[3]) const;

   std::string                                      mShapeName;
   UtoRawShape*                                     mShapePtr{nullptr};
   osg::ref_ptr<osg::Geometry>                      mGeometry;
   vespa::VaLocationReference                       mMeasureSrc;
   int                                              mMeasureSrcCallbackId{-1};
   std::function<void(AttachmentRubberband&)>       mUpdateFunc;
   std::function<void(const AttachmentRubberband&)> mFinishFunc;
   vespa::VaEntity*                                 mHoverEntityPtr{nullptr};
   vespa::VaPosition                                mHoverLocation;
   UtoColor                                         mColor;
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentRubberband)
#endif
