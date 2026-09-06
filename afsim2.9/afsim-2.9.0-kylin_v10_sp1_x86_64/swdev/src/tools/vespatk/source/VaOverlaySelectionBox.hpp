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

#ifndef VaOverlaySelectionBox_hpp
#define VaOverlaySelectionBox_hpp

#include "vespatk_export.h"

#include "VaOverlay.hpp"

class UtoPolyLineShape;

namespace vespa
{
class VESPATK_EXPORT VaOverlaySelectionBox : public VaOverlay
{
public:
   VaOverlaySelectionBox();
   VaOverlaySelectionBox(const std::string& aName);
   ~VaOverlaySelectionBox() override;

   VaOverlaySelectionBox* Clone() const override;

   bool Initialize() override;
   void Update() override;

   HandleEventMode GetHandleEventMode() const override { return cHANDLE_EVENTS_ALWAYS; }

   bool Btn1Down(int aMouseX, int aMouseY, int aState) override;

   bool Btn1Drag(int aMouseX, int aMouseY, int aState) override;

   bool Btn1Up(int aMouseX, int aMouseY, int aState) override;

   bool Btn3Down(int aMouseX, int aMouseY, int aState) override;

protected:
   VaOverlaySelectionBox(const VaOverlaySelectionBox& aSrc);

   UtoPolyLineShape* mSelectionLinePtr;
   UtoPolygonShape*  mSelectionBoxPtr;
   int               mBoxX1;
   int               mBoxY1;
   int               mBoxX2;
   int               mBoxY2;

   virtual void BoxEnd();
   virtual void BoxDraw(int aX1, int aY1, int aX2, int aY2);

private:
   bool mBoxPossible;
   bool mBoxActive;
   bool mBoxFirst;
   int  mBoxLastBuf;
   int  mBoxLastX2[2];
   int  mBoxLastY2[2];
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlaySelectionBox)

#endif
