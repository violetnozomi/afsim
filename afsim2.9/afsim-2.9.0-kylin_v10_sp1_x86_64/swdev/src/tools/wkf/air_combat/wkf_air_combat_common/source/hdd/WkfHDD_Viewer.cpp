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

#include "hdd/WkfHDD_Viewer.hpp"

#include "VaCamera.hpp"
#include "VaCameraMotion.hpp"

HDD::Viewer::Viewer(int aViewerType, int aViewerOptions)
   : vespa::VaViewer(aViewerType, aViewerOptions)
{
}

int HDD::Viewer::WheelUp(int aDelta)
{
   return WheelImp(aDelta);
}

int HDD::Viewer::WheelDown(int aDelta)
{
   return WheelImp(aDelta);
}

int HDD::Viewer::WheelImp(int aDelta)
{
   int status = DontPassOn;
   if (WheelOverlays(mMouseX, mMouseY, aDelta))
   {
      return status;
   }

   // if (mInitialized)
   //{
   //    if (GetCamera()->MouseScroll(aDelta))
   //    {
   //       if (mCameraPtr->GetCameraMotion()->GetMotionType() != vespa::VaCameraMotion::cTETHERED)
   //       {
   //          // Start the zoom box animation
   //          ShowZoomBox(aDelta < 0);
   //       }

   //      mUpdateView = true;
   //   }
   //}

   return status;
}
