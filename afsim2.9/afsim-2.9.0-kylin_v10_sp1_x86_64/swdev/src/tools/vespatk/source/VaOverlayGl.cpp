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

#include "VaOverlayGl.hpp"

#include <iostream>

#include "UtLog.hpp"
#include "Uto.hpp"
#include "UtoGLShape.hpp"

namespace vespa
{

VaOverlayGl::VaOverlayGl()
   : VaOverlay("gl_overlay", cOVERLAY_WORLD)
   , mShapePtr(nullptr)
   , mLighting(false)
   , mLineSmoothing(false)
{
   SetType<VaOverlayGl>();
}

VaOverlayGl::VaOverlayGl(const std::string& aName)
   : VaOverlay(aName, cOVERLAY_WORLD)
   , mShapePtr(nullptr)
   , mLighting(false)
   , mLineSmoothing(false)
{
   SetType<VaOverlayGl>();
}

VaOverlayGl::VaOverlayGl(const std::string& aName, VaOverlayGl::OverlayType aType)
   : VaOverlay(aName, aType)
   , mShapePtr(nullptr)
   , mLighting(false)
   , mLineSmoothing(false)
{
   SetType<VaOverlayGl>();
}

VaOverlayGl::VaOverlayGl(const VaOverlayGl& aSrc)
   : VaOverlay(aSrc)
   , mShapePtr(nullptr)
   , mLighting(aSrc.mLighting)
   , mLineSmoothing(aSrc.mLineSmoothing)
{
   SetType<VaOverlayGl>();
}

VaOverlayGl::~VaOverlayGl()
{
   UnbindShape("gl");
}

VaOverlayGl* VaOverlayGl::Clone() const
{
   return new VaOverlayGl(*this);
}

bool VaOverlayGl::Initialize()
{
   bool initialized = false;

   UtoGLShape glShapePtr(this, std::mem_fn(&VaOverlayGl::DrawFunction));
   mShapePtr = dynamic_cast<UtoGLShape*>(BindShape("gl", glShapePtr));
   if (mShapePtr == nullptr)
   {
      ut::log::warning() << "Could not bind shape: gl";
   }

   if (mShapePtr != nullptr)
   {
      mShapePtr->SetColor(UtoColor(0, 0, 0, 128));
      mShapePtr->SetSize(20000000.0);
      mShapePtr->EnableLineSmoothing(mLineSmoothing);
      mShapePtr->EnableLighting(mLighting);
      SetVisible(true);
      initialized = true;
   }
   return initialized;
}

void VaOverlayGl::Update() {}

void VaOverlayGl::DrawFunction() {}

void VaOverlayGl::EnableLineSmoothing(bool aState)
{
   mLineSmoothing = aState;
   if (nullptr != mShapePtr)
   {
      mShapePtr->EnableLineSmoothing(aState);
   }
}

void VaOverlayGl::EnableLighting(bool aState)
{
   mLighting = aState;
   if (nullptr != mShapePtr)
   {
      mShapePtr->EnableLighting(aState);
   }
}

} // namespace vespa
