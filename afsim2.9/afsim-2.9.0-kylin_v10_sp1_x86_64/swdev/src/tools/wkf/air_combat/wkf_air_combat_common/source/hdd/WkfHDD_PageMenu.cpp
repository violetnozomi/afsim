// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfHDD_PageMenu.hpp"

#include <iomanip>
#include <sstream>

#include <QRect>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WkfHDD_Dockable.hpp"

HDD::PageMenu::PageMenu(vespa::VaViewer* aViewerPtr, const size_t aPosition, Dockable* aParentPtr)
   : Page(aViewerPtr, aPosition, aParentPtr)
{
   std::string overlayName = "hdd_menu" + std::to_string(aPosition);
   mOverlayPtr             = new Overlay(overlayName.c_str(), aViewerPtr->GetWidth(), aViewerPtr->GetHeight());

   // First row
   mButtonMap.emplace(1, TextButton(overlayName, "ASR", aViewerPtr));
   mButtonMap.emplace(2, TextButton(overlayName, "CKLST", aViewerPtr));
   mButtonMap.emplace(3, TextButton(overlayName, "CNI", aViewerPtr));
   mButtonMap.emplace(4, TextButton(overlayName, "DAS", aViewerPtr));
   mButtonMap.emplace(5, TextButton(overlayName, "DIM", aViewerPtr));
   // Second row
   mButtonMap.emplace(6, TextButton(overlayName, "EFI", aViewerPtr));
   mButtonMap.emplace(7, TextButton(overlayName, "ENG", aViewerPtr));
   mButtonMap.emplace(8, TextButton(overlayName, "FCS", aViewerPtr));
   mButtonMap.emplace(9, TextButton(overlayName, "FTA", aViewerPtr));
   mButtonMap.emplace(10, TextButton(overlayName, "FTI", aViewerPtr));
   // Third row
   mButtonMap.emplace(11, TextButton(overlayName, "FUEL", aViewerPtr));
   mButtonMap.emplace(12, TextButton(overlayName, "HUD", aViewerPtr));
   mButtonMap.emplace(13, TextButton(overlayName, "ICAWS", aViewerPtr));
   mButtonMap.emplace(14, TextButton(overlayName, "PWM", aViewerPtr));
   mButtonMap.emplace(15, TextButton(overlayName, "SMS", aViewerPtr));
   // Fourth row
   mButtonMap.emplace(16, TextButton(overlayName, "SACH", aViewerPtr));
   mButtonMap.emplace(17, TextButton(overlayName, "TFLIR", aViewerPtr));
   mButtonMap.emplace(18, TextButton(overlayName, "TSD-1", aViewerPtr));
   mButtonMap.emplace(19, TextButton(overlayName, "TSD-2", aViewerPtr));
   mButtonMap.emplace(20, TextButton(overlayName, "TSD-3", aViewerPtr));
   // Fifth row
   mButtonMap.emplace(21, TextButton(overlayName, "TWD", aViewerPtr));
   mButtonMap.emplace(22, TextButton(overlayName, "WPN-A", aViewerPtr));
   mButtonMap.emplace(23, TextButton(overlayName, "WPN-S", aViewerPtr));

   aViewerPtr->AddOverlay(mOverlayPtr);
   SetPosition(mPosition);
   mOverlayPtr->SetVisible(false);
   mOverlayPtr->SetDrawFunction(std::bind(&PageMenu::Draw, this));
   mOverlayPtr->SetName("Menu" + std::to_string(aPosition));

   // Set the button click action for each button
   for (auto& button : mButtonMap)
   {
      button.second.SetVisible(false);
      button.second.SetDrawFunction(std::bind(&TextButton::Draw, std::ref(button.second)));
      button.second.SetButton1ClickAction(
         [this, button, aParentPtr](int x, int y, int s) -> bool
         {
            if (button.second.IsVisible())
            {
               aParentPtr->MenuPageSelected(this->GetPosition(), button.second.GetText());
            }
            return false;
         });
   }
}

HDD::PageMenu::~PageMenu() = default;

void HDD::PageMenu::Draw()
{
   mOverlayPtr->DrawBackground(Color::Black);
   mOverlayPtr->DrawOutline(Color::Gray);
}

void HDD::PageMenu::SetPosition(const size_t aPosition)
{
   Page::SetPosition(aPosition);

   // Center position of the page in viewer coords
   double x = mRect.center().x() * mViewerPtr->GetWidth();
   double y = mRect.center().y() * mViewerPtr->GetHeight();

   // Width and height of the page overlay
   double w = static_cast<float>(mOverlayPtr->GetWidth());
   double h = static_cast<float>(mOverlayPtr->GetHeight());

   // Width and height of the button
   double bW = 0.14 * w;
   double bH = 0.1 * h * mRatio;

   // Set all of the button positions
   mButtonMap.at(1).SetPosition(QRectF(x + (-0.4 * w) - (bW * 0.5), y + (0.3 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(2).SetPosition(QRectF(x + (-0.2 * w) - (bW * 0.5), y + (0.3 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(3).SetPosition(QRectF(x + (0.0 * w) - (bW * 0.5), y + (0.3 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(4).SetPosition(QRectF(x + (0.2 * w) - (bW * 0.5), y + (0.3 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(5).SetPosition(QRectF(x + (0.4 * w) - (bW * 0.5), y + (0.3 * h) - (bH * 0.5), bW, bH));

   mButtonMap.at(6).SetPosition(QRectF(x + (-0.4 * w) - (bW * 0.5), y + (0.15 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(7).SetPosition(QRectF(x + (-0.2 * w) - (bW * 0.5), y + (0.15 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(8).SetPosition(QRectF(x + (0.0 * w) - (bW * 0.5), y + (0.15 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(9).SetPosition(QRectF(x + (0.2 * w) - (bW * 0.5), y + (0.15 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(10).SetPosition(QRectF(x + (0.4 * w) - (bW * 0.5), y + (0.15 * h) - (bH * 0.5), bW, bH));

   mButtonMap.at(11).SetPosition(QRectF(x + (-0.4 * w) - (bW * 0.5), y + (0.0 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(12).SetPosition(QRectF(x + (-0.2 * w) - (bW * 0.5), y + (0.0 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(13).SetPosition(QRectF(x + (0.0 * w) - (bW * 0.5), y + (0.0 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(14).SetPosition(QRectF(x + (0.2 * w) - (bW * 0.5), y + (0.0 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(15).SetPosition(QRectF(x + (0.4 * w) - (bW * 0.5), y + (0.0 * h) - (bH * 0.5), bW, bH));

   mButtonMap.at(16).SetPosition(QRectF(x + (-0.4 * w) - (bW * 0.5), y + (-0.15 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(17).SetPosition(QRectF(x + (-0.2 * w) - (bW * 0.5), y + (-0.15 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(18).SetPosition(QRectF(x + (0.0 * w) - (bW * 0.5), y + (-0.15 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(19).SetPosition(QRectF(x + (0.2 * w) - (bW * 0.5), y + (-0.15 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(20).SetPosition(QRectF(x + (0.4 * w) - (bW * 0.5), y + (-0.15 * h) - (bH * 0.5), bW, bH));

   mButtonMap.at(21).SetPosition(QRectF(x + (-0.4 * w) - (bW * 0.5), y + (-0.3 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(22).SetPosition(QRectF(x + (-0.2 * w) - (bW * 0.5), y + (-0.3 * h) - (bH * 0.5), bW, bH));
   mButtonMap.at(23).SetPosition(QRectF(x + (0.0 * w) - (bW * 0.5), y + (-0.3 * h) - (bH * 0.5), bW, bH));
}

void HDD::PageMenu::SetVisible(const bool aState)
{
   mOverlayPtr->SetVisible(aState);

   for (auto&& button : mButtonMap)
   {
      button.second.SetVisible(aState);
   }
}
