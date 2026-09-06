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

#ifndef VAMODELVIEWER_HPP
#define VAMODELVIEWER_HPP

#include "vespatk_export.h"

#include "VaAttachment.hpp"
#include "VaViewer.hpp"

class UtoRawShape;

namespace osg
{
class PositionAttitudeTransform;
}

namespace vespa
{
class VaEntity;
class VaAttachmentModel;

class VaAttachmentModelRaw : public VaAttachment
{
public:
   VaAttachmentModelRaw(VaEntity& aParent, VaViewer* aViewer)
      : VaAttachment(aParent, aViewer, "rawAtt")
      , mShapePtr(nullptr)
      , mShapeName()
   {
      SetType<VaAttachmentModelRaw>();
   }
   void SetNode(osg::Node* aNode);

private:
   void         PrivateLoad() override;
   UtoRawShape* mShapePtr;
   std::string  mShapeName;
};

class VESPATK_EXPORT VaModelViewer : public VaViewer
{
public:
   enum GridMode
   {
      XY,
      XZ,
      YZ
   };
   VaModelViewer();
   ~VaModelViewer() override;

   void         Initialize(osgViewer::Viewer* aViewerPtr, bool aShaderable, bool aGeomShaderable) override;
   void         SetupViewer(osgViewer::Viewer* aViewerPtr, bool aUseShaders) override;
   virtual void ClearRawModel();
   virtual void ShowRawModel(osg::Node* aNodePtr);

   virtual void UseGrid(double aX, double aY, double aZ, GridMode aMode);
   virtual void HideGrid();

protected:
   VaModelViewer(const VaModelViewer& aSrc);

private:
   UtoShape*                                    AddShapeToParent(const std::string& aShapeName, UtoShape& aShape);
   VaEntity*                                    mEntityPtr;
   VaAttachmentModelRaw*                        mRawPtr;
   VaAttachmentModelRaw*                        mGridPtr;
   osg::ref_ptr<osg::PositionAttitudeTransform> mOffset;
};

} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentModelRaw)

#endif
