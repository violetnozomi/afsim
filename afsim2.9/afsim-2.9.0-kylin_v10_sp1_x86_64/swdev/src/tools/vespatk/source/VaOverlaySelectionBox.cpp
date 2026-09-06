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

#include "VaOverlaySelectionBox.hpp"

#include <iostream>

#include "UtLog.hpp"
#include "Uto.hpp"
#include "VaCamera.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaSelectionBox.hpp"
#include "VaViewer.hpp"

namespace vespa
{
VaOverlaySelectionBox::VaOverlaySelectionBox()
   : VaOverlay("selection_box")
   , mSelectionLinePtr(nullptr)
   , mSelectionBoxPtr(nullptr)
   , mBoxX1(0)
   , mBoxY1(0)
   , mBoxX2(0)
   , mBoxY2(0)
   , mBoxPossible(false)
   , mBoxActive(false)
   , mBoxFirst(false)
   , mBoxLastBuf(0)
{
   SetType<VaOverlaySelectionBox>();
   mBoxLastX2[0] = 0;
   mBoxLastX2[1] = 0;
   mBoxLastY2[0] = 0;
   mBoxLastY2[1] = 0;
}

VaOverlaySelectionBox::VaOverlaySelectionBox(const std::string& aName)
   : VaOverlay(aName)
   , mSelectionLinePtr(nullptr)
   , mSelectionBoxPtr(nullptr)
   , mBoxX1(0)
   , mBoxY1(0)
   , mBoxX2(0)
   , mBoxY2(0)
   , mBoxPossible(false)
   , mBoxActive(false)
   , mBoxFirst(false)
   , mBoxLastBuf(0)
{
   SetType<VaOverlaySelectionBox>();
   mBoxLastX2[0] = 0;
   mBoxLastX2[1] = 0;
   mBoxLastY2[0] = 0;
   mBoxLastY2[1] = 0;
}

VaOverlaySelectionBox::VaOverlaySelectionBox(const VaOverlaySelectionBox& aSrc)
   : VaOverlay(aSrc)
   , mSelectionLinePtr(nullptr)
   , mSelectionBoxPtr(nullptr)
   , mBoxX1(aSrc.mBoxX1)
   , mBoxY1(aSrc.mBoxY1)
   , mBoxX2(aSrc.mBoxX2)
   , mBoxY2(aSrc.mBoxY2)
   , mBoxPossible(aSrc.mBoxPossible)
   , mBoxActive(aSrc.mBoxActive)
   , mBoxFirst(aSrc.mBoxFirst)
   , mBoxLastBuf(aSrc.mBoxLastBuf)
{
   SetType<VaOverlaySelectionBox>();
   mBoxLastX2[0] = aSrc.mBoxLastX2[0];
   mBoxLastX2[1] = aSrc.mBoxLastX2[1];
   mBoxLastY2[0] = aSrc.mBoxLastY2[0];
   mBoxLastY2[1] = aSrc.mBoxLastY2[1];
}

VaOverlaySelectionBox::~VaOverlaySelectionBox()
{
   UnbindShape("selection_line_shape");
   UnbindShape("selection_box_shape");
}

VaOverlaySelectionBox* VaOverlaySelectionBox::Clone() const
{
   return new VaOverlaySelectionBox(*this);
}

bool VaOverlaySelectionBox::Initialize()
{
   bool initialized = false;

   UtoPolyLineShape tempSelectionLine;
   UtoPolygonShape  tempSelectionBox;
   UtoColor         color(35, 31, 32, 255);
   tempSelectionLine.SetColor(color);
   color.SetAlpha(64);
   tempSelectionBox.SetColor(color);
   tempSelectionLine.SetWidth(2.0);
   mSelectionLinePtr = dynamic_cast<UtoPolyLineShape*>(BindShape("selection_line_shape", tempSelectionLine));
   mSelectionBoxPtr  = dynamic_cast<UtoPolygonShape*>(BindShape("selection_box_shape", tempSelectionBox));
   if (mSelectionLinePtr && mSelectionBoxPtr)
   {
      initialized = true;
   }
   else
   {
      ut::log::warning() << "Could not bind shape: performance";
   }

   return initialized;
}

void VaOverlaySelectionBox::Update()
{
   if (mBoxActive)
   {
      // Erase the old box
      BoxDraw(mBoxX1, mBoxY1, mBoxLastX2[mBoxLastBuf], mBoxLastY2[mBoxLastBuf]);

      // Draw the new box
      mBoxLastX2[mBoxLastBuf] = mBoxX2;
      mBoxLastY2[mBoxLastBuf] = mBoxY2;
      BoxDraw(mBoxX1, mBoxY1, mBoxLastX2[mBoxLastBuf], mBoxLastY2[mBoxLastBuf]);

      mBoxLastBuf ^= 1; // swap buffer
   }
}

bool VaOverlaySelectionBox::Btn1Down(int aMouseX, int aMouseY, int aState)
{
   mBoxPossible = false;
   mBoxActive   = false;

   const int state = aState & (~VaViewer::CapsNumLockStates);

   if (state == (VaViewer::CtrlKey | VaViewer::ShiftKey))
   {
      // Possible start of Box operation
      mBoxPossible = true;
      mBoxX1       = aMouseX;
      mBoxY1       = aMouseY;
      VaObserver::BeginSelectionChange(GetViewer());
   }
   return false;
}

bool VaOverlaySelectionBox::Btn1Drag(int aMouseX, int aMouseY, int aState)
{
   if (mBoxPossible)
   {
      // Note: We do not actually draw the line here. We just set up the parameters and queue up a redraw.
      if (!mBoxActive)
      {
         mBoxActive    = true;
         mBoxFirst     = true;
         mBoxLastBuf   = 0;
         mBoxLastX2[0] = mBoxLastX2[1] = mBoxX1;
         mBoxLastY2[0] = mBoxLastY2[1] = mBoxY1;

         SetVisible(true);
      }
      mBoxX2 = aMouseX;
      mBoxY2 = aMouseY;
   }
   return false;
}

bool VaOverlaySelectionBox::Btn1Up(int aMouseX, int aMouseY, int aState)
{
   if (mBoxActive)
   {
      BoxEnd();
      VaObserver::EndSelectionChange(GetViewer());
   }
   mBoxPossible = false;
   mBoxActive   = false;

   return false;
}

bool VaOverlaySelectionBox::Btn3Down(int aMouseX, int aMouseY, int aState)
{
   mSelectionBoxPtr->Clear();
   SetVisible(false);
   mBoxPossible = false;
   mBoxActive   = false;

   return false;
}

void VaOverlaySelectionBox::BoxEnd()
{
   // Normalize the box coordinates (X1,Y1 should be the lower left corner)

   int temp;
   if (mBoxX2 < mBoxX1)
   {
      temp   = mBoxX1;
      mBoxX1 = mBoxX2;
      mBoxX2 = temp;
   }
   if (mBoxY2 < mBoxY1)
   {
      temp   = mBoxY1;
      mBoxY1 = mBoxY2;
      mBoxY2 = temp;
   }

   // the comments after this should move to the application in response to the boxselect observer message

   // Convert the middle point of the box to world coordinates.  We don't
   // do anything unless the midpoint is on the map...

   int    midX = (mBoxX1 + mBoxX2) / 2;
   int    midY = (mBoxY1 + mBoxY2) / 2;
   double midLat, midLon, midAlt;
   double lat1, lon1, alt1;
   double lat2, lon2, alt2;
   double lat3, lon3, alt3;
   double lat4, lon4, alt4;
   if (GetViewer()->GetCamera()->ConvertScreenToLLA(midX, midY, midLat, midLon, midAlt))
   {
      GetViewer()->GetCamera()->ConvertScreenToLLA(mBoxX1, mBoxY1, lat1, lon1, alt1);
      GetViewer()->GetCamera()->ConvertScreenToLLA(mBoxX1, mBoxY2, lat2, lon2, alt2);
      GetViewer()->GetCamera()->ConvertScreenToLLA(mBoxX2, mBoxY2, lat3, lon3, alt3);
      GetViewer()->GetCamera()->ConvertScreenToLLA(mBoxX2, mBoxY1, lat4, lon4, alt4);
      VaSelectionBox selectionBox(lat1, lon1, lat2, lon2, lat3, lon3, lat4, lon4);

      VaObserver::HandleSelectionBox(GetViewer(), selectionBox);
   }
   mSelectionLinePtr->Clear();

   SetVisible(false);
}

void VaOverlaySelectionBox::BoxDraw(int aX1, int aY1, int aX2, int aY2)
{
   if ((aX1 != aX2) || (aY1 != aY2))
   {
      mSelectionLinePtr->Clear();
      mSelectionLinePtr->Insert(0, UtoPosition(aX1, aY1, 0));
      mSelectionLinePtr->Insert(1, UtoPosition(aX2, aY1, 0));
      mSelectionLinePtr->Insert(2, UtoPosition(aX2, aY2, 0));
      mSelectionLinePtr->Insert(3, UtoPosition(aX1, aY2, 0));
      mSelectionLinePtr->Insert(4, UtoPosition(aX1, aY1, 0));

      mSelectionBoxPtr->Clear();
      mSelectionBoxPtr->Insert(0, UtoPosition(aX1, aY1, 0));
      mSelectionBoxPtr->Insert(1, UtoPosition(aX2, aY1, 0));
      mSelectionBoxPtr->Insert(2, UtoPosition(aX2, aY2, 0));
      mSelectionBoxPtr->Insert(3, UtoPosition(aX1, aY2, 0));
   }
}
} // end namespace vespa
