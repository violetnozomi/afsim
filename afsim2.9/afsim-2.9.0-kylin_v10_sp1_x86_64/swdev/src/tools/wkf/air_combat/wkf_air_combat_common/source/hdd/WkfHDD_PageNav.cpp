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

#include "WkfHDD_PageNav.hpp"

#include "VaCamera.hpp"
#include "VaOverlayTexture.hpp"
#include "VaUtils.hpp"
#include "VaViewer.hpp"
#include "WkfEnvironment.hpp"
#include "WkfSharedMapPreferencesObject.hpp"

HDD::PageNav::PageNav(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr, vespa::VaEntity* aEntityPtr)
   : PageTex(aViewerPtr, aPosition, aParentPtr)
   , mEntityPtr(nullptr)
   , mZoom(200)
{
   // Create the overlay for drawing over the viewer.
   mOverlayPtr =
      new Overlay("tri", static_cast<float>(aViewerPtr->GetWidth()), static_cast<float>(aViewerPtr->GetHeight()));
   mOverlayPtr->SetDrawFunction(std::bind(&PageNav::Draw, this));
   mOverlayPtr->SetVisible(true);

   GetRect(mPosition, mRect);
   unsigned int resx = mRect.width() * aViewerPtr->GetWidth();
   unsigned int resy = mRect.height() * aViewerPtr->GetHeight();
   mNavViewerPtr     = mOverlayTexPtr->CreateViewerTexture(aViewerPtr, resx, resy);

   mNavViewerPtr->AddOverlay(mOverlayPtr);

   // Position and size must be set after the the overlay has been added to the viewer.
   mOverlayPtr->SetPosition(0.0f, 0.0f);
   mOverlayPtr->SetSize(mNavViewerPtr->GetWidth(), mNavViewerPtr->GetHeight());

   // If this wkfEnv call doesn't occur, all of our overlays will draw horribly.  This sets up a topdown view of
   // our aircraft, on a navigational map.
   wkfEnv.GetPreferenceObject<wkf::SharedMapPreferencesObject>()->SetMapProfile(wkf::SharedMapPreferencesData::NAVIGATIONAL,
                                                                                mNavViewerPtr);
   mNavViewerPtr->GetCamera()->UseOrthographicProjection();
   mNavViewerPtr->GetCamera()->ManageProjection(false);
   mEntityPtr = aEntityPtr;
   if (aEntityPtr)
   {
      mNavViewerPtr->GetCamera()->Tether(aEntityPtr);
   }
   mNavViewerPtr->GetCamera()->GetCameraMotion()->SetEl(90);
   mNavViewerPtr->GetCamera()->GetCameraMotion()->SetAz(0);
   mNavViewerPtr->GetCamera()->GetCameraMotion()->RequestDist(60000);
   mOverlayTexPtr->RecalculateOrthographic(resx, resy, mZoom);

   mOverlayTexPtr->SetMouseWheelAction(
      [this](int x, int y, int d) -> bool
      {
         MouseWheel(x, y, d);
         return false;
      });

   mPageName = "NAV";
}

void HDD::PageNav::Draw()
{
   mOverlayPtr->DrawOutline(Color::Gray);
   DrawTriangle();
}

void HDD::PageNav::DrawTriangle()
{
   double w = static_cast<double>(mOverlayPtr->GetWidth());
   double h = static_cast<double>(mOverlayPtr->GetHeight());

   // Red triangle
   osg::ref_ptr<osg::Vec2Array> triangleArray(new osg::Vec2Array);
   triangleArray->push_back(osg::Vec2d(-0.015 * w, -0.015 * h));
   triangleArray->push_back(osg::Vec2d(0.0 * w, 0.015 * h));
   triangleArray->push_back(osg::Vec2d(0.015 * w, -0.015 * h));
   mOverlayPtr->GetGeode()->addChild(mOverlayPtr->CreateGeometry(triangleArray, GL_LINE_LOOP, Color::Red).get());
}

void HDD::PageNav::MouseWheel(int aX, int aY, int aDelta)
{
   float zf = (mZoom - 10) / 2990;
   zf       = zf * 290 + 10;
   mZoom += aDelta / 120 * zf;
   if (mZoom < 10)
   {
      mZoom = 10;
   }
   if (mZoom > 3000)
   {
      mZoom = 3000;
   }
   GetRect(mPosition, mRect);

   unsigned int resx = mRect.width() * mViewerPtr->GetWidth();
   unsigned int resy = mRect.height() * mViewerPtr->GetHeight();
   mOverlayTexPtr->RecalculateOrthographic(resx, resy, mZoom);
}
