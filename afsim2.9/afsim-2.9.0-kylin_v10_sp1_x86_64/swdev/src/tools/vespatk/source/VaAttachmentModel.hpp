// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VaAttachmentModel_hpp
#define VaAttachmentModel_hpp

#include "vespatk_export.h"

#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/ref_ptr>

#include "UtoRawShape.hpp"
#include "VaAttachment.hpp"

class UtoMapProjection;

namespace vespa
{
#define VAHWINST

class VESPATK_EXPORT VaAttachmentModel : public VaAttachment
{
public:
   VaAttachmentModel(VaEntity& aParent, VaViewer* aViewerPtr, const std::string& aName = "model");

   ~VaAttachmentModel() override;

   bool PrivateInitialize() override;

   void         SetIcon(const std::string& aName);
   void         SetScale(const float aScale);
   void         UseLightSpecularUniform(bool aUse);
   void         UseTeamColorUniform(bool aShow);
   void         UseBillboard(bool aUse);
   virtual void SetColor(const UtColor& aColor);
   float        GetModelScale() const { return mModelScale; }
#ifdef VAHWINST
   void UpdateFrame(double aTime) override;
   void EntityMovedCB(VaEntity* aEntityPtr, bool aMoveInTime);
   void SetRelativeToParentTransform(osg::RefMatrix* aTransform);
   void MapProjectionChangedCB(unsigned int aViewerId, const UtoMapProjection* aProjection);
#else
#endif

   unsigned int GetPickMode() const override { return cPICK_ENTITY; }

   void ArticulateModel(const std::string& aPartName, const osg::Matrix& aMatrix);

protected:
   unsigned int mModelId;
   osg::Matrix  mModelMatrix;
   osg::Matrix  mViewMatrix;
   osg::Vec4    mTeamColor;

private:
   void SetIconPrivate(const std::string& aName);
#ifdef VAHWINST
   void ViewMatrixChanged(VaViewer* aViewerPtr);
#endif

   bool Highlight(VaViewer* aViewerPtr, const VaAttachmentSubId& aSubpartId, bool aHighlight, const UtColor& aColor) override;
#ifndef VAHWINST
   UtoRawShape*            mShapePtr;
   osg::ref_ptr<osg::Node> mDB_ModelRefPtr;
#else
   osg::ref_ptr<osg::RefMatrix>       mRelativeToParentTransform;
   osg::ref_ptr<osg::MatrixTransform> mTransformNode;
   osg::ref_ptr<osg::Node>            mHighlightNode;
   std::vector<UtColor>               mHighlightColorStack;
   double                             mXYZ[3] = {0.0, 0.0, 0.0};
   double                             mHPR[3] = {0.0, 0.0, 0.0};
#endif
   std::string mIconName;
   // this is a bounding sphere radius of the current model
   float       mModelScale;
   osg::Matrix mScaleMatrix;

   // currently used by event markers/track icons to distinguish their settings from other models: can be used by other
   // models to ignore model-wide settings the double boolean approach is used because there is a situation where both
   // should be false (when no overwrite of other model settings is necessary)
   bool mIgnoreTeamColorUniform;     // always suppress team color, regardless of other settings
   bool mUseTeamColorUniform;        // always show team color, regardless of other settings
   bool mIgnoreLightSpecularUniform; // always ignore the "light source" coming from view camera
   bool mUseLightSpecularUniform;    // always assume a "light source" coming from the view camera
   bool mIgnoreBillboard;
   bool mUseBillboard;
};

} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentModel)

#endif
