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

#include "VaOverlayText.hpp"

#include <iostream>
#include <string>

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
VaOverlayText::VaOverlayText()
   : VaOverlay("text_overlay")
   , mTextPtr(nullptr)
{
   SetType<VaOverlayText>();
}

VaOverlayText::VaOverlayText(const std::string& aName)
   : VaOverlay(aName)
   , mTextPtr(nullptr)
{
   SetType<VaOverlayText>();
}

VaOverlayText::VaOverlayText(const VaOverlayText& aSrc)
   : VaOverlay(aSrc)
   , mTextPtr(nullptr)
{
   SetType<VaOverlayText>();
}

VaOverlayText::~VaOverlayText()
{
   UnbindShape("text");
}

VaOverlayText* VaOverlayText::Clone() const
{
   return new VaOverlayText(*this);
}

bool VaOverlayText::Initialize()
{
   bool initialized = false;
   try
   {
      UtoTextShape text("No Data");
      text.SetColor(UtoColor(0, 255, 0));
      text.SetPosition(UtoPosition(10, 10, 0));
      text.SetSize(8);
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

void VaOverlayText::Update() {}

void VaOverlayText::SetPosition(const UtoPosition& aPosition)
{
   mTextPtr->SetPosition(aPosition);
}

void VaOverlayText::SetText(const std::string& aText)
{
   mTextPtr->SetString(aText);
}

void VaOverlayText::SetText(const std::wstring& aText)
{
   mTextPtr->SetString(aText);
}

void VaOverlayText::SetColor(const UtColor& aColor)
{
   mTextPtr->SetColor(VaUtils::glToUtoColor(aColor));
}

void VaOverlayText::SetColor(const UtoColor& aColor)
{
   mTextPtr->SetColor(aColor);
}

void VaOverlayText::SetSize(const int& aSize)
{
   mTextPtr->SetSize(aSize);
}
} // namespace vespa
