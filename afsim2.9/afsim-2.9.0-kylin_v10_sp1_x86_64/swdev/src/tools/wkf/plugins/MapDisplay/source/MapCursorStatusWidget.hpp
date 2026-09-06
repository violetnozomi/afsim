// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef MAPCURSORSTATUSWIDGET_HPP
#define MAPCURSORSTATUSWIDGET_HPP

#include <QLabel>

#include "VaCallbackHolder.hpp"

namespace vespa
{
class VaViewer;
}

namespace Map
{
class CursorStatusWidget : public QLabel
{
public:
   CursorStatusWidget(const vespa::VaViewer& aViewer);

   void SetShowAltitude(bool aState) { mShowTerrain = aState; }

   void Update();

private:
   void UpdateCursorPositionCB(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY);

   vespa::VaCallbackHolder mCallbacks;
   const vespa::VaViewer&  mViewer;
   int                     mMouseX;
   int                     mMouseY;
   bool                    mShowTerrain{true};
   size_t                  mDtedRegistrationId;
};
} // namespace Map
#endif
