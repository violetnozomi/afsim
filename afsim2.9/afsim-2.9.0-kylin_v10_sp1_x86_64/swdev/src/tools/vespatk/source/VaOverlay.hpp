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

#ifndef VaOverlay_hpp
#define VaOverlay_hpp

#include "vespatk_export.h"

#include <string>

#include "VaDefs.hpp"
#include "VaObject.hpp"

class UtoShape;

#ifdef KeyPress
#undef KeyPress // X11 defines KeyPress :(
#endif

namespace vespa
{
class VaViewer;

class VESPATK_EXPORT VaOverlay : public VaObject
{
public:
   enum HandleEventMode
   {
      cHANDLE_EVENTS_NEVER,
      cHANDLE_EVENTS_ALWAYS,
      cHANDLE_EVENTS_FOCUS
   };

   enum OverlayType
   {
      cOVERLAY_WINDOW,
      cOVERLAY_WORLD
   };

   enum OverlayScope
   {
      cOVERLAY_GLOBAL,
      cOVERLAY_ENTITY
   };

   friend class VaViewer;

   // when aCreateUniqueName is set, the overlay will create a unique name from the provided name and the VaObject unique-id
   // this being the case, it is up to the user to store the name, should they want to find it later
   // but also we can automatically dodge name collision.
   VaOverlay(const std::string& aOverlayName, OverlayType aOverlayType = cOVERLAY_WINDOW, bool aCreateUniqueName = false);

   virtual VaOverlay* Clone() const = 0;

   OverlayType  GetOverlayType() const { return mOverlayType; }
   OverlayScope GetOverlayScope() const { return mOverlayScope; }
   void         SetOverlayScope(OverlayScope aOverlayScope) { mOverlayScope = aOverlayScope; }

   // Overlay methods.

   virtual bool Initialize() = 0;
   virtual void Update();
   virtual void Resize() {}

   const std::string& GetName() const { return mName; }

   VaViewer*       GetViewer() { return mViewerPtr; }
   const VaViewer* GetViewer() const { return mViewerPtr; }

   bool         IsVisible() const;
   virtual void SetVisible(bool aVisible);

   void SetSelectable(bool aSelectable) { mSelectable = aSelectable; }

   virtual bool Selectable() const { return mSelectable; }
   virtual bool SelectableAt(int aMouseX, int aMouseY) const { return false; }

   virtual HandleEventMode GetHandleEventMode() const { return cHANDLE_EVENTS_NEVER; }

   virtual bool Btn1Click(int aMouseX, int aMouseY, int aState) { return Selectable(); }
   virtual bool Btn1DblClick(int aMouseX, int aMouseY, int aState) { return Selectable(); }
   virtual bool Btn1Down(int aMouseX, int aMouseY, int aState) { return Selectable(); }
   virtual bool Btn1Drag(int aMouseX, int aMouseY, int aState) { return Selectable(); }
   virtual bool Btn1Up(int aMouseX, int aMouseY, int aState) { return Selectable(); }
   virtual bool Btn3Down(int aMouseX, int aMouseY, int aState) { return Selectable(); }

   virtual bool MouseMove(int aMouseX, int aMouseY) { return Selectable(); }

   virtual void MousedOffOf(int aMouseX, int aMouseY) { return; }

   virtual bool KeyPress(int aMouseX, int aMouseY, int aKey, int aState) { return false; }

   virtual bool MouseWheel(int aMouseX, int aMouseY, int aDelta) { return false; }

protected:
   ~VaOverlay() override;
   VaOverlay(const VaOverlay& aSrc);

   bool AttachViewer(VaViewer* aViewerPtr);

   virtual UtoShape* FindShape(const std::string& aName) const;
   virtual UtoShape* BindShape(const std::string& aName, const UtoShape& aShape);
   virtual void      UnbindShape(const std::string& aName);

private:
   VaOverlay(); // Disable use.

   bool IsVisibleWindow() const;
   void SetVisibleWindow(bool aVisible);

   bool IsVisibleWorld() const;
   void SetVisibleWorld(bool aVisible);

   UtoShape* FindShapeWindow(const std::string& aName) const;
   UtoShape* BindShapeWindow(const std::string& aName, const UtoShape& aShape);
   void      UnbindShapeWindow(const std::string& aName);

   UtoShape* FindShapeWorld(const std::string& aName) const;
   UtoShape* BindShapeWorld(const std::string& aName, const UtoShape& aShape);
   void      UnbindShapeWorld(const std::string& aName);

   VaViewer*    mViewerPtr;
   std::string  mName;
   OverlayType  mOverlayType;
   OverlayScope mOverlayScope;
   bool         mSelectable;
   bool         mUpdateVisibilityHACK;
   bool         mNewVisibilityHACK;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlay)

#endif
