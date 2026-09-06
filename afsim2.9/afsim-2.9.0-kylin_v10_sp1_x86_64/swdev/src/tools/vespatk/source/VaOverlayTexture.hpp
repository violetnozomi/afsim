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

#ifndef VaOverlayTexture_hpp
#define VaOverlayTexture_hpp

#include "vespatk_export.h"

#include <functional>

#include <osg/MatrixTransform>
#include <osg/ref_ptr>

#include "VaCallbackHolder.hpp"
#include "VaOverlay.hpp"

class UtoRawShape;

namespace vespa
{
class VaViewerSubordinate;

class VESPATK_EXPORT VaOverlayTexture : public VaOverlay
{
public:
   VaOverlayTexture();
   VaOverlayTexture(const std::string& aName);
   ~VaOverlayTexture() override;

   VaOverlayTexture* Clone() const override;

   bool Initialize() override;

   void             SetPosition(float aX, float aY);
   void             SetJustification(bool aLeft, bool aBottom);
   void             SetSize(float aW, float aH);
   void             LoadImageFile(const char* aFilename);
   void             LoadImageBits(unsigned int aW, unsigned int aH, const unsigned char* aBits);
   void             LoadSubTextureFromImageFile(const char* aFilename, float afX, float afY, float afW, float afH);
   vespa::VaViewer* CreateViewerTexture(VaViewer* aParentPtr, unsigned int aResX, unsigned int aResY);
   vespa::VaViewer* GetOwnedViewer();
   void             RecalculateOrthographic(unsigned int aResX, unsigned int aResY, double aZoom);
   void             SetShader(const char* aVertexShaderName, const char* aFragmentShaderName);
   void             SetUniform(const char* aName, float aValue);
   void             SetUniform4f(const char* aName, float aX, float aY, float aZ, float aW);
   void             EnableTransparency(bool aState);

   void SetButton1ClickAction(std::function<bool(int, int, int)> aFunction);
   void SetMouseWheelAction(std::function<bool(int, int, int)> aFunction);
   bool Btn1Click(int aMouseX, int aMouseY, int aState) override;
   bool MouseWheel(int aMouseX, int aMouseY, int aDelta) override;
   bool SelectableAt(int aMouseX, int aMouseY) const override;
   void AdjustTextureCoordinatesToAspectRatio(double aRatio);

private:
   HandleEventMode GetHandleEventMode() const override { return mHandleEventMode; }

   VaOverlayTexture(const VaOverlayTexture& aSrc);

   void ViewerResizedCB(VaViewer* aViewerPtr);
   void UpdateMatrix();

   UtoRawShape*                       mShape;
   osg::ref_ptr<osg::MatrixTransform> mMatrixNode;
   float                              mX;
   float                              mY;
   enum
   {
      cRIGHT = 1,
      cTOP   = 2
   };
   unsigned int                       mJustification;
   float                              mWidth;
   float                              mHeight;
   vespa::VaViewerSubordinate*        mOwnedViewerPtr;
   std::function<bool(int, int, int)> mBtn1Function;
   std::function<bool(int, int, int)> mWheelFunction;
   HandleEventMode                    mHandleEventMode;
   VaCallbackHolder                   mCallbacks;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayTexture)
#endif
