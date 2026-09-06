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

#include "VaOverlayCompass.hpp"

#include <iostream>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "Uto.hpp"
#include "VaCamera.hpp"
#include "VaPosition.hpp"
#include "VaViewer.hpp"

namespace vespa
{
VaOverlayCompass::VaOverlayCompass()
   : VaOverlay("compass_overlay")
   , mCompassPtr(nullptr)
{
   SetType<VaOverlayCompass>();
}

VaOverlayCompass::VaOverlayCompass(const std::string& aName)
   : VaOverlay(aName)
   , mCompassPtr(nullptr)
{
   SetType<VaOverlayCompass>();
}

VaOverlayCompass::VaOverlayCompass(const VaOverlayCompass& aSrc)
   : VaOverlay(aSrc)
   , mCompassPtr(nullptr)
{
   SetType<VaOverlayCompass>();
}

VaOverlayCompass::~VaOverlayCompass()
{
   UnbindShape("compass");
}

VaOverlayCompass* VaOverlayCompass::Clone() const
{
   return new VaOverlayCompass(*this);
}

bool VaOverlayCompass::Initialize()
{
   bool initialized = false;
   try
   {
      UtoModelShape compass(UtoResDBLookup<UtoModelShape>("Compass"));
      mCompassPtr = dynamic_cast<UtoModelShape*>(BindShape("compass", compass));
      if (mCompassPtr == nullptr)
      {
         ut::log::warning() << "Could not bind shape: compass";
      }

      if (mCompassPtr != nullptr)
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

void VaOverlayCompass::Update()
{
   if (IsVisible() && (mCompassPtr != nullptr))
   {
      const UtoPosition& pos  = mCompassPtr->Position();
      int                posx = GetViewer()->GetWidth() - 55;
      int                posy = GetViewer()->GetHeight() - 55;
      if ((int)pos.x() != posx || (int)pos.y() != posy)
      {
         mCompassPtr->SetPosition(UtoPosition(posx, posy, 0));
      }
      double heading = GetViewer()->GetCamera()->GetGeographicHeading();

      mCompassPtr->SetOrientation(UtoOrientation(-heading, 0.0, 0.0));
   }
}
} // namespace vespa
