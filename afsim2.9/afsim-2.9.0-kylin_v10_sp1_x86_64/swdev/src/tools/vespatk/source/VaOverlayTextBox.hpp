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

#ifndef VaOverlayTextBox_hpp
#define VaOverlayTextBox_hpp

#include "vespatk_export.h"

#include "VaCallbackHolder.hpp"
#include "VaOverlayWindow.hpp"

class UtoTextShape;

namespace vespa
{
class VESPATK_DEPRECATED_EXPORT VaOverlayTextBox : public VaOverlayWindow
{
public:
   VaOverlayTextBox();
   VaOverlayTextBox(const std::string& aName);
   ~VaOverlayTextBox() override;

   VaOverlayTextBox* Clone() const override;

   bool               Initialize() override;
   void               Update() override;
   virtual void       SetText(const std::string& aString);
   const std::string& GetText() { return mString; }
   void               SetTextColor(const UtColor& aColor) override;
   virtual void       SetTextSize(int aSize);

   // setting the alignment will define which area of the box will be positioned by reposition calls
   // using aUseAsOrigin will additionally move the screen origin for the box to the corresponding viewport position.
   virtual void SetAlignment(int aAlignment, bool aUseAsOrigin = false);

   void RepositionWindow(int aX, int aY) override;
   void ResizeWindow(int aW, int aH) override;

   virtual void SetMinSize(int aW, int aH);

   enum
   {
      ALIGN_LEFT    = 0x01,
      ALIGN_RIGHT   = 0x02,
      ALIGN_HCENTER = 0x04,
      ALIGN_TOP     = 0x08,
      ALIGN_BOTTOM  = 0x10,
      ALIGN_VCENTER = 0x20
   };

protected:
   VaOverlayTextBox(const VaOverlayTextBox& aSrc);

   VaCallbackHolder mCallbacks;

private:
   void ViewerResizedCB(VaViewer* aViewerPtr);

   virtual void RepositionWindowPrivate();
   virtual void UpdateWindowSize();

   bool Btn1Drag(int aMouseX, int aMouseY, int aState) override;

   bool Btn1DblClick(int aMouseX, int aMouseY, int aState) override;

   UtoTextShape* mTextPtr;

   int mAlignment;
   int mAlignmentPreDrag; // this is used to restore a text box to its original alignment, after dragging, by double-clicking
   bool        mUseAlignmentAsOrigin;
   int         mTextBoxPositionX;
   int         mTextBoxPositionY;
   int         mTextBoxWidth;
   int         mTextBoxHeight;
   int         mMinWidth;
   int         mMinHeight;
   int         mTextSize;
   std::string mString;

   unsigned int mCallbackId;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayTextBox)
#endif
