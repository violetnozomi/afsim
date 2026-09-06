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

#ifndef VaOverlayWindow_hpp
#define VaOverlayWindow_hpp

#include "vespatk_export.h"

#include "UtColor.hpp"
#include "VaOverlay.hpp"

namespace vespa
{

class VESPATK_DEPRECATED_EXPORT VaOverlayWindow : public VaOverlay
{
public:
   VaOverlayWindow(const std::string& aName, int aX, int aY, int aW, int aH);
   VaOverlayWindow(int aX, int aY, int aW, int aH);

   VaOverlayWindow(const VaOverlayWindow& aSrc);

   ~VaOverlayWindow() override;

   VaOverlayWindow* Clone() const override;

   bool Initialize() override;
   void Update() override;

   HandleEventMode GetHandleEventMode() const override { return cHANDLE_EVENTS_FOCUS; }

   virtual void SetBackgroundColor(const UtColor& aColor);
   virtual void SetFrameColor(const UtColor& aColor);
   virtual void SetTextColor(const UtColor& aColor);
   virtual void RepositionWindow(int aX, int aY);
   virtual void ResizeWindow(int aW, int aH);

   int GetWindowX() { return mX; }
   int GetWindowY() { return mY; }
   int GetWindowWidth() { return mW; }
   int GetWindowHeight() { return mH; }

   bool Btn1Click(int aMouseX, int aMouseY, int aState) override;
   bool Btn1DblClick(int aMouseX, int aMouseY, int aState) override;
   bool Btn1Down(int aMouseX, int aMouseY, int aState) override;
   bool Btn1Drag(int aMouseX, int aMouseY, int aState) override;
   bool Btn1Up(int aMouseX, int aMouseY, int aState) override;

   bool SelectableAt(int aMouseX, int aMouseY) const override;

   virtual void SetDraggable(bool aState) { mDraggable = aState; }
   virtual bool Draggable() { return mDraggable; }
   virtual void SetTitle(const std::string& aTitle);

   virtual void  SetCornerRadius(float aRadius);
   virtual float GetCornerRadius() { return mCornerRadius; }
   virtual float GetCurveSampling() { return mCurveSampling; }

   UtColor GetColor() { return mBackgroundColor; }
   UtColor GetFrameColor() { return mFrameColor; }
   UtColor GetTextColor() { return mTextColor; }

protected:
   UtoShape* FindShape(const std::string& aName) const override;
   UtoShape* BindShape(const std::string& aName, const UtoShape& aShape) override;
   void      UnbindShape(const std::string& aName) override;

private:
   virtual void BuildWindowFrame(int               aWidth,
                                 int               aHeight,
                                 float             aCornerRadius,
                                 float             aCurveSampling,
                                 UtoPolyLineShape& aOutline,
                                 UtoPolygonShape&  aBackground);

   UtColor     mBackgroundColor;
   UtColor     mFrameColor;
   UtColor     mTextColor;
   int         mX;
   int         mY;
   int         mW;
   int         mH;
   bool        mDraggable;
   int         mMouseX;
   int         mMouseY;
   float       mCornerRadius;
   float       mCurveSampling;
   std::string mTitleString;
   std::string mWindowShapeName;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayWindow)
#endif
