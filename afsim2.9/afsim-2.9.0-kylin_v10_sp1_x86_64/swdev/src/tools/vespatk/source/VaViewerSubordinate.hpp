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

#ifndef VaViewerSubordinate_hpp
#define VaViewerSubordinate_hpp

#include "vespatk_export.h"

#include <osg/Texture2D>

#include "VaViewer.hpp"

namespace vespa
{
//! An abstract base class for application defined viewers.
class VESPATK_EXPORT VaViewerSubordinate : public VaViewer
{
public:
   VaViewerSubordinate(VaViewer* aParentViewerPtr, unsigned int aResX, unsigned int aResY, int aViewerOptions = 0);
   ~VaViewerSubordinate() override;

   // GlDraw Interface

   void Initialize();

   void SetupViewer(bool aUseShaders);

   void            SetUpdateView(bool aUpdateView) { mUpdateView = aUpdateView; }
   osg::Texture2D* GetTexture();
   void            PrepareToRedraw() override;

protected:
   VaViewerSubordinate(const VaViewerSubordinate& aSrc); // Disable copy construction.

   VaViewer*    mParentViewerPtr;
   unsigned int mResX;
   unsigned int mResY;
};
} // end namespace vespa
#endif
