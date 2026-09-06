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

#include "VaOverlayTextBox.hpp"

#include <iostream>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "Uto.hpp"
#include "UtoTextShape.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaPosition.hpp"
#include "VaUtils.hpp"
#include "VaViewer.hpp"

namespace vespa
{
VaOverlayTextBox::VaOverlayTextBox()
   : VaOverlayWindow("textbox_overlay", 100, 60, 950, 575)
   , mTextPtr(nullptr)
   , mAlignment(ALIGN_LEFT | ALIGN_BOTTOM)
   , mAlignmentPreDrag(0)
   , mUseAlignmentAsOrigin(false)
   , mTextBoxPositionX(0)
   , mTextBoxPositionY(0)
   , mTextBoxWidth(10)
   , mTextBoxHeight(10)
   , mMinWidth(0)
   , mMinHeight(0)
   , mTextSize(8)
   , mCallbackId(0)
{
   SetType<VaOverlayTextBox>();
}

VaOverlayTextBox::VaOverlayTextBox(const std::string& aName)
   : VaOverlayWindow(aName, 100, 60, 950, 575)
   , mTextPtr(nullptr)
   , mAlignment(ALIGN_LEFT | ALIGN_BOTTOM)
   , mAlignmentPreDrag(0)
   , mUseAlignmentAsOrigin(false)
   , mTextBoxPositionX(0)
   , mTextBoxPositionY(0)
   , mTextBoxWidth(10)
   , mTextBoxHeight(10)
   , mMinWidth(0)
   , mMinHeight(0)
   , mTextSize(8)
   , mCallbackId(0)
{
   SetType<VaOverlayTextBox>();
}

VaOverlayTextBox::VaOverlayTextBox(const VaOverlayTextBox& aSrc)
   : VaOverlayWindow(aSrc)
   , mTextPtr(nullptr)
   , mAlignment(ALIGN_LEFT | ALIGN_BOTTOM)
   , mAlignmentPreDrag(0)
   , mUseAlignmentAsOrigin(false)
   , mTextBoxPositionX(0)
   , mTextBoxPositionY(0)
   , mTextBoxWidth(10)
   , mTextBoxHeight(10)
   , mMinWidth(0)
   , mMinHeight(0)
   , mTextSize(8)
   , mCallbackId(0)
{
   SetType<VaOverlayTextBox>();
}

VaOverlayTextBox::~VaOverlayTextBox()
{
   UnbindShape("text");
}

VaOverlayTextBox* VaOverlayTextBox::Clone() const
{
   return new VaOverlayTextBox(*this);
}

bool VaOverlayTextBox::Initialize()
{
   SetFrameColor(UtColor(1.0f, 1.0f, 1.0f, 1.0f));
   SetBackgroundColor(UtColor(0.3f, 0.3f, 0.3f, 0.75f));
   VaOverlayWindow::Initialize();

   bool initialized = false;
   try
   {
      UtoTextShape text;
      text.SetColor(UtoColor(0, 255, 0));
      text.SetBackdropColor(UtoColor(0, 0, 0));
      text.SetPosition(UtoPosition(5, 5, 0));
      text.SetSize(mTextSize);
      text.SetAlignment(UtoTextShape::BOTTOM | UtoTextShape::LEFT);
      mTextPtr = dynamic_cast<UtoTextShape*>(BindShape("text", text));
      if (mTextPtr == nullptr)
      {
         ut::log::warning() << "Could not bind shape: text";
      }

      if (mTextPtr != nullptr)
      {
         SetVisible(true);
         initialized = true;
      }
   }
   catch (Utok::no_resource& res)
   {
      auto out = ut::log::warning() << "Resource not found.";
      out.AddNote() << "Resource: " << res.what();
   }

   return initialized;
}

void VaOverlayTextBox::Update() {}

// virtual
void VaOverlayTextBox::SetText(const std::string& aString)
{
   if (mTextPtr == nullptr)
   {
      Initialize();
   }
   if (mTextPtr != nullptr)
   {
      if (mString != aString)
      {
         mString = aString;
         mTextPtr->SetString(aString);
         UpdateWindowSize();
      }
   }
}

// virtual
void VaOverlayTextBox::SetTextColor(const UtColor& aColor)
{
   if (mTextPtr == nullptr)
   {
      Initialize();
   }
   if (mTextPtr != nullptr)
   {
      mTextPtr->SetColor(VaUtils::glToUtoColor(aColor));
   }
   VaOverlayWindow::SetTextColor(aColor); // set title color for window
}

// virtual
void VaOverlayTextBox::SetTextSize(int aSize)
{
   if (mTextPtr == nullptr)
   {
      Initialize();
   }
   if (mTextPtr != nullptr)
   {
      mTextSize = aSize;
      mTextPtr->SetSize(aSize);
      UpdateWindowSize();
   }
}

void VaOverlayTextBox::SetAlignment(int aAlignment, bool aUseAsOrigin)
{
   mAlignment            = aAlignment;
   mUseAlignmentAsOrigin = aUseAsOrigin;
   if (aUseAsOrigin)
   {
      if (mCallbackId == 0)
      {
         mCallbackId = mCallbacks.Add(
            VaObserver::ViewerResized.Connect(&VaOverlayTextBox::ViewerResizedCB, this, GetViewer()->GetUniqueId()));
      }
   }
   else
   {
      if (mCallbackId != 0)
      {
         mCallbacks.Remove(mCallbackId);
         mCallbackId = 0;
      }
   }
   RepositionWindowPrivate();
}


void VaOverlayTextBox::ViewerResizedCB(VaViewer* aViewerPtr)
{
   RepositionWindowPrivate();
}

void VaOverlayTextBox::RepositionWindow(int aX, int aY)
{
   mTextBoxPositionX = aX;
   mTextBoxPositionY = aY;
   RepositionWindowPrivate();
}

void VaOverlayTextBox::ResizeWindow(int aW, int aH)
{
   mTextBoxWidth  = aW;
   mTextBoxHeight = aH;
   RepositionWindowPrivate();
}

void VaOverlayTextBox::RepositionWindowPrivate()
{
   int x = mTextBoxPositionX;
   int y = mTextBoxPositionY;

   if (mAlignment > 0) // if text box is aligned to viewer, adjust x & y by line width of window border
   {
      double lw = dynamic_cast<UtoPolyLineShape*>(FindShape("WindowBorder"))->Width();
      x += mAlignment & ALIGN_RIGHT ? -lw : lw;
      y += mAlignment & ALIGN_TOP ? -lw : lw;
   }

   if (mAlignment & ALIGN_RIGHT)
   {
      if (mUseAlignmentAsOrigin)
      {
         x = GetViewer()->GetWidth() + x - mTextBoxWidth;
      }
      else
      {
         x -= mTextBoxWidth;
      }
   }
   else if (mAlignment & ALIGN_HCENTER)
   {
      if (mUseAlignmentAsOrigin)
      {
         x = (int)(GetViewer()->GetWidth() * 0.5) + x - (int)(0.5 * mTextBoxWidth);
      }
      else
      {
         x -= (int)(0.5 * mTextBoxWidth);
      }
   }
   if (mAlignment & ALIGN_TOP)
   {
      if (mUseAlignmentAsOrigin)
      {
         y = GetViewer()->GetHeight() + y - mTextBoxHeight;
      }
      else
      {
         y -= mTextBoxHeight;
      }
   }
   else if (mAlignment & ALIGN_VCENTER)
   {
      if (mUseAlignmentAsOrigin)
      {
         y = (int)(GetViewer()->GetHeight() * 0.5) + y - (int)(0.5 * mTextBoxHeight);
      }
      else
      {
         y -= (int)(0.5 * mTextBoxHeight);
      }
   }

   VaOverlayWindow::RepositionWindow(x, y);
   VaOverlayWindow::ResizeWindow(mTextBoxWidth, mTextBoxHeight);
}

void VaOverlayTextBox::UpdateWindowSize()
{
   // modified to use the text object to resize
   float width  = 0.0;
   float height = 0.0;
   mTextPtr->GetBounds(width, height);
   width += 2 * GetCornerRadius();
   height += 2 * GetCornerRadius();
   if (width < mMinWidth)
   {
      width = mMinWidth;
   }
   if (height < mMinHeight)
   {
      height = mMinHeight;
   }
   ResizeWindow(width, height);
}

void VaOverlayTextBox::SetMinSize(int aW, int aH)
{
   mMinWidth  = aW;
   mMinHeight = aH;

   SetText(mString);
}

bool VaOverlayTextBox::Btn1Drag(int aMouseX, int aMouseY, int aState)
{
   // If window is dragged, it can no longer be aligned to viewer extents
   if (!Selectable() || !Draggable())
   {
      return false;
   }

   if (mAlignment > 0)
   {
      mAlignmentPreDrag = mAlignment;
      mAlignment        = 0;
   }

   return VaOverlayWindow::Btn1Drag(aMouseX, aMouseY, aState);
}

bool VaOverlayTextBox::Btn1DblClick(int aMouseX, int aMouseY, int aState)
{
   if (!Selectable() || !Draggable())
   {
      return false;
   }

   if (mAlignmentPreDrag > 0)
   {
      mAlignment        = mAlignmentPreDrag;
      mAlignmentPreDrag = 0;
      RepositionWindow(0, 0);
      return true;
   }

   return VaOverlayWindow::Btn1DblClick(aMouseX, aMouseY, aState);
}

} // namespace vespa
