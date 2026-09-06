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

#ifndef VaOverlayZoomBox_hpp
#define VaOverlayZoomBox_hpp

#include "VaCallbackHolder.hpp"
#include "VaOverlay.hpp"

class UtoPolyLineShape;
class VaViewer;

namespace vespa
{
class VESPATK_EXPORT VaOverlayZoomBox : public vespa::VaOverlay
{
public:
   VaOverlayZoomBox();
   VaOverlayZoomBox(const std::string& aName);
   ~VaOverlayZoomBox() override;

   VaOverlayZoomBox* Clone() const override;

   bool Initialize() override;
   void Update() override;

   enum ZoomDirection
   {
      cZOOM_OUT = -1,
      cZOOM_IN  = 1
   };
   void SetZoomDirection(ZoomDirection aZoomDir);

private:
   VaOverlayZoomBox(const VaOverlayZoomBox& aSrc);

   // Called from Update() to animate the box when zooming
   void UpdateAnimation();
   void SetBoxSize(int aWidth, int aHeight);

   void UpdateCursorPositionCB(VaViewer* aViewerPtr, int aMouseX, int aMouseY);

   UtoPolyLineShape*       mZoomBoxPtr;
   vespa::VaCallbackHolder mCallbacks;
   int                     mMouseX;
   int                     mMouseY;
   int                     mSemiWidth;  // half the box width
   int                     mSemiHeight; // half the box height
   int                     mZoomDirection;
   int                     mZoomAnimationState;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayZoomBox)
#endif
