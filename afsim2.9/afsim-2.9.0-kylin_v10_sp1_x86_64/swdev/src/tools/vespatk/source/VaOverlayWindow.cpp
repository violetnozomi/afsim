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

#include "VaOverlayWindow.hpp"

#include <iostream>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "Uto.hpp"
#include "UtoGroupShape.hpp"
#include "UtoTextShape.hpp"
#include "VaUtils.hpp"

namespace vespa
{
VaOverlayWindow::VaOverlayWindow(const std::string& aName, int aX, int aY, int aW, int aH)
   : VaOverlay(aName)
   , mBackgroundColor(1.0f, 1.0f, 1.0f, 0.4f)
   , mFrameColor(1.0f, 1.0f, 1.0f, 1.0f)
   , mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
   , mX(aX)
   , mY(aY)
   , mW(aW)
   , mH(aH)
   , mMouseX(-2000)
   , mMouseY(-2000)
   , mCornerRadius(5.0f)
   , mCurveSampling(32.0f)
   , mTitleString("")
{
   SetType<VaOverlayWindow>();
}

VaOverlayWindow::VaOverlayWindow(int aX, int aY, int aW, int aH)
   : VaOverlay("window")
   , mBackgroundColor(1.0f, 1.0f, 1.0f, 0.4f)
   , mFrameColor(1.0f, 1.0f, 1.0f, 1.0f)
   , mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
   , mX(aX)
   , mY(aY)
   , mW(aW)
   , mH(aH)
   , mDraggable(false)
   , mMouseX(-2000)
   , mMouseY(-2000)
   , mCornerRadius(5.0f)
   , mCurveSampling(32.0f)
   , mTitleString("")
{
   SetType<VaOverlayWindow>();
}

VaOverlayWindow::VaOverlayWindow(const VaOverlayWindow& aSrc)
   : VaOverlay(aSrc)
   , mBackgroundColor(aSrc.mBackgroundColor)
   , mFrameColor(aSrc.mFrameColor)
   , mTextColor(aSrc.mTextColor)
   , mX(aSrc.mX)
   , mY(aSrc.mY)
   , mW(aSrc.mW)
   , mH(aSrc.mH)
   , mDraggable(aSrc.mDraggable)
   , mMouseX(-2000)
   , mMouseY(-2000)
   , mCornerRadius(aSrc.mCornerRadius)
   , mCurveSampling(aSrc.mCurveSampling)
   , mTitleString(aSrc.mTitleString)
{
   SetType<VaOverlayWindow>();
}

VaOverlayWindow::~VaOverlayWindow()
{
   UnbindShape("WindowBackDrop");
   UnbindShape("WindowBorder");
   VaOverlay::UnbindShape(mWindowShapeName); // this doesn't unbind all children?
}

VaOverlayWindow* VaOverlayWindow::Clone() const
{
   return new VaOverlayWindow(*this);
}

// virtual
UtoShape* VaOverlayWindow::FindShape(const std::string& aName) const
{
   UtoGroupShape* gsPtr = dynamic_cast<UtoGroupShape*>(VaOverlay::FindShape(mWindowShapeName));
   if (gsPtr != nullptr)
   {
      return gsPtr->FindShape(aName);
   }
   return nullptr;
}

// virtual
UtoShape* VaOverlayWindow::BindShape(const std::string& aName, const UtoShape& aShape)
{
   UtoGroupShape* gsPtr = dynamic_cast<UtoGroupShape*>(VaOverlay::FindShape(mWindowShapeName));
   if (gsPtr != nullptr)
   {
      return gsPtr->Bind(aName, aShape);
      ;
   }
   return nullptr;
}

// virtual
void VaOverlayWindow::UnbindShape(const std::string& aName)
{
   UtoGroupShape* gsPtr = dynamic_cast<UtoGroupShape*>(VaOverlay::FindShape(mWindowShapeName));
   if (gsPtr != nullptr)
   {
      gsPtr->Unbind(aName);
   }
}

bool VaOverlayWindow::Initialize()
{
   bool initialized = false;
   try
   {
      std::ostringstream oss;
      oss << "window_" << GetUniqueId();
      mWindowShapeName = oss.str();
      UtoGroupShape  groupShape;
      UtoGroupShape* gsPtr = (UtoGroupShape*)(VaOverlay::BindShape(mWindowShapeName, groupShape));
      gsPtr->SetPosition(UtoPosition(mX, mY, 0));

      UtoPolygonShape  backdrop;
      UtoPolyLineShape outline;
      backdrop.SetColor(VaUtils::glToUtoColor(mBackgroundColor));
      outline.SetColor(VaUtils::glToUtoColor(mFrameColor));
      outline.SetAggregation(UtoPolyLineShape::LINE_LOOP);
      outline.SetWidth(2.0);
      BuildWindowFrame(mW, mH, mCornerRadius, mCurveSampling, outline, backdrop);

      BindShape("WindowBackDrop", backdrop);
      BindShape("WindowBorder", outline);

      if (mTitleString != "")
      {
         SetTitle(mTitleString);
      }

      SetVisible(true);
      initialized = true;
   }
   catch (Utok::no_resource& res)
   {
      auto out = ut::log::warning() << "Resource not found.";
      out.AddNote() << "Resource: " << res.what();
   }
   return initialized;
}


void VaOverlayWindow::BuildWindowFrame(int               aWidth,
                                       int               aHeight,
                                       float             aCornerRadius,
                                       float             aCurveSampling,
                                       UtoPolyLineShape& aOutline,
                                       UtoPolygonShape&  aBackground)
{
   int   i;
   int   idx = 0;
   float a;
   for (i = 0; i < aCurveSampling; ++i)
   {
      a = i / (float)aCurveSampling * UtMath::cTWO_PI;

      if (i <= aCurveSampling / 4)
      {
         UtoPosition temp(aCornerRadius * sin(a) + aWidth - aCornerRadius,
                          aCornerRadius * cos(a) + aHeight - aCornerRadius,
                          0);
         aOutline.Insert(idx, temp);
         aBackground.Insert(idx++, temp);
      }
      if ((i >= aCurveSampling / 4) && (i <= aCurveSampling / 2))
      {
         UtoPosition temp(aCornerRadius * sin(a) + aWidth - aCornerRadius, aCornerRadius * cos(a) + aCornerRadius, 0);
         aOutline.Insert(idx, temp);
         aBackground.Insert(idx++, temp);
      }
      if ((i >= aCurveSampling / 2) && (i <= 3 * aCurveSampling / 4))
      {
         UtoPosition temp(aCornerRadius * sin(a) + aCornerRadius, aCornerRadius * cos(a) + aCornerRadius, 0);
         aOutline.Insert(idx, temp);
         aBackground.Insert(idx++, temp);
      }
      if (i >= 3 * aCurveSampling / 4)
      {
         UtoPosition temp(aCornerRadius * sin(a) + aCornerRadius, aCornerRadius * cos(a) + aHeight - aCornerRadius, 0);
         aOutline.Insert(idx, temp);
         aBackground.Insert(idx++, temp);
      }
   }
}

void VaOverlayWindow::Update()
{
   VaOverlay::Update();
}

void VaOverlayWindow::SetBackgroundColor(const UtColor& aColor)
{
   UtoPolygonShape* shape = dynamic_cast<UtoPolygonShape*>(FindShape("WindowBackDrop"));
   if (shape != nullptr)
   {
      shape->SetColor(VaUtils::glToUtoColor(aColor));
   }
   mBackgroundColor = aColor;
}

void VaOverlayWindow::SetFrameColor(const UtColor& aColor)
{
   UtoPolyLineShape* shape = dynamic_cast<UtoPolyLineShape*>(FindShape("WindowBorder"));
   if (shape != nullptr)
   {
      shape->SetColor(VaUtils::glToUtoColor(aColor));
   }
   mFrameColor = aColor;
}

void VaOverlayWindow::SetTextColor(const UtColor& aColor)
{
   UtoTextShape* title = dynamic_cast<UtoTextShape*>(FindShape("WindowTitle"));
   if (title != nullptr)
   {
      title->SetColor(VaUtils::glToUtoColor(aColor));
   }
   mTextColor = aColor;
}

void VaOverlayWindow::RepositionWindow(int aX, int aY)
{
   UtoGroupShape* gsPtr = dynamic_cast<UtoGroupShape*>(VaOverlay::FindShape(mWindowShapeName));
   if (gsPtr != nullptr)
   {
      gsPtr->SetPosition(UtoPosition(aX, aY, 0));
   }

   mX = aX;
   mY = aY;
}

void VaOverlayWindow::ResizeWindow(int aW, int aH)
{
   UtoPolyLineShape* plshape = dynamic_cast<UtoPolyLineShape*>(FindShape("WindowBorder"));
   UtoPolygonShape*  pgshape = dynamic_cast<UtoPolygonShape*>(FindShape("WindowBackDrop"));
   if ((pgshape != nullptr) && (plshape != nullptr))
   {
      pgshape->Clear();
      plshape->Clear();

      BuildWindowFrame(aW, aH, mCornerRadius, mCurveSampling, *plshape, *pgshape);
      mW = aW;
      mH = aH;
   }
   UtoTextShape* title = dynamic_cast<UtoTextShape*>(FindShape("WindowTitle"));
   if (title != nullptr)
   {
      title->SetPosition(UtoPosition(mW / 2, mH - 4, 0));
   }
}

void VaOverlayWindow::SetCornerRadius(float aRadius)
{
   mCornerRadius = aRadius;

   UtoPolyLineShape* plshape = dynamic_cast<UtoPolyLineShape*>(FindShape("WindowBorder"));
   UtoPolygonShape*  pgshape = dynamic_cast<UtoPolygonShape*>(FindShape("WindowBackDrop"));
   if ((pgshape != nullptr) && (plshape != nullptr))
   {
      pgshape->Clear();
      plshape->Clear();

      BuildWindowFrame(mW, mH, mCornerRadius, mCurveSampling, *plshape, *pgshape);
   }
}

void VaOverlayWindow::SetTitle(const std::string& aTitle)
{
   UtoTextShape* titleShape = dynamic_cast<UtoTextShape*>(FindShape("WindowTitle"));
   if (titleShape == nullptr)
   {
      UtoTextShape protoTitle(aTitle);
      protoTitle.SetColor(VaUtils::glToUtoColor(mTextColor));
      protoTitle.SetPosition(UtoPosition(mW / 2, mH - 4, 0));
      protoTitle.SetSize(8);
      protoTitle.SetAlignment(UtoTextShape::TOP | UtoTextShape::CENTER);

      BindShape("WindowTitle", protoTitle);
   }
   else
   {
      titleShape->SetString(aTitle);
   }
   mTitleString = aTitle;
}

bool VaOverlayWindow::SelectableAt(int aMouseX, int aMouseY) const
{
   if (!Selectable())
   {
      return false;
   }
   if ((aMouseX > mX) && (aMouseY > mY) && (aMouseX < mX + mW) && (aMouseY < mY + mH))
   {
      return true;
   }
   return false;
}

bool VaOverlayWindow::Btn1Click(int aMouseX, int aMouseY, int aState)
{
   if (SelectableAt(aMouseX, aMouseY))
   {
      mMouseX = -2000;
      mMouseY = -2000;
      return true;
   }
   return false;
}

bool VaOverlayWindow::Btn1DblClick(int aMouseX, int aMouseY, int aState)
{
   if (SelectableAt(aMouseX, aMouseY))
   {
      mMouseX = -2000;
      mMouseY = -2000;
      return true;
   }
   return false;
}

bool VaOverlayWindow::Btn1Down(int aMouseX, int aMouseY, int aState)
{
   if (SelectableAt(aMouseX, aMouseY))
   {
      mMouseX = aMouseX;
      mMouseY = aMouseY;
      return true;
   }
   return false;
}

bool VaOverlayWindow::Btn1Drag(int aMouseX, int aMouseY, int aState)
{
   if ((!Selectable()) || (!mDraggable))
   {
      return false;
   }

   if ((mMouseX > -1000) && (mMouseY > -1000))
   {
      RepositionWindow(mX + aMouseX - mMouseX, mY + aMouseY - mMouseY);
   }
   mMouseX = aMouseX;
   mMouseY = aMouseY;
   return true;
}

bool VaOverlayWindow::Btn1Up(int aMouseX, int aMouseY, int aState)
{
   if (!Selectable())
   {
      return false;
   }
   mMouseX = -2000;
   mMouseY = -2000;
   return false;
}
} // namespace vespa
