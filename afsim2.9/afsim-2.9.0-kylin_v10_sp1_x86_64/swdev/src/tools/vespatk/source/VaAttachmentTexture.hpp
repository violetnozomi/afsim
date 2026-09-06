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

#ifndef VaAttachmentTexture_hpp
#define VaAttachmentTexture_hpp

#include "vespatk_export.h"

#include <osg/MatrixTransform>
#include <osg/ref_ptr>

#include "UtoRawShape.hpp"
#include "VaAttachment.hpp"

namespace vespa
{
class VaViewerSubordinate;

//! Class for textured quads that can be attached to entities
class VESPATK_EXPORT VaAttachmentTexture : public VaAttachment
{
public:
   VaAttachmentTexture(VaEntity& aParent, VaViewer* aViewerPtr);
   ~VaAttachmentTexture() override;

   void SetPosition(float aX, float aY, float aZ);
   void SetSize(float aWidth, float aHeight);

   void             LoadImageFile(const char* aFilename);
   vespa::VaViewer* CreateViewerTexture(VaViewer* aParentPtr, unsigned int aResX, unsigned int aResY);

   // virtual void UpdateFrame(double aTime) { }
   // void Update() override;
   void SetShader(const char* aVertexShaderName, const char* aFragmentShaderName);
   void SetUniform(const char* aName, float aValue);
   void SetUniform4f(const char* aName, float aX, float aY, float aZ, float aW);

   bool IsVisible() const override { return true; }

private:
   void PrivateLoad() override;
   void UpdateMatrix();

   static unsigned int sId;

   UtoRawShape*                       mShapePtr; //!< The visual representation of the ring
   std::string                        mShapeName;
   osg::ref_ptr<osg::MatrixTransform> mMatrixNode;
   float                              mX;
   float                              mY;
   float                              mZ;
   float                              mWidth;
   float                              mHeight;
   vespa::VaViewerSubordinate*        mViewerPtr;
};
} // end namespace vespa.

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentTexture)
#endif
