// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WkfAttachmentGreatCircleRuler_hpp
#define WkfAttachmentGreatCircleRuler_hpp

#include "wkf_export.h"

#include <string>

#include "UtoTypes.hpp"
#include "VaAttachment.hpp"
#include "VaCallbackHolder.hpp"
#include "VaDecoratorNode.hpp"
#include "VaEntity.hpp"
#include "VaLocationReference.hpp"
#include "VaPosition.hpp"
#include "VaViewer.hpp"

namespace wkf
{
class WKF_EXPORT AttachmentGreatCircleRuler : public vespa::VaAttachment
{
public:
   enum RulerMode
   {
      cGREAT_CIRCLE,
      cSTRAIGHT_LINE
   };

   enum LabelMode
   {
      cHEADING        = 0x01,
      cRANGE          = 0x02,
      cSPEED          = 0x04,
      cETA            = 0x08,
      cAT_SOURCE      = 0x10,
      cAT_DESTINATION = 0x20,
      cORTHO_RANGES   = 0x40, // currently only working on cAT_SOURCE, by design
      cDEFAULT_LABEL  = 0x3F
   };

   AttachmentGreatCircleRuler(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr);

   ~AttachmentGreatCircleRuler() override;

   RulerMode GetMode() const { return mMeasureMode; }
   void      SetMode(RulerMode aMode);
   RulerMode ToggleMode();

   LabelMode    GetLabelMode() const { return mLabelMode; }
   virtual void SetLabelMode(int aMode);

   int  GetLabelSize() const { return mLabelSize; }
   void SetLabelSize(int aSize);

   bool GetActive() const { return mMeasureActive; }
   void SetActive(bool aActive) { mMeasureActive = aActive; }

   void              SetPosition(const vespa::VaPosition& aPosition);
   void              SetPosition(const vespa::VaEntity& aEntity);
   void              SetSrcPosition(const vespa::VaPosition& aPosition);
   void              SetSrcPosition(const vespa::VaEntity& aEntity);
   void              SetDstPosition(const vespa::VaPosition& aPosition);
   void              SetDstPosition(const vespa::VaEntity& aEntity);
   int               GetSrcDstSet() const { return mMeasureSrcDstSet; }
   vespa::VaPosition GetLastSrc() const { return mMeasureLastSrc; }
   vespa::VaPosition GetLastDst() const { return mMeasureLastDst; }

   // in some cases we may want the endpoint to belong to the measurement tool (a temporary entity)
   // this give management of the pointer this attachment
   void Take(vespa::VaEntity* aEntityPtr);

   void Clear();

   bool Pick(vespa::VaViewer*                aViewerPtr,
             int                             aMouseX,
             int                             aMouseY,
             bool                            aToggle,
             bool                            aDoubleClick         = false,
             const vespa::VaAttachmentSubId& aAttachmentSubPartId = 0) override;

   unsigned int GetPickMode() const override { return VaAttachment::cPICK_ATTACHMENT; }

   void SetSelected(vespa::VaViewer*                aViewerPtr,
                    bool                            aSelected,
                    const vespa::VaAttachmentSubId& aSubId,
                    bool                            aForce = false) override;

   bool IsHighlightable() override { return false; }

   vespa::VaLocationReference& GetSrcLocationRef() { return mMeasureSrc; }
   vespa::VaLocationReference& GetDstLocationRef() { return mMeasureDst; }

   void SetMeasureLengthUnits(int aUnits);
   void ResetMeasureLengthUnits();

   void SetColor(const UtoColor& aNewColor);

   void Build(bool aForce = false);

   std::string GetDescription(unsigned int aSubId) override { return std::string(); }

protected:
   void ToggleColor();

private:
   void PrivateInitializeCallbacks() override;
   void PrivateLoad() override;
   void Update(double aTime) override;
   void UpdateFrame(double aTime) override;
   void UpdateColor();

   void UnitsChangedCB(int aMeasurementType, int aUnit);
   void EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving);

   void UpdateCursorPositionCB(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY);

   void GetRelativePosition(const vespa::VaPosition& aLLA, double aRelXYZ[3]) const;

   vespa::VaCallbackHolder              mCallbacks;
   RulerMode                            mMeasureMode;
   RulerMode                            mMeasureLastMode;
   LabelMode                            mLabelMode;
   LabelMode                            mLabelLastMode;
   int                                  mLabelSize;
   bool                                 mMeasureActive;
   int                                  mMeasureSrcDstSet;
   int                                  mMeasureLengthUnits;
   bool                                 mLengthUnitOverride;
   int                                  mMeasureSpeedUnits;
   int                                  mMeasureTimeUnits;
   int                                  mMeasureAngleUnits;
   bool                                 mMeasureFlag;
   int                                  mMeasureSrcCallbackId;
   int                                  mMeasureDstCallbackId;
   vespa::VaLocationReference           mMeasureSrc;
   vespa::VaLocationReference           mMeasureDst;
   vespa::VaPosition                    mMeasureLastSrc;
   vespa::VaPosition                    mMeasureLastDst;
   double                               mMeasureLastSrcVel[3];
   double                               mMeasureLastDstVel[3];
   std::string                          mMeasureLineShapeName;
   UtoPolyLineShape*                    mMeasureLinePtr;
   std::string                          mMeasureVerticalLineShapeName;
   UtoPolyLineShape*                    mMeasureVerticalLinePtr;
   std::string                          mSrcMeasureLabelShapeName;
   QString                              mSrcString;
   osg::ref_ptr<osg::MatrixTransform>   mSrcTransform;
   osg::ref_ptr<vespa::VaDecoratorNode> mSrcMeasureLabelPtr;
   std::string                          mDstMeasureLabelShapeName;
   QString                              mDstString;
   osg::ref_ptr<osg::MatrixTransform>   mDstTransform;
   osg::ref_ptr<vespa::VaDecoratorNode> mDstMeasureLabelPtr;
   UtoColor                             mColor;
   std::vector<vespa::VaEntity*>        mOwnershipList;
};

} // namespace wkf


VA_DECLARE_OBJECT_TYPE(wkf::AttachmentGreatCircleRuler)
#endif
